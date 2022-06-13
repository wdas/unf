#include "../broker.h"
#include "./predicate.h"

#include <pxr/pxr.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/common.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/base/tf/pyPtrHelpers.h>
#include <pxr/base/tf/makePyConstructor.h>
#include <pxr/base/tf/pyFunction.h>

#include <boost/python.hpp>

using namespace boost::python;

PXR_NAMESPACE_USING_DIRECTIVE

//self is the noticebroker that we're wrapping in Python. 
//The obj is a python function that we are passing in.
//We had to expand it out to this form instead of the usual form because of weird boost bindings.
//Afterwards, we can pass the callable Python function into BeginTransaction.
//https://stackoverflow.com/questions/33875004/expose-c-member-function-that-has-stdfunction-as-argument-with-boostpython

void NoticeBroker_BeginTransaction_aux(NoticeBroker& self, boost::python::object obj)
{
  self.BeginTransactionWrap(obj);
}


void NoticeBroker_Process_aux(NoticeBroker& self, UsdBrokerNotice::StageNoticeRefPtr notice)
{
  self.Process(notice);
}


void wrapBroker()
{
    // Ensure that predicate function can be passed from Python.
    TfPyFunctionFromPython<_CaturePredicateFuncRaw>();

    class_<NoticeBroker, NoticeBrokerWeakPtr, boost::noncopyable>
        ("NoticeBroker", no_init)

        .def(TfPyRefAndWeakPtr())

        .def("Create", &NoticeBroker::Create, arg("stage"), 
            return_value_policy<TfPyRefPtrFactory<> >())
        .staticmethod("Create")

        .def("GetStage", &NoticeBroker::GetStage,
            return_value_policy<return_by_value>())

        .def("IsInTransaction", &NoticeBroker::IsInTransaction)

        //For now, the Python clients can go through the Process function
        //We would need to look at some of the stakeholders who use Python-only notice
        //systems to understand what sort of API they really want.
        .def("Process", &NoticeBroker::ProcessWrap, arg("notice"))

        //This definitely passes the wrong arguments -- don't do it like this!!
        //.def("BeginTransaction2", &NoticeBroker::BeginTransaction2, arg("predicate")=object());

        .def("BeginTransaction", &NoticeBroker_BeginTransaction_aux)

        .def("EndTransaction", &NoticeBroker::EndTransaction);
}

TF_REFPTR_CONST_VOLATILE_GET(Broker)
