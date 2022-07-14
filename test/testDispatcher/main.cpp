#include "broker.h"
#include "dispatcher.h"

#include <pxr/pxr.h>

class TestDispatcher : public PXR_NS::Dispatcher
{
public:
    TestDispatcher(const PXR_NS::NoticeBrokerWeakPtr& broker)
    : PXR_NS::Dispatcher(broker) {}

    virtual std::string GetIdentifier() const { return "test"; };
};

PXR_NAMESPACE_OPEN_SCOPE

TF_REGISTRY_FUNCTION(TfType)
{
    DispatcherDefine<TestDispatcher, Dispatcher>();
}

PXR_NAMESPACE_CLOSE_SCOPE
