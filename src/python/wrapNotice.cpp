#include "../notice.h"

#include "pxr/pxr.h"
#include "pxr/base/tf/notice.h"
#include "pxr/base/tf/pyNoticeWrapper.h"

using namespace boost::python;
using namespace UsdBrokerNotice;

PXR_NAMESPACE_USING_DIRECTIVE

TF_INSTANTIATE_NOTICE_WRAPPER(StageNotice, TfNotice);
TF_INSTANTIATE_NOTICE_WRAPPER(StageContentsChanged, StageNotice);
TF_INSTANTIATE_NOTICE_WRAPPER(ObjectsChanged, StageNotice);
TF_INSTANTIATE_NOTICE_WRAPPER(StageEditTargetChanged, StageNotice);
TF_INSTANTIATE_NOTICE_WRAPPER(LayerMutingChanged, StageNotice);

void wrapNotice()
{    
    TfPyNoticeWrapper<StageNotice, TfNotice>::Wrap();
    TfPyNoticeWrapper<StageContentsChanged, StageNotice>::Wrap();
    TfPyNoticeWrapper<ObjectsChanged, StageNotice>::Wrap();
    TfPyNoticeWrapper<StageEditTargetChanged, StageNotice>::Wrap();
    TfPyNoticeWrapper<LayerMutingChanged, StageNotice>::Wrap();
}
