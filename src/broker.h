#ifndef NOTICE_BROKER_BROKER_H
#define NOTICE_BROKER_BROKER_H

#include "notice.h"
#include "merger.h"

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

PXR_NAMESPACE_OPEN_SCOPE

class NoticeBroker;
class Dispatcher;
class Broadcaster;

using NoticeBrokerPtr = TfRefPtr<NoticeBroker>;
using NoticeBrokerWeakPtr = TfWeakPtr<NoticeBroker>;

using DispatcherPtr = TfRefPtr<Dispatcher>;
using BroadcasterPtr = TfRefPtr<Broadcaster>;
using BroadcasterPtrList = std::vector<BroadcasterPtr>;

class NoticeBroker : public TfRefBase, public TfWeakBase {
public:
    static NoticeBrokerPtr Create(const UsdStageWeakPtr& stage);

    virtual ~NoticeBroker() {}

    // Don't allow copies
    NoticeBroker(const NoticeBroker &) = delete;
    NoticeBroker &operator=(const NoticeBroker &) = delete;

    const UsdStageWeakPtr& GetStage() const { return _stage; }

    bool IsInTransaction();

    void BeginTransaction(const NoticeCaturePredicateFunc& predicate=nullptr);
    void EndTransaction();

    template<class BrokerNotice, class... Args>
    void Send(Args&&... args);

    void Send(const UsdBrokerNotice::StageNoticeRefPtr&);

    DispatcherPtr& GetDispatcher(std::string identifier);
    BroadcasterPtr& GetBroadcaster(std::string identifier);

    template<class T>
    void AddDispatcher() {
        static_assert(std::is_base_of<Dispatcher, T>::value);
        auto self = TfCreateWeakPtr(this);
        DispatcherPtr dispatcher = TfCreateRefPtr(new T(self));
        _Add(dispatcher);
    }

    template<class T>
    void AddBroadcaster() {
        static_assert(std::is_base_of<Broadcaster, T>::value);
        auto self = TfCreateWeakPtr(this);
        BroadcasterPtr broadcaster = TfCreateRefPtr(new T(self));
        _Add(broadcaster);
    }

private:
    NoticeBroker(const UsdStageWeakPtr&);

    static void _CleanCache();

    void _DiscoverDispatchers();
    void _DiscoverBroadcasters();

    void _Add(const BroadcasterPtr&);
    void _Add(const DispatcherPtr&);

    template<class OutputPtr, class OutputFactory>
    void _LoadFromPlugin(const TfType& type);

    void _ExecuteBroadcasters(NoticeMerger& merger);

    // A registry of hashed stage ptr to the corresponding stage's broker ptr.
    static std::unordered_map<size_t, NoticeBrokerPtr> Registry;

    UsdStageWeakPtr _stage;

    std::vector<NoticeMerger> _mergers;

    std::unordered_map<std::string, DispatcherPtr> _dispatcherMap;
    std::unordered_map<std::string, BroadcasterPtr> _broadcasterMap;
    std::vector<std::string> _rootBroadcasters;
};

template<class BrokerNotice, class... Args>
void NoticeBroker::Send(Args&&... args)
{
    TfRefPtr<BrokerNotice> _notice = BrokerNotice::Create(
        std::forward<Args>(args)...);

    Send(_notice);
}

template<class OutputPtr, class OutputFactory>
void NoticeBroker::_LoadFromPlugin(const TfType& type)
{
    const PlugPluginPtr plugin =
        PlugRegistry::GetInstance().GetPluginForType(type);

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
    }

    _Add(output);
}

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROKER_BROKER_H
