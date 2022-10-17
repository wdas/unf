#include <unf/broker.h>

#include <unfTest/listener.h>
#include <unfTest/notice.h>
#include <unfTest/observer.h>

#include <gtest/gtest.h>
#include <pxr/usd/sdf/layer.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usd/stage.h>

#include <string>
#include <typeinfo>

// namespace aliases for convenience.
using _USD = PXR_NS::UsdNotice;
namespace _Broker = unf::BrokerNotice;

class MuteLayersTest : public ::testing::Test {
  protected:
    using UsdListener = ::Test::Listener<
        _USD::StageNotice, _USD::StageContentsChanged, _USD::ObjectsChanged,
        _USD::StageEditTargetChanged, _USD::LayerMutingChanged>;

    using BrokerListener = ::Test::Listener<
        _Broker::StageNotice, _Broker::StageContentsChanged,
        _Broker::ObjectsChanged, _Broker::StageEditTargetChanged,
        _Broker::LayerMutingChanged>;

    void SetUp() override
    {
        _stage = PXR_NS::UsdStage::CreateInMemory();
        _AddLayers(3);

        _usdListener.SetStage(_stage);
        _brokerListener.SetStage(_stage);
    }

    void _AddLayers(int number)
    {
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
    auto broker = unf::Broker::Create(_stage);

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
    auto broker = unf::Broker::Create(_stage);

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
    auto broker = unf::Broker::Create(_stage);

    // Pass a predicate to block all broker notices.
    broker->BeginTransaction([](const _Broker::StageNotice&) { return false; });

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
    auto broker = unf::Broker::Create(_stage);

    std::string target = typeid(_Broker::LayerMutingChanged).name();

    // Pass a predicate to block all broker notices.
    broker->BeginTransaction([&](const _Broker::StageNotice& n) {
        return (n.GetTypeId() == target);
    });

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
    auto broker = unf::Broker::Create(_stage);

    // Create prim before caching to trigger resync path when muting.
    _stage->SetEditTarget(PXR_NS::UsdEditTarget(_layers[0]));
    _stage->DefinePrim(PXR_NS::SdfPath{"/Foo"});

    ::Test::Observer<_Broker::ObjectsChanged> observer(_stage);

    broker->BeginTransaction();

    _stage->MuteLayer(_layerIds[0]);
    _stage->MuteLayer(_layerIds[1]);
    _stage->UnmuteLayer(_layerIds[1]);
    _stage->MuteAndUnmuteLayers(
        std::vector<std::string>{_layerIds[2], _layerIds[1]},
        std::vector<std::string>{});

    ASSERT_EQ(observer.Received(), 0);

    broker->EndTransaction();

    ASSERT_EQ(observer.Received(), 1);

    const auto& n = observer.GetLatestNotice();
    ASSERT_EQ(n.GetResyncedPaths().size(), 1);
    ASSERT_EQ(n.GetResyncedPaths().at(0), PXR_NS::SdfPath{"/"});
    ASSERT_EQ(n.GetChangedInfoOnlyPaths().size(), 0);
}

TEST_F(MuteLayersTest, Transaction_LayerMutingChanged)
{
    auto broker = unf::Broker::Create(_stage);

    ::Test::Observer<_Broker::LayerMutingChanged> observer(_stage);

    broker->BeginTransaction();

    _stage->MuteLayer(_layerIds[0]);
    _stage->MuteLayer(_layerIds[1]);
    _stage->UnmuteLayer(_layerIds[1]);
    _stage->MuteAndUnmuteLayers(
        std::vector<std::string>{_layerIds[2], _layerIds[1]},
        std::vector<std::string>{});

    ASSERT_EQ(observer.Received(), 0);

    broker->EndTransaction();

    ASSERT_EQ(observer.Received(), 1);

    const auto& n = observer.GetLatestNotice();
    auto layerIds = _stage->GetRootLayer()->GetSubLayerPaths();
    ASSERT_EQ(n.GetMutedLayers().size(), 3);
    ASSERT_EQ(n.GetMutedLayers().at(0), std::string(layerIds[0]));
    ASSERT_EQ(n.GetMutedLayers().at(1), std::string(layerIds[2]));
    ASSERT_EQ(n.GetMutedLayers().at(2), std::string(layerIds[1]));
    ASSERT_EQ(n.GetUnmutedLayers().size(), 0);
}
