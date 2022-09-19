#ifndef NOTICE_BROKER_DISPATCHER_H
#define NOTICE_BROKER_DISPATCHER_H

#include "unf/broker.h"
#include "unf/notice.h"

#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/type.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>

namespace unf {

class Dispatcher : public PXR_NS::TfRefBase, public PXR_NS::TfWeakBase {
  public:
    virtual ~Dispatcher() { Revoke(); };

    virtual std::string GetIdentifier() const = 0;

    virtual void Register() = 0;
    virtual void Revoke();

  protected:
    Dispatcher(const BrokerWeakPtr&);

    template <class InputNotice, class OutputNotice>
    void _Register()
    {
        auto self = PXR_NS::TfCreateWeakPtr(this);
        auto cb = &Dispatcher::_OnReceiving<InputNotice, OutputNotice>;
        _keys.push_back(
            PXR_NS::TfNotice::Register(self, cb, _broker->GetStage()));
    }

    template <class InputNotice, class OutputNotice>
    void _OnReceiving(const InputNotice& notice)
    {
        PXR_NS::TfRefPtr<OutputNotice> _notice = OutputNotice::Create(notice);
        _broker->Send(_notice);
    }

    BrokerWeakPtr _broker;
    std::vector<PXR_NS::TfNotice::Key> _keys;
};

class StageDispatcher : public Dispatcher {
  public:
    virtual std::string GetIdentifier() const { return "StageDispatcher"; }

    virtual void Register();

  private:
    StageDispatcher(const BrokerWeakPtr& broker);

    friend class Broker;
};

class DispatcherFactory : public PXR_NS::TfType::FactoryBase {
  public:
    virtual PXR_NS::TfRefPtr<Dispatcher> New(
        const BrokerWeakPtr& broker) const = 0;
};

template <class T>
class DispatcherFactoryImpl : public DispatcherFactory {
  public:
    virtual PXR_NS::TfRefPtr<Dispatcher> New(
        const BrokerWeakPtr& broker) const override
    {
        return PXR_NS::TfCreateRefPtr(new T(broker));
    }
};

template <class T, class... Bases>
void DispatcherDefine()
{
    PXR_NS::TfType::Define<T, PXR_NS::TfType::Bases<Bases...> >()
        .template SetFactory<DispatcherFactoryImpl<T> >();
}

}  // namespace unf

#endif  // NOTICE_BROKER_DISPATCHER_H
