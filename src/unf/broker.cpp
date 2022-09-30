#include "unf/broker.h"
#include "unf/dispatcher.h"
#include "unf/notice.h"
#include "unf/merger.h"

#include <pxr/base/tf/weakPtr.h>
#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>
#include <pxr/usd/usd/notice.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace unf {

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
    for (auto& element : _dispatcherMap) {
        element.second->Register();
    }
}

BrokerPtr Broker::Create(const UsdStageWeakPtr& stage)
{
    size_t stageHash = hash_value(stage);

    Broker::_CleanCache();

    // If there doesn't exist a broker for the given stage, create a new broker.
    if (Registry.find(stageHash) == Registry.end()) {
        Registry[stageHash] = TfCreateRefPtr(new Broker(stage));
    }

    return Registry[stageHash];
}

bool Broker::IsInTransaction() { return _mergers.size() > 0; }

void Broker::BeginTransaction(
    const NoticeCaturePredicateFunc& predicate)
{
    _mergers.push_back(NoticeMerger(predicate));
}

void Broker::EndTransaction()
{
    if (!IsInTransaction()) {
        return;
    }

    NoticeMerger& merger = _mergers.back();

    // If there are only one merger left, process all notices.
    if (_mergers.size() == 1) {
        merger.Merge();
        merger.Send(_stage);
    }
    // Otherwise, it means that we are in a nested transaction that should
    // not be processed yet. Join data with next merger.
    else {
       (_mergers.end()-2)->Join(merger);
    }

    _mergers.pop_back();
}

void Broker::Send(const BrokerNotice::StageNoticeRefPtr& notice)
{
    if (_mergers.size() > 0) {
        _mergers.back().Add(notice);
    }
    // Otherwise, send the notice.
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

void Broker::Reset()
{
    size_t stageHash = hash_value(_stage);
    Registry.erase(stageHash);
}

void Broker::ResetAll()
{
    Registry.clear();
}

void Broker::_CleanCache()
{
    for (auto it = Registry.begin(); it != Registry.end();) {
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

void Broker::_Add(const DispatcherPtr& dispatcher)
{
    _dispatcherMap[dispatcher->GetIdentifier()] = dispatcher;
}

}  // namespace unf
