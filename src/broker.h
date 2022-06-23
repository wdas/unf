#ifndef NOTICE_BROKER_BROKER_H
#define NOTICE_BROKER_BROKER_H

#include "notice.h"

#include "pxr/pxr.h"
#include "pxr/base/tf/refBase.h"
#include "pxr/base/tf/refPtr.h"
#include "pxr/base/tf/weakBase.h"
#include "pxr/base/tf/weakPtr.h"
#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usd/common.h"

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

using NoticeCaturePredicateFunc =
    std::function<bool (const UsdBrokerNotice::StageNotice &)>;

using DispatcherPtr = TfRefPtr<Dispatcher>;

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

    template<class BrokerNotice>
    void Process(TfRefPtr<BrokerNotice>&);

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
            std::vector<TfRefPtr<UsdBrokerNotice::StageNotice>>;

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

template<class BrokerNotice>
void NoticeBroker::Process(TfRefPtr<BrokerNotice>& notice)
{
    // Capture the notice to be processed later if a transaction is pending.
    if (_transactions.size() > 0) {
        _TransactionHandler& transaction = _transactions.back();

        // Indicate whether the notice needs to be captured.
        if (transaction.predicate && !transaction.predicate(*notice))
            return;

        // Store notices per type name, so that each type can be merged if
        // required.
        std::string name = typeid(notice).name();
        transaction.noticeMap[name].push_back(std::move(notice));
    }
    // Otherwise, send the notice.
    else {
        notice->Send(_stage);
    }
}

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROKER_BROKER_H
