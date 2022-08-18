#include "TestDispatcher.h"

#include <unf/dispatcher.h>

#include <pxr/pxr.h>

PXR_NAMESPACE_OPEN_SCOPE

TF_REGISTRY_FUNCTION(TfType)
{
    unf::DispatcherDefine<::Test::TestDispatcher, unf::Dispatcher>();
}

PXR_NAMESPACE_CLOSE_SCOPE
