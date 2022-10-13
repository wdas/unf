#ifndef USD_NOTICE_FRAMEWORK_NOTICE_CACHE_H
#define USD_NOTICE_FRAMEWORK_NOTICE_CACHE_H

#include "unf/notice.h"

#include <pxr/base/tf/anyWeakPtr.h>
#include <pxr/base/tf/notice.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/pxr.h>

#include <type_traits>
#include <vector>

namespace unf {

class BaseNoticeCache : public PXR_NS::TfWeakBase {
  public:
    BaseNoticeCache() {}
    BaseNoticeCache(BaseNoticeCache&&) = default;

    virtual size_t Size() const = 0;
    virtual void Clear() = 0;
    virtual void MergeAll() = 0;
};

template <class T>
class NoticeCache : public BaseNoticeCache {
  public:
    NoticeCache()
    {
        static_assert(
            std::is_base_of<BrokerNotice::StageNotice, T>::value
                && !std::is_same<BrokerNotice::StageNotice, T>::value,
            "Expecting a notice derived from BrokerNotice::StageNotice.");

        _key = PXR_NS::TfNotice::Register(
            PXR_NS::TfCreateWeakPtr(this), &NoticeCache::_OnReceiving);
    }

    NoticeCache(const PXR_NS::TfAnyWeakPtr& sender)
    {
        static_assert(
            std::is_base_of<BrokerNotice::StageNotice, T>::value
                && !std::is_same<BrokerNotice::StageNotice, T>::value,
            "Expecting a notice derived from BrokerNotice::StageNotice.");

        _key = PXR_NS::TfNotice::Register(
            PXR_NS::TfCreateWeakPtr(this), &NoticeCache::_OnReceiving, sender);
    }

    ~NoticeCache() { PXR_NS::TfNotice::Revoke(_key); }

    virtual size_t Size() const override { return _notices.size(); }

    virtual const std::vector<PXR_NS::TfRefPtr<const T> >& GetAll() const
    {
        return _notices;
    }

    virtual void MergeAll() override
    {
        if (!(_notices.size() > 1 && _notices[0]->IsMergeable())) {
            return;
        }

        // Copy and merge all notices.
        PXR_NS::TfRefPtr<T> notice = _notices.at(0)->Copy();
        auto it = std::next(_notices.begin());

        while (it != _notices.end()) {
            PXR_NS::TfRefPtr<T> notice2 = (*it)->Copy();
            notice->Merge(std::move(*notice2));
            it++;
        }

        // Replace list of notices with merged notice.
        _notices = std::vector<PXR_NS::TfRefPtr<const T> >{notice};
    }

    virtual void Clear() override { _notices.clear(); }

  private:
    void _OnReceiving(const T& notice)
    {
        _notices.push_back(PXR_NS::TfRefPtr<const T>(&notice));
    }

    std::vector<PXR_NS::TfRefPtr<const T> > _notices;
    PXR_NS::TfNotice::Key _key;
};

}  // namespace unf

#endif  // USD_NOTICE_FRAMEWORK_NOTICE_CACHE_H
