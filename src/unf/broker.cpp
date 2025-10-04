#include "unf/broker.h"
#include "unf/capturePredicate.h"
#include "unf/dispatcher.h"
#include "unf/notice.h"

#include <pxr/base/tf/weakPtr.h>
#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>
#include <pxr/usd/usd/notice.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace unf {

// Initiate static registry.
std::unordered_map<UsdStageWeakPtr, BrokerPtr, Broker::UsdStageWeakPtrHasher>
    Broker::Registry;

Broker::Broker(const UsdStageWeakPtr& stage) : _stage(stage)
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
    Broker::_CleanCache();

    // If there doesn't exist a broker for the given stage, create a new broker.
    if (Registry.find(stage) == Registry.end()) {
        Registry[stage] = TfCreateRefPtr(new Broker(stage));
    }

    return Registry[stage];
}

const UsdStageWeakPtr Broker::GetStage() const { return _stage; }

bool Broker::IsInTransaction() { return _mergers.size() > 0; }

void Broker::BeginTransaction(CapturePredicate predicate)
{
    _mergers.push_back(_NoticeMerger(predicate));
}

void Broker::BeginTransaction(const CapturePredicateFunc& function)
{
    _mergers.push_back(_NoticeMerger(CapturePredicate(function)));
}

void Broker::EndTransaction()
{
    if (!IsInTransaction()) {
        return;
    }

    _NoticeMerger& merger = _mergers.back();

    // If there are only one merger left, process all notices.
    if (_mergers.size() == 1) {
        merger.Merge();
        merger.PostProcess();
        merger.Send(_stage);
    }
    // Otherwise, it means that we are in a nested transaction that should
    // not be processed yet. Join data with next merger.
    else {
        (_mergers.end() - 2)->Join(merger);
    }

    _mergers.pop_back();
}

void Broker::Send(const UnfNotice::StageNoticeRefPtr& notice)
{
    if (_mergers.size() > 0) {
        _mergers.back().Add(notice);
    }
    // Otherwise, send the notice.
    else {
        notice->Send(_stage);
    }
}

DispatcherPtr& Broker::GetDispatcher(std::string identifier)
{
    return _dispatcherMap.at(identifier);
}

void Broker::Reset() { Registry.erase(_stage); }

void Broker::ResetAll() { Registry.clear(); }

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

Broker::_NoticeMerger::_NoticeMerger(CapturePredicate predicate)
    : _predicate(std::move(predicate))
{
}

void Broker::_NoticeMerger::Add(const UnfNotice::StageNoticeRefPtr& notice)
{
    // Indicate whether the notice needs to be captured.
    if (!_predicate(*notice)) return;

    // Store notices per type name, so that each type can be merged if
    // required.
    std::string name = notice->GetTypeId();
    _noticeMap[name].push_back(notice);
}

void Broker::_NoticeMerger::Join(_NoticeMerger& merger)
{
    for (auto& element : merger._noticeMap) {
        auto& source = element.second;
        auto& target = _noticeMap[element.first];

        target.reserve(target.size() + source.size());
        std::move(
            std::begin(source), std::end(source), std::back_inserter(target));

        source.clear();
    }

    merger._noticeMap.clear();
}

void Broker::_NoticeMerger::Merge()
{
    for (auto& element : _noticeMap) {
        auto& notices = element.second;

        // If there are more than one notice for this type and
        // if the notices are mergeable, we only need to keep the
        // first notice, and all other can be pruned.
        if (notices.size() > 1 && notices[0]->IsMergeable()) {
            auto& notice = notices.at(0);

            auto it = std::next(notices.begin());
            for (; it != notices.end(); ++it) {
                // Attempt to merge content of notice with first notice
                // if this is possible.
                notice->Merge(std::move(**it));
            }
            notices.resize(1);
        }
    }
}

void Broker::_NoticeMerger::PostProcess()
{
    for (auto& element : _noticeMap) {
        auto& notice = element.second[0];
        notice->PostProcess();
    }
}

void Broker::_NoticeMerger::Send(const UsdStageWeakPtr& stage)
{
    for (auto& element : _noticeMap) {
        auto& notices = element.second;

        // Send all remaining notices.
        for (const auto& notice : element.second) {
            notice->Send(stage);
        }
    }
}

}  // namespace unf
