#ifndef USD_NOTICE_FRAMEWORK_TRANSACTION_H
#define USD_NOTICE_FRAMEWORK_TRANSACTION_H

/// \file unf/transaction.h

#include "unf/api.h"
#include "unf/broker.h"
#include "unf/capturePredicate.h"

#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>

namespace unf {

/// \class NoticeTransaction
///
/// \brief
/// Convenient [RAII](https://en.cppreference.com/w/cpp/language/raii) object
/// to consolidate and filter notices derived from UnfNotice::StageNotice
/// within a specific scope.
class NoticeTransaction {
  public:
    /// \brief
    /// Create transaction from a Broker.
    ///
    /// Notices derived from UnfNotice::StageNotice will be held during
    /// the transaction and emitted at the end.
    ///
    /// By default, all UnfNotice::StageNotice notices will be captured during
    /// the entire scope of the transaction. A CapturePredicate can be passed to
    /// influence which notices are captured. Notices that are not captured
    /// will not be emitted.
    UNF_API NoticeTransaction(
        const BrokerPtr &,
        CapturePredicate predicate = CapturePredicate::Default());

    /// \brief
    /// Create transaction from a Broker with a capture predicate function.
    ///
    /// The following example will filter out all 'Foo' notices emitted during
    /// the transaction.
    ///
    /// \code{.cpp}
    /// NoticeTransaction t(broker, [&](const unf::UnfNotice::StageNotice& n) {
    ///     return (n.GetTypeId() != typeid(Foo).name());
    /// });
    /// \endcode
    UNF_API NoticeTransaction(const BrokerPtr &, const CapturePredicateFunc &);

    /// \brief
    /// Create transaction from a UsdStage.
    ///
    /// Convenient constructor to encapsulate the creation of the broker.
    ///
    /// \sa
    /// NoticeTransaction(const BrokerPtr &, CapturePredicate predicate =
    /// CapturePredicate::Default())
    UNF_API NoticeTransaction(
        const PXR_NS::UsdStageRefPtr &,
        CapturePredicate predicate = CapturePredicate::Default());

    /// \brief
    /// Create transaction from a UsdStage with a capture predicate function.
    ///
    /// Convenient constructor to encapsulate the creation of the broker.
    ///
    /// \sa
    /// NoticeTransaction(const BrokerPtr &, const CapturePredicateFunc&)

    UNF_API NoticeTransaction(
        const PXR_NS::UsdStageRefPtr &, const CapturePredicateFunc &);

    /// Delete object and end transaction.
    UNF_API virtual ~NoticeTransaction();

    /// Remove default copy constructor.
    UNF_API NoticeTransaction(const NoticeTransaction &) = delete;

    /// Remove default assignment operator.
    UNF_API NoticeTransaction &operator=(const NoticeTransaction &) = delete;

    /// Return associated Broker instance.
    UNF_API BrokerPtr GetBroker() { return _broker; }

  private:
    /// Broker associated with transaction.
    BrokerPtr _broker;
};

}  // namespace unf

#endif  // USD_NOTICE_FRAMEWORK_TRANSACTION_H
