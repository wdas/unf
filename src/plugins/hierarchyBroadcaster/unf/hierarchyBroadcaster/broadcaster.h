#ifndef USD_NOTICE_FRAMEWORK_HIERARCHY_BROADCASTER_H
#define USD_NOTICE_FRAMEWORK_HIERARCHY_BROADCASTER_H

#include "cache.h"
#include "notice.h"

#include <unf/broadcaster.h>
#include <unf/notice.h>

#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/type.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>

#include <string>
#include <vector>

namespace unf {

using _StageNoticePtrList = std::vector<BrokerNotice::StageNoticeRefPtr>;
using _StageNoticePtrMap = std::unordered_map<std::string, _StageNoticePtrList>;

using ObjectsChangedRefPtr = PXR_NS::TfRefPtr<BrokerNotice::ObjectsChanged>;

class HierarchyBroadcaster : public Broadcaster {
  public:
    HierarchyBroadcaster(const BrokerWeakPtr& broker)
        : Broadcaster(broker), _cache(broker->GetStage())
    {
    }

    std::string GetIdentifier() const override
    {
        return "HierarchyBroadcaster";
    }
    
    const PXR_NS::SdfPathVector& GetAdded() const { return _cache.GetAdded(); }
    const PXR_NS::SdfPathVector& GetRemoved() const
    {
        return _cache.GetRemoved();
    }
    const PXR_NS::SdfPathVector& GetModified() const
    {
        return _cache.GetModified();
    }
    
    const ChangedFieldMap& GetChangedFields() const { return *_changedFields; }

    void Clear()
    {
        _changedFields = nullptr;
        _cache.Clear();
    }

    void Execute(void* parent) override;

  private:
    HierarchyCache _cache;
    const ChangedFieldMap* _changedFields;
};

}  // namespace unf

#endif  // USD_NOTICE_FRAMEWORK_HIERARCHY_BROADCASTER_H
