#include <UsdNoticeBroker/broker.h>
#include <UsdNoticeBroker/transaction.h>

#include <TestUsdNoticeBroker/listener.h>
#include <TestUsdNoticeBroker/testNotice.h>

#include <gtest/gtest.h>
#include <pxr/usd/usd/stage.h>

class TransactionTest : public ::testing::Test {
protected:
    using Listener = ::Test::Listener<
        ::Test::MergeableNotice,
        ::Test::UnMergeableNotice
    >;

    void SetUp() override {
        _stage = PXR_NS::UsdStage::CreateInMemory();
        _listener.SetStage(_stage);
    }

    PXR_NS::UsdStageRefPtr _stage;
    Listener _listener;
};

TEST_F(TransactionTest, Simple)
{
    auto broker = PXR_NS::UNB::Broker::Create(_stage);

    ASSERT_FALSE(broker->IsInTransaction());

    {
        PXR_NS::UNB::NoticeTransaction transaction(broker);
        ASSERT_EQ(transaction.GetBroker(), broker);

        ASSERT_TRUE(broker->IsInTransaction());

        broker->Send<::Test::MergeableNotice>();
        broker->Send<::Test::MergeableNotice>();
        broker->Send<::Test::MergeableNotice>();

        broker->Send<::Test::UnMergeableNotice>();
        broker->Send<::Test::UnMergeableNotice>();
        broker->Send<::Test::UnMergeableNotice>();

        // No notices are emitted during a transaction.
        ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 0);
        ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 0);
    }

    // Consolidated notices (if required) are sent when transaction is over.
    ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 1);
    ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 3);
}

TEST_F(TransactionTest, WithoutBroker)
{
    {
        PXR_NS::UNB::NoticeTransaction transaction(_stage);

        auto broker = transaction.GetBroker();
        ASSERT_EQ(broker, PXR_NS::UNB::Broker::Create(_stage));

        broker->Send<::Test::MergeableNotice>();
        broker->Send<::Test::MergeableNotice>();
        broker->Send<::Test::MergeableNotice>();

        broker->Send<::Test::UnMergeableNotice>();
        broker->Send<::Test::UnMergeableNotice>();
        broker->Send<::Test::UnMergeableNotice>();

        // No notices are emitted during a transaction.
        ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 0);
        ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 0);
    }

    // Consolidated notices (if required) are sent when transaction is over.
    ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 1);
    ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 3);
}

TEST_F(TransactionTest, Nested)
{
    auto broker = PXR_NS::UNB::Broker::Create(_stage);

    ASSERT_FALSE(broker->IsInTransaction());

    {
        PXR_NS::UNB::NoticeTransaction transaction1(broker);
        ASSERT_EQ(transaction1.GetBroker(), broker);

        ASSERT_TRUE(broker->IsInTransaction());

        broker->Send<::Test::MergeableNotice>();
        broker->Send<::Test::MergeableNotice>();
        broker->Send<::Test::MergeableNotice>();

        broker->Send<::Test::UnMergeableNotice>();
        broker->Send<::Test::UnMergeableNotice>();
        broker->Send<::Test::UnMergeableNotice>();

        {
            PXR_NS::UNB::NoticeTransaction transaction2(broker);
            ASSERT_EQ(transaction2.GetBroker(), broker);

            ASSERT_TRUE(broker->IsInTransaction());

            broker->Send<::Test::MergeableNotice>();
            broker->Send<::Test::MergeableNotice>();
            broker->Send<::Test::MergeableNotice>();

            broker->Send<::Test::UnMergeableNotice>();
            broker->Send<::Test::UnMergeableNotice>();
            broker->Send<::Test::UnMergeableNotice>();

            // No notices are emitted during a transaction.
            ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 0);
            ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 0);
        }

        ASSERT_TRUE(broker->IsInTransaction());

        // No notices are emitted during a transaction.
        ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 0);
        ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 0);
    }

    ASSERT_FALSE(broker->IsInTransaction());

    // Consolidated notices (if required) are sent when all
    // transactions are over.
    ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 1);
    ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 6);
}
