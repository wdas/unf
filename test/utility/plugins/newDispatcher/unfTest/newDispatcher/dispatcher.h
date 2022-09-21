#ifndef TEST_USD_NOTICE_FRAMEWORK_PLUGIN_NEW_DISPATCHER_H
#define TEST_USD_NOTICE_FRAMEWORK_PLUGIN_NEW_DISPATCHER_H

#include <unf/dispatcher.h>

#include <unfTest/notice.h>

#include <pxr/pxr.h>

namespace Test {

class NewDispatcher : public unf::Dispatcher {
  public:
    NewDispatcher(const unf::BrokerWeakPtr& broker) : unf::Dispatcher(broker) {}

    virtual std::string GetIdentifier() const { return "NewDispatcher"; };

    void Register() { _Register<InputNotice, OutputNotice2>(); }
};

}  // namespace Test

#endif  // TEST_USD_NOTICE_FRAMEWORK_PLUGIN_NEW_DISPATCHER_H
