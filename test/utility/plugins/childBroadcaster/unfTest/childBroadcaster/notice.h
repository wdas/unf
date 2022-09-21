#ifndef TEST_NOTICE_BROKER_PLUGIN_CHILD_BROADCASTER_NOTICE_H
#define TEST_NOTICE_BROKER_PLUGIN_CHILD_BROADCASTER_NOTICE_H

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
        UnorderedSdfPathSet added, UnorderedSdfPathSet removed,
        UnorderedSdfPathSet modified, ChangedFieldMap changedInfo)
        : _added(std::move(added)),
          _removed(std::move(removed)),
          _modified(std::move(modified)),
          _changedInfo(std::move(changedInfo))
    {
    }

    ~ChildBroadcasterNotice() = default;

    const UnorderedSdfPathSet& GetAdded() const { return _added; }
    const UnorderedSdfPathSet& GetRemoved() const { return _removed; }
    const UnorderedSdfPathSet& GetModified() const { return _modified; }
    const ChangedFieldMap& GetChangedFields() const { return _changedInfo; }

  private:
    UnorderedSdfPathSet _added;
    UnorderedSdfPathSet _removed;
    UnorderedSdfPathSet _modified;
    ChangedFieldMap _changedInfo;
};

}  // namespace Test

#endif  // TEST_NOTICE_BROKER_PLUGIN_CHILD_BROADCASTER_NOTICE_H
