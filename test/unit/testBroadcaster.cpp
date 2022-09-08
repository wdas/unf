#include "unf/hierarchycache.h"
#include "unf/hierarchyBroadcaster.h"
#include "unf/notice.h"
#include "unf/broker.h"
#include "unf/transaction.h"

#include "unfTest/listener.h"
#include "unfTest/testBroadcaster.h"

#include <gtest/gtest.h>
#include <pxr/pxr.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/base/vt/dictionary.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/variantSets.h>

#include <cstdlib>
#include <string>

namespace {
    std::string GetTestFilePath(const std::string& filePath) {
        std::string root = std::getenv("USD_TEST_PATH");
        return root + filePath;
    }

    UsdStageRefPtr GetStage(const std::string& filePath) {
        return PXR_NS::UsdStage::Open(GetTestFilePath(filePath));
    }
}

using HierarchyBroadcasterRefPtr = PXR_NS::TfRefPtr<PXR_NS::unf::HierarchyBroadcaster>;
using ChildBroadcasterRefPtr = PXR_NS::TfRefPtr<::Test::ChildBroadcaster>;

TEST(Broadcaster, HierarchyBroadcasterInitialize)
{
    auto stage = GetStage("/scene.usda");
    auto broker = PXR_NS::unf::Broker::Create(stage);
    broker->AddBroadcaster<PXR_NS::unf::HierarchyBroadcaster>();
    
     // Ensure that the hierarchy broadcaster type is correct.
    auto broadcaster = broker->GetBroadcaster("HierarchyBroadcaster");
    ASSERT_TRUE(PXR_NS::TfDynamic_cast<HierarchyBroadcasterRefPtr>(broadcaster));
}

TEST(Broadcaster, HierarchyBroadcasterNoticeSimple)
{
    auto stage = GetStage("/scene.usda");
    auto broker = PXR_NS::unf::Broker::Create(stage);
    broker->AddBroadcaster<PXR_NS::unf::HierarchyBroadcaster>();

    ::Test::HierarchyChangedListener l = ::Test::HierarchyChangedListener();
    
    stage->DefinePrim(SdfPath("/scene/AA"));
    
    ASSERT_EQ(l.GetCount(), 1);
    
    ASSERT_EQ(l.GetAdded().size(), 1);
    ASSERT_EQ(l.GetModified().size(), 0);
    ASSERT_EQ(l.GetRemoved().size(), 0);
    ASSERT_NE(l.GetAdded().find(SdfPath("/scene/AA")), l.GetAdded().end());
    
    UsdPrim p = stage->GetPrimAtPath(SdfPath("/scene/testvariant1/V"));
    p.GetVariantSet("myVariant").SetVariantSelection("v");

    //The HierarchyChangedNotice should contain lists of added/removed/modified prims that are collapsed (i.e. no descendants)
    ASSERT_EQ(l.GetAdded().size(), 1);
    ASSERT_EQ(l.GetRemoved().size(), 2);
    ASSERT_EQ(l.GetModified().size(), 1);
    ASSERT_NE(l.GetRemoved().find(SdfPath("/scene/testvariant1/V/SphereGroup2")), l.GetRemoved().end());
}

TEST(Broadcaster, HierarchyBroadcasterTransaction)
{
    auto stage = GetStage("/scene.usda");
    auto broker = PXR_NS::unf::Broker::Create(stage);
    broker->AddBroadcaster<PXR_NS::unf::HierarchyBroadcaster>();
    
    ::Test::HierarchyChangedListener l = ::Test::HierarchyChangedListener();
    
    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

        stage->DefinePrim(SdfPath("/scene/K/J"));
        stage->DefinePrim(SdfPath("/scene/K/M"));
        stage->DefinePrim(SdfPath("/scene/K/M/L"));
    }
    ASSERT_EQ(l.GetModified().size(), 0);
    ASSERT_EQ(l.GetAdded().size(), 1);
    ASSERT_EQ(l.GetRemoved().size(), 0);
    
    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

        stage->DefinePrim(SdfPath("/scene/M/J"));
        stage->DefinePrim(SdfPath("/scene/M/N"));
        stage->RemovePrim(SdfPath("/scene/M/N"));
    }
    ASSERT_EQ(l.GetModified().size(), 0);
    ASSERT_EQ(l.GetAdded().size(), 1);
    ASSERT_EQ(l.GetRemoved().size(), 0);
    
    l.ResetCount();
    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

        stage->DefinePrim(SdfPath("/scene/RemovePrim"));
        stage->RemovePrim(SdfPath("/scene/RemovePrim"));
    }
    //The transaction added, then deleted the prim -- therefore, there was no change to the cache and no notice should be emitted
    ASSERT_EQ(l.GetCount(), 0);
    
    l.ResetCount();
    
    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

        stage->RemovePrim(SdfPath("/scene/K/M"));
        stage->DefinePrim(SdfPath("/scene/K/M/L"));
    }
    ASSERT_EQ(l.GetModified().size(), 1);
    ASSERT_EQ(l.GetAdded().size(), 0);
    ASSERT_EQ(l.GetRemoved().size(), 0);
    
    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

        UsdPrim p2 = stage->GetPrimAtPath(SdfPath("/scene/D/a"));
        p2.SetInstanceable(true);
        stage->DefinePrim(SdfPath("/scene/sublayerShared/K/k2"));
        stage->RemovePrim(SdfPath("/scene/sublayerShared/K/k2"));
    }
    
    ASSERT_EQ(l.GetModified().size(), 1);
    ASSERT_EQ(l.GetAdded().size(), 1);
    ASSERT_EQ(l.GetRemoved().size(), 0);  
    ASSERT_EQ(l.GetChangedFields().size(), 1);
    
    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

        std::string layerIdentifier = GetTestFilePath("/sublayer2.usda");
        stage->MuteLayer(layerIdentifier);
        stage->UnmuteLayer(layerIdentifier);
    }
    
    ASSERT_EQ(l.GetModified().size(), 1);
    ASSERT_EQ(l.GetAdded().size(), 0);
    ASSERT_EQ(l.GetRemoved().size(), 0);

    std::string layer1Identifier = GetTestFilePath("/sublayer.usda");
    std::string layer2Identifier = GetTestFilePath("/sublayer2.usda");
    
    stage->MuteLayer(layer1Identifier);
    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

        stage->MuteLayer(layer2Identifier);
        stage->UnmuteLayer(layer1Identifier);
    }
    
    ASSERT_EQ(l.GetModified().size(), 1);
    ASSERT_EQ(l.GetAdded().size(), 3);
    ASSERT_EQ(l.GetRemoved().size(), 4);
}

TEST(Broadcaster, HierarchyBroadcasterTransactionNested) {
    auto stage = GetStage("/scene.usda");
    auto broker = PXR_NS::unf::Broker::Create(stage);
    broker->AddBroadcaster<PXR_NS::unf::HierarchyBroadcaster>();
    
    ::Test::HierarchyChangedListener l = ::Test::HierarchyChangedListener();
    /*
    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

        stage->DefinePrim(SdfPath("/scene/K/J"));
        {
            PXR_NS::unf::NoticeTransaction transaction(broker);
            stage->DefinePrim(SdfPath("/scene/K/M"));
            stage->DefinePrim(SdfPath("/scene/K/M/L"));
        }
    }
    ASSERT_EQ(l.GetCount(), 1);
    ASSERT_EQ(l.GetModified().size(), 0);
    ASSERT_EQ(l.GetAdded().size(), 1);
    ASSERT_EQ(l.GetRemoved().size(), 0);
    l.ResetCount();
    */
    
    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

        stage->DefinePrim(SdfPath("/scene/K/J"));
        stage->DefinePrim(SdfPath("/scene/K/M"));
        stage->DefinePrim(SdfPath("/scene/K/M/L"));
        {
            PXR_NS::unf::NoticeTransaction transaction(broker);
            stage->DefinePrim(SdfPath("/scene/M/J"));
            stage->DefinePrim(SdfPath("/scene/M/N"));
            stage->RemovePrim(SdfPath("/scene/M/N"));
        }
    }
    ASSERT_EQ(l.GetCount(), 1);
    ASSERT_EQ(l.GetModified().size(), 0);
    ASSERT_EQ(l.GetAdded().size(), 2);
    ASSERT_EQ(l.GetRemoved().size(), 0);
    l.ResetCount();

    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

        UsdPrim p = stage->GetPrimAtPath(SdfPath("/scene/D/a"));
        p.SetInstanceable(true);
        stage->DefinePrim(SdfPath("/scene/sublayerShared/K/k2"));
        stage->RemovePrim(SdfPath("/scene/sublayerShared/K/k2"));
        {
            PXR_NS::unf::NoticeTransaction transaction(broker);
            
            UsdPrim p = stage->GetPrimAtPath(SdfPath("/scene/test/a"));
            p.SetInstanceable(true);
        }
    }
    ASSERT_EQ(l.GetCount(), 1);
    ASSERT_EQ(l.GetModified().size(), 2);
    ASSERT_EQ(l.GetAdded().size(), 1);
    ASSERT_EQ(l.GetRemoved().size(), 0);  
    ASSERT_EQ(l.GetChangedFields().size(), 2);
    l.ResetCount();

    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

        stage->RemovePrim(SdfPath("/scene/K/M"));
        stage->DefinePrim(SdfPath("/scene/K/M/L"));

        stage->DefinePrim(SdfPath("/scene/E/P"));
        
        {
            PXR_NS::unf::NoticeTransaction transaction(broker);

            stage->DefinePrim(SdfPath("/scene/E/N"));
            stage->RemovePrim(SdfPath("/scene/E"));
        }
    }
    ASSERT_EQ(l.GetCount(), 1);
    ASSERT_EQ(l.GetModified().size(), 1);
    ASSERT_EQ(l.GetAdded().size(), 0);
    ASSERT_EQ(l.GetRemoved().size(), 0);
    l.ResetCount();

    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

        stage->RemovePrim(SdfPath("/scene/K/M"));

        {
            PXR_NS::unf::NoticeTransaction transaction(broker);
            stage->DefinePrim(SdfPath("/scene/K/M/L"));
        }
    }
    ASSERT_EQ(l.GetCount(), 1);
    ASSERT_EQ(l.GetModified().size(), 1);
    ASSERT_EQ(l.GetAdded().size(), 0);
    ASSERT_EQ(l.GetRemoved().size(), 0);
    l.ResetCount();
}

TEST(Broadcaster, ChildBroadcasterInitialize) {
    auto stage = GetStage("/scene.usda");
    auto broker = PXR_NS::unf::Broker::Create(stage);
    broker->AddBroadcaster<PXR_NS::unf::HierarchyBroadcaster>();
    broker->AddBroadcaster<::Test::ChildBroadcaster>();
    
    // Ensure that the hierarchy broadcaster type is correct.
    auto hierarchybroadcaster = broker->GetBroadcaster("HierarchyBroadcaster");
    ASSERT_TRUE(PXR_NS::TfDynamic_cast<HierarchyBroadcasterRefPtr>(hierarchybroadcaster));

    // Ensure that the child broadcaster type is correct.
    auto childbroadcaster = broker->GetBroadcaster("ChildBroadcaster");
    ASSERT_TRUE(PXR_NS::TfDynamic_cast<ChildBroadcasterRefPtr>(childbroadcaster));
}

TEST(Broadcaster, ChildBroadcasterNoticeSimple) {
    auto stage = GetStage("/scene.usda");
    auto broker = PXR_NS::unf::Broker::Create(stage);
    broker->AddBroadcaster<PXR_NS::unf::HierarchyBroadcaster>();
    broker->AddBroadcaster<::Test::ChildBroadcaster>();
    
    auto hierarchybroadcaster = broker->GetBroadcaster("HierarchyBroadcaster");
    auto childbroadcaster = broker->GetBroadcaster("ChildBroadcaster");
    
    ::Test::ChildBroadcasterNoticeListener l = ::Test::ChildBroadcasterNoticeListener();

    stage->DefinePrim(SdfPath("/scene/AA"));
    
    ASSERT_EQ(l.GetCount(), 1);
    
    ASSERT_EQ(l.GetAdded().size(), 1);
    ASSERT_EQ(l.GetModified().size(), 0);
    ASSERT_EQ(l.GetRemoved().size(), 0);
    ASSERT_NE(l.GetAdded().find(SdfPath("/scene/AA")), l.GetAdded().end());
    
    UsdPrim p = stage->GetPrimAtPath(SdfPath("/scene/testvariant1/V"));
    p.GetVariantSet("myVariant").SetVariantSelection("v");

    //The ChildBroadcasterNotice should have a list of non-collapsed notices (i.e. full list of changed prims w/ descendants)
    ASSERT_EQ(l.GetAdded().size(), 1);
    ASSERT_EQ(l.GetRemoved().size(), 2);
    ASSERT_EQ(l.GetModified().size(), 4);
    ASSERT_NE(l.GetRemoved().find(SdfPath("/scene/testvariant1/V/SphereGroup2")), l.GetRemoved().end());
    
}

TEST(Broadcaster, ChildBroadcasterTransaction)
{
    auto stage = GetStage("/scene.usda");
    auto broker = PXR_NS::unf::Broker::Create(stage);
    broker->AddBroadcaster<PXR_NS::unf::HierarchyBroadcaster>();
    broker->AddBroadcaster<::Test::ChildBroadcaster>();
    
    ::Test::ChildBroadcasterNoticeListener l = ::Test::ChildBroadcasterNoticeListener();
    
    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

        stage->DefinePrim(SdfPath("/scene/K/J"));
        stage->DefinePrim(SdfPath("/scene/K/M"));
        stage->DefinePrim(SdfPath("/scene/K/M/L"));
    }
    ASSERT_EQ(l.GetModified().size(), 0);
    ASSERT_EQ(l.GetAdded().size(), 4);
    ASSERT_EQ(l.GetRemoved().size(), 0);
    l.ResetCount();

    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

        stage->DefinePrim(SdfPath("/scene/RemovePrim"));
        stage->RemovePrim(SdfPath("/scene/RemovePrim"));
    }
    //The transaction added, then deleted the prim -- therefore, there was no change to the cache and the child broadcasters should not be executed.
    ASSERT_EQ(l.GetCount(), 0);
    
    l.ResetCount();
    
    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

        stage->RemovePrim(SdfPath("/scene/K/M"));
        stage->DefinePrim(SdfPath("/scene/K/M/L"));
    }
    ASSERT_EQ(l.GetModified().size(), 2);
    ASSERT_EQ(l.GetAdded().size(), 0);
    ASSERT_EQ(l.GetRemoved().size(), 0);
    
    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

        UsdPrim p2 = stage->GetPrimAtPath(SdfPath("/scene/sublayerShared"));
        p2.SetInstanceable(true);
        stage->DefinePrim(SdfPath("/scene/sublayerShared/K/k2"));
        stage->RemovePrim(SdfPath("/scene/sublayerShared/K/k2"));
    }
    
    ASSERT_EQ(l.GetModified().size(), 3);
    ASSERT_EQ(l.GetAdded().size(), 1);
    ASSERT_EQ(l.GetRemoved().size(), 0);  
    ASSERT_EQ(l.GetChangedFields().size(), 1);
    
    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

        std::string layerIdentifier = GetTestFilePath("/sublayer2.usda");
        stage->MuteLayer(layerIdentifier);
        stage->UnmuteLayer(layerIdentifier);
    }
    
    ASSERT_EQ(l.GetModified().size(), 37);
    ASSERT_EQ(l.GetAdded().size(), 0);
    ASSERT_EQ(l.GetRemoved().size(), 0);

    std::string layer1Identifier = GetTestFilePath("/sublayer.usda");
    std::string layer2Identifier = GetTestFilePath("/sublayer2.usda");
    
    stage->MuteLayer(layer1Identifier);
    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

        stage->MuteLayer(layer2Identifier);
        stage->UnmuteLayer(layer1Identifier);
    }
    
    ASSERT_EQ(l.GetModified().size(), 29);
    ASSERT_EQ(l.GetAdded().size(), 3);
    ASSERT_EQ(l.GetRemoved().size(), 5);
}

TEST(Broadcaster, ChildBroadcasterTransactionNested)
{
    auto stage = GetStage("/scene.usda");
    auto broker = PXR_NS::unf::Broker::Create(stage);
    broker->AddBroadcaster<PXR_NS::unf::HierarchyBroadcaster>();
    broker->AddBroadcaster<::Test::ChildBroadcaster>();
    
    ::Test::ChildBroadcasterNoticeListener l = ::Test::ChildBroadcasterNoticeListener();
    
    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

        stage->DefinePrim(SdfPath("/scene/K/J"));
        {
            PXR_NS::unf::NoticeTransaction transaction(broker);
            stage->DefinePrim(SdfPath("/scene/K/M"));
            stage->DefinePrim(SdfPath("/scene/K/M/L"));
        }
    }
    //Child Broadcaster should have been executed twice -- at the end of each nested transaction.
    ASSERT_EQ(l.GetCount(), 2);
    l.ResetCount();
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
