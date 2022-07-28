#include "broker.h"
#include "dispatcher.h"

#include <pxr/pxr.h>

#include <iostream>

class TestDispatcher : public PXR_NS::Dispatcher
{
public:
    TestDispatcher(const PXR_NS::NoticeBrokerWeakPtr& broker)
    : PXR_NS::Dispatcher(broker) {
        std::cout << "....TestDispatcher\n";
    }

    void Register()
    {

    }

    virtual std::string GetIdentifier() const { return "TestDispatcher"; };
};

PXR_NAMESPACE_OPEN_SCOPE

TF_REGISTRY_FUNCTION(TfType)
{
    DispatcherDefine<TestDispatcher, Dispatcher>();
}

PXR_NAMESPACE_CLOSE_SCOPE
