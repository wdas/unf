#ifndef USD_NOTICE_FRAMEWORK_BROKER_H
#define USD_NOTICE_FRAMEWORK_BROKER_H

#include "unf/notice.h"
#include "unf/capturePredicate.h"

#include <pxr/base/plug/plugin.h>
#include <pxr/base/plug/registry.h>
#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>
#include <pxr/usd/usd/stage.h>

#include <functional>
#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace unf {

class Broker;
class Dispatcher;

using BrokerPtr = PXR_NS::TfRefPtr<Broker>;
using BrokerWeakPtr = PXR_NS::TfWeakPtr<Broker>;

using DispatcherPtr = PXR_NS::TfRefPtr<Dispatcher>;

class Broker : public PXR_NS::TfRefBase, public PXR_NS::TfWeakBase {
  public:
    static BrokerPtr Create(const PXR_NS::UsdStageWeakPtr& stage);

    virtual ~Broker() {}

    // Don't allow copies
    Broker(const Broker&) = delete;
    Broker& operator=(const Broker&) = delete;

    const PXR_NS::UsdStageWeakPtr GetStage() const { return _stage; }

    bool IsInTransaction();

    void BeginTransaction(
        CapturePredicate predicate = CapturePredicate::Default());

    void BeginTransaction(const CapturePredicateFunc&);

    void EndTransaction();

    template <class UnfNotice, class... Args>
    void Send(Args&&... args);

    void Send(const UnfNotice::StageNoticeRefPtr&);

    DispatcherPtr& GetDispatcher(std::string identifier);

    template <class T>
    void AddDispatcher();

    void Reset();
    static void ResetAll();

  private:
    Broker(const PXR_NS::UsdStageWeakPtr&);

    static void _CleanCache();

    void _DiscoverDispatchers();

    void _Add(const DispatcherPtr&);

    template <class T>
    DispatcherPtr _AddDispatcher();

    template <class OutputPtr, class OutputFactory>
    void _LoadFromPlugins(const PXR_NS::TfType& type);

    // A registry of hashed stage ptr to the corresponding stage's broker ptr.
    static std::unordered_map<size_t, BrokerPtr> Registry;

    class _NoticeMerger {
    public:
        _NoticeMerger(CapturePredicate predicate = CapturePredicate::Default());

        void Add(const UnfNotice::StageNoticeRefPtr&);
        void Join(_NoticeMerger&);
        void Merge();
        void Send(const PXR_NS::UsdStageWeakPtr&);

    private:
        using _NoticePtrList = std::vector<UnfNotice::StageNoticeRefPtr>;
        using _NoticePtrMap = std::unordered_map<std::string, _NoticePtrList>;

        _NoticePtrMap _noticeMap;
        CapturePredicate _predicate;
    };

    PXR_NS::UsdStageWeakPtr _stage;
    std::vector<_NoticeMerger> _mergers;
    std::unordered_map<std::string, DispatcherPtr> _dispatcherMap;
};

template <class UnfNotice, class... Args>
void Broker::Send(Args&&... args)
{
    PXR_NS::TfRefPtr<UnfNotice> _notice =
        UnfNotice::Create(std::forward<Args>(args)...);

    Send(_notice);
}

template <class T>
DispatcherPtr Broker::_AddDispatcher()
{
    static_assert(std::is_base_of<Dispatcher, T>::value);
    auto self = PXR_NS::TfCreateWeakPtr(this);
    DispatcherPtr dispatcher = PXR_NS::TfCreateRefPtr(new T(self));
    _Add(dispatcher);
    return dispatcher;
}

template <class T>
void Broker::AddDispatcher()
{
    const auto& dispatcher = _AddDispatcher<T>();
    dispatcher->Register();
}

template <class OutputPtr, class OutputFactory>
void Broker::_LoadFromPlugins(const PXR_NS::TfType& type)
{
    PXR_NAMESPACE_USING_DIRECTIVE

    const PXR_NS::PlugPluginPtr plugin =
        PXR_NS::PlugRegistry::GetInstance().GetPluginForType(type);

    if (!plugin) {
        return;
    }

    if (!plugin->Load()) {
        TF_CODING_ERROR(
            "Failed to load plugin %s for %s",
            plugin->GetName().c_str(),
            type.GetTypeName().c_str());
        return;
    }

    OutputPtr output;
    OutputFactory* factory = type.GetFactory<OutputFactory>();

    if (factory) {
        output = factory->New(TfCreateWeakPtr(this));
    }

    if (!output) {
        TF_CODING_ERROR(
            "Failed to manufacture %s from plugin %s",
            type.GetTypeName().c_str(),
            plugin->GetName().c_str());
        return;
    }

    _Add(output);
}

}  // namespace unf

#endif  // USD_NOTICE_FRAMEWORK_BROKER_H
