#include <unf/broker.h>
#include <unf/transaction.h>
#include <unf/blocker.h>

#include <unfTest/listener.h>
#include <unfTest/notice.h>

#include <gtest/gtest.h>
#include <pxr/usd/usd/stage.h>

class BlockerTest : public ::testing::Test {
  protected:
    using Listener =
        ::Test::Listener<::Test::MergeableNotice, ::Test::UnMergeableNotice>;

    void SetUp() override
    {
        _stage = PXR_NS::UsdStage::CreateInMemory();
        _listener.SetStage(_stage);
    }

    PXR_NS::UsdStageRefPtr _stage;
    Listener _listener;
};

TEST_F(BlockerTest, Simple)
{
    auto broker = unf::Broker::Create(_stage);

    ASSERT_FALSE(broker->IsInTransaction());

    {
        unf::NoticeBlocker blocker(broker);
        ASSERT_EQ(blocker.GetBroker(), broker);

        ASSERT_TRUE(broker->IsInTransaction());

        broker->Send<::Test::MergeableNotice>();
        broker->Send<::Test::MergeableNotice>();
        broker->Send<::Test::MergeableNotice>();

        broker->Send<::Test::UnMergeableNotice>();
        broker->Send<::Test::UnMergeableNotice>();
        broker->Send<::Test::UnMergeableNotice>();

        // No notices are emitted in this scope.
        ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 0);
        ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 0);
    }

    // Ensure that no broker notices are sent after a transaction either.
    ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 0);
    ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 0);
}

TEST_F(BlockerTest, WithoutBroker)
{
    {
        unf::NoticeBlocker blocker(_stage);

        auto broker = blocker.GetBroker();
        ASSERT_EQ(broker, unf::Broker::Create(_stage));

        broker->Send<::Test::MergeableNotice>();
        broker->Send<::Test::MergeableNotice>();
        broker->Send<::Test::MergeableNotice>();

        broker->Send<::Test::UnMergeableNotice>();
        broker->Send<::Test::UnMergeableNotice>();
        broker->Send<::Test::UnMergeableNotice>();

        // No notices are emitted in this scope.
        ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 0);
        ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 0);
    }

    // Consolidated notices (if required) are sent when transaction is over.
    ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 0);
    ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 0);
}

TEST_F(BlockerTest, NestedWithTransaction)
{
    auto broker = unf::Broker::Create(_stage);

    ASSERT_FALSE(broker->IsInTransaction());

    {
        unf::NoticeTransaction transaction(broker);
        ASSERT_EQ(transaction.GetBroker(), broker);

        ASSERT_TRUE(broker->IsInTransaction());

        broker->Send<::Test::MergeableNotice>();
        broker->Send<::Test::MergeableNotice>();
        broker->Send<::Test::MergeableNotice>();

        broker->Send<::Test::UnMergeableNotice>();
        broker->Send<::Test::UnMergeableNotice>();
        broker->Send<::Test::UnMergeableNotice>();

        {
            unf::NoticeBlocker blocker(broker);
            ASSERT_EQ(blocker.GetBroker(), broker);

            ASSERT_TRUE(broker->IsInTransaction());

            broker->Send<::Test::MergeableNotice>();
            broker->Send<::Test::MergeableNotice>();
            broker->Send<::Test::MergeableNotice>();

            broker->Send<::Test::UnMergeableNotice>();
            broker->Send<::Test::UnMergeableNotice>();
            broker->Send<::Test::UnMergeableNotice>();

            // No notices are emitted in this scope.
            ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 0);
            ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 0);
        }

        ASSERT_TRUE(broker->IsInTransaction());

        // No notices are emitted during a transaction.
        ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 0);
        ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 0);
    }

    ASSERT_FALSE(broker->IsInTransaction());

    // Consolidated notices (if required) are sent for the transaction
    // scope only.
    ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 1);
    ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 3);
}
