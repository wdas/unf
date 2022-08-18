#ifndef NOTICE_BROKER_DISPATCHER_H
#define NOTICE_BROKER_DISPATCHER_H

#include "unf/notice.h"
#include "unf/broker.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/type.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/usd/usd/common.h>

PXR_NAMESPACE_OPEN_SCOPE

namespace unf {

class Dispatcher : public TfRefBase, public TfWeakBase {
public:
    virtual ~Dispatcher() { Revoke(); };

    virtual std::string GetIdentifier() const =0;

    virtual void Register() =0;
    virtual void Revoke();

protected:
    Dispatcher(const BrokerWeakPtr&);

    template<class InputNotice, class OutputNotice>
    void _Register()
    {
        auto self = TfCreateWeakPtr(this);
        auto cb = &Dispatcher::_OnReceiving<InputNotice, OutputNotice>;
        _keys.push_back(TfNotice::Register(self, cb, _broker->GetStage()));
    }

    template<class InputNotice, class OutputNotice>
    void _OnReceiving(const InputNotice& notice)
    {
        TfRefPtr<OutputNotice> _notice = OutputNotice::Create(notice);
        _broker->Send(_notice);
    }

    BrokerWeakPtr _broker;
    std::vector<TfNotice::Key> _keys;
};

class StageDispatcher : public Dispatcher {
public:
    virtual std::string GetIdentifier() const { return "StageDispatcher"; }

    virtual void Register();

private:
    StageDispatcher(const BrokerWeakPtr& broker);

    friend class Broker;
};

class DispatcherFactory : public TfType::FactoryBase
{
public:
    virtual TfRefPtr<Dispatcher> New(
        const BrokerWeakPtr& broker) const = 0;
};

template <class T>
class DispatcherFactoryImpl : public DispatcherFactory
{
public:
    virtual TfRefPtr<Dispatcher> New(
        const BrokerWeakPtr& broker) const override
    {
        return TfCreateRefPtr(new T(broker));
    }
};

template <class T, class ...Bases>
void DispatcherDefine()
{
    TfType::Define<T, TfType::Bases<Bases...> >()
        .template SetFactory<DispatcherFactoryImpl<T> >();
}

} // namespace unf

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROKER_DISPATCHER_H
