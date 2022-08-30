#include "unf/notice.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/notice.h>
#include <pxr/base/tf/pyNoticeWrapper.h>

#include <boost/python.hpp>

using namespace boost::python;
using namespace unf::BrokerNotice;

PXR_NAMESPACE_USING_DIRECTIVE

TF_INSTANTIATE_NOTICE_WRAPPER(StageNotice, TfNotice);
TF_INSTANTIATE_NOTICE_WRAPPER(StageContentsChanged, StageNotice);
TF_INSTANTIATE_NOTICE_WRAPPER(ObjectsChanged, StageNotice);
TF_INSTANTIATE_NOTICE_WRAPPER(StageEditTargetChanged, StageNotice);
TF_INSTANTIATE_NOTICE_WRAPPER(LayerMutingChanged, StageNotice);

// Dummy class to reproduce namespace in Python.
class PythonBrokerNotice {};

void wrapNotice()
{
    scope s = class_<PythonBrokerNotice>("BrokerNotice", no_init);

    TfPyNoticeWrapper<StageNotice, TfNotice>::Wrap();

    TfPyNoticeWrapper<StageContentsChanged, StageNotice>::Wrap();

    TfPyNoticeWrapper<ObjectsChanged, StageNotice>::Wrap()
        .def("GetResyncedPaths", &ObjectsChanged::GetResyncedPaths,
            return_value_policy<return_by_value>())
        .def("GetChangedInfoOnlyPaths", &ObjectsChanged::GetChangedInfoOnlyPaths,
            return_value_policy<return_by_value>());

    TfPyNoticeWrapper<StageEditTargetChanged, StageNotice>::Wrap();

    TfPyNoticeWrapper<LayerMutingChanged, StageNotice>::Wrap()
        .def("GetMutedLayers", &LayerMutingChanged::GetMutedLayers,
            return_value_policy<return_by_value>())
        .def("GetUnmutedLayers", &LayerMutingChanged::GetUnmutedLayers,
            return_value_policy<return_by_value>());
}
