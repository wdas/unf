#include "testNotice.h"

#include <UsdNoticeBroker/notice.h>

#include <pxr/pxr.h>
#include <pxr/base/tf/notice.h>

PXR_NAMESPACE_OPEN_SCOPE

TF_REGISTRY_FUNCTION(TfType)
{
    TfType::Define<
        ::Test::MergeableNotice,
        TfType::Bases<UNB::BrokerNotice::StageNotice> >();

    TfType::Define<
        ::Test::UnMergeableNotice,
        TfType::Bases<UNB::BrokerNotice::StageNotice> >();

    TfType::Define<
        ::Test::InputNotice,
        TfType::Bases<TfNotice> >();

    TfType::Define<
        ::Test::OutputNotice1,
        TfType::Bases<UNB::BrokerNotice::StageNotice> >();

    TfType::Define<
        ::Test::OutputNotice2,
        TfType::Bases<UNB::BrokerNotice::StageNotice> >();
}

PXR_NAMESPACE_CLOSE_SCOPE
