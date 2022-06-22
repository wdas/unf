#ifndef NOTICE_BROKER_PYTHON_PREDICATE_H
#define NOTICE_BROKER_PYTHON_PREDICATE_H

#include "broker.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/pyNoticeWrapper.h>
#include <pxr/base/tf/pyLock.h>

#include <boost/python.hpp>

#include <functional>

using namespace boost::python;

PXR_NAMESPACE_USING_DIRECTIVE

using _CaturePredicateFuncRaw = bool (object const &);
using _CaturePredicateFunc = std::function<_CaturePredicateFuncRaw>;

static NoticeCaturePredicateFunc WrapPredicate(_CaturePredicateFunc fn)
{
    // Capture by-copy to prevent boost object from being destroyed.
    return [=](const UsdBrokerNotice::StageNotice& notice) { 
        TfPyLock lock;

        if (!fn)
            return true;

        // Creates a Python version of the notice by inspecting its type and
        // converting the generic StageNotice to the real notice inside.
        object _notice = Tf_PyNoticeObjectGenerator::Invoke(notice);
        return fn(_notice);
    };
}

#endif // NOTICE_BROKER_PYTHON_PREDICATE_H
