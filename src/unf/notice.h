#ifndef USD_NOTICE_FRAMEWORK_NOTICE_H
#define USD_NOTICE_FRAMEWORK_NOTICE_H

/// \file unf/notice.h

#include <pxr/base/arch/demangle.h>
#include <pxr/base/tf/notice.h>
#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/pxr.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/notice.h>

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace unf {

/// Convenient alias for set of tokens.
using TfTokenSet =
    std::unordered_set<PXR_NS::TfToken, PXR_NS::TfToken::HashFunctor>;

/// Convenient alias for map of token sets organized per path.
using ChangedFieldMap =
    std::unordered_map<PXR_NS::SdfPath, TfTokenSet, PXR_NS::SdfPath::Hash>;

namespace UnfNotice {

/// \class StageNotice
///
/// \brief
/// Base class for autonomous PXR_NS::UsdStage notices.
///
/// This notice type is the autonomous equivalent of the
/// PXR_NS::UsdNotice::StageNotice notice type.
///
/// \note
/// This class is not abstact so that a corresponding Python type can be
/// created.
class StageNotice : public PXR_NS::TfNotice, public PXR_NS::TfRefBase {
  public:
    virtual ~StageNotice() = default;

    /// \brief
    /// Indicate whether notice can be merged with other StageNotice notices.
    ///
    /// By default, this method return true.
    ///
    /// \sa
    /// Merge
    virtual bool IsMergeable() const { return true; }

    virtual void PostProcess(){};

    /// \brief
    /// Interface method for merging StageNotice.
    ///
    /// \warning
    /// This method should be considered as pure virtual.
    virtual void Merge(StageNotice&&) {
        PXR_NAMESPACE_USING_DIRECTIVE
        TF_FATAL_ERROR("Abstract class 'StageNotice' cannot be merged.");
    }

    /// \brief
    /// Interface method for returing unique type identifier.
    ///
    /// \warning
    /// This method should be considered as pure virtual.
    virtual std::string GetTypeId() const {
        PXR_NAMESPACE_USING_DIRECTIVE
        TF_FATAL_ERROR(
          "Abstract class 'StageNotice' does not have a unique identifier.");
        return "";
    }

    /// \brief
    /// Interface method to return a copy of the notice.
    ///
    /// \warning
    /// This method should be considered as pure virtual.
    PXR_NS::TfRefPtr<StageNotice> Clone() const
    {
        return PXR_NS::TfCreateRefPtr(_Clone());
    }

  protected:
    StageNotice() = default;

  private:
    /// \brief
    /// Interface to return a raw pointer to a copy of the notice.
    ///
    /// \note
    /// Intermediate method needed as covariant return type is not possible
    /// with PXR_NS::TfRefPtr.
    ///
    /// \warning
    /// This method should be considered as pure virtual.
    virtual StageNotice* _Clone() const {
        PXR_NAMESPACE_USING_DIRECTIVE
        TF_FATAL_ERROR("Abstract class 'StageNotice' cannot be cloned.");
        return nullptr;
    }
};

/// Convenient alias for StageNotice reference pointer
using StageNoticeRefPtr = PXR_NS::TfRefPtr<StageNotice>;

/// Convenient alias for StageNotice weak pointer
using StageNoticeWeakPtr = PXR_NS::TfWeakPtr<StageNotice>;

/// \class StageNoticeImpl
///
/// \brief
/// Intermediate interface using the
/// [CRTP](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)
/// idiom to provide factory and default merging logic.
///
/// Typical usage is as follows:
///
/// \code{.cpp}
/// class MyNotice
///     : public unf::UnfNotice::StageNoticeImpl<MyNotice> {
///   public:
///     virtual ~MyNotice() = default;
/// };
/// \endcode
template <class Self>
class StageNoticeImpl : public StageNotice {
  public:
    virtual ~StageNoticeImpl() = default;

    /// Create a notice with variadic arguments.
    template <class... Args>
    static PXR_NS::TfRefPtr<Self> Create(Args&&... args)
    {
        return PXR_NS::TfCreateRefPtr(new Self(std::forward<Args>(args)...));
    }

    /// Return a copy of the notice.
    PXR_NS::TfRefPtr<Self> Clone() const
    {
        return PXR_NS::TfCreateRefPtr(static_cast<Self*>(_Clone()));
    }

    virtual void Merge(StageNotice&& notice) override
    {
        Merge(dynamic_cast<Self&&>(notice));
    }

    /// \brief
    /// Base method for merging notice with similar type.
    ///
    /// By default, no data is moved.
    virtual void Merge(Self&&) {}

    /// \brief
    /// Base method for returing unique type identifier.
    ///
    /// By default, the full type name of the notice is returned.
    virtual std::string GetTypeId() const
    {
        return PXR_NS::ArchGetDemangled(typeid(Self).name());
    }

  private:
    /// \brief
    /// Return a raw pointer to a copy of the notice.
    ///
    /// \note
    /// Intermediate method needed as covariant return type is not possible
    /// with PXR_NS::TfRefPtr.
    virtual StageNotice* _Clone() const {
        return new Self(static_cast<const Self&>(*this));
    }
};

/// \class StageContentsChanged
///
/// \brief
/// Notice sent when the given PXR_NS::UsdStage's contents have changed in
/// any way.
///
/// This notice type is the autonomous equivalent of the
/// PXR_NS::UsdNotice::StageContentsChanged notice type.
class StageContentsChanged : public StageNoticeImpl<StageContentsChanged> {
  public:
    virtual ~StageContentsChanged() = default;

  protected:
    /// Create notice from PXR_NS::UsdNotice::StageContentsChanged instance.
    explicit StageContentsChanged(
        const PXR_NS::UsdNotice::StageContentsChanged&)
    {
    }

    /// Ensure that StageNoticeImpl::Create method can call constructor.
    friend StageNoticeImpl<StageContentsChanged>;
};

/// \class ObjectsChanged
///
/// \brief
/// Notice sent in response to authored changes that affect any
/// PXR_NS::UsdObject.
///
/// This notice type is the autonomous equivalent of the
/// PXR_NS::UsdNotice::ObjectsChanged notice type.
class ObjectsChanged : public StageNoticeImpl<ObjectsChanged> {
  public:
    virtual ~ObjectsChanged() = default;

    /// Copy constructor.
    ObjectsChanged(const ObjectsChanged&);

    /// Assignment operator.
    ObjectsChanged& operator=(const ObjectsChanged&);

    /// \brief
    /// Merge notice with another ObjectsChanged notice.
    ///
    /// \note
    /// Data will be move out of incoming ObjectsChanged notice.
    virtual void Merge(ObjectsChanged&&) override;
    virtual void PostProcess() override;

    /// \brief
    /// Indicate whether \p object was affected by the change that generated
    /// this notice.
    ///
    /// \note
    /// Equivalent from PXR_NS::UsdNotice::ObjectsChanged::AffectedObject
    bool AffectedObject(const PXR_NS::UsdObject& object) const
    {
        return ResyncedObject(object) || ChangedInfoOnly(object);
    }

    /// \brief
    /// Indicate whether \p object was resynced by the change that generated
    /// this notice.
    ///
    /// \note
    /// Equivalent from PXR_NS::UsdNotice::ObjectsChanged::ResyncedObject
    bool ResyncedObject(const PXR_NS::UsdObject&) const;

    /// \brief
    /// Indicate whether \p object was modified but not resynced by the change
    /// that generated this notice.
    ///
    /// \note
    /// Equivalent from PXR_NS::UsdNotice::ObjectsChanged::ChangedInfoOnly
    bool ChangedInfoOnly(const PXR_NS::UsdObject&) const;

    /// \brief
    /// Return vector of paths that are resynced in lexicographical order.
    ///
    /// \note
    /// Equivalent from PXR_NS::UsdNotice::ObjectsChanged::GetResyncedPaths
    const PXR_NS::SdfPathVector& GetResyncedPaths() const
    {
        return _resyncChanges;
    }

    /// \brief
    /// Return vector of paths that are modified but not resynced in
    /// lexicographical order.
    ///
    /// \note
    /// Equivalent from
    /// PXR_NS::UsdNotice::ObjectsChanged::GetChangedInfoOnlyPaths
    const PXR_NS::SdfPathVector& GetChangedInfoOnlyPaths() const
    {
        return _infoChanges;
    }

    /// \brief
    /// Return the set of changed fields in layers that affected the \p object.
    ///
    /// \note
    /// Equivalent from
    /// PXR_NS::UsdNotice::ObjectsChanged::GetChangedFields(const UsdObject&) const
    TfTokenSet GetChangedFields(const PXR_NS::UsdObject&) const;

    /// \brief
    /// Return the set of changed fields in layers that affected the \p path.
    ///
    /// \note
    /// Equivalent from
    /// PXR_NS::UsdNotice::ObjectsChanged::GetChangedFields(const SdfPath&) const
    TfTokenSet GetChangedFields(const PXR_NS::SdfPath&) const;

    /// \brief
    /// Indicate whether any changed fields affected the \p object.
    ///
    /// \note
    /// Equivalent from
    /// PXR_NS::UsdNotice::ObjectsChanged::HasChangedFields(const UsdObject&) const
    bool HasChangedFields(const PXR_NS::UsdObject&) const;

    /// \brief
    /// Indicate whether any changed fields affected the \p path.
    ///
    /// \note
    /// Equivalent from
    /// PXR_NS::UsdNotice::ObjectsChanged::HasChangedFields(const SdfPath&) const
    bool HasChangedFields(const PXR_NS::SdfPath&) const;

    /// \brief
    /// Return map of affected token sets organized per path.
    const ChangedFieldMap& GetChangedFieldMap() const { return _changedFields; }

  protected:
      /// Create notice from PXR_NS::UsdNotice::ObjectsChanged instance.
    explicit ObjectsChanged(const PXR_NS::UsdNotice::ObjectsChanged&);

    /// Ensure that StageNoticeImpl::Create method can call constructor.
    friend StageNoticeImpl<ObjectsChanged>;

  private:
      /// List of resynced paths.
    PXR_NS::SdfPathVector _resyncChanges;

    /// List of paths which are modified but not resynced.
    PXR_NS::SdfPathVector _infoChanges;

    /// Map of affected token sets organized per path.
    ChangedFieldMap _changedFields;
};

/// \class StageEditTargetChanged
///
/// \brief
/// Notice sent when a stage's EditTarget has changed.
///
/// This notice type is the autonomous equivalent of the
/// PXR_NS::UsdNotice::StageEditTargetChanged notice type.
class StageEditTargetChanged : public StageNoticeImpl<StageEditTargetChanged> {
  public:
    virtual ~StageEditTargetChanged() = default;

  protected:
    /// Create notice from PXR_NS::UsdNotice::StageEditTargetChanged instance.
    explicit StageEditTargetChanged(
        const PXR_NS::UsdNotice::StageEditTargetChanged&)
    {
    }

    /// Ensure that StageNoticeImpl::Create method can call constructor.
    friend StageNoticeImpl<StageEditTargetChanged>;
};

/// \class LayerMutingChanged
///
/// \brief
/// Notice sent after a set of layers have been newly muted or unmuted.
///
/// This notice type is the autonomous equivalent of the
/// PXR_NS::UsdNotice::LayerMutingChanged notice type.
class LayerMutingChanged : public StageNoticeImpl<LayerMutingChanged> {
  public:
    virtual ~LayerMutingChanged() = default;

    /// Copy constructor.
    LayerMutingChanged(const LayerMutingChanged&);

    /// Assignment operator.
    LayerMutingChanged& operator=(const LayerMutingChanged&);

    /// \brief
    /// Merge notice with another LayerMutingChanged notice.
    ///
    /// \note
    /// Data will be move out of incoming LayerMutingChanged notice.
    virtual void Merge(LayerMutingChanged&&) override;

    /// \brief
    /// Returns identifiers of the layers that were muted.
    ///
    /// \note
    /// Equivalent from
    /// PXR_NS::UsdNotice::LayerMutingChanged::GetMutedLayers
    const std::vector<std::string>& GetMutedLayers() const
    {
        return _mutedLayers;
    }

    /// \brief
    /// Returns identifiers of the layers that were unmuted.
    ///
    /// \note
    /// Equivalent from
    /// PXR_NS::UsdNotice::LayerMutingChanged::GetUnmutedLayers
    const std::vector<std::string>& GetUnmutedLayers() const
    {
        return _unmutedLayers;
    }

  protected:
    /// Create notice from PXR_NS::UsdNotice::LayerMutingChanged instance.
    explicit LayerMutingChanged(const PXR_NS::UsdNotice::LayerMutingChanged&);

    /// Ensure that StageNoticeImpl::Create method can call constructor.
    friend StageNoticeImpl<LayerMutingChanged>;

  private:
    /// List of layer identifiers that were muted.
    std::vector<std::string> _mutedLayers;

    /// List of layer identifiers that were unmuted.
    std::vector<std::string> _unmutedLayers;
};

}  // namespace UnfNotice

}  // namespace unf

#endif  // USD_NOTICE_FRAMEWORK_NOTICE_H
