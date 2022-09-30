#include "broadcaster.h"

PXR_NAMESPACE_USING_DIRECTIVE

namespace unf {

void HierarchyBroadcaster::Execute(void* parent)
{
    _StageNoticePtrMap& noticeMap = *static_cast<_StageNoticePtrMap*>(parent);
    _StageNoticePtrList objChangedNotices =
        noticeMap[BrokerNotice::ObjectsChanged::GetStaticTypeId()];

    assert(objChangedNotices.size() <= 1);

    if (objChangedNotices.size() == 1) {
        ObjectsChangedRefPtr notice =
            PXR_NS::TfStatic_cast<ObjectsChangedRefPtr>(objChangedNotices[0]);
            
        notice->RemoveDescendants();
        _cache.Update(notice->GetResyncedPaths());

        if (notice->GetChangedInfoOnlyPaths().size() > 0
            || _cache.DidCacheChange()) {
            _changedFields = &notice->GetChangedFieldMap();

            for (auto& c : _children) {
                c->Execute(this);
            }
            
            _broker->Send<BroadcasterNotice::HierarchyChanged>(
                _cache.TakeAdded(),
                _cache.TakeRemoved(),
                _cache.TakeModified(),
                *_changedFields);
            

            Clear();
        }
    }
}

}  // namespace unf
