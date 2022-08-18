#include "unf/transaction.h"
#include "unf/broker.h"

#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>

PXR_NAMESPACE_OPEN_SCOPE

namespace unf {

NoticeTransaction::NoticeTransaction(
    const BrokerPtr& broker,
    const NoticeCaturePredicateFunc& predicate)
    : _broker(broker)
{
    _broker->BeginTransaction(predicate);
}

// TODO: Add a register to ensure a unique Broker per Stage
NoticeTransaction::NoticeTransaction(
    const UsdStageRefPtr& stage,
    const NoticeCaturePredicateFunc& predicate)
    : _broker(Broker::Create(stage))
{
    _broker->BeginTransaction(predicate);
}

NoticeTransaction::~NoticeTransaction()
{
    _broker->EndTransaction();
}

} // namespace unf

PXR_NAMESPACE_CLOSE_SCOPE
