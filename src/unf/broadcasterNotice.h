#ifndef NOTICE_BROADCASTER_NOTICE_H
#define NOTICE_BROADCASTER_NOTICE_H

#include "notice.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/notice.h>
#include <pxr/usd/sdf/path.h>

namespace unf {

namespace BroadcasterNotice {

class HierarchyChanged : public BrokerNotice::StageNoticeImpl<HierarchyChanged> {
    public:
        HierarchyChanged(PXR_NS::SdfPathVector added, PXR_NS::SdfPathVector removed, PXR_NS::SdfPathVector modified,
                                ChangedFieldMap changedFields) : _added(std::move(added)), _removed(std::move(removed)), 
                                                    _modified(std::move(modified)), _changedFields(std::move(changedFields)){}
        
        virtual ~HierarchyChanged() = default;

        const PXR_NS::SdfPathVector& GetAdded() const {
            return _added;
        }
        const PXR_NS::SdfPathVector& GetRemoved() const {
            return _removed;
        }
        const PXR_NS::SdfPathVector& GetModified() const {
            return _modified;
        }
        const ChangedFieldMap& GetChangedFields() const {
            return _changedFields;
        }

        virtual void Merge(HierarchyChanged&&) override;
    
    private:
        PXR_NS::SdfPathVector _added;
        PXR_NS::SdfPathVector _removed;
        PXR_NS::SdfPathVector _modified;
        ChangedFieldMap _changedFields;
};


} // namespace BroadcasterNotice
} // namespace unf

#endif // NOTICE_BROADCASTER_NOTICE_H
