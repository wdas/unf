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
#include <vector>

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
    static NoticeBrokerPtr Create(const UsdStageWeakPtr& stage) {
        return TfCreateRefPtr(new NoticeBroker(stage));
    }

    virtual ~NoticeBroker() {}

    const UsdStageWeakPtr& GetStage() const { return _stage; }

    bool IsInTransaction();

    void BeginTransaction(const NoticeCaturePredicateFunc& predicate=nullptr);
    void EndTransaction();

    template<class BrokerNotice>
    void Send();

    template<class BrokerNotice>
    void Send(TfRefPtr<BrokerNotice>&);

    // Don't allow copies
    NoticeBroker(const NoticeBroker &) = delete;
    NoticeBroker &operator=(const NoticeBroker &) = delete;

    template<class T>
    void AddDispatcher() {
        static_assert(std::is_base_of<Dispatcher, T>::value);
        auto self = TfCreateWeakPtr(this);
        _dispatchers.push_back(TfCreateRefPtr(new T(self)));
    }

private:
    NoticeBroker(const UsdStageWeakPtr&);

    struct _TransactionHandler {
        _TransactionHandler() {}
        _TransactionHandler(_TransactionHandler&& t)
            : noticeMap(std::move(t.noticeMap))
            , predicate(t.predicate) {}

        std::map<std::string, UsdBrokerNotice::StageNoticePtrList> noticeMap;
        NoticeCaturePredicateFunc predicate = nullptr;

        void Join(_TransactionHandler&);
    };

    void _SendNotices(_TransactionHandler&);

private:
    UsdStageWeakPtr _stage;
    std::vector<_TransactionHandler> _transactions;
    std::vector<DispatcherPtr> _dispatchers;
};

template<class BrokerNotice>
void NoticeBroker::Send()
{
    TfRefPtr<BrokerNotice> _notice = BrokerNotice::Create();
    Send(_notice);
}

template<class BrokerNotice>
void NoticeBroker::Send(TfRefPtr<BrokerNotice>& notice)
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
