.. _introduction:

************
Introduction
************

The USD Notice Framework (UNF) is built over :term:`USD`'s 
:term:`Tf Notification System`. It provides a :ref:`C++ <api_reference/cpp>` and
:ref:`Python <api_reference/python>` API to efficiently manage the flow of
notifications emitted when authoring one or more :term:`USD` stages.

:term:`USD` notices are delivered synchronously and are deleted after delivery. UNF
introduces :ref:`durable notices` that can be used for deferred delivery, and
can be aggregated per notice type, when applicable.


What does this solve?
=====================

Pixar designed :term:`USD` as an open and extensible framework for composable data
interchange across different tools.  As such, it is highly optimized for that
purpose.  Born out of Pixar's :term:`Presto Animation` package, some application level
features were intentionally omitted to maintain speed, scalability and
robustness to support it's core usage.

Given that, there are challenges when using :term:`USD` "out of the box" when building
interactive applications directly on top of :term:`USD` as a data model. UNF is designed
as a framework for use in higher level apis and application logic to help
mitigate those issues.

One of the challenges a developer might face when developing an application
on top of a :term:`USD` data is the interactive editing performance when using
:term:`USD` as a data model. When editing :term:`USD` data, the stage and/or
layers produce a high volume of change notifications that can be hard to manage
when crafting a performant user experience.  This high volume of notices can
cause frequent and costly cache invalidation overhead, leading to sluggish
performance, and overly complicated code.

While bringing such features directly into the :term:`USD` API has
been considered (see `prototype
<https://github.com/wdas/USD/compare/release...prototype-transaction>`_),
ensuring that USD notices such as `UsdNotice::ObjectsChanged`_
or `UsdNotice::LayerMutingChanged`_ own their data instead of referencing it
from stage. It would have added an overhead which would have degraded the
performance for clients that do not take advantage of the feature.

UNF however is built on top of :term:`USD` and can be added to applications
when needed.


How does it work?
=================

UNF provides a framework to aggregate and even simplify change notifications
across a series of :term:`USD` edits.  UNF provides utilities for building new higher
level apis and experiences on top of :term:`USD` data in a performant manner.  In doing
so, it introduces _some_ overhead on top of :term:`USD` but allows developers to write
vastly more performant and sustainable tools to observe and author :term:`USD` data
directly.

Note that UNF does not affect the _internal_ performance of :term:`USD`, and
therefore will not affect Hydra or composition performance or the results they
generate.  Hydra and the internal logic of :term:`USD` remains unchanged as they observe
the raw notices that :term:`USD` provides. UNF sits on top of :term:`USD`'s infrastructure to
provide more robust ways of sheparding :term:`USD` change notifications.

Here's a typical example of how you might create a prim and set a few
attributes, and the notices :term:`USD` produces from those operations.  To an end user, 
this may be the result of calling a higher level api to create a new piece of
geometry.

.. code-block:: python

    from pxr import Usd, Tf

    def updated(notice, stage):
        """Print resynced paths from the stage."""
        print(notice.GetResyncedPaths())

    def createStick(stage):
        """High level api to create a stick"""
        prim = stage.DefinePrim("/Foo", "Cylinder")
        prim.GetAttribute("radius").Set(5)
        prim.GetAttribute("height").Set(10)
        return prim

    stage = Usd.Stage.CreateInMemory()
    # Watch for raw ObjectsChanged notices on the stage
    key = Tf.Notice.Register(Usd.Notice.ObjectsChanged, updated, stage)
    stick = createStick(stage)

Output:

.. code-block:: python

    [Sdf.Path('/Foo')]
    [Sdf.Path('/Foo.radius')]
    []
    [Sdf.Path('/Foo.height')]
    []

Note in the output that calling `createStick()` produced 5 stage change
notifications, which means that downstream obververs of the stage may need
to do multiple lookups, prim refreshes, or cache invalidations to see if their
data is affected by the stage change.

Now here's the same example written using UNF's new notices and transactions:

.. code-block:: python

    from pxr import Usd, Tf
    import unf

    def updated(notice, stage):
        """Print resynced paths from the stage."""
        print(notice.GetResyncedPaths())

    def createStick(stage):
        """High level api to create a stick"""
        with unf.NoticeTransaction(stage):
            prim = stage.DefinePrim("/Foo", "Cylinder")
            prim.GetAttribute("radius").Set(5)
            prim.GetAttribute("height").Set(10)
        return prim

    stage = Usd.Stage.CreateInMemory()
    # Watch for the builtin UNF notice for ObjectsChanged
    key = Tf.Notice.Register(unf.Notice.ObjectsChanged, updated, stage)
    stick = createStick(stage)

Output:

.. code-block:: python

    [Sdf.Path('/Foo')]

Note that we bundled our :term:`USD` stage edits with a :ref:`unf.NoticeTransaction`
context manager. In doing so, UNF aggregated the 5 native :term:`USD` notices into
a single UNF notice. With a deeper understanding of what :term:`USD`'s ObjectsChange
notice is `designed
<https://openusd.org/dev/api/class_usd_notice_1_1_objects_changed.html#detail>`_
to do with respect to resync'd paths for cache invalidation, you can see how this
can greatly optimize downstream observers so they only receive the notifications
they need.
