#ifndef NOTICE_BROKER_BROADCASTER_H
#define NOTICE_BROKER_BROADCASTER_H

#include "broker.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/type.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/usd/usd/common.h>

#include <string>
#include <vector>

PXR_NAMESPACE_OPEN_SCOPE

class Broadcaster : public TfRefBase, public TfWeakBase {
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
    Broadcaster(const NoticeBrokerWeakPtr&);

private:
    void _AddChild(const BroadcasterPtr&);

    std::vector<BroadcasterPtr> _children;

    NoticeBrokerWeakPtr _broker;

    std::unordered_map<std::string, _StageNoticePtrList> _noticeMap;

    friend class NoticeBroker;
};

class ChildBroadcaster : public Broadcaster {
public:
    ChildBroadcaster(const NoticeBrokerWeakPtr& broker) : Broadcaster(broker){}
    virtual std::string GetIdentifier() const { return "ChildBroadcaster"; }

    virtual void Execute(void* parent) {
        /*
        // (need to stage dispatcher static cast)
        for (auto& element : dispatcher.GetNotices()) {
            auto& notices = element.second;
        // Process notices into custo
        
        m members / getter functions
    }
        */
        Broadcaster::Execute(this);
    }
};


class BroadcasterFactory : public TfType::FactoryBase
{
public:
    virtual TfRefPtr<Broadcaster> New(
        const NoticeBrokerWeakPtr& broker) const = 0;
};

template <class T>
class BroadcasterFactoryImpl : public BroadcasterFactory
{
public:
    virtual TfRefPtr<Broadcaster> New(
        const NoticeBrokerWeakPtr& broker) const override
    {
        return TfCreateRefPtr(new T(broker));
    }
};

template <class T, class ...Bases>
void BroadcasterDefine()
{
    TfType::Define<T, TfType::Bases<Bases...> >()
        .template SetFactory<BroadcasterFactoryImpl<T> >();
}

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROKER_BROADCASTER_H
