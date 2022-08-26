#ifndef NOTICE_BROKER_NOTICE_H
#define NOTICE_BROKER_NOTICE_H

#include <pxr/pxr.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/notice.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/notice.h>

#include <map>
#include <string>
#include <vector>

namespace unf {

namespace BrokerNotice {

class StageNotice : public PXR_NS::TfNotice, public PXR_NS::TfRefBase {
public:
    virtual ~StageNotice() = default;

    StageNotice(const StageNotice &) = default;
    StageNotice &operator=(const StageNotice &) = default;

    // TODO: Should those methods be pure virtual?
    virtual bool IsMergeable() const { return true; }
    virtual void Merge(StageNotice&&) {};
    virtual std::string GetTypeId() const {return "";}

    // Exposes the Copy function to the interface
    virtual PXR_NS::TfRefPtr<StageNotice> CopyAsStageNotice() const
    {
        return nullptr;
    }

protected:
    StageNotice() = default;
};

using StageNoticeRefPtr = PXR_NS::TfRefPtr<StageNotice>;
using StageNoticeWeakPtr = PXR_NS::TfWeakPtr<StageNotice>;

template<class Self>
class StageNoticeImpl : public StageNotice {
public:
    template <class... Args>
    static PXR_NS::TfRefPtr<Self> Create(Args&&... args)
    {
        return PXR_NS::TfCreateRefPtr(new Self(std::forward<Args>(args)...));
    }

    virtual PXR_NS::TfRefPtr<StageNotice> CopyAsStageNotice() const override
    {
        return Copy();
    }

    PXR_NS::TfRefPtr<Self> Copy() const
    {
        return PXR_NS::TfCreateRefPtr(
            new Self(static_cast<const Self&>(*this)));
    }

    virtual void Merge(StageNotice&& notice) override
    {
        Merge(dynamic_cast<Self&&>(notice));
    }

    virtual void Merge(Self&&) {}

    virtual std::string GetTypeId() const {
        return typeid(Self).name();
    }
};

class StageContentsChanged : public StageNoticeImpl<StageContentsChanged> {
protected:
    explicit StageContentsChanged(
        const PXR_NS::UsdNotice::StageContentsChanged&) {}

    friend StageNoticeImpl<StageContentsChanged>;
};

class ObjectsChanged : public StageNoticeImpl<ObjectsChanged> {
public:
    ObjectsChanged(const ObjectsChanged&);
    ObjectsChanged &operator=(const ObjectsChanged&);

    virtual void Merge(ObjectsChanged&&) override;

    const std::vector<PXR_NS::SdfPath>& GetResyncedPaths() const
    {
        return _resyncChanges;
    }

    const std::vector<PXR_NS::SdfPath>& GetChangedInfoOnlyPaths() const
    {
        return _infoChanges;
    }

protected:
    explicit ObjectsChanged(const PXR_NS::UsdNotice::ObjectsChanged&);

private:
    std::vector<PXR_NS::SdfPath> _resyncChanges;
    std::vector<PXR_NS::SdfPath> _infoChanges;

    friend StageNoticeImpl<ObjectsChanged>;
};

class StageEditTargetChanged : public StageNoticeImpl<StageEditTargetChanged> {
protected:
    explicit StageEditTargetChanged(
        const PXR_NS::UsdNotice::StageEditTargetChanged&) {}

    friend StageNoticeImpl<StageEditTargetChanged>;
};

class LayerMutingChanged : public StageNoticeImpl<LayerMutingChanged> {
public:
    LayerMutingChanged(const LayerMutingChanged&);
    LayerMutingChanged &operator=(const LayerMutingChanged&);

    virtual void Merge(LayerMutingChanged&&) override;

    const std::vector<std::string>& GetMutedLayers() const
    {
        return _mutedLayers;
    }

    const std::vector<std::string>& GetUnmutedLayers() const
    {
        return _unmutedLayers;
    }

protected:
    explicit LayerMutingChanged(const PXR_NS::UsdNotice::LayerMutingChanged&);

private:
    std::vector<std::string> _mutedLayers;
    std::vector<std::string> _unmutedLayers;

    friend StageNoticeImpl<LayerMutingChanged>;
};

} // namespace BrokerNotice

} // namespace unf

#endif // NOTICE_BROKER_NOTICE_H
