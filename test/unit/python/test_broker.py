# -*- coding: utf-8 -*-

from pxr import Usd
from usd_notice_broker import Broker


def test_create():
    """Create a broker from stage."""
    stage = Usd.Stage.CreateInMemory()
    broker = Broker.Create(stage)
    assert broker.GetStage() == stage

def test_create_twice():
    """Create two brokers from stage."""
    stage = Usd.Stage.CreateInMemory()
    broker1 = Broker.Create(stage)
    broker2 = Broker.Create(stage)
    assert broker1 == broker2

def test_transaction():
    """Start and end a transaction."""
    stage = Usd.Stage.CreateInMemory()
    broker = Broker.Create(stage)

    broker.BeginTransaction()
    assert broker.IsInTransaction() is True

    broker.EndTransaction()
    assert broker.IsInTransaction() is False

def test_transaction_nested():
    """Start and end a nested transaction."""
    stage = Usd.Stage.CreateInMemory()
    broker = Broker.Create(stage)

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

