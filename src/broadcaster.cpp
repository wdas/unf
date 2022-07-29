#include "broadcaster.h"
#include "broker.h"
#include "context.h"
#include "notice.h"

#include <pxr/pxr.h>

PXR_NAMESPACE_OPEN_SCOPE

TF_REGISTRY_FUNCTION(TfType)
{
    TfType::Define<Broadcaster>();
}

Broadcaster::Broadcaster(const NoticeBrokerWeakPtr& broker)
    : _broker(broker)
{

}

void Broadcaster::_AddChild(const BroadcasterPtr& child)
{
    _children.push_back(child);
}

void Broadcaster::_Execute(NoticeContext& context)
{
    Execute(context);

    for (auto& child: _children) {
        child->Execute(context);
    }
}

PXR_NAMESPACE_CLOSE_SCOPE
