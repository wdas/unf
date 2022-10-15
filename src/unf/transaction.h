#ifndef USD_NOTICE_FRAMEWORK_TRANSACTION_H
#define USD_NOTICE_FRAMEWORK_TRANSACTION_H

/// \file unf/transaction.h

#include "unf/broker.h"
#include "unf/capturePredicate.h"

#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>

namespace unf {

class NoticeTransaction {
  public:
    NoticeTransaction(
        const BrokerPtr &,
        CapturePredicate predicate = CapturePredicate::Default());

    NoticeTransaction(const BrokerPtr &, const CapturePredicateFunc&);

    NoticeTransaction(
        const PXR_NS::UsdStageRefPtr &,
        CapturePredicate predicate = CapturePredicate::Default());

    NoticeTransaction(
      const PXR_NS::UsdStageRefPtr &, const CapturePredicateFunc&);

    ~NoticeTransaction();

    // Don't allow copies
    NoticeTransaction(const NoticeTransaction &) = delete;
    NoticeTransaction &operator=(const NoticeTransaction &) = delete;

    BrokerPtr GetBroker() { return _broker; }

  private:
    BrokerPtr _broker;
};

}  // namespace unf

#endif  // USD_NOTICE_FRAMEWORK_TRANSACTION_H
