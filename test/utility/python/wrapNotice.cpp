#include <UsdNoticeBroker/pyNoticeWrapper.h>

#include <TestUsdNoticeBroker/testNotice.h>

#include <pxr/pxr.h>
#include <pxr/base/tf/notice.h>
#include <pxr/base/tf/pyNoticeWrapper.h>
#include <pxr/base/tf/pyPtrHelpers.h>

#include <boost/python.hpp>

using namespace boost::python;
using namespace PXR_NS::UNB::BrokerNotice;
using namespace PXR_NS::UNB;

PXR_NAMESPACE_USING_DIRECTIVE

TF_INSTANTIATE_NOTICE_WRAPPER(::Test::MergeableNotice, StageNotice);
TF_INSTANTIATE_NOTICE_WRAPPER(::Test::UnMergeableNotice, StageNotice);

// Dummy class to reproduce namespace in Python.
class PythonTestNotice {};

void wrapNotice()
{
    scope s = class_<PythonTestNotice>("TestNotice", no_init);

    TfPyNoticeWrapper<::Test::MergeableNotice, StageNotice>::Wrap()
        .def("GetData", &::Test::MergeableNotice::GetData,
            return_value_policy<return_by_value>());

    TfPyNoticeWrapper<::Test::UnMergeableNotice, StageNotice>::Wrap();

    // Create double layer wrappers to handle notice emission via Python.
    PyBrokerNoticeWrapper<::Test::UnMergeableNotice>::Wrap
        ("UnMergeableNoticeWrapper");

    // PyBrokerNoticeWrapper<::Test::MergeableNotice>::Wrap
    //     <dict>("MergeableNoticeWrapper");
}
