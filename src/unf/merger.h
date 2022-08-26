#ifndef NOTICE_BROKER_MERGER_H
#define NOTICE_BROKER_MERGER_H

#include "unf/notice.h"

#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>
#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/refPtr.h>

#include <functional>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace unf {

class NoticeMerger;

using NoticeMergerPtr = PXR_NS::TfRefPtr<NoticeMerger>;

using NoticeCaturePredicateFunc =
    std::function<bool (const BrokerNotice::StageNotice &)>;

using _StageNoticePtrList = std::vector<BrokerNotice::StageNoticeRefPtr>;

using _StageNoticePtrMap = std::unordered_map<std::string, _StageNoticePtrList>;

class NoticeMerger : public PXR_NS::TfRefBase {
public:
    static NoticeMergerPtr Create(
        const NoticeCaturePredicateFunc& predicate=nullptr)
    {
        return PXR_NS::TfCreateRefPtr(new NoticeMerger(predicate));
    }

    void Add(const BrokerNotice::StageNoticeRefPtr&);
    void Join(NoticeMerger&);
    void Merge();
    void Send(const PXR_NS::UsdStageWeakPtr&);

    _StageNoticePtrMap& GetNotices() { return _noticeMap; }

private:
    NoticeMerger(const NoticeCaturePredicateFunc& predicate=nullptr)
        : _predicate(predicate) {}

    _StageNoticePtrMap _noticeMap;
    NoticeCaturePredicateFunc _predicate = nullptr;
};

} // namespace unf

#endif // NOTICE_BROKER_MERGER_H
