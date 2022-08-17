# -*- coding: utf-8 -*-

from pxr import Usd
from usd_notice_broker import Broker, NoticeTransaction


def test_create_from_stage():
    """Create a transaction from stage."""
    stage = Usd.Stage.CreateInMemory()

    with NoticeTransaction(stage) as transaction:
        broker = transaction.GetBroker()
        assert broker.GetStage() == stage
        assert broker.IsInTransaction() is True

    assert broker.IsInTransaction() is False

def test_create_from_broker():
    """Create a transaction from broker."""
    stage = Usd.Stage.CreateInMemory()
    broker = Broker.Create(stage)

    assert broker.IsInTransaction() is False

    with NoticeTransaction(broker) as transaction:
        assert transaction.GetBroker() == broker
        assert broker.GetStage() == stage
        assert broker.IsInTransaction() is True

    assert broker.IsInTransaction() is False

def test_nested():
    """Create nested transactions."""
    stage = Usd.Stage.CreateInMemory()

    with NoticeTransaction(stage) as transaction1:
        broker = transaction1.GetBroker()
        assert broker.GetStage() == stage
        assert broker.IsInTransaction() is True

        with NoticeTransaction(stage) as transaction2:
            _broker = transaction2.GetBroker()
            assert _broker == broker
            assert _broker.GetStage() == stage
            assert _broker.IsInTransaction() is True

        assert broker.IsInTransaction() is True

    assert broker.IsInTransaction() is False
