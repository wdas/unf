# :coding: utf-8

from pxr import Usd, Sdf, Tf
from usd_notice_broker import NoticeBroker, BrokerNotice

import pytest


def test_transaction():
    """Start and end a transaction."""
    stage = Usd.Stage.CreateInMemory()
    broker = NoticeBroker.Create(stage)

    broker.BeginTransaction()
    assert broker.IsInTransaction() is True

    broker.EndTransaction()
    assert broker.IsInTransaction() is False

def test_transaction_nested():
    """Start and end a nested transaction."""
    stage = Usd.Stage.CreateInMemory()
    broker = NoticeBroker.Create(stage)

    broker.BeginTransaction()
    assert broker.IsInTransaction() is True

    broker.BeginTransaction()
    assert broker.IsInTransaction() is True

    broker.BeginTransaction()
    assert broker.IsInTransaction() is True

    broker.EndTransaction()
    assert broker.IsInTransaction() is True

    broker.EndTransaction()
    assert broker.IsInTransaction() is True

    broker.EndTransaction()
    assert broker.IsInTransaction() is False

@pytest.mark.parametrize("notice_type, excepted, consolidated", [
    ("StageNotice", 2, 2),
    ("StageContentsChanged", 1, 1),
    ("ObjectsChanged", 1, 1),
    ("StageEditTargetChanged", 0, 0),
    ("LayerMutingChanged", 0, 0),
], ids=[
    "StageNotice",
    "StageContentsChanged",
    "ObjectsChanged",
    "StageEditTargetChanged",
    "LayerMutingChanged",
])
def test_add_prim(notice_type, excepted, consolidated):
    """Add one prim to the stage and listen to notices.
    """
    stage = Usd.Stage.CreateInMemory()
    broker = NoticeBroker.Create(stage)

    # Listen to broker notice.
    # received_broker = []
    # key1 = Tf.Notice.Register(
    #     getattr(BrokerNotice, notice_type),
    #     lambda n, _: received_broker.append(n), stage)

    # # Listen to corresponding USD notice.
    # received_usd = []
    # key2 = Tf.Notice.Register(
    #     getattr(Usd.Notice, notice_type),
    #     lambda n, _: received_usd.append(n), stage)

    broker.BeginTransaction()

    assert broker.IsInTransaction() is True

    # # Edit the stage...
    stage.DefinePrim("/Foo")

    # # Ensure that broker notices are not sent during a transaction.
    # assert len(received_broker) == 0

    # # While USD Notices are being sent as expected.
    # assert len(received_usd) == excepted

    broker.EndTransaction()

    assert broker.IsInTransaction() is False

    # # Ensure that consolidated broker notices are sent after a transaction.
    # assert len(received_broker) == consolidated
