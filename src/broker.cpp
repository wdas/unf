#include "broker.h"
#include "notice.h"
#include "dispatcher.h"

#include "pxr/pxr.h"
#include "pxr/base/tf/weakPtr.h"
#include "pxr/usd/usd/common.h"
#include "pxr/usd/usd/notice.h"

PXR_NAMESPACE_OPEN_SCOPE

NoticeBroker::NoticeBroker(const UsdStageWeakPtr& stage)
    : _stage(stage)
{
    AddDispatcher<StageDispatcher>();
}

bool NoticeBroker::IsInTransaction()
{
    return _transactions.size() > 0;
}

void NoticeBroker::BeginTransaction(
    const NoticeCaturePredicateFunc& predicate)
{
    _TransactionHandler transaction;
    transaction.predicate = predicate;

    _transactions.push_back(std::move(transaction));
}

void NoticeBroker::EndTransaction()
{
    if (_transactions.size() == 0) {
        return;
    }

    _TransactionHandler& transaction = _transactions.back();

    // If there are only one transaction left, process all notices.
    if (_transactions.size() == 1) {
        _SendNotices(transaction);
    }
    // Otherwise, it means that we are in a nested transaction that should
    // not be processed yet. Join transaction data with next broker.
    else {
       (_transactions.end()-2)->Join(transaction);
    }

    _transactions.pop_back();
}

void NoticeBroker::_SendNotices(_TransactionHandler& transaction)
{
    for (auto& element : transaction.noticeMap) {
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

        // Send all remaining notices.
        for (const auto& notice: notices) {
            notice->Send(_stage);
        }
    }
}

void NoticeBroker::_TransactionHandler::Join(
    _TransactionHandler& transaction)
{
    for (auto& element : transaction.noticeMap) {
        auto& source = element.second;
        auto& target = noticeMap[element.first];

        target.reserve(target.size() + source.size());
        std::move(
            std::begin(source), 
            std::end(source), 
            std::back_inserter(target));
        source.clear();
    }
    transaction.noticeMap.clear();
}

PXR_NAMESPACE_CLOSE_SCOPE
