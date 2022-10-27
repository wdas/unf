.. _notices:

***********************
Using Autonomous Notice
***********************

The Usd Notice Framework provides a `UnfNotice::StageNotice`_ interface
which can be used to create autonomous notices related to the :term:`Usd` stage.

Autonomous notices present the following features:

1. **They do not reference data from the stage**

This rule ensures that notices are safe to use in asynchronous context.
By contrast, Usd notices such as `ObjectsChanged <UsdNotice::ObjectsChanged>`_
and `LayerMutingChanged <UsdNotice::LayerMutingChanged>`_ both reference data
from the stage and thus are not safe to use when the stage is not longer
reachable.

2. **They can include logic for consolidation with notice of the same type**

This makes it possible to reduce the number of notices emitted during a
transaction without loosing information.

.. _notices/transaction:

Using notice transaction
========================

A transaction is a time frame during which the emission of autonomous notices
needs to be withheld. The Broker_ is in charge of capturing these notices via
a NoticeTransaction_ instance:

.. code-block:: cpp

    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = unf::Broker::Create(stage);

    {
        unf::NoticeTransaction transaction(broker);

        // Emission of autonomous notices is deferred until the end of the
        // transaction. Other notices are sent as normal.
    }

A NoticeTransaction_ instance can also be constructed directly from the Usd
stage, which encapsulates the creation of the broker if none have been
previously created for this stage:

.. code-block:: cpp

    auto stage = PXR_NS::UsdStage::CreateInMemory();

    {
        unf::NoticeTransaction transaction(stage);
        // ...
    }

At the end of a transaction, all notices captured are emitted. for an autonomous
notice to be captured, it needs to be sent via the Broker_. Let's consider a
ficticious autonomous notice named "Foo". It can be created and sent with
this templated method:

.. code-block:: cpp

    auto broker = unf::Broker::Create(stage);
    broker->Send<Foo>()

It can also be created separately and sent as follows:

.. code-block:: cpp

    auto broker = unf::Broker::Create(stage);
    auto notice = Foo::Create();

    broker->Send(notice);

.. warning::

    If the notice is sent as follows, it will not be captured by the broker:

    .. code-block:: cpp

        auto notice = Foo::Create();
        notice->Send();

    Autonomous notices can not be sent in Python.

.. note::

    The sending process is usually handled by a :ref:`Dispatcher <dispatchers>`.

A notice can be defined as "mergeable" or "unmergeable". If a notice is defined
as unmergeable, no consolidation will take place during a transaction. In the
following example, one consolidated "Foo" notice will be sent at the end
of the transaction if the notice was mergeable. Otherwise, the three notices
are sent:

.. code-block:: cpp

    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = unf::Broker::Create(stage);

    auto notice = Foo::Create();

    // Indicate whether the notice can be merged.
    printf(notice->IsMergeable())

    {
        unf::NoticeTransaction transaction(broker);

        // The following notices will be captured by the broker during the
        // scope of the transaction.
        broker->Send(notice);
        broker->Send(notice);
        broker->Send(notice);
    }

It is possible to start the transaction with a predicate function to indicate
which notices are captured during the transaction. The following example will
only filter in the "Foo" notices:

.. code-block:: cpp

    auto predicate = [&](const unf::UnfNotice::StageNotice& notice) {
        return (typeid(notice).name() == typeid(Foo).name());
    };

    {
        unf::NoticeTransaction transaction(broker, predicate);
        // ...
    }

For convenience, a predicate has been provided to block all notices emitted
during a transaction:

.. code-block:: cpp

    {
        unf::NoticeTransaction transaction(
            broker, unf::CapturePredicate::BlockAll());
        // ...
    }

.. _notices/default:

Default notices
===============

By default, the broker will emit autonomous equivalent for each :term:`Usd`
notices:

===================================== ====================================
Usd notices                           Autonomous Notices
===================================== ====================================
`UsdNotice::ObjectsChanged`_          `UnfNotice::ObjectsChanged`_
`UsdNotice::LayerMutingChanged`_      `UnfNotice::LayerMutingChanged`_
`UsdNotice::StageContentsChanged`_    `UnfNotice::StageContentsChanged`_
`UsdNotice::StageEditTargetChanged`_  `UnfNotice::StageEditTargetChanged`_
===================================== ====================================

Python bindings are provided for each notice:

* :class:`~Notice.ObjectsChanged`
* :class:`~Notice.LayerMutingChanged`
* :class:`~Notice.StageContentsChanged`
* :class:`~Notice.StageEditTargetChanged`

All of these notices are defined as mergeable and therefore will be
consolidated per notice type during a transaction.

.. note::

    These notices are handled by the :ref:`StageDispatcher <dispatchers/stage>`.

.. _notices/custom:

Custom notices
==============

The `UnfNotice::StageNotice`_ interface can be safely derived as follows to
create new notices:

.. code-block:: cpp

    class Foo : public unf::UnfNotice::StageNoticeImpl<Foo> {
    public:
        Foo() = default;
        virtual ~Foo() = default;
    };

By default, this notice will be mergeable, it can be made unmergeable as
follows:

.. code-block:: cpp

    class Foo : public unf::UnfNotice::StageNoticeImpl<Foo> {
    public:
        Foo() = default;
        virtual ~Foo() = default;

        virtual bool IsMergeable() const { return false; }
    };

If the notice is mergeable and contain some data, the "Merge" method needs
to be implemented to indicate how notices are consolidated. The "PostProcess"
method could also be implemented to process the data after it has been merged
with other notices:

.. code-block:: cpp

    using DataMap = std::unordered_map<std::string, std::string>;

    class Foo : public unf::UnfNotice::StageNoticeImpl<Foo> {
    public:
        Foo() = default;
        virtual ~Foo() = default;

        virtual void Merge(Foo&& notice) override
        {
            for (const auto& it : notice._data) {
                _data[it.first] = std::move(it.second);
            }
        }

        virtual void PostProcess() override
        {
            // ...
        }

    private:
        DataMap _data;
    };


.. note::

    The copy constructor and assignment operator should be implemented as well
    if the notice contains data.

.. warning::

    Custom autonomous notices can not be implemented in Python.
