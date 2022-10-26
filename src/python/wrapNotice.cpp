#include "unf/notice.h"

#include <pxr/base/tf/notice.h>
#include <pxr/base/tf/pyContainerConversions.h>
#include <pxr/base/tf/pyNoticeWrapper.h>
#include <pxr/base/tf/pyResultConversions.h>

#include <pxr/pxr.h>

#include <boost/python.hpp>

using namespace boost::python;
using namespace unf::UnfNotice;

PXR_NAMESPACE_USING_DIRECTIVE

TF_INSTANTIATE_NOTICE_WRAPPER(StageNotice, TfNotice);
TF_INSTANTIATE_NOTICE_WRAPPER(StageContentsChanged, StageNotice);
TF_INSTANTIATE_NOTICE_WRAPPER(ObjectsChanged, StageNotice);
TF_INSTANTIATE_NOTICE_WRAPPER(StageEditTargetChanged, StageNotice);
TF_INSTANTIATE_NOTICE_WRAPPER(LayerMutingChanged, StageNotice);

// Dummy class to reproduce namespace in Python.
class PythonUnfNotice {
};

void wrapNotice()
{
    scope s = class_<PythonUnfNotice>("Notice", no_init);

    TfPyNoticeWrapper<StageNotice, TfNotice>::Wrap()
        .def("IsMergeable", &StageNotice::IsMergeable)
        .def("GetTypeId", &StageNotice::GetTypeId);

    TfPyNoticeWrapper<StageContentsChanged, StageNotice>::Wrap();

    TfPyNoticeWrapper<ObjectsChanged, StageNotice>::Wrap()
        .def("AffectedObject", &ObjectsChanged::AffectedObject)
        .def("ResyncedObject", &ObjectsChanged::ResyncedObject)
        .def("ChangedInfoOnly", &ObjectsChanged::ChangedInfoOnly)
        .def(
            "GetResyncedPaths",
            &ObjectsChanged::GetResyncedPaths,
            return_value_policy<return_by_value>())
        .def(
            "GetChangedInfoOnlyPaths",
            &ObjectsChanged::GetChangedInfoOnlyPaths,
            return_value_policy<return_by_value>())
        .def(
            "GetChangedFields",
            (unf::TfTokenSet(ObjectsChanged::*)(const SdfPath&) const)
                & ObjectsChanged::GetChangedFields,
            return_value_policy<TfPySequenceToList>())
        .def(
            "GetChangedFields",
            (unf::TfTokenSet(ObjectsChanged::*)(const UsdObject&) const)
                & ObjectsChanged::GetChangedFields,
            return_value_policy<TfPySequenceToList>())
        .def(
            "HasChangedFields",
            (bool (ObjectsChanged::*)(const SdfPath&) const)
                & ObjectsChanged::HasChangedFields)
        .def(
            "HasChangedFields",
            (bool (ObjectsChanged::*)(const UsdObject&) const)
                & ObjectsChanged::HasChangedFields);

    TfPyNoticeWrapper<StageEditTargetChanged, StageNotice>::Wrap();

    TfPyNoticeWrapper<LayerMutingChanged, StageNotice>::Wrap()
        .def(
            "GetMutedLayers",
            &LayerMutingChanged::GetMutedLayers,
            return_value_policy<return_by_value>())
        .def(
            "GetUnmutedLayers",
            &LayerMutingChanged::GetUnmutedLayers,
            return_value_policy<return_by_value>());
}
