# -*- coding: utf-8 -*-

from pxr import Usd
import usd_notice_framework as unf


def test_create():
    """Create a broker from stage."""
    stage = Usd.Stage.CreateInMemory()
    broker = unf.Broker.Create(stage)
    assert broker.GetStage() == stage

def test_create_twice():
    """Create two brokers from stage."""
    stage = Usd.Stage.CreateInMemory()
    broker1 = unf.Broker.Create(stage)
    broker2 = unf.Broker.Create(stage)
    assert broker1 == broker2

def test_transaction():
    """Start and end a transaction."""
    stage = Usd.Stage.CreateInMemory()
    broker = unf.Broker.Create(stage)

    broker.BeginTransaction()
    assert broker.IsInTransaction() is True

    broker.EndTransaction()
    assert broker.IsInTransaction() is False

def test_transaction_nested():
    """Start and end a nested transaction."""
    stage = Usd.Stage.CreateInMemory()
    broker = unf.Broker.Create(stage)

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

