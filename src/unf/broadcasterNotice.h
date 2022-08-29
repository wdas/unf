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
namespace BroadcasterNotice {

class ChangeSummaryNotice : public BrokerNotice::StageNoticeImpl<ChangeSummaryNotice> {
    public:
        ChangeSummaryNotice(UnorderedSdfPathSet added, UnorderedSdfPathSet removed, UnorderedSdfPathSet modified,
                                ChangedFieldMap changedFields) : _added(std::move(added)), _removed(std::move(removed)), 
                                                    _modified(std::move(modified)), _changedFields(std::move(_changedFields)){}
        
        virtual ~ChangeSummaryNotice() = default;

        const UnorderedSdfPathSet& GetAdded() {
            return _added;
        }
        const UnorderedSdfPathSet& GetRemoved() {
            return _removed;
        }
        const UnorderedSdfPathSet& GetModified() {
            return _modified;
        }
        const ChangedFieldMap& GetChangedFields() const {
            return _changedFields;
        }

        virtual void Merge(ChangeSummaryNotice&&) override;
    
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
