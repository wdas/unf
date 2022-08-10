#include "broker.h"
#include "dispatcher.h"

#include <TestUsdNoticeBroker/listener.h>
#include <TestUsdNoticeBroker/testDispatcher.h>

#include <gtest/gtest.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/weakBase.h>

class DispatcherTest : public ::testing::Test {
protected:
    using StageDispatcherPtr = PXR_NS::TfRefPtr<PXR_NS::StageDispatcher>;
    using NewStageDispatcherPtr = PXR_NS::TfRefPtr<::Test::NewStageDispatcher>;
    using TestDispatcherPtr = PXR_NS::TfRefPtr<::Test::TestDispatcher>;

    using Listener = ::Test::Listener<
        ::Test::InputNotice,
        ::Test::OutputNotice1,
        ::Test::OutputNotice2
    >;

    void SetUp() override {
        _stage = PXR_NS::UsdStage::CreateInMemory();
        _listener.SetStage(_stage);
    }

    PXR_NS::UsdStageRefPtr _stage;
    Listener _listener;
};

TEST_F(DispatcherTest, Original)
{
    auto broker = PXR_NS::NoticeBroker::Create(_stage);

    // Ensure that the stage dispatcher type is correct.
    auto dispatcher = broker->GetDispatcher("StageDispatcher");
    ASSERT_TRUE(TfDynamic_cast<StageDispatcherPtr>(dispatcher));

    // Sending InputNotice does not trigger any new notices.
    ::Test::InputNotice().Send(PXR_NS::TfWeakPtr<PXR_NS::UsdStage>(_stage));

    ASSERT_EQ(_listener.Received<::Test::OutputNotice1>(), 0);
    ASSERT_EQ(_listener.Received<::Test::OutputNotice2>(), 0);
}

TEST_F(DispatcherTest, ReplaceOriginal)
{
    auto broker = PXR_NS::NoticeBroker::Create(_stage);
    broker->AddDispatcher<::Test::NewStageDispatcher>();

    // Ensure that stage dispacther has been replaced as expected.
    auto dispatcher = broker->GetDispatcher("StageDispatcher");
    ASSERT_TRUE(TfDynamic_cast<NewStageDispatcherPtr>(dispatcher));
    ASSERT_FALSE(TfDynamic_cast<StageDispatcherPtr>(dispatcher));

    // Sending InputNotice now triggers OutputNotice1.
    ::Test::InputNotice().Send(PXR_NS::TfWeakPtr<PXR_NS::UsdStage>(_stage));

    ASSERT_EQ(_listener.Received<::Test::OutputNotice1>(), 1);
    ASSERT_EQ(_listener.Received<::Test::OutputNotice2>(), 0);
}

TEST_F(DispatcherTest, AddNew)
{
    auto broker = PXR_NS::NoticeBroker::Create(_stage);
    broker->AddDispatcher<::Test::TestDispatcher>();

    // Ensure that stage dispacther has not been replaced.
    auto dispatcher1 = broker->GetDispatcher("StageDispatcher");
    ASSERT_TRUE(TfDynamic_cast<StageDispatcherPtr>(dispatcher1));
    ASSERT_FALSE(TfDynamic_cast<TestDispatcherPtr>(dispatcher1));

    // Ensure that the new dispatcher is accessible from its identifier.
    auto dispatcher2 = broker->GetDispatcher("TestDispatcher");
    ASSERT_TRUE(TfDynamic_cast<TestDispatcherPtr>(dispatcher2));

    // Sending the special InputNotice now triggers OutputNotice2.
    ::Test::InputNotice().Send(PXR_NS::TfWeakPtr<PXR_NS::UsdStage>(_stage));

    ASSERT_EQ(_listener.Received<::Test::OutputNotice1>(), 0);
    ASSERT_EQ(_listener.Received<::Test::OutputNotice2>(), 1);
}
