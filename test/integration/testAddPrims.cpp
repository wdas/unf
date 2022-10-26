#include <unf/broker.h>

#include <unfTest/listener.h>
#include <unfTest/notice.h>
#include <unfTest/observer.h>

#include <gtest/gtest.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usd/stage.h>

#include <typeinfo>

// namespace aliases for convenience.
using _USD = PXR_NS::UsdNotice;
namespace _UNF = unf::UnfNotice;

class AddPrimsTest : public ::testing::Test {
  protected:
    using UsdListener = ::Test::Listener<
        _USD::StageNotice, _USD::StageContentsChanged, _USD::ObjectsChanged,
        _USD::StageEditTargetChanged, _USD::LayerMutingChanged>;

    using BrokerListener = ::Test::Listener<
        _UNF::StageNotice, _UNF::StageContentsChanged,
        _UNF::ObjectsChanged, _UNF::StageEditTargetChanged,
        _UNF::LayerMutingChanged>;

    void SetUp() override
    {
        _stage = PXR_NS::UsdStage::CreateInMemory();
        _usdListener.SetStage(_stage);
        _brokerListener.SetStage(_stage);
    }

    PXR_NS::UsdStageRefPtr _stage;

    UsdListener _usdListener;
    BrokerListener _brokerListener;
};

TEST_F(AddPrimsTest, Simple)
{
    auto broker = unf::Broker::Create(_stage);

    _stage->DefinePrim(PXR_NS::SdfPath{"/Foo"});
    _stage->DefinePrim(PXR_NS::SdfPath{"/Bar"});
    _stage->DefinePrim(PXR_NS::SdfPath{"/Baz"});

    // Ensure that similar notices are received via the stage and the broker.
    ASSERT_EQ(_usdListener.Received<_USD::StageNotice>(), 6);
    ASSERT_EQ(_usdListener.Received<_USD::StageContentsChanged>(), 3);
    ASSERT_EQ(_usdListener.Received<_USD::ObjectsChanged>(), 3);
    ASSERT_EQ(_usdListener.Received<_USD::StageEditTargetChanged>(), 0);
    ASSERT_EQ(_usdListener.Received<_USD::LayerMutingChanged>(), 0);

    ASSERT_EQ(_brokerListener.Received<_UNF::StageNotice>(), 6);
    ASSERT_EQ(_brokerListener.Received<_UNF::StageContentsChanged>(), 3);
    ASSERT_EQ(_brokerListener.Received<_UNF::ObjectsChanged>(), 3);
    ASSERT_EQ(_brokerListener.Received<_UNF::StageEditTargetChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_UNF::LayerMutingChanged>(), 0);
}

TEST_F(AddPrimsTest, Batching)
{
    auto broker = unf::Broker::Create(_stage);

    broker->BeginTransaction();

    _stage->DefinePrim(PXR_NS::SdfPath{"/Foo"});
    _stage->DefinePrim(PXR_NS::SdfPath{"/Bar"});
    _stage->DefinePrim(PXR_NS::SdfPath{"/Baz"});

    // Ensure that USD Notices are being sent as expected.
    ASSERT_EQ(_usdListener.Received<_USD::StageNotice>(), 6);
    ASSERT_EQ(_usdListener.Received<_USD::StageContentsChanged>(), 3);
    ASSERT_EQ(_usdListener.Received<_USD::ObjectsChanged>(), 3);
    ASSERT_EQ(_usdListener.Received<_USD::StageEditTargetChanged>(), 0);
    ASSERT_EQ(_usdListener.Received<_USD::LayerMutingChanged>(), 0);

    // While broker notices are blocked during a transaction.
    ASSERT_EQ(_brokerListener.Received<_UNF::StageNotice>(), 0);
    ASSERT_EQ(_brokerListener.Received<_UNF::StageContentsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_UNF::ObjectsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_UNF::StageEditTargetChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_UNF::LayerMutingChanged>(), 0);

    broker->EndTransaction();

    // Ensure that consolidated broker notices are sent after a transaction.
    ASSERT_EQ(_brokerListener.Received<_UNF::StageNotice>(), 2);
    ASSERT_EQ(_brokerListener.Received<_UNF::StageContentsChanged>(), 1);
    ASSERT_EQ(_brokerListener.Received<_UNF::ObjectsChanged>(), 1);
    ASSERT_EQ(_brokerListener.Received<_UNF::StageEditTargetChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_UNF::LayerMutingChanged>(), 0);
}

TEST_F(AddPrimsTest, Blocking)
{
    auto broker = unf::Broker::Create(_stage);

    // Pass a predicate to block all broker notices.
    broker->BeginTransaction([](const _UNF::StageNotice&) { return false; });

    _stage->DefinePrim(PXR_NS::SdfPath{"/Foo"});
    _stage->DefinePrim(PXR_NS::SdfPath{"/Bar"});
    _stage->DefinePrim(PXR_NS::SdfPath{"/Baz"});

    // Ensure that USD Notices are being sent as expected.
    ASSERT_EQ(_usdListener.Received<_USD::StageNotice>(), 6);
    ASSERT_EQ(_usdListener.Received<_USD::StageContentsChanged>(), 3);
    ASSERT_EQ(_usdListener.Received<_USD::ObjectsChanged>(), 3);
    ASSERT_EQ(_usdListener.Received<_USD::StageEditTargetChanged>(), 0);
    ASSERT_EQ(_usdListener.Received<_USD::LayerMutingChanged>(), 0);

    // While broker notices are blocked during a transaction.
    ASSERT_EQ(_brokerListener.Received<_UNF::StageNotice>(), 0);
    ASSERT_EQ(_brokerListener.Received<_UNF::StageContentsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_UNF::ObjectsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_UNF::StageEditTargetChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_UNF::LayerMutingChanged>(), 0);

    broker->EndTransaction();

    // Ensure that no broker notices are sent after a transaction either.
    ASSERT_EQ(_brokerListener.Received<_UNF::StageNotice>(), 0);
    ASSERT_EQ(_brokerListener.Received<_UNF::StageContentsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_UNF::ObjectsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_UNF::StageEditTargetChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_UNF::LayerMutingChanged>(), 0);
}

TEST_F(AddPrimsTest, PartialBlocking)
{
    auto broker = unf::Broker::Create(_stage);

    std::string target = typeid(_UNF::ObjectsChanged).name();

    // Pass a predicate to block all broker notices.
    broker->BeginTransaction([&](const _UNF::StageNotice& n) {
        return (n.GetTypeId() == target);
    });

    _stage->DefinePrim(PXR_NS::SdfPath{"/Foo"});
    _stage->DefinePrim(PXR_NS::SdfPath{"/Bar"});
    _stage->DefinePrim(PXR_NS::SdfPath{"/Baz"});

    // Ensure that USD Notices are being sent as expected.
    ASSERT_EQ(_usdListener.Received<_USD::StageNotice>(), 6);
    ASSERT_EQ(_usdListener.Received<_USD::StageContentsChanged>(), 3);
    ASSERT_EQ(_usdListener.Received<_USD::ObjectsChanged>(), 3);
    ASSERT_EQ(_usdListener.Received<_USD::StageEditTargetChanged>(), 0);
    ASSERT_EQ(_usdListener.Received<_USD::LayerMutingChanged>(), 0);

    // While broker notices are blocked during a transaction.
    ASSERT_EQ(_brokerListener.Received<_UNF::StageNotice>(), 0);
    ASSERT_EQ(_brokerListener.Received<_UNF::StageContentsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_UNF::ObjectsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_UNF::StageEditTargetChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_UNF::LayerMutingChanged>(), 0);

    broker->EndTransaction();

    // Ensure that only consolidated ObjectsChanged broker notice are sent.
    ASSERT_EQ(_brokerListener.Received<_UNF::StageNotice>(), 1);
    ASSERT_EQ(_brokerListener.Received<_UNF::StageContentsChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_UNF::ObjectsChanged>(), 1);
    ASSERT_EQ(_brokerListener.Received<_UNF::StageEditTargetChanged>(), 0);
    ASSERT_EQ(_brokerListener.Received<_UNF::LayerMutingChanged>(), 0);
}

TEST_F(AddPrimsTest, Transaction_ObjectsChanged)
{
    auto broker = unf::Broker::Create(_stage);

    ::Test::Observer<_UNF::ObjectsChanged> observer(_stage);

    broker->BeginTransaction();

    _stage->DefinePrim(PXR_NS::SdfPath{"/Foo"});
    _stage->DefinePrim(PXR_NS::SdfPath{"/Bar"});
    _stage->DefinePrim(PXR_NS::SdfPath{"/Baz"});

    ASSERT_EQ(observer.Received(), 0);

    broker->EndTransaction();

    ASSERT_EQ(observer.Received(), 1);

    const auto& n = observer.GetLatestNotice();
    ASSERT_EQ(n.GetResyncedPaths().size(), 3);
    ASSERT_EQ(n.GetResyncedPaths().at(0), PXR_NS::SdfPath{"/Bar"});
    ASSERT_EQ(n.GetResyncedPaths().at(1), PXR_NS::SdfPath{"/Baz"});
    ASSERT_EQ(n.GetResyncedPaths().at(2), PXR_NS::SdfPath{"/Foo"});
    ASSERT_EQ(n.GetChangedInfoOnlyPaths().size(), 0);
}