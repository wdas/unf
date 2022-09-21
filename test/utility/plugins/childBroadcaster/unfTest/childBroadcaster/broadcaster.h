#ifndef TEST_USD_NOTICE_FRAMEWORK_PLUGIN_CHILD_BROADCASTER_H
#define TEST_USD_NOTICE_FRAMEWORK_PLUGIN_CHILD_BROADCASTER_H

#include "notice.h"

#include <unfTest/notice.h>

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

        n.Send(_broker->GetStage());
    }
};

}  // namespace Test

#endif  // TEST_USD_NOTICE_FRAMEWORK_PLUGIN_CHILD_BROADCASTER_H
