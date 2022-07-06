#include "./noticeWrapper.h"

#include "cache.h"

#include <pxr/pxr.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/common.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/base/tf/pyPtrHelpers.h>
#include <pxr/base/tf/makePyConstructor.h>
#include <pxr/base/tf/pyFunction.h>
#include <pxr/base/tf/pyNoticeWrapper.h>
#include <pxr/base/tf/pyLock.h>
#include <pxr/base/tf/pyUtils.h>

#include <boost/python.hpp>

using namespace boost::python;

PXR_NAMESPACE_USING_DIRECTIVE

// Expose NoticeCache without template for Python.
class PythonNoticeCache : public BaseNoticeCache
{
public:
    PythonNoticeCache() {}

    PythonNoticeCache(const TfType type)
    {
        if (type == TfType::Find<UsdBrokerNotice::StageNotice>()
            || !type.IsA<UsdBrokerNotice::StageNotice>())
        {
            TfPyThrowRuntimeError(
                "Expecting a notice derived from UsdBrokerNotice::StageNotice.");
        }

        _key = TfNotice::Register(
            TfCreateWeakPtr(this),
            &PythonNoticeCache::_OnReceiving,
            type, nullptr);
    }

    PythonNoticeCache(const TfType type, const TfAnyWeakPtr &sender)
    {
        if (type == TfType::Find<UsdBrokerNotice::StageNotice>()
            || !type.IsA<UsdBrokerNotice::StageNotice>())
        {
            TfPyThrowRuntimeError(
                "Expecting a notice derived from UsdBrokerNotice::StageNotice.");
        }

        _key = TfNotice::Register(
            TfCreateWeakPtr(this),
            &PythonNoticeCache::_OnReceiving,
            type, sender);
    }

    ~PythonNoticeCache()
    {
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
        for (auto& notice: _notices){
            noticeObjects.push_back(
                Tf_PyNoticeObjectGenerator::Invoke(*notice));
        }

        return noticeObjects;
    }

    virtual void MergeAll() override
    {
        if (!(_notices.size() > 1 && _notices[0]->IsMergeable())) {
            return;
        }

        // Copy and merge all notices.
        auto notice = _notices.at(0)->CopyAsStageNotice();
        auto it = std::next(_notices.begin());

        while(it != _notices.end()) {
            auto notice2 = (*it)->CopyAsStageNotice();
            notice->Merge(std::move(*notice2));
            it++;
        }

        // Replace list of notices with merged notice.
        _notices =
            std::vector<TfRefPtr<const UsdBrokerNotice::StageNotice> > {notice};
    }

    virtual void Clear() override { _notices.clear(); }

private:
    void _OnReceiving(
        const TfNotice& notice,
        const TfType& noticeType,
        TfWeakBase *sender,
        const void *senderUniqueId,
        const std::type_info& senderType)
    {
        _notices.push_back(
            TfRefPtr<const UsdBrokerNotice::StageNotice>(
                &dynamic_cast<const UsdBrokerNotice::StageNotice&>(notice))
        );
    }

   std::vector<TfRefPtr<const UsdBrokerNotice::StageNotice> > _notices;
   TfNotice::Key _key;
};

void wrapCache()
{
    class_<PythonNoticeCache, boost::noncopyable>("NoticeCache")

        .def(init<TfType>())

        .def("Size", &PythonNoticeCache::Size,
            return_value_policy<return_by_value>())

        .def("GetAll", &PythonNoticeCache::GetAll,
            return_value_policy<return_by_value>())

        .def("MergeAll", &PythonNoticeCache::MergeAll)

        .def("Clear", &PythonNoticeCache::Clear);
}
