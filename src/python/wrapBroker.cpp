#include "./predicate.h"

#include "unf/broker.h"
#include "unf/capturePredicate.h"
#include "unf/pyNoticeWrapper.h"

#include <pxr/base/tf/makePyConstructor.h>
#include <pxr/base/tf/pyFunction.h>
#include <pxr/base/tf/pyPtrHelpers.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>
#include <pxr/usd/usd/stage.h>

#include <boost/python.hpp>

using namespace boost::python;
using namespace unf;

PXR_NAMESPACE_USING_DIRECTIVE

void Broker_BeginTransaction_WithFunc(Broker& self, object predicate)
{
    auto _predicate = WrapPredicate(predicate);
    self.BeginTransaction(_predicate);
}

void Broker_Send(Broker& self, TfRefPtr<PyBrokerNoticeWrapperBase> notice)
{
    self.Send(notice->Get());
}

void wrapBroker()
{
    // Ensure that predicate function can be passed from Python.
    TfPyFunctionFromPython<_CapturePredicateFuncRaw>();

    class_<Broker, BrokerWeakPtr, boost::noncopyable>("Broker", no_init)

        .def(TfPyRefAndWeakPtr())

        .def(
            "Create",
            &Broker::Create,
            arg("stage"),
            return_value_policy<TfPyRefPtrFactory<> >())
        .staticmethod("Create")

        .def(
            "GetStage",
            &Broker::GetStage,
            return_value_policy<return_by_value>())

        .def("IsInTransaction", &Broker::IsInTransaction)

        .def("Send", &Broker_Send)

        .def(
            "BeginTransaction",
            (void(Broker::*)(CapturePredicate))
            &Broker::BeginTransaction,
            (arg("predicate") = CapturePredicate::Default()))

        .def(
            "BeginTransaction",
            &Broker_BeginTransaction_WithFunc,
            ((arg("self"), arg("predicate"))))

        .def("EndTransaction", &Broker::EndTransaction);
}

TF_REFPTR_CONST_VOLATILE_GET(Broker)
