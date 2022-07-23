#include "broker.h"
#include "notice.h"
#include "dispatcher.h"
#include "merger.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/usd/usd/common.h>
#include <pxr/usd/usd/notice.h>
#include <pxr/base/plug/plugin.h>
#include <pxr/base/plug/registry.h>

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
    DiscoverDispatchers();

    // Register all dispatchers
    for (auto& d: _dispatcherMap) {
        d.second->Register();
    }
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
    return _mergers.size() > 0;
}

void NoticeBroker::BeginTransaction(
    const NoticeCaturePredicateFunc& predicate)
{
    _mergers.push_back(NoticeMerger(predicate));
}

void NoticeBroker::EndTransaction()
{
    if (!IsInTransaction()) {
        return;
    }

    NoticeMerger& merger = _mergers.back();

    // If there are only one merger left, process all notices.
    if (_mergers.size() == 1) {
        merger.MergeAndSend(_stage);
    }
    // Otherwise, it means that we are in a nested transaction that should
    // not be processed yet. Join merger data with next merger.
    else {
       (_mergers.end()-2)->Join(merger);
    }

    _mergers.pop_back();
}

void NoticeBroker::Send(const UsdBrokerNotice::StageNoticeRefPtr notice)
{
    // Capture the notice to be processed later if a merger is pending.
    if (_mergers.size() > 0) {
        _mergers.back().Capture(notice);
    }
    // Otherwise, send the notice.
    else {
        notice->Send(_stage);
    }
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

void NoticeBroker::DiscoverDispatchers()
{
    std::set<TfType> dispatcherTypes;
    PlugRegistry::GetAllDerivedTypes(
        TfType::Find<Dispatcher>(), &dispatcherTypes);

    auto self = TfCreateWeakPtr(this);

    for (const TfType& dispatcherType : dispatcherTypes) {
        const PlugPluginPtr plugin =
            PlugRegistry::GetInstance().GetPluginForType(dispatcherType);

        if (!plugin) {
            continue;
        }

        if (!plugin->Load()) {
            TF_CODING_ERROR("Failed to load plugin %s for %s",
                plugin->GetName().c_str(),
                dispatcherType.GetTypeName().c_str());
            continue;
        }

        DispatcherPtr dispatcher;
        DispatcherFactoryBase* factory =
            dispatcherType.GetFactory<DispatcherFactoryBase>();

        if (factory) {
            dispatcher = factory->New(self);
        }

        if (!dispatcher) {
            TF_CODING_ERROR(
                "Failed to manufacture dispatcher %s from plugin %s",
                dispatcherType.GetTypeName().c_str(),
                plugin->GetName().c_str());
        }

        _dispatcherMap[dispatcher->GetIdentifier()] = dispatcher;
    }
}

PXR_NAMESPACE_CLOSE_SCOPE
