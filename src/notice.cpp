#include "notice.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/notice.h>
#include <pxr/usd/usd/notice.h>

#include <iostream>
#include <utility>

PXR_NAMESPACE_OPEN_SCOPE

namespace UsdBrokerNotice {

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

    for (const auto& path: notice.GetResyncedPaths()) {
        _resyncChanges.push_back(path);
    }
    for (const auto& path: notice.GetChangedInfoOnlyPaths()) {
        _infoChanges.push_back(path);
    }
}

ObjectsChanged::ObjectsChanged(const ObjectsChanged& other)
    : _resyncChanges(other._resyncChanges)
    , _infoChanges(other._infoChanges)
{

}

ObjectsChanged& ObjectsChanged::operator=(const ObjectsChanged& other)
{
    ObjectsChanged copy(other);
    std::swap(_resyncChanges, copy._resyncChanges);
    std::swap(_infoChanges, copy._infoChanges);
    return *this;
}

void ObjectsChanged::Merge(ObjectsChanged&& notice)
{
    size_t resyncChangesSize = _resyncChanges.size();

    for (const auto& path: notice._resyncChanges) {
        auto begin = _resyncChanges.begin();
        auto end = begin + resyncChangesSize;
        auto it = std::find(begin, end, path);
        if (it == end) {
            _resyncChanges.push_back(std::move(path));
        }
    }

    size_t infoChangesSize = _infoChanges.size();

    for (const auto& path: notice._infoChanges) {
        auto begin = _infoChanges.begin();
        auto end = begin + infoChangesSize;
        auto it = std::find(begin, end, path);
        if (it == end) {
            _infoChanges.push_back(std::move(path));
        }
    }
}

LayerMutingChanged::LayerMutingChanged(
    const UsdNotice::LayerMutingChanged& notice)
{
    for (const auto& layer: notice.GetMutedLayers()) {
        _mutedLayers.push_back(layer);
    }

    for (const auto& layer: notice.GetUnmutedLayers()) {
        _unmutedLayers.push_back(layer);
    }
}

LayerMutingChanged::LayerMutingChanged(const LayerMutingChanged& other)
    : _mutedLayers(other._mutedLayers)
    , _unmutedLayers(other._unmutedLayers)
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

    for (const auto& layer: notice._mutedLayers) {
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

    for (const auto& layer: notice._unmutedLayers) {
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

} // namespace UsdBrokerNotice

PXR_NAMESPACE_CLOSE_SCOPE
