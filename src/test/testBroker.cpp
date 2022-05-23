#include "../broker.h"

#include "./utility.h"

#include "gtest/gtest.h"
#include "pxr/pxr.h"
#include "pxr/usd/sdf/path.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usd/stage.h"

TEST(AddPrim, NoticesComparison)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    namespace _Broker = PXR_NS::UsdBrokerNotice;
    using _USD = PXR_NS::UsdNotice;

    // Subscribe to notices emitted by the broker.
    ::Test::Listener<
        _Broker::StageNotice,
        _Broker::StageContentsChanged,
        _Broker::ObjectsChanged,
        _Broker::StageEditTargetChanged,
        _Broker::LayerMutingChanged
    > listener1(stage);

    // Subscribe to Usd Notices emitted by the stage.
    ::Test::Listener<
        _USD::StageNotice,
        _USD::StageContentsChanged,
        _USD::ObjectsChanged,
        _USD::StageEditTargetChanged,
        _USD::LayerMutingChanged
    > listener2(stage);

    // Edit the stage...
    stage->DefinePrim(PXR_NS::SdfPath {"/Foo"});

    // Ensure that similar notices are received via the stage and the broker. 
    ASSERT_EQ(listener1.Received<_Broker::StageNotice>(), 2);
    ASSERT_EQ(listener1.Received<_Broker::StageContentsChanged>(), 1);
    ASSERT_EQ(listener1.Received<_Broker::ObjectsChanged>(), 1);
    ASSERT_EQ(listener1.Received<_Broker::StageEditTargetChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::LayerMutingChanged>(), 0);

    ASSERT_EQ(listener2.Received<_USD::StageNotice>(), 2);
    ASSERT_EQ(listener2.Received<_USD::StageContentsChanged>(), 1);
    ASSERT_EQ(listener2.Received<_USD::ObjectsChanged>(), 1);
    ASSERT_EQ(listener2.Received<_USD::StageEditTargetChanged>(), 0);
    ASSERT_EQ(listener2.Received<_USD::LayerMutingChanged>(), 0);
}

TEST(AddPrims, NoticesComparison)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    namespace _Broker = PXR_NS::UsdBrokerNotice;
    using _USD = PXR_NS::UsdNotice;

    // Subscribe to notices emitted by the broker.
    ::Test::Listener<
        _Broker::StageNotice,
        _Broker::StageContentsChanged,
        _Broker::ObjectsChanged,
        _Broker::StageEditTargetChanged,
        _Broker::LayerMutingChanged
    > listener1(stage);

    // Subscribe to Usd Notices emitted by the stage.
    ::Test::Listener<
        _USD::StageNotice,
        _USD::StageContentsChanged,
        _USD::ObjectsChanged,
        _USD::StageEditTargetChanged,
        _USD::LayerMutingChanged
    > listener2(stage);

    // Edit the stage...    
    stage->DefinePrim(PXR_NS::SdfPath {"/Foo"});
    stage->DefinePrim(PXR_NS::SdfPath {"/Bar"});
    stage->DefinePrim(PXR_NS::SdfPath {"/Baz"});

    // Ensure that similar notices are received via the stage and the broker. 
    ASSERT_EQ(listener1.Received<_Broker::StageNotice>(), 6);
    ASSERT_EQ(listener1.Received<_Broker::StageContentsChanged>(), 3);
    ASSERT_EQ(listener1.Received<_Broker::ObjectsChanged>(), 3);
    ASSERT_EQ(listener1.Received<_Broker::StageEditTargetChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::LayerMutingChanged>(), 0);

    ASSERT_EQ(listener2.Received<_USD::StageNotice>(), 6);
    ASSERT_EQ(listener2.Received<_USD::StageContentsChanged>(), 3);
    ASSERT_EQ(listener2.Received<_USD::ObjectsChanged>(), 3);
    ASSERT_EQ(listener2.Received<_USD::StageEditTargetChanged>(), 0);
    ASSERT_EQ(listener2.Received<_USD::LayerMutingChanged>(), 0);
}

TEST(MuteLayer, NoticesComparison)
{
    auto stage = ::Test::CreateStageWithLayers();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    namespace _Broker = PXR_NS::UsdBrokerNotice;
    using _USD = PXR_NS::UsdNotice;

    // Subscribe to notices emitted by the broker.
    ::Test::Listener<
        _Broker::StageNotice,
        _Broker::StageContentsChanged,
        _Broker::ObjectsChanged,
        _Broker::StageEditTargetChanged,
        _Broker::LayerMutingChanged
    > listener1(stage);

    // Subscribe to Usd Notices emitted by the stage.
    ::Test::Listener<
        _USD::StageNotice,
        _USD::StageContentsChanged,
        _USD::ObjectsChanged,
        _USD::StageEditTargetChanged,
        _USD::LayerMutingChanged
    > listener2(stage);

    // Edit the stage...    
    auto layers = stage->GetRootLayer()->GetSubLayerPaths();
    stage->MuteLayer(layers[0]);

    // Ensure that similar notices are received via the stage and the broker. 
    ASSERT_EQ(listener1.Received<_Broker::StageNotice>(), 3);
    ASSERT_EQ(listener1.Received<_Broker::StageContentsChanged>(), 1);
    ASSERT_EQ(listener1.Received<_Broker::ObjectsChanged>(), 1);
    ASSERT_EQ(listener1.Received<_Broker::StageEditTargetChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::LayerMutingChanged>(), 1);

    ASSERT_EQ(listener2.Received<_USD::StageNotice>(), 3);
    ASSERT_EQ(listener2.Received<_USD::StageContentsChanged>(), 1);
    ASSERT_EQ(listener2.Received<_USD::ObjectsChanged>(), 1);
    ASSERT_EQ(listener2.Received<_USD::StageEditTargetChanged>(), 0);
    ASSERT_EQ(listener2.Received<_USD::LayerMutingChanged>(), 1);
}

TEST(MuteLayers, NoticesComparison)
{
    auto stage = ::Test::CreateStageWithLayers();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    namespace _Broker = PXR_NS::UsdBrokerNotice;
    using _USD = PXR_NS::UsdNotice;

    // Subscribe to notices emitted by the broker.
    ::Test::Listener<
        _Broker::StageNotice,
        _Broker::StageContentsChanged,
        _Broker::ObjectsChanged,
        _Broker::StageEditTargetChanged,
        _Broker::LayerMutingChanged
    > listener1(stage);

    // Subscribe to Usd Notices emitted by the stage.
    ::Test::Listener<
        _USD::StageNotice,
        _USD::StageContentsChanged,
        _USD::ObjectsChanged,
        _USD::StageEditTargetChanged,
        _USD::LayerMutingChanged
    > listener2(stage);

    // Edit the stage...    
    auto layers = stage->GetRootLayer()->GetSubLayerPaths();

    // Keep ref pointer to the layer we try to mute and unmute to
    // prevent it for being destroyed when it is muted.
    auto _layer = PXR_NS::SdfLayer::FindOrOpen(layers[1]);

    stage->MuteLayer(layers[0]);
    stage->MuteLayer(layers[1]);
    stage->UnmuteLayer(layers[1]);
    stage->MuteAndUnmuteLayers(
        std::vector<std::string>{layers[2], layers[1]}, 
        std::vector<std::string>{});

    // Ensure that similar notices are received via the stage and the broker. 
    ASSERT_EQ(listener1.Received<_Broker::StageNotice>(), 12);
    ASSERT_EQ(listener1.Received<_Broker::StageContentsChanged>(), 4);
    ASSERT_EQ(listener1.Received<_Broker::ObjectsChanged>(), 4);
    ASSERT_EQ(listener1.Received<_Broker::StageEditTargetChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::LayerMutingChanged>(), 4);

    ASSERT_EQ(listener2.Received<_USD::StageNotice>(), 12);
    ASSERT_EQ(listener2.Received<_USD::StageContentsChanged>(), 4);
    ASSERT_EQ(listener2.Received<_USD::ObjectsChanged>(), 4);
    ASSERT_EQ(listener2.Received<_USD::StageEditTargetChanged>(), 0);
    ASSERT_EQ(listener2.Received<_USD::LayerMutingChanged>(), 4);
}

TEST(ChangeEditTarget, NoticesComparison)
{
    auto stage = ::Test::CreateStageWithLayers();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    namespace _Broker = PXR_NS::UsdBrokerNotice;
    using _USD = PXR_NS::UsdNotice;

    // Subscribe to notices emitted by the broker.
    ::Test::Listener<
        _Broker::StageNotice,
        _Broker::StageContentsChanged,
        _Broker::ObjectsChanged,
        _Broker::StageEditTargetChanged,
        _Broker::LayerMutingChanged
    > listener1(stage);

    // Subscribe to Usd Notices emitted by the stage.
    ::Test::Listener<
        _USD::StageNotice,
        _USD::StageContentsChanged,
        _USD::ObjectsChanged,
        _USD::StageEditTargetChanged,
        _USD::LayerMutingChanged
    > listener2(stage);

    // Edit the stage...
    auto layers = stage->GetRootLayer()->GetSubLayerPaths();
    auto layer1 = PXR_NS::SdfLayer::Find(layers[0]);
    auto layer2 = PXR_NS::SdfLayer::Find(layers[1]);

    stage->SetEditTarget(PXR_NS::UsdEditTarget(layer1));
    stage->SetEditTarget(PXR_NS::UsdEditTarget(layer2));

    // Ensure that similar notices are received via the stage and the broker. 
    ASSERT_EQ(listener1.Received<_Broker::StageNotice>(), 2);
    ASSERT_EQ(listener1.Received<_Broker::StageContentsChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::ObjectsChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::StageEditTargetChanged>(), 2);
    ASSERT_EQ(listener1.Received<_Broker::LayerMutingChanged>(), 0);

    ASSERT_EQ(listener2.Received<_USD::StageNotice>(), 2);
    ASSERT_EQ(listener2.Received<_USD::StageContentsChanged>(), 0);
    ASSERT_EQ(listener2.Received<_USD::ObjectsChanged>(), 0);
    ASSERT_EQ(listener2.Received<_USD::StageEditTargetChanged>(), 2);
    ASSERT_EQ(listener2.Received<_USD::LayerMutingChanged>(), 0);
}

TEST(CustomNotices, Handling)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    namespace _Broker = PXR_NS::UsdBrokerNotice;

    // Subscribe to notices emitted by the broker.
    ::Test::Listener<
        ::Test::MergeableNotice,
        ::Test::UnMergeableNotice
    > listener(stage);

    broker->Send<::Test::MergeableNotice>(::Test::DataMap({{"Foo", "Test1"}}));
    broker->Send<::Test::MergeableNotice>(::Test::DataMap({{"Foo", "Test2"}}));
    broker->Send<::Test::MergeableNotice>(::Test::DataMap({{"Bar", "Test3"}}));

    broker->Send<::Test::UnMergeableNotice>();
    broker->Send<::Test::UnMergeableNotice>();
    broker->Send<::Test::UnMergeableNotice>();

    ASSERT_EQ(listener.Received<::Test::MergeableNotice>(), 3);
    ASSERT_EQ(listener.Received<::Test::UnMergeableNotice>(), 3);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
