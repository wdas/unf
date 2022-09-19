# -*- coding: utf-8 -*-

from pxr import Usd, Sdf, Tf
from usd_notice_framework import Broker, BrokerNotice, NoticeCache

import pytest


@pytest.mark.parametrize("notice_type, excepted", [
    ("StageNotice", 2),
    ("StageContentsChanged", 0),
    ("ObjectsChanged", 0),
    ("StageEditTargetChanged", 2),
    ("LayerMutingChanged", 0),
], ids=[
    "StageNotice",
    "StageContentsChanged",
    "ObjectsChanged",
    "StageEditTargetChanged",
    "LayerMutingChanged",
])
def test_change_edit_target(notice_type, excepted, stage_with_layers):
    """Change edit target.
    """
    stage = stage_with_layers
    Broker.Create(stage)

    # Listen to broker notice.
    received_broker = []
    key1 = Tf.Notice.Register(
        getattr(BrokerNotice, notice_type),
        lambda n, _: received_broker.append(n), stage)

    # Listen to corresponding USD notice.
    received_usd = []
    key2 = Tf.Notice.Register(
        getattr(Usd.Notice, notice_type),
        lambda n, _: received_usd.append(n), stage)

    layers = stage.GetRootLayer().subLayerPaths
    layer1 = Sdf.Layer.Find(layers[0])
    layer2 = Sdf.Layer.Find(layers[1])

    stage.SetEditTarget(Usd.EditTarget(layer1))
    stage.SetEditTarget(Usd.EditTarget(layer2))

    # Ensure that we received the same number of notices.
    assert len(received_broker) == excepted
    assert len(received_usd) == excepted

@pytest.mark.parametrize("notice_type, expected_usd, expected_broker", [
    ("StageNotice", 2, 1),
    ("StageContentsChanged", 0, 0),
    ("ObjectsChanged", 0, 0),
    ("StageEditTargetChanged", 2, 1),
    ("LayerMutingChanged", 0, 0),
], ids=[
    "StageNotice",
    "StageContentsChanged",
    "ObjectsChanged",
    "StageEditTargetChanged",
    "LayerMutingChanged",
])
def test_change_edit_target_batching(
    notice_type, expected_usd, expected_broker, stage_with_layers
):
    """Change edit target and batch broker notices.
    """
    stage = stage_with_layers
    broker = Broker.Create(stage)

    # Listen to broker notice.
    received_broker = []
    key1 = Tf.Notice.Register(
        getattr(BrokerNotice, notice_type),
        lambda n, _: received_broker.append(n), stage)

    # Listen to corresponding USD notice.
    received_usd = []
    key2 = Tf.Notice.Register(
        getattr(Usd.Notice, notice_type),
        lambda n, _: received_usd.append(n), stage)

    broker.BeginTransaction()

    layers = stage.GetRootLayer().subLayerPaths
    layer1 = Sdf.Layer.Find(layers[0])
    layer2 = Sdf.Layer.Find(layers[1])

    stage.SetEditTarget(Usd.EditTarget(layer1))
    stage.SetEditTarget(Usd.EditTarget(layer2))

    # Ensure that broker notices are not sent during a transaction.
    assert len(received_broker) == 0

    # While USD Notices are being sent as expected.
    assert len(received_usd) == expected_usd

    broker.EndTransaction()

    # Ensure that consolidated broker notices are sent after a transaction.
    assert len(received_broker) == expected_broker

@pytest.mark.parametrize("notice_type, expected_usd", [
    ("StageNotice", 2),
    ("StageContentsChanged", 0),
    ("ObjectsChanged", 0),
    ("StageEditTargetChanged", 2),
    ("LayerMutingChanged", 0),
], ids=[
    "StageNotice",
    "StageContentsChanged",
    "ObjectsChanged",
    "StageEditTargetChanged",
    "LayerMutingChanged",
])
def test_change_edit_target_blocking(
    notice_type, expected_usd, stage_with_layers
):
    """Change edit target and block broker notices.
    """
    stage = stage_with_layers
    broker = Broker.Create(stage)

    # Listen to broker notice.
    received_broker = []
    key1 = Tf.Notice.Register(
        getattr(BrokerNotice, notice_type),
        lambda n, _: received_broker.append(n), stage)

    # Listen to corresponding USD notice.
    received_usd = []
    key2 = Tf.Notice.Register(
        getattr(Usd.Notice, notice_type),
        lambda n, _: received_usd.append(n), stage)

    broker.BeginTransaction()
    # Predicate blocking all broker notices.
    broker.AddFilter(predicate=lambda _: False)

    layers = stage.GetRootLayer().subLayerPaths
    layer1 = Sdf.Layer.Find(layers[0])
    layer2 = Sdf.Layer.Find(layers[1])

    stage.SetEditTarget(Usd.EditTarget(layer1))
    stage.SetEditTarget(Usd.EditTarget(layer2))

    # Ensure that broker notices are not sent during a transaction.
    assert len(received_broker) == 0

    # While USD Notices are being sent as expected.
    assert len(received_usd) == expected_usd

    broker.PopFilter()
    broker.EndTransaction()

    # Ensure that no broker notices have been received.
    assert len(received_broker) == 0

def test_change_edit_target_caching(stage_with_layers):
    """Change edit target while caching StageEditTargetChanged notices.
    """
    stage = stage_with_layers
    Broker.Create(stage)

    cache = NoticeCache(BrokerNotice.StageEditTargetChanged)

    # Edit the stage...
    layers = stage.GetRootLayer().subLayerPaths
    layer1 = Sdf.Layer.Find(layers[0])
    layer2 = Sdf.Layer.Find(layers[1])

    stage.SetEditTarget(Usd.EditTarget(layer1))
    stage.SetEditTarget(Usd.EditTarget(layer2))

    # Ensure that two notices has been cached.
    assert cache.Size() == 2

    cache.MergeAll()

    # Ensure that we have one notice after consolidation.
    assert cache.Size() == 1

