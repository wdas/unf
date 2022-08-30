#include "unf/broadcaster.h"
#include "unf/broker.h"
#include "unf/notice.h"

#include <pxr/pxr.h>

PXR_NAMESPACE_USING_DIRECTIVE

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
