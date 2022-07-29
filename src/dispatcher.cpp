#include "dispatcher.h"
#include "broker.h"
#include "notice.h"

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

StageDispatcher::StageDispatcher(const NoticeBrokerWeakPtr& broker)
    : Dispatcher(broker)
{

}

void StageDispatcher::Register()
{
    _keys.reserve(4);

    _Register<
        UsdBrokerNotice::StageContentsChanged,
        UsdNotice::StageContentsChanged>();
    _Register<
        UsdBrokerNotice::ObjectsChanged,
        UsdNotice::ObjectsChanged>();
    _Register<
        UsdBrokerNotice::StageEditTargetChanged,
        UsdNotice::StageEditTargetChanged>();
    _Register<
        UsdBrokerNotice::LayerMutingChanged,
        UsdNotice::LayerMutingChanged>();
}

void StageDispatcher::Revoke()
{
    for (auto& key: _keys) {
        TfNotice::Revoke(key);
    }
}

PXR_NAMESPACE_CLOSE_SCOPE
