#ifndef NOTICE_BROKER_MERGER_H
#define NOTICE_BROKER_MERGER_H

#include "unf/notice.h"

#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>

#include <functional>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace unf {

class NoticeMerger;

using NoticeCaturePredicateFunc =
    std::function<bool(const BrokerNotice::StageNotice&)>;

using _StageNoticePtrList = std::vector<BrokerNotice::StageNoticeRefPtr>;

using _StageNoticePtrMap = std::unordered_map<std::string, _StageNoticePtrList>;

class NoticeMerger {
  public:
    NoticeMerger(const NoticeCaturePredicateFunc& predicate = nullptr)
        : _predicate(predicate)
    {
    }

    void Add(const BrokerNotice::StageNoticeRefPtr&);
    void Join(NoticeMerger&);
    void Merge();
    void Send(const PXR_NS::UsdStageWeakPtr&);

    _StageNoticePtrMap& GetNotices() { return _noticeMap; }

  private:
    _StageNoticePtrMap _noticeMap;
    NoticeCaturePredicateFunc _predicate = nullptr;
};

}  // namespace unf

#endif  // NOTICE_BROKER_MERGER_H
