#include "unf/broker.h"
#include "unf/notice.h"
#include "unf/dispatcher.h"
#include "unf/broadcaster.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/usd/usd/common.h>
#include <pxr/usd/usd/notice.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace unf {

// Initiate static registry.
std::unordered_map<size_t, BrokerPtr> Broker::Registry;

Broker::Broker(const UsdStageWeakPtr& stage)
    : _stage(stage), _transactionDepth(0)
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

void Broker::_MergeNotices() {
    for (auto& element : _noticeMap) {
        auto& notices = element.second;

        // If there are more than one notice for this type and
        // if the notices are mergeable, we only need to keep the
        // first notice, and all other can be pruned.
        if (notices.size() > 1 && notices[0]->IsMergeable()) {
            auto& notice = notices.at(0);
            auto it = std::next(notices.begin());

            while(it != notices.end()) {
                // Attempt to merge content of notice with first notice
                // if this is possible.
                notice->Merge(std::move(**it));
                it = notices.erase(it);
            }
        }
    }
}

bool Broker::IsInTransaction()
{
    return _transactionDepth != 0;
}

void Broker::BeginTransaction()
{
    _transactionDepth++;
}

void Broker::EndTransaction()
{
    if (!IsInTransaction()) {
        return;
    }

    //If it's the last transaction merge the notices, execute the broadcasters
    //and send out the queued notices.
    if(_transactionDepth == 1) {
        _MergeNotices();
        _ExecuteBroadcasters(_noticeMap);
        
        for (auto& element : _noticeMap) {
        auto& notices = element.second;
            // Send all remaining notices.
            for (const auto& notice: element.second) {
                notice->Send(_stage);
            }
        }
        _noticeMap.clear();
    }

    _transactionDepth --;
}

void Broker::AddFilter(const NoticeCaturePredicateFunc& predicate){
    _predicates.push_back(predicate);
}
void Broker::PopFilter() {
    _predicates.pop_back();
}

void Broker::Send(
    const BrokerNotice::StageNoticeRefPtr& notice)
{
    if (_transactionDepth > 0) {
        for (auto& p : _predicates) {
            if (!p(*notice)) {
                return;
            }
        }
        _noticeMap[notice->GetTypeId()].push_back(notice);
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

void Broker::_ExecuteBroadcasters(_StageNoticePtrMap& noticeMap)
{
    for (auto& broadcaster : _rootBroadcasters) {
        GetBroadcaster(broadcaster)->Execute(&noticeMap);
    }
}

} // namespace unf
