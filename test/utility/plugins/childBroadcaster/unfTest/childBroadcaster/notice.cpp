#include "notice.h"

#include <pxr/base/tf/notice.h>
#include <pxr/pxr.h>

PXR_NAMESPACE_USING_DIRECTIVE

TF_REGISTRY_FUNCTION(TfType)
{
    TfType::Define< ::Test::ChildBroadcasterNotice, TfType::Bases<TfNotice> >();
}
