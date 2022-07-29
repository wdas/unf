#ifndef NOTICE_BROKER_CONTEXT_H
#define NOTICE_BROKER_CONTEXT_H

#include "notice.h"

#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>

#include <functional>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

PXR_NAMESPACE_OPEN_SCOPE

using NoticeCaturePredicateFunc =
    std::function<bool (const UsdBrokerNotice::StageNotice &)>;

using NoticePtrList = std::vector<UsdBrokerNotice::StageNoticeRefPtr>;

using NoticePtrMap = std::unordered_map<std::string, NoticePtrList>;

class NoticeContext {
public:
    NoticeContext() = default;
    NoticeContext(const UsdBrokerNotice::StageNoticeRefPtr&);

    void SetFilterPredicate(const NoticeCaturePredicateFunc&);

    void Add(const UsdBrokerNotice::StageNoticeRefPtr&);
    const NoticePtrList& Get(const std::string&) const;

    void Join(NoticeContext&);
    void Merge();
    void SendAll(const UsdStageWeakPtr& stage);

private:
    NoticePtrMap _noticeMap;
    NoticeCaturePredicateFunc _predicate = nullptr;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROKER_CONTEXT_H
