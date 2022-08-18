#include <unf/broker.h>
#include <unf/cache.h>

#include <unfTest/listener.h>
#include <unfTest/testNotice.h>

#include <gtest/gtest.h>
#include <pxr/usd/sdf/layer.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usd/stage.h>

#include <string>
#include <typeinfo>

// namespace aliases for convenience.
using _USD = PXR_NS::UsdNotice;
namespace _Broker = PXR_NS::unf::BrokerNotice;

class MuteLayersTest : public ::testing::Test {
protected:
    using UsdListener = ::Test::Listener<
        _USD::StageNotice,
        _USD::StageContentsChanged,
        _USD::ObjectsChanged,
        _USD::StageEditTargetChanged,
        _USD::LayerMutingChanged
    >;

    using BrokerListener = ::Test::Listener<
        _Broker::StageNotice,
        _Broker::StageContentsChanged,
        _Broker::ObjectsChanged,
        _Broker::StageEditTargetChanged,
        _Broker::LayerMutingChanged
    >;

    void SetUp() override {
        _stage = PXR_NS::UsdStage::CreateInMemory();
        _AddLayers(3);

        _usdListener.SetStage(_stage);
        _brokerListener.SetStage(_stage);
    }

    void _AddLayers(int number) {
        _layers.reserve(number);
        _layerIds.reserve(number);

        while (_layers.size() < number) {
            auto layer = PXR_NS::SdfLayer::CreateAnonymous(".usda");
            _layers.push_back(layer);
            _layerIds.push_back(layer->GetIdentifier());
        }

        _stage->GetRootLayer()->SetSubLayerPaths(_layerIds);

    }

    PXR_NS::UsdStageRefPtr _stage;
    std::vector<PXR_NS::SdfLayerRefPtr> _layers;
    std::vector<std::string> _layerIds;

    UsdListener _usdListener;
    BrokerListener _brokerListener;
};

TEST_F(MuteLayersTest, Simple)
{
    auto broker = PXR_NS::unf::Broker::Create(_stage);

    _stage->MuteLayer(_layerIds[0]);
    _stage->MuteLayer(_layerIds[1]);
    _stage->UnmuteLayer(_layerIds[1]);
    _stage->MuteAndUnmuteLayers(
        std::vector<std::string>{_layerIds[2], _layerIds[1]},
        std::vector<std::string>{});

    // Ensure that similar notices are received via the stage and the broker.
    ASSERT_EQ(_usdListener.Received<_USD::StageNotice>(), 12);
    ASSERT_EQ(_usdListener.Received<_USD::StageContentsChanged>(), 4);
    ASSERT_EQ(_usdListener.Received<_USD::ObjectsChanged>(), 4);
    ASSERT_EQ(_usdListener.Received<_USD::StageEditTargetChanged>(), 0);
    ASSERT_EQ(_usdListener.Received<_USD::LayerMutingChanged>(), 4);

    ASSERT_EQ(_brokerListener.Received<_Broker::StageNotice>(), 12);
    ASSERT_EQ(_brokerListener.Received<_Broker::StageContentsChanged>(), 4);
    ASSERT_EQ(_brokerListener.Received<_Broker::ObjectsChanged>(), 4);
    ASSERT_EQ(_brokerListener.Received<_Broker::StageEditTargetChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::LayerMutingChanged>(), 4);
}

TEST_F(MuteLayersTest, Batching)
{
    auto broker = PXR_NS::unf::Broker::Create(_stage);

    broker->BeginTransaction();

    _stage->MuteLayer(_layerIds[0]);
    _stage->MuteLayer(_layerIds[1]);
    _stage->UnmuteLayer(_layerIds[1]);
    _stage->MuteAndUnmuteLayers(
        std::vector<std::string>{_layerIds[2], _layerIds[1]},
        std::vector<std::string>{});

    // Ensure that USD Notices are being sent as expected.
    ASSERT_EQ(_usdListener.Received<_USD::StageNotice>(), 12);
    ASSERT_EQ(_usdListener.Received<_USD::StageContentsChanged>(), 4);
    ASSERT_EQ(_usdListener.Received<_USD::ObjectsChanged>(), 4);
    ASSERT_EQ(_usdListener.Received<_USD::StageEditTargetChanged>(), 0);
    ASSERT_EQ(_usdListener.Received<_USD::LayerMutingChanged>(), 4);

    // While broker notices are blocked during a transaction.
    ASSERT_EQ(_brokerListener.Received<_Broker::StageNotice>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::StageContentsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::ObjectsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::StageEditTargetChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::LayerMutingChanged>(), 0);

    broker->EndTransaction();

    // Ensure that consolidated broker notices are sent after a transaction.
    ASSERT_EQ(_brokerListener.Received<_Broker::StageNotice>(), 3);
    ASSERT_EQ(_brokerListener.Received<_Broker::StageContentsChanged>(), 1);
    ASSERT_EQ(_brokerListener.Received<_Broker::ObjectsChanged>(), 1);
    ASSERT_EQ(_brokerListener.Received<_Broker::StageEditTargetChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::LayerMutingChanged>(), 1);
}

TEST_F(MuteLayersTest, Blocking)
{
    auto broker = PXR_NS::unf::Broker::Create(_stage);

    // Pass a predicate to block all broker notices.
    broker->BeginTransaction(
        [](const _Broker::StageNotice &){ return false; });

    _stage->MuteLayer(_layerIds[0]);
    _stage->MuteLayer(_layerIds[1]);
    _stage->UnmuteLayer(_layerIds[1]);
    _stage->MuteAndUnmuteLayers(
        std::vector<std::string>{_layerIds[2], _layerIds[1]},
        std::vector<std::string>{});

    // Ensure that USD Notices are being sent as expected.
    ASSERT_EQ(_usdListener.Received<_USD::StageNotice>(), 12);
    ASSERT_EQ(_usdListener.Received<_USD::StageContentsChanged>(), 4);
    ASSERT_EQ(_usdListener.Received<_USD::ObjectsChanged>(), 4);
    ASSERT_EQ(_usdListener.Received<_USD::StageEditTargetChanged>(), 0);
    ASSERT_EQ(_usdListener.Received<_USD::LayerMutingChanged>(), 4);

    // While broker notices are blocked during a transaction.
    ASSERT_EQ(_brokerListener.Received<_Broker::StageNotice>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::StageContentsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::ObjectsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::StageEditTargetChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::LayerMutingChanged>(), 0);

    broker->EndTransaction();

    // Ensure that no broker notices are sent after a transaction either.
    ASSERT_EQ(_brokerListener.Received<_Broker::StageNotice>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::StageContentsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::ObjectsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::StageEditTargetChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::LayerMutingChanged>(), 0);
}

TEST_F(MuteLayersTest, PartialBlocking)
{
    auto broker = PXR_NS::unf::Broker::Create(_stage);

    std::string target = typeid(_Broker::LayerMutingChanged).name();

    // Pass a predicate to block all broker notices.
    broker->BeginTransaction(
        [&](const _Broker::StageNotice &n){return (n.GetTypeId() == target); });

    _stage->MuteLayer(_layerIds[0]);
    _stage->MuteLayer(_layerIds[1]);
    _stage->UnmuteLayer(_layerIds[1]);
    _stage->MuteAndUnmuteLayers(
        std::vector<std::string>{_layerIds[2], _layerIds[1]},
        std::vector<std::string>{});

    // Ensure that USD Notices are being sent as expected.
    ASSERT_EQ(_usdListener.Received<_USD::StageNotice>(), 12);
    ASSERT_EQ(_usdListener.Received<_USD::StageContentsChanged>(), 4);
    ASSERT_EQ(_usdListener.Received<_USD::ObjectsChanged>(), 4);
    ASSERT_EQ(_usdListener.Received<_USD::StageEditTargetChanged>(), 0);
    ASSERT_EQ(_usdListener.Received<_USD::LayerMutingChanged>(), 4);

    // While broker notices are blocked during a transaction.
    ASSERT_EQ(_brokerListener.Received<_Broker::StageNotice>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::StageContentsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::ObjectsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::StageEditTargetChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::LayerMutingChanged>(), 0);

    broker->EndTransaction();

    // Ensure that only consolidated LayerMutingChanged broker notice are sent.
    ASSERT_EQ(_brokerListener.Received<_Broker::StageNotice>(), 1);
    ASSERT_EQ(_brokerListener.Received<_Broker::StageContentsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::ObjectsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::StageEditTargetChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::LayerMutingChanged>(), 1);
}

TEST_F(MuteLayersTest, Transaction_ObjectsChanged)
{
    auto broker = PXR_NS::unf::Broker::Create(_stage);

    // Create prim before caching to trigger resync path when muting.
    _stage->SetEditTarget(PXR_NS::UsdEditTarget(_layers[0]));
    _stage->DefinePrim(PXR_NS::SdfPath {"/Foo"});

    using Notice = _Broker::ObjectsChanged;

    class DataListener : public ::Test::ListenerBase<Notice>
    {
    public:
        using ::Test::ListenerBase<Notice>::ListenerBase;

    private:
        void OnReceiving(
            const Notice& n,
            const PXR_NS::UsdStageWeakPtr&) override
        {
            ASSERT_EQ(n.GetResyncedPaths().size(), 1);
            ASSERT_EQ(n.GetResyncedPaths().at(0), PXR_NS::SdfPath {"/"});
            ASSERT_EQ(n.GetChangedInfoOnlyPaths().size(), 0);
        }
    };

    DataListener listener(_stage);

    broker->BeginTransaction();

    _stage->MuteLayer(_layerIds[0]);
    _stage->MuteLayer(_layerIds[1]);
    _stage->UnmuteLayer(_layerIds[1]);
    _stage->MuteAndUnmuteLayers(
        std::vector<std::string>{_layerIds[2], _layerIds[1]},
        std::vector<std::string>{});

    broker->EndTransaction();
}

TEST_F(MuteLayersTest, Transaction_LayerMutingChanged)
{
    auto broker = PXR_NS::unf::Broker::Create(_stage);

    using Notice = _Broker::LayerMutingChanged;

    class DataListener : public ::Test::ListenerBase<Notice>
    {
    public:
        using ::Test::ListenerBase<Notice>::ListenerBase;

    private:
        void OnReceiving(
            const Notice& n,
            const PXR_NS::UsdStageWeakPtr& stage) override
        {
            auto layerIds = stage->GetRootLayer()->GetSubLayerPaths();
            ASSERT_EQ(n.GetMutedLayers().size(), 3);
            ASSERT_EQ(n.GetMutedLayers().at(0), std::string(layerIds[0]));
            ASSERT_EQ(n.GetMutedLayers().at(1), std::string(layerIds[2]));
            ASSERT_EQ(n.GetMutedLayers().at(2), std::string(layerIds[1]));
            ASSERT_EQ(n.GetUnmutedLayers().size(), 0);
        }
    };

    DataListener listener(_stage);

    broker->BeginTransaction();

    _stage->MuteLayer(_layerIds[0]);
    _stage->MuteLayer(_layerIds[1]);
    _stage->UnmuteLayer(_layerIds[1]);
    _stage->MuteAndUnmuteLayers(
        std::vector<std::string>{_layerIds[2], _layerIds[1]},
        std::vector<std::string>{});

    broker->EndTransaction();
}

TEST_F(MuteLayersTest, Caching_ObjectsChanged)
{
    auto broker = PXR_NS::unf::Broker::Create(_stage);

    // Create prims before caching to trigger resync path when muting.
    _stage->SetEditTarget(PXR_NS::UsdEditTarget(_layers[0]));
    _stage->DefinePrim(PXR_NS::SdfPath {"/Foo"});
    _stage->SetEditTarget(PXR_NS::UsdEditTarget(_layers[1]));
    _stage->DefinePrim(PXR_NS::SdfPath {"/Bar"});


    PXR_NS::unf::NoticeCache<_Broker::ObjectsChanged> cache;

    _stage->MuteLayer(_layerIds[0]);
    _stage->MuteLayer(_layerIds[1]);
    _stage->UnmuteLayer(_layerIds[1]);
    _stage->MuteAndUnmuteLayers(
        std::vector<std::string>{_layerIds[2], _layerIds[1]},
        std::vector<std::string>{});

    // Ensure that four notices have been cached.
    ASSERT_EQ(cache.Size(), 4);

    // Ensure that notice data are all as expected.
    {
        auto& n1 = cache.GetAll().at(0);
        ASSERT_EQ(n1->GetResyncedPaths().size(), 1);
        ASSERT_EQ(n1->GetResyncedPaths().at(0), PXR_NS::SdfPath {"/"});
        ASSERT_EQ(n1->GetChangedInfoOnlyPaths().size(), 0);

        auto& n2 = cache.GetAll().at(1);
        ASSERT_EQ(n2->GetResyncedPaths().size(), 1);
        ASSERT_EQ(n2->GetResyncedPaths().at(0), PXR_NS::SdfPath {"/"});
        ASSERT_EQ(n2->GetChangedInfoOnlyPaths().size(), 0);

        auto& n3 = cache.GetAll().at(2);
        ASSERT_EQ(n3->GetResyncedPaths().size(), 1);
        ASSERT_EQ(n3->GetResyncedPaths().at(0), PXR_NS::SdfPath {"/"});
        ASSERT_EQ(n3->GetChangedInfoOnlyPaths().size(), 0);

        auto& n4 = cache.GetAll().at(2);
        ASSERT_EQ(n4->GetResyncedPaths().size(), 1);
        ASSERT_EQ(n4->GetResyncedPaths().at(0), PXR_NS::SdfPath {"/"});
        ASSERT_EQ(n4->GetChangedInfoOnlyPaths().size(), 0);
    }

    cache.MergeAll();

    // Ensure that we have one merged notice after consolidation.
    ASSERT_EQ(cache.Size(), 1);

    // Ensure that the content of the notice is correct.
    {
        auto& n = cache.GetAll().at(0);
        ASSERT_EQ(n->GetResyncedPaths().size(), 1);
        ASSERT_EQ(n->GetResyncedPaths().at(0), PXR_NS::SdfPath {"/"});
        ASSERT_EQ(n->GetChangedInfoOnlyPaths().size(), 0);
    }
}

TEST_F(MuteLayersTest, Caching_StageContentsChanged)
{
    auto broker = PXR_NS::unf::Broker::Create(_stage);

    PXR_NS::unf::NoticeCache<_Broker::StageContentsChanged> cache;

    _stage->MuteLayer(_layerIds[0]);
    _stage->MuteLayer(_layerIds[1]);
    _stage->UnmuteLayer(_layerIds[1]);
    _stage->MuteAndUnmuteLayers(
        std::vector<std::string>{_layerIds[2], _layerIds[1]},
        std::vector<std::string>{});

    // Ensure that four notices have been cached.
    ASSERT_EQ(cache.Size(), 4);

    cache.MergeAll();

    // Ensure that we have one merged notice after consolidation.
    ASSERT_EQ(cache.Size(), 1);
}

TEST_F(MuteLayersTest, Caching_LayerMutingChanged)
{
    auto broker = PXR_NS::unf::Broker::Create(_stage);

    PXR_NS::unf::NoticeCache<_Broker::LayerMutingChanged> cache;

    _stage->MuteLayer(_layerIds[0]);
    _stage->MuteLayer(_layerIds[1]);
    _stage->UnmuteLayer(_layerIds[1]);
    _stage->MuteAndUnmuteLayers(
        std::vector<std::string>{_layerIds[2], _layerIds[1]},
        std::vector<std::string>{});

    // Ensure that four notices have been cached.
    ASSERT_EQ(cache.Size(), 4);

    // Ensure that notice data are all as expected.
    {
        auto& n1 = cache.GetAll().at(0);
        ASSERT_EQ(n1->GetMutedLayers().size(), 1);
        ASSERT_EQ(n1->GetMutedLayers().at(0), std::string(_layerIds[0]));
        ASSERT_EQ(n1->GetUnmutedLayers().size(), 0);

        auto& n2 = cache.GetAll().at(1);
        ASSERT_EQ(n2->GetMutedLayers().size(), 1);
        ASSERT_EQ(n2->GetMutedLayers().at(0), std::string(_layerIds[1]));
        ASSERT_EQ(n2->GetUnmutedLayers().size(), 0);

        auto& n3 = cache.GetAll().at(2);
        ASSERT_EQ(n3->GetMutedLayers().size(), 0);
        ASSERT_EQ(n3->GetUnmutedLayers().size(), 1);
        ASSERT_EQ(n3->GetUnmutedLayers().at(0), std::string(_layerIds[1]));

        auto& n4 = cache.GetAll().at(3);
        ASSERT_EQ(n4->GetMutedLayers().size(), 2);
        ASSERT_EQ(n4->GetMutedLayers().at(0), std::string(_layerIds[2]));
        ASSERT_EQ(n4->GetMutedLayers().at(1), std::string(_layerIds[1]));
        ASSERT_EQ(n4->GetUnmutedLayers().size(), 0);
    }

    cache.MergeAll();

    // Ensure that we have one merged notice after consolidation.
    ASSERT_EQ(cache.Size(), 1);

    // Ensure that the content of the notice is correct.
    {
        auto& n = cache.GetAll().at(0);
        ASSERT_EQ(n->GetMutedLayers().size(), 3);
        ASSERT_EQ(n->GetMutedLayers().at(0), std::string(_layerIds[0]));
        ASSERT_EQ(n->GetMutedLayers().at(1), std::string(_layerIds[2]));
        ASSERT_EQ(n->GetMutedLayers().at(2), std::string(_layerIds[1]));
        ASSERT_EQ(n->GetUnmutedLayers().size(), 0);
    }
}
