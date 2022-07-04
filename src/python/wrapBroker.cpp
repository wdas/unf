#include "broker.h"

#include "pxr/pxr.h"
#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usd/common.h"
#include "pxr/base/tf/weakPtr.h"
#include "pxr/base/tf/pyPtrHelpers.h"
#include "pxr/base/tf/makePyConstructor.h"
#include "pxr/base/tf/pyFunction.h"

#include "boost/python.hpp"

using namespace boost::python;

PXR_NAMESPACE_USING_DIRECTIVE

void wrapBroker()
{
    class_<NoticeBroker, NoticeBrokerWeakPtr, boost::noncopyable>
        ("NoticeBroker", no_init)

        .def(TfPyRefAndWeakPtr())

        .def("Create", &NoticeBroker::Create, arg("stage"),
            return_value_policy<TfPyRefPtrFactory<> >())
        .staticmethod("Create")

        .def("GetStage", &NoticeBroker::GetStage,
            return_value_policy<return_by_value>())

        .def("IsInTransaction", &NoticeBroker::IsInTransaction)
        ;

        // TODO: Add bindings for BeginTransaction and EndTransaction
        // TODO: Add binding for Send
}

TF_REFPTR_CONST_VOLATILE_GET(Broker)
