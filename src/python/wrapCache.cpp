#include "../cache.h"
#include "../noticeWrapper.h"

#include <pxr/pxr.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/common.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/base/tf/pyPtrHelpers.h>
#include <pxr/base/tf/makePyConstructor.h>
#include <pxr/base/tf/pyFunction.h>
#include <pxr/base/tf/pyNoticeWrapper.h>
#include <pxr/base/tf/pyLock.h>

#include <boost/python.hpp>


using namespace boost::python;

PXR_NAMESPACE_USING_DIRECTIVE

/*
An alternate NoticeCache for the purpose of Python Wrapping: the original NoticeCache used templates, and this implementation
avoids the use of templates.
Question: Do we want to use this NoticeCache for both C++ & Python or keep the two separate?
*/
class NoticeCache2 : public BaseNoticeCache
{
public:
    NoticeCache2() {}

    NoticeCache2(TfRefPtr<NoticeWrapper> wrapper):_wrapper(wrapper)
    {
        //Registers the _OnReceiving handler to be invoked when the notice is received.
        _key = wrapper->Register(std::bind(&NoticeCache2::_OnReceiving, this, std::placeholders::_1));
    }

    ~NoticeCache2() {
        _wrapper->UnRegister(_key);
    }

    virtual size_t Size() const override
    {
        return _notices.size();
    }

    virtual const std::vector<object> GetAll() const
    { 
        TfPyLock lock;

        std::vector<object> noticeObjects;
        for (auto& n : _notices){
            noticeObjects.push_back(Tf_PyNoticeObjectGenerator::Invoke(*n));
        }

        return noticeObjects;
    }

    virtual void MergeAll() override
    { 
        if (!(_notices.size() > 1 && _notices[0]->IsMergeable())) {
            return;
        }

        // Copy and merge all notices.
        TfRefPtr<UsdBrokerNotice::StageNotice> notice = _notices.at(0)->CopyAsStageNotice();
        auto it = std::next(_notices.begin());

        while(it != _notices.end()) {
            TfRefPtr<UsdBrokerNotice::StageNotice> notice2 = (*it)->CopyAsStageNotice();
            notice->Merge(std::move(*notice2));
            it++;
        }

        // Replace list of notices with merged notice.
        _notices = std::vector<TfRefPtr<const UsdBrokerNotice::StageNotice>> {notice};
    }

    virtual void Clear() override { _notices.clear(); } 

private:
    void _OnReceiving(TfRefPtr<const UsdBrokerNotice::StageNotice> notice)
    {
        _notices.push_back(notice);
    }

    TfRefPtr<NoticeWrapper> _wrapper;
    std::vector<TfRefPtr<const UsdBrokerNotice::StageNotice>> _notices;
    size_t _key;

};

void wrapCache()
{
    class_<NoticeCache2, boost::noncopyable>("NoticeCache")
        .def(init<TfRefPtr<NoticeWrapper>>())
        .def("GetAll", &NoticeCache2::GetAll)
        .def("MergeAll", &NoticeCache2::MergeAll);
}
