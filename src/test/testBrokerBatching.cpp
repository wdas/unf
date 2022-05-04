#include "../broker.h"
#include "./listener.h"
#include "./notice.h"

#include "gtest/gtest.h"
#include "pxr/pxr.h"
#include "pxr/usd/sdf/path.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usd/stage.h"

TEST(NoticeBroker, addPrim)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    Listener listener(stage);

    broker->BeginTransaction();

    ASSERT_TRUE(broker->IsInTransaction());

    stage->DefinePrim(PXR_NS::SdfPath {"/Foo"});

    ASSERT_EQ(listener.StageNotices(), 0);
    ASSERT_EQ(listener.StageContentsChangedNotices(), 0);
    ASSERT_EQ(listener.ObjectsChangedNotices(), 0);
    ASSERT_EQ(listener.StageEditTargetChangedNotices(), 0);
    ASSERT_EQ(listener.LayerMutingChangedNotices(), 0);
    ASSERT_EQ(listener.CustomMergeableNotices(), 0);
    ASSERT_EQ(listener.CustomUnMergeableNotices(), 0);

    broker->EndTransaction();

    ASSERT_FALSE(broker->IsInTransaction());

    ASSERT_EQ(listener.StageNotices(), 2);
    ASSERT_EQ(listener.StageContentsChangedNotices(), 1);
    ASSERT_EQ(listener.ObjectsChangedNotices(), 1);
    ASSERT_EQ(listener.StageEditTargetChangedNotices(), 0);
    ASSERT_EQ(listener.LayerMutingChangedNotices(), 0);
    ASSERT_EQ(listener.CustomMergeableNotices(), 0);
    ASSERT_EQ(listener.CustomUnMergeableNotices(), 0);
}

TEST(NoticeBroker, muteLayer)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    stage->DefinePrim(PXR_NS::SdfPath {"/Foo"});

    Listener listener(stage);

    broker->BeginTransaction();

    ASSERT_TRUE(broker->IsInTransaction());

    stage->MuteLayer("/Foo");

    ASSERT_EQ(listener.StageNotices(), 0);
    ASSERT_EQ(listener.StageContentsChangedNotices(), 0);
    ASSERT_EQ(listener.ObjectsChangedNotices(), 0);
    ASSERT_EQ(listener.StageEditTargetChangedNotices(), 0);
    ASSERT_EQ(listener.LayerMutingChangedNotices(), 0);
    ASSERT_EQ(listener.CustomMergeableNotices(), 0);
    ASSERT_EQ(listener.CustomUnMergeableNotices(), 0);

    broker->EndTransaction();

    ASSERT_FALSE(broker->IsInTransaction());

    ASSERT_EQ(listener.StageNotices(), 1);
    ASSERT_EQ(listener.StageContentsChangedNotices(), 0);
    ASSERT_EQ(listener.ObjectsChangedNotices(), 0);
    ASSERT_EQ(listener.StageEditTargetChangedNotices(), 0);
    ASSERT_EQ(listener.LayerMutingChangedNotices(), 1);
    ASSERT_EQ(listener.CustomMergeableNotices(), 0);
    ASSERT_EQ(listener.CustomUnMergeableNotices(), 0);
}

TEST(NoticeBroker, changeEditTarget)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto rootLayer = stage->GetRootLayer();

    auto subLayer = PXR_NS::SdfLayer::CreateAnonymous(".usda");
    rootLayer->SetSubLayerPaths({subLayer->GetIdentifier()});

    auto broker = PXR_NS::NoticeBroker::Create(stage);

    Listener listener(stage);

    broker->BeginTransaction();

    ASSERT_TRUE(broker->IsInTransaction());

    stage->SetEditTarget(PXR_NS::UsdEditTarget(subLayer));

    ASSERT_EQ(listener.StageNotices(), 0);
    ASSERT_EQ(listener.StageContentsChangedNotices(), 0);
    ASSERT_EQ(listener.ObjectsChangedNotices(), 0);
    ASSERT_EQ(listener.StageEditTargetChangedNotices(), 0);
    ASSERT_EQ(listener.LayerMutingChangedNotices(), 0);
    ASSERT_EQ(listener.CustomMergeableNotices(), 0);
    ASSERT_EQ(listener.CustomUnMergeableNotices(), 0);

    broker->EndTransaction();

    ASSERT_FALSE(broker->IsInTransaction());

    ASSERT_EQ(listener.StageNotices(), 1);
    ASSERT_EQ(listener.StageContentsChangedNotices(), 0);
    ASSERT_EQ(listener.ObjectsChangedNotices(), 0);
    ASSERT_EQ(listener.StageEditTargetChangedNotices(), 1);
    ASSERT_EQ(listener.LayerMutingChangedNotices(), 0);
    ASSERT_EQ(listener.CustomMergeableNotices(), 0);
    ASSERT_EQ(listener.CustomUnMergeableNotices(), 0);
}

TEST(NoticeBroker, customNotices)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    Listener listener(stage);

    broker->BeginTransaction();

    ASSERT_TRUE(broker->IsInTransaction());

    broker->Send<MergeableNotice>();
    broker->Send<MergeableNotice>();
    broker->Send<MergeableNotice>();

    broker->Send<UnMergeableNotice>();
    broker->Send<UnMergeableNotice>();
    broker->Send<UnMergeableNotice>();

    ASSERT_EQ(listener.StageNotices(), 0);
    ASSERT_EQ(listener.StageContentsChangedNotices(), 0);
    ASSERT_EQ(listener.ObjectsChangedNotices(), 0);
    ASSERT_EQ(listener.StageEditTargetChangedNotices(), 0);
    ASSERT_EQ(listener.LayerMutingChangedNotices(), 0);
    ASSERT_EQ(listener.CustomMergeableNotices(), 0);
    ASSERT_EQ(listener.CustomUnMergeableNotices(), 0);

    broker->EndTransaction();

    ASSERT_FALSE(broker->IsInTransaction());

    ASSERT_EQ(listener.StageNotices(), 4);
    ASSERT_EQ(listener.StageContentsChangedNotices(), 0);
    ASSERT_EQ(listener.ObjectsChangedNotices(), 0);
    ASSERT_EQ(listener.StageEditTargetChangedNotices(), 0);
    ASSERT_EQ(listener.LayerMutingChangedNotices(), 0);
    ASSERT_EQ(listener.CustomMergeableNotices(), 1);
    ASSERT_EQ(listener.CustomUnMergeableNotices(), 3);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
