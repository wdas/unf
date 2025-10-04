#include "unf/notice.h"

#include <pxr/base/tf/notice.h>
#include <pxr/pxr.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/notice.h>

#include <utility>

PXR_NAMESPACE_USING_DIRECTIVE

namespace unf {

namespace UnfNotice {

TF_REGISTRY_FUNCTION(TfType)
{
    TfType::Define<StageNotice, TfType::Bases<TfNotice> >();

    TfType::Define<StageContentsChanged, TfType::Bases<StageNotice> >();
    TfType::Define<StageEditTargetChanged, TfType::Bases<StageNotice> >();
    TfType::Define<ObjectsChanged, TfType::Bases<StageNotice> >();
    TfType::Define<LayerMutingChanged, TfType::Bases<StageNotice> >();
}

ObjectsChanged::ObjectsChanged(const UsdNotice::ObjectsChanged& notice)
{
    // TODO: Update Usd Notice to give easier access to fields.

    for (const auto& path : notice.GetResyncedPaths()) {
        _resyncChanges.push_back(path);

        auto tokens = notice.GetChangedFields(path);
        if (tokens.size() > 0) {
            _changedFields[path] = TfTokenSet(tokens.begin(), tokens.end());
        }
    }
    for (const auto& path : notice.GetChangedInfoOnlyPaths()) {
        _infoChanges.push_back(path);

        auto tokens = notice.GetChangedFields(path);
        if (tokens.size() > 0) {
            _changedFields[path] = TfTokenSet(tokens.begin(), tokens.end());
        }
    }
}

ObjectsChanged::ObjectsChanged(const ObjectsChanged& other)
    : _resyncChanges(other._resyncChanges),
      _infoChanges(other._infoChanges),
      _changedFields(other._changedFields)
{
}

ObjectsChanged& ObjectsChanged::operator=(const ObjectsChanged& other)
{
    ObjectsChanged copy(other);
    std::swap(_resyncChanges, copy._resyncChanges);
    std::swap(_infoChanges, copy._infoChanges);
    std::swap(_changedFields, copy._changedFields);
    return *this;
}

void ObjectsChanged::Merge(ObjectsChanged&& notice)
{
    // Update resyncChanges if necessary.
    for (auto& path : notice._resyncChanges) {
        const auto iter =
            std::find(_resyncChanges.begin(), _resyncChanges.end(), path);
        if (iter == _resyncChanges.end()) {
            _resyncChanges.emplace_back(std::move(path));
        }
    }

    // Update infoChanges if necessary.
    for (const auto& path : notice._infoChanges) {
        const SdfPath& primPath = path.GetPrimPath();

        // Skip if the path is already in resyncedPaths.
        {
            const auto it = std::find(
                _resyncChanges.begin(), _resyncChanges.end(), primPath);
            if (it != _resyncChanges.end()) continue;
        }

        // Skip if an ancestor of the path is already in resyncedPaths.
        bool ancestorResynced = false;
        for (const auto& ancestor : primPath.GetPrefixes()) {
            const auto it = std::find(
                _resyncChanges.begin(), _resyncChanges.end(), ancestor);
            if (it != _resyncChanges.end()) {
                goto continue_ancestorResynced;
            }
        }

        // Add infoChanges, when not already available
        {
            const auto it =
                std::find(_infoChanges.begin(), _infoChanges.end(), path);
            if (it == _infoChanges.end()) {
                _infoChanges.push_back(std::move(path));
            }
        }
    continue_ancestorResynced:;
    }

    // Update changeFields.
    for (auto const& entry : notice._changedFields) {
        auto const& path = entry.first;

        if (_changedFields.find(path) == _changedFields.end()) {
            _changedFields[path] = std::move(notice._changedFields[path]);
        }
        else {
            _changedFields[path].insert(
                notice._changedFields[path].begin(),
                notice._changedFields[path].end());
        }
    }
}

void ObjectsChanged::PostProcess()
{
    SdfPath::RemoveDescendentPaths(&_resyncChanges);
}

bool ObjectsChanged::ResyncedObject(const PXR_NS::UsdObject& object) const
{
    auto path = PXR_NS::SdfPathFindLongestPrefix(
        _resyncChanges.begin(), _resyncChanges.end(), object.GetPath());
    return path != _resyncChanges.end();
}

bool ObjectsChanged::ChangedInfoOnly(const PXR_NS::UsdObject& object) const
{
    auto path = PXR_NS::SdfPathFindLongestPrefix(
        _infoChanges.begin(), _infoChanges.end(), object.GetPath());
    return path != _infoChanges.end();
}

TfTokenSet ObjectsChanged::GetChangedFields(
    const PXR_NS::UsdObject& object) const
{
    return GetChangedFields(object.GetPath());
}

TfTokenSet ObjectsChanged::GetChangedFields(const PXR_NS::SdfPath& path) const
{
    if (HasChangedFields(path)) {
        return _changedFields.at(path);
    }
    return TfTokenSet();
}

bool ObjectsChanged::HasChangedFields(const UsdObject& object) const
{
    return HasChangedFields(object.GetPath());
}

bool ObjectsChanged::HasChangedFields(const SdfPath& path) const
{
    if (_changedFields.find(path) != _changedFields.end()) {
        return true;
    }

    return false;
}

LayerMutingChanged::LayerMutingChanged(
    const UsdNotice::LayerMutingChanged& notice)
{
    for (const auto& layer : notice.GetMutedLayers()) {
        _mutedLayers.push_back(layer);
    }

    for (const auto& layer : notice.GetUnmutedLayers()) {
        _unmutedLayers.push_back(layer);
    }
}

LayerMutingChanged::LayerMutingChanged(const LayerMutingChanged& other)
    : _mutedLayers(other._mutedLayers), _unmutedLayers(other._unmutedLayers)
{
}

LayerMutingChanged& LayerMutingChanged::operator=(
    const LayerMutingChanged& other)
{
    LayerMutingChanged copy(other);
    std::swap(_mutedLayers, copy._mutedLayers);
    std::swap(_unmutedLayers, copy._unmutedLayers);
    return *this;
}

void LayerMutingChanged::Merge(LayerMutingChanged&& notice)
{
    size_t mutedLayersSize = _mutedLayers.size();
    size_t unmutedLayersSize = _unmutedLayers.size();

    for (const auto& layer : notice._mutedLayers) {
        auto begin = _unmutedLayers.begin();
        auto end = begin + unmutedLayersSize;
        auto it = std::find(begin, end, layer);
        if (it != end) {
            _unmutedLayers.erase(it);
            unmutedLayersSize -= 1;
        }
        else {
            _mutedLayers.push_back(std::move(layer));
        }
    }

    for (const auto& layer : notice._unmutedLayers) {
        auto begin = _mutedLayers.begin();
        auto end = begin + mutedLayersSize;
        auto it = std::find(begin, end, layer);
        if (it != end) {
            _mutedLayers.erase(it);
            mutedLayersSize -= 1;
        }
        else {
            _unmutedLayers.push_back(std::move(layer));
        }
    }
}

}  // namespace UnfNotice

}  // namespace unf
