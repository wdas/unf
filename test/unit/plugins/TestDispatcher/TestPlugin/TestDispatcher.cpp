#include "TestDispatcher.h"

#include <pxr/pxr.h>

PXR_NAMESPACE_OPEN_SCOPE

TF_REGISTRY_FUNCTION(TfType)
{
    DispatcherDefine<::Test::TestDispatcher, Dispatcher>();
}

PXR_NAMESPACE_CLOSE_SCOPE
