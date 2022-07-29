#include "merger.h"
#include "dispatcher.h"
#include "notice.h"

#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>

PXR_NAMESPACE_OPEN_SCOPE

void NoticeMerger::Add(
    const UsdBrokerNotice::StageNoticeRefPtr& notice)
{
    // Indicate whether the notice needs to be captured.
    if (_predicate && !_predicate(*notice))
        return;

    // Store notices per type name, so that each type can be merged if
    // required.
    std::string name = notice->GetTypeId();
    _noticeMap[name].push_back(notice);
}

void NoticeMerger::Join(NoticeMerger& merger)
{
    for (auto& element : merger._noticeMap) {
        auto& source = element.second;
        auto& target = _noticeMap[element.first];

        target.reserve(target.size() + source.size());
        std::move(
            std::begin(source),
            std::end(source),
            std::back_inserter(target));
        source.clear();
    }

    merger._noticeMap.clear();
}

void NoticeMerger::Merge() {
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

void NoticeMerger::Send(const UsdStageWeakPtr& stage)
{
    for (auto& element : _noticeMap) {
        auto& notices = element.second;

        // Send all remaining notices.
        for (const auto& notice: element.second) {
            notice->Send(stage);
        }
    }
}

PXR_NAMESPACE_CLOSE_SCOPE