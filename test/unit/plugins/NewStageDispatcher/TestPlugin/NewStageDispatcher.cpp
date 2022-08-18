#include "NewStageDispatcher.h"

#include <unf/dispatcher.h>

#include <pxr/pxr.h>

PXR_NAMESPACE_OPEN_SCOPE

TF_REGISTRY_FUNCTION(TfType)
{
    unf::DispatcherDefine<Test::NewStageDispatcher, unf::Dispatcher>();
}

PXR_NAMESPACE_CLOSE_SCOPE
