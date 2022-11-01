.. _getting_started:

***************
Getting started
***************

.. highlight:: python

Once the library is :ref:`installed <installing>` with Python binding, we can
test the Python API by following a simple example.

.. _getting_started/editing_stage:

Editing the Stage
=================

Let's start by creating a :term:`Usd` stage in memory and register a callback
to listen to `Usd.Notice.ObjectsChanged`_ and print all updated paths::

    from pxr import Usd, Tf

    stage = Usd.Stage.CreateInMemory()

    def _updated(notice, stage):
        """Print updated paths from the stage."""
        print("Resynced Paths: {}".format([
            (path, notice.GetChangedFields(path))
            for path in notice.GetResyncedPaths()
        ]))
        print("ChangedInfoOnly Paths: {}\n".format([
            (path, notice.GetChangedFields(path))
            for path in notice.GetChangedInfoOnlyPaths()
        ]))

    key = Tf.Notice.Register(Usd.Notice.ObjectsChanged, _updated, stage)

Let's now edit the stage by adding a cylinder prim and update the attributes::

    prim = stage.DefinePrim("/Foo", "Cylinder")
    prim.GetAttribute("radius").Set(5)
    prim.GetAttribute("height").Set(10)

This should have triggered five `Usd.Notice.ObjectsChanged`_ notices to be
emitted. The first notice was emitted when the prim was created, the second and
the fourth when both attributes where created, the third and fifth when they
were given a default value. As a result, the following information will be
printed in the shell:

.. code-block:: bash

    Resynced Paths: [(Sdf.Path('/Foo'), ['specifier', 'typeName'])]
    ChangedInfoOnly Paths: []

    Resynced Paths: [(Sdf.Path('/Foo.radius'), [])]
    ChangedInfoOnly Paths: []

    Resynced Paths: []
    ChangedInfoOnly Paths: [(Sdf.Path('/Foo.radius'), ['default'])]

    Resynced Paths: [(Sdf.Path('/Foo.height'), [])]
    ChangedInfoOnly Paths: []

    Resynced Paths: []
    ChangedInfoOnly Paths: [(Sdf.Path('/Foo.height'), ['default'])]

.. _getting_started/editing_layer:

Editing the Layer
=================

To consolidate the number of notices emitted, we could use the :term:`Sdf` API
to edit the root layer, then use a `Sdf.ChangeBlock`_ which would also limit the
number of recompositions and greatly improve overall performance::

    from pxr import Sdf

    layer = stage.GetRootLayer()

    with Sdf.ChangeBlock():
        prim = Sdf.CreatePrimInLayer(layer, "/Foo")
        prim.specifier = Sdf.SpecifierDef
        prim.typeName = "Cylinder"

        attr = Sdf.AttributeSpec(prim, "radius", Sdf.ValueTypeNames.Double)
        attr.default = 5

        attr = Sdf.AttributeSpec(prim, "height", Sdf.ValueTypeNames.Double)
        attr.default = 10

.. warning::

    It is not safe to edit the stage with the :term:`Usd` API when using
    `Sdf.ChangeBlock`_.

One single notice will be emitted:

.. code-block:: bash

    Resynced Paths: [(Sdf.Path('/Foo'), ['specifier', 'typeName'])]
    ChangedInfoOnly Paths: []

.. _getting_started/using:

Using the library
=================

Let's now create a new stage and modify the notice registration to target the
:class:`unf.Notice.ObjectsChanged <Notice.ObjectsChanged>` notice:

.. code-block:: python
    :emphasize-lines: 2,17

    from pxr import Usd, Tf
    import usd_notice_framework as unf

    stage = Usd.Stage.CreateInMemory()

    def _updated(notice, stage):
        """Print updated paths from the stage."""
        print("Resynced Paths: {}".format([
            (path, notice.GetChangedFields(path))
            for path in notice.GetResyncedPaths()
        ]))
        print("ChangedInfoOnly Paths: {}\n".format([
            (path, notice.GetChangedFields(path))
            for path in notice.GetChangedInfoOnlyPaths()
        ]))

    key = Tf.Notice.Register(unf.Notice.ObjectsChanged, _updated, stage)

To ensure that a :class:`unf.Notice.ObjectsChanged <Notice.ObjectsChanged>`
notice is sent whenever a `Usd.Notice.ObjectsChanged`_ is emitted, we need to
create a :class:`Broker` associated with the stage::

    broker = unf.Broker.Create(stage)

.. note::

    The :class:`Broker` is using a :ref:`dispatcher <dispatchers>` to
    emit an autonomous notice for each Usd notice.

Let's now edit the stage once again with the :term:`Usd` API::

    prim = stage.DefinePrim("/Foo", "Cylinder")
    prim.GetAttribute("radius").Set(5)
    prim.GetAttribute("height").Set(10)

Like in the first section, five notices are emitted with the same information
as with the `Usd.Notice.ObjectsChanged`_ notice. However, the
:class:`unf.Notice.ObjectsChanged <Notice.ObjectsChanged>` notice is defined as
mergeable. It is therefore possible to reduce the number of notices emitted by
using a :ref:`notice transaction <notices/transaction>`::

    broker.BeginTransaction()

    prim = stage.DefinePrim("/Foo", "Cylinder")
    prim.GetAttribute("radius").Set(5)
    prim.GetAttribute("height").Set(10)

    broker.EndTransaction()

For safety, it is recommended to use the :class:`NoticeTransaction` object
instead which can be used as a context manager::

    with unf.NoticeTransaction(broker):
        prim = stage.DefinePrim("/Foo", "Cylinder")
        prim.GetAttribute("radius").Set(5)
        prim.GetAttribute("height").Set(10)

As a result, only one notice will be emitted:

.. code-block:: bash

    Resynced Paths: [(Sdf.Path('/Foo'), ['typeName', 'specifier'])]
    ChangedInfoOnly Paths: [(Sdf.Path('/Foo.radius'), ['default']), (Sdf.Path('/Foo.height'), ['default'])]

It is sometimes necessary to de-register listeners to a particular set of
notices when editing the stage. If many clients are listening to Usd notices,
this process can be tedious.

The Unf library provides a way to filter out some or all Unf notices during a
transaction using a predicate function. For instance, the following transaction
will only emit "Foo" notices::

    def _predicate(notice):
        """Indicate whether *notice* should be captured and emitted."""
        return isinstance(notice, Foo)

    with unf.NoticeTransaction(stage, predicate=_predicate):
        # Stage editing ...

For convenience, a :meth:`CapturePredicate.BlockAll` predicate has been provided
to block all notices emitted during a transaction::

    with unf.NoticeTransaction(
        stage, predicate=unf.CapturePredicate.BlockAll()
    ):
        # Stage editing ...

.. seealso:: :ref:`notices`
