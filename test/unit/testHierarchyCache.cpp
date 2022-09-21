#include <unf/hierarchyBroadcaster/cache.h>

#include <unf/broker.h>
#include <unf/notice.h>
#include <unf/transaction.h>

#include <unfTest/observer.h>

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

// namespace aliases for convenience.
using _USD = PXR_NS::UsdNotice;
namespace _Broker = unf::BrokerNotice;

class HierarchyCacheTest : public ::testing::Test {
  protected:
    using UsdObserver = ::Test::Observer<_USD::ObjectsChanged>;
    using BrokerObserver = ::Test::Observer<_Broker::ObjectsChanged>;

    void SetUp() override
    {
        _stage = PXR_NS::UsdStage::Open(GetTestFilePath("/scene.usda"));
    }

    std::string GetTestFilePath(const std::string& filePath)
    {
        std::string root = std::getenv("USD_TEST_PATH");
        return root + filePath;
    }

    template <class T>
    ::Test::Observer<T> CreateObserver(unf::HierarchyCache& cache)
    {
        ::Test::Observer<T> observer(_stage);

        observer.SetCallback([&](const T& notice) {
            PXR_NS::SdfPathVector resyncedChanges;
            for (const auto& path : notice.GetResyncedPaths()) {
                resyncedChanges.push_back(path);
            }
            cache.Update(resyncedChanges);
        });

        return observer;
    }

    PXR_NS::UsdStageRefPtr _stage;
};

TEST_F(HierarchyCacheTest, AddPrim)
{
    unf::HierarchyCache cache(_stage);
    auto observer = CreateObserver<_USD::ObjectsChanged>(cache);

    ASSERT_EQ(false, cache.FindNode(PXR_NS::SdfPath("/scene/AA")));

    _stage->DefinePrim(PXR_NS::SdfPath("/scene/AA"));

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

TEST_F(HierarchyCacheTest, ModifyPrim)
{
    unf::HierarchyCache cache(_stage);
    auto observer = CreateObserver<_USD::ObjectsChanged>(cache);

    PXR_NS::UsdPrim prim = _stage->GetPrimAtPath(PXR_NS::SdfPath("/scene"));
    prim.SetInstanceable(true);

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
}

TEST_F(HierarchyCacheTest, RemovePrimBase)
{
    unf::HierarchyCache cache(_stage);
    auto observer = CreateObserver<_USD::ObjectsChanged>(cache);

    _stage->RemovePrim(PXR_NS::SdfPath("/scene/AA"));
    ASSERT_EQ(cache.GetRemoved().size(), 0);
    ASSERT_EQ(cache.GetModified().size(), 0);
    ASSERT_EQ(cache.GetAdded().size(), 0);
    cache.Clear();

    _stage->RemovePrim(PXR_NS::SdfPath("/scene/A/a"));
    ASSERT_EQ(cache.GetRemoved().size(), 1);
    ASSERT_NE(
        cache.GetRemoved().find(PXR_NS::SdfPath("/scene/A/a")),
        cache.GetRemoved().end());
    ASSERT_EQ(cache.GetAdded().size(), 0);
    ASSERT_EQ(cache.GetModified().size(), 0);
    ASSERT_EQ(false, cache.FindNode(PXR_NS::SdfPath("/scene/A/a")));
    cache.Clear();

    _stage->RemovePrim(PXR_NS::SdfPath("/scene/A"));
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
}

TEST_F(HierarchyCacheTest, RemovePrimComplex)
{
    unf::HierarchyCache cache(_stage);
    auto observer = CreateObserver<_USD::ObjectsChanged>(cache);

    PXR_NS::UsdPrim prim1 =
        _stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/sublayer2"));
    prim1.SetActive(false);

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

    PXR_NS::UsdPrim prim2 =
        _stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/sublayerShared"));
    prim2.SetActive(false);

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

    PXR_NS::UsdPrim prim3 = _stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/D"));
    prim3.SetActive(false);

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
}

TEST_F(HierarchyCacheTest, AddPrimComplex)
{
    unf::HierarchyCache cache(_stage);
    auto observer = CreateObserver<_USD::ObjectsChanged>(cache);

    PXR_NS::UsdPrim prim1 =
        _stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/sublayer2"));
    prim1.SetActive(false);
    cache.Clear();
    prim1.SetActive(true);

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

    PXR_NS::UsdPrim prim2 =
        _stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/sublayerShared"));
    prim2.SetActive(false);
    cache.Clear();
    prim2.SetActive(true);

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

    PXR_NS::UsdPrim prim3 = _stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/D"));
    prim3.SetActive(false);
    cache.Clear();
    prim3.SetActive(true);

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
}

TEST_F(HierarchyCacheTest, VariantSwitch)
{
    unf::HierarchyCache cache(_stage);
    auto observer = CreateObserver<_USD::ObjectsChanged>(cache);

    PXR_NS::UsdPrim prim =
        _stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/testvariant1/V"));
    prim.GetVariantSet("myVariant").SetVariantSelection("v");

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
}

TEST_F(HierarchyCacheTest, MuteAndUnmuteLayers)
{
    unf::HierarchyCache cache(_stage);
    auto observer = CreateObserver<_USD::ObjectsChanged>(cache);

    std::string layerIdentifier = GetTestFilePath("/sublayer.usda");

    _stage->MuteLayer(layerIdentifier);

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

    _stage->UnmuteLayer(layerIdentifier);
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

TEST_F(HierarchyCacheTest, TransactionChanges)
{
    unf::HierarchyCache cache(_stage);
    auto observer = CreateObserver<_Broker::ObjectsChanged>(cache);

    {
        unf::NoticeTransaction transaction(_stage);

        _stage->DefinePrim(PXR_NS::SdfPath("/scene/K/J"));
        _stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M"));
        _stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M/L"));
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
        unf::NoticeTransaction transaction(_stage);

        _stage->DefinePrim(PXR_NS::SdfPath("/scene/M/J"));
        _stage->DefinePrim(PXR_NS::SdfPath("/scene/M/N"));
        _stage->RemovePrim(PXR_NS::SdfPath("/scene/M/N"));
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
        unf::NoticeTransaction transaction(_stage);

        _stage->DefinePrim(PXR_NS::SdfPath("/scene/RemovePrim"));
        _stage->RemovePrim(PXR_NS::SdfPath("/scene/RemovePrim"));
    }
    ASSERT_EQ(cache.GetModified().size(), 0);
    ASSERT_EQ(cache.GetAdded().size(), 0);
    ASSERT_EQ(cache.GetRemoved().size(), 0);

    cache.Clear();

    {
        unf::NoticeTransaction transaction(_stage);

        _stage->RemovePrim(PXR_NS::SdfPath("/scene/K/M"));
        _stage->DefinePrim(PXR_NS::SdfPath("/scene/K/M/L"));
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
        unf::NoticeTransaction transaction(_stage);

        PXR_NS::UsdPrim prim =
            _stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/sublayerShared"));
        prim.SetInstanceable(true);

        _stage->DefinePrim(PXR_NS::SdfPath("/scene/sublayerShared/K/k2"));
        _stage->RemovePrim(PXR_NS::SdfPath("/scene/sublayerShared/K/k2"));
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
        unf::NoticeTransaction transaction(_stage);

        std::string layerIdentifier = GetTestFilePath("/sublayer2.usda");
        _stage->MuteLayer(layerIdentifier);
        _stage->UnmuteLayer(layerIdentifier);
    }

    ASSERT_EQ(cache.GetModified().size(), 39);
    ASSERT_EQ(cache.GetAdded().size(), 0);
    ASSERT_EQ(cache.GetRemoved().size(), 0);

    std::string layer1Identifier = GetTestFilePath("/sublayer.usda");
    std::string layer2Identifier = GetTestFilePath("/sublayer2.usda");

    _stage->MuteLayer(layer1Identifier);

    cache.Clear();

    {
        unf::NoticeTransaction transaction(_stage);

        _stage->MuteLayer(layer2Identifier);
        _stage->UnmuteLayer(layer1Identifier);
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
        unf::NoticeTransaction transaction(_stage);

        PXR_NS::UsdPrim prim =
            _stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/sublayerShared"));
        prim.SetActive(false);
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
        unf::NoticeTransaction transaction(_stage);

        PXR_NS::UsdPrim p =
            _stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/sublayerShared"));
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
        unf::NoticeTransaction transaction(_stage);

        PXR_NS::UsdPrim p =
            _stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/sublayerShared"));
        p.SetActive(true);
        PXR_NS::UsdPrim p2 = _stage->GetPrimAtPath(PXR_NS::SdfPath("/scene/D"));
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
}
