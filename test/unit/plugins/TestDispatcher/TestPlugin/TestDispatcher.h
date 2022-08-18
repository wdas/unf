#ifndef TEST_NOTICE_BROKER_PLUGIN_TEST_DISPATCHER_H
#define TEST_NOTICE_BROKER_PLUGIN_TEST_DISPATCHER_H

#include <unf/dispatcher.h>

#include <unfTest/testNotice.h>

#include <pxr/pxr.h>

namespace Test {

class TestDispatcher : public PXR_NS::unf::Dispatcher
{
public:
    TestDispatcher(const PXR_NS::unf::BrokerWeakPtr& broker)
    : PXR_NS::unf::Dispatcher(broker) {}

    virtual std::string GetIdentifier() const { return "TestDispatcher"; };

    void Register() {
        _Register<InputNotice, OutputNotice2>();
    }
};

} // namespace Test

#endif // TEST_NOTICE_BROKER_PLUGIN_TEST_DISPATCHER_H
