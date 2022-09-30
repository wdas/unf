#include "notice.h"

#include <unf/notice.h>

#include <pxr/base/tf/notice.h>
#include <pxr/pxr.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace unf {

namespace BroadcasterNotice {

TF_REGISTRY_FUNCTION(TfType)
{
    TfType::Define<
        HierarchyChanged,
        TfType::Bases<unf::BrokerNotice::StageNotice>>();
}

void HierarchyChanged::Merge(HierarchyChanged&& notice)
{
    //TODO: Write logic for HierarchyChanged merge
    return;
}

}  // namespace BroadcasterNotice

}  // namespace unf
