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
namespace _Broker = unf::BrokerNotice;

class ChangeEditTargetTest : public ::testing::Test {
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
        _AddLayers(2);

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

TEST_F(ChangeEditTargetTest, Simple)
{
    auto broker = unf::Broker::Create(_stage);

    _stage->SetEditTarget(PXR_NS::UsdEditTarget(_layers[0]));
    _stage->SetEditTarget(PXR_NS::UsdEditTarget(_layers[1]));

    // Ensure that similar notices are received via the stage and the broker.
    ASSERT_EQ(_usdListener.Received<_USD::StageNotice>(), 2);
    ASSERT_EQ(_usdListener.Received<_USD::StageContentsChanged>(), 0);
    ASSERT_EQ(_usdListener.Received<_USD::ObjectsChanged>(), 0);
    ASSERT_EQ(_usdListener.Received<_USD::StageEditTargetChanged>(), 2);
    ASSERT_EQ(_usdListener.Received<_USD::LayerMutingChanged>(), 0);

    ASSERT_EQ(_brokerListener.Received<_Broker::StageNotice>(), 2);
    ASSERT_EQ(_brokerListener.Received<_Broker::StageContentsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::ObjectsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::StageEditTargetChanged>(), 2);
    ASSERT_EQ(_brokerListener.Received<_Broker::LayerMutingChanged>(), 0);
}

TEST_F(ChangeEditTargetTest, Batching)
{
    auto broker = unf::Broker::Create(_stage);

    broker->BeginTransaction();

    _stage->SetEditTarget(PXR_NS::UsdEditTarget(_layers[0]));
    _stage->SetEditTarget(PXR_NS::UsdEditTarget(_layers[1]));

    // Ensure that USD Notices are being sent as expected.
    ASSERT_EQ(_usdListener.Received<_USD::StageNotice>(), 2);
    ASSERT_EQ(_usdListener.Received<_USD::StageContentsChanged>(), 0);
    ASSERT_EQ(_usdListener.Received<_USD::ObjectsChanged>(), 0);
    ASSERT_EQ(_usdListener.Received<_USD::StageEditTargetChanged>(), 2);
    ASSERT_EQ(_usdListener.Received<_USD::LayerMutingChanged>(), 0);

    // While broker notices are blocked during a transaction.
    ASSERT_EQ(_brokerListener.Received<_Broker::StageNotice>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::StageContentsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::ObjectsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::StageEditTargetChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::LayerMutingChanged>(), 0);

    broker->EndTransaction();

    // Ensure that consolidated broker notices are sent after a transaction.
    ASSERT_EQ(_brokerListener.Received<_Broker::StageNotice>(), 1);
    ASSERT_EQ(_brokerListener.Received<_Broker::StageContentsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::ObjectsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_Broker::StageEditTargetChanged>(), 1);
    ASSERT_EQ(_brokerListener.Received<_Broker::LayerMutingChanged>(), 0);
}

TEST_F(ChangeEditTargetTest, Blocking)
{
    auto broker = unf::Broker::Create(_stage);

    // Pass a predicate to block all broker notices.
    broker->BeginTransaction(
        [](const _Broker::StageNotice &){ return false; });

    _stage->SetEditTarget(PXR_NS::UsdEditTarget(_layers[0]));
    _stage->SetEditTarget(PXR_NS::UsdEditTarget(_layers[1]));

    // Ensure that USD Notices are being sent as expected.
    ASSERT_EQ(_usdListener.Received<_USD::StageNotice>(), 2);
    ASSERT_EQ(_usdListener.Received<_USD::StageContentsChanged>(), 0);
    ASSERT_EQ(_usdListener.Received<_USD::ObjectsChanged>(), 0);
    ASSERT_EQ(_usdListener.Received<_USD::StageEditTargetChanged>(), 2);
    ASSERT_EQ(_usdListener.Received<_USD::LayerMutingChanged>(), 0);

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

TEST_F(ChangeEditTargetTest, Caching_StageEditTargetChanged)
{
    auto broker = unf::Broker::Create(_stage);

    unf::NoticeCache<_Broker::StageEditTargetChanged> cache;

    _stage->SetEditTarget(PXR_NS::UsdEditTarget(_layers[0]));
    _stage->SetEditTarget(PXR_NS::UsdEditTarget(_layers[1]));

    // Ensure that two notices have been cached.
    ASSERT_EQ(cache.Size(), 2);

    cache.MergeAll();

    // Ensure that we have one merged notice after consolidation.
    ASSERT_EQ(cache.Size(), 1);
}
