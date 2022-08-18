#ifndef TEST_NOTICE_BROKER_PLUGIN_NEW_STAGE_DISPATCHER_H
#define TEST_NOTICE_BROKER_PLUGIN_NEW_STAGE_DISPATCHER_H

#include <unf/dispatcher.h>

#include <unfTest/testNotice.h>

#include <pxr/pxr.h>

namespace Test {

class NewStageDispatcher : public PXR_NS::unf::Dispatcher
{
public:
    NewStageDispatcher(const PXR_NS::unf::BrokerWeakPtr& broker)
    : PXR_NS::unf::Dispatcher(broker) {}

    virtual std::string GetIdentifier() const { return "StageDispatcher"; };

    void Register() {
        _Register<InputNotice, OutputNotice1>();
    }
};

} // namespace Test

#endif // TEST_NOTICE_BROKER_PLUGIN_NEW_STAGE_DISPATCHER_H
