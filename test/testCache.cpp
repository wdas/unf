#include "cache.h"
#include "broker.h"

#include "./utility.h"

#include <gtest/gtest.h>
#include <pxr/pxr.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usd/stage.h>

#include <string>

TEST(AddPrim, ObjectsChanged)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    namespace _Broker = PXR_NS::UsdBrokerNotice;

    PXR_NS::NoticeCache<_Broker::ObjectsChanged> cache;

    // Edit the stage...
    stage->DefinePrim(PXR_NS::SdfPath {"/Foo"});

    // Ensure that one notice has been cached.
    ASSERT_EQ(cache.Size(), 1);

    // Ensure that notice data is as expected.
    {
        auto& n = cache.GetAll().at(0);
        ASSERT_EQ(n->GetResyncedPaths().size(), 1);
        ASSERT_EQ(n->GetResyncedPaths().at(0), PXR_NS::SdfPath {"/Foo"});
        ASSERT_EQ(n->GetChangedInfoOnlyPaths().size(), 0);
    }

    cache.MergeAll();

    // Ensure that we have still one notice after consolidation.
    ASSERT_EQ(cache.Size(), 1);

    // Ensure that the content of the notice didn't change.
    {
        auto& n = cache.GetAll().at(0);
        ASSERT_EQ(n->GetResyncedPaths().size(), 1);
        ASSERT_EQ(n->GetResyncedPaths().at(0), PXR_NS::SdfPath {"/Foo"});
        ASSERT_EQ(n->GetChangedInfoOnlyPaths().size(), 0);
    }
}

TEST(AddPrims, ObjectsChanged)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    namespace _Broker = PXR_NS::UsdBrokerNotice;

    PXR_NS::NoticeCache<_Broker::ObjectsChanged> cache;

    // Edit the stage...
    stage->DefinePrim(PXR_NS::SdfPath {"/Foo"});
    stage->DefinePrim(PXR_NS::SdfPath {"/Bar"});
    stage->DefinePrim(PXR_NS::SdfPath {"/Baz"});

    // Ensure that three notices have been cached.
    ASSERT_EQ(cache.Size(), 3);

    // Ensure that notice data are all as expected.
    {
        auto& n1 = cache.GetAll().at(0);
        ASSERT_EQ(n1->GetResyncedPaths().size(), 1);
        ASSERT_EQ(n1->GetResyncedPaths().at(0), PXR_NS::SdfPath {"/Foo"});
        ASSERT_EQ(n1->GetChangedInfoOnlyPaths().size(), 0);

        auto& n2 = cache.GetAll().at(1);
        ASSERT_EQ(n2->GetResyncedPaths().size(), 1);
        ASSERT_EQ(n2->GetResyncedPaths().at(0), PXR_NS::SdfPath {"/Bar"});
        ASSERT_EQ(n2->GetChangedInfoOnlyPaths().size(), 0);

        auto& n3 = cache.GetAll().at(2);
        ASSERT_EQ(n3->GetResyncedPaths().size(), 1);
        ASSERT_EQ(n3->GetResyncedPaths().at(0), PXR_NS::SdfPath {"/Baz"});
        ASSERT_EQ(n3->GetChangedInfoOnlyPaths().size(), 0);
    }

    cache.MergeAll();

    // Ensure that we have one merged notice after consolidation.
    ASSERT_EQ(cache.Size(), 1);

    // Ensure that the content of the notice is correct.
    {
        auto& n = cache.GetAll().at(0);
        ASSERT_EQ(n->GetResyncedPaths().size(), 3);
        ASSERT_EQ(n->GetResyncedPaths().at(0), PXR_NS::SdfPath {"/Foo"});
        ASSERT_EQ(n->GetResyncedPaths().at(1), PXR_NS::SdfPath {"/Bar"});
        ASSERT_EQ(n->GetResyncedPaths().at(2), PXR_NS::SdfPath {"/Baz"});
        ASSERT_EQ(n->GetChangedInfoOnlyPaths().size(), 0);
    }
}

TEST(AddPrim, StageContentsChanged)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    namespace _Broker = PXR_NS::UsdBrokerNotice;

    PXR_NS::NoticeCache<_Broker::StageContentsChanged> cache;

    // Edit the stage...
    stage->DefinePrim(PXR_NS::SdfPath {"/Foo"});

    // Ensure that one notice has been cached.
    ASSERT_EQ(cache.Size(), 1);

    cache.MergeAll();

    // Ensure that we have still one notice after consolidation.
    ASSERT_EQ(cache.Size(), 1);
}

TEST(AddPrims, StageContentsChanged)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    namespace _Broker = PXR_NS::UsdBrokerNotice;

    PXR_NS::NoticeCache<_Broker::StageContentsChanged> cache;

    // Edit the stage...
    stage->DefinePrim(PXR_NS::SdfPath {"/Foo"});
    stage->DefinePrim(PXR_NS::SdfPath {"/Bar"});
    stage->DefinePrim(PXR_NS::SdfPath {"/Baz"});

    // Ensure that three notices have been cached.
    ASSERT_EQ(cache.Size(), 3);

    cache.MergeAll();

    // Ensure that we have one merged notice after consolidation.
    ASSERT_EQ(cache.Size(), 1);
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

    PXR_NS::NoticeCache<_Broker::ObjectsChanged> cache;

    // Edit the stage...
    stage->MuteLayer(layers[0]);

    // Ensure that one notice has been cached.
    ASSERT_EQ(cache.Size(), 1);

    // Ensure that notice data is as expected.
    {
        auto& n = cache.GetAll().at(0);
        ASSERT_EQ(n->GetResyncedPaths().size(), 1);
        ASSERT_EQ(n->GetResyncedPaths().at(0), PXR_NS::SdfPath {"/"});
        ASSERT_EQ(n->GetChangedInfoOnlyPaths().size(), 0);
    }

    cache.MergeAll();

    // Ensure that we have still one notice after consolidation.
    ASSERT_EQ(cache.Size(), 1);

    // Ensure that the content of the notice didn't change.
    {
        auto& n = cache.GetAll().at(0);
        ASSERT_EQ(n->GetResyncedPaths().size(), 1);
        ASSERT_EQ(n->GetResyncedPaths().at(0), PXR_NS::SdfPath {"/"});
        ASSERT_EQ(n->GetChangedInfoOnlyPaths().size(), 0);
    }
}

TEST(MuteLayers, ObjectsChanged)
{
    auto stage = ::Test::CreateStageWithLayers();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    // Create prims before caching to trigger resync path when muting.
    auto layers = stage->GetRootLayer()->GetSubLayerPaths();
    auto layer1 = PXR_NS::SdfLayer::Find(layers[0]);
    stage->SetEditTarget(PXR_NS::UsdEditTarget(layer1));
    stage->DefinePrim(PXR_NS::SdfPath {"/Foo"});

    // Keep ref pointer to the layer we try to mute and unmute to
    // prevent it for being destroyed when it is muted.
    auto layer2 = PXR_NS::SdfLayer::FindOrOpen(layers[1]);
    stage->SetEditTarget(PXR_NS::UsdEditTarget(layer2));
    stage->DefinePrim(PXR_NS::SdfPath {"/Bar"});

    namespace _Broker = PXR_NS::UsdBrokerNotice;

    PXR_NS::NoticeCache<_Broker::ObjectsChanged> cache;

    // Edit the stage...
    stage->MuteLayer(layers[0]);
    stage->MuteLayer(layers[1]);
    stage->UnmuteLayer(layers[1]);
    stage->MuteAndUnmuteLayers(
        std::vector<std::string>{layers[2], layers[1]},
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

TEST(MuteLayer, StageContentsChanged)
{
    auto stage = ::Test::CreateStageWithLayers();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    namespace _Broker = PXR_NS::UsdBrokerNotice;

    PXR_NS::NoticeCache<_Broker::StageContentsChanged> cache;

    // Edit the stage...
    auto layers = stage->GetRootLayer()->GetSubLayerPaths();
    stage->MuteLayer(layers[0]);

    // Ensure that one notice has been cached.
    ASSERT_EQ(cache.Size(), 1);

    cache.MergeAll();

    // Ensure that we have still one notice after consolidation.
    ASSERT_EQ(cache.Size(), 1);
}

TEST(MuteLayers, StageContentsChanged)
{
    auto stage = ::Test::CreateStageWithLayers();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    namespace _Broker = PXR_NS::UsdBrokerNotice;

    PXR_NS::NoticeCache<_Broker::StageContentsChanged> cache;

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

    // Ensure that four notices have been cached.
    ASSERT_EQ(cache.Size(), 4);

    cache.MergeAll();

    // Ensure that we have one merged notice after consolidation.
    ASSERT_EQ(cache.Size(), 1);
}

TEST(MuteLayer, LayerMutingChanged)
{
    auto stage = ::Test::CreateStageWithLayers();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    namespace _Broker = PXR_NS::UsdBrokerNotice;

    PXR_NS::NoticeCache<_Broker::LayerMutingChanged> cache;

    // Edit the stage...
    auto layers = stage->GetRootLayer()->GetSubLayerPaths();
    stage->MuteLayer(layers[0]);

    // Ensure that one notice has been cached.
    ASSERT_EQ(cache.Size(), 1);

    // Ensure that notice data is as expected.
    {
        auto& n = cache.GetAll().at(0);
        ASSERT_EQ(n->GetMutedLayers().size(), 1);
        ASSERT_EQ(n->GetMutedLayers().at(0), std::string(layers[0]));
        ASSERT_EQ(n->GetUnmutedLayers().size(), 0);
    }

    cache.MergeAll();

    // Ensure that we have still one notice after consolidation.
    ASSERT_EQ(cache.Size(), 1);

    // Ensure that the content of the notice didn't change.
    {
        auto& n = cache.GetAll().at(0);
        ASSERT_EQ(n->GetMutedLayers().size(), 1);
        ASSERT_EQ(n->GetMutedLayers().at(0), std::string(layers[0]));
        ASSERT_EQ(n->GetUnmutedLayers().size(), 0);
    }
}

TEST(MuteLayers, LayerMutingChanged)
{
    auto stage = ::Test::CreateStageWithLayers();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    namespace _Broker = PXR_NS::UsdBrokerNotice;

    PXR_NS::NoticeCache<_Broker::LayerMutingChanged> cache;

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

    // Ensure that four notices have been cached.
    ASSERT_EQ(cache.Size(), 4);

    // Ensure that notice data are all as expected.
    {
        auto& n1 = cache.GetAll().at(0);
        ASSERT_EQ(n1->GetMutedLayers().size(), 1);
        ASSERT_EQ(n1->GetMutedLayers().at(0), std::string(layers[0]));
        ASSERT_EQ(n1->GetUnmutedLayers().size(), 0);

        auto& n2 = cache.GetAll().at(1);
        ASSERT_EQ(n2->GetMutedLayers().size(), 1);
        ASSERT_EQ(n2->GetMutedLayers().at(0), std::string(layers[1]));
        ASSERT_EQ(n2->GetUnmutedLayers().size(), 0);

        auto& n3 = cache.GetAll().at(2);
        ASSERT_EQ(n3->GetMutedLayers().size(), 0);
        ASSERT_EQ(n3->GetUnmutedLayers().size(), 1);
        ASSERT_EQ(n3->GetUnmutedLayers().at(0), std::string(layers[1]));

        auto& n4 = cache.GetAll().at(3);
        ASSERT_EQ(n4->GetMutedLayers().size(), 2);
        ASSERT_EQ(n4->GetMutedLayers().at(0), std::string(layers[2]));
        ASSERT_EQ(n4->GetMutedLayers().at(1), std::string(layers[1]));
        ASSERT_EQ(n4->GetUnmutedLayers().size(), 0);
    }

    cache.MergeAll();

    // Ensure that we have one merged notice after consolidation.
    ASSERT_EQ(cache.Size(), 1);

    // Ensure that the content of the notice is correct.
    {
        auto& n = cache.GetAll().at(0);
        ASSERT_EQ(n->GetMutedLayers().size(), 3);
        ASSERT_EQ(n->GetMutedLayers().at(0), std::string(layers[0]));
        ASSERT_EQ(n->GetMutedLayers().at(1), std::string(layers[2]));
        ASSERT_EQ(n->GetMutedLayers().at(2), std::string(layers[1]));
        ASSERT_EQ(n->GetUnmutedLayers().size(), 0);
    }
}

TEST(ChangeEditTarget, StageEditTargetChanged)
{
    auto stage = ::Test::CreateStageWithLayers();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    namespace _Broker = PXR_NS::UsdBrokerNotice;

    PXR_NS::NoticeCache<_Broker::StageEditTargetChanged> cache;

    // Edit the stage...
    auto layers = stage->GetRootLayer()->GetSubLayerPaths();
    auto layer1 = PXR_NS::SdfLayer::Find(layers[0]);
    auto layer2 = PXR_NS::SdfLayer::Find(layers[1]);

    stage->SetEditTarget(PXR_NS::UsdEditTarget(layer1));
    stage->SetEditTarget(PXR_NS::UsdEditTarget(layer2));

    // Ensure that two notices have been cached.
    ASSERT_EQ(cache.Size(), 2);

    cache.MergeAll();

    // Ensure that we have one merged notice after consolidation.
    ASSERT_EQ(cache.Size(), 1);
}

TEST(CustomNotices, MergeableNotice)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    PXR_NS::NoticeCache<::Test::MergeableNotice> cache;

    broker->Send<::Test::MergeableNotice>(::Test::DataMap({{"Foo", "Test1"}}));
    broker->Send<::Test::MergeableNotice>(::Test::DataMap({{"Foo", "Test2"}}));
    broker->Send<::Test::MergeableNotice>(::Test::DataMap({{"Bar", "Test3"}}));

    // Ensure that three notices have been cached.
    ASSERT_EQ(cache.Size(), 3);

    // Ensure that notice data are all as expected.
    {
        auto& n1 = cache.GetAll().at(0);
        ASSERT_EQ(n1->GetData(), ::Test::DataMap({{"Foo", "Test1"}}));

        auto& n2 = cache.GetAll().at(1);
        ASSERT_EQ(n2->GetData(), ::Test::DataMap({{"Foo", "Test2"}}));

        auto& n3 = cache.GetAll().at(2);
        ASSERT_EQ(n3->GetData(), ::Test::DataMap({{"Bar", "Test3"}}));
    }

    cache.MergeAll();

    // Ensure that we have one merged notice after consolidation.
    ASSERT_EQ(cache.Size(), 1);

    // Ensure that the content of the notice is correct.
    {
        auto& n = cache.GetAll().at(0);
        ASSERT_EQ(n->GetData(),
            ::Test::DataMap({{"Foo", "Test2"}, {"Bar", "Test3"}}));
    }
}

TEST(CustomNotices, UnMergeableNotice)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    PXR_NS::NoticeCache<::Test::UnMergeableNotice> cache;

    broker->Send<::Test::UnMergeableNotice>();
    broker->Send<::Test::UnMergeableNotice>();
    broker->Send<::Test::UnMergeableNotice>();

    // Ensure that three notices have been cached.
    ASSERT_EQ(cache.Size(), 3);

    cache.MergeAll();

    // Ensure that we still have three notice after consolidation.
    ASSERT_EQ(cache.Size(), 3);
}
