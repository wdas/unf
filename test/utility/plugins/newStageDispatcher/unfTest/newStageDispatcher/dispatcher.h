#ifndef TEST_USD_NOTICE_FRAMEWORK_PLUGIN_NEW_STAGE_DISPATCHER_H
#define TEST_USD_NOTICE_FRAMEWORK_PLUGIN_NEW_STAGE_DISPATCHER_H

#include <unf/dispatcher.h>

#include <unfTest/testNotice.h>

#include <pxr/pxr.h>

namespace Test {

class NewStageDispatcher : public unf::Dispatcher {
  public:
    NewStageDispatcher(const unf::BrokerWeakPtr& broker)
        : unf::Dispatcher(broker)
    {
    }

    virtual std::string GetIdentifier() const { return "StageDispatcher"; };

    void Register() { _Register<InputNotice, OutputNotice1>(); }
};

}  // namespace Test

#endif  // TEST_USD_NOTICE_FRAMEWORK_PLUGIN_NEW_STAGE_DISPATCHER_H
