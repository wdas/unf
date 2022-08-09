#include "cache.h"
#include "broker.h"

#include <TestUsdNoticeBroker/testNotice.h>

#include <gtest/gtest.h>
#include <pxr/usd/usd/stage.h>

TEST(NoticeCacheTest, MergeableNotice)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    PXR_NS::NoticeCache<::Test::MergeableNotice> cache;

    broker->Send<::Test::MergeableNotice>(::Test::DataMap({{"Foo", "Test1"}}));
    broker->Send<::Test::MergeableNotice>(::Test::DataMap({{"Foo", "Test2"}}));
    broker->Send<::Test::MergeableNotice>(::Test::DataMap({{"Bar", "Test3"}}));

    // Ensure that three notices have been cached.
    ASSERT_EQ(cache.Size(), 3);

    // Ensure that notice data are all as expected.
    {
        auto& n1 = cache.GetAll().at(0);
        ASSERT_EQ(n1->GetData(), ::Test::DataMap({{"Foo", "Test1"}}));

        auto& n2 = cache.GetAll().at(1);
        ASSERT_EQ(n2->GetData(), ::Test::DataMap({{"Foo", "Test2"}}));

        auto& n3 = cache.GetAll().at(2);
        ASSERT_EQ(n3->GetData(), ::Test::DataMap({{"Bar", "Test3"}}));
    }

    cache.MergeAll();

    // Ensure that we have one merged notice after consolidation.
    ASSERT_EQ(cache.Size(), 1);

    // Ensure that the content of the notice is correct.
    {
        auto& n = cache.GetAll().at(0);
        ASSERT_EQ(n->GetData(),
            ::Test::DataMap({{"Foo", "Test2"}, {"Bar", "Test3"}}));
    }
}

TEST(NoticeCacheTest, UnMergeableNotice)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    PXR_NS::NoticeCache<::Test::UnMergeableNotice> cache;

    broker->Send<::Test::UnMergeableNotice>();
    broker->Send<::Test::UnMergeableNotice>();
    broker->Send<::Test::UnMergeableNotice>();

    // Ensure that three notices have been cached.
    ASSERT_EQ(cache.Size(), 3);

    cache.MergeAll();

    // Ensure that we still have three notice after consolidation.
    ASSERT_EQ(cache.Size(), 3);
}
