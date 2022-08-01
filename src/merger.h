#ifndef NOTICE_BROKER_MERGER_H
#define NOTICE_BROKER_MERGER_H

#include "notice.h"

#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>

#include <functional>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

PXR_NAMESPACE_OPEN_SCOPE
class Dispatcher;

using NoticeCaturePredicateFunc =
    std::function<bool (const UsdBrokerNotice::StageNotice &)>;

using DispatcherPtr = TfRefPtr<Dispatcher>;

using _StageNoticePtrList =
    std::vector<UsdBrokerNotice::StageNoticeRefPtr>;

class NoticeMerger {
public:
    NoticeMerger(const NoticeCaturePredicateFunc& predicate=nullptr, bool topLevel = false)
        : _predicate(predicate), _topLevel(topLevel) {}

    void Add(const UsdBrokerNotice::StageNoticeRefPtr&);
    void Join(NoticeMerger&);
    void Merge();
    void Send(const UsdStageWeakPtr&);
    std::unordered_map<std::string, _StageNoticePtrList>& GetNotices() {
        return _noticeMap;
    }
    void Clear() {
        _noticeMap.clear();
    }

private:

    std::unordered_map<std::string, _StageNoticePtrList> _noticeMap;
    NoticeCaturePredicateFunc _predicate = nullptr;
    bool _topLevel;

};

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROKER_MERGER_H
