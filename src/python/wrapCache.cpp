#include "../cache.h"
#include "noticeWrapper.h"

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
*/
class PythonNoticeCache : public BaseNoticeCache
{
public:
    PythonNoticeCache() {}

    PythonNoticeCache(const TfType type)
    {
        _key = TfNotice::Register(
            TfCreateWeakPtr(this), 
            &PythonNoticeCache::_OnReceiving, type, nullptr);
    }

    PythonNoticeCache(const TfType type, const TfAnyWeakPtr &sender)
    {
        _key = TfNotice::Register(
            TfCreateWeakPtr(this), 
            &PythonNoticeCache::_OnReceiving, type, sender);
    }


    ~PythonNoticeCache() {
        TfNotice::Revoke(_key);
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
    void _OnReceiving(const TfNotice& notice, const TfType& noticeType, TfWeakBase *sender, const void *senderUniqueId, const std::type_info& senderType)
    {
        _notices.push_back(TfRefPtr<const UsdBrokerNotice::StageNotice>(&dynamic_cast<const UsdBrokerNotice::StageNotice&>(notice)));
    }
 
   std::vector<TfRefPtr<const UsdBrokerNotice::StageNotice>> _notices;
   TfNotice::Key _key;


};

void wrapCache()
{
    class_<PythonNoticeCache, boost::noncopyable>("NoticeCache")
        .def(init<TfType>())
        .def("GetAll", &PythonNoticeCache::GetAll)
        .def("MergeAll", &PythonNoticeCache::MergeAll);
}
