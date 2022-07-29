#include "broker.h"
#include "broadcaster.h"
#include "merger.h"

#include <pxr/pxr.h>

class TestBroadcaster2 : public PXR_NS::Broadcaster
{
public:
    TestBroadcaster2(const PXR_NS::NoticeBrokerWeakPtr& broker)
    : PXR_NS::Broadcaster(broker) {}

    virtual std::string GetIdentifier() const { return "TestBroadcaster2"; };
    virtual std::string GetParentIdentifier() const {
        return "TestBroadcaster";
    };
};

PXR_NAMESPACE_OPEN_SCOPE

TF_REGISTRY_FUNCTION(TfType)
{
    BroadcasterDefine<TestBroadcaster2, Broadcaster>();
}

PXR_NAMESPACE_CLOSE_SCOPE
