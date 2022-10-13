#include "./predicate.h"

#include "unf/broker.h"
#include "unf/transaction.h"
#include "unf/capturePredicate.h"

#include <pxr/base/tf/pyFunction.h>
#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>
#include <pxr/usd/usd/stage.h>

#include <boost/python.hpp>
#include <boost/python/return_internal_reference.hpp>

using namespace boost::python;
using namespace unf;

PXR_NAMESPACE_USING_DIRECTIVE

// Expose C++ RAII class as python context manager.
struct PythonNoticeTransaction {
    PythonNoticeTransaction(
        const BrokerWeakPtr& broker, const _CapturePredicateFunc& func)
        : _func(func)
    {
        _makeContext = [=]() {
            return new NoticeTransaction(broker, WrapPredicate(_func));
        };
    }

    PythonNoticeTransaction(
        const BrokerWeakPtr& broker, CapturePredicate predicate)
        : _predicate(predicate)
    {
        _makeContext = [=]() {
            return new NoticeTransaction(broker, _predicate);
        };
    }

    PythonNoticeTransaction(
        const UsdStageWeakPtr& stage, const _CapturePredicateFunc& func)
        : _func(func)
    {
        _makeContext = [=]() {
            return new NoticeTransaction(stage, WrapPredicate(_func));
        };
    }

    PythonNoticeTransaction(
        const UsdStageWeakPtr& stage, CapturePredicate predicate)
        : _predicate(predicate)
    {
        _makeContext = [=]() {
            return new NoticeTransaction(stage, _predicate);
        };
    }

    // Instantiate the C++ class object and hold it by shared_ptr.
    PythonNoticeTransaction const* __enter__()
    {
        _context.reset(_makeContext());
        return this;
    }

    // Drop the shared_ptr.
    void __exit__(object, object, object) { _context.reset(); }

    BrokerPtr GetBroker() { return _context->GetBroker(); }

  private:
    std::shared_ptr<NoticeTransaction> _context;
    std::function<NoticeTransaction*()> _makeContext;

    _CapturePredicateFunc _func = nullptr;
    CapturePredicate _predicate = CapturePredicate::Default();
};

void wrapTransaction()
{
    // Ensure that predicate function can be passed from Python.
    TfPyFunctionFromPython<_CapturePredicateFuncRaw>();

    class_<PythonNoticeTransaction>("NoticeTransaction", no_init)

        .def(init<const BrokerWeakPtr&, CapturePredicate>(
            (arg("broker"), arg("predicate") = CapturePredicate::Default())))

        .def(init<const BrokerWeakPtr&, const _CapturePredicateFunc&>(
            (arg("broker"), arg("predicate"))))

        .def(init<const UsdStageWeakPtr&, CapturePredicate>(
            (arg("stage"), arg("predicate") = CapturePredicate::Default())))

        .def(init<const UsdStageWeakPtr&, const _CapturePredicateFunc&>(
            (arg("stage"), arg("predicate"))))

        .def(
            "__enter__",
            &PythonNoticeTransaction::__enter__,
            return_internal_reference<>())

        .def("__exit__", &PythonNoticeTransaction::__exit__)

        .def(
            "GetBroker",
            &PythonNoticeTransaction::GetBroker,
            return_value_policy<return_by_value>());
}
