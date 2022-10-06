#include "unf/broker.h"
#include "unf/blocker.h"

#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>
#include <pxr/usd/usd/stage.h>

#include <boost/python.hpp>
#include <boost/python/return_internal_reference.hpp>

using namespace boost::python;
using namespace unf;

PXR_NAMESPACE_USING_DIRECTIVE

// Expose C++ RAII class as python context manager.
struct PythonNoticeBlocker {
    PythonNoticeBlocker(const BrokerWeakPtr& broker)
    {
        _makeContext = [&]() { return new NoticeBlocker(broker); };
    }

    PythonNoticeBlocker(const UsdStageWeakPtr& stage)
    {
        _makeContext = [&]() { return new NoticeBlocker(stage); };
    }

    // Instantiate the C++ class object and hold it by shared_ptr.
    PythonNoticeBlocker const* __enter__()
    {
        _context.reset(_makeContext());
        return this;
    }

    // Drop the shared_ptr.
    void __exit__(object, object, object) { _context.reset(); }

    BrokerPtr GetBroker() { return _context->GetBroker(); }

  private:
    std::shared_ptr<NoticeBlocker> _context;
    std::function<NoticeBlocker*()> _makeContext;
};

void wrapBlocker()
{
    class_<PythonNoticeBlocker>("NoticeBlocker", no_init)

        .def(init<const BrokerWeakPtr&>((arg("broker"))))

        .def(init<const UsdStageWeakPtr&>((arg("stage"))))

        .def(
            "__enter__",
            &PythonNoticeBlocker::__enter__,
            return_internal_reference<>())

        .def("__exit__", &PythonNoticeBlocker::__exit__)

        .def(
            "GetBroker",
            &PythonNoticeBlocker::GetBroker,
            return_value_policy<return_by_value>());
}
