#include "notice.h"

#include <unf/notice.h>

#include <pxr/base/tf/notice.h>
#include <pxr/pxr.h>

#include <utility>

PXR_NAMESPACE_USING_DIRECTIVE

namespace Test {

TF_REGISTRY_FUNCTION(TfType)
{
    TfType::
        Define<MergeableNotice, TfType::Bases<unf::UnfNotice::StageNotice> >();

    TfType::Define<
        UnMergeableNotice,
        TfType::Bases<unf::UnfNotice::StageNotice> >();

    TfType::Define<InputNotice, TfType::Bases<TfNotice> >();

    TfType::
        Define<OutputNotice1, TfType::Bases<unf::UnfNotice::StageNotice> >();

    TfType::
        Define<OutputNotice2, TfType::Bases<unf::UnfNotice::StageNotice> >();
}

MergeableNotice::MergeableNotice(const DataMap& data) : _data(data) {}

MergeableNotice::MergeableNotice(const MergeableNotice& other)
    : _data(other._data)
{
}

MergeableNotice& MergeableNotice::operator=(const MergeableNotice& other)
{
    MergeableNotice copy(other);
    std::swap(_data, copy._data);
    return *this;
}

void MergeableNotice::Merge(MergeableNotice&& notice)
{
    for (const auto& it : notice.GetData()) {
        _data[it.first] = std::move(it.second);
    }
}

const DataMap& MergeableNotice::GetData() const { return _data; }

bool UnMergeableNotice::IsMergeable() const { return false; }

InputNotice::InputNotice() {}

OutputNotice1::OutputNotice1(const InputNotice&) {}

OutputNotice2::OutputNotice2(const InputNotice&) {}

}  // namespace Test
