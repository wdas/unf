#include "pyNoticeWrapper.h"

using namespace boost::python;
using namespace PXR_NS::UsdBrokerNotice;

PXR_NAMESPACE_USING_DIRECTIVE

void wrapNoticeWrapper(){
    class_<PyBrokerNoticeWrapperBase,
        TfWeakPtr<PyBrokerNoticeWrapperBase>,
        boost::noncopyable>
        ("PyBrokerNoticeWrapperBase", init<>())

        .def(TfPyRefAndWeakPtr())

        .def("Get", &PyBrokerNoticeWrapperBase::GetWrap)

        .def("Send", &PyBrokerNoticeWrapperBase::Send);
}
