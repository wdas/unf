#ifndef NOTICE_BROKER_TRANSACTION_H
#define NOTICE_BROKER_TRANSACTION_H

#include "UsdNoticeBroker/broker.h"

#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>

PXR_NAMESPACE_OPEN_SCOPE

namespace UNB {

class NoticeTransaction {
public:
    NoticeTransaction(
        const BrokerPtr&,
        const NoticeCaturePredicateFunc& predicate=nullptr);
    NoticeTransaction(
        const UsdStageRefPtr&,
        const NoticeCaturePredicateFunc& predicate=nullptr);

    ~NoticeTransaction();

    // Don't allow copies
    NoticeTransaction(const NoticeTransaction &) = delete;
    NoticeTransaction &operator=(const NoticeTransaction &) = delete;

    BrokerPtr GetBroker() { return _broker; }

private:
    BrokerPtr _broker;
};

} // namespace UNB

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROKER_TRANSACTION_H
