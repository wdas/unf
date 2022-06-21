#ifndef NOTICE_BROKER_TEST_UTILITY_H
#define NOTICE_BROKER_TEST_UTILITY_H

#include "notice.h"

#include "pxr/pxr.h"
#include "pxr/base/tf/notice.h"
#include "pxr/base/tf/weakBase.h"
#include "pxr/usd/sdf/layer.h"
#include "pxr/usd/usd/common.h"
#include "pxr/usd/usd/stage.h"

#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <typeinfo>
#include <utility>
#include <vector>

namespace Test {

using DataMap = std::unordered_map<std::string, std::string>;

PXR_NS::UsdStageRefPtr CreateStageWithLayers()
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto rootLayer = stage->GetRootLayer();

    size_t layersNb = 3;

    std::vector<PXR_NS::SdfLayerRefPtr> layers;
    layers.reserve(layersNb);

    std::vector<std::string> identifiers;
    identifiers.reserve(layersNb);

    while (layers.size() < layersNb) {
        auto layer = PXR_NS::SdfLayer::CreateAnonymous(".usda");
        layers.push_back(layer);
        identifiers.push_back(layer->GetIdentifier());
    }

    rootLayer->SetSubLayerPaths(identifiers);

    return stage;
}

// Interface to examine content of notice received.
template <class T>
class ListenerBase : public PXR_NS::TfWeakBase
{
public:
    ListenerBase(const PXR_NS::UsdStageWeakPtr &stage) {
        auto self = PXR_NS::TfCreateWeakPtr(this);
        _key =  PXR_NS::TfNotice::Register(
            PXR_NS::TfCreateWeakPtr(this), 
            &ListenerBase::OnReceiving, 
            stage);
    }

    virtual ~ListenerBase() { PXR_NS::TfNotice::Revoke(_key); }

private:
    virtual void OnReceiving(const T&, const PXR_NS::UsdStageWeakPtr&) =0;

    PXR_NS::TfNotice::Key _key;
};

// Container to listen to several types of Tf notices.
template <class... Types>
class Listener : public PXR_NS::TfWeakBase
{
public:
    Listener(const PXR_NS::UsdStageWeakPtr &stage) {
        auto self = PXR_NS::TfCreateWeakPtr(this);
        _keys = std::unordered_map<std::string, PXR_NS::TfNotice::Key>({
            _Register<Types>(self, stage)...
        });
    }

    virtual ~Listener() {
        for (auto& element: _keys) {
            PXR_NS::TfNotice::Revoke(element.second);
        }
    }

    template <class T>
    size_t Received()
    {
        std::string name = typeid(T).name();
        if (_received.find(name) == _received.end())
            return 0;

        return _received.at(name);
    }

    void Reset()
    {
        for (auto& element: _received) {
            element.second = 0;
        }
    }

private:
    template<class T>
    std::pair<std::string, PXR_NS::TfNotice::Key> _Register(
        const PXR_NS::TfWeakPtr<Listener>& self, 
        const PXR_NS::UsdStageWeakPtr &stage)
    {
        auto cb = &Listener::_Callback<T>;
        std::string name = typeid(T).name();
        auto key =  PXR_NS::TfNotice::Register(self, cb, stage);

        return std::make_pair(name, key);
    }

    template<class T>
    void _Callback(const T& notice, const PXR_NS::UsdStageWeakPtr &sender)
    {
        std::string name = typeid(T).name();
        if (_received.find(name) == _received.end())
            _received[name] = 0;

        _received[name] += 1;
    }

    std::unordered_map<std::string, PXR_NS::TfNotice::Key> _keys;
    std::unordered_map<std::string, size_t> _received;
};

// Custom notice with can be consolidated within broker transactions.
class MergeableNotice 
: public PXR_NS::UsdBrokerNotice::StageNoticeImpl<MergeableNotice>
{
public:
    MergeableNotice(const DataMap& data)
    : _data(data) 
    {}

    MergeableNotice(const MergeableNotice& other)
    : _data(other._data)
    {}

    MergeableNotice& operator=(const MergeableNotice& other)
    {
        MergeableNotice copy(other);
        std::swap(_data, copy._data);
        return *this;
    }

    virtual ~MergeableNotice() = default;

    virtual void Merge(MergeableNotice&& notice) override
    {
        for (const auto& it: notice.GetData()) {
            _data[it.first] = std::move(it.second);
        }
    }

    const DataMap& GetData() const { return _data; }

private:
    DataMap _data;
};

// Custom notice with can not be consolidated within broker transactions.
class UnMergeableNotice 
: public PXR_NS::UsdBrokerNotice::StageNoticeImpl<UnMergeableNotice>
{
public:
    UnMergeableNotice() = default;
    virtual ~UnMergeableNotice() = default;

    virtual bool IsMergeable() const { return false; }
};

} // namespace Test

PXR_NAMESPACE_OPEN_SCOPE

TF_REGISTRY_FUNCTION(TfType)
{
    TfType::Define<
        ::Test::MergeableNotice,
        TfType::Bases<UsdBrokerNotice::StageNotice> >();

    TfType::Define<
        ::Test::UnMergeableNotice,
        TfType::Bases<UsdBrokerNotice::StageNotice> >();
}

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROKER_TEST_UTILITY_H
