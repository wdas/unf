#ifndef TEST_USD_NOTICE_FRAMEWORK_NOTICE_H
#define TEST_USD_NOTICE_FRAMEWORK_NOTICE_H

#include <unf/notice.h>

#include <pxr/pxr.h>

#include <string>
#include <unordered_map>
#include <utility>

namespace Test {

using DataMap = std::unordered_map<std::string, std::string>;

// Notice with can be consolidated within broker transactions.
class MergeableNotice
    : public unf::UnfNotice::StageNoticeImpl<MergeableNotice> {
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

    // Bring all Merge declarations from base class to prevent
    // overloaded-virtual warning.
    using unf::UnfNotice::StageNoticeImpl<MergeableNotice>::Merge;

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

// Notice which cannot be consolidated within broker transactions.
class UnMergeableNotice
    : public unf::UnfNotice::StageNoticeImpl<UnMergeableNotice> {
  public:
    UnMergeableNotice() = default;
    virtual ~UnMergeableNotice() = default;

    virtual bool IsMergeable() const { return false; }
};

// Declare notices used by the test dispatchers.
class InputNotice : public PXR_NS::TfNotice {};

class OutputNotice1 : public unf::UnfNotice::StageNoticeImpl<OutputNotice1> {
  public:
    OutputNotice1(const InputNotice&) {}
};

class OutputNotice2 : public unf::UnfNotice::StageNoticeImpl<OutputNotice2> {
  public:
    OutputNotice2(const InputNotice&) {}
};

}  // namespace Test

#endif  // TEST_USD_NOTICE_FRAMEWORK_NOTICE_H
