#ifndef NOTICE_BROKER_HIERARCHY_BROADCASTER_H
#define NOTICE_BROKER_HIERARCHY_BROADCASTER_H

#include "broadcaster.h"
#include "notice.h"
#include "hierarchycache.h"
#include "broadcasterNotice.h"
#include <pxr/pxr.h>
#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/type.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/usd/usd/common.h>

#include <string>
#include <vector>

PXR_NAMESPACE_OPEN_SCOPE

namespace unf {
using _StageNoticePtrList = std::vector<BrokerNotice::StageNoticeRefPtr>;   
using _StageNoticePtrMap = std::unordered_map<std::string, _StageNoticePtrList>;
using ObjectsChangedRefPtr = TfRefPtr<BrokerNotice::ObjectsChanged>;

class HierarchyBroadcaster : Broadcaster {
    public:
        HierarchyBroadcaster(const BrokerWeakPtr& broker) : Broadcaster(broker), _cache(broker->GetStage()){}

        std::string GetIdentifier() const override {return "hierarchy";}

        const UnorderedSdfPathSet& GetAdded() const {
            return _cache.GetAdded();
        }
        const UnorderedSdfPathSet& GetRemoved() const {
            return _cache.GetRemoved();
        }
        const UnorderedSdfPathSet& GetModified() const {
            return _cache.GetModified();
        }
        const ChangedFieldMap& GetChangedFields() const {
            return *_changedFields;
        }

        void Clear() {
            _changedFields = nullptr;
            _cache.Clear();
        }

        void Execute(void* parent) override{
            _StageNoticePtrMap& noticeMap = *static_cast<_StageNoticePtrMap*>(parent);
            _StageNoticePtrList objChangedNotices = noticeMap[BrokerNotice::ObjectsChanged::GetStaticTypeId()];

            assert(objChangedNotices.size() == 1);
            
            ObjectsChangedRefPtr notice = TfStatic_cast<ObjectsChangedRefPtr>(objChangedNotices[0]);
            _changedFields = &notice->GetChangedFieldMap();
            _cache.Update(notice->GetResyncedPaths());
            
            for(auto& c : _children) {
                c->Execute(this);
            }
            _broker->Send<BroadcasterNotice::ChangeSummaryNotice>(_cache.TakeAdded(), _cache.TakeRemoved(),
                            _cache.TakeModified(), *_changedFields);
            
            Clear();
    }
    
    private:
        void _AddChild(TfRefPtr<const Broadcaster>);
        std::vector<TfRefPtr<Broadcaster>> _children;
        BrokerWeakPtr _broker;
        HierarchyCache _cache;
        const ChangedFieldMap* _changedFields;
};
}// namespace unf

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROKER_HIERARCHY_BROADCASTER_H
