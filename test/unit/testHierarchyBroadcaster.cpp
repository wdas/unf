#include <unf/hierarchyBroadcaster/broadcaster.h>
#include <unf/hierarchyBroadcaster/cache.h>

#include <unf/broker.h>
#include <unf/notice.h>
#include <unf/transaction.h>

#include <unfTest/observer.h>
#include <unfTest/childBroadcaster/broadcaster.h>

#include <gtest/gtest.h>
#include <pxr/base/vt/dictionary.h>
#include <pxr/pxr.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/variantSets.h>

#include <cstdlib>
#include <string>

class HierarchyBroadcasterTest : public ::testing::Test {
  protected:
    using HierarchyBroadcasterPtr = PXR_NS::TfRefPtr<unf::HierarchyBroadcaster>;
    using ChildBroadcasterPtr = PXR_NS::TfRefPtr<::Test::ChildBroadcaster>;

    void SetUp() override
    {
        _stage = PXR_NS::UsdStage::Open(GetTestFilePath("/scene.usda"));

        _broker = unf::Broker::Create(_stage);
        _broker->AddBroadcaster<unf::HierarchyBroadcaster>();
    }

    std::string GetTestFilePath(const std::string& filePath)
    {
        std::string root = std::getenv("USD_TEST_PATH");
        return root + filePath;
    }

    PXR_NS::UsdStageRefPtr _stage;
    unf::BrokerPtr _broker;
};

TEST_F(HierarchyBroadcasterTest, Initialize)
{
    // Ensure that the hierarchy broadcaster type is correct.
    auto b = _broker->GetBroadcaster("HierarchyBroadcaster");
    ASSERT_TRUE(PXR_NS::TfDynamic_cast<HierarchyBroadcasterPtr>(b));
}

TEST_F(HierarchyBroadcasterTest, NoticeSimple)
{
    ::Test::Observer<unf::BroadcasterNotice::HierarchyChanged> observer(_stage);

    _stage->DefinePrim(PXR_NS::SdfPath("/scene/AA"));

    ASSERT_EQ(observer.Received(), 1);

    const auto& n = observer.GetLatestNotice();
    ASSERT_EQ(n.GetAdded().size(), 1);
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/AA"))),
        n.GetAdded().end());
    ASSERT_EQ(n.GetModified().size(), 0);
    ASSERT_EQ(n.GetRemoved().size(), 0);

    PXR_NS::UsdPrim p =
        _stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/testvariant1/V"));
    p.GetVariantSet("myVariant").SetVariantSelection("v");


    ASSERT_EQ(
        n.GetRemoved(),
        (PXR_NS::SdfPathVector{
            PXR_NS::SdfPath(
                "/scene/testvariant1/V/SphereGroup1/emptyPrim/something1"),
            PXR_NS::SdfPath("/scene/testvariant1/V/SphereGroup2")}));
    ASSERT_EQ(
        n.GetModified(),
        PXR_NS::SdfPathVector{PXR_NS::SdfPath("/scene/testvariant1/V")});

    // The HierarchyChangedNotice should contain lists of added/removed/modified
    // prims that are collapsed (i.e. no descendants)
    ASSERT_EQ(n.GetAdded().size(), 1);
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath(
                "/scene/testvariant1/V/SphereGroup1/emptyPrim/something2"))),
        n.GetAdded().end());
    ASSERT_EQ(n.GetRemoved().size(), 2);
    ASSERT_NE(
        (std::find(
            n.GetRemoved().begin(),
            n.GetRemoved().end(),
            PXR_NS::SdfPath(
                "/scene/testvariant1/V/SphereGroup1/emptyPrim/something1"))),
        n.GetRemoved().end());
    ASSERT_NE(
        (std::find(
            n.GetRemoved().begin(),
            n.GetRemoved().end(),
            PXR_NS::SdfPath("/scene/testvariant1/V/SphereGroup2"))),
        n.GetRemoved().end());
    ASSERT_EQ(n.GetModified().size(), 1);
    ASSERT_NE(
        (std::find(
            n.GetModified().begin(),
            n.GetModified().end(),
            PXR_NS::SdfPath("/scene/testvariant1/V"))),
        n.GetModified().end());
}

TEST_F(HierarchyBroadcasterTest, Transaction)
{
    ::Test::Observer<unf::BroadcasterNotice::HierarchyChanged> observer(_stage);

    {
        unf::NoticeTransaction transaction(_broker);

        _stage->DefinePrim(PXR_NS::SdfPath("/scene/K/J"));
        _stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M"));
        _stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M/L"));
    }

    ASSERT_EQ(observer.Received(), 1);

    const auto& n = observer.GetLatestNotice();
    ASSERT_EQ(n.GetModified().size(), 0);
    ASSERT_EQ(n.GetAdded().size(), 1);
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/K"))),
        n.GetAdded().end());
    ASSERT_EQ(n.GetRemoved().size(), 0);

    {
        unf::NoticeTransaction transaction(_broker);

        _stage->DefinePrim(PXR_NS::SdfPath("/scene/M/J"));
        _stage->DefinePrim(PXR_NS::SdfPath("/scene/M/N"));
        _stage->RemovePrim(PXR_NS::SdfPath("/scene/M/N"));
    }
    ASSERT_EQ(n.GetModified().size(), 0);
    ASSERT_EQ(n.GetAdded().size(), 1);
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/M"))),
        n.GetAdded().end());
    ASSERT_EQ(n.GetRemoved().size(), 0);

    observer.Reset();

    {
        unf::NoticeTransaction transaction(_broker);

        _stage->DefinePrim(PXR_NS::SdfPath("/scene/RemovePrim"));
        _stage->RemovePrim(PXR_NS::SdfPath("/scene/RemovePrim"));
    }
    // The transaction added, then deleted the prim -- therefore, there was no
    // change to the cache and no notice should be emitted
    ASSERT_EQ(observer.Received(), 0);

    observer.Reset();

    {
        unf::NoticeTransaction transaction(_broker);

        _stage->RemovePrim(PXR_NS::SdfPath("/scene/K/M"));
        _stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M/L"));
    }
    ASSERT_EQ(n.GetModified().size(), 1);
    ASSERT_NE(
        (std::find(
            n.GetModified().begin(),
            n.GetModified().end(),
            PXR_NS::SdfPath("/scene/K/M"))),
        n.GetModified().end());
    ASSERT_EQ(n.GetAdded().size(), 0);
    ASSERT_EQ(n.GetRemoved().size(), 0);

    observer.Reset();

    {
        unf::NoticeTransaction transaction(_broker);

        _stage->RemovePrim(PXR_NS::SdfPath("/scene/K/M"));
        _stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M/P"));
    }
    ASSERT_EQ(n.GetModified().size(), 1);
    ASSERT_NE(
        (std::find(
            n.GetModified().begin(),
            n.GetModified().end(),
            PXR_NS::SdfPath("/scene/K/M"))),
        n.GetModified().end());
    ASSERT_EQ(n.GetAdded().size(), 1);
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/K/M/P"))),
        n.GetAdded().end());
    ASSERT_EQ(n.GetRemoved().size(), 1);
    ASSERT_NE(
        (std::find(
            n.GetRemoved().begin(),
            n.GetRemoved().end(),
            PXR_NS::SdfPath("/scene/K/M/L"))),
        n.GetRemoved().end());

    {
        unf::NoticeTransaction transaction(_broker);

        PXR_NS::UsdPrim p2 =
            _stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/D/a"));
        p2.SetInstanceable(true);

        _stage->DefinePrim(PXR_NS::SdfPath("/scene/sublayerShared/K/k2"));
        _stage->RemovePrim(PXR_NS::SdfPath("/scene/sublayerShared/K/k2"));
    }

    ASSERT_EQ(n.GetModified().size(), 1);
    ASSERT_NE(
        (std::find(
            n.GetModified().begin(),
            n.GetModified().end(),
            PXR_NS::SdfPath("/scene/D/a"))),
        n.GetModified().end());
    ASSERT_EQ(n.GetAdded().size(), 1);
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/sublayerShared/K"))),
        n.GetAdded().end());
    ASSERT_EQ(n.GetRemoved().size(), 0);
    ASSERT_EQ(n.GetChangedFields().size(), 1);

    {
        unf::NoticeTransaction transaction(_broker);

        std::string layerIdentifier = GetTestFilePath("/sublayer2.usda");
        _stage->MuteLayer(layerIdentifier);
        _stage->UnmuteLayer(layerIdentifier);
    }

    ASSERT_EQ(n.GetModified().size(), 1);
    ASSERT_NE(
        (std::find(
            n.GetModified().begin(),
            n.GetModified().end(),
            PXR_NS::SdfPath("/"))),
        n.GetModified().end());
    ASSERT_EQ(n.GetAdded().size(), 0);
    ASSERT_EQ(n.GetRemoved().size(), 0);

    std::string layer1Identifier = GetTestFilePath("/sublayer.usda");
    std::string layer2Identifier = GetTestFilePath("/sublayer2.usda");

    _stage->MuteLayer(layer1Identifier);
    {
        unf::NoticeTransaction transaction(_broker);

        _stage->MuteLayer(layer2Identifier);
        _stage->UnmuteLayer(layer1Identifier);
    }

    ASSERT_EQ(n.GetModified().size(), 1);
    ASSERT_NE(
        (std::find(
            n.GetModified().begin(),
            n.GetModified().end(),
            PXR_NS::SdfPath("/"))),
        n.GetModified().end());
    ASSERT_EQ(n.GetAdded().size(), 3);
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/B/bb"))),
        n.GetAdded().end());
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/sublayer"))),
        n.GetAdded().end());
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild"))),
        n.GetAdded().end());
    ASSERT_EQ(n.GetRemoved().size(), 4);
    ASSERT_NE(
        (std::find(
            n.GetRemoved().begin(),
            n.GetRemoved().end(),
            PXR_NS::SdfPath("/scene/B/bb2"))),
        n.GetRemoved().end());
    ASSERT_NE(
        (std::find(
            n.GetRemoved().begin(),
            n.GetRemoved().end(),
            PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild2"))),
        n.GetRemoved().end());
    ASSERT_NE(
        (std::find(
            n.GetRemoved().begin(),
            n.GetRemoved().end(),
            PXR_NS::SdfPath("/scene/G"))),
        n.GetRemoved().end());
    ASSERT_NE(
        (std::find(
            n.GetRemoved().begin(),
            n.GetRemoved().end(),
            PXR_NS::SdfPath("/scene/sublayer2"))),
        n.GetRemoved().end());
}

TEST_F(HierarchyBroadcasterTest, TransactionNested)
{
    ::Test::Observer<unf::BroadcasterNotice::HierarchyChanged> observer(_stage);

    {
        unf::NoticeTransaction transaction(_broker);

        _stage->DefinePrim(PXR_NS::SdfPath("/scene/P/J"));
        {
            unf::NoticeTransaction transaction(_broker);
            _stage->DefinePrim(PXR_NS::SdfPath("/scene/P/M"));
            _stage->DefinePrim(PXR_NS::SdfPath("/scene/P/M/L"));
        }
    }

    ASSERT_EQ(observer.Received(), 1);

    const auto& n = observer.GetLatestNotice();
    ASSERT_EQ(n.GetModified().size(), 0);
    ASSERT_EQ(n.GetAdded().size(), 1);
    ASSERT_EQ(n.GetRemoved().size(), 0);

    observer.Reset();

    {
        unf::NoticeTransaction transaction(_broker);

        _stage->RemovePrim(PXR_NS::SdfPath("/scene/P"));
        {
            unf::NoticeTransaction transaction(_broker);
            _stage->DefinePrim(PXR_NS::SdfPath("/scene/P/K/L"));
        }
    }

    ASSERT_EQ(observer.Received(), 1);

    ASSERT_EQ(n.GetModified().size(), 1);
    ASSERT_NE(
        (std::find(
            n.GetModified().begin(),
            n.GetModified().end(),
            PXR_NS::SdfPath("/scene/P"))),
        n.GetModified().end());
    ASSERT_EQ(n.GetAdded().size(), 1);
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/P/K"))),
        n.GetAdded().end());
    ASSERT_EQ(n.GetRemoved().size(), 2);
    ASSERT_NE(
        (std::find(
            n.GetRemoved().begin(),
            n.GetRemoved().end(),
            PXR_NS::SdfPath("/scene/P/M"))),
        n.GetRemoved().end());
    ASSERT_NE(
        (std::find(
            n.GetRemoved().begin(),
            n.GetRemoved().end(),
            PXR_NS::SdfPath("/scene/P/J"))),
        n.GetRemoved().end());

    observer.Reset();

    {
        unf::NoticeTransaction transaction(_broker);

        _stage->DefinePrim(PXR_NS::SdfPath("/scene/K/J"));
        _stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M"));
        _stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M/L"));
        {
            unf::NoticeTransaction transaction(_broker);
            _stage->DefinePrim(PXR_NS::SdfPath("/scene/M/J"));
            _stage->DefinePrim(PXR_NS::SdfPath("/scene/M/N"));
            _stage->RemovePrim(PXR_NS::SdfPath("/scene/M/N"));
        }
    }

    ASSERT_EQ(observer.Received(), 1);

    ASSERT_EQ(n.GetModified().size(), 0);
    ASSERT_EQ(n.GetAdded().size(), 2);
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/K"))),
        n.GetAdded().end());
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/M"))),
        n.GetAdded().end());
    ASSERT_EQ(n.GetRemoved().size(), 0);

    observer.Reset();

    {
        unf::NoticeTransaction transaction(_broker);

        PXR_NS::UsdPrim p = _stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/D/a"));
        p.SetInstanceable(true);
        _stage->DefinePrim(PXR_NS::SdfPath("/scene/sublayerShared/K/k2"));
        _stage->RemovePrim(PXR_NS::SdfPath("/scene/sublayerShared/K/k2"));
        {
            unf::NoticeTransaction transaction(_broker);

            PXR_NS::UsdPrim p =
                _stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/test/a"));
            p.SetInstanceable(true);
        }
    }

    ASSERT_EQ(observer.Received(), 1);

    ASSERT_EQ(n.GetModified().size(), 2);
    ASSERT_NE(
        (std::find(
            n.GetModified().begin(),
            n.GetModified().end(),
            PXR_NS::SdfPath("/scene/D/a"))),
        n.GetModified().end());
    ASSERT_NE(
        (std::find(
            n.GetModified().begin(),
            n.GetModified().end(),
            PXR_NS::SdfPath("/scene/test/a"))),
        n.GetModified().end());
    ASSERT_EQ(n.GetAdded().size(), 1);
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/sublayerShared/K"))),
        n.GetAdded().end());
    ASSERT_EQ(n.GetRemoved().size(), 0);
    ASSERT_EQ(n.GetChangedFields().size(), 2);

    observer.Reset();

    {
        unf::NoticeTransaction transaction(_broker);

        _stage->RemovePrim(PXR_NS::SdfPath("/scene/K/M"));
        _stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M/L"));

        _stage->DefinePrim(PXR_NS::SdfPath("/scene/E/P"));

        {
            unf::NoticeTransaction transaction(_broker);

            _stage->DefinePrim(PXR_NS::SdfPath("/scene/E/N"));
            _stage->RemovePrim(PXR_NS::SdfPath("/scene/E"));
        }
    }

    ASSERT_EQ(observer.Received(), 1);

    ASSERT_EQ(n.GetModified().size(), 1);
    ASSERT_NE(
        (std::find(
            n.GetModified().begin(),
            n.GetModified().end(),
            PXR_NS::SdfPath("/scene/K/M"))),
        n.GetModified().end());
    ASSERT_EQ(n.GetAdded().size(), 0);
    ASSERT_EQ(n.GetRemoved().size(), 0);

    observer.Reset();

    {
        unf::NoticeTransaction transaction(_broker);

        _stage->RemovePrim(PXR_NS::SdfPath("/scene/M/J"));

        {
            unf::NoticeTransaction transaction(_broker);
            _stage->DefinePrim(PXR_NS::SdfPath("/scene/M/J/L"));
        }
    }

    ASSERT_EQ(observer.Received(), 1);

    ASSERT_EQ(n.GetModified().size(), 1);
    ASSERT_NE(
        (std::find(
            n.GetModified().begin(),
            n.GetModified().end(),
            PXR_NS::SdfPath("/scene/M/J"))),
        n.GetModified().end());
    ASSERT_EQ(n.GetAdded().size(), 1);
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/M/J/L"))),
        n.GetAdded().end());
    ASSERT_EQ(n.GetRemoved().size(), 0);

    observer.Reset();

    {
        unf::NoticeTransaction transaction(_broker);

        _stage->RemovePrim(PXR_NS::SdfPath("/scene/M/J"));

        {
            unf::NoticeTransaction transaction(_broker);
            _stage->DefinePrim(PXR_NS::SdfPath("/scene/M/J"));
        }
    }

    ASSERT_EQ(observer.Received(), 1);

    ASSERT_EQ(n.GetModified().size(), 1);
    ASSERT_NE(
        (std::find(
            n.GetModified().begin(),
            n.GetModified().end(),
            PXR_NS::SdfPath("/scene/M/J"))),
        n.GetModified().end());
    ASSERT_EQ(n.GetAdded().size(), 0);
    ASSERT_EQ(n.GetRemoved().size(), 1);
    ASSERT_NE(
        (std::find(
            n.GetRemoved().begin(),
            n.GetRemoved().end(),
            PXR_NS::SdfPath("/scene/M/J/L"))),
        n.GetRemoved().end());

    observer.Reset();

    PXR_NS::UsdPrim p =
        _stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/sublayerShared"));
    p.SetActive(false);
    {
        unf::NoticeTransaction transaction(_broker);

        p.SetActive(true);
        PXR_NS::UsdPrim p2 = _stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/D"));
        p2.SetActive(false);
    }
    ASSERT_EQ(n.GetModified().size(), 2);
    ASSERT_NE(
        (std::find(
            n.GetModified().begin(),
            n.GetModified().end(),
            PXR_NS::SdfPath("/scene/D"))),
        n.GetModified().end());
    ASSERT_NE(
        (std::find(
            n.GetModified().begin(),
            n.GetModified().end(),
            PXR_NS::SdfPath("/scene/sublayerShared"))),
        n.GetModified().end());
    ASSERT_EQ(n.GetRemoved().size(), 4);
    ASSERT_NE(
        (std::find(
            n.GetRemoved().begin(),
            n.GetRemoved().end(),
            PXR_NS::SdfPath("/scene/D/r2"))),
        n.GetRemoved().end());
    ASSERT_NE(
        (std::find(
            n.GetRemoved().begin(),
            n.GetRemoved().end(),
            PXR_NS::SdfPath("/scene/D/b"))),
        n.GetRemoved().end());
    ASSERT_NE(
        (std::find(
            n.GetRemoved().begin(),
            n.GetRemoved().end(),
            PXR_NS::SdfPath("/scene/D/r"))),
        n.GetRemoved().end());
    ASSERT_NE(
        (std::find(
            n.GetRemoved().begin(),
            n.GetRemoved().end(),
            PXR_NS::SdfPath("/scene/D/a"))),
        n.GetRemoved().end());
    ASSERT_EQ(n.GetAdded().size(), 3);
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild"))),
        n.GetAdded().end());
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/sublayerShared/K"))),
        n.GetAdded().end());
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/sublayerShared/K"))),
        n.GetAdded().end());

    observer.Reset();

    {
        unf::NoticeTransaction transaction(_broker);
        PXR_NS::UsdPrim p = _stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/A/a"));
        p.SetInstanceable(true);

        {
            unf::NoticeTransaction transaction(_broker);
            PXR_NS::UsdPrim p2 =
                _stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/A"));
            p2.SetInstanceable(true);
        }
    }

    ASSERT_EQ(observer.Received(), 1);

    ASSERT_EQ(n.GetModified().size(), 1);
    ASSERT_NE(
        (std::find(
            n.GetModified().begin(),
            n.GetModified().end(),
            PXR_NS::SdfPath("/scene/A"))),
        n.GetModified().end());
    ASSERT_EQ(n.GetAdded().size(), 0);
    ASSERT_EQ(n.GetRemoved().size(), 0);
}

TEST_F(HierarchyBroadcasterTest, ChildInitialize)
{
    _broker->AddBroadcaster<::Test::ChildBroadcaster>();

    // Ensure that the hierarchy broadcaster type is correct.
    auto b1 = _broker->GetBroadcaster("HierarchyBroadcaster");
    ASSERT_TRUE(PXR_NS::TfDynamic_cast<HierarchyBroadcasterPtr>(b1));

    // Ensure that the child broadcaster type is correct.
    auto b2 = _broker->GetBroadcaster("ChildBroadcaster");
    ASSERT_TRUE(PXR_NS::TfDynamic_cast<ChildBroadcasterPtr>(b2));
}

TEST_F(HierarchyBroadcasterTest, ChildNoticeSimple)
{
    _broker->AddBroadcaster<::Test::ChildBroadcaster>();

    ::Test::Observer<::Test::ChildBroadcasterNotice> observer(_stage);

    _stage->DefinePrim(PXR_NS::SdfPath("/scene/AA"));

    ASSERT_EQ(observer.Received(), 1);

    const auto& n = observer.GetLatestNotice();
    ASSERT_EQ(n.GetAdded().size(), 1);
    ASSERT_NE(
        std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/AA")),
        n.GetAdded().end());
    ASSERT_EQ(n.GetModified().size(), 0);
    ASSERT_EQ(n.GetRemoved().size(), 0);

    PXR_NS::UsdPrim p =
        _stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/testvariant1/V"));
    p.GetVariantSet("myVariant").SetVariantSelection("v");

    // The ChildBroadcasterNotice should have a list of non-collapsed notices
    // (i.e. full list of changed prims w/ descendants)
    ASSERT_EQ(n.GetAdded().size(), 1);
    ASSERT_NE(
        std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath(
                "/scene/testvariant1/V/SphereGroup1/emptyPrim/something2")),
        n.GetAdded().end());
    ASSERT_EQ(n.GetRemoved().size(), 2);
    ASSERT_NE(
        std::find(
            n.GetRemoved().begin(),
            n.GetRemoved().end(),
            PXR_NS::SdfPath("/scene/testvariant1/V/SphereGroup2")),
        n.GetRemoved().end());
    ASSERT_NE(
        std::find(
            n.GetRemoved().begin(),
            n.GetRemoved().end(),
            PXR_NS::SdfPath(
                "/scene/testvariant1/V/SphereGroup1/emptyPrim/something1")),
        n.GetRemoved().end());
    ASSERT_EQ(n.GetModified().size(), 4);
    ASSERT_NE(
        std::find(
            n.GetModified().begin(),
            n.GetModified().end(),
            PXR_NS::SdfPath("/scene/testvariant1/V")),
        n.GetModified().end());
    ASSERT_NE(
        std::find(
            n.GetModified().begin(),
            n.GetModified().end(),
            PXR_NS::SdfPath("/scene/testvariant1/V/SphereGroup1")),
        n.GetModified().end());
    ASSERT_NE(
        std::find(
            n.GetModified().begin(),
            n.GetModified().end(),
            PXR_NS::SdfPath("/scene/testvariant1/V/SphereGroup1/sphere")),
        n.GetModified().end());
    ASSERT_NE(
        std::find(
            n.GetModified().begin(),
            n.GetModified().end(),
            PXR_NS::SdfPath("/scene/testvariant1/V/SphereGroup1/emptyPrim")),
        n.GetModified().end());
}

TEST_F(HierarchyBroadcasterTest, ChildTransaction)
{
    _broker->AddBroadcaster<::Test::ChildBroadcaster>();

    ::Test::Observer<::Test::ChildBroadcasterNotice> observer(_stage);

    {
        unf::NoticeTransaction transaction(_broker);

        _stage->DefinePrim(PXR_NS::SdfPath("/scene/K/J"));
        _stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M"));
        _stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M/L"));
    }

    ASSERT_EQ(observer.Received(), 1);

    const auto& n = observer.GetLatestNotice();
    ASSERT_EQ(n.GetModified().size(), 0);
    ASSERT_EQ(n.GetAdded().size(), 4);
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/K"))),
        n.GetAdded().end());
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/K/J"))),
        n.GetAdded().end());
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/K/M"))),
        n.GetAdded().end());
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/K/M/L"))),
        n.GetAdded().end());
    ASSERT_EQ(n.GetRemoved().size(), 0);

    observer.Reset();

    {
        unf::NoticeTransaction transaction(_broker);

        _stage->DefinePrim(PXR_NS::SdfPath("/scene/RemovePrim"));
        _stage->RemovePrim(PXR_NS::SdfPath("/scene/RemovePrim"));
    }
    // The transaction added, then deleted the prim -- therefore, there was no
    // change to the cache and the child broadcasters should not be executed.
    ASSERT_EQ(observer.Received(), 0);

    observer.Reset();

    {
        unf::NoticeTransaction transaction(_broker);

        _stage->RemovePrim(PXR_NS::SdfPath("/scene/K/M"));
        _stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M/L"));
    }
    ASSERT_EQ(n.GetModified().size(), 2);
    ASSERT_NE(
        (std::find(
            n.GetModified().begin(),
            n.GetModified().end(),
            PXR_NS::SdfPath("/scene/K/M"))),
        n.GetModified().end());
    ASSERT_NE(
        (std::find(
            n.GetModified().begin(),
            n.GetModified().end(),
            PXR_NS::SdfPath("/scene/K/M/L"))),
        n.GetModified().end());
    ASSERT_EQ(n.GetAdded().size(), 0);
    ASSERT_EQ(n.GetRemoved().size(), 0);

    observer.Reset();

    {
        unf::NoticeTransaction transaction(_broker);

        PXR_NS::UsdPrim p2 =
            _stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/sublayerShared"));
        p2.SetInstanceable(true);
        _stage->DefinePrim(PXR_NS::SdfPath("/scene/sublayerShared/K/k2"));
        _stage->RemovePrim(PXR_NS::SdfPath("/scene/sublayerShared/K/k2"));
    }

    ASSERT_EQ(n.GetModified().size(), 3);
    ASSERT_NE(
        (std::find(
            n.GetModified().begin(),
            n.GetModified().end(),
            PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild"))),
        n.GetModified().end());
    ASSERT_NE(
        (std::find(
            n.GetModified().begin(),
            n.GetModified().end(),
            PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild2"))),
        n.GetModified().end());
    ASSERT_NE(
        (std::find(
            n.GetModified().begin(),
            n.GetModified().end(),
            PXR_NS::SdfPath("/scene/sublayerShared"))),
        n.GetModified().end());
    ASSERT_EQ(n.GetAdded().size(), 1);
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/sublayerShared/K"))),
        n.GetAdded().end());
    ASSERT_EQ(n.GetRemoved().size(), 0);
    ASSERT_EQ(n.GetChangedFields().size(), 1);

    observer.Reset();

    {
        unf::NoticeTransaction transaction(_broker);

        std::string layerIdentifier = GetTestFilePath("/sublayer2.usda");
        _stage->MuteLayer(layerIdentifier);
        _stage->UnmuteLayer(layerIdentifier);
    }

    ASSERT_EQ(n.GetModified().size(), 37);
    ASSERT_EQ(n.GetAdded().size(), 0);
    ASSERT_EQ(n.GetRemoved().size(), 0);

    observer.Reset();

    std::string layer1Identifier = GetTestFilePath("/sublayer.usda");
    std::string layer2Identifier = GetTestFilePath("/sublayer2.usda");

    _stage->MuteLayer(layer1Identifier);
    {
        unf::NoticeTransaction transaction(_broker);

        _stage->MuteLayer(layer2Identifier);
        _stage->UnmuteLayer(layer1Identifier);
    }

    ASSERT_EQ(n.GetModified().size(), 29);
    ASSERT_EQ(n.GetAdded().size(), 3);
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild"))),
        n.GetAdded().end());
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/sublayer"))),
        n.GetAdded().end());
    ASSERT_NE(
        (std::find(
            n.GetAdded().begin(),
            n.GetAdded().end(),
            PXR_NS::SdfPath("/scene/B/bb"))),
        n.GetAdded().end());
    ASSERT_EQ(n.GetRemoved().size(), 5);
    ASSERT_NE(
        (std::find(
            n.GetRemoved().begin(),
            n.GetRemoved().end(),
            PXR_NS::SdfPath("/scene/G"))),
        n.GetRemoved().end());
    ASSERT_NE(
        (std::find(
            n.GetRemoved().begin(),
            n.GetRemoved().end(),
            PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild2"))),
        n.GetRemoved().end());
    ASSERT_NE(
        (std::find(
            n.GetRemoved().begin(),
            n.GetRemoved().end(),
            PXR_NS::SdfPath("/scene/sublayer2"))),
        n.GetRemoved().end());
    ASSERT_NE(
        (std::find(
            n.GetRemoved().begin(),
            n.GetRemoved().end(),
            PXR_NS::SdfPath("/scene/B/bb2"))),
        n.GetRemoved().end());
    ASSERT_NE(
        (std::find(
            n.GetRemoved().begin(),
            n.GetRemoved().end(),
            PXR_NS::SdfPath("/scene/sublayer2/sublayer2Child"))),
        n.GetRemoved().end());
}

TEST_F(HierarchyBroadcasterTest, ChildTransactionNested)
{
    _broker->AddBroadcaster<::Test::ChildBroadcaster>();

    ::Test::Observer<::Test::ChildBroadcasterNotice> observer(_stage);

    {
        unf::NoticeTransaction transaction(_broker);

        _stage->DefinePrim(PXR_NS::SdfPath("/scene/K/J"));
        {
            unf::NoticeTransaction transaction(_broker);
            _stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M"));
            _stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M/L"));
        }
    }

    // Child Broadcaster should have been executed once.
    ASSERT_EQ(observer.Received(), 1);
}
