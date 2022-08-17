#include <UsdNoticeBroker/broker.h>
#include <UsdNoticeBroker/dispatcher.h>

#include <TestUsdNoticeBroker/listener.h>

#include <TestPlugin/TestDispatcher.h>
#include <TestPlugin/NewStageDispatcher.h>

#include <gtest/gtest.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/weakBase.h>

class DispatcherTest : public ::testing::Test {
protected:
    using StageDispatcherPtr = PXR_NS::TfRefPtr<PXR_NS::UNB::StageDispatcher>;
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

TEST_F(DispatcherTest, Discover)
{
    auto broker = PXR_NS::UNB::Broker::Create(_stage);

    // Ensure that stage dispacther has been replaced as expected.
    auto dispatcher = broker->GetDispatcher("StageDispatcher");
    ASSERT_TRUE(PXR_NS::TfDynamic_cast<NewStageDispatcherPtr>(dispatcher));
    ASSERT_FALSE(PXR_NS::TfDynamic_cast<StageDispatcherPtr>(dispatcher));

    // Sending InputNotice now triggers OutputNotice1 and OutputNotice2.
    ::Test::InputNotice().Send(PXR_NS::TfWeakPtr<PXR_NS::UsdStage>(_stage));

    ASSERT_EQ(_listener.Received<::Test::OutputNotice1>(), 1);
    ASSERT_EQ(_listener.Received<::Test::OutputNotice2>(), 1);
}
