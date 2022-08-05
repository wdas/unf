#ifndef TEST_NOTICE_BROKER_DISPATCHER_H
#define TEST_NOTICE_BROKER_DISPATCHER_H

#include "testNotice.h"

#include "dispatcher.h"
#include "broker.h"

#include <pxr/pxr.h>

namespace Test {

// Declare test dispatchers.
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

class TestDispatcher : public PXR_NS::Dispatcher
{
public:
    TestDispatcher(const PXR_NS::NoticeBrokerWeakPtr& broker)
    : PXR_NS::Dispatcher(broker) {}

    virtual std::string GetIdentifier() const { return "TestDispatcher"; };

    void Register() {
        _Register<InputNotice, OutputNotice2>();
    }
};

} // namespace Test

#endif // TEST_NOTICE_BROKER_DISPATCHER_H
