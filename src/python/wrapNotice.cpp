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
    scope s = class_<PythonUnfNotice>(
        "Notice",
        "Regroup all standalone notices used by the library.",
        no_init);

    TfPyNoticeWrapper<StageNotice, TfNotice>::Wrap()
        .def(
            "IsMergeable",
            &StageNotice::IsMergeable,
            "Indicate whether notice from the same type can be consolidated "
            "during a transaction")

        .def(
            "GetTypeId",
            &StageNotice::GetTypeId,
            "Return unique type identifier");

    TfPyNoticeWrapper<StageContentsChanged, StageNotice>::Wrap();

    TfPyNoticeWrapper<ObjectsChanged, StageNotice>::Wrap()
        .def(
            "AffectedObject",
            &ObjectsChanged::AffectedObject,
            "Indicate whether object was affected by the change that generated "
            "this notice.")

        .def(
            "ResyncedObject",
            &ObjectsChanged::ResyncedObject,
            "Indicate whether object was resynced by the change that generated "
            "this notice.")

        .def(
            "ChangedInfoOnly",
            &ObjectsChanged::ChangedInfoOnly,
            "Indicate whether object was modified but not resynced by the "
            "change that generated this notice.")

        .def(
            "GetResyncedPaths",
            &ObjectsChanged::GetResyncedPaths,
            "Return list of paths that are resynced in lexicographical order.",
            return_value_policy<return_by_value>())

        .def(
            "GetChangedInfoOnlyPaths",
            &ObjectsChanged::GetChangedInfoOnlyPaths,
            "Return list of paths that are modified but not resynced in "
            "lexicographical order.",
            return_value_policy<return_by_value>())

        .def(
            "GetChangedFields",
            (unf::TfTokenSet(ObjectsChanged::*)(const SdfPath&) const)
                & ObjectsChanged::GetChangedFields,
            "Return the list of changed fields in layers that affected the "
            "path",
            return_value_policy<TfPySequenceToList>())

        .def(
            "GetChangedFields",
            (unf::TfTokenSet(ObjectsChanged::*)(const UsdObject&) const)
                & ObjectsChanged::GetChangedFields,
            "Return the list of changed fields in layers that affected the "
            "object",
            return_value_policy<TfPySequenceToList>())

        .def(
            "HasChangedFields",
            (bool (ObjectsChanged::*)(const SdfPath&) const)
                & ObjectsChanged::HasChangedFields,
            "Indicate whether any changed fields affected the path")

        .def(
            "HasChangedFields",
            (bool (ObjectsChanged::*)(const UsdObject&) const)
                & ObjectsChanged::HasChangedFields,
            "Indicate whether any changed fields affected the object");

    TfPyNoticeWrapper<StageEditTargetChanged, StageNotice>::Wrap();

    TfPyNoticeWrapper<LayerMutingChanged, StageNotice>::Wrap()
        .def(
            "GetMutedLayers",
            &LayerMutingChanged::GetMutedLayers,
            "Returns identifiers of the layers that were muted.",
            return_value_policy<return_by_value>())

        .def(
            "GetUnmutedLayers",
            &LayerMutingChanged::GetUnmutedLayers,
            "Returns identifiers of the layers that were unmuted.",
            return_value_policy<return_by_value>());
}
