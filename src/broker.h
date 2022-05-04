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
#include "pxr/usd/usd/notice.h"

#include <functional>
#include <memory>
#include <string>
#include <typeinfo>
#include <vector>

PXR_NAMESPACE_OPEN_SCOPE

class NoticeBroker;

using NoticeBrokerPtr = TfRefPtr<NoticeBroker>;
using NoticeBrokerWeakPtr = TfWeakPtr<NoticeBroker>;

using NoticeCaturePredicateFunc = 
    std::function<bool (const UsdBrokerNotice::StageNotice &)>;

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
    void Send(const std::shared_ptr<BrokerNotice>&);

    // Don't allow copies
    NoticeBroker(const NoticeBroker &) = delete;
    NoticeBroker &operator=(const NoticeBroker &) = delete;

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

    template<class BrokerNotice, class UsdNotice>
    void _Register(const NoticeBrokerWeakPtr& self);

    template<class BrokerNotice, class UsdNotice>
    void _Notify(const UsdNotice&, const UsdStageWeakPtr&);

    void _SendNotices(_TransactionHandler&);

private:
    UsdStageWeakPtr _stage;
    std::vector<_TransactionHandler> _transactions;
    std::vector<TfNotice::Key> _keys;
};

template<class BrokerNotice, class UsdNotice>
void NoticeBroker::_Register(const NoticeBrokerWeakPtr& self)
{
    auto cb = &NoticeBroker::_Notify<BrokerNotice, UsdNotice>;
    _keys.push_back(TfNotice::Register(self, cb, _stage));
}

template<class BrokerNotice, class UsdNotice>
void NoticeBroker::_Notify(
    const UsdNotice& notice, const UsdStageWeakPtr& stage)
{
    auto _notice = std::shared_ptr<BrokerNotice>(new BrokerNotice(notice));
    Send(_notice);
}

template<class BrokerNotice>
void NoticeBroker::Send()
{
    auto _notice = std::make_shared<BrokerNotice>();
    Send(_notice);
}

template<class BrokerNotice>
void NoticeBroker::Send(const std::shared_ptr<BrokerNotice>& notice)
{
    // Capture the notice to be processed later if a transaction is pending.
    if (_transactions.size() > 0) {
        _TransactionHandler& transaction = _transactions.back();

        // Indicate whether the notice needs to be captured.
        if (transaction.predicate && !transaction.predicate(*notice.get()))
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
