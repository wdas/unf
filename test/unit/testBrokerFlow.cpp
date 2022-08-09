#include "broker.h"

#include <TestUsdNoticeBroker/listener.h>
#include <TestUsdNoticeBroker/testNotice.h>

#include <gtest/gtest.h>
#include <pxr/usd/usd/stage.h>

class BrokerFlowTest : public ::testing::Test {
protected:
    using Listener = ::Test::Listener<
        ::Test::MergeableNotice,
        ::Test::UnMergeableNotice
    >;

    void SetUp() override {
        _stage = PXR_NS::UsdStage::CreateInMemory();
        _listener.SetStage(_stage);
    }

    UsdStageRefPtr _stage;
    Listener _listener;
};

TEST_F(BrokerFlowTest, Send)
{
    auto broker = PXR_NS::NoticeBroker::Create(_stage);

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
    auto broker = PXR_NS::NoticeBroker::Create(_stage);

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
    auto broker = PXR_NS::NoticeBroker::Create(_stage);

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
    auto broker = PXR_NS::NoticeBroker::Create(_stage);

    class DataListener : public ::Test::ListenerBase<::Test::MergeableNotice>
    {
    public:
        using ::Test::ListenerBase<::Test::MergeableNotice>::ListenerBase;

    private:
        void OnReceiving(
            const ::Test::MergeableNotice& n,
            const PXR_NS::UsdStageWeakPtr&) override
        {
            // Ensure that data is merged as expected.
            ASSERT_EQ(n.GetData(),
                ::Test::DataMap({{"Foo", "Test2"}, {"Bar", "Test3"}}));
        }
    };

    DataListener dataListener(_stage);

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
}
