#ifndef NOTICE_BROKER_NOTICE_WRAPPER_H
#define NOTICE_BROKER_NOTICE_WRAPPER_H

#include "UsdNoticeBroker/notice.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/pyNoticeWrapper.h>
#include <pxr/base/tf/pyPtrHelpers.h>
#include <pxr/base/tf/makePyConstructor.h>
#include <pxr/base/tf/pyResultConversions.h>
#include <pxr/base/tf/pyContainerConversions.h>
#include <pxr/base/tf/pyNoticeWrapper.h>
#include <pxr/base/tf/pyLock.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/type.h>
#include <pxr/base/tf/notice.h>
#include <pxr/usd/usd/pyConversions.h>
#include <pxr/usd/usd/notice.h>

using namespace boost::python;

PXR_NAMESPACE_OPEN_SCOPE

namespace UNB {

// Interface object to process custom notice in Python.
// TODO: Should we forbid handling of custom notices via Python?
class PyBrokerNoticeWrapperBase : public TfRefBase, public TfWeakBase {
public:
    PyBrokerNoticeWrapperBase() {};

    virtual TfRefPtr<BrokerNotice::StageNotice> Get() { return nullptr; }

    virtual object GetWrap() { return object(); }

    virtual void Send() {}
};

template <class Self>
class PyBrokerNoticeWrapper : public PyBrokerNoticeWrapperBase
{
public:
    PyBrokerNoticeWrapper() {}

    // Creates a PyBrokerNoticeWrapperBase and forwards the arguments to the
    // underlying notice.
    template <class... Args>
    static TfRefPtr<PyBrokerNoticeWrapper<Self> > Init(Args... args)
    {
        TfRefPtr<PyBrokerNoticeWrapper<Self> > instance =
            TfCreateRefPtr(new PyBrokerNoticeWrapper<Self>());
        instance->_notice = TfCreateRefPtr(new Self(args...));
        return instance;
    }

    virtual object GetWrap() override
    {
        TfPyLock lock;

        return Tf_PyNoticeObjectGenerator::Invoke(*_notice);
    }

    virtual TfRefPtr<BrokerNotice::StageNotice> Get()
    {
        return _notice;
    };

    virtual void Send() override
    {
        _notice->Send();
    }

    //Allows smoother Python wrapping for users.
    template <class... Args>
    static void Wrap(const char* name) {
        class_<PyBrokerNoticeWrapper<Self>,
            TfWeakPtr<PyBrokerNoticeWrapper<Self> >,
            bases<PyBrokerNoticeWrapperBase> >(name)

            .def(TfPyRefAndWeakPtr())

            .def("Init", &PyBrokerNoticeWrapper<Self>::Init<Args...>,
                return_value_policy<TfPyRefPtrFactory<> >())
            .staticmethod("Init");
    }

private:
    TfRefPtr<Self> _notice;
};

} // namespace UNB

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROKER_NOTICE_WRAPPER
