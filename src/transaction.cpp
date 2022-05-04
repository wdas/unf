#include "transaction.h"
#include "broker.h"

#include "pxr/pxr.h"
#include "pxr/usd/usd/common.h"

PXR_NAMESPACE_OPEN_SCOPE

NoticeTransaction::NoticeTransaction(
    const NoticeBrokerPtr& broker, 
    const NoticeCaturePredicateFunc& predicate)
    : _broker(broker)
{
    _broker->BeginTransaction(predicate);
}

// TODO: Add a register to ensure a unique Broker per Stage
NoticeTransaction::NoticeTransaction(
    const PXR_NS::UsdStageRefPtr& stage, 
    const NoticeCaturePredicateFunc& predicate)
    : _broker(NoticeBroker::Create(stage))
{
    _broker->BeginTransaction(predicate);
}

NoticeTransaction::~NoticeTransaction()
{
    _broker->EndTransaction();
}

PXR_NAMESPACE_CLOSE_SCOPE
