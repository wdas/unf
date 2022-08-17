#ifndef TEST_NOTICE_BROKER_PLUGIN_NEW_STAGE_DISPATCHER_H
#define TEST_NOTICE_BROKER_PLUGIN_NEW_STAGE_DISPATCHER_H

#include <UsdNoticeBroker/dispatcher.h>

#include <TestUsdNoticeBroker/testNotice.h>

#include <pxr/pxr.h>

namespace Test {

class NewStageDispatcher : public PXR_NS::UNB::Dispatcher
{
public:
    NewStageDispatcher(const PXR_NS::UNB::BrokerWeakPtr& broker)
    : PXR_NS::UNB::Dispatcher(broker) {}

    virtual std::string GetIdentifier() const { return "StageDispatcher"; };

    void Register() {
        _Register<InputNotice, OutputNotice1>();
    }
};

} // namespace Test

#endif // TEST_NOTICE_BROKER_PLUGIN_NEW_STAGE_DISPATCHER_H
