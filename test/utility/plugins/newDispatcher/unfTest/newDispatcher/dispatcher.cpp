#include "dispatcher.h"

#include <unf/dispatcher.h>

#include <pxr/pxr.h>

PXR_NAMESPACE_USING_DIRECTIVE

TF_REGISTRY_FUNCTION(TfType)
{
    unf::DispatcherDefine<::Test::NewDispatcher, unf::Dispatcher>();
}
