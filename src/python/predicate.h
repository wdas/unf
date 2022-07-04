#ifndef NOTICE_BROKER_PYTHON_PREDICATE_H
#define NOTICE_BROKER_PYTHON_PREDICATE_H

#include "broker.h"

#include "pxr/pxr.h"
#include "pxr/base/tf/pyNoticeWrapper.h"
#include "pxr/base/tf/pyLock.h"

#include "boost/python.hpp"

#include <functional>

using namespace boost::python;

PXR_NAMESPACE_USING_DIRECTIVE

using _CaturePredicateFuncRaw = bool (object const &);
using _CaturePredicateFunc = std::function<_CaturePredicateFuncRaw>;

static NoticeCaturePredicateFunc WrapPredicate(const _CaturePredicateFunc &fn)
{
    return [&](const UsdBrokerNotice::StageNotice& notice) {
        TfPyLock lock;

        if (!fn)
            return true;

        // TODO:: Not sure whether this works with Python notices.
        object _notice = Tf_PyNoticeObjectGenerator::Invoke(notice);
        return fn(_notice);
    };
}

#endif // NOTICE_BROKER_PYTHON_PREDICATE_H
