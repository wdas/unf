#ifndef TEST_USD_NOTICE_FRAMEWORK_LISTENER_H
#define TEST_USD_NOTICE_FRAMEWORK_LISTENER_H

#include <pxr/base/tf/notice.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/pxr.h>
#include <pxr/usd/usd/stage.h>

#include <string>
#include <typeinfo>
#include <unordered_map>

namespace Test {

// Container to listen to several types of Tf notices.
template <class... Types>
class Listener : public PXR_NS::TfWeakBase {
  public:
    Listener() = default;
    Listener(const PXR_NS::UsdStageWeakPtr& stage) { SetStage(stage); }

    virtual ~Listener()
    {
        for (auto& element : _keys) {
            PXR_NS::TfNotice::Revoke(element.second);
        }
    }

    void SetStage(const PXR_NS::UsdStageWeakPtr& stage)
    {
        auto self = PXR_NS::TfCreateWeakPtr(this);
        _keys = std::unordered_map<std::string, PXR_NS::TfNotice::Key>(
            {_Register<Types>(self, stage)...});
    }

    template <class T>
    size_t Received()
    {
        std::string name = typeid(T).name();
        if (_received.find(name) == _received.end()) return 0;

        return _received.at(name);
    }

    void Reset()
    {
        for (auto& element : _received) {
            element.second = 0;
        }
    }

  private:
    template <class T>
    std::pair<std::string, PXR_NS::TfNotice::Key> _Register(
        const PXR_NS::TfWeakPtr<Listener>& self,
        const PXR_NS::UsdStageWeakPtr& stage)
    {
        auto cb = &Listener::_Callback<T>;
        std::string name = typeid(T).name();
        auto key = PXR_NS::TfNotice::Register(self, cb, stage);

        return std::make_pair(name, key);
    }

    template <class T>
    void _Callback(const T& notice, const PXR_NS::UsdStageWeakPtr& sender)
    {
        std::string name = typeid(T).name();

        if (_received.find(name) == _received.end()) _received[name] = 0;

        _received[name] += 1;
    }

    std::unordered_map<std::string, PXR_NS::TfNotice::Key> _keys;
    std::unordered_map<std::string, size_t> _received;
};

}  // namespace Test

#endif  // TEST_USD_NOTICE_FRAMEWORK_LISTENER_H
