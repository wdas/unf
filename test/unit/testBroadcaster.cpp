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

    PXR_NS::UsdStageRefPtr GetStage(const std::string& filePath) {
        return PXR_NS::UsdStage::Open(GetTestFilePath(filePath));
    }
}

using HierarchyBroadcasterRefPtr = PXR_NS::TfRefPtr<unf::HierarchyBroadcaster>;
using ChildBroadcasterRefPtr = PXR_NS::TfRefPtr<::Test::ChildBroadcaster>;

TEST(Broadcaster, HierarchyBroadcasterInitialize)
{
    auto stage = GetStage("/scene.usda");
    auto broker = unf::Broker::Create(stage);
    broker->AddBroadcaster<unf::HierarchyBroadcaster>();

     // Ensure that the hierarchy broadcaster type is correct.
    auto broadcaster = broker->GetBroadcaster("HierarchyBroadcaster");
    ASSERT_TRUE(PXR_NS::TfDynamic_cast<HierarchyBroadcasterRefPtr>(broadcaster));
}

TEST(Broadcaster, HierarchyBroadcasterNoticeSimple)
{
    auto stage = GetStage("/scene.usda");
    auto broker = unf::Broker::Create(stage);
    broker->AddBroadcaster<unf::HierarchyBroadcaster>();

    ::Test::HierarchyChangedListener l = ::Test::HierarchyChangedListener();

    stage->DefinePrim(PXR_NS::SdfPath("/scene/AA"));

    ASSERT_EQ(l.GetCount(), 1);

    ASSERT_EQ(l.GetAdded().size(), 1);
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/AA"))), l.GetAdded().end());
    ASSERT_EQ(l.GetModified().size(), 0);
    ASSERT_EQ(l.GetRemoved().size(), 0);

    PXR_NS::UsdPrim p = stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/testvariant1/V"));
    p.GetVariantSet("myVariant").SetVariantSelection("v");


    ASSERT_EQ(l.GetRemoved(), (PXR_NS::SdfPathVector{PXR_NS::SdfPath("/scene/testvariant1/V/SphereGroup1/emptyPrim/something1"), PXR_NS::SdfPath("/scene/testvariant1/V/SphereGroup2")}));
    ASSERT_EQ(l.GetModified(), PXR_NS::SdfPathVector{PXR_NS::SdfPath("/scene/testvariant1/V")});

    //The HierarchyChangedNotice should contain lists of added/removed/modified prims that are collapsed (i.e. no descendants)
    ASSERT_EQ(l.GetAdded().size(), 1);
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/testvariant1/V/SphereGroup1/emptyPrim/something2"))), l.GetAdded().end());
    ASSERT_EQ(l.GetRemoved().size(), 2);
    ASSERT_NE((std::find(l.GetRemoved().begin(), l.GetRemoved().end(), PXR_NS::SdfPath("/scene/testvariant1/V/SphereGroup1/emptyPrim/something1"))), l.GetRemoved().end());
    ASSERT_NE((std::find(l.GetRemoved().begin(), l.GetRemoved().end(), PXR_NS::SdfPath("/scene/testvariant1/V/SphereGroup2"))), l.GetRemoved().end());
    ASSERT_EQ(l.GetModified().size(), 1);
    ASSERT_NE((std::find(l.GetModified().begin(), l.GetModified().end(), PXR_NS::SdfPath("/scene/testvariant1/V"))), l.GetModified().end());
}

TEST(Broadcaster, HierarchyBroadcasterTransaction)
{
    auto stage = GetStage("/scene.usda");
    auto broker = unf::Broker::Create(stage);
    broker->AddBroadcaster<unf::HierarchyBroadcaster>();

    ::Test::HierarchyChangedListener l = ::Test::HierarchyChangedListener();

    {
        unf::NoticeTransaction transaction(broker);

        stage->DefinePrim(PXR_NS::SdfPath("/scene/K/J"));
        stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M"));
        stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M/L"));
    }
    ASSERT_EQ(l.GetModified().size(), 0);
    ASSERT_EQ(l.GetAdded().size(), 1);
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/K"))), l.GetAdded().end());
    ASSERT_EQ(l.GetRemoved().size(), 0);

    {
        unf::NoticeTransaction transaction(broker);

        stage->DefinePrim(PXR_NS::SdfPath("/scene/M/J"));
        stage->DefinePrim(PXR_NS::SdfPath("/scene/M/N"));
        stage->RemovePrim(PXR_NS::SdfPath("/scene/M/N"));
    }
    ASSERT_EQ(l.GetModified().size(), 0);
    ASSERT_EQ(l.GetAdded().size(), 1);
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/M"))), l.GetAdded().end());
    ASSERT_EQ(l.GetRemoved().size(), 0);

    l.ResetCount();

    {
        unf::NoticeTransaction transaction(broker);

        stage->DefinePrim(PXR_NS::SdfPath("/scene/RemovePrim"));
        stage->RemovePrim(PXR_NS::SdfPath("/scene/RemovePrim"));
    }
    //The transaction added, then deleted the prim -- therefore, there was no change to the cache and no notice should be emitted
    ASSERT_EQ(l.GetCount(), 0);

    l.ResetCount();

    {
        unf::NoticeTransaction transaction(broker);

        stage->RemovePrim(PXR_NS::SdfPath("/scene/K/M"));
        stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M/L"));
    }
    ASSERT_EQ(l.GetModified().size(), 1);
    ASSERT_NE((std::find(l.GetModified().begin(), l.GetModified().end(), PXR_NS::SdfPath("/scene/K/M"))), l.GetModified().end());
    ASSERT_EQ(l.GetAdded().size(), 0);
    ASSERT_EQ(l.GetRemoved().size(), 0);

    l.ResetCount();
    {
        unf::NoticeTransaction transaction(broker);

        stage->RemovePrim(PXR_NS::SdfPath("/scene/K/M"));
        stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M/P"));
    }
    ASSERT_EQ(l.GetModified().size(), 1);
    ASSERT_NE((std::find(l.GetModified().begin(), l.GetModified().end(), PXR_NS::SdfPath("/scene/K/M"))), l.GetModified().end());
    ASSERT_EQ(l.GetAdded().size(), 1);
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/K/M/P"))), l.GetAdded().end());
    ASSERT_EQ(l.GetRemoved().size(), 1);
    ASSERT_NE((std::find(l.GetRemoved().begin(), l.GetRemoved().end(), PXR_NS::SdfPath("/scene/K/M/L"))), l.GetRemoved().end());

    {
        unf::NoticeTransaction transaction(broker);

        PXR_NS::UsdPrim p2 = stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/D/a"));
        p2.SetInstanceable(true);

        stage->DefinePrim(PXR_NS::SdfPath("/scene/sublayerShared/K/k2"));
        stage->RemovePrim(PXR_NS::SdfPath("/scene/sublayerShared/K/k2"));
    }

    ASSERT_EQ(l.GetModified().size(), 1);
    ASSERT_NE((std::find(l.GetModified().begin(), l.GetModified().end(), PXR_NS::SdfPath("/scene/D/a"))), l.GetModified().end());
    ASSERT_EQ(l.GetAdded().size(), 1);
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/sublayerShared/K"))), l.GetAdded().end());
    ASSERT_EQ(l.GetRemoved().size(), 0);
    ASSERT_EQ(l.GetChangedFields().size(), 1);

    {
        unf::NoticeTransaction transaction(broker);

        std::string layerIdentifier = GetTestFilePath("/sublayer2.usda");
        stage->MuteLayer(layerIdentifier);
        stage->UnmuteLayer(layerIdentifier);
    }

    ASSERT_EQ(l.GetModified().size(), 1);
    ASSERT_NE((std::find(l.GetModified().begin(), l.GetModified().end(), PXR_NS::SdfPath("/"))), l.GetModified().end());
    ASSERT_EQ(l.GetAdded().size(), 0);
    ASSERT_EQ(l.GetRemoved().size(), 0);

    std::string layer1Identifier = GetTestFilePath("/sublayer.usda");
    std::string layer2Identifier = GetTestFilePath("/sublayer2.usda");

    stage->MuteLayer(layer1Identifier);
    {
        unf::NoticeTransaction transaction(broker);

        stage->MuteLayer(layer2Identifier);
        stage->UnmuteLayer(layer1Identifier);
    }

    ASSERT_EQ(l.GetModified().size(), 1);
    ASSERT_NE((std::find(l.GetModified().begin(), l.GetModified().end(), PXR_NS::SdfPath("/"))), l.GetModified().end());
    ASSERT_EQ(l.GetAdded().size(), 3);
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/B/bb"))), l.GetAdded().end());
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/sublayer"))), l.GetAdded().end());
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild"))), l.GetAdded().end());
    ASSERT_EQ(l.GetRemoved().size(), 4);
    ASSERT_NE((std::find(l.GetRemoved().begin(), l.GetRemoved().end(), PXR_NS::SdfPath("/scene/B/bb2"))), l.GetRemoved().end());
    ASSERT_NE((std::find(l.GetRemoved().begin(), l.GetRemoved().end(), PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild2"))), l.GetRemoved().end());
    ASSERT_NE((std::find(l.GetRemoved().begin(), l.GetRemoved().end(), PXR_NS::SdfPath("/scene/G"))), l.GetRemoved().end());
    ASSERT_NE((std::find(l.GetRemoved().begin(), l.GetRemoved().end(), PXR_NS::SdfPath("/scene/sublayer2"))), l.GetRemoved().end());

}


TEST(Broadcaster, HierarchyBroadcasterTransactionNested) {
    auto stage = GetStage("/scene.usda");
    auto broker = unf::Broker::Create(stage);
    broker->AddBroadcaster<unf::HierarchyBroadcaster>();

    ::Test::HierarchyChangedListener l = ::Test::HierarchyChangedListener();

    
    {
        unf::NoticeTransaction transaction(broker);

        stage->DefinePrim(PXR_NS::SdfPath("/scene/P/J"));
        {
            unf::NoticeTransaction transaction(broker);
            stage->DefinePrim(PXR_NS::SdfPath("/scene/P/M"));
            stage->DefinePrim(PXR_NS::SdfPath("/scene/P/M/L"));
        }
    }
    ASSERT_EQ(l.GetCount(), 1);
    ASSERT_EQ(l.GetModified().size(), 0);
    ASSERT_EQ(l.GetAdded().size(), 1);
    ASSERT_EQ(l.GetRemoved().size(), 0);
    l.ResetCount();

    {
        unf::NoticeTransaction transaction(broker);

        stage->RemovePrim(PXR_NS::SdfPath("/scene/P"));
        {
            unf::NoticeTransaction transaction(broker);
            stage->DefinePrim(PXR_NS::SdfPath("/scene/P/K/L"));
        }
    }
    ASSERT_EQ(l.GetCount(), 1);
    ASSERT_EQ(l.GetModified().size(), 1);
    ASSERT_NE((std::find(l.GetModified().begin(), l.GetModified().end(), PXR_NS::SdfPath("/scene/P"))), l.GetModified().end());
    ASSERT_EQ(l.GetAdded().size(), 1);
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/P/K"))), l.GetAdded().end());
    ASSERT_EQ(l.GetRemoved().size(), 2);
    ASSERT_NE((std::find(l.GetRemoved().begin(), l.GetRemoved().end(), PXR_NS::SdfPath("/scene/P/M"))), l.GetRemoved().end());
    ASSERT_NE((std::find(l.GetRemoved().begin(), l.GetRemoved().end(), PXR_NS::SdfPath("/scene/P/J"))), l.GetRemoved().end());

    l.ResetCount();

    {
        unf::NoticeTransaction transaction(broker);

        stage->DefinePrim(PXR_NS::SdfPath("/scene/K/J"));
        stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M"));
        stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M/L"));
        {
            unf::NoticeTransaction transaction(broker);
            stage->DefinePrim(PXR_NS::SdfPath("/scene/M/J"));
            stage->DefinePrim(PXR_NS::SdfPath("/scene/M/N"));
            stage->RemovePrim(PXR_NS::SdfPath("/scene/M/N"));
        }
    }
    ASSERT_EQ(l.GetCount(), 1);
    ASSERT_EQ(l.GetModified().size(), 0);
    ASSERT_EQ(l.GetAdded().size(), 2);
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/K"))), l.GetAdded().end());
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/M"))), l.GetAdded().end());
    ASSERT_EQ(l.GetRemoved().size(), 0);
    l.ResetCount();

    {
        unf::NoticeTransaction transaction(broker);

        PXR_NS::UsdPrim p = stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/D/a"));
        p.SetInstanceable(true);
        stage->DefinePrim(PXR_NS::SdfPath("/scene/sublayerShared/K/k2"));
        stage->RemovePrim(PXR_NS::SdfPath("/scene/sublayerShared/K/k2"));
        {
            unf::NoticeTransaction transaction(broker);

            PXR_NS::UsdPrim p = stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/test/a"));
            p.SetInstanceable(true);
        }
    }
    ASSERT_EQ(l.GetCount(), 1);
    ASSERT_EQ(l.GetModified().size(), 2);
    ASSERT_NE((std::find(l.GetModified().begin(), l.GetModified().end(), PXR_NS::SdfPath("/scene/D/a"))), l.GetModified().end());
    ASSERT_NE((std::find(l.GetModified().begin(), l.GetModified().end(), PXR_NS::SdfPath("/scene/test/a"))), l.GetModified().end());
    ASSERT_EQ(l.GetAdded().size(), 1);
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/sublayerShared/K"))), l.GetAdded().end());
    ASSERT_EQ(l.GetRemoved().size(), 0);
    ASSERT_EQ(l.GetChangedFields().size(), 2);
    l.ResetCount();

    {
        unf::NoticeTransaction transaction(broker);

        stage->RemovePrim(PXR_NS::SdfPath("/scene/K/M"));
        stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M/L"));

        stage->DefinePrim(PXR_NS::SdfPath("/scene/E/P"));

        {
            unf::NoticeTransaction transaction(broker);

            stage->DefinePrim(PXR_NS::SdfPath("/scene/E/N"));
            stage->RemovePrim(PXR_NS::SdfPath("/scene/E"));
        }
    }
    ASSERT_EQ(l.GetCount(), 1);
    ASSERT_EQ(l.GetModified().size(), 1);
    ASSERT_NE((std::find(l.GetModified().begin(), l.GetModified().end(), PXR_NS::SdfPath("/scene/K/M"))), l.GetModified().end());
    ASSERT_EQ(l.GetAdded().size(), 0);
    ASSERT_EQ(l.GetRemoved().size(), 0);
    l.ResetCount();

    {
        unf::NoticeTransaction transaction(broker);

        stage->RemovePrim(PXR_NS::SdfPath("/scene/M/J"));

        {
            unf::NoticeTransaction transaction(broker);
            stage->DefinePrim(PXR_NS::SdfPath("/scene/M/J/L"));
        }
    }

    ASSERT_EQ(l.GetCount(), 1);
    ASSERT_EQ(l.GetModified().size(), 1);
    ASSERT_NE((std::find(l.GetModified().begin(), l.GetModified().end(), PXR_NS::SdfPath("/scene/M/J"))), l.GetModified().end());
    ASSERT_EQ(l.GetAdded().size(), 1);
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/M/J/L"))), l.GetAdded().end());
    ASSERT_EQ(l.GetRemoved().size(), 0);
    l.ResetCount();

    {
        unf::NoticeTransaction transaction(broker);

        stage->RemovePrim(PXR_NS::SdfPath("/scene/M/J"));

        {
            unf::NoticeTransaction transaction(broker);
            stage->DefinePrim(PXR_NS::SdfPath("/scene/M/J"));
        }
    }

    ASSERT_EQ(l.GetCount(), 1);
    ASSERT_EQ(l.GetModified().size(), 1);
    ASSERT_NE((std::find(l.GetModified().begin(), l.GetModified().end(), PXR_NS::SdfPath("/scene/M/J"))), l.GetModified().end());
    ASSERT_EQ(l.GetAdded().size(), 0);
    ASSERT_EQ(l.GetRemoved().size(), 1);
    ASSERT_NE((std::find(l.GetRemoved().begin(), l.GetRemoved().end(), PXR_NS::SdfPath("/scene/M/J/L"))), l.GetRemoved().end());
    l.ResetCount();

    PXR_NS::UsdPrim p = stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/sublayerShared"));
    p.SetActive(false);
    {
        unf::NoticeTransaction transaction(broker);

        p.SetActive(true);
        PXR_NS::UsdPrim p2 = stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/D"));
        p2.SetActive(false);
    }
    ASSERT_EQ(l.GetModified().size(), 2);
    ASSERT_NE((std::find(l.GetModified().begin(), l.GetModified().end(), PXR_NS::SdfPath("/scene/D"))), l.GetModified().end());
    ASSERT_NE((std::find(l.GetModified().begin(), l.GetModified().end(), PXR_NS::SdfPath("/scene/sublayerShared"))), l.GetModified().end());
    ASSERT_EQ(l.GetRemoved().size(), 4);
    ASSERT_NE((std::find(l.GetRemoved().begin(), l.GetRemoved().end(), PXR_NS::SdfPath("/scene/D/r2"))), l.GetRemoved().end());
    ASSERT_NE((std::find(l.GetRemoved().begin(), l.GetRemoved().end(), PXR_NS::SdfPath("/scene/D/b"))), l.GetRemoved().end());
    ASSERT_NE((std::find(l.GetRemoved().begin(), l.GetRemoved().end(), PXR_NS::SdfPath("/scene/D/r"))), l.GetRemoved().end());
    ASSERT_NE((std::find(l.GetRemoved().begin(), l.GetRemoved().end(), PXR_NS::SdfPath("/scene/D/a"))), l.GetRemoved().end());
    ASSERT_EQ(l.GetAdded().size(), 3);
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild"))), l.GetAdded().end());
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/sublayerShared/K"))), l.GetAdded().end());
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/sublayerShared/K"))), l.GetAdded().end());
    l.ResetCount();

    {
        unf::NoticeTransaction transaction(broker);
        PXR_NS::UsdPrim p = stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/A/a"));
        p.SetInstanceable(true);

        {
            unf::NoticeTransaction transaction(broker);
            PXR_NS::UsdPrim p2 = stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/A"));
            p2.SetInstanceable(true);
        }
    }

    ASSERT_EQ(l.GetCount(), 1);
    ASSERT_EQ(l.GetModified().size(), 1);
    ASSERT_NE((std::find(l.GetModified().begin(), l.GetModified().end(), PXR_NS::SdfPath("/scene/A"))), l.GetModified().end());
    ASSERT_EQ(l.GetAdded().size(), 0);
    ASSERT_EQ(l.GetRemoved().size(), 0);
    l.ResetCount();
}

TEST(Broadcaster, ChildBroadcasterInitialize) {
    auto stage = GetStage("/scene.usda");
    auto broker = unf::Broker::Create(stage);
    broker->AddBroadcaster<unf::HierarchyBroadcaster>();
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
    auto broker = unf::Broker::Create(stage);
    broker->AddBroadcaster<unf::HierarchyBroadcaster>();
    broker->AddBroadcaster<::Test::ChildBroadcaster>();

    auto hierarchybroadcaster = broker->GetBroadcaster("HierarchyBroadcaster");
    auto childbroadcaster = broker->GetBroadcaster("ChildBroadcaster");

    ::Test::ChildBroadcasterNoticeListener l = ::Test::ChildBroadcasterNoticeListener();

    stage->DefinePrim(PXR_NS::SdfPath("/scene/AA"));

    ASSERT_EQ(l.GetCount(), 1);
    ASSERT_EQ(l.GetAdded().size(), 1);
    ASSERT_NE(std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/AA")), l.GetAdded().end());
    ASSERT_EQ(l.GetModified().size(), 0);
    ASSERT_EQ(l.GetRemoved().size(), 0);

    PXR_NS::UsdPrim p = stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/testvariant1/V"));
    p.GetVariantSet("myVariant").SetVariantSelection("v");

    //The ChildBroadcasterNotice should have a list of non-collapsed notices (i.e. full list of changed prims w/ descendants)
    ASSERT_EQ(l.GetAdded().size(), 1);
    ASSERT_NE(std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/testvariant1/V/SphereGroup1/emptyPrim/something2")), l.GetAdded().end());
    ASSERT_EQ(l.GetRemoved().size(), 2);
    ASSERT_NE(std::find(l.GetRemoved().begin(), l.GetRemoved().end(), PXR_NS::SdfPath("/scene/testvariant1/V/SphereGroup2")), l.GetRemoved().end());
    ASSERT_NE(std::find(l.GetRemoved().begin(), l.GetRemoved().end(), PXR_NS::SdfPath("/scene/testvariant1/V/SphereGroup1/emptyPrim/something1")), l.GetRemoved().end());
    ASSERT_EQ(l.GetModified().size(), 4);
    ASSERT_NE(std::find(l.GetModified().begin(), l.GetModified().end(), PXR_NS::SdfPath("/scene/testvariant1/V")), l.GetModified().end());
    ASSERT_NE(std::find(l.GetModified().begin(), l.GetModified().end(), PXR_NS::SdfPath("/scene/testvariant1/V/SphereGroup1")), l.GetModified().end());
    ASSERT_NE(std::find(l.GetModified().begin(), l.GetModified().end(), PXR_NS::SdfPath("/scene/testvariant1/V/SphereGroup1/sphere")), l.GetModified().end());
    ASSERT_NE(std::find(l.GetModified().begin(), l.GetModified().end(), PXR_NS::SdfPath("/scene/testvariant1/V/SphereGroup1/emptyPrim")), l.GetModified().end());

}

TEST(Broadcaster, ChildBroadcasterTransaction)
{
    auto stage = GetStage("/scene.usda");
    auto broker = unf::Broker::Create(stage);
    broker->AddBroadcaster<unf::HierarchyBroadcaster>();
    broker->AddBroadcaster<::Test::ChildBroadcaster>();

    ::Test::ChildBroadcasterNoticeListener l = ::Test::ChildBroadcasterNoticeListener();

    {
        unf::NoticeTransaction transaction(broker);

        stage->DefinePrim(PXR_NS::SdfPath("/scene/K/J"));
        stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M"));
        stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M/L"));
    }
    ASSERT_EQ(l.GetModified().size(), 0);
    ASSERT_EQ(l.GetAdded().size(), 4);
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/K"))), l.GetAdded().end());
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/K/J"))), l.GetAdded().end());
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/K/M"))), l.GetAdded().end());
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/K/M/L"))), l.GetAdded().end());
    ASSERT_EQ(l.GetRemoved().size(), 0);
    l.ResetCount();

    {
        unf::NoticeTransaction transaction(broker);

        stage->DefinePrim(PXR_NS::SdfPath("/scene/RemovePrim"));
        stage->RemovePrim(PXR_NS::SdfPath("/scene/RemovePrim"));
    }
    //The transaction added, then deleted the prim -- therefore, there was no change to the cache and the child broadcasters should not be executed.
    ASSERT_EQ(l.GetCount(), 0);
    l.ResetCount();

    {
        unf::NoticeTransaction transaction(broker);

        stage->RemovePrim(PXR_NS::SdfPath("/scene/K/M"));
        stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M/L"));
    }
    ASSERT_EQ(l.GetModified().size(), 2);
    ASSERT_NE((std::find(l.GetModified().begin(), l.GetModified().end(), PXR_NS::SdfPath("/scene/K/M"))), l.GetModified().end());
    ASSERT_NE((std::find(l.GetModified().begin(), l.GetModified().end(), PXR_NS::SdfPath("/scene/K/M/L"))), l.GetModified().end());
    ASSERT_EQ(l.GetAdded().size(), 0);
    ASSERT_EQ(l.GetRemoved().size(), 0);
    l.ResetCount();

    {
        unf::NoticeTransaction transaction(broker);

        PXR_NS::UsdPrim p2 = stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/sublayerShared"));
        p2.SetInstanceable(true);
        stage->DefinePrim(PXR_NS::SdfPath("/scene/sublayerShared/K/k2"));
        stage->RemovePrim(PXR_NS::SdfPath("/scene/sublayerShared/K/k2"));
    }

    ASSERT_EQ(l.GetModified().size(), 3);
    ASSERT_NE((std::find(l.GetModified().begin(), l.GetModified().end(), PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild"))), l.GetModified().end());
    ASSERT_NE((std::find(l.GetModified().begin(), l.GetModified().end(), PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild2"))), l.GetModified().end());
    ASSERT_NE((std::find(l.GetModified().begin(), l.GetModified().end(), PXR_NS::SdfPath("/scene/sublayerShared"))), l.GetModified().end());
    ASSERT_EQ(l.GetAdded().size(), 1);
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/sublayerShared/K"))), l.GetAdded().end());
    ASSERT_EQ(l.GetRemoved().size(), 0);
    ASSERT_EQ(l.GetChangedFields().size(), 1);
    l.ResetCount();

    {
        unf::NoticeTransaction transaction(broker);

        std::string layerIdentifier = GetTestFilePath("/sublayer2.usda");
        stage->MuteLayer(layerIdentifier);
        stage->UnmuteLayer(layerIdentifier);
    }

    ASSERT_EQ(l.GetModified().size(), 37);
    ASSERT_EQ(l.GetAdded().size(), 0);
    ASSERT_EQ(l.GetRemoved().size(), 0);
    l.ResetCount();

    std::string layer1Identifier = GetTestFilePath("/sublayer.usda");
    std::string layer2Identifier = GetTestFilePath("/sublayer2.usda");

    stage->MuteLayer(layer1Identifier);
    {
        unf::NoticeTransaction transaction(broker);

        stage->MuteLayer(layer2Identifier);
        stage->UnmuteLayer(layer1Identifier);
    }

    ASSERT_EQ(l.GetModified().size(), 29);
    ASSERT_EQ(l.GetAdded().size(), 3);
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild"))), l.GetAdded().end());
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/sublayer"))), l.GetAdded().end());
    ASSERT_NE((std::find(l.GetAdded().begin(), l.GetAdded().end(), PXR_NS::SdfPath("/scene/B/bb"))), l.GetAdded().end());
    ASSERT_EQ(l.GetRemoved().size(), 5);
    ASSERT_NE((std::find(l.GetRemoved().begin(), l.GetRemoved().end(), PXR_NS::SdfPath("/scene/G"))), l.GetRemoved().end());
    ASSERT_NE((std::find(l.GetRemoved().begin(), l.GetRemoved().end(), PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild2"))), l.GetRemoved().end());
    ASSERT_NE((std::find(l.GetRemoved().begin(), l.GetRemoved().end(), PXR_NS::SdfPath("/scene/sublayer2"))), l.GetRemoved().end());
    ASSERT_NE((std::find(l.GetRemoved().begin(), l.GetRemoved().end(), PXR_NS::SdfPath("/scene/B/bb2"))), l.GetRemoved().end());
    ASSERT_NE((std::find(l.GetRemoved().begin(), l.GetRemoved().end(), PXR_NS::SdfPath("/scene/sublayer2/sublayer2Child"))), l.GetRemoved().end());
    l.ResetCount();
}

TEST(Broadcaster, ChildBroadcasterTransactionNested)
{
    auto stage = GetStage("/scene.usda");
    auto broker = unf::Broker::Create(stage);
    broker->AddBroadcaster<unf::HierarchyBroadcaster>();
    broker->AddBroadcaster<::Test::ChildBroadcaster>();

    ::Test::ChildBroadcasterNoticeListener l = ::Test::ChildBroadcasterNoticeListener();

    {
        unf::NoticeTransaction transaction(broker);

        stage->DefinePrim(PXR_NS::SdfPath("/scene/K/J"));
        {
            unf::NoticeTransaction transaction(broker);
            stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M"));
            stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M/L"));
        }
    }
    //Child Broadcaster should have been executed once.
    ASSERT_EQ(l.GetCount(), 1);
    l.ResetCount();
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
