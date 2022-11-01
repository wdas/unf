.. _dispatchers:

*****************
Using Dispatchers
*****************

As mentioned :ref:`previously <notices>`, in order to capture autonomour
notices during a transaction, it needs to be sent via the Broker_. However,
notices are rarely sent directly within the transaction like in our examples.
Most of the time, we want autonomous notices to be triggered by other incoming
notices.

It is therefore preferrable to send autonomous notices within a Dispatcher_.
A dispatcher is an object which triggers :ref:`notices <notices>` when incoming
notices are received. It can be retrieved from the Broker_ via its identifier:

.. code-block:: cpp

    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = unf::Broker::Create(stage);
    auto dispatcher = broker->GetDispatcher("...");

Listeners registered by any dispatcher can be revoked and re-registered as
follows:

.. code-block:: cpp

    // Revoke all registered listeners.
    dispatcher->Revoke();

    // Re-register listeners.
    dispatcher->Register();

.. warning::

    Dispatchers can not be manipulated in Python.

.. _dispatchers/stage:

Stage Dispatcher
================

By default, the Broker_ only contains the StageDispatcher_, which is in charge
of emitting :ref:`default autonomous notices <notices/default>` for each
:term:`Usd` notices received.

Its identifier is "StageDispatcher":

.. code-block:: cpp

    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = unf::Broker::Create(stage);
    auto dispatcher = broker->GetDispatcher("StageDispatcher");

.. note::

    The Stage Dispatcher can be overriden by adding a new dispatcher with the
    same identifier.

.. _dispatchers/create:

Creating a Dispatcher
=====================

The Dispatcher_ interface can be used to create a new dispatcher. The "Register"
method must be implemented to register the listeners in charge of emitting the
new notices.

A convenient "_Register" protected method is provided to trigger a new notice
from one specific notice type. The new notice will be created by passing the
notice received to its constructor:

.. code-block:: cpp

    class NewDispatcher : public unf::Dispatcher {
    public:
        NewDispatcher(const unf::BrokerWeakPtr& broker)
        : unf::Dispatcher(broker) {}

        std::string GetIdentifier() const override { return "NewDispatcher"; };

        void Register() {
            // Register listener to create and emit 'OutputNotice' when
            // 'InputNotice' is received.
            _Register<InputNotice, OutputNotice>();
        }
    };

Otherwise, the listener can be registered as follows:

.. code-block:: cpp

    class NewDispatcher : public unf::Dispatcher {
    public:
        NewDispatcher(const unf::BrokerWeakPtr& broker)
        : unf::Dispatcher(broker) {}

        std::string GetIdentifier() const override { return "NewDispatcher"; };

        void Register() {
            auto self = PXR_NS::TfCreateWeakPtr(this);
            auto cb = &NewDispatcher::_OnReceiving;
            _keys.push_back(
                PXR_NS::TfNotice::Register(self, cb, _broker->GetStage()));
        }

        void _OnReceiving(const InputNotice& notice)
        {
            _broker->Send<OutputNotice>(/* arguments to create notice */);
        }

    };

The new dispatcher can be added to the Broker_ as follows:

.. code-block:: cpp

    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = unf::Broker::Create(stage);

    broker->AddDispatcher<NewDispatcher>();

.. _dispatchers/plugin:

Creating a plugin
=================

The new dispatcher can be automatically discovered and registered when the
Broker_ is created using the :term:`Plug` framework.

First, a corresponding runtime TfType must be defined:

.. code-block:: cpp

    TF_REGISTRY_FUNCTION(TfType)
    {
        unf::DispatcherDefine<NewDispatcher, unf::Dispatcher>();
    }

Then a :file:`plugInfo.json` configuration must be created. It should be in
the form of:

.. code-block:: json

    {
        "Plugins": [
            {
                "Info": {
                    "Types" : {
                        "NewDispatcher" : {
                            "bases": [ "Dispatcher" ]
                        }
                    }
                },
                "LibraryPath": "libNewDispatcher.so",
                "Name": "NewDispatcher",
                "Type": "library"
            }
        ]
    }

The path to this configuration file must be included in the
:envvar:`PXR_PLUGINPATH_NAME` environment variable.
