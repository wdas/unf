#ifndef USD_NOTICE_FRAMEWORK_PYTHON_PREDICATE_H
#define USD_NOTICE_FRAMEWORK_PYTHON_PREDICATE_H

#include "unf/broker.h"
#include "unf/capturePredicate.h"

#include <pxr/base/tf/pyLock.h>
#include <pxr/base/tf/pyNoticeWrapper.h>
#include <pxr/pxr.h>

#ifndef PXR_USE_INTERNAL_BOOST_PYTHON
#include <boost/python.hpp>
using namespace boost::python;
#else
#include <pxr/external/boost/python.hpp>
using namespace PXR_BOOST_PYTHON_NAMESPACE;
#endif

#include <functional>

using namespace unf;

PXR_NAMESPACE_USING_DIRECTIVE

using _CapturePredicateFuncRaw = bool(object const&);
using _CapturePredicateFunc = std::function<_CapturePredicateFuncRaw>;

static CapturePredicateFunc WrapPredicate(_CapturePredicateFunc fn)
{
    // Capture by-copy to prevent boost object from being destroyed.
    return [=](const UnfNotice::StageNotice& notice) {
        TfPyLock lock;

        if (!fn) return true;

        // Creates a Python version of the notice by inspecting its type and
        // converting the generic StageNotice to the real notice inside.
        object _notice = Tf_PyNoticeObjectGenerator::Invoke(notice);
        return fn(_notice);
    };
}

#endif  // USD_NOTICE_FRAMEWORK_PYTHON_PREDICATE_H
