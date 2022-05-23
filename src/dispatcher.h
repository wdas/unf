#ifndef NOTICE_BROKER_DISPATCHER_H
#define NOTICE_BROKER_DISPATCHER_H

#include "notice.h"
#include "broker.h"

#include "pxr/pxr.h"
#include "pxr/base/tf/refBase.h"
#include "pxr/base/tf/refPtr.h"
#include "pxr/base/tf/weakBase.h"
#include "pxr/usd/usd/common.h"

PXR_NAMESPACE_OPEN_SCOPE

class Dispatcher : public TfRefBase, public TfWeakBase {
public:
    virtual ~Dispatcher() = default;

protected:
    Dispatcher(const NoticeBrokerWeakPtr&);

    NoticeBrokerWeakPtr _broker;
};

class StageDispatcher : public Dispatcher {
public:
    virtual ~StageDispatcher() {}

private:
    StageDispatcher(const NoticeBrokerWeakPtr& broker);

    template<class BrokerNotice, class UsdNotice>
    void _Register()
    {
        auto self = TfCreateWeakPtr(this);
        auto cb = &StageDispatcher::_OnReceiving<BrokerNotice, UsdNotice>;
        _keys.push_back(TfNotice::Register(self, cb, _broker->GetStage()));
    }

    template<class BrokerNotice, class UsdNotice>
    void _OnReceiving(
        const UsdNotice& notice, const UsdStageWeakPtr& stage)
    {
         TfRefPtr<BrokerNotice> _notice = BrokerNotice::Create(notice);
        _broker->Process(_notice);
    }

    std::vector<TfNotice::Key> _keys;

    friend class NoticeBroker;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROKER_DISPATCHER_H
