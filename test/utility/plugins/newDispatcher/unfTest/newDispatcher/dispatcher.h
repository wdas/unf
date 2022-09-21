#ifndef TEST_NOTICE_BROKER_PLUGIN_NEW_DISPATCHER_H
#define TEST_NOTICE_BROKER_PLUGIN_NEW_DISPATCHER_H

#include <unf/dispatcher.h>

#include <unfTest/testNotice.h>

#include <pxr/pxr.h>

namespace Test {

class NewDispatcher : public unf::Dispatcher {
  public:
    NewDispatcher(const unf::BrokerWeakPtr& broker) : unf::Dispatcher(broker) {}

    virtual std::string GetIdentifier() const { return "NewDispatcher"; };

    void Register() { _Register<InputNotice, OutputNotice2>(); }
};

}  // namespace Test

#endif  // TEST_NOTICE_BROKER_PLUGIN_NEW_DISPATCHER_H
