#include "./predicate.h"

#include "transaction.h"

#include <pxr/pxr.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/common.h>
#include <pxr/base/tf/pyFunction.h>

#include <boost/python.hpp>

using namespace boost::python;

PXR_NAMESPACE_USING_DIRECTIVE

// Expose C++ RAII class as python context manager.
struct PythonNoticeTransaction
{
    PythonNoticeTransaction(
        const NoticeBrokerWeakPtr& broker,
        const _CaturePredicateFunc &predicate)
        : _predicate(predicate)
    {
        _makeContext = [&]() {
            return new NoticeTransaction(broker, WrapPredicate(_predicate));
        };
    }

    PythonNoticeTransaction(
        const UsdStageWeakPtr& stage,
        const _CaturePredicateFunc &predicate)
        : _predicate(predicate)
    {
        _makeContext = [&]() {
            return new NoticeTransaction(stage, WrapPredicate(_predicate));
        };
    }

    // Instantiate the C++ class object and hold it by shared_ptr.
    void __enter__() { _context.reset(_makeContext()); }

    // Drop the shared_ptr.
    void __exit__(object, object, object) { _context.reset(); }

private:
    std::shared_ptr<NoticeTransaction> _context;
    std::function<NoticeTransaction *()> _makeContext;

    _CaturePredicateFunc _predicate;
};

void
wrapTransaction()
{
    // Ensure that predicate function can be passed from Python.
    TfPyFunctionFromPython<_CaturePredicateFuncRaw>();

    class_<PythonNoticeTransaction>("NoticeTransaction", no_init)
        .def(init<const NoticeBrokerWeakPtr&, const _CaturePredicateFunc&>
             ((arg("broker"), arg("predicate")=object())))
        .def(init<const UsdStageWeakPtr&, const _CaturePredicateFunc&>
             ((arg("stage"), arg("predicate")=object())))
        .def("__enter__", &PythonNoticeTransaction::__enter__)
        .def("__exit__", &PythonNoticeTransaction::__exit__)
        ;
}
