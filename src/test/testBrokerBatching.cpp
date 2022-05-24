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

    broker->BeginTransaction();

    ASSERT_TRUE(broker->IsInTransaction());

    // Edit the stage...
    stage->DefinePrim(PXR_NS::SdfPath {"/Foo"});

    // Ensure that broker notices are not sent during a transaction. 
    ASSERT_EQ(listener1.Received<_Broker::StageNotice>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::StageContentsChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::ObjectsChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::StageEditTargetChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::LayerMutingChanged>(), 0);

    // While USD Notices are being sent as expected.
    ASSERT_EQ(listener2.Received<_USD::StageNotice>(), 2);
    ASSERT_EQ(listener2.Received<_USD::StageContentsChanged>(), 1);
    ASSERT_EQ(listener2.Received<_USD::ObjectsChanged>(), 1);
    ASSERT_EQ(listener2.Received<_USD::StageEditTargetChanged>(), 0);
    ASSERT_EQ(listener2.Received<_USD::LayerMutingChanged>(), 0);

    broker->EndTransaction();

    ASSERT_FALSE(broker->IsInTransaction());

    // Ensure that consolidated broker notices are sent after a transaction.
    ASSERT_EQ(listener1.Received<_Broker::StageNotice>(), 2);
    ASSERT_EQ(listener1.Received<_Broker::StageContentsChanged>(), 1);
    ASSERT_EQ(listener1.Received<_Broker::ObjectsChanged>(), 1);
    ASSERT_EQ(listener1.Received<_Broker::StageEditTargetChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::LayerMutingChanged>(), 0);
}

TEST(AddPrim, ObjectsChanged)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    namespace _Broker = PXR_NS::UsdBrokerNotice;
    using Notice = _Broker::ObjectsChanged;

    class Listener : public ::Test::ListenerBase<Notice>
    {
    public:
        using ::Test::ListenerBase<Notice>::ListenerBase;

    private:
        void OnReceiving(
            const Notice& n, 
            const PXR_NS::UsdStageWeakPtr&) override 
        {
            ASSERT_EQ(n.GetResyncedPaths().size(), 1);
            ASSERT_EQ(n.GetResyncedPaths().at(0), PXR_NS::SdfPath {"/Foo"});
            ASSERT_EQ(n.GetChangedInfoOnlyPaths().size(), 0);
        }
    };

    Listener listener(stage);

    broker->BeginTransaction();

    // Edit the stage...
    stage->DefinePrim(PXR_NS::SdfPath {"/Foo"});

    broker->EndTransaction();
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

    broker->BeginTransaction();

    ASSERT_TRUE(broker->IsInTransaction());

    // Edit the stage...
    stage->DefinePrim(PXR_NS::SdfPath {"/Foo"});
    stage->DefinePrim(PXR_NS::SdfPath {"/Bar"});
    stage->DefinePrim(PXR_NS::SdfPath {"/Baz"});

    // Ensure that broker notices are not sent during a transaction. 
    ASSERT_EQ(listener1.Received<_Broker::StageNotice>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::StageContentsChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::ObjectsChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::StageEditTargetChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::LayerMutingChanged>(), 0);

    // While USD Notices are being sent as expected.
    ASSERT_EQ(listener2.Received<_USD::StageNotice>(), 6);
    ASSERT_EQ(listener2.Received<_USD::StageContentsChanged>(), 3);
    ASSERT_EQ(listener2.Received<_USD::ObjectsChanged>(), 3);
    ASSERT_EQ(listener2.Received<_USD::StageEditTargetChanged>(), 0);
    ASSERT_EQ(listener2.Received<_USD::LayerMutingChanged>(), 0);

    broker->EndTransaction();

    ASSERT_FALSE(broker->IsInTransaction());

    // Ensure that consolidated broker notices are sent after a transaction.
    ASSERT_EQ(listener1.Received<_Broker::StageNotice>(), 2);
    ASSERT_EQ(listener1.Received<_Broker::StageContentsChanged>(), 1);
    ASSERT_EQ(listener1.Received<_Broker::ObjectsChanged>(), 1);
    ASSERT_EQ(listener1.Received<_Broker::StageEditTargetChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::LayerMutingChanged>(), 0);
}

TEST(AddPrims, ObjectsChanged)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    namespace _Broker = PXR_NS::UsdBrokerNotice;
    using Notice = _Broker::ObjectsChanged;

    class Listener : public ::Test::ListenerBase<Notice>
    {
    public:
        using ::Test::ListenerBase<Notice>::ListenerBase;

    private:
        void OnReceiving(
            const Notice& n, 
            const PXR_NS::UsdStageWeakPtr&) override 
        {
            ASSERT_EQ(n.GetResyncedPaths().size(), 3);
            ASSERT_EQ(n.GetResyncedPaths().at(0), PXR_NS::SdfPath {"/Foo"});
            ASSERT_EQ(n.GetResyncedPaths().at(1), PXR_NS::SdfPath {"/Bar"});
            ASSERT_EQ(n.GetResyncedPaths().at(2), PXR_NS::SdfPath {"/Baz"});
            ASSERT_EQ(n.GetChangedInfoOnlyPaths().size(), 0);
        }
    };

    Listener listener(stage);

    broker->BeginTransaction();

    // Edit the stage...
    stage->DefinePrim(PXR_NS::SdfPath {"/Foo"});
    stage->DefinePrim(PXR_NS::SdfPath {"/Bar"});
    stage->DefinePrim(PXR_NS::SdfPath {"/Baz"});

    broker->EndTransaction();
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

    broker->BeginTransaction();

    ASSERT_TRUE(broker->IsInTransaction());

    // Edit the stage...
    auto layers = stage->GetRootLayer()->GetSubLayerPaths();
    stage->MuteLayer(layers[0]);

    // Ensure that broker notices are not sent during a transaction. 
    ASSERT_EQ(listener1.Received<_Broker::StageNotice>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::StageContentsChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::ObjectsChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::StageEditTargetChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::LayerMutingChanged>(), 0);

    // While USD Notices are being sent as expected.
    ASSERT_EQ(listener2.Received<_USD::StageNotice>(), 3);
    ASSERT_EQ(listener2.Received<_USD::StageContentsChanged>(), 1);
    ASSERT_EQ(listener2.Received<_USD::ObjectsChanged>(), 1);
    ASSERT_EQ(listener2.Received<_USD::StageEditTargetChanged>(), 0);
    ASSERT_EQ(listener2.Received<_USD::LayerMutingChanged>(), 1);

    broker->EndTransaction();

    ASSERT_FALSE(broker->IsInTransaction());

    // Ensure that consolidated broker notices are sent after a transaction.
    ASSERT_EQ(listener1.Received<_Broker::StageNotice>(), 3);
    ASSERT_EQ(listener1.Received<_Broker::StageContentsChanged>(), 1);
    ASSERT_EQ(listener1.Received<_Broker::ObjectsChanged>(), 1);
    ASSERT_EQ(listener1.Received<_Broker::StageEditTargetChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::LayerMutingChanged>(), 1);
}

TEST(MuteLayer, ObjectsChanged)
{
    auto stage = ::Test::CreateStageWithLayers();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    // Create prim before caching to trigger resync path when muting.
    auto layers = stage->GetRootLayer()->GetSubLayerPaths();
    auto layer = PXR_NS::SdfLayer::Find(layers[0]);
    stage->SetEditTarget(PXR_NS::UsdEditTarget(layer));
    stage->DefinePrim(PXR_NS::SdfPath {"/Foo"});

    namespace _Broker = PXR_NS::UsdBrokerNotice;
    using Notice = _Broker::ObjectsChanged;

    class Listener : public ::Test::ListenerBase<Notice>
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

    Listener listener(stage);

    broker->BeginTransaction();

    // Edit the stage...
    stage->MuteLayer(layers[0]);

    broker->EndTransaction();
}

TEST(MuteLayer, LayerMutingChanged)
{
    auto stage = ::Test::CreateStageWithLayers();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    namespace _Broker = PXR_NS::UsdBrokerNotice;
    using Notice = _Broker::LayerMutingChanged;

    class Listener : public ::Test::ListenerBase<Notice>
    {
    public:
        using ::Test::ListenerBase<Notice>::ListenerBase;

    private:
        void OnReceiving(
            const Notice& n, 
            const PXR_NS::UsdStageWeakPtr& stage) override 
        {
            auto _layers = stage->GetRootLayer()->GetSubLayerPaths();
            ASSERT_EQ(n.GetMutedLayers().size(), 1);
            ASSERT_EQ(n.GetMutedLayers().at(0), std::string(_layers[0]));
            ASSERT_EQ(n.GetUnmutedLayers().size(), 0);
        }
    };

    Listener listener(stage);

    broker->BeginTransaction();

    // Edit the stage...
    auto layers = stage->GetRootLayer()->GetSubLayerPaths();
    stage->MuteLayer(layers[0]);

    broker->EndTransaction();
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

    broker->BeginTransaction();

    ASSERT_TRUE(broker->IsInTransaction());

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

    // Ensure that broker notices are not sent during a transaction. 
    ASSERT_EQ(listener1.Received<_Broker::StageNotice>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::StageContentsChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::ObjectsChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::StageEditTargetChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::LayerMutingChanged>(), 0);

    // While USD Notices are being sent as expected.
    ASSERT_EQ(listener2.Received<_USD::StageNotice>(), 12);
    ASSERT_EQ(listener2.Received<_USD::StageContentsChanged>(), 4);
    ASSERT_EQ(listener2.Received<_USD::ObjectsChanged>(), 4);
    ASSERT_EQ(listener2.Received<_USD::StageEditTargetChanged>(), 0);
    ASSERT_EQ(listener2.Received<_USD::LayerMutingChanged>(), 4);

    broker->EndTransaction();

    ASSERT_FALSE(broker->IsInTransaction());

    // Ensure that consolidated broker notices are sent after a transaction.
    ASSERT_EQ(listener1.Received<_Broker::StageNotice>(), 3);
    ASSERT_EQ(listener1.Received<_Broker::StageContentsChanged>(), 1);
    ASSERT_EQ(listener1.Received<_Broker::ObjectsChanged>(), 1);
    ASSERT_EQ(listener1.Received<_Broker::StageEditTargetChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::LayerMutingChanged>(), 1);
}

TEST(MuteLayers, ObjectsChanged)
{
    auto stage = ::Test::CreateStageWithLayers();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    // Create prim before caching to trigger resync path when muting.
    auto layers = stage->GetRootLayer()->GetSubLayerPaths();
    auto layer = PXR_NS::SdfLayer::Find(layers[0]);
    stage->SetEditTarget(PXR_NS::UsdEditTarget(layer));
    stage->DefinePrim(PXR_NS::SdfPath {"/Foo"});

    // Keep ref pointer to the layer we try to mute and unmute to
    // prevent it for being destroyed when it is muted.
    auto layer2 = PXR_NS::SdfLayer::FindOrOpen(layers[1]);
    stage->SetEditTarget(PXR_NS::UsdEditTarget(layer2));
    stage->DefinePrim(PXR_NS::SdfPath {"/Bar"});

    namespace _Broker = PXR_NS::UsdBrokerNotice;
    using Notice = _Broker::ObjectsChanged;

    class Listener : public ::Test::ListenerBase<Notice>
    {
    public:
        using ::Test::ListenerBase<Notice>::ListenerBase;

    private:
        void OnReceiving(
            const Notice& n, 
            const PXR_NS::UsdStageWeakPtr&) override 
        {
            // TODO: Update merging logic to prevent duplicated paths.
            ASSERT_EQ(n.GetResyncedPaths().size(), 4);
            ASSERT_EQ(n.GetResyncedPaths().at(0), PXR_NS::SdfPath {"/"});
            ASSERT_EQ(n.GetResyncedPaths().at(1), PXR_NS::SdfPath {"/"});
            ASSERT_EQ(n.GetResyncedPaths().at(2), PXR_NS::SdfPath {"/"});
            ASSERT_EQ(n.GetResyncedPaths().at(3), PXR_NS::SdfPath {"/"});
            ASSERT_EQ(n.GetChangedInfoOnlyPaths().size(), 0);
        }
    };

    Listener listener(stage);

    broker->BeginTransaction();

    // Edit the stage...
    stage->MuteLayer(layers[0]);
    stage->MuteLayer(layers[1]);
    stage->UnmuteLayer(layers[1]);
    stage->MuteAndUnmuteLayers(
        std::vector<std::string>{layers[2], layers[1]}, 
        std::vector<std::string>{});

    broker->EndTransaction();
}

TEST(MuteLayers, LayerMutingChanged)
{
    auto stage = ::Test::CreateStageWithLayers();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    namespace _Broker = PXR_NS::UsdBrokerNotice;
    using Notice = _Broker::LayerMutingChanged;

    class Listener : public ::Test::ListenerBase<Notice>
    {
    public:
        using ::Test::ListenerBase<Notice>::ListenerBase;

    private:
        void OnReceiving(
            const Notice& n, 
            const PXR_NS::UsdStageWeakPtr& stage) override 
        {
            auto _layers = stage->GetRootLayer()->GetSubLayerPaths();
            ASSERT_EQ(n.GetMutedLayers().size(), 3);
            ASSERT_EQ(n.GetMutedLayers().at(0), std::string(_layers[0]));
            ASSERT_EQ(n.GetMutedLayers().at(1), std::string(_layers[2]));
            ASSERT_EQ(n.GetMutedLayers().at(2), std::string(_layers[1]));
            ASSERT_EQ(n.GetUnmutedLayers().size(), 0);
        }
    };

    Listener listener(stage);

    broker->BeginTransaction();

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

    broker->EndTransaction();
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

    broker->BeginTransaction();

    ASSERT_TRUE(broker->IsInTransaction());

    // Edit the stage...
    auto layers = stage->GetRootLayer()->GetSubLayerPaths();
    auto layer1 = PXR_NS::SdfLayer::Find(layers[0]);
    auto layer2 = PXR_NS::SdfLayer::Find(layers[1]);

    stage->SetEditTarget(PXR_NS::UsdEditTarget(layer1));
    stage->SetEditTarget(PXR_NS::UsdEditTarget(layer2));

    // Ensure that broker notices are not sent during a transaction. 
    ASSERT_EQ(listener1.Received<_Broker::StageNotice>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::StageContentsChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::ObjectsChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::StageEditTargetChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::LayerMutingChanged>(), 0);

    // While USD Notices are being sent as expected.
    ASSERT_EQ(listener2.Received<_USD::StageNotice>(), 2);
    ASSERT_EQ(listener2.Received<_USD::StageContentsChanged>(), 0);
    ASSERT_EQ(listener2.Received<_USD::ObjectsChanged>(), 0);
    ASSERT_EQ(listener2.Received<_USD::StageEditTargetChanged>(), 2);
    ASSERT_EQ(listener2.Received<_USD::LayerMutingChanged>(), 0);

    broker->EndTransaction();

    ASSERT_FALSE(broker->IsInTransaction());

    // Ensure that consolidated broker notices are sent after a transaction.
    ASSERT_EQ(listener1.Received<_Broker::StageNotice>(), 1);
    ASSERT_EQ(listener1.Received<_Broker::StageContentsChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::ObjectsChanged>(), 0);
    ASSERT_EQ(listener1.Received<_Broker::StageEditTargetChanged>(), 1);
    ASSERT_EQ(listener1.Received<_Broker::LayerMutingChanged>(), 0);
}

TEST(CustomNotices, Comparison)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    // Subscribe to notices emitted by the broker.
    ::Test::Listener<
        ::Test::MergeableNotice,
        ::Test::UnMergeableNotice
    > listener(stage);

    broker->BeginTransaction();

    ASSERT_TRUE(broker->IsInTransaction());

    broker->Send<::Test::MergeableNotice>(::Test::DataMap({{"Foo", "Test1"}}));
    broker->Send<::Test::MergeableNotice>(::Test::DataMap({{"Foo", "Test2"}}));
    broker->Send<::Test::MergeableNotice>(::Test::DataMap({{"Bar", "Test3"}}));

    broker->Send<::Test::UnMergeableNotice>();
    broker->Send<::Test::UnMergeableNotice>();
    broker->Send<::Test::UnMergeableNotice>();

    // Ensure that broker notices are not sent during a transaction. 
    ASSERT_EQ(listener.Received<::Test::MergeableNotice>(), 0);
    ASSERT_EQ(listener.Received<::Test::UnMergeableNotice>(), 0);

    broker->EndTransaction();

    ASSERT_FALSE(broker->IsInTransaction());

    // Ensure that mergeable notices are sent as one notice after a transaction.
    ASSERT_EQ(listener.Received<::Test::MergeableNotice>(), 1);

    // While all un-mergeable notices are sent after a transaction.
    ASSERT_EQ(listener.Received<::Test::UnMergeableNotice>(), 3);
}

TEST(CustomNotices, MergeableNotice)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    using Notice = ::Test::MergeableNotice;

    class Listener : public ::Test::ListenerBase<Notice>
    {
    public:
        using ::Test::ListenerBase<Notice>::ListenerBase;

    private:
        void OnReceiving(
            const Notice& n, 
            const PXR_NS::UsdStageWeakPtr&) override 
        {
            ASSERT_EQ(n.GetData(), 
                ::Test::DataMap({{"Foo", "Test2"}, {"Bar", "Test3"}}));
        }
    };

    Listener listener(stage);

    broker->BeginTransaction();

    broker->Send<::Test::MergeableNotice>(::Test::DataMap({{"Foo", "Test1"}}));
    broker->Send<::Test::MergeableNotice>(::Test::DataMap({{"Foo", "Test2"}}));
    broker->Send<::Test::MergeableNotice>(::Test::DataMap({{"Bar", "Test3"}}));

    broker->EndTransaction();
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
