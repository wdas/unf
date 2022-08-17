#include "./predicate.h"

#include "UsdNoticeBroker/broker.h"
#include "UsdNoticeBroker/pyNoticeWrapper.h"

#include <pxr/pxr.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/common.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/base/tf/pyPtrHelpers.h>
#include <pxr/base/tf/makePyConstructor.h>
#include <pxr/base/tf/pyFunction.h>

#include <boost/python.hpp>

using namespace boost::python;
using namespace UNB;

PXR_NAMESPACE_USING_DIRECTIVE

void Broker_BeginTransaction(Broker& self, object predicate)
{
    NoticeCaturePredicateFunc _predicate = nullptr;
    if (predicate) {
        _predicate = WrapPredicate(predicate);
    }

    self.BeginTransaction(_predicate);
}

void Broker_Send(Broker& self, TfRefPtr<PyBrokerNoticeWrapperBase> notice)
{
    self.Send(notice->Get());
}

void wrapBroker()
{
    // Ensure that predicate function can be passed from Python.
    TfPyFunctionFromPython<_CaturePredicateFuncRaw>();

    class_<Broker, BrokerWeakPtr, boost::noncopyable>
        ("Broker", no_init)

        .def(TfPyRefAndWeakPtr())

        .def("Create", &Broker::Create, arg("stage"),
            return_value_policy<TfPyRefPtrFactory<> >())
        .staticmethod("Create")

        .def("GetStage", &Broker::GetStage,
            return_value_policy<return_by_value>())

        .def("IsInTransaction", &Broker::IsInTransaction)

        .def("Send", &Broker_Send)

        .def("BeginTransaction", &Broker_BeginTransaction,
            ((arg("self"), arg("predicate")=object())))

        .def("EndTransaction", &Broker::EndTransaction);
}

TF_REFPTR_CONST_VOLATILE_GET(Broker)
