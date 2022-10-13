#include "unf/transaction.h"
#include "unf/broker.h"
#include "unf/capturePredicate.h"

#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace unf {

NoticeTransaction::NoticeTransaction(
    const BrokerPtr& broker, CapturePredicate predicate)
    : _broker(broker)
{
    _broker->BeginTransaction(predicate);
}

NoticeTransaction::NoticeTransaction(
    const BrokerPtr& broker, const CapturePredicateFunc& predicate)
    : _broker(broker)
{
    _broker->BeginTransaction(predicate);
}

NoticeTransaction::NoticeTransaction(
    const UsdStageRefPtr& stage, CapturePredicate predicate)
    : _broker(Broker::Create(stage))
{
    _broker->BeginTransaction(predicate);
}

NoticeTransaction::NoticeTransaction(
    const PXR_NS::UsdStageRefPtr& stage, const CapturePredicateFunc& predicate)
    : _broker(Broker::Create(stage))
{
    _broker->BeginTransaction(predicate);
}

NoticeTransaction::~NoticeTransaction() { _broker->EndTransaction(); }

}  // namespace unf
