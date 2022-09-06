#ifndef NOTICE_BROKER_BROADCASTER_H
#define NOTICE_BROKER_BROADCASTER_H

#include "unf/broker.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/type.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/usd/usd/common.h>

#include <string>
#include <vector>
#include <iostream>

namespace unf {

class Broadcaster : public PXR_NS::TfRefBase, public PXR_NS::TfWeakBase {
public:
    virtual ~Broadcaster() = default;

    virtual std::string GetIdentifier() const =0;
    virtual std::string GetParentIdentifier() const { return std::string(); }

    virtual void Execute(void* parent) {
        for(auto c : _children) {
            c->Execute(this);
        }
    }

protected:
    Broadcaster(const BrokerWeakPtr&);
    void _AddChild(const BroadcasterPtr&);

    std::vector<BroadcasterPtr> _children;
    BrokerWeakPtr _broker;

private:
    friend class Broker;
};

class BroadcasterFactory : public PXR_NS::TfType::FactoryBase
{
public:
    virtual PXR_NS::TfRefPtr<Broadcaster> New(
        const BrokerWeakPtr& broker) const = 0;
};

template <class T>
class BroadcasterFactoryImpl : public BroadcasterFactory
{
public:
    virtual PXR_NS::TfRefPtr<Broadcaster> New(
        const BrokerWeakPtr& broker) const override
    {
        return TfCreateRefPtr(new T(broker));
    }
};

template <class T, class ...Bases>
void BroadcasterDefine()
{
    PXR_NS::TfType::Define<T, PXR_NS::TfType::Bases<Bases...> >()
        .template SetFactory<BroadcasterFactoryImpl<T> >();
}

} // namespace unf

#endif // NOTICE_BROKER_BROADCASTER_H
