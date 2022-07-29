#include "broker.h"
#include "notice.h"
#include "dispatcher.h"
#include "broadcaster.h"
#include "context.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/usd/usd/common.h>
#include <pxr/usd/usd/notice.h>

PXR_NAMESPACE_OPEN_SCOPE

// Initiate static registry.
std::unordered_map<size_t, NoticeBrokerPtr> NoticeBroker::Registry;

NoticeBroker::NoticeBroker(const UsdStageWeakPtr& stage)
    : _stage(stage)
{
    // Add default dispatcher.
    AddDispatcher<StageDispatcher>();

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

NoticeBrokerPtr NoticeBroker::Create(const UsdStageWeakPtr& stage)
{
    size_t stageHash = hash_value(stage);

    NoticeBroker::_CleanCache();

    // If there doesn't exist a broker for the given stage, create a new broker.
    if(Registry.find(stageHash) == Registry.end()) {
        Registry[stageHash] = TfCreateRefPtr(new NoticeBroker(stage));
    }

    return Registry[stageHash];
}

bool NoticeBroker::IsInTransaction()
{
    return _transactions.size() > 0;
}

void NoticeBroker::BeginTransaction(
    const NoticeCaturePredicateFunc& predicate)
{
    _transactions.push_back(NoticeContext());
    _transactions.back().SetFilterPredicate(predicate);
}

void NoticeBroker::EndTransaction()
{
    if (!IsInTransaction()) {
        return;
    }

    NoticeContext& transaction = _transactions.back();

    // TODO: Figure out how to execute broadcasters properly.

    // If there are only one transaction left, process all notices.
    if (_transactions.size() == 1) {
        transaction.Merge();
        transaction.SendAll(_stage);
    }
    // Otherwise, it means that we are in a nested transaction that should
    // not be processed yet. Join transaction data with next transaction.
    else {
       (_transactions.end()-2)->Join(transaction);
    }

    _transactions.pop_back();
}

void NoticeBroker::Send(
    const UsdBrokerNotice::StageNoticeRefPtr& notice)
{
    if (_transactions.size() > 0) {
        _transactions.back().Capture(notice);
    }
    // Otherwise, send the notice via broadcaster.
    else {
        NoticeContext context(notice);

        for (const auto& identifier: _rootBroadcasters) {
            GetBroadcaster(identifier)->_Execute(context);
        }

        context.SendAll(_stage);
    }
}

DispatcherPtr& NoticeBroker::GetDispatcher(std::string identifier)
{
    return _dispatcherMap.at(identifier);
}

BroadcasterPtr& NoticeBroker::GetBroadcaster(std::string identifier)
{
    return _broadcasterMap.at(identifier);
}

void NoticeBroker::_CleanCache() {
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

void NoticeBroker::_DiscoverDispatchers()
{
    TfType root = TfType::Find<Dispatcher>();
    std::set<TfType> types;
    PlugRegistry::GetAllDerivedTypes(root, &types);

    for (const TfType& type : types) {
        _LoadFromPlugin<DispatcherPtr, DispatcherFactory>(type);
    }
}

void NoticeBroker::_DiscoverBroadcasters()
{
    TfType root = TfType::Find<Broadcaster>();
    std::set<TfType> types;
    PlugRegistry::GetAllDerivedTypes(root, &types);

    for (const TfType& type : types) {
        _LoadFromPlugin<BroadcasterPtr, BroadcasterFactory>(type);
    }

    // Construct hierarchy.
    for (auto& element: _broadcasterMap) {
        const auto& identifier = element.first;
        auto& broadcaster = element.second;

        const auto& parentId = broadcaster->GetParentIdentifier();
        if (!parentId.size()) {
            _rootBroadcasters.push_back(identifier);
        }
        else {
            auto& parent = GetBroadcaster(parentId);
            parent->_AddChild(broadcaster);
        }
    }

    // Detect errors
    // TODO: Detect cycles
}

void NoticeBroker::_Add(const DispatcherPtr& dispatcher)
{
    _dispatcherMap[dispatcher->GetIdentifier()] = dispatcher;
}

void NoticeBroker::_Add(const BroadcasterPtr& broadcaster)
{
    _broadcasterMap[broadcaster->GetIdentifier()] = broadcaster;
}

PXR_NAMESPACE_CLOSE_SCOPE
