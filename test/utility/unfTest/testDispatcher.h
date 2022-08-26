#ifndef TEST_NOTICE_BROKER_DISPATCHER_H
#define TEST_NOTICE_BROKER_DISPATCHER_H

#include "testNotice.h"

#include <unf/dispatcher.h>
#include <unf/broker.h>

#include <pxr/pxr.h>

namespace Test {

// Declare test dispatchers.
class NewStageDispatcher : public unf::Dispatcher
{
public:
    NewStageDispatcher(const unf::BrokerWeakPtr& broker)
    : unf::Dispatcher(broker) {}

    virtual std::string GetIdentifier() const { return "StageDispatcher"; };

    void Register() {
        _Register<InputNotice, OutputNotice1>();
    }
};

class TestDispatcher : public unf::Dispatcher
{
public:
    TestDispatcher(const unf::BrokerWeakPtr& broker)
    : unf::Dispatcher(broker) {}

    virtual std::string GetIdentifier() const { return "TestDispatcher"; };

    void Register() {
        _Register<InputNotice, OutputNotice2>();
    }
};

} // namespace Test

#endif // TEST_NOTICE_BROKER_DISPATCHER_H
