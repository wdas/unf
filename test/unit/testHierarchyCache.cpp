#include "unf/broker.h"
#include "unf/hierarchycache.h"
#include "unf/notice.h"
#include "unf/transaction.h"

#include "unfTest/listener.h"

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

using unf::HierarchyCache;
namespace {
std::string GetTestFilePath(const std::string& filePath)
{
    std::string root = std::getenv("USD_TEST_PATH");
    return root + filePath;
}

PXR_NS::UsdStageRefPtr GetStage(const std::string& filePath)
{
    return PXR_NS::UsdStage::Open(GetTestFilePath(filePath));
}
}  // namespace

TEST(HierarchyCache, AddPrim)
{
    auto stage = GetStage("/scene.usda");

    HierarchyCache cache = HierarchyCache(stage);

    ::Test::ObjChangedListener l = ::Test::ObjChangedListener(&cache);

    ASSERT_EQ(false, cache.FindNode(PXR_NS::SdfPath("/scene/AA")));

    stage->DefinePrim(PXR_NS::SdfPath("/scene/AA"));

    ASSERT_EQ(cache.GetAdded().size(), 1);
    ASSERT_NE(
        cache.GetAdded().find(PXR_NS::SdfPath("/scene/AA")),
        cache.GetAdded().end());
    ASSERT_EQ(cache.GetModified().size(), 0);
    ASSERT_EQ(cache.GetRemoved().size(), 0);
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene/AA")));

    cache.Clear();
    ASSERT_EQ(cache.GetAdded().size(), 0);
}

TEST(HierarchyCache, ModifyPrim)
{
    auto stage = GetStage("/scene.usda");
    HierarchyCache cache = HierarchyCache(stage);

    ::Test::ObjChangedListener l = ::Test::ObjChangedListener(&cache);

    PXR_NS::UsdPrim p = stage->GetPrimAtPath(PXR_NS::SdfPath("/scene"));
    p.SetInstanceable(true);


    ASSERT_EQ(cache.GetModified().size(), 32);
    cache.Clear();

    cache.Update(PXR_NS::SdfPathVector{PXR_NS::SdfPath("/scene/D")});

    ASSERT_EQ(cache.GetModified().size(), 6);
    ASSERT_NE(
        cache.GetModified().find(PXR_NS::SdfPath("/scene/D/r")),
        cache.GetModified().end());
    ASSERT_NE(
        cache.GetModified().find(PXR_NS::SdfPath("/scene/D/b/x")),
        cache.GetModified().end());
    ASSERT_NE(
        cache.GetModified().find(PXR_NS::SdfPath("/scene/D/r2")),
        cache.GetModified().end());
    ASSERT_NE(
        cache.GetModified().find(PXR_NS::SdfPath("/scene/D/b")),
        cache.GetModified().end());
    ASSERT_NE(
        cache.GetModified().find(PXR_NS::SdfPath("/scene/D/a")),
        cache.GetModified().end());
    ASSERT_NE(
        cache.GetModified().find(PXR_NS::SdfPath("/scene/D")),
        cache.GetModified().end());
    ASSERT_EQ(cache.GetAdded().size(), 0);
    ASSERT_EQ(cache.GetRemoved().size(), 0);
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene")));
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene/D")));
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene/D/r2")));

    cache.Clear();
}

TEST(HierarchyCache, RemovePrimBase)
{
    auto stage = GetStage("/scene.usda");
    HierarchyCache cache = HierarchyCache(stage);

    ::Test::ObjChangedListener l = ::Test::ObjChangedListener(&cache);

    stage->RemovePrim(PXR_NS::SdfPath("/scene/AA"));
    ASSERT_EQ(cache.GetRemoved().size(), 0);
    ASSERT_EQ(cache.GetModified().size(), 0);
    ASSERT_EQ(cache.GetAdded().size(), 0);
    cache.Clear();

    stage->RemovePrim(PXR_NS::SdfPath("/scene/A/a"));
    ASSERT_EQ(cache.GetRemoved().size(), 1);
    ASSERT_NE(
        cache.GetRemoved().find(PXR_NS::SdfPath("/scene/A/a")),
        cache.GetRemoved().end());
    ASSERT_EQ(cache.GetAdded().size(), 0);
    ASSERT_EQ(cache.GetModified().size(), 0);
    ASSERT_EQ(false, cache.FindNode(PXR_NS::SdfPath("/scene/A/a")));
    cache.Clear();

    stage->RemovePrim(PXR_NS::SdfPath("/scene/A"));
    ASSERT_EQ(cache.GetRemoved().size(), 3);
    ASSERT_NE(
        cache.GetRemoved().find(PXR_NS::SdfPath("/scene/A")),
        cache.GetRemoved().end());
    ASSERT_NE(
        cache.GetRemoved().find(PXR_NS::SdfPath("/scene/A/b")),
        cache.GetRemoved().end());
    ASSERT_NE(
        cache.GetRemoved().find(PXR_NS::SdfPath("/scene/A/b/bb")),
        cache.GetRemoved().end());
    ASSERT_EQ(cache.GetAdded().size(), 0);
    ASSERT_EQ(cache.GetModified().size(), 0);
    ASSERT_EQ(false, cache.FindNode(PXR_NS::SdfPath("/scene/A")));
    ASSERT_EQ(false, cache.FindNode(PXR_NS::SdfPath("/scene/A/b")));
    ASSERT_EQ(false, cache.FindNode(PXR_NS::SdfPath("/scene/A/b/bb")));
    cache.Clear();
}

TEST(HierarchyCache, RemovePrimComplex)
{
    auto stage = GetStage("/scene.usda");
    HierarchyCache cache = HierarchyCache(stage);

    ::Test::ObjChangedListener l = ::Test::ObjChangedListener(&cache);

    PXR_NS::UsdPrim p =
        stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/sublayer2"));
    p.SetActive(false);
    ASSERT_EQ(cache.GetRemoved().size(), 1);
    ASSERT_NE(
        cache.GetRemoved().find(
            PXR_NS::SdfPath("/scene/sublayer2/sublayer2Child")),
        cache.GetRemoved().end());
    ASSERT_EQ(cache.GetAdded().size(), 0);
    ASSERT_EQ(cache.GetModified().size(), 1);
    ASSERT_NE(
        cache.GetModified().find(PXR_NS::SdfPath("/scene/sublayer2")),
        cache.GetModified().end());
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene/sublayer2")));
    ASSERT_EQ(
        false,
        cache.FindNode(PXR_NS::SdfPath("/scene/sublayer2/sublayer2Child")));

    cache.Clear();

    PXR_NS::UsdPrim p2 =
        stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/sublayerShared"));
    p2.SetActive(false);
    ASSERT_EQ(cache.GetModified().size(), 1);
    ASSERT_NE(
        cache.GetModified().find(PXR_NS::SdfPath("/scene/sublayerShared")),
        cache.GetModified().end());
    ASSERT_EQ(cache.GetRemoved().size(), 2);
    ASSERT_NE(
        cache.GetRemoved().find(
            PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild")),
        cache.GetRemoved().end());
    ASSERT_NE(
        cache.GetRemoved().find(
            PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild2")),
        cache.GetRemoved().end());
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene/sublayerShared")));
    ASSERT_EQ(
        false,
        cache.FindNode(PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild")));
    ASSERT_EQ(
        false,
        cache.FindNode(
            PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild2")));

    cache.Clear();

    PXR_NS::UsdPrim p3 = stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/D"));
    p3.SetActive(false);
    ASSERT_EQ(cache.GetModified().size(), 1);
    ASSERT_NE(
        cache.GetModified().find(PXR_NS::SdfPath("/scene/D")),
        cache.GetModified().end());
    ASSERT_EQ(cache.GetRemoved().size(), 5);
    ASSERT_NE(
        cache.GetRemoved().find(PXR_NS::SdfPath("/scene/D/r")),
        cache.GetRemoved().end());
    ASSERT_NE(
        cache.GetRemoved().find(PXR_NS::SdfPath("/scene/D/r2")),
        cache.GetRemoved().end());
    ASSERT_NE(
        cache.GetRemoved().find(PXR_NS::SdfPath("/scene/D/a")),
        cache.GetRemoved().end());
    ASSERT_NE(
        cache.GetRemoved().find(PXR_NS::SdfPath("/scene/D/b")),
        cache.GetRemoved().end());
    ASSERT_NE(
        cache.GetRemoved().find(PXR_NS::SdfPath("/scene/D/b/x")),
        cache.GetRemoved().end());
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene/D")));
    ASSERT_EQ(false, cache.FindNode(PXR_NS::SdfPath("/scene/D/r")));
    ASSERT_EQ(false, cache.FindNode(PXR_NS::SdfPath("/scene/D/r2")));
    ASSERT_EQ(false, cache.FindNode(PXR_NS::SdfPath("/scene/D/a")));
    ASSERT_EQ(false, cache.FindNode(PXR_NS::SdfPath("/scene/D/b")));
    ASSERT_EQ(false, cache.FindNode(PXR_NS::SdfPath("/scene/D/b/x")));

    cache.Clear();
}

TEST(HierarchyCache, AddPrimComplex)
{
    auto stage = GetStage("/scene.usda");
    HierarchyCache cache = HierarchyCache(stage);

    ::Test::ObjChangedListener l = ::Test::ObjChangedListener(&cache);

    PXR_NS::UsdPrim p =
        stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/sublayer2"));
    p.SetActive(false);
    cache.Clear();
    p.SetActive(true);
    ASSERT_EQ(cache.GetRemoved().size(), 0);
    ASSERT_EQ(cache.GetAdded().size(), 1);
    ASSERT_NE(
        cache.GetAdded().find(
            PXR_NS::SdfPath("/scene/sublayer2/sublayer2Child")),
        cache.GetAdded().end());
    ASSERT_EQ(cache.GetModified().size(), 1);
    ASSERT_NE(
        cache.GetModified().find(PXR_NS::SdfPath("/scene/sublayer2")),
        cache.GetModified().end());
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene/sublayer2")));
    ASSERT_EQ(
        true,
        cache.FindNode(PXR_NS::SdfPath("/scene/sublayer2/sublayer2Child")));

    cache.Clear();

    PXR_NS::UsdPrim p2 =
        stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/sublayerShared"));
    p2.SetActive(false);
    cache.Clear();
    p2.SetActive(true);
    ASSERT_EQ(cache.GetModified().size(), 1);
    ASSERT_NE(
        cache.GetModified().find(PXR_NS::SdfPath("/scene/sublayerShared")),
        cache.GetModified().end());
    ASSERT_EQ(cache.GetAdded().size(), 2);
    ASSERT_NE(
        cache.GetAdded().find(
            PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild")),
        cache.GetAdded().end());
    ASSERT_NE(
        cache.GetAdded().find(
            PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild2")),
        cache.GetAdded().end());
    ASSERT_EQ(cache.GetRemoved().size(), 0);
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene/sublayerShared")));
    ASSERT_EQ(
        true,
        cache.FindNode(PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild")));
    ASSERT_EQ(
        true,
        cache.FindNode(
            PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild2")));

    cache.Clear();

    PXR_NS::UsdPrim p3 = stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/D"));
    p3.SetActive(false);
    cache.Clear();
    p3.SetActive(true);
    ASSERT_EQ(cache.GetModified().size(), 1);
    ASSERT_NE(
        cache.GetModified().find(PXR_NS::SdfPath("/scene/D")),
        cache.GetModified().end());
    ASSERT_EQ(cache.GetAdded().size(), 5);
    ASSERT_NE(
        cache.GetAdded().find(PXR_NS::SdfPath("/scene/D/r2")),
        cache.GetAdded().end());
    ASSERT_NE(
        cache.GetAdded().find(PXR_NS::SdfPath("/scene/D/b/x")),
        cache.GetAdded().end());
    ASSERT_EQ(cache.GetRemoved().size(), 0);
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene/D")));
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene/D/r")));
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene/D/r2")));
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene/D/a")));
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene/D/b")));
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene/D/b/x")));

    cache.Clear();
}

TEST(HierarchyCache, VariantSwitch)
{
    auto stage = GetStage("/scene.usda");
    HierarchyCache cache = HierarchyCache(stage);

    ::Test::ObjChangedListener l = ::Test::ObjChangedListener(&cache);

    PXR_NS::UsdPrim p =
        stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/testvariant1/V"));
    p.GetVariantSet("myVariant").SetVariantSelection("v");

    ASSERT_EQ(cache.GetAdded().size(), 1);
    ASSERT_NE(
        cache.GetAdded().find(PXR_NS::SdfPath(
            "/scene/testvariant1/V/SphereGroup1/emptyPrim/something2")),
        cache.GetAdded().end());
    ASSERT_EQ(cache.GetRemoved().size(), 2);
    ASSERT_NE(
        cache.GetRemoved().find(
            PXR_NS::SdfPath("/scene/testvariant1/V/SphereGroup2")),
        cache.GetRemoved().end());
    ASSERT_NE(
        cache.GetRemoved().find(PXR_NS::SdfPath(
            "/scene/testvariant1/V/SphereGroup1/emptyPrim/something1")),
        cache.GetRemoved().end());
    ASSERT_EQ(cache.GetModified().size(), 4);
    ASSERT_NE(
        cache.GetModified().find(
            PXR_NS::SdfPath("/scene/testvariant1/V/SphereGroup1/sphere")),
        cache.GetModified().end());
    ASSERT_NE(
        cache.GetModified().find(
            PXR_NS::SdfPath("/scene/testvariant1/V/SphereGroup1")),
        cache.GetModified().end());
    ASSERT_NE(
        cache.GetModified().find(PXR_NS::SdfPath("/scene/testvariant1/V")),
        cache.GetModified().end());
    ASSERT_NE(
        cache.GetModified().find(
            PXR_NS::SdfPath("/scene/testvariant1/V/SphereGroup1/emptyPrim")),
        cache.GetModified().end());
    ASSERT_EQ(
        false,
        cache.FindNode(PXR_NS::SdfPath("/scene/testvariant1/V/SphereGroup2")));
    ASSERT_EQ(
        false,
        cache.FindNode(PXR_NS::SdfPath(
            "/scene/testvariant1/V/SphereGroup1/emptyPrim/something1")));
    ASSERT_EQ(
        true,
        cache.FindNode(PXR_NS::SdfPath(
            "/scene/testvariant1/V/SphereGroup1/emptyPrim/something2")));

    cache.Clear();
}

TEST(HierarchyCache, MuteAndUnmuteLayers)
{
    auto stage = GetStage("/scene.usda");
    HierarchyCache cache = HierarchyCache(stage);

    ::Test::ObjChangedListener l = ::Test::ObjChangedListener(&cache);

    std::string layerIdentifier = GetTestFilePath("/sublayer.usda");

    stage->MuteLayer(layerIdentifier);

    ASSERT_EQ(cache.GetRemoved().size(), 3);
    ASSERT_NE(
        cache.GetRemoved().find(PXR_NS::SdfPath("/scene/sublayer")),
        cache.GetRemoved().end());
    ASSERT_NE(
        cache.GetRemoved().find(
            PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild")),
        cache.GetRemoved().end());
    ASSERT_NE(
        cache.GetRemoved().find(PXR_NS::SdfPath("/scene/B/bb")),
        cache.GetRemoved().end());
    ASSERT_EQ(cache.GetAdded().size(), 0);
    ASSERT_EQ(cache.GetModified().size(), 29);
    ASSERT_EQ(false, cache.FindNode(PXR_NS::SdfPath("/scene/sublayer")));
    ASSERT_EQ(
        false,
        cache.FindNode(PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild")));
    ASSERT_EQ(false, cache.FindNode(PXR_NS::SdfPath("/scene/B/bb")));

    cache.Clear();

    stage->UnmuteLayer(layerIdentifier);
    ASSERT_EQ(cache.GetAdded().size(), 3);
    ASSERT_NE(
        cache.GetAdded().find(PXR_NS::SdfPath("/scene/sublayer")),
        cache.GetAdded().end());
    ASSERT_NE(
        cache.GetAdded().find(
            PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild")),
        cache.GetAdded().end());
    ASSERT_NE(
        cache.GetAdded().find(PXR_NS::SdfPath("/scene/B/bb")),
        cache.GetAdded().end());
    ASSERT_EQ(cache.GetRemoved().size(), 0);
    ASSERT_EQ(cache.GetModified().size(), 29);
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene/sublayer")));
    ASSERT_EQ(
        true,
        cache.FindNode(PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild")));
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene/B/bb")));
}

TEST(HierarchyCache, TransactionChanges)
{
    auto stage = GetStage("/scene.usda");
    HierarchyCache cache = HierarchyCache(stage);
    ::Test::unfObjChangedListener l = ::Test::unfObjChangedListener(&cache);

    auto broker = unf::Broker::Create(stage);

    {
        unf::NoticeTransaction transaction(broker);

        stage->DefinePrim(PXR_NS::SdfPath("/scene/K/J"));
        stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M"));
        stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M/L"));
    }
    ASSERT_EQ(cache.GetModified().size(), 0);
    ASSERT_EQ(cache.GetAdded().size(), 4);
    ASSERT_NE(
        cache.GetAdded().find(PXR_NS::SdfPath("/scene/K")),
        cache.GetAdded().end());
    ASSERT_NE(
        cache.GetAdded().find(PXR_NS::SdfPath("/scene/K/J")),
        cache.GetAdded().end());
    ASSERT_NE(
        cache.GetAdded().find(PXR_NS::SdfPath("/scene/K/M")),
        cache.GetAdded().end());
    ASSERT_NE(
        cache.GetAdded().find(PXR_NS::SdfPath("/scene/K/M/L")),
        cache.GetAdded().end());
    ASSERT_EQ(cache.GetRemoved().size(), 0);
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene/K/M/L")));

    cache.Clear();

    {
        unf::NoticeTransaction transaction(broker);

        stage->DefinePrim(PXR_NS::SdfPath("/scene/M/J"));
        stage->DefinePrim(PXR_NS::SdfPath("/scene/M/N"));
        stage->RemovePrim(PXR_NS::SdfPath("/scene/M/N"));
    }
    ASSERT_EQ(cache.GetModified().size(), 0);
    ASSERT_EQ(cache.GetAdded().size(), 2);
    ASSERT_NE(
        cache.GetAdded().find(PXR_NS::SdfPath("/scene/M")),
        cache.GetAdded().end());
    ASSERT_NE(
        cache.GetAdded().find(PXR_NS::SdfPath("/scene/M/J")),
        cache.GetAdded().end());
    ASSERT_EQ(cache.GetRemoved().size(), 0);
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene/M/J")));

    cache.Clear();

    {
        unf::NoticeTransaction transaction(broker);

        stage->DefinePrim(PXR_NS::SdfPath("/scene/RemovePrim"));
        stage->RemovePrim(PXR_NS::SdfPath("/scene/RemovePrim"));
    }
    ASSERT_EQ(cache.GetModified().size(), 0);
    ASSERT_EQ(cache.GetAdded().size(), 0);
    ASSERT_EQ(cache.GetRemoved().size(), 0);

    cache.Clear();

    {
        unf::NoticeTransaction transaction(broker);

        stage->RemovePrim(PXR_NS::SdfPath("/scene/K/M"));
        stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M/L"));
    }
    ASSERT_EQ(cache.GetModified().size(), 2);
    ASSERT_NE(
        cache.GetModified().find(PXR_NS::SdfPath("/scene/K/M")),
        cache.GetModified().end());
    ASSERT_NE(
        cache.GetModified().find(PXR_NS::SdfPath("/scene/K/M/L")),
        cache.GetModified().end());
    ASSERT_EQ(cache.GetAdded().size(), 0);
    ASSERT_EQ(cache.GetRemoved().size(), 0);
    cache.Clear();

    {
        unf::NoticeTransaction transaction(broker);

        PXR_NS::UsdPrim p2 =
            stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/sublayerShared"));
        p2.SetInstanceable(true);
        stage->DefinePrim(PXR_NS::SdfPath("/scene/sublayerShared/K/k2"));
        stage->RemovePrim(PXR_NS::SdfPath("/scene/sublayerShared/K/k2"));
    }
    ASSERT_EQ(cache.GetModified().size(), 3);
    ASSERT_NE(
        cache.GetModified().find(PXR_NS::SdfPath("/scene/sublayerShared")),
        cache.GetModified().end());
    ASSERT_NE(
        cache.GetModified().find(
            PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild")),
        cache.GetModified().end());
    ASSERT_NE(
        cache.GetModified().find(
            PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild2")),
        cache.GetModified().end());
    ASSERT_EQ(cache.GetAdded().size(), 1);
    ASSERT_NE(
        cache.GetAdded().find(PXR_NS::SdfPath("/scene/sublayerShared/K")),
        cache.GetAdded().end());
    ASSERT_EQ(cache.GetRemoved().size(), 0);
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene/sublayerShared/K")));

    cache.Clear();
    {
        unf::NoticeTransaction transaction(broker);

        std::string layerIdentifier = GetTestFilePath("/sublayer2.usda");
        stage->MuteLayer(layerIdentifier);
        stage->UnmuteLayer(layerIdentifier);
    }

    ASSERT_EQ(cache.GetModified().size(), 39);
    ASSERT_EQ(cache.GetAdded().size(), 0);
    ASSERT_EQ(cache.GetRemoved().size(), 0);

    std::string layer1Identifier = GetTestFilePath("/sublayer.usda");
    std::string layer2Identifier = GetTestFilePath("/sublayer2.usda");

    stage->MuteLayer(layer1Identifier);
    cache.Clear();
    {
        unf::NoticeTransaction transaction(broker);

        stage->MuteLayer(layer2Identifier);
        stage->UnmuteLayer(layer1Identifier);
    }

    ASSERT_EQ(cache.GetModified().size(), 31);
    ASSERT_EQ(cache.GetAdded().size(), 3);
    ASSERT_NE(
        cache.GetAdded().find(
            PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild")),
        cache.GetAdded().end());
    ASSERT_NE(
        cache.GetAdded().find(PXR_NS::SdfPath("/scene/sublayer")),
        cache.GetAdded().end());
    ASSERT_NE(
        cache.GetAdded().find(PXR_NS::SdfPath("/scene/B/bb")),
        cache.GetAdded().end());
    ASSERT_EQ(cache.GetRemoved().size(), 5);
    ASSERT_NE(
        cache.GetRemoved().find(
            PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild2")),
        cache.GetRemoved().end());
    ASSERT_NE(
        cache.GetRemoved().find(PXR_NS::SdfPath("/scene/G")),
        cache.GetRemoved().end());
    ASSERT_NE(
        cache.GetRemoved().find(PXR_NS::SdfPath("/scene/sublayer2")),
        cache.GetRemoved().end());
    ASSERT_NE(
        cache.GetRemoved().find(PXR_NS::SdfPath("/scene/B/bb2")),
        cache.GetRemoved().end());
    ASSERT_NE(
        cache.GetRemoved().find(
            PXR_NS::SdfPath("/scene/sublayer2/sublayer2Child")),
        cache.GetRemoved().end());
    ASSERT_EQ(
        true,
        cache.FindNode(PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild")));
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene/B/bb")));
    ASSERT_EQ(false, cache.FindNode(PXR_NS::SdfPath("/scene/B/bb2")));
    ASSERT_EQ(
        false,
        cache.FindNode(PXR_NS::SdfPath("/scene/sublayer2/sublayer2Child")));
    cache.Clear();

    {
        unf::NoticeTransaction transaction(broker);

        PXR_NS::UsdPrim p =
            stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/sublayerShared"));
        p.SetActive(false);
    }
    ASSERT_EQ(cache.GetModified().size(), 1);
    ASSERT_NE(
        cache.GetModified().find(PXR_NS::SdfPath("/scene/sublayerShared")),
        cache.GetModified().end());
    ASSERT_EQ(cache.GetRemoved().size(), 2);
    ASSERT_NE(
        cache.GetRemoved().find(
            PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild")),
        cache.GetRemoved().end());
    ASSERT_NE(
        cache.GetRemoved().find(PXR_NS::SdfPath("/scene/sublayerShared/K")),
        cache.GetRemoved().end());
    ASSERT_EQ(cache.GetAdded().size(), 0);
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene/sublayerShared")));
    ASSERT_EQ(
        false,
        cache.FindNode(PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild")));
    ASSERT_EQ(
        false, cache.FindNode(PXR_NS::SdfPath("/scene/sublayerShared/K")));
    cache.Clear();

    {
        unf::NoticeTransaction transaction(broker);

        PXR_NS::UsdPrim p =
            stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/sublayerShared"));
        p.SetActive(true);
        p.SetActive(false);
    }
    ASSERT_EQ(cache.GetModified().size(), 1);
    ASSERT_NE(
        cache.GetModified().find(PXR_NS::SdfPath("/scene/sublayerShared")),
        cache.GetModified().end());
    ASSERT_EQ(cache.GetAdded().size(), 0);
    ASSERT_EQ(cache.GetRemoved().size(), 0);
    cache.Clear();

    {
        unf::NoticeTransaction transaction(broker);

        PXR_NS::UsdPrim p =
            stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/sublayerShared"));
        p.SetActive(true);
        PXR_NS::UsdPrim p2 = stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/D"));
        p2.SetActive(false);
    }
    ASSERT_EQ(cache.GetModified().size(), 2);
    ASSERT_NE(
        cache.GetModified().find(PXR_NS::SdfPath("/scene/D")),
        cache.GetModified().end());
    ASSERT_NE(
        cache.GetModified().find(PXR_NS::SdfPath("/scene/sublayerShared")),
        cache.GetModified().end());
    ASSERT_EQ(cache.GetRemoved().size(), 5);
    ASSERT_NE(
        cache.GetRemoved().find(PXR_NS::SdfPath("/scene/D/r2")),
        cache.GetAdded().end());
    ASSERT_NE(
        cache.GetRemoved().find(PXR_NS::SdfPath("/scene/D/b/x")),
        cache.GetAdded().end());
    ASSERT_EQ(cache.GetAdded().size(), 2);
    ASSERT_NE(
        cache.GetAdded().find(
            PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild")),
        cache.GetAdded().end());
    ASSERT_NE(
        cache.GetAdded().find(PXR_NS::SdfPath("/scene/sublayerShared/K")),
        cache.GetAdded().end());
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene/D")));
    ASSERT_EQ(false, cache.FindNode(PXR_NS::SdfPath("/scene/D/r")));
    ASSERT_EQ(false, cache.FindNode(PXR_NS::SdfPath("/scene/D/r2")));
    ASSERT_EQ(false, cache.FindNode(PXR_NS::SdfPath("/scene/D/a")));
    ASSERT_EQ(false, cache.FindNode(PXR_NS::SdfPath("/scene/D/b")));
    ASSERT_EQ(false, cache.FindNode(PXR_NS::SdfPath("/scene/D/b/x")));
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene/sublayerShared")));
    ASSERT_EQ(
        true,
        cache.FindNode(PXR_NS::SdfPath("/scene/sublayerShared/sublayerChild")));
    ASSERT_EQ(true, cache.FindNode(PXR_NS::SdfPath("/scene/sublayerShared/K")));
    cache.Clear();
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
