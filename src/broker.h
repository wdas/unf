#ifndef NOTICE_BROKER_BROKER_H
#define NOTICE_BROKER_BROKER_H

#include "notice.h"
#include "merger.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/common.h>

#include <functional>
#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

PXR_NAMESPACE_OPEN_SCOPE

class NoticeBroker;
class Dispatcher;

using NoticeBrokerPtr = TfRefPtr<NoticeBroker>;
using NoticeBrokerWeakPtr = TfWeakPtr<NoticeBroker>;

using DispatcherPtr = TfRefPtr<Dispatcher>;

class NoticeBroker : public TfRefBase, public TfWeakBase {
public:
    static NoticeBrokerPtr Create(const UsdStageWeakPtr& stage);

    virtual ~NoticeBroker() {}

    // Don't allow copies
    NoticeBroker(const NoticeBroker &) = delete;
    NoticeBroker &operator=(const NoticeBroker &) = delete;

    const UsdStageWeakPtr& GetStage() const { return _stage; }

    bool IsInTransaction();

    void BeginTransaction(const NoticeCaturePredicateFunc& predicate=nullptr);
    void EndTransaction();

    template<class BrokerNotice, class... Args>
    void Send(Args&&... args);

    void Send(const UsdBrokerNotice::StageNoticeRefPtr notice);

    template<class T>
    void AddDispatcher() {
        static_assert(std::is_base_of<Dispatcher, T>::value);
        auto self = TfCreateWeakPtr(this);
        auto dispatcher = TfCreateRefPtr(new T(self));
        _dispatcherMap[dispatcher->GetIdentifier()] = dispatcher;
    }

    DispatcherPtr& GetDispatcher(std::string identifier) {
        return _dispatcherMap.at(identifier);
    }

    void DiscoverDispatchers();

private:
    NoticeBroker(const UsdStageWeakPtr&);

    static void _CleanCache();

    // A registry of hashed stage ptr to the corresponding stage's broker ptr.
    static std::unordered_map<size_t, NoticeBrokerPtr> Registry;

    UsdStageWeakPtr _stage;
    std::vector<NoticeMerger> _mergers;
    std::unordered_map<std::string, DispatcherPtr> _dispatcherMap;
};

template<class BrokerNotice, class... Args>
void NoticeBroker::Send(Args&&... args)
{
    TfRefPtr<BrokerNotice> _notice = BrokerNotice::Create(
        std::forward<Args>(args)...);

    Send(_notice);
}

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROKER_BROKER_H
