#ifndef TEST_NOTICE_BROKER_BROADCASTER_H
#define TEST_NOTICE_BROKER_BROADCASTER_H

#include "testNotice.h"

#include <unf/broadcaster.h>
#include <unf/broker.h>
#include <unf/hierarchyBroadcaster.h>

#include <pxr/pxr.h>

namespace Test {

using HierarchyBroadcaster = unf::HierarchyBroadcaster;

// Declare test broadcasters.
class ChildBroadcaster : public unf::Broadcaster {
  public:
    ChildBroadcaster(const unf::BrokerWeakPtr& broker)
        : unf::Broadcaster(broker)
    {
    }

    virtual std::string GetIdentifier() const override
    {
        return "ChildBroadcaster";
    };
    virtual std::string GetParentIdentifier() const override
    {
        return "HierarchyBroadcaster";
    }

    void Execute(void* parent) override
    {
        HierarchyBroadcaster* hierarchyBroadcaster =
            static_cast<HierarchyBroadcaster*>(parent);
        ChildBroadcasterNotice n = ChildBroadcasterNotice(
            hierarchyBroadcaster->GetAdded(),
            hierarchyBroadcaster->GetRemoved(),
            hierarchyBroadcaster->GetModified(),
            hierarchyBroadcaster->GetChangedFields());
        n.Send();
    }
};

}  // namespace Test

#endif  // TEST_NOTICE_BROKER_DISPATCHER_H
