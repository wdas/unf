#ifndef NOTICE_BROKER_TRANSACTION_H
#define NOTICE_BROKER_TRANSACTION_H

#include "broker.h"

#include "pxr/pxr.h"
#include "pxr/usd/usd/common.h"

PXR_NAMESPACE_OPEN_SCOPE

class NoticeTransaction {
public:
    NoticeTransaction(
        const NoticeBrokerPtr&, 
        const NoticeCaturePredicateFunc& predicate=nullptr);
    NoticeTransaction(
        const UsdStageRefPtr&, 
        const NoticeCaturePredicateFunc& predicate=nullptr);

    ~NoticeTransaction();

    // Don't allow copies
    NoticeTransaction(const NoticeTransaction &) = delete;
    NoticeTransaction &operator=(const NoticeTransaction &) = delete;

    // TODO: Add Getter to access broker

private:
    NoticeBrokerPtr _broker;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROKER_TRANSACTION_H
