#include "broker.h"
#include "broadcaster.h"
#include "merger.h"

#include <pxr/pxr.h>

class TestBroadcaster : public PXR_NS::Broadcaster
{
public:
    TestBroadcaster(const PXR_NS::NoticeBrokerWeakPtr& broker)
    : PXR_NS::Broadcaster(broker) {}

    virtual std::string GetIdentifier() const { return "TestBroadcaster"; };

    virtual void Execute(NoticeMerger& context)
    {

    }
};

PXR_NAMESPACE_OPEN_SCOPE

TF_REGISTRY_FUNCTION(TfType)
{
    BroadcasterDefine<TestBroadcaster, Broadcaster>();
}

PXR_NAMESPACE_CLOSE_SCOPE
