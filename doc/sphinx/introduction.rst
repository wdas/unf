.. _introduction:

************
Introduction
************

The Usd Notice Framework is built over :term:`USD` notices and uses the
:term:`Tf Notification System`. It provides a :ref:`C++ <api_reference/cpp>` and
:ref:`Python <api_reference/python>` API to efficiently manage the flow of
notifications emitted when authoring the :term:`Usd` stage.

It introduces the concept of autonomous notices and notice transaction which
defers notification and consolidate notices per type when applicable:

.. code-block:: python

    from pxr import Usd, Tf
    import usd_notice_framework as unf

    stage = Usd.Stage.CreateInMemory()

    def updated(notice, stage):
        """Print resynced paths from the stage."""
        print(notice.GetResyncedPaths())

    key = Tf.Notice.Register(unf.Notice.ObjectsChanged, updated, stage)

    with unf.NoticeTransaction(stage):
        prim = stage.DefinePrim("/Foo", "Cylinder")
        prim.GetAttribute("radius").Set(5)
        prim.GetAttribute("height").Set(10)

A predicate can be applied during a transaction to block some or all Unf notices
emitted in this scope:

.. code-block:: python

    with unf.NoticeTransaction(
        stage, predicate=unf.CapturePredicate.BlockAll()
    ):
        prim = stage.DefinePrim("/Foo", "Cylinder")
        prim.GetAttribute("radius").Set(5)
        prim.GetAttribute("height").Set(10)

.. _introduction/why:

Why is it necessary ?
=====================

The information provided by the :term:`Usd` stage has high volume and low-level
data. It is lacking features for asynchronous handling and upstream filtering
of notices.

While bringing such features directly into the :term:`Usd` API has
been considered (see `prototype
<https://github.com/wdas/USD/compare/release...prototype-transaction>`_),
ensuring that USD notices such as `UsdNotice::ObjectsChanged`_
or `UsdNotice::LayerMutingChanged`_ own their data instead of referencing it
from stage would have added an overhead which would have degraded the
performance for client not interested with this feature.
