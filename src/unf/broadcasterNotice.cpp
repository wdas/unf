#include "broadcasterNotice.h"
#include "notice.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/notice.h>

PXR_NAMESPACE_OPEN_SCOPE
namespace unf {
namespace BroadcasterNotice {

TF_REGISTRY_FUNCTION(TfType)
{
    TfType::Define<
        ChangeSummaryNotice,
        TfType::Bases<unf::BrokerNotice::StageNotice>>();
}

virtual void ChangeSummaryNotice::Merge(ChangeSummaryNotice&& notice) override {
    for(const auto& a: notice._added) {
        _added.insert(a);
    }
    for(const auto& r: notice._removed) {
        _removed.insert(r);
    }
    for(const auto& m: notice._modified) {
        _modified.insert(m);
    }
}
} // namespace BroadcasterNotice
} // namespace unf

PXR_NAMESPACE_CLOSE_SCOPE
