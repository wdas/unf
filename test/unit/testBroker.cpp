#include <unf/broker.h>

#include <gtest/gtest.h>
#include <pxr/usd/usd/stage.h>

TEST(BrokerTest, Create)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = unf::Broker::Create(stage);
    ASSERT_EQ(broker->GetStage(), stage);
}

TEST(BrokerTest, CreateMultiple)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker1 = unf::Broker::Create(stage);
    ASSERT_EQ(broker1->GetCurrentCount(), 2);

    auto broker2 = unf::Broker::Create(stage);
    ASSERT_EQ(broker1->GetCurrentCount(), 3);
    ASSERT_EQ(broker2->GetCurrentCount(), 3);

    // Brokers referencing the same stage are identical.
    ASSERT_EQ(broker1, broker2);

    auto otherStage = PXR_NS::UsdStage::CreateInMemory();
    auto broker3 = unf::Broker::Create(otherStage);
    ASSERT_EQ(broker3->GetCurrentCount(), 2);

    // Brokers referencing distinct stages are different.
    ASSERT_NE(broker1, broker3);

    // Reference counters from previous variables did not change.
    ASSERT_EQ(broker1->GetCurrentCount(), 3);
    ASSERT_EQ(broker2->GetCurrentCount(), 3);
}

TEST(BrokerTest, CleanRegistry)
{
    auto stage1 = PXR_NS::UsdStage::CreateInMemory();
    auto broker1 = unf::Broker::Create(stage1);
    ASSERT_EQ(broker1->GetCurrentCount(), 2);

    // Stage is destroyed, but broker reference is kept in registry.
    stage1.Reset();
    ASSERT_EQ(broker1->GetCurrentCount(), 2);

    // Registry reference is removed when a new broker is added.
    auto stage2 = PXR_NS::UsdStage::CreateInMemory();
    unf::Broker::Create(stage2);
    ASSERT_EQ(broker1->GetCurrentCount(), 1);
}

TEST(BrokerTest, Reset)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = unf::Broker::Create(stage);
    ASSERT_EQ(broker->GetCurrentCount(), 2);

    broker->Reset();

    ASSERT_EQ(broker->GetCurrentCount(), 1);
}

TEST(BrokerTest, ResetAll)
{
    auto stage1 = PXR_NS::UsdStage::CreateInMemory();
    auto broker1 = unf::Broker::Create(stage1);
    ASSERT_EQ(broker1->GetCurrentCount(), 2);

    auto stage2 = PXR_NS::UsdStage::CreateInMemory();
    auto broker2 = unf::Broker::Create(stage2);
    ASSERT_EQ(broker2->GetCurrentCount(), 2);

    auto stage3 = PXR_NS::UsdStage::CreateInMemory();
    auto broker3 = unf::Broker::Create(stage3);
    ASSERT_EQ(broker3->GetCurrentCount(), 2);

    unf::Broker::ResetAll();

    ASSERT_EQ(broker1->GetCurrentCount(), 1);
    ASSERT_EQ(broker2->GetCurrentCount(), 1);
    ASSERT_EQ(broker3->GetCurrentCount(), 1);
}
