#include <unf/broker.h>
#include <unf/capturePredicate.h>

#include <unfTest/listener.h>
#include <unfTest/notice.h>
#include <unfTest/observer.h>

#include <gtest/gtest.h>
#include <pxr/usd/usd/stage.h>

class BrokerFlowTest : public ::testing::Test {
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

TEST_F(BrokerFlowTest, Send)
{
    auto broker = unf::Broker::Create(_stage);

    broker->Send<::Test::MergeableNotice>();
    broker->Send<::Test::MergeableNotice>();
    broker->Send<::Test::MergeableNotice>();

    broker->Send<::Test::UnMergeableNotice>();
    broker->Send<::Test::UnMergeableNotice>();
    broker->Send<::Test::UnMergeableNotice>();

    ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 3);
    ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 3);
}

TEST_F(BrokerFlowTest, Transaction)
{
    auto broker = unf::Broker::Create(_stage);

    ASSERT_FALSE(broker->IsInTransaction());

    broker->BeginTransaction();
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

    broker->EndTransaction();
    ASSERT_FALSE(broker->IsInTransaction());

    // Consolidated notices (if required) are sent when transaction is over.
    ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 1);
    ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 3);
}

TEST_F(BrokerFlowTest, NestedTransaction)
{
    auto broker = unf::Broker::Create(_stage);

    ASSERT_FALSE(broker->IsInTransaction());

    broker->BeginTransaction();
    ASSERT_TRUE(broker->IsInTransaction());

    broker->Send<::Test::MergeableNotice>();
    broker->Send<::Test::MergeableNotice>();
    broker->Send<::Test::MergeableNotice>();

    broker->Send<::Test::UnMergeableNotice>();
    broker->Send<::Test::UnMergeableNotice>();
    broker->Send<::Test::UnMergeableNotice>();

    broker->BeginTransaction();
    ASSERT_TRUE(broker->IsInTransaction());

    broker->Send<::Test::MergeableNotice>();
    broker->Send<::Test::MergeableNotice>();
    broker->Send<::Test::MergeableNotice>();

    broker->Send<::Test::UnMergeableNotice>();
    broker->Send<::Test::UnMergeableNotice>();
    broker->Send<::Test::UnMergeableNotice>();

    broker->EndTransaction();
    ASSERT_TRUE(broker->IsInTransaction());

    // No notices are emitted while at least one transaction is started.
    ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 0);
    ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 0);

    broker->EndTransaction();
    ASSERT_FALSE(broker->IsInTransaction());

    // Consolidated notices (if required) are sent when all
    // transactions are over.
    ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 1);
    ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 6);
}

TEST_F(BrokerFlowTest, MergeableNotice)
{
    auto broker = unf::Broker::Create(_stage);

    ::Test::Observer<::Test::MergeableNotice> observer(_stage);

    ASSERT_FALSE(broker->IsInTransaction());

    broker->BeginTransaction();
    ASSERT_TRUE(broker->IsInTransaction());

    broker->Send<::Test::MergeableNotice>(::Test::DataMap({{"Foo", "Test1"}}));
    broker->Send<::Test::MergeableNotice>(::Test::DataMap({{"Foo", "Test2"}}));
    broker->Send<::Test::MergeableNotice>(::Test::DataMap({{"Bar", "Test3"}}));

    broker->EndTransaction();
    ASSERT_FALSE(broker->IsInTransaction());

    // Ensure that only one consolidated notice is received.
    ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 1);

    const auto& n = observer.GetLatestNotice();
    ASSERT_EQ(
        n.GetData(), ::Test::DataMap({{"Foo", "Test2"}, {"Bar", "Test3"}}));
}

TEST_F(BrokerFlowTest, WithFilter)
{
    auto broker = unf::Broker::Create(_stage);

    ASSERT_FALSE(broker->IsInTransaction());

    // Filter out UnMergeableNotice type.
    std::string target = typeid(::Test::UnMergeableNotice).name();
    auto predicate = [&](const unf::UnfNotice::StageNotice& n) {
        return (typeid(n).name() != target);
    };

    broker->BeginTransaction(predicate);
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

    broker->EndTransaction();
    ASSERT_FALSE(broker->IsInTransaction());

    // Consolidated notices (if required) are sent when transaction is over.
    ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 1);
    ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 0);
}

TEST_F(BrokerFlowTest, WithDefaultPredicate)
{
    auto broker = unf::Broker::Create(_stage);

    ASSERT_FALSE(broker->IsInTransaction());

    broker->BeginTransaction(unf::CapturePredicate::Default());
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

    broker->EndTransaction();
    ASSERT_FALSE(broker->IsInTransaction());

    // No notices are emitted after the transaction either.
    ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 1);
    ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 3);
}

TEST_F(BrokerFlowTest, WithBlockAllPredicate)
{
    auto broker = unf::Broker::Create(_stage);

    ASSERT_FALSE(broker->IsInTransaction());

    broker->BeginTransaction(unf::CapturePredicate::BlockAll());
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

    broker->EndTransaction();
    ASSERT_FALSE(broker->IsInTransaction());

    // No notices are emitted after the transaction either.
    ASSERT_EQ(_listener.Received<::Test::MergeableNotice>(), 0);
    ASSERT_EQ(_listener.Received<::Test::UnMergeableNotice>(), 0);
}
