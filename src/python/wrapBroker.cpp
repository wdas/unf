#include "./predicate.h"

#include "unf/broker.h"
#include "unf/capturePredicate.h"

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

void wrapBroker()
{
    // Ensure that predicate function can be passed from Python.
    TfPyFunctionFromPython<_CapturePredicateFuncRaw>();

    class_<Broker, BrokerWeakPtr, boost::noncopyable>(
        "Broker",
        "Intermediate object between the Usd Stage and any clients that needs "
        "asynchronous handling and upstream filtering of notices.",
        no_init)

        .def(TfPyRefAndWeakPtr())

        .def(
            "Create",
            &Broker::Create,
            arg("stage"),
            "Create a broker from a Usd Stage.",
            return_value_policy<TfPyRefPtrFactory<> >())
        .staticmethod("Create")

        .def(
            "GetStage",
            &Broker::GetStage,
            "Return Usd Stage associated with the broker.",
            return_value_policy<return_by_value>())

        .def(
            "IsInTransaction",
            &Broker::IsInTransaction,
            "Indicate whether a notice transaction has been started.")

        .def(
            "BeginTransaction",
            (void (Broker::*)(CapturePredicate)) & Broker::BeginTransaction,
            (arg("predicate") = CapturePredicate::Default()),
            "Start a notice transaction.")

        .def(
            "BeginTransaction",
            &Broker_BeginTransaction_WithFunc,
            ((arg("self"), arg("predicate"))),
            "Start a notice transaction with a function predicate.")

        .def(
            "EndTransaction",
            &Broker::EndTransaction,
            "Stop a notice transaction.");
}

TF_REFPTR_CONST_VOLATILE_GET(Broker)
