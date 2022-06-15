#include "../noticeWrapper.h"
#include "../notice.h"

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

using namespace boost::python;
using namespace PXR_NS::UsdBrokerNotice;

PXR_NAMESPACE_USING_DIRECTIVE

/*
Listener class that registers to listen to a given notice, and stores a list of functions that are executed
upon the receival of the notice.
*/
template <class Self>
class NoticeListener : public TfRefBase, public TfWeakBase {
public:
    NoticeListener() {
        _key = TfNotice::Register(
            TfCreateWeakPtr(this), 
            &NoticeListener<Self>::OnNotice);
    }

    ~NoticeListener() {
        TfNotice::Revoke(_key);
    }

    void OnNotice(const Self& notice) {
        TfRefPtr<const Self> instance = TfRefPtr<const Self>(&notice);
        for (auto& callback : _callbacks) {
            callback.second(instance);
        }
    }

    //Registers the passed in function to be invoked when the notice is received. Returns a registration key
    //to be used to UnRegister.
    size_t Register(std::function<void(TfRefPtr<const UsdBrokerNotice::StageNotice>)> callback) {
        ctr++;
        _callbacks[ctr] = callback;
        return ctr;
    }

    void UnRegister(size_t key) {
        _callbacks.erase(key);
    }


private:
    TfNotice::Key _key;
    size_t ctr = 0;
    //map of key : handler function
    std::unordered_map<size_t, std::function<void(TfRefPtr<const UsdBrokerNotice::StageNotice>)>> _callbacks;
};


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

    //Registers the given callback function to the notice listener.
    size_t Register(std::function<void(TfRefPtr<const UsdBrokerNotice::StageNotice>)> callback) {
        if (!_listener) {
            _listener = TfCreateRefPtr(new NoticeListener<Self>());
        }
        return _listener->Register(callback);
    }

    void UnRegister(size_t key) {
        _listener->UnRegister(key);
    }

private:
    TfRefPtr<Self> _notice;
    TfRefPtr<NoticeListener<Self>> _listener;
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