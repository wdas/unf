#include "./predicate.h"
#include "./noticeWrapper.h"

#include "broker.h"

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

void NoticeBroker_BeginTransaction(NoticeBroker& self, object obj)
{
    self.BeginTransaction(WrapPredicate(obj));
}

void NoticeBroker_Process(NoticeBroker& self, TfRefPtr<NoticeWrapper> notice)
{
    self.Process(notice->Get());
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

        .def("Process", &NoticeBroker_Process)

        .def("BeginTransaction", &NoticeBroker_BeginTransaction,
            ((arg("self"), arg("predicate")=object())))

        .def("EndTransaction", &NoticeBroker::EndTransaction);
}

TF_REFPTR_CONST_VOLATILE_GET(Broker)
