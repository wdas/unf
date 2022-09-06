#include "unf/hierarchycache.h"
#include "unf/notice.h"
#include "unf/broker.h"
#include "unf/transaction.h"

#include "unfTest/listener.h"

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

using unf::HierarchyCache;
namespace {
    std::string GetTestFilePath(const std::string& filePath) {
        std::string root = std::getenv("USD_TEST_PATH");
        return root + filePath;
    }

    UsdStageRefPtr GetStage(const std::string& filePath) {
        return PXR_NS::UsdStage::Open(GetTestFilePath(filePath));
    }
}

TEST(HierarchyCache, AddPrim)
{
    auto stage = GetStage("/scene.usda");

    HierarchyCache cache = HierarchyCache(stage);

    ::Test::ObjChangedListener l = ::Test::ObjChangedListener(&cache);
    
    ASSERT_EQ(false, cache.FindNode(SdfPath("/scene/AA")));
    
    stage->DefinePrim(SdfPath("/scene/AA"));

    ASSERT_EQ(cache.GetAdded().size(), 1);
    ASSERT_EQ(cache.GetModified().size(), 0);
    ASSERT_EQ(cache.GetRemoved().size(), 0);
    ASSERT_NE(cache.GetAdded().find(SdfPath("/scene/AA")), cache.GetAdded().end());
    ASSERT_EQ(true, cache.FindNode(SdfPath("/scene/AA")));

    cache.Clear();
    ASSERT_EQ(cache.GetAdded().size(), 0);
}

TEST(HierarchyCache, ModifyPrim)
{
    auto stage = GetStage("/scene.usda");
    HierarchyCache cache = HierarchyCache(stage);

    ::Test::ObjChangedListener l = ::Test::ObjChangedListener(&cache);

    UsdPrim p = stage->GetPrimAtPath(SdfPath("/scene"));
    p.SetInstanceable(true);

    
    ASSERT_EQ(cache.GetModified().size(), 32);
    cache.Clear();
    
    cache.Update(SdfPathVector{SdfPath("/scene/D")});

    ASSERT_EQ(cache.GetModified().size(), 6);
    ASSERT_EQ(cache.GetAdded().size(), 0);
    ASSERT_EQ(cache.GetRemoved().size(), 0);
    ASSERT_EQ(true, cache.FindNode(SdfPath("/scene")));
    ASSERT_EQ(true, cache.FindNode(SdfPath("/scene/A")));

    cache.Clear();
}

TEST(HierarchyCache, RemovePrimBase)
{
    auto stage = GetStage("/scene.usda");
    HierarchyCache cache = HierarchyCache(stage);

    ::Test::ObjChangedListener l = ::Test::ObjChangedListener(&cache);

    ASSERT_EQ(true, cache.FindNode(SdfPath("/scene/D")));
    ASSERT_EQ(true, cache.FindNode(SdfPath("/scene/D/b")));

    stage->RemovePrim(SdfPath("/scene/AA"));
    ASSERT_EQ(cache.GetRemoved().size(), 0);
    ASSERT_EQ(cache.GetModified().size(), 0);
    ASSERT_EQ(cache.GetAdded().size(), 0);

    stage->RemovePrim(SdfPath("/scene/A/a"));
    ASSERT_EQ(false, cache.FindNode(SdfPath("/scene/A/a")));
    ASSERT_EQ(cache.GetRemoved().size(), 1);
    ASSERT_EQ(cache.GetAdded().size(), 0);
    ASSERT_EQ(cache.GetModified().size(), 0);
    cache.Clear();
    
    
    stage->RemovePrim(SdfPath("/scene/A"));
    ASSERT_EQ(cache.GetRemoved().size(), 3);
    ASSERT_EQ(cache.GetAdded().size(), 0);
    ASSERT_EQ(cache.GetModified().size(), 0);
    ASSERT_EQ(false, cache.FindNode(SdfPath("/scene/A")));
    ASSERT_EQ(false, cache.FindNode(SdfPath("/scene/A/b")));
    ASSERT_EQ(false, cache.FindNode(SdfPath("/scene/A/bb")));
    
    cache.Clear();
}

TEST(HierarchyCache, RemovePrimComplex)
{
    
    auto stage = GetStage("/scene.usda");
    HierarchyCache cache = HierarchyCache(stage);

    ::Test::ObjChangedListener l = ::Test::ObjChangedListener(&cache);

    UsdPrim p = stage->GetPrimAtPath(SdfPath("/scene/sublayer2"));
    p.SetActive(false);
    ASSERT_EQ(cache.GetRemoved().size(), 1);
    ASSERT_EQ(cache.GetAdded().size(), 0);
    ASSERT_EQ(cache.GetModified().size(), 1);
    ASSERT_EQ(true, cache.FindNode(SdfPath("/scene/sublayer2")));
    ASSERT_EQ(false, cache.FindNode(SdfPath("/scene/sublayer2/sublayer2Child")));
    
    cache.Clear();
    
    UsdPrim p2 = stage->GetPrimAtPath(SdfPath("/scene/sublayerShared"));
    p2.SetActive(false);
    ASSERT_EQ(cache.GetModified().size(), 1);
    ASSERT_EQ(cache.GetRemoved().size(), 2);
    ASSERT_EQ(true, cache.FindNode(SdfPath("/scene/sublayerShared")));
    ASSERT_EQ(false, cache.FindNode(SdfPath("/scene/sublayerShared/sublayerChild")));
    ASSERT_EQ(false, cache.FindNode(SdfPath("/scene/sublayerShared/sublayerChild2")));

    cache.Clear();

    UsdPrim p3 = stage->GetPrimAtPath(SdfPath("/scene/D"));
    p3.SetActive(false);
    ASSERT_EQ(cache.GetModified().size(), 1);
    ASSERT_EQ(cache.GetRemoved().size(), 5);
    ASSERT_EQ(true, cache.FindNode(SdfPath("/scene/D")));
    ASSERT_EQ(false, cache.FindNode(SdfPath("/scene/D/r")));
    ASSERT_EQ(false, cache.FindNode(SdfPath("/scene/D/r2")));
    ASSERT_EQ(false, cache.FindNode(SdfPath("/scene/D/a")));
    ASSERT_EQ(false, cache.FindNode(SdfPath("/scene/D/b")));
    ASSERT_EQ(false, cache.FindNode(SdfPath("/scene/D/b/x")));

    cache.Clear();
}

TEST(HierarchyCache, AddPrimComplex)
{
    auto stage = GetStage("/scene.usda");
    HierarchyCache cache = HierarchyCache(stage);

    ::Test::ObjChangedListener l = ::Test::ObjChangedListener(&cache);

    UsdPrim p = stage->GetPrimAtPath(SdfPath("/scene/sublayer2"));
    p.SetActive(false);
    cache.Clear();
    p.SetActive(true);
    ASSERT_EQ(cache.GetRemoved().size(), 0);
    ASSERT_EQ(cache.GetAdded().size(), 1);
    ASSERT_EQ(cache.GetModified().size(), 1);
    ASSERT_EQ(true, cache.FindNode(SdfPath("/scene/sublayer2")));
    ASSERT_EQ(true, cache.FindNode(SdfPath("/scene/sublayer2/sublayer2Child")));
    
    cache.Clear();
    
    UsdPrim p2 = stage->GetPrimAtPath(SdfPath("/scene/sublayerShared"));
    p2.SetActive(false);
    cache.Clear();
    p2.SetActive(true);
    ASSERT_EQ(cache.GetModified().size(), 1);
    ASSERT_EQ(cache.GetAdded().size(), 2);
    ASSERT_EQ(true, cache.FindNode(SdfPath("/scene/sublayerShared")));
    ASSERT_EQ(true, cache.FindNode(SdfPath("/scene/sublayerShared/sublayerChild")));
    ASSERT_EQ(true, cache.FindNode(SdfPath("/scene/sublayerShared/sublayerChild2")));

    cache.Clear();

    UsdPrim p3 = stage->GetPrimAtPath(SdfPath("/scene/D"));
    p3.SetActive(false);
    cache.Clear();
    p3.SetActive(true);
    ASSERT_EQ(cache.GetModified().size(), 1);
    ASSERT_EQ(cache.GetAdded().size(), 5);
    ASSERT_EQ(true, cache.FindNode(SdfPath("/scene/D")));
    ASSERT_EQ(true, cache.FindNode(SdfPath("/scene/D/r")));
    ASSERT_EQ(true, cache.FindNode(SdfPath("/scene/D/r2")));
    ASSERT_EQ(true, cache.FindNode(SdfPath("/scene/D/a")));
    ASSERT_EQ(true, cache.FindNode(SdfPath("/scene/D/b")));
    ASSERT_EQ(true, cache.FindNode(SdfPath("/scene/D/b/x")));
    
    cache.Clear();
}

TEST(HierarchyCache, VariantSwitch) {
    auto stage = GetStage("/scene.usda");
    HierarchyCache cache = HierarchyCache(stage);

    ::Test::ObjChangedListener l = ::Test::ObjChangedListener(&cache);

    UsdPrim p = stage->GetPrimAtPath(SdfPath("/scene/testvariant1/V"));
    p.GetVariantSet("myVariant").SetVariantSelection("v");

    ASSERT_EQ(cache.GetAdded().size(), 1);
    ASSERT_EQ(cache.GetRemoved().size(), 2);
    ASSERT_EQ(cache.GetModified().size(), 4);
    ASSERT_EQ(false, cache.FindNode(SdfPath("/scene/testvariant1/V/SphereGroup2")));
    ASSERT_EQ(false, cache.FindNode(SdfPath("/scene/testvariant1/V/SphereGroup1/emptyPrim/something1")));
    ASSERT_EQ(true, cache.FindNode(SdfPath("/scene/testvariant1/V/SphereGroup1/emptyPrim/something2")));
    
    cache.Clear();
}

TEST(HierarchyCache, MuteAndUnmuteLayers) {
    auto stage = GetStage("/scene.usda");
    HierarchyCache cache = HierarchyCache(stage);

    ::Test::ObjChangedListener l = ::Test::ObjChangedListener(&cache);

    std::string layerIdentifier = GetTestFilePath("/sublayer.usda");

    stage->MuteLayer(layerIdentifier);
    
    ASSERT_EQ(cache.GetRemoved().size(), 3);
    ASSERT_EQ(cache.GetAdded().size(), 0);
    ASSERT_EQ(cache.GetModified().size(), 29);

    cache.Clear();

    stage->UnmuteLayer(layerIdentifier);
    ASSERT_EQ(cache.GetAdded().size(), 3);
    ASSERT_EQ(cache.GetRemoved().size(), 0);
    ASSERT_EQ(cache.GetModified().size(), 29);

}

TEST(HierarchyCache, TransactionChanges) {
    auto stage = GetStage("/scene.usda");
    HierarchyCache cache = HierarchyCache(stage);
    ::Test::unfObjChangedListener l = ::Test::unfObjChangedListener(&cache);

    auto broker = PXR_NS::unf::Broker::Create(stage);
    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

        stage->DefinePrim(SdfPath("/scene/K/J"));
        stage->DefinePrim(SdfPath("/scene/K/M"));
        stage->DefinePrim(SdfPath("/scene/K/M/L"));
    }
    ASSERT_EQ(cache.GetModified().size(), 0);
    ASSERT_EQ(cache.GetAdded().size(), 4);
    ASSERT_EQ(cache.GetRemoved().size(), 0);

    cache.Clear();

    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

        stage->DefinePrim(SdfPath("/scene/M/J"));
        stage->DefinePrim(SdfPath("/scene/M/N"));
        stage->RemovePrim(SdfPath("/scene/M/N"));
    }
    ASSERT_EQ(cache.GetModified().size(), 0);
    ASSERT_EQ(cache.GetAdded().size(), 2);
    ASSERT_EQ(cache.GetRemoved().size(), 0);

    cache.Clear();

    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

        stage->DefinePrim(SdfPath("/scene/RemovePrim"));
        stage->RemovePrim(SdfPath("/scene/RemovePrim"));
    }
    ASSERT_EQ(cache.GetModified().size(), 0);
    ASSERT_EQ(cache.GetAdded().size(), 0);
    ASSERT_EQ(cache.GetRemoved().size(), 0);

    cache.Clear();

    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

        stage->RemovePrim(SdfPath("/scene/K/M"));
        stage->DefinePrim(SdfPath("/scene/K/M/L"));
    }
    ASSERT_EQ(cache.GetModified().size(), 2);
    ASSERT_EQ(cache.GetAdded().size(), 0);
    ASSERT_EQ(cache.GetRemoved().size(), 0);

    cache.Clear();

    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

        UsdPrim p2 = stage->GetPrimAtPath(SdfPath("/scene/sublayerShared"));
        p2.SetInstanceable(true);
        stage->DefinePrim(SdfPath("/scene/sublayerShared/K/k2"));
        stage->RemovePrim(SdfPath("/scene/sublayerShared/K/k2"));
    }
    ASSERT_EQ(cache.GetModified().size(), 3);
    ASSERT_EQ(cache.GetAdded().size(), 1);
    ASSERT_EQ(cache.GetRemoved().size(), 0);

    cache.Clear();
    {
        PXR_NS::unf::NoticeTransaction transaction(broker);

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
        PXR_NS::unf::NoticeTransaction transaction(broker);

        stage->MuteLayer(layer2Identifier);
        stage->UnmuteLayer(layer1Identifier);
    }
    
    ASSERT_EQ(cache.GetModified().size(), 31);
    ASSERT_EQ(cache.GetAdded().size(), 3);
    ASSERT_EQ(cache.GetRemoved().size(), 5);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
