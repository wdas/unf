#include "notice.h"

#include "pxr/pxr.h"
#include "pxr/base/tf/notice.h"
#include "pxr/usd/usd/notice.h"

PXR_NAMESPACE_OPEN_SCOPE

namespace UsdBrokerNotice {

TF_REGISTRY_FUNCTION(TfType)
{
    TfType::Define<StageNotice, TfType::Bases<TfNotice> >();

    TfType::Define<StageContentsChanged, TfType::Bases<StageNotice> >();
    TfType::Define<StageEditTargetChanged, TfType::Bases<StageNotice> >();
    TfType::Define<ObjectsChanged, TfType::Bases<StageNotice> >();
    TfType::Define<LayerMutingChanged, TfType::Bases<StageNotice> >();
}

ObjectsChanged::ObjectsChanged(
    const UsdNotice::ObjectsChanged& notice)
{
    // TODO: Extract logic from Stage notice
}

void ObjectsChanged::Merge(StageNotice&& notice)
{
    ObjectsChanged&& _notice = dynamic_cast<ObjectsChanged&&>(notice);
    // TODO: Write merge logic
}

LayerMutingChanged::LayerMutingChanged(
    const PXR_NS::UsdNotice::LayerMutingChanged& notice)
{
    // TODO: Extract logic from Stage notice
}

void LayerMutingChanged::Merge(StageNotice&& notice)
{
    LayerMutingChanged&& _notice = dynamic_cast<LayerMutingChanged&&>(notice);
    // TODO: Write merge logic
}

} // namespace UsdBrokerNotice

PXR_NAMESPACE_CLOSE_SCOPE
