#ifndef TEST_NOTICE_BROKER_BROADCASTER_H
#define TEST_NOTICE_BROKER_BROADCASTER_H

#include "testNotice.h"

#include <unf/broker.h>
#include <unf/broadcaster.h>
#include <unf/hierarchyBroadcaster.h>
#include <iostream>

#include <pxr/pxr.h>

namespace Test {
using HierarchyBroadcaster = PXR_NS::unf::HierarchyBroadcaster;

// Declare test broadcasters.
class ChildBroadcaster : public PXR_NS::unf::Broadcaster
{
public:
    ChildBroadcaster(const PXR_NS::unf::BrokerWeakPtr& broker)
    : PXR_NS::unf::Broadcaster(broker) {}

    virtual std::string GetIdentifier() const override{ return "ChildBroadcaster"; };
    virtual std::string GetParentIdentifier() const override{ return "HierarchyBroadcaster"; }

    void Execute(void* parent) override{
        HierarchyBroadcaster* hierarchyBroadcaster = static_cast<HierarchyBroadcaster*>(parent);
        ChildBroadcasterNotice n = ChildBroadcasterNotice(hierarchyBroadcaster->GetAdded(), hierarchyBroadcaster->GetRemoved(),
                                                            hierarchyBroadcaster->GetModified(), hierarchyBroadcaster->GetChangedFields());
        n.Send();

    }
};

} // namespace Test

#endif // TEST_NOTICE_BROKER_DISPATCHER_H
