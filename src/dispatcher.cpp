#include "dispatcher.h"
#include "broker.h"
#include "notice.h"
#include "merger.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/usd/usd/common.h>
#include <pxr/usd/usd/notice.h>

PXR_NAMESPACE_OPEN_SCOPE

TF_REGISTRY_FUNCTION(TfType)
{
    TfType::Define<Dispatcher>();
}

Dispatcher::Dispatcher(const NoticeBrokerWeakPtr& broker)
    : _broker(broker)
{

}

void Dispatcher::Revoke()
{
    for (auto& key: _keys) {
        TfNotice::Revoke(key);
    }
}

StageDispatcher::StageDispatcher(const NoticeBrokerWeakPtr& broker)
    : Dispatcher(broker)
{
     _keys.reserve(4);

    _Register<
        UsdNotice::StageContentsChanged,
        UsdBrokerNotice::StageContentsChanged>();
    _Register<
        UsdNotice::ObjectsChanged,
        UsdBrokerNotice::ObjectsChanged>();
    _Register<
        UsdNotice::StageEditTargetChanged,
        UsdBrokerNotice::StageEditTargetChanged>();
    _Register<
        UsdNotice::LayerMutingChanged,
        UsdBrokerNotice::LayerMutingChanged>();
}

void StageDispatcher::Execute(NoticeMerger& merger) {
    _noticeMap = merger.GetNotices();
    Dispatcher::Execute(nullptr);
}


PXR_NAMESPACE_CLOSE_SCOPE
