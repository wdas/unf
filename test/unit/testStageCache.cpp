#include "unf/stagecache.h"
#include "unf/notice.h"

#include "unfTest/listener.h"

#include <gtest/gtest.h>
#include <pxr/pxr.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/base/vt/dictionary.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>

#include <string>

using unf::Cache;

TEST(AddPrim, StageCache)
{
    auto stage = PXR_NS::UsdStage::Open("/disney/users/chong/projects/usd-notice-broker/test/testenv/scene.usda");
    //auto stage = PXR_NS::UsdStage::Open("../testenv/scene.usda");
    Cache cache = Cache(stage);

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

TEST(ModifyPrim, StageCache)
{
    auto stage = PXR_NS::UsdStage::Open("/disney/users/chong/projects/usd-notice-broker/test/testenv/scene.usda");
    //auto stage = PXR_NS::UsdStage::Open("../testenv/scene.usda");
    Cache cache = Cache(stage);

    ::Test::ObjChangedListener l = ::Test::ObjChangedListener(&cache);

    UsdPrim p = stage->GetPrimAtPath(SdfPath("/scene"));
    p.SetInstanceable(true);

    
    ASSERT_EQ(cache.GetModified().size(), 30);
    cache.Clear();
    
    cache.Update(SdfPathVector{SdfPath("/scene/D")});

    ASSERT_EQ(cache.GetModified().size(), 6);
    ASSERT_EQ(cache.GetAdded().size(), 0);
    ASSERT_EQ(cache.GetRemoved().size(), 0);
    ASSERT_EQ(true, cache.FindNode(SdfPath("/scene")));
    ASSERT_EQ(true, cache.FindNode(SdfPath("/scene/A")));
}


TEST(RemovePrimBase, StageCache)
{
    auto stage = PXR_NS::UsdStage::Open("/disney/users/chong/projects/usd-notice-broker/test/testenv/scene.usda");
    //auto stage = PXR_NS::UsdStage::Open("../testenv/scene.usda");
    Cache cache = Cache(stage);

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

TEST(RemovePrimComplex, StageCache)
{
    
    auto stage = PXR_NS::UsdStage::Open("/disney/users/chong/projects/usd-notice-broker/test/testenv/scene.usda");
    //auto stage = PXR_NS::UsdStage::Open("../testenv/scene.usda");
    Cache cache = Cache(stage);

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

TEST(AddPrimComplex, StageCache)
{
    auto stage = PXR_NS::UsdStage::Open("/disney/users/chong/projects/usd-notice-broker/test/testenv/scene.usda");
    //auto stage = PXR_NS::UsdStage::Open("../testenv/scene.usda");
    Cache cache = Cache(stage);

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
}


int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
