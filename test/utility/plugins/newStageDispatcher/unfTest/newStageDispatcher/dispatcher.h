#ifndef TEST_USD_NOTICE_FRAMEWORK_PLUGIN_NEW_STAGE_DISPATCHER_H
#define TEST_USD_NOTICE_FRAMEWORK_PLUGIN_NEW_STAGE_DISPATCHER_H

#include <unf/api.h>
#include <unf/dispatcher.h>

namespace Test {

class NewStageDispatcher : public unf::Dispatcher {
  public:
    UNF_API NewStageDispatcher(const unf::BrokerWeakPtr& broker)
        : unf::Dispatcher(broker)
    {
    }

    UNF_API std::string GetIdentifier() const { return "StageDispatcher"; };

    UNF_API void Register() override;
};

}  // namespace Test

#endif  // TEST_USD_NOTICE_FRAMEWORK_PLUGIN_NEW_STAGE_DISPATCHER_H
