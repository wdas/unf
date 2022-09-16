#ifndef NOTICE_BROKER_BROKER_H
#define NOTICE_BROKER_BROKER_H

#include "unf/notice.h"
#include "unf/merger.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/base/plug/plugin.h>
#include <pxr/base/plug/registry.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/common.h>

#include <functional>
#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace unf {

class Broker;
class Dispatcher;
class Broadcaster;

using BrokerPtr = PXR_NS::TfRefPtr<Broker>;
using BrokerWeakPtr = PXR_NS::TfWeakPtr<Broker>;

using DispatcherPtr = PXR_NS::TfRefPtr<Dispatcher>;
using BroadcasterPtr = PXR_NS::TfRefPtr<Broadcaster>;
using BroadcasterPtrList = std::vector<BroadcasterPtr>;
using _StageNoticePtrList = std::vector<BrokerNotice::StageNoticeRefPtr>;
using _StageNoticePtrMap = std::unordered_map<std::string, _StageNoticePtrList>;
using NoticeCaturePredicateFunc =
    std::function<bool (const BrokerNotice::StageNotice &)>;

class Broker : public PXR_NS::TfRefBase, public PXR_NS::TfWeakBase {
public:
    static BrokerPtr Create(const PXR_NS::UsdStageWeakPtr& stage);

    virtual ~Broker() {}

    // Don't allow copies
    Broker(const Broker &) = delete;
    Broker &operator=(const Broker &) = delete;

    const PXR_NS::UsdStageWeakPtr GetStage() const { return _stage; }

    bool IsInTransaction();

    void BeginTransaction();
    void EndTransaction();

    void BeginFilter(const NoticeCaturePredicateFunc& predicate);
    void EndFilter();

    template<class BrokerNotice, class... Args>
    void Send(Args&&... args);

    void Send(const BrokerNotice::StageNoticeRefPtr&);

    DispatcherPtr& GetDispatcher(std::string identifier);
    BroadcasterPtr& GetBroadcaster(std::string identifier);

    template<class T>
    void AddDispatcher();

    template<class T>
    void AddBroadcaster();

private:
    Broker(const PXR_NS::UsdStageWeakPtr&);

    void _MergeNotices();

    static void _CleanCache();

    void _DiscoverDispatchers();
    void _DiscoverBroadcasters();

    void _Add(const BroadcasterPtr&);
    void _Add(const DispatcherPtr&);

    template<class T>
    DispatcherPtr _AddDispatcher();

    template<class T>
    BroadcasterPtr _AddBroadcaster();

    template<class OutputPtr, class OutputFactory>
    void _LoadFromPlugins(const PXR_NS::TfType& type);

    void _RegisterBroadcaster(const BroadcasterPtr&);
    void _ExecuteBroadcasters(_StageNoticePtrMap&);

    // A registry of hashed stage ptr to the corresponding stage's broker ptr.
    static std::unordered_map<size_t, BrokerPtr> Registry;

    PXR_NS::UsdStageWeakPtr _stage;

    _StageNoticePtrMap _noticeMap;
    std::vector<NoticeCaturePredicateFunc> _predicates;
    std::unordered_map<std::string, DispatcherPtr> _dispatcherMap;
    std::unordered_map<std::string, BroadcasterPtr> _broadcasterMap;
    std::vector<std::string> _rootBroadcasters;
    size_t _transactionSize;
};

template<class BrokerNotice, class... Args>
void Broker::Send(Args&&... args)
{
    PXR_NS::TfRefPtr<BrokerNotice> _notice = BrokerNotice::Create(
        std::forward<Args>(args)...);

    Send(_notice);
}

template<class T>
DispatcherPtr Broker::_AddDispatcher()
{
    static_assert(std::is_base_of<Dispatcher, T>::value);
    auto self = PXR_NS::TfCreateWeakPtr(this);
    DispatcherPtr dispatcher = PXR_NS::TfCreateRefPtr(new T(self));
    _Add(dispatcher);
    return dispatcher;
}

template<class T>
void Broker::AddDispatcher()
{
    const auto& dispatcher = _AddDispatcher<T>();
    dispatcher->Register();
}

template<class T>
BroadcasterPtr Broker::_AddBroadcaster()
{
    static_assert(std::is_base_of<Broadcaster, T>::value);
    auto self = PXR_NS::TfCreateWeakPtr(this);
    BroadcasterPtr broadcaster = PXR_NS::TfCreateRefPtr(new T(self));
    _Add(broadcaster);
    return broadcaster;
}

template<class T>
void Broker::AddBroadcaster()
{
    const auto& broadcaster = _AddBroadcaster<T>();
    _RegisterBroadcaster(broadcaster);

    // TODO: Detect cycles
}

template<class OutputPtr, class OutputFactory>
void Broker::_LoadFromPlugins(const PXR_NS::TfType& type)
{
    PXR_NAMESPACE_USING_DIRECTIVE

    const PXR_NS::PlugPluginPtr plugin =
        PXR_NS::PlugRegistry::GetInstance().GetPluginForType(type);

    if (!plugin) {
        return;
    }

    if (!plugin->Load()) {
        TF_CODING_ERROR("Failed to load plugin %s for %s",
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

} // namespace unf

#endif // NOTICE_BROKER_BROKER_H
