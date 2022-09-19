#ifndef TEST_NOTICE_BROKER_NOTICE_H
#define TEST_NOTICE_BROKER_NOTICE_H

#include <unf/notice.h>

#include <pxr/pxr.h>

#include <string>
#include <unordered_map>
#include <utility>

namespace Test {

using ChangedFieldMap = unf::ChangedFieldMap;
using DataMap = std::unordered_map<std::string, std::string>;
using UnorderedSdfPathSet =
    std::unordered_set<PXR_NS::SdfPath, PXR_NS::SdfPath::Hash>;

// Notice with can be consolidated within broker transactions.
class MergeableNotice
    : public unf::BrokerNotice::StageNoticeImpl<MergeableNotice> {
  public:
    MergeableNotice() = default;
    MergeableNotice(const DataMap& data) : _data(data) {}

    MergeableNotice(const MergeableNotice& other) : _data(other._data) {}

    MergeableNotice& operator=(const MergeableNotice& other)
    {
        MergeableNotice copy(other);
        std::swap(_data, copy._data);
        return *this;
    }

    virtual ~MergeableNotice() = default;

    virtual void Merge(MergeableNotice&& notice) override
    {
        for (const auto& it : notice.GetData()) {
            _data[it.first] = std::move(it.second);
        }
    }

    const DataMap& GetData() const { return _data; }

  private:
    DataMap _data;
};

// Notice with can not be consolidated within broker transactions.
class UnMergeableNotice
    : public unf::BrokerNotice::StageNoticeImpl<UnMergeableNotice> {
  public:
    UnMergeableNotice() = default;
    virtual ~UnMergeableNotice() = default;

    virtual bool IsMergeable() const { return false; }
};

// Declare notices used by the test dispatchers.
class InputNotice : public PXR_NS::TfNotice {
};

class OutputNotice1 : public unf::BrokerNotice::StageNoticeImpl<OutputNotice1> {
  public:
    OutputNotice1(const InputNotice&) {}
};

class OutputNotice2 : public unf::BrokerNotice::StageNoticeImpl<OutputNotice2> {
  public:
    OutputNotice2(const InputNotice&) {}
};


// Declare notices used by the test broadcasters.
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

#endif  // TEST_NOTICE_BROKER_NOTICE_H
