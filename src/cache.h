#ifndef NOTICE_BROKER_NOTICE_CACHE_H
#define NOTICE_BROKER_NOTICE_CACHE_H

#include "notice.h"

#include "pxr/pxr.h"
#include "pxr/base/tf/notice.h"
#include "pxr/base/tf/weakBase.h"
#include "pxr/base/tf/weakPtr.h"
#include "pxr/base/tf/anyWeakPtr.h"

#include <memory>
#include <vector>
#include <type_traits>

PXR_NAMESPACE_OPEN_SCOPE

class BaseNoticeCache : public PXR_NS::TfWeakBase
{
public:
    BaseNoticeCache() {}
    BaseNoticeCache(BaseNoticeCache&&) = default;

    virtual const UsdBrokerNotice::StageNoticeConstPtrList& GetAll() const = 0;
    virtual void Clear() = 0;
    virtual void MergeAll() = 0;
};

template <class T>
class NoticeCache : public BaseNoticeCache
{
public:
    NoticeCache()
    {
        static_assert(std::is_base_of<UsdBrokerNotice::StageNotice, T>::value);
        _key = PXR_NS::TfNotice::Register(
            PXR_NS::TfCreateWeakPtr(this), 
            &NoticeCache::_OnReceiving);
    }

    NoticeCache(const PXR_NS::TfAnyWeakPtr &sender)
    {
        static_assert(std::is_base_of<UsdBrokerNotice::StageNotice, T>::value);
        _key = PXR_NS::TfNotice::Register(
            PXR_NS::TfCreateWeakPtr(this), 
            &NoticeCache::_OnReceiving, 
            sender);
    }

    ~NoticeCache()
    {
        PXR_NS::TfNotice::Revoke(_key);
    }

    virtual const UsdBrokerNotice::StageNoticeConstPtrList& GetAll() const override
    { 
        return _notices; 
    }

    virtual void MergeAll() override
    { 
        if (!(_notices.size() > 1 && _notices[0]->IsMergeable())) {
            return;
        }

        // Copy and merge all notices.
        UsdBrokerNotice::StageNotice notice = *_notices.at(0).get();
        auto it = std::next(_notices.begin());

        while(it != _notices.end()) {
            UsdBrokerNotice::StageNotice notice2 = **it++;
            notice.Merge(std::move(notice2));
        }

        // Replace list of notices with merged notice.
        _notices = UsdBrokerNotice::StageNoticeConstPtrList {
            std::shared_ptr<UsdBrokerNotice::StageNotice>(&notice)
        };
    }

    virtual void Clear() override { _notices.clear(); } 

private:
    void _OnReceiving(const T& notice)
    {
        _notices.push_back(notice.GetReference());
    }

    UsdBrokerNotice::StageNoticeConstPtrList _notices;
    PXR_NS::TfNotice::Key _key;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROKER_NOTICE_CACHE_H
