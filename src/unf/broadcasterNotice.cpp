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
        ChangeSummary,
        TfType::Bases<unf::BrokerNotice::StageNotice>>();
}

void ChangeSummary::Merge(ChangeSummary&& notice) {
    for (const auto& a: notice._added) {
        _added.insert(a);
    }
    for (const auto& r: notice._removed) {
        _removed.insert(r);
    }
    for (const auto& m: notice._modified) {
        _modified.insert(m);
    }
    for (const auto& changedField : notice._changedFields) {
        _changedFields[changedField.first].insert(changedField.second.begin(), changedField.second.end());
    }
}
} // namespace BroadcasterNotice
} // namespace unf

PXR_NAMESPACE_CLOSE_SCOPE
