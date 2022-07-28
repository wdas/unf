#include "broker.h"
#include "broadcaster.h"

#include <pxr/pxr.h>

#include <iostream>

class TestBroadcaster : public PXR_NS::Broadcaster
{
public:
    TestBroadcaster(const PXR_NS::NoticeBrokerWeakPtr& broker)
    : PXR_NS::Broadcaster(broker) {
        std::cout << "....TestBroadcaster\n";
    }

    virtual std::string GetIdentifier() const { return "TestBroadcaster"; };
};

PXR_NAMESPACE_OPEN_SCOPE

TF_REGISTRY_FUNCTION(TfType)
{
    BroadcasterDefine<TestBroadcaster, Broadcaster>();
}

PXR_NAMESPACE_CLOSE_SCOPE
