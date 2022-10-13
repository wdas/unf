#ifndef TEST_USD_NOTICE_FRAMEWORK_OBSERVER_H
#define TEST_USD_NOTICE_FRAMEWORK_OBSERVER_H

#include <pxr/base/tf/notice.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/pxr.h>
#include <pxr/usd/usd/stage.h>

#include <boost/optional.hpp>

#include <exception>

namespace Test {

// Interface to examine content of notice received.

template <class T>
class Observer : public PXR_NS::TfWeakBase {
  public:
    Observer() = default;
    Observer(const PXR_NS::UsdStageWeakPtr& stage) : _count(0)
    {
        SetStage(stage);
    }

    virtual ~Observer() { PXR_NS::TfNotice::Revoke(_key); }

    void SetStage(const PXR_NS::UsdStageWeakPtr& stage)
    {
        auto self = PXR_NS::TfCreateWeakPtr(this);
        _key = PXR_NS::TfNotice::Register(
            PXR_NS::TfCreateWeakPtr(this), &Observer::OnReceiving, stage);
    }

    void SetCallback(std::function<void(const T&)> callback)
    {
        _callback = callback;
    }

    const T& GetLatestNotice() const
    {
        if (!_notice) {
            throw std::runtime_error("Impossible to access latest notice.");
        }
        return *_notice;
    }

    size_t Received() const { return _count; }

    void Reset()
    {
        _count = 0;
        _notice.reset();
    }

  private:
    void OnReceiving(const T& notice, const PXR_NS::UsdStageWeakPtr&)
    {
        _notice = notice;
        _count++;

        if (_callback) {
            _callback(notice);
        }
    }

    boost::optional<T> _notice;
    size_t _count;
    PXR_NS::TfNotice::Key _key;
    std::function<void(const T&)> _callback;
};

}  // namespace Test

#endif  // TEST_USD_NOTICE_FRAMEWORK_OBSERVER_H
