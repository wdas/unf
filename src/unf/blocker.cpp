#include "unf/blocker.h"
#include "unf/notice.h"
#include "unf/broker.h"

#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace unf {

NoticeBlocker::NoticeBlocker(const BrokerPtr& broker)
    : _broker(broker)
{
    _broker->BeginTransaction(
        [](const unf::BrokerNotice::StageNotice&) { return false; });
}

NoticeBlocker::NoticeBlocker(const UsdStageRefPtr& stage)
    : _broker(Broker::Create(stage))
{
    _broker->BeginTransaction(
        [](const unf::BrokerNotice::StageNotice&) { return false; });
}

NoticeBlocker::~NoticeBlocker() { _broker->EndTransaction(); }

}  // namespace unf
