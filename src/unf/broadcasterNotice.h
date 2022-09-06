#ifndef NOTICE_BROADCASTER_NOTICE_H
#define NOTICE_BROADCASTER_NOTICE_H

#include "notice.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/notice.h>
#include <pxr/usd/sdf/path.h>

PXR_NAMESPACE_OPEN_SCOPE

namespace unf {

using UnorderedSdfPathSet = std::unordered_set<SdfPath, SdfPath::Hash>;

namespace BroadcasterNotice {

class ChangeSummary : public BrokerNotice::StageNoticeImpl<ChangeSummary> {
    public:
        ChangeSummary(UnorderedSdfPathSet added, UnorderedSdfPathSet removed, UnorderedSdfPathSet modified,
                                ChangedFieldMap changedFields) : _added(std::move(added)), _removed(std::move(removed)), 
                                                    _modified(std::move(modified)), _changedFields(std::move(changedFields)){}
        
        virtual ~ChangeSummary() = default;

        const UnorderedSdfPathSet& GetAdded() const{
            return _added;
        }
        const UnorderedSdfPathSet& GetRemoved() const{
            return _removed;
        }
        const UnorderedSdfPathSet& GetModified() const{
            return _modified;
        }
        const ChangedFieldMap& GetChangedFields() const {
            return _changedFields;
        }

        virtual void Merge(ChangeSummary&&) override;
    
    private:
        UnorderedSdfPathSet _added;
        UnorderedSdfPathSet _removed;
        UnorderedSdfPathSet _modified;
        ChangedFieldMap _changedFields;
};


} // namespace BroadcasterNotice
} // namespace unf

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROADCASTER_NOTICE_H
