#include "UsdNoticeBroker/broadcaster.h"
#include "UsdNoticeBroker/broker.h"
#include "UsdNoticeBroker/notice.h"

#include <pxr/pxr.h>

namespace UNB {

PXR_NAMESPACE_OPEN_SCOPE

TF_REGISTRY_FUNCTION(TfType)
{
    TfType::Define<Broadcaster>();
}

Broadcaster::Broadcaster(const BrokerWeakPtr& broker)
    : _broker(broker)
{

}

void Broadcaster::_AddChild(const BroadcasterPtr& child)
{
    _children.push_back(child);
}

} // namespace UNB

PXR_NAMESPACE_CLOSE_SCOPE
