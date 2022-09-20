#ifndef TEST_NOTICE_BROKER_PLUGIN_CHILD_BROADCASTER_H
#define TEST_NOTICE_BROKER_PLUGIN_CHILD_BROADCASTER_H

#include <unfTest/testNotice.h>

#include <unf/hierarchyBroadcaster/broadcaster.h>

#include <unf/broadcaster.h>
#include <unf/broker.h>

#include <pxr/pxr.h>

namespace Test {

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
        unf::HierarchyBroadcaster* hierarchyBroadcaster =
            static_cast<unf::HierarchyBroadcaster*>(parent);

        ChildBroadcasterNotice n = ChildBroadcasterNotice(
            hierarchyBroadcaster->GetAdded(),
            hierarchyBroadcaster->GetRemoved(),
            hierarchyBroadcaster->GetModified(),
            hierarchyBroadcaster->GetChangedFields());

        n.Send();
    }
};

}  // namespace Test

#endif  // TEST_NOTICE_BROKER_PLUGIN_CHILD_BROADCASTER_H
