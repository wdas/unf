#ifndef NOTICE_BROKER_NOTICE_WRAPPER_H
#define NOTICE_BROKER_NOTICE_WRAPPER_H

#include "unf/notice.h"

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

namespace unf {

// Interface object to process custom notice in Python.
// TODO: Should we forbid handling of custom notices via Python?
class PyBrokerNoticeWrapperBase
: public PXR_NS::TfRefBase, public PXR_NS::TfWeakBase {
public:
    PyBrokerNoticeWrapperBase() {};

    virtual PXR_NS::TfRefPtr<BrokerNotice::StageNotice> Get() {
        return nullptr;
    }

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
    static PXR_NS::TfRefPtr<PyBrokerNoticeWrapper<Self> > Init(Args... args)
    {
        PXR_NS::TfRefPtr<PyBrokerNoticeWrapper<Self> > instance =
            PXR_NS::TfCreateRefPtr(new PyBrokerNoticeWrapper<Self>());
        instance->_notice = PXR_NS::TfCreateRefPtr(new Self(args...));
        return instance;
    }

    virtual object GetWrap() override
    {
        PXR_NS::TfPyLock lock;

        return PXR_NS::Tf_PyNoticeObjectGenerator::Invoke(*_notice);
    }

    virtual PXR_NS::TfRefPtr<BrokerNotice::StageNotice> Get()
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
            PXR_NS::TfWeakPtr<PyBrokerNoticeWrapper<Self> >,
            bases<PyBrokerNoticeWrapperBase> >(name)

            .def(PXR_NS::TfPyRefAndWeakPtr())

            .def("Init", &PyBrokerNoticeWrapper<Self>::Init<Args...>,
                return_value_policy<PXR_NS::TfPyRefPtrFactory<> >())
            .staticmethod("Init");
    }

private:
    PXR_NS::TfRefPtr<Self> _notice;
};

} // namespace unf

#endif // NOTICE_BROKER_NOTICE_WRAPPER
