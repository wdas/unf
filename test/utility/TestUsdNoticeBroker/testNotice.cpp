#include "testNotice.h"

#include "notice.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/notice.h>

PXR_NAMESPACE_OPEN_SCOPE

TF_REGISTRY_FUNCTION(TfType)
{
    TfType::Define<
        ::Test::MergeableNotice,
        TfType::Bases<UsdBrokerNotice::StageNotice> >();

    TfType::Define<
        ::Test::UnMergeableNotice,
        TfType::Bases<UsdBrokerNotice::StageNotice> >();

    TfType::Define<
        ::Test::InputNotice,
        TfType::Bases<PXR_NS::TfNotice> >();

    TfType::Define<
        ::Test::OutputNotice1,
        TfType::Bases<UsdBrokerNotice::StageNotice> >();

    TfType::Define<
        ::Test::OutputNotice2,
        TfType::Bases<UsdBrokerNotice::StageNotice> >();
}

PXR_NAMESPACE_CLOSE_SCOPE
