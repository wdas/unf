#include "notice.h"

#include "./noticeWrapper.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/notice.h>
#include <pxr/base/tf/pyNoticeWrapper.h>
#include <pxr/base/tf/pyPtrHelpers.h>
#include <pxr/base/tf/makePyConstructor.h>
#include <pxr/base/tf/pyResultConversions.h>
#include <pxr/usd/usd/pyConversions.h>
#include <pxr/base/tf/pyContainerConversions.h>
#include <pxr/base/tf/pyNoticeWrapper.h>
#include <pxr/base/tf/pyLock.h>
#include <pxr/base/tf/type.h>

using namespace boost::python;
using namespace PXR_NS::UsdBrokerNotice;

PXR_NAMESPACE_USING_DIRECTIVE

/*
Creates a NoticeWrapper instance of the StageNotice type passed in as template.
*/
template <class Self>
class NoticeWrapperImpl : public NoticeWrapper
{
public:
    NoticeWrapperImpl(){}

    //Creates a NoticeWrapper and forwards the arguments to the underlying notice.
    template <class... Args>
    static TfRefPtr<NoticeWrapperImpl<Self>> Init(Args... args) {
        TfRefPtr<NoticeWrapperImpl<Self>> instance = TfCreateRefPtr(new NoticeWrapperImpl<Self>());
        instance->_notice = TfCreateRefPtr(new Self(args...));
        return instance;
    }

    virtual boost::python::object GetWrap() override {
        TfPyLock lock;
        return Tf_PyNoticeObjectGenerator::Invoke(*_notice);
    }

    virtual TfRefPtr<UsdBrokerNotice::StageNotice> Get() {return _notice;};

    virtual void Send() override {
        _notice->Send();
    }

    //Allows smoother Python wrapping for users.
    template <class... Args>
    static void Wrap(const char* name) {
        class_<NoticeWrapperImpl<Self>, TfWeakPtr<NoticeWrapperImpl<Self>>, bases<NoticeWrapper>>(name)
            .def(TfPyRefAndWeakPtr())
            .def("Init", &NoticeWrapperImpl<Self>::Init<Args...>, return_value_policy<TfPyRefPtrFactory<> >()).staticmethod("Init");

    }

private:
    TfRefPtr<Self> _notice;
};

void wrapNoticeWrapper(){
    class_<NoticeWrapper, TfWeakPtr<NoticeWrapper>, boost::noncopyable>(
      "NoticeWrapper", init<>())
    .def(TfPyRefAndWeakPtr())
    .def("Get", &NoticeWrapper::GetWrap)
    .def("Send", &NoticeWrapper::Send);

    //TODO: remove -- TEST!!
    NoticeWrapperImpl<TestNotice>::Wrap<int>("TestNoticeWrapper");
}
