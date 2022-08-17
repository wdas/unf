#include "UsdNoticeBroker/broker.h"
#include "UsdNoticeBroker/notice.h"
#include "UsdNoticeBroker/dispatcher.h"
#include "UsdNoticeBroker/broadcaster.h"
#include "UsdNoticeBroker/merger.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/usd/usd/common.h>
#include <pxr/usd/usd/notice.h>

PXR_NAMESPACE_OPEN_SCOPE

namespace UNB {

// Initiate static registry.
std::unordered_map<size_t, BrokerPtr> Broker::Registry;

Broker::Broker(const UsdStageWeakPtr& stage)
    : _stage(stage)
{
    // Add default dispatcher.
    _AddDispatcher<StageDispatcher>();

    // Discover dispatchers added via plugin to complete or override
    // default dispatcher.
    _DiscoverDispatchers();

    // Register all dispatchers
    for (auto& element: _dispatcherMap) {
        element.second->Register();
    }

    // Discover broadcaster added via plugin to infer granular notices.
    _DiscoverBroadcasters();
}

BrokerPtr Broker::Create(const UsdStageWeakPtr& stage)
{
    size_t stageHash = hash_value(stage);

    Broker::_CleanCache();

    // If there doesn't exist a broker for the given stage, create a new broker.
    if(Registry.find(stageHash) == Registry.end()) {
        Registry[stageHash] = TfCreateRefPtr(new Broker(stage));
    }

    return Registry[stageHash];
}

bool Broker::IsInTransaction()
{
    return _mergers.size() > 0;
}

void Broker::BeginTransaction(
    const NoticeCaturePredicateFunc& predicate)
{
    _mergers.push_back(NoticeMerger::Create(predicate));
}

void Broker::EndTransaction()
{
    if (!IsInTransaction()) {
        return;
    }

    NoticeMergerPtr& merger = _mergers.back();

    // Merge all notices captured in this transaction and run
    // all broadcasters.
    merger->Merge();
    _ExecuteBroadcasters(merger);

    // Join previous transaction if necessary.
    if (_latestMerger) {
        merger->Join(*_latestMerger);
        _latestMerger = nullptr;
    }

    // Merge again to ensure that new notices added by broadcasters
    // and previous transaction are optimized.
    merger->Merge();

    // If there are only one merger left, process all notices.
    if (_mergers.size() == 1) {
        merger->Send(_stage);
    }
    // Otherwise, it means that we are in a nested transaction that
    // should not be processed yet. Save it for joining it with next
    // transaction later.
    else {
        _latestMerger = merger;
    }

    _mergers.pop_back();
}

void Broker::Send(
    const BrokerNotice::StageNoticeRefPtr& notice)
{
    if (_mergers.size() > 0) {
        _mergers.back()->Add(notice);
    }
    // Otherwise, send the notice via broadcaster.
    else {
        BeginTransaction();
        Send(notice);
        EndTransaction();
    }
}

DispatcherPtr& Broker::GetDispatcher(std::string identifier)
{
    return _dispatcherMap.at(identifier);
}

BroadcasterPtr& Broker::GetBroadcaster(std::string identifier)
{
    return _broadcasterMap.at(identifier);
}

void Broker::_CleanCache() {
    for (auto it = Registry.begin();
        it != Registry.end();)
    {
        // If the stage doesn't exist anymore, delete the corresponding
        // broker from the registry.
        if (it->second->GetStage().IsExpired()) {
            it = Registry.erase(it);
        }
        else {
            it++;
        }
    }
}

void Broker::_DiscoverDispatchers()
{
    TfType root = TfType::Find<Dispatcher>();
    std::set<TfType> types;
    PlugRegistry::GetAllDerivedTypes(root, &types);

    for (const TfType& type : types) {
        _LoadFromPlugins<DispatcherPtr, DispatcherFactory>(type);
    }
}

void Broker::_DiscoverBroadcasters()
{
    TfType root = TfType::Find<Broadcaster>();
    std::set<TfType> types;
    PlugRegistry::GetAllDerivedTypes(root, &types);

    for (const TfType& type : types) {
        _LoadFromPlugins<BroadcasterPtr, BroadcasterFactory>(type);
    }

    // Register all broadcasters to build up dependency graph.
    for (auto& element: _broadcasterMap) {
        _RegisterBroadcaster(element.second);
    }

    // Detect errors
    // TODO: Detect cycles
}

void Broker::_Add(const DispatcherPtr& dispatcher)
{
    _dispatcherMap[dispatcher->GetIdentifier()] = dispatcher;
}

void Broker::_Add(const BroadcasterPtr& broadcaster)
{
    _broadcasterMap[broadcaster->GetIdentifier()] = broadcaster;
}

void Broker::_RegisterBroadcaster(
    const BroadcasterPtr& broadcaster)
{
    const auto& identifier = broadcaster->GetIdentifier();
    const auto& parentId = broadcaster->GetParentIdentifier();

    if (!parentId.size()) {
        _rootBroadcasters.push_back(identifier);
    }
    else {
        auto& parent = GetBroadcaster(parentId);
        parent->_AddChild(broadcaster);
    }
}

void Broker::_ExecuteBroadcasters(NoticeMergerPtr& merger)
{
    _StageNoticePtrMap rootNotices = merger->GetNotices();

    for (auto& broadcaster : _rootBroadcasters) {
        GetBroadcaster(broadcaster)->Execute(&rootNotices);
    }
}

} // namespace UNB

PXR_NAMESPACE_CLOSE_SCOPE
