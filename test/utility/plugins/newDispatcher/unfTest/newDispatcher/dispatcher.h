#ifndef TEST_USD_NOTICE_FRAMEWORK_PLUGIN_NEW_DISPATCHER_H
#define TEST_USD_NOTICE_FRAMEWORK_PLUGIN_NEW_DISPATCHER_H

#include <unf/api.h>
#include <unf/dispatcher.h>

namespace Test {

class NewDispatcher : public unf::Dispatcher {
  public:
    UNF_API NewDispatcher(const unf::BrokerWeakPtr& broker)
        : unf::Dispatcher(broker)
    {
    }

    UNF_API std::string GetIdentifier() const { return "NewDispatcher"; };

    UNF_API void Register() override;
};

}  // namespace Test

#endif  // TEST_USD_NOTICE_FRAMEWORK_PLUGIN_NEW_DISPATCHER_H
