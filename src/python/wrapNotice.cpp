#include "../notice.h"
#include "../noticeWrapper.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/notice.h>
#include <pxr/base/tf/pyNoticeWrapper.h>
#include <pxr/base/tf/pyPtrHelpers.h>
#include <pxr/base/tf/makePyConstructor.h>
#include <pxr/base/tf/pyResultConversions.h>
#include <pxr/usd/usd/pyConversions.h>
#include <pxr/base/tf/pyContainerConversions.h>

using namespace boost::python;
using namespace PXR_NS::UsdBrokerNotice;

PXR_NAMESPACE_USING_DIRECTIVE

TF_INSTANTIATE_NOTICE_WRAPPER(StageNotice, TfNotice);
TF_INSTANTIATE_NOTICE_WRAPPER(StageContentsChanged, StageNotice);
TF_INSTANTIATE_NOTICE_WRAPPER(ObjectsChanged, StageNotice);
TF_INSTANTIATE_NOTICE_WRAPPER(StageEditTargetChanged, StageNotice);
TF_INSTANTIATE_NOTICE_WRAPPER(LayerMutingChanged, StageNotice);

//TODO: REMOVE -- Just for testing purposes
TF_INSTANTIATE_NOTICE_WRAPPER(TestNotice, StageNotice);

void wrapNotice()
{    
    TfPyNoticeWrapper<StageNotice, TfNotice>::Wrap();
    TfPyNoticeWrapper<StageContentsChanged, StageNotice>::Wrap();
    TfPyNoticeWrapper<ObjectsChanged, StageNotice>::Wrap().def("GetResyncedPaths", &ObjectsChanged::GetResyncedPaths, return_value_policy<return_by_value>());
    TfPyNoticeWrapper<StageEditTargetChanged, StageNotice>::Wrap();
    TfPyNoticeWrapper<LayerMutingChanged, StageNotice>::Wrap();

    //TODO: REMOVE
    TfPyNoticeWrapper<TestNotice, StageNotice>::Wrap().def("GetCount", &TestNotice::GetCount);
}
