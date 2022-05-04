#ifndef NOTICE_BROKER_NOTICE_H
#define NOTICE_BROKER_NOTICE_H

#include "pxr/pxr.h"
#include "pxr/base/tf/notice.h"
#include "pxr/usd/usd/notice.h"

#include <memory>
#include <vector>

PXR_NAMESPACE_OPEN_SCOPE

namespace UsdBrokerNotice {

class StageNotice;

using StageNoticePtr = std::shared_ptr<StageNotice>;
using StageNoticePtrList = std::vector<StageNoticePtr>;

class StageNotice 
: public TfNotice
, public std::enable_shared_from_this<StageNotice> {
public:
    virtual ~StageNotice() {}

    virtual bool IsMergeable() const { return true; }
    virtual void Merge(StageNotice&&) {}

    std::shared_ptr<const StageNotice> GetReference() const
    {
        return shared_from_this();
    }

protected:
    StageNotice() {}
};

class StageContentsChanged : public StageNotice {
public:
    explicit StageContentsChanged(
        const UsdNotice::StageContentsChanged&) {}
    virtual ~StageContentsChanged() {}
};

class ObjectsChanged : public StageNotice {
public:
    explicit ObjectsChanged(const UsdNotice::ObjectsChanged&);
    virtual ~ObjectsChanged() {}

    virtual void Merge(StageNotice&&);
};

class StageEditTargetChanged : public StageNotice {
public:
    explicit StageEditTargetChanged(
        const UsdNotice::StageEditTargetChanged&) {}
    virtual ~StageEditTargetChanged() {}
};

class LayerMutingChanged : public StageNotice {
public:
    explicit LayerMutingChanged(const UsdNotice::LayerMutingChanged&);
    virtual ~LayerMutingChanged() {}

    virtual void Merge(StageNotice&&);
};

} // namespace UsdBrokerNotice

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROKER_NOTICE_H
