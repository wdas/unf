#ifndef NOTICE_BROKER_PYTHON_PREDICATE_H
#define NOTICE_BROKER_PYTHON_PREDICATE_H

#include <pxr/pxr.h>
#include <pxr/base/tf/pyNoticeWrapper.h>
#include <pxr/base/tf/pyLock.h>

#include <boost/python.hpp>

#include <functional>

using namespace boost::python;

PXR_NAMESPACE_USING_DIRECTIVE

using _CaturePredicateFuncRaw = bool (object const &);
using _CaturePredicateFunc = std::function<_CaturePredicateFuncRaw>;

//When passing by reference, boost doesn't know how long to keep the object around because it's only giving us a reference.
//And boost is the one that's holding onto the obj -- not us. Boost is probably creating the wrapper for the function and then
//Giving it to us, and afterwards destorying it -- otherwise it would be a memory leak to keep it around forever.
//By changing it to pass-by-value, we make a copy of the wrapper that boost made, so that we can hold onto it and use it later.
//Any reference had to be changed to be a copy along this chain of functions.
static NoticeCaturePredicateFunc WrapPredicate(_CaturePredicateFunc fn)
{
    return [=](const UsdBrokerNotice::StageNotice& notice) { 
        TfPyLock lock;

        if (!fn)
            return true;

        // Creates a Python version of the notice by inspecting its type and converting the generic StageNotice to the
        // real notice inside.
        object _notice = Tf_PyNoticeObjectGenerator::Invoke(notice);
        return fn(_notice);
    };
}

#endif // NOTICE_BROKER_PYTHON_PREDICATE_H
