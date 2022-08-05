#ifndef TEST_NOTICE_BROKER_PLUGIN_NEW_STAGE_DISPATCHER_H
#define TEST_NOTICE_BROKER_PLUGIN_NEW_STAGE_DISPATCHER_H

#include "dispatcher.h"

#include <TestUsdNoticeBroker/testNotice.h>

#include <pxr/pxr.h>

namespace Test {

class NewStageDispatcher : public PXR_NS::Dispatcher
{
public:
    NewStageDispatcher(const PXR_NS::NoticeBrokerWeakPtr& broker)
    : PXR_NS::Dispatcher(broker) {}

    virtual std::string GetIdentifier() const { return "StageDispatcher"; };

    void Register() {
        _Register<InputNotice, OutputNotice1>();
    }
};

} // namespace Test

#endif // TEST_NOTICE_BROKER_PLUGIN_NEW_STAGE_DISPATCHER_H
