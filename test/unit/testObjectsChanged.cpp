#include <unf/broker.h>
#include <unf/notice.h>

#include <unfTest/observer.h>

#include <gtest/gtest.h>
#include <pxr/usd/sdf/layer.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/sdf/valueTypeName.h>
#include <pxr/usd/usd/attribute.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usd/stage.h>

class ObjectsChangedTest : public ::testing::Test {
  protected:
    void SetUp() override
    {
        _stage = PXR_NS::UsdStage::CreateInMemory();
        _broker = unf::Broker::Create(_stage);
    }

    PXR_NS::UsdStageRefPtr _stage;
    unf::BrokerPtr _broker;
};

TEST_F(ObjectsChangedTest, ResyncedObject)
{
    auto prim1 = _stage->DefinePrim(PXR_NS::SdfPath{"/Foo"});

    ::Test::Observer<unf::UnfNotice::ObjectsChanged> observer(_stage);

    auto prim2 = _stage->DefinePrim(PXR_NS::SdfPath{"/Bar"});

    ASSERT_EQ(observer.Received(), 1);

    const auto& n = observer.GetLatestNotice();
    ASSERT_FALSE(n.ResyncedObject(prim1));
    ASSERT_TRUE(n.ResyncedObject(prim2));
    ASSERT_FALSE(n.AffectedObject(prim1));
    ASSERT_TRUE(n.AffectedObject(prim2));
}

TEST_F(ObjectsChangedTest, ChangedInfoOnly)
{
    auto prim1 = _stage->DefinePrim(PXR_NS::SdfPath{"/Foo"});
    auto prim2 = _stage->DefinePrim(PXR_NS::SdfPath{"/Bar"});

    ::Test::Observer<unf::UnfNotice::ObjectsChanged> observer(_stage);

    prim2.SetMetadata(PXR_NS::TfToken{"comment"}, "This is a test");

    ASSERT_EQ(observer.Received(), 1);

    const auto& n = observer.GetLatestNotice();
    ASSERT_FALSE(n.ChangedInfoOnly(prim1));
    ASSERT_TRUE(n.ChangedInfoOnly(prim2));
    ASSERT_FALSE(n.AffectedObject(prim1));
    ASSERT_TRUE(n.AffectedObject(prim2));
}

TEST_F(ObjectsChangedTest, GetResyncedPaths)
{
    ::Test::Observer<unf::UnfNotice::ObjectsChanged> observer(_stage);

    _stage->DefinePrim(PXR_NS::SdfPath{"/Foo"});

    ASSERT_EQ(observer.Received(), 1);

    const auto& n = observer.GetLatestNotice();
    ASSERT_EQ(
        n.GetResyncedPaths(), PXR_NS::SdfPathVector{PXR_NS::SdfPath{"/Foo"}});
}

TEST_F(ObjectsChangedTest, GetChangedInfoOnlyPaths)
{
    auto prim = _stage->DefinePrim(PXR_NS::SdfPath{"/Foo"});

    ::Test::Observer<unf::UnfNotice::ObjectsChanged> observer(_stage);

    prim.SetMetadata(PXR_NS::TfToken{"comment"}, "This is a test");

    ASSERT_EQ(observer.Received(), 1);

    const auto& n = observer.GetLatestNotice();
    ASSERT_EQ(
        n.GetChangedInfoOnlyPaths(),
        PXR_NS::SdfPathVector{PXR_NS::SdfPath{"/Foo"}});
}

TEST_F(ObjectsChangedTest, GetChangedFields)
{
    auto prim = _stage->DefinePrim(PXR_NS::SdfPath{"/Foo"});

    ::Test::Observer<unf::UnfNotice::ObjectsChanged> observer(_stage);

    prim.SetMetadata(PXR_NS::TfToken{"comment"}, "This is a test");

    ASSERT_EQ(observer.Received(), 1);

    const auto& n = observer.GetLatestNotice();
    ASSERT_EQ(
        n.GetChangedFields(prim), unf::TfTokenSet{PXR_NS::TfToken{"comment"}});
    ASSERT_EQ(
        n.GetChangedFields(PXR_NS::SdfPath{"/Foo"}),
        unf::TfTokenSet{PXR_NS::TfToken{"comment"}});
    ASSERT_EQ(
        n.GetChangedFields(PXR_NS::SdfPath{"/Incorrect"}), unf::TfTokenSet{});
}

TEST_F(ObjectsChangedTest, HasChangedFields)
{
    auto prim = _stage->DefinePrim(PXR_NS::SdfPath{"/Foo"});

    ::Test::Observer<unf::UnfNotice::ObjectsChanged> observer(_stage);

    prim.SetMetadata(PXR_NS::TfToken{"comment"}, "This is a test");

    ASSERT_EQ(observer.Received(), 1);

    const auto& n1 = observer.GetLatestNotice();
    ASSERT_TRUE(n1.HasChangedFields(prim));
    ASSERT_TRUE(n1.HasChangedFields(PXR_NS::SdfPath{"/Foo"}));
    ASSERT_FALSE(n1.HasChangedFields(PXR_NS::SdfPath{"/Incorrect"}));

    _stage->RemovePrim(PXR_NS::SdfPath{"/Foo"});

    // No change field expected when prim is removed.
    const auto& n2 = observer.GetLatestNotice();
    ASSERT_FALSE(n2.HasChangedFields(PXR_NS::SdfPath{"/Foo"}));
}

TEST_F(ObjectsChangedTest, Descendants)
{
    ::Test::Observer<unf::UnfNotice::ObjectsChanged> observer(_stage);

    _stage->DefinePrim(PXR_NS::SdfPath{"/Foo/Bar"});

    // Ensure that two Unf notices are received (for "/Foo" and "/Foo/Bar")
    ASSERT_EQ(observer.Received(), 2);

    const auto& n = observer.GetLatestNotice();
    ASSERT_EQ(n.GetResyncedPaths().at(0), PXR_NS::SdfPath{"/Foo/Bar"});
}

TEST_F(ObjectsChangedTest, MergingResyncSingle)
{
    ::Test::Observer<unf::UnfNotice::ObjectsChanged> observer(_stage);

    _broker->BeginTransaction();
    _stage->DefinePrim(PXR_NS::SdfPath{"/Foo"});
    _broker->EndTransaction();

    ASSERT_EQ(observer.Received(), 1);

    const auto& n = observer.GetLatestNotice();
    ASSERT_EQ(
        n.GetResyncedPaths(), PXR_NS::SdfPathVector{PXR_NS::SdfPath{"/Foo"}});
    ASSERT_EQ(
        n.GetChangedFields(PXR_NS::SdfPath{"/Foo"}),
        unf::TfTokenSet{PXR_NS::TfToken{"specifier"}});
}

TEST_F(ObjectsChangedTest, MergingResyncMultiple)
{
    ::Test::Observer<unf::UnfNotice::ObjectsChanged> observer(_stage);

    _broker->BeginTransaction();
    _stage->DefinePrim(PXR_NS::SdfPath{"/Foo"});
    _stage->DefinePrim(PXR_NS::SdfPath{"/Bar"});
    _stage->DefinePrim(PXR_NS::SdfPath{"/Bim"});
    _broker->EndTransaction();

    ASSERT_EQ(observer.Received(), 1);

    // Ensure that Unf notice includes sorted resynced prims from all events.
    const auto& n = observer.GetLatestNotice();
    const auto& paths = n.GetResyncedPaths();
    ASSERT_EQ(paths.size(), 3);
    ASSERT_EQ(paths.at(0), PXR_NS::SdfPath{"/Bar"});
    ASSERT_EQ(paths.at(1), PXR_NS::SdfPath{"/Bim"});
    ASSERT_EQ(paths.at(2), PXR_NS::SdfPath{"/Foo"});

    ASSERT_EQ(
        n.GetChangedFields(PXR_NS::SdfPath{"/Foo"}),
        unf::TfTokenSet{PXR_NS::TfToken{"specifier"}});
    ASSERT_EQ(
        n.GetChangedFields(PXR_NS::SdfPath{"/Bar"}),
        unf::TfTokenSet{PXR_NS::TfToken{"specifier"}});
    ASSERT_EQ(
        n.GetChangedFields(PXR_NS::SdfPath{"/Foo"}),
        unf::TfTokenSet{PXR_NS::TfToken{"specifier"}});
}

TEST_F(ObjectsChangedTest, MergingResyncDescendants)
{
    ::Test::Observer<unf::UnfNotice::ObjectsChanged> observer(_stage);

    _broker->BeginTransaction();
    _stage->DefinePrim(PXR_NS::SdfPath{"/Foo/Bar"});
    _broker->EndTransaction();

    ASSERT_EQ(observer.Received(), 1);

    // Ensure that Unf notice only includes top-level resynced prim.
    const auto& n = observer.GetLatestNotice();
    ASSERT_EQ(
        n.GetResyncedPaths(), PXR_NS::SdfPathVector{PXR_NS::SdfPath{"/Foo"}});

    ASSERT_EQ(
        n.GetChangedFields(PXR_NS::SdfPath{"/Foo"}),
        unf::TfTokenSet{PXR_NS::TfToken{"specifier"}});
    ASSERT_EQ(
        n.GetChangedFields(PXR_NS::SdfPath{"/Foo/Bar"}),
        unf::TfTokenSet{PXR_NS::TfToken{"specifier"}});
}

TEST_F(ObjectsChangedTest, MergingChangeInfoSingle)
{
    auto prim = _stage->DefinePrim(PXR_NS::SdfPath{"/Foo"});

    ::Test::Observer<unf::UnfNotice::ObjectsChanged> observer(_stage);

    _broker->BeginTransaction();
    prim.SetMetadata(PXR_NS::TfToken{"comment"}, "This is a test");
    _broker->EndTransaction();

    ASSERT_EQ(observer.Received(), 1);

    // Ensure that Unf notice only includes top-level resynced prim.
    const auto& n = observer.GetLatestNotice();
    ASSERT_EQ(
        n.GetChangedInfoOnlyPaths(),
        PXR_NS::SdfPathVector{PXR_NS::SdfPath{"/Foo"}});

    ASSERT_EQ(
        n.GetChangedFields(PXR_NS::SdfPath{"/Foo"}),
        unf::TfTokenSet{PXR_NS::TfToken{"comment"}});
}

TEST_F(ObjectsChangedTest, MergingChangeInfoMultiple)
{
    auto prim1 = _stage->DefinePrim(PXR_NS::SdfPath{"/Foo"});
    auto prim2 = _stage->DefinePrim(PXR_NS::SdfPath{"/Bar"});
    auto prim3 = _stage->DefinePrim(PXR_NS::SdfPath{"/Bim"});

    ::Test::Observer<unf::UnfNotice::ObjectsChanged> observer(_stage);

    _broker->BeginTransaction();
    prim1.SetMetadata(PXR_NS::TfToken{"comment"}, "This is a test");
    prim2.SetMetadata(PXR_NS::TfToken{"comment"}, "This is a test");
    prim3.SetMetadata(PXR_NS::TfToken{"comment"}, "This is a test");
    _broker->EndTransaction();

    ASSERT_EQ(observer.Received(), 1);

    // Ensure that Unf notice includes resynced prims from all events.
    const auto& n = observer.GetLatestNotice();
    const auto& paths = n.GetChangedInfoOnlyPaths();
    ASSERT_EQ(paths.size(), 3);
    ASSERT_EQ(paths.at(0), PXR_NS::SdfPath{"/Foo"});
    ASSERT_EQ(paths.at(1), PXR_NS::SdfPath{"/Bar"});
    ASSERT_EQ(paths.at(2), PXR_NS::SdfPath{"/Bim"});

    ASSERT_EQ(
        n.GetChangedFields(PXR_NS::SdfPath{"/Foo"}),
        unf::TfTokenSet{PXR_NS::TfToken{"comment"}});
    ASSERT_EQ(
        n.GetChangedFields(PXR_NS::SdfPath{"/Bar"}),
        unf::TfTokenSet{PXR_NS::TfToken{"comment"}});
    ASSERT_EQ(
        n.GetChangedFields(PXR_NS::SdfPath{"/Bim"}),
        unf::TfTokenSet{PXR_NS::TfToken{"comment"}});
}

TEST_F(ObjectsChangedTest, MergingResyncAndChangeInfo)
{
    ::Test::Observer<unf::UnfNotice::ObjectsChanged> observer(_stage);

    _broker->BeginTransaction();
    auto prim = _stage->DefinePrim(
        PXR_NS::SdfPath{"/Foo"}, PXR_NS::TfToken("Cylinder"));
    prim.GetAttribute(PXR_NS::TfToken("radius")).Set(5.0);
    prim.GetAttribute(PXR_NS::TfToken("height")).Set(10.0);
    _broker->EndTransaction();

    ASSERT_EQ(observer.Received(), 1);

    const auto& n = observer.GetLatestNotice();
    const auto& resyncedPaths = n.GetResyncedPaths();
    ASSERT_EQ(
        n.GetResyncedPaths(), PXR_NS::SdfPathVector{PXR_NS::SdfPath{"/Foo"}});

    // Prim defined as resynced are not recorded in changeinfo
    const auto& changedInfoPaths = n.GetChangedInfoOnlyPaths();
    ASSERT_EQ(changedInfoPaths.size(), 0);

    // tokens
    const auto& tokens = n.GetChangedFields(PXR_NS::SdfPath{"/Foo"});
    ASSERT_EQ(tokens.size(), 2);
    ASSERT_NE(tokens.find(PXR_NS::TfToken{"specifier"}), tokens.end());
    ASSERT_NE(tokens.find(PXR_NS::TfToken{"typeName"}), tokens.end());
}
