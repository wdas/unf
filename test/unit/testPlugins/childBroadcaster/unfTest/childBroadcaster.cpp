#include "childBroadcaster.h"

#include <unf/broadcaster.h>

#include <pxr/pxr.h>

PXR_NAMESPACE_USING_DIRECTIVE

TF_REGISTRY_FUNCTION(TfType)
{
    unf::BroadcasterDefine<::Test::ChildBroadcaster, unf::Broadcaster>();
}
