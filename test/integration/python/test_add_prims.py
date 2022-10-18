# -*- coding: utf-8 -*-

from pxr import Usd, Tf
from usd_notice_framework import Broker, UnfNotice

import pytest


@pytest.mark.parametrize("notice_type, excepted", [
    ("StageNotice", 6),
    ("StageContentsChanged", 3),
    ("ObjectsChanged", 3),
    ("StageEditTargetChanged", 0),
    ("LayerMutingChanged", 0),
], ids=[
    "StageNotice",
    "StageContentsChanged",
    "ObjectsChanged",
    "StageEditTargetChanged",
    "LayerMutingChanged",
])
def test_add_prims(notice_type, excepted):
    """Add several prims to the stage.
    """
    stage = Usd.Stage.CreateInMemory()
    Broker.Create(stage)

    # Listen to broker notice.
    received_broker = []
    key1 = Tf.Notice.Register(
        getattr(UnfNotice, notice_type),
        lambda n, _: received_broker.append(n), stage)

    # Listen to corresponding USD notice.
    received_usd = []
    key2 = Tf.Notice.Register(
        getattr(Usd.Notice, notice_type),
        lambda n, _: received_usd.append(n), stage)

    stage.DefinePrim("/Foo")
    stage.DefinePrim("/Bar")
    stage.DefinePrim("/Baz")

    # Ensure that we received the same number of notices.
    assert len(received_broker) == excepted
    assert len(received_usd) == excepted

@pytest.mark.parametrize("notice_type, expected_usd, expected_broker", [
    ("StageNotice", 6, 2),
    ("StageContentsChanged", 3, 1),
    ("ObjectsChanged", 3, 1),
    ("StageEditTargetChanged", 0, 0),
    ("LayerMutingChanged", 0, 0),
], ids=[
    "StageNotice",
    "StageContentsChanged",
    "ObjectsChanged",
    "StageEditTargetChanged",
    "LayerMutingChanged",
])
def test_add_prims_batching(notice_type, expected_usd, expected_broker):
    """Add several prims to the stage and batch broker notices.
    """
    stage = Usd.Stage.CreateInMemory()
    broker = Broker.Create(stage)

    # Listen to broker notice.
    received_broker = []
    key1 = Tf.Notice.Register(
        getattr(UnfNotice, notice_type),
        lambda n, _: received_broker.append(n), stage)

    # Listen to corresponding USD notice.
    received_usd = []
    key2 = Tf.Notice.Register(
        getattr(Usd.Notice, notice_type),
        lambda n, _: received_usd.append(n), stage)

    broker.BeginTransaction()

    stage.DefinePrim("/Foo")
    stage.DefinePrim("/Bar")
    stage.DefinePrim("/Baz")

    # Ensure that broker notices are not sent during a transaction.
    assert len(received_broker) == 0

    # While USD Notices are being sent as expected.
    assert len(received_usd) == expected_usd

    broker.EndTransaction()

    # Ensure that consolidated broker notices are sent after a transaction.
    assert len(received_broker) == expected_broker

@pytest.mark.parametrize("notice_type, expected_usd", [
    ("StageNotice", 6),
    ("StageContentsChanged", 3),
    ("ObjectsChanged", 3),
    ("StageEditTargetChanged", 0),
    ("LayerMutingChanged", 0),
], ids=[
    "StageNotice",
    "StageContentsChanged",
    "ObjectsChanged",
    "StageEditTargetChanged",
    "LayerMutingChanged",
])
def test_add_prims_blocking(notice_type, expected_usd):
    """Add several prims to the stage and block broker notices.
    """
    stage = Usd.Stage.CreateInMemory()
    broker = Broker.Create(stage)

    # Listen to broker notice.
    received_broker = []
    key1 = Tf.Notice.Register(
        getattr(UnfNotice, notice_type),
        lambda n, _: received_broker.append(n), stage)

    # Listen to corresponding USD notice.
    received_usd = []
    key2 = Tf.Notice.Register(
        getattr(Usd.Notice, notice_type),
        lambda n, _: received_usd.append(n), stage)

    # Predicate blocking all broker notices.
    broker.BeginTransaction(predicate=lambda _: False)

    stage.DefinePrim("/Foo")
    stage.DefinePrim("/Bar")
    stage.DefinePrim("/Baz")

    # Ensure that broker notices are not sent during a transaction.
    assert len(received_broker) == 0

    # While USD Notices are being sent as expected.
    assert len(received_usd) == expected_usd

    broker.EndTransaction()

    # Ensure that no broker notices have been received.
    assert len(received_broker) == 0

def test_add_prims_transaction_objectschanged():
    """Add several prims to the stage during transaction and analyze
    ObjectsChanged notice.

    """
    stage = Usd.Stage.CreateInMemory()
    broker = Broker.Create(stage)

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        assert len(notice.GetResyncedPaths()) == 3
        assert len(notice.GetChangedInfoOnlyPaths()) == 0
        assert notice.GetResyncedPaths()[0] == "/Foo"
        assert notice.GetResyncedPaths()[1] == "/Bar"
        assert notice.GetResyncedPaths()[2] == "/Baz"
        received.append(notice)

    key = Tf.Notice.Register(UnfNotice.ObjectsChanged, _validate, stage)

    broker.BeginTransaction()

    stage.DefinePrim("/Foo")
    stage.DefinePrim("/Bar")
    stage.DefinePrim("/Baz")

    broker.EndTransaction()

    # Ensure that one notice was received.
    assert len(received) == 1
