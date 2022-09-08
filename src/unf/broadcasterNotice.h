#ifndef NOTICE_BROADCASTER_NOTICE_H
#define NOTICE_BROADCASTER_NOTICE_H

#include "notice.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/notice.h>
#include <pxr/usd/sdf/path.h>

namespace unf {

using UnorderedSdfPathSet = std::unordered_set<PXR_NS::SdfPath, PXR_NS::SdfPath::Hash>;

namespace BroadcasterNotice {

class HierarchyChanged : public BrokerNotice::StageNoticeImpl<HierarchyChanged> {
    public:
        HierarchyChanged(UnorderedSdfPathSet added, UnorderedSdfPathSet removed, UnorderedSdfPathSet modified,
                                ChangedFieldMap changedFields) : _added(std::move(added)), _removed(std::move(removed)), 
                                                    _modified(std::move(modified)), _changedFields(std::move(changedFields)){}
        
        virtual ~HierarchyChanged() = default;

        const UnorderedSdfPathSet& GetAdded() const {
            return _added;
        }
        const UnorderedSdfPathSet& GetRemoved() const {
            return _removed;
        }
        const UnorderedSdfPathSet& GetModified() const {
            return _modified;
        }
        const ChangedFieldMap& GetChangedFields() const {
            return _changedFields;
        }

        virtual void Merge(HierarchyChanged&&) override;
    
    private:
        UnorderedSdfPathSet _added;
        UnorderedSdfPathSet _removed;
        UnorderedSdfPathSet _modified;
        ChangedFieldMap _changedFields;
};


} // namespace BroadcasterNotice
} // namespace unf

#endif // NOTICE_BROADCASTER_NOTICE_H
