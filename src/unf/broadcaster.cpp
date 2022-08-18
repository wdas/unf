#include "unf/broadcaster.h"
#include "unf/broker.h"
#include "unf/notice.h"

#include <pxr/pxr.h>

PXR_NAMESPACE_OPEN_SCOPE

namespace unf {

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

} // namespace unf

PXR_NAMESPACE_CLOSE_SCOPE
