#include "broadcaster.h"
#include "broker.h"
#include "notice.h"

#include <pxr/pxr.h>

PXR_NAMESPACE_OPEN_SCOPE

TF_REGISTRY_FUNCTION(TfType)
{
    TfType::Define<Broadcaster>();
}

BroadcasterContext::BroadcasterContext(
    const UsdBrokerNotice::StageNoticeRefPtr& notice)
{
    std::string name = notice->GetTypeId();
    _noticeMap[name].push_back(notice);
}

BroadcasterContext::BroadcasterContext(NoticePtrMap& noticeMap)
: _noticeMap(noticeMap)
{

}

NoticePtrList& BroadcasterContext::Get(
    const std::string& identifier)
{
    return _noticeMap.at(identifier);
}

Broadcaster::Broadcaster(const NoticeBrokerWeakPtr& broker)
    : _broker(broker)
{

}

void Broadcaster::_AddChild(const BroadcasterPtr& child)
{
    _children.push_back(child);
}

void Broadcaster::_Execute(
    const UsdBrokerNotice::StageNoticeRefPtr& notice)
{
    BroadcasterContext context(notice);

    Execute(context);
    _ExecuteChildren(context);
}

void Broadcaster::_Execute(NoticePtrMap& noticeMap)
{
    BroadcasterContext context(noticeMap);

    Execute(context);
    _ExecuteChildren(context);
}


void Broadcaster::_ExecuteChildren(BroadcasterContext& context)
{
    for (auto& child: _children) {
        child->Execute(context);
    }
}


PXR_NAMESPACE_CLOSE_SCOPE
