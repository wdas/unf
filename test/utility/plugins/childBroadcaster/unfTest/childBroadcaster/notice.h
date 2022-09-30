#ifndef TEST_USD_NOTICE_FRAMEWORK_PLUGIN_CHILD_BROADCASTER_NOTICE_H
#define TEST_USD_NOTICE_FRAMEWORK_PLUGIN_CHILD_BROADCASTER_NOTICE_H

#include <unf/hierarchyBroadcaster/notice.h>

#include <pxr/pxr.h>

#include <string>
#include <unordered_map>
#include <utility>

namespace Test {

using ChangedFieldMap = unf::ChangedFieldMap;
using UnorderedSdfPathSet =
    std::unordered_set<PXR_NS::SdfPath, PXR_NS::SdfPath::Hash>;

class ChildBroadcasterNotice : public PXR_NS::TfNotice {
  public:
    ChildBroadcasterNotice(
        PXR_NS::SdfPathVector added, PXR_NS::SdfPathVector removed,
        PXR_NS::SdfPathVector modified, ChangedFieldMap changedInfo)
        : _added(std::move(added)),
          _removed(std::move(removed)),
          _modified(std::move(modified)),
          _changedInfo(std::move(changedInfo))
    {
    }

    ~ChildBroadcasterNotice() = default;

    const PXR_NS::SdfPathVector& GetAdded() const { return _added; }
    const PXR_NS::SdfPathVector& GetRemoved() const { return _removed; }
    const PXR_NS::SdfPathVector& GetModified() const { return _modified; }
    const ChangedFieldMap& GetChangedFields() const { return _changedInfo; }

  private:
    PXR_NS::SdfPathVector _added;
    PXR_NS::SdfPathVector _removed;
    PXR_NS::SdfPathVector _modified;
    ChangedFieldMap _changedInfo;
};

}  // namespace Test

#endif  // TEST_USD_NOTICE_FRAMEWORK_PLUGIN_CHILD_BROADCASTER_NOTICE_H
