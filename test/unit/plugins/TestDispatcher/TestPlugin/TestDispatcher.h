#ifndef TEST_NOTICE_BROKER_PLUGIN_TEST_DISPATCHER_H
#define TEST_NOTICE_BROKER_PLUGIN_TEST_DISPATCHER_H

#include <UsdNoticeBroker/dispatcher.h>

#include <TestUsdNoticeBroker/testNotice.h>

#include <pxr/pxr.h>

namespace Test {

class TestDispatcher : public PXR_NS::UNB::Dispatcher
{
public:
    TestDispatcher(const PXR_NS::UNB::BrokerWeakPtr& broker)
    : PXR_NS::UNB::Dispatcher(broker) {}

    virtual std::string GetIdentifier() const { return "TestDispatcher"; };

    void Register() {
        _Register<InputNotice, OutputNotice2>();
    }
};

} // namespace Test

#endif // TEST_NOTICE_BROKER_PLUGIN_TEST_DISPATCHER_H
