#include "broker.h"
#include "transaction.h"

#include "./utility.h"

#include "gtest/gtest.h"
#include "pxr/pxr.h"
#include "pxr/usd/sdf/path.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usd/stage.h"

TEST(Batching, WithBroker)
{
    auto stage = ::Test::CreateStageWithLayers();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    ASSERT_FALSE(broker->IsInTransaction());

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

    {
        PXR_NS::NoticeTransaction transaction(broker);

        ASSERT_TRUE(broker->IsInTransaction());

        // Edit the stage...
        auto layers = stage->GetRootLayer()->GetSubLayerPaths();
        auto layer1 = PXR_NS::SdfLayer::Find(layers[0]);

        stage->DefinePrim(PXR_NS::SdfPath {"/Foo"});
        stage->MuteLayer(layers[1]);
        stage->SetEditTarget(PXR_NS::UsdEditTarget(layer1));

        // Ensure that broker notices are not sent during a transaction.
        ASSERT_EQ(listener1.Received<_Broker::StageNotice>(), 0);
        ASSERT_EQ(listener1.Received<_Broker::StageContentsChanged>(), 0);
        ASSERT_EQ(listener1.Received<_Broker::ObjectsChanged>(), 0);
        ASSERT_EQ(listener1.Received<_Broker::StageEditTargetChanged>(), 0);
        ASSERT_EQ(listener1.Received<_Broker::LayerMutingChanged>(), 0);

        // While USD Notices are being sent as expected.
        ASSERT_EQ(listener2.Received<_USD::StageNotice>(), 6);
        ASSERT_EQ(listener2.Received<_USD::StageContentsChanged>(), 2);
        ASSERT_EQ(listener2.Received<_USD::ObjectsChanged>(), 2);
        ASSERT_EQ(listener2.Received<_USD::StageEditTargetChanged>(), 1);
        ASSERT_EQ(listener2.Received<_USD::LayerMutingChanged>(), 1);
    }

    // Ensure that consolidated broker notices are sent after a transaction.
    ASSERT_EQ(listener1.Received<_Broker::StageNotice>(), 4);
    ASSERT_EQ(listener1.Received<_Broker::StageContentsChanged>(), 1);
    ASSERT_EQ(listener1.Received<_Broker::ObjectsChanged>(), 1);
    ASSERT_EQ(listener1.Received<_Broker::StageEditTargetChanged>(), 1);
    ASSERT_EQ(listener1.Received<_Broker::LayerMutingChanged>(), 1);
}

TEST(Batching, WithoutBroker)
{
    auto stage = ::Test::CreateStageWithLayers();

    using namespace PXR_NS::UsdBrokerNotice;

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

    {
        PXR_NS::NoticeTransaction transaction(stage);

        // Edit the stage...
        auto layers = stage->GetRootLayer()->GetSubLayerPaths();
        auto layer1 = PXR_NS::SdfLayer::Find(layers[0]);

        stage->DefinePrim(PXR_NS::SdfPath {"/Foo"});
        stage->MuteLayer(layers[1]);
        stage->SetEditTarget(PXR_NS::UsdEditTarget(layer1));

        // Ensure that broker notices are not sent during a transaction.
        ASSERT_EQ(listener1.Received<_Broker::StageNotice>(), 0);
        ASSERT_EQ(listener1.Received<_Broker::StageContentsChanged>(), 0);
        ASSERT_EQ(listener1.Received<_Broker::ObjectsChanged>(), 0);
        ASSERT_EQ(listener1.Received<_Broker::StageEditTargetChanged>(), 0);
        ASSERT_EQ(listener1.Received<_Broker::LayerMutingChanged>(), 0);

        // While USD Notices are being sent as expected.
        ASSERT_EQ(listener2.Received<_USD::StageNotice>(), 6);
        ASSERT_EQ(listener2.Received<_USD::StageContentsChanged>(), 2);
        ASSERT_EQ(listener2.Received<_USD::ObjectsChanged>(), 2);
        ASSERT_EQ(listener2.Received<_USD::StageEditTargetChanged>(), 1);
        ASSERT_EQ(listener2.Received<_USD::LayerMutingChanged>(), 1);
    }

    // Ensure that consolidated broker notices are sent after a transaction.
    ASSERT_EQ(listener1.Received<_Broker::StageNotice>(), 4);
    ASSERT_EQ(listener1.Received<_Broker::StageContentsChanged>(), 1);
    ASSERT_EQ(listener1.Received<_Broker::ObjectsChanged>(), 1);
    ASSERT_EQ(listener1.Received<_Broker::StageEditTargetChanged>(), 1);
    ASSERT_EQ(listener1.Received<_Broker::LayerMutingChanged>(), 1);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
