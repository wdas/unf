# -*- coding: utf-8 -*-

from pxr import Usd, Sdf, Tf
from usd_notice_framework import Broker, BrokerNotice, NoticeCache

import pytest


@pytest.mark.parametrize("notice_type, excepted", [
    ("StageNotice", 12),
    ("StageContentsChanged", 4),
    ("ObjectsChanged", 4),
    ("StageEditTargetChanged", 0),
    ("LayerMutingChanged", 4),
], ids=[
    "StageNotice",
    "StageContentsChanged",
    "ObjectsChanged",
    "StageEditTargetChanged",
    "LayerMutingChanged",
])
def test_mute_layers(notice_type, excepted, stage_with_layers):
    """Mute several layers.
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

    # Keep ref pointer to the layer we try to mute and unmute to
    # prevent it for being destroyed when it is muted.
    _layer = Sdf.Layer.FindOrOpen(layers[1])

    stage.MuteLayer(layers[0])
    stage.MuteLayer(layers[1])
    stage.UnmuteLayer(layers[1])
    stage.MuteAndUnmuteLayers([layers[2], layers[1]], [])

    # Ensure that we received the same number of notices.
    assert len(received_broker) == excepted
    assert len(received_usd) == excepted

@pytest.mark.parametrize("notice_type, expected_usd, expected_broker", [
    ("StageNotice", 12, 3),
    ("StageContentsChanged", 4, 1),
    ("ObjectsChanged", 4, 1),
    ("StageEditTargetChanged", 0, 0),
    ("LayerMutingChanged", 4, 1),
], ids=[
    "StageNotice",
    "StageContentsChanged",
    "ObjectsChanged",
    "StageEditTargetChanged",
    "LayerMutingChanged",
])
def test_mute_layers_batching(
    notice_type, expected_usd, expected_broker, stage_with_layers
):
    """Mute several layers and batch broker notices.
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

    # Keep ref pointer to the layer we try to mute and unmute to
    # prevent it for being destroyed when it is muted.
    _layer = Sdf.Layer.FindOrOpen(layers[1])

    stage.MuteLayer(layers[0])
    stage.MuteLayer(layers[1])
    stage.UnmuteLayer(layers[1])
    stage.MuteAndUnmuteLayers([layers[2], layers[1]], [])

    # Ensure that broker notices are not sent during a transaction.
    assert len(received_broker) == 0

    # While USD Notices are being sent as expected.
    assert len(received_usd) == expected_usd

    broker.EndTransaction()

    # Ensure that consolidated broker notices are sent after a transaction.
    assert len(received_broker) == expected_broker

@pytest.mark.parametrize("notice_type, expected_usd", [
    ("StageNotice", 12),
    ("StageContentsChanged", 4),
    ("ObjectsChanged", 4),
    ("StageEditTargetChanged", 0),
    ("LayerMutingChanged", 4),
], ids=[
    "StageNotice",
    "StageContentsChanged",
    "ObjectsChanged",
    "StageEditTargetChanged",
    "LayerMutingChanged",
])
def test_mute_layers_blocking(
    notice_type, expected_usd, stage_with_layers
):
    """Mute several layers and block broker notices.
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

    # Predicate blocking all broker notices.
    broker.BeginTransaction(predicate=lambda _: False)

    layers = stage.GetRootLayer().subLayerPaths

    # Keep ref pointer to the layer we try to mute and unmute to
    # prevent it for being destroyed when it is muted.
    _layer = Sdf.Layer.FindOrOpen(layers[1])

    stage.MuteLayer(layers[0])
    stage.MuteLayer(layers[1])
    stage.UnmuteLayer(layers[1])
    stage.MuteAndUnmuteLayers([layers[2], layers[1]], [])

    # Ensure that broker notices are not sent during a transaction.
    assert len(received_broker) == 0

    # While USD Notices are being sent as expected.
    assert len(received_usd) == expected_usd

    broker.EndTransaction()

    # Ensure that no broker notices have been received.
    assert len(received_broker) == 0

def test_mute_layers_transaction_objectschanged(stage_with_layers):
    """Mute several layers during transaction and analyze ObjectsChanged notice.
    """
    stage = stage_with_layers
    broker = Broker.Create(stage)

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        assert len(notice.GetResyncedPaths()) == 1
        assert len(notice.GetChangedInfoOnlyPaths()) == 0
        assert notice.GetResyncedPaths()[0] == "/"
        received.append(notice)

    # Create prim before caching to trigger resync path when muting.
    layers = stage.GetRootLayer().subLayerPaths
    layer = Sdf.Layer.Find(layers[0])
    stage.SetEditTarget(Usd.EditTarget(layer))
    stage.DefinePrim("/Foo")

    layer2 = Sdf.Layer.Find(layers[1])
    stage.SetEditTarget(Usd.EditTarget(layer2))
    stage.DefinePrim("/Bar")

    key = Tf.Notice.Register(BrokerNotice.ObjectsChanged, _validate, stage)

    broker.BeginTransaction()

    stage.MuteLayer(layers[0])
    stage.MuteLayer(layers[1])
    stage.UnmuteLayer(layers[1])
    stage.MuteAndUnmuteLayers([layers[2], layers[1]], [])

    broker.EndTransaction()

    # Ensure that one notice was received.
    assert len(received) == 1

def test_mute_layers_transaction_layermutingchanged(stage_with_layers):
    """Mute several layers during transaction and analyze
    LayerMutingChanged notice.

    """
    stage = stage_with_layers
    layers = stage.GetRootLayer().subLayerPaths

    broker = Broker.Create(stage)

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        assert len(notice.GetMutedLayers()) == 3
        assert len(notice.GetUnmutedLayers()) == 0
        assert notice.GetMutedLayers()[0] == layers[0]
        assert notice.GetMutedLayers()[1] == layers[2]
        assert notice.GetMutedLayers()[2] == layers[1]
        received.append(notice)

    key = Tf.Notice.Register(BrokerNotice.LayerMutingChanged, _validate, stage)

    broker.BeginTransaction()

    # Keep ref pointer to the layer we try to mute and unmute to
    # prevent it for being destroyed when it is muted.
    _layer = Sdf.Layer.FindOrOpen(layers[1])

    stage.MuteLayer(layers[0])
    stage.MuteLayer(layers[1])
    stage.UnmuteLayer(layers[1])
    stage.MuteAndUnmuteLayers([layers[2], layers[1]], [])

    broker.EndTransaction()

    # Ensure that one notice was received.
    assert len(received) == 1

def test_mute_layers_caching_objectschanged(stage_with_layers):
    """Mute several layers while caching ObjectsChanged notices.
    """
    stage = stage_with_layers
    Broker.Create(stage)

    # Create prim before caching to trigger resync path when muting.
    layers = stage.GetRootLayer().subLayerPaths
    layer1 = Sdf.Layer.Find(layers[0])
    stage.SetEditTarget(Usd.EditTarget(layer1))
    stage.DefinePrim("/Foo")

    layer2 = Sdf.Layer.Find(layers[1])
    stage.SetEditTarget(Usd.EditTarget(layer2))
    stage.DefinePrim("/Bar")

    cache = NoticeCache(BrokerNotice.ObjectsChanged)

    # Edit the stage...
    stage.MuteLayer(layers[0])
    stage.MuteLayer(layers[1])
    stage.UnmuteLayer(layers[1])
    stage.MuteAndUnmuteLayers([layers[2], layers[1]], [])

    # Ensure that four notices have been cached.
    assert cache.Size() == 4

    # Ensure that notice data are all as expected.
    assert len(cache.GetAll()[0].GetResyncedPaths()) == 1
    assert len(cache.GetAll()[0].GetChangedInfoOnlyPaths()) == 0
    assert cache.GetAll()[0].GetResyncedPaths()[0] == "/"

    assert len(cache.GetAll()[1].GetResyncedPaths()) == 1
    assert len(cache.GetAll()[1].GetChangedInfoOnlyPaths()) == 0
    assert cache.GetAll()[1].GetResyncedPaths()[0] == "/"

    assert len(cache.GetAll()[2].GetResyncedPaths()) == 1
    assert len(cache.GetAll()[2].GetChangedInfoOnlyPaths()) == 0
    assert cache.GetAll()[2].GetResyncedPaths()[0] == "/"

    assert len(cache.GetAll()[3].GetResyncedPaths()) == 1
    assert len(cache.GetAll()[3].GetChangedInfoOnlyPaths()) == 0
    assert cache.GetAll()[3].GetResyncedPaths()[0] == "/"

    cache.MergeAll()

    # Ensure that we have one merged notice after consolidation.
    assert cache.Size() == 1

    # Ensure that the content of the notice is correct.
    assert len(cache.GetAll()[0].GetResyncedPaths()) == 1
    assert len(cache.GetAll()[0].GetChangedInfoOnlyPaths()) == 0
    assert cache.GetAll()[0].GetResyncedPaths()[0] == "/"

def test_mute_layers_caching_stagecontentschanged(stage_with_layers):
    """Mute several layers while caching StageContentsChanged notices.
    """
    stage = stage_with_layers
    Broker.Create(stage)

    cache = NoticeCache(BrokerNotice.StageContentsChanged)

    # Edit the stage...
    layers = stage.GetRootLayer().subLayerPaths

    # Keep ref pointer to the layer we try to mute and unmute to
    # prevent it for being destroyed when it is muted.
    _layer = Sdf.Layer.FindOrOpen(layers[1])

    stage.MuteLayer(layers[0])
    stage.MuteLayer(layers[1])
    stage.UnmuteLayer(layers[1])
    stage.MuteAndUnmuteLayers([layers[2], layers[1]], [])

    # Ensure that four notices have been cached.
    assert cache.Size() == 4

    cache.MergeAll()

    # Ensure that we have still one notice after consolidation.
    assert cache.Size() == 1

def test_mute_layers_caching_layermutingchanged(stage_with_layers):
    """Mute several layers while caching LayerMutingChanged notices.
    """
    stage = stage_with_layers
    Broker.Create(stage)

    cache = NoticeCache(BrokerNotice.LayerMutingChanged)

    # Edit the stage...
    layers = stage.GetRootLayer().subLayerPaths

    # Keep ref pointer to the layer we try to mute and unmute to
    # prevent it for being destroyed when it is muted.
    _layer = Sdf.Layer.FindOrOpen(layers[1])

    stage.MuteLayer(layers[0])
    stage.MuteLayer(layers[1])
    stage.UnmuteLayer(layers[1])
    stage.MuteAndUnmuteLayers([layers[2], layers[1]], [])

    # Ensure that four notices have been cached.
    assert cache.Size() == 4

    # Ensure that notice data are all as expected.
    assert len(cache.GetAll()[0].GetMutedLayers()) == 1
    assert len(cache.GetAll()[0].GetUnmutedLayers()) == 0
    assert cache.GetAll()[0].GetMutedLayers()[0] == layers[0]

    assert len(cache.GetAll()[1].GetMutedLayers()) == 1
    assert len(cache.GetAll()[1].GetUnmutedLayers()) == 0
    assert cache.GetAll()[1].GetMutedLayers()[0] == layers[1]

    assert len(cache.GetAll()[2].GetMutedLayers()) == 0
    assert len(cache.GetAll()[2].GetUnmutedLayers()) == 1
    assert cache.GetAll()[2].GetUnmutedLayers()[0] == layers[1]

    assert len(cache.GetAll()[3].GetMutedLayers()) == 2
    assert len(cache.GetAll()[3].GetUnmutedLayers()) == 0
    assert cache.GetAll()[3].GetMutedLayers()[0] == layers[2]
    assert cache.GetAll()[3].GetMutedLayers()[1] == layers[1]

    cache.MergeAll()

    # Ensure that we have one merged notice after consolidation.
    assert cache.Size() == 1

    # Ensure that the content of the notice is correct.
    assert len(cache.GetAll()[0].GetMutedLayers()) == 3
    assert len(cache.GetAll()[0].GetUnmutedLayers()) == 0
    assert cache.GetAll()[0].GetMutedLayers()[0] == layers[0]
    assert cache.GetAll()[0].GetMutedLayers()[1] == layers[2]
    assert cache.GetAll()[0].GetMutedLayers()[2] == layers[1]
