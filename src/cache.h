#ifndef NOTICE_BROKER_NOTICE_CACHE_H
#define NOTICE_BROKER_NOTICE_CACHE_H

#include "notice.h"

#include "pxr/pxr.h"
#include "pxr/base/tf/notice.h"
#include "pxr/base/tf/weakBase.h"
#include "pxr/base/tf/weakPtr.h"
#include "pxr/base/tf/anyWeakPtr.h"
#include "pxr/base/tf/refPtr.h"

#include <vector>
#include <type_traits>

PXR_NAMESPACE_OPEN_SCOPE

class BaseNoticeCache : public TfWeakBase
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
        _key = TfNotice::Register(
            TfCreateWeakPtr(this), 
            &NoticeCache::_OnReceiving);
    }

    NoticeCache(const TfAnyWeakPtr &sender)
    {
        static_assert(std::is_base_of<UsdBrokerNotice::StageNotice, T>::value);
        _key = TfNotice::Register(
            TfCreateWeakPtr(this), 
            &NoticeCache::_OnReceiving, 
            sender);
    }

    ~NoticeCache()
    {
        TfNotice::Revoke(_key);
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
        UsdBrokerNotice::StageNotice notice = *_notices.at(0);
        auto it = std::next(_notices.begin());

        while(it != _notices.end()) {
            UsdBrokerNotice::StageNotice notice2 = **it++;
            notice.Merge(std::move(notice2));
        }

        // Replace list of notices with merged notice.
        _notices = UsdBrokerNotice::StageNoticeConstPtrList {
            UsdBrokerNotice::StageNoticePtr(&notice)
        };
    }

    virtual void Clear() override { _notices.clear(); } 

private:
    void _OnReceiving(const T& notice)
    {
        _notices.push_back(UsdBrokerNotice::StageNoticeConstPtr(&notice));
    }

    UsdBrokerNotice::StageNoticeConstPtrList _notices;
    TfNotice::Key _key;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROKER_NOTICE_CACHE_H
