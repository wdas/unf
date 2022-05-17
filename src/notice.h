#ifndef NOTICE_BROKER_NOTICE_H
#define NOTICE_BROKER_NOTICE_H

#include "pxr/pxr.h"
#include "pxr/base/tf/refPtr.h"
#include "pxr/base/tf/refBase.h"
#include "pxr/base/tf/notice.h"
#include "pxr/usd/usd/notice.h"

#include <vector>

PXR_NAMESPACE_OPEN_SCOPE

namespace UsdBrokerNotice {

class StageNotice;

using StageNoticePtr = TfRefPtr<StageNotice>;
using StageNoticePtrList = std::vector<StageNoticePtr>;

using StageNoticeConstPtr = TfRefPtr<const StageNotice>;
using StageNoticeConstPtrList = std::vector<StageNoticeConstPtr>;

class StageNotice : public TfNotice, public TfRefBase {
public:
    virtual ~StageNotice() = default;

    virtual bool IsMergeable() const { return true; }
    virtual void Merge(StageNotice&&) {};

protected:
    StageNotice() {}
};

template<class Self>
class StageNoticeImpl : public StageNotice {
public:
    template <class... Args>
    static TfRefPtr<Self> Create(Args&&... args) { 
        return TfCreateRefPtr(new Self(std::forward<Args>(args)...)); 
    }

    virtual void Merge(StageNotice&& notice) override {
        Merge(static_cast<Self&&>(notice));
    }

protected:
    virtual void Merge(Self&&) {}
};

class StageContentsChanged : public StageNoticeImpl<StageContentsChanged> {
protected:
    explicit StageContentsChanged(
        const UsdNotice::StageContentsChanged&) {}

    friend StageNoticeImpl<StageContentsChanged>;
};

class ObjectsChanged : public StageNoticeImpl<ObjectsChanged> {
protected:
    explicit ObjectsChanged(const UsdNotice::ObjectsChanged&);

    virtual void Merge(ObjectsChanged&&) override;

    friend StageNoticeImpl<ObjectsChanged>;
};

class StageEditTargetChanged : public StageNoticeImpl<StageEditTargetChanged> {
protected:
    explicit StageEditTargetChanged(
        const UsdNotice::StageEditTargetChanged&) {}

    friend StageNoticeImpl<StageEditTargetChanged>;

};

class LayerMutingChanged : public StageNoticeImpl<LayerMutingChanged> {
protected:
    explicit LayerMutingChanged(const UsdNotice::LayerMutingChanged&);

    virtual void Merge(LayerMutingChanged&&) override;

    friend StageNoticeImpl<LayerMutingChanged>;
};

} // namespace UsdBrokerNotice

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROKER_NOTICE_H
