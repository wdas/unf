# -*- coding: utf-8 -*-

from pxr import Usd, Tf
from usd_notice_framework import (
    Broker, BrokerNotice, NoticeBlocker, NoticeTransaction
)


def test_create_from_stage():
    """Create a blocker from stage."""
    stage = Usd.Stage.CreateInMemory()

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        received.append(notice)

    key = Tf.Notice.Register(BrokerNotice.ObjectsChanged, _validate, stage)

    with NoticeBlocker(stage) as blocker:
        stage.DefinePrim("/Foo")

        broker = blocker.GetBroker()
        assert broker.GetStage() == stage
        assert broker.IsInTransaction() is True

    assert broker.IsInTransaction() is False

    # Ensure that no notice was received.
    assert len(received) == 0

def test_create_from_broker():
    """Create a blocker from broker."""
    stage = Usd.Stage.CreateInMemory()
    broker = Broker.Create(stage)

    assert broker.IsInTransaction() is False

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        received.append(notice)

    key = Tf.Notice.Register(BrokerNotice.ObjectsChanged, _validate, stage)

    with NoticeBlocker(broker) as blocker:
        stage.DefinePrim("/Foo")

        assert blocker.GetBroker() == broker
        assert broker.GetStage() == stage
        assert broker.IsInTransaction() is True

    assert broker.IsInTransaction() is False

    # Ensure that no notice was received.
    assert len(received) == 0

def test_nested_with_transaction():
    """Create nested blocker with transaction."""
    stage = Usd.Stage.CreateInMemory()

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        assert notice.GetResyncedPaths() == ["/Foo"]
        received.append(notice)

    key = Tf.Notice.Register(BrokerNotice.ObjectsChanged, _validate, stage)

    with NoticeTransaction(stage) as transaction:
        stage.DefinePrim("/Foo")

        broker = transaction.GetBroker()
        assert broker.GetStage() == stage
        assert broker.IsInTransaction() is True

        with NoticeBlocker(stage) as blocker:
            stage.DefinePrim("/Bar")

            _broker = blocker.GetBroker()
            assert _broker == broker
            assert _broker.GetStage() == stage
            assert _broker.IsInTransaction() is True

        assert broker.IsInTransaction() is True

    assert broker.IsInTransaction() is False

    # Ensure that one notice was received.
    assert len(received) == 1
