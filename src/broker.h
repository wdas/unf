#ifndef NOTICE_BROKER_BROKER_H
#define NOTICE_BROKER_BROKER_H

#include "notice.h"
#include "python/noticeWrapper.h"

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
#include <unordered_map>

PXR_NAMESPACE_OPEN_SCOPE

class NoticeBroker;
class Dispatcher;

using NoticeBrokerPtr = TfRefPtr<NoticeBroker>;
using NoticeBrokerWeakPtr = TfWeakPtr<NoticeBroker>;

using DispatcherPtr = TfRefPtr<Dispatcher>;
using NoticeCaturePredicateFunc = 
    std::function<bool (const UsdBrokerNotice::StageNotice &)>;

class NoticeBroker : public TfRefBase, public TfWeakBase {
public:
    static NoticeBrokerPtr Create(const UsdStageWeakPtr& stage);

    virtual ~NoticeBroker() {}

    const UsdStageWeakPtr& GetStage() const { return _stage; }

    bool IsInTransaction();

    void BeginTransaction(const NoticeCaturePredicateFunc& predicate=nullptr);
    void EndTransaction();

    template<class BrokerNotice, class... Args>
    void Send(Args&&... args);

    void Process(const UsdBrokerNotice::StageNoticeRefPtr notice);

    // Don't allow copies
    NoticeBroker(const NoticeBroker &) = delete;
    NoticeBroker &operator=(const NoticeBroker &) = delete;

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

private:
    NoticeBroker(const UsdStageWeakPtr&);

    struct _TransactionHandler {
        _TransactionHandler() {}
        _TransactionHandler(_TransactionHandler&& t)
            : noticeMap(std::move(t.noticeMap))
            , predicate(t.predicate) {}

        using _StageNoticePtrList = 
            std::vector<UsdBrokerNotice::StageNoticeRefPtr>;

        std::unordered_map<std::string, _StageNoticePtrList> noticeMap;
        NoticeCaturePredicateFunc predicate = nullptr;

        void Join(_TransactionHandler&);
    };

    void _SendNotices(_TransactionHandler&);
    static void _CleanCache();
    
    // A registry of hashed stage ptr to the corresponding stage's broker ptr.
    static std::unordered_map<size_t, TfRefPtr<NoticeBroker>> noticeBrokerRegistry;

private:
    UsdStageWeakPtr _stage;
    std::vector<_TransactionHandler> _transactions;
    std::unordered_map<std::string, DispatcherPtr> _dispatcherMap;
};

template<class BrokerNotice, class... Args>
void NoticeBroker::Send(Args&&... args)
{
    TfRefPtr<BrokerNotice> _notice = BrokerNotice::Create(
        std::forward<Args>(args)...);

    Process(_notice);
}

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROKER_BROKER_H
