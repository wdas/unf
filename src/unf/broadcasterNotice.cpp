#include "broadcasterNotice.h"
#include "notice.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/notice.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace unf {
namespace BroadcasterNotice {

TF_REGISTRY_FUNCTION(TfType)
{
    TfType::Define<
        HierarchyChanged,
        TfType::Bases<unf::BrokerNotice::StageNotice>>();
}

void HierarchyChanged::Merge(HierarchyChanged&& notice) {
    for (auto& a: notice._added) {
        _added.push_back(std::move(a));
    }
    for (auto& r: notice._removed) {
        _removed.push_back(std::move(r));
    }
    for (auto& m: notice._modified) {
        _modified.push_back(std::move(m));
    }
    for (const auto& changedField : notice._changedFields) {
        _changedFields[changedField.first].insert(changedField.second.begin(), changedField.second.end());
    }
    SdfPath::RemoveDescendentPaths(&_added);
    SdfPath::RemoveDescendentPaths(&_removed);
    SdfPath::RemoveDescendentPaths(&_modified);
}
} // namespace BroadcasterNotice
} // namespace unf

