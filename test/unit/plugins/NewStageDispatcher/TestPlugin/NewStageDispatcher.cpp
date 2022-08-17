#include "NewStageDispatcher.h"

#include <UsdNoticeBroker/dispatcher.h>

#include <pxr/pxr.h>

PXR_NAMESPACE_OPEN_SCOPE

TF_REGISTRY_FUNCTION(TfType)
{
    UNB::DispatcherDefine<Test::NewStageDispatcher, UNB::Dispatcher>();
}

PXR_NAMESPACE_CLOSE_SCOPE
