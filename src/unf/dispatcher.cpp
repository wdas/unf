#include "unf/dispatcher.h"
#include "unf/broker.h"
#include "unf/notice.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/usd/usd/common.h>
#include <pxr/usd/usd/notice.h>

PXR_NAMESPACE_OPEN_SCOPE

namespace unf {

TF_REGISTRY_FUNCTION(TfType)
{
    TfType::Define<Dispatcher>();
}

Dispatcher::Dispatcher(const BrokerWeakPtr& broker)
    : _broker(broker)
{

}

void Dispatcher::Revoke()
{
    for (auto& key: _keys) {
        TfNotice::Revoke(key);
    }
}

StageDispatcher::StageDispatcher(const BrokerWeakPtr& broker)
    : Dispatcher(broker)
{

}

void StageDispatcher::Register()
{
    _keys.reserve(4);

    _Register<
        UsdNotice::StageContentsChanged,
        BrokerNotice::StageContentsChanged>();
    _Register<
        UsdNotice::ObjectsChanged,
        BrokerNotice::ObjectsChanged>();
    _Register<
        UsdNotice::StageEditTargetChanged,
        BrokerNotice::StageEditTargetChanged>();
    _Register<
        UsdNotice::LayerMutingChanged,
        BrokerNotice::LayerMutingChanged>();
}

} // namespace unf

PXR_NAMESPACE_CLOSE_SCOPE
