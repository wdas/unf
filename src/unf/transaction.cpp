#include "unf/transaction.h"
#include "unf/broker.h"

#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace unf {

NoticeTransaction::NoticeTransaction(
    const BrokerPtr& broker, const NoticeCaturePredicateFunc& predicate)
    : _broker(broker)
{
    _broker->BeginTransaction();
}

NoticeTransaction::NoticeTransaction(
    const UsdStageRefPtr& stage, const NoticeCaturePredicateFunc& predicate)
    : _broker(Broker::Create(stage))
{
    _broker->BeginTransaction(predicate);
}

NoticeTransaction::~NoticeTransaction() { _broker->EndTransaction(); }

}  // namespace unf
