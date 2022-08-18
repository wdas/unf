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

PXR_NAMESPACE_OPEN_SCOPE

namespace unf {

class NoticeMerger;

using NoticeMergerPtr = TfRefPtr<NoticeMerger>;

using NoticeCaturePredicateFunc =
    std::function<bool (const BrokerNotice::StageNotice &)>;

using _StageNoticePtrList = std::vector<BrokerNotice::StageNoticeRefPtr>;

using _StageNoticePtrMap = std::unordered_map<std::string, _StageNoticePtrList>;

class NoticeMerger : public TfRefBase {
public:
    static NoticeMergerPtr Create(
        const NoticeCaturePredicateFunc& predicate=nullptr)
    {
        return TfCreateRefPtr(new NoticeMerger(predicate));
    }

    void Add(const BrokerNotice::StageNoticeRefPtr&);
    void Join(NoticeMerger&);
    void Merge();
    void Send(const UsdStageWeakPtr&);

    _StageNoticePtrMap& GetNotices() { return _noticeMap; }

private:
    NoticeMerger(const NoticeCaturePredicateFunc& predicate=nullptr)
        : _predicate(predicate) {}

    _StageNoticePtrMap _noticeMap;
    NoticeCaturePredicateFunc _predicate = nullptr;
};

} // namespace unf

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROKER_MERGER_H
