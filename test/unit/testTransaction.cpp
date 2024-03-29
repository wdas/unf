#include <unf/broker.h>
#include <unf/capturePredicate.h>
#include <unf/transaction.h>

#include <unfTest/listener.h>
#include <unfTest/notice.h>

#include <gtest/gtest.h>
#include <pxr/usd/usd/stage.h>

class TransactionTest : public ::testing::Test {
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

TEST_F(TransactionTest, FromBroker)
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

        // No notices are emitted during a transaction.
        ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 0);
        ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 0);
    }

    // Consolidated notices (if required) are sent when transaction is over.
    ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 1);
    ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 3);
}

TEST_F(TransactionTest, FromBrokerWithFilter)
{
    auto broker = unf::Broker::Create(_stage);

    ASSERT_FALSE(broker->IsInTransaction());

    {
        // Filter out UnMergeableNotice type.
        std::string target = typeid(::Test::UnMergeableNotice).name();
        auto predicate = [&](const unf::UnfNotice::StageNotice& n) {
            return (typeid(n).name() != target);
        };

        unf::NoticeTransaction transaction(broker, predicate);
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
    ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 0);
}

TEST_F(TransactionTest, FromBrokerWithDefaultPredicate)
{
    auto broker = unf::Broker::Create(_stage);

    ASSERT_FALSE(broker->IsInTransaction());

    {
        unf::NoticeTransaction transaction(
            broker, unf::CapturePredicate::Default());
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

TEST_F(TransactionTest, FromBrokerWithBlockAllPredicate)
{
    auto broker = unf::Broker::Create(_stage);

    ASSERT_FALSE(broker->IsInTransaction());

    {
        unf::NoticeTransaction transaction(
            broker, unf::CapturePredicate::BlockAll());
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

    // No notices are emitted after the transaction either.
    ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 0);
    ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 0);
}

TEST_F(TransactionTest, FromStage)
{
    {
        unf::NoticeTransaction transaction(_stage);

        auto broker = transaction.GetBroker();
        ASSERT_EQ(broker, unf::Broker::Create(_stage));

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

TEST_F(TransactionTest, FromStageWithFilter)
{
    {
        // Filter out UnMergeableNotice type.
        std::string target = typeid(::Test::UnMergeableNotice).name();
        auto predicate = [&](const unf::UnfNotice::StageNotice& n) {
            return (typeid(n).name() != target);
        };

        unf::NoticeTransaction transaction(_stage, predicate);

        auto broker = transaction.GetBroker();
        ASSERT_EQ(broker, unf::Broker::Create(_stage));

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
    ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 0);
}

TEST_F(TransactionTest, FromStageWithDefaultPredicate)
{
    {
        unf::NoticeTransaction transaction(
            _stage, unf::CapturePredicate::Default());

        auto broker = transaction.GetBroker();
        ASSERT_EQ(broker, unf::Broker::Create(_stage));

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

TEST_F(TransactionTest, FromStageWithBlockAllPredicate)
{
    {
        unf::NoticeTransaction transaction(
            _stage, unf::CapturePredicate::BlockAll());

        auto broker = transaction.GetBroker();
        ASSERT_EQ(broker, unf::Broker::Create(_stage));

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

    // No notices are emitted after the transaction either.
    ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 0);
    ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 0);
}

TEST_F(TransactionTest, Nested)
{
    auto broker = unf::Broker::Create(_stage);

    ASSERT_FALSE(broker->IsInTransaction());

    {
        unf::NoticeTransaction transaction1(broker);
        ASSERT_EQ(transaction1.GetBroker(), broker);

        ASSERT_TRUE(broker->IsInTransaction());

        broker->Send<::Test::MergeableNotice>();
        broker->Send<::Test::MergeableNotice>();
        broker->Send<::Test::MergeableNotice>();

        broker->Send<::Test::UnMergeableNotice>();
        broker->Send<::Test::UnMergeableNotice>();
        broker->Send<::Test::UnMergeableNotice>();

        {
            unf::NoticeTransaction transaction2(broker);
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

TEST_F(TransactionTest, NestedWithFilter)
{
    auto broker = unf::Broker::Create(_stage);

    ASSERT_FALSE(broker->IsInTransaction());

    {
        unf::NoticeTransaction transaction1(broker);
        ASSERT_EQ(transaction1.GetBroker(), broker);

        ASSERT_TRUE(broker->IsInTransaction());

        broker->Send<::Test::MergeableNotice>();
        broker->Send<::Test::MergeableNotice>();
        broker->Send<::Test::MergeableNotice>();

        broker->Send<::Test::UnMergeableNotice>();
        broker->Send<::Test::UnMergeableNotice>();
        broker->Send<::Test::UnMergeableNotice>();

        {
            // Filter out UnMergeableNotice type.
            std::string target = typeid(::Test::UnMergeableNotice).name();
            auto predicate = [&](const unf::UnfNotice::StageNotice& n) {
                return (typeid(n).name() != target);
            };

            unf::NoticeTransaction transaction2(broker, predicate);
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
    ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 3);
}
