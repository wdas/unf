#include "../broker.h"
#include "../transaction.h"
#include "./listener.h"

#include "gtest/gtest.h"
#include "pxr/pxr.h"
#include "pxr/usd/sdf/path.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usd/stage.h"

TEST(NoticeTransaction, withBroker)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    ASSERT_FALSE(broker->IsInTransaction());

    Listener listener(stage);

    {
        PXR_NS::NoticeTransaction transaction(broker);

        ASSERT_TRUE(broker->IsInTransaction());

        stage->DefinePrim(PXR_NS::SdfPath {"/Foo"});
        stage->MuteLayer("/Foo");

        ASSERT_EQ(listener.StageNotices(), 0);
        ASSERT_EQ(listener.StageContentsChangedNotices(), 0);
        ASSERT_EQ(listener.ObjectsChangedNotices(), 0);
        ASSERT_EQ(listener.StageEditTargetChangedNotices(), 0);
        ASSERT_EQ(listener.LayerMutingChangedNotices(), 0);
        ASSERT_EQ(listener.CustomMergeableNotices(), 0);
        ASSERT_EQ(listener.CustomUnMergeableNotices(), 0);
    }

    ASSERT_EQ(listener.StageNotices(), 3);
    ASSERT_EQ(listener.StageContentsChangedNotices(), 1);
    ASSERT_EQ(listener.ObjectsChangedNotices(), 1);
    ASSERT_EQ(listener.StageEditTargetChangedNotices(), 0);
    ASSERT_EQ(listener.LayerMutingChangedNotices(), 1);
    ASSERT_EQ(listener.CustomMergeableNotices(), 0);
    ASSERT_EQ(listener.CustomUnMergeableNotices(), 0);
}

TEST(Transaction, withoutBroker)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();

    Listener listener(stage);

    {
        PXR_NS::NoticeTransaction transaction(stage);

        stage->DefinePrim(PXR_NS::SdfPath {"/Foo"});
        stage->MuteLayer("/Foo");

        ASSERT_EQ(listener.StageNotices(), 0);
        ASSERT_EQ(listener.StageContentsChangedNotices(), 0);
        ASSERT_EQ(listener.ObjectsChangedNotices(), 0);
        ASSERT_EQ(listener.StageEditTargetChangedNotices(), 0);
        ASSERT_EQ(listener.LayerMutingChangedNotices(), 0);
        ASSERT_EQ(listener.CustomMergeableNotices(), 0);
        ASSERT_EQ(listener.CustomUnMergeableNotices(), 0);
    }

    ASSERT_EQ(listener.StageNotices(), 3);
    ASSERT_EQ(listener.StageContentsChangedNotices(), 1);
    ASSERT_EQ(listener.ObjectsChangedNotices(), 1);
    ASSERT_EQ(listener.StageEditTargetChangedNotices(), 0);
    ASSERT_EQ(listener.LayerMutingChangedNotices(), 1);
    ASSERT_EQ(listener.CustomMergeableNotices(), 0);
    ASSERT_EQ(listener.CustomUnMergeableNotices(), 0);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
