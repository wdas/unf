#include "notice.h"

#include <unf/notice.h>

#include <pxr/base/tf/notice.h>
#include <pxr/pxr.h>

PXR_NAMESPACE_USING_DIRECTIVE

TF_REGISTRY_FUNCTION(TfType)
{
    TfType::Define<
        ::Test::MergeableNotice,
        TfType::Bases<unf::UnfNotice::StageNotice> >();

    TfType::Define<
        ::Test::UnMergeableNotice,
        TfType::Bases<unf::UnfNotice::StageNotice> >();

    TfType::Define< ::Test::InputNotice, TfType::Bases<TfNotice> >();

    TfType::Define<
        ::Test::OutputNotice1,
        TfType::Bases<unf::UnfNotice::StageNotice> >();

    TfType::Define<
        ::Test::OutputNotice2,
        TfType::Bases<unf::UnfNotice::StageNotice> >();
}
