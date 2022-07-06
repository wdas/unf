# :coding: utf-8

from pxr import Usd, Sdf, Tf
from usd_notice_broker import NoticeBroker, BrokerNotice

import pytest


def test_create():
    """Create a broker from stage."""
    stage = Usd.Stage.CreateInMemory()
    broker = NoticeBroker.Create(stage)
    assert broker.GetStage() == stage
    assert broker.IsInTransaction() is False

def test_create_twice():
    """Create two brokers from stage."""
    stage = Usd.Stage.CreateInMemory()
    broker1 = NoticeBroker.Create(stage)
    broker2 = NoticeBroker.Create(stage)
    assert broker1 == broker2

@pytest.mark.parametrize("notice_type, excepted", [
    ("StageNotice", 2),
    ("StageContentsChanged", 1),
    ("ObjectsChanged", 1),
    ("StageEditTargetChanged", 0),
    ("LayerMutingChanged", 0),
], ids=[
    "StageNotice",
    "StageContentsChanged",
    "ObjectsChanged",
    "StageEditTargetChanged",
    "LayerMutingChanged",
])
def test_add_prim(notice_type, excepted):
    """Add one prim to the stage and listen to notices.
    """
    stage = Usd.Stage.CreateInMemory()
    NoticeBroker.Create(stage)

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

    # Edit the stage...
    stage.DefinePrim("/Foo")

    # Ensure that we received the same number of notices.
    assert len(received_broker) == excepted
    assert len(received_usd) == excepted

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
    """Add several prims to the stage and listen to notices.
    """
    stage = Usd.Stage.CreateInMemory()
    NoticeBroker.Create(stage)

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

    # Edit the stage...
    stage.DefinePrim("/Foo")
    stage.DefinePrim("/Bar")
    stage.DefinePrim("/Baz")

    # Ensure that we received the same number of notices.
    assert len(received_broker) == excepted
    assert len(received_usd) == excepted

@pytest.mark.parametrize("notice_type, excepted", [
    ("StageNotice", 3),
    ("StageContentsChanged", 1),
    ("ObjectsChanged", 1),
    ("StageEditTargetChanged", 0),
    ("LayerMutingChanged", 1),
], ids=[
    "StageNotice",
    "StageContentsChanged",
    "ObjectsChanged",
    "StageEditTargetChanged",
    "LayerMutingChanged",
])
def test_mute_layer(notice_type, excepted, stage_with_layers):
    """Mute one layer and listen to notices.
    """
    stage = stage_with_layers
    NoticeBroker.Create(stage)

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

    # Edit the stage...
    layers = stage.GetRootLayer().subLayerPaths
    stage.MuteLayer(layers[0])

    # Ensure that we received the same number of notices.
    assert len(received_broker) == excepted
    assert len(received_usd) == excepted

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
    """Mute several layers and listen to notices.
    """
    stage = stage_with_layers
    NoticeBroker.Create(stage)

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

    # Edit the stage...
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
    """Change edit target and listen to notices.
    """
    stage = stage_with_layers
    NoticeBroker.Create(stage)

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

    # Edit the stage...
    layers = stage.GetRootLayer().subLayerPaths
    layer1 = Sdf.Layer.Find(layers[0])
    layer2 = Sdf.Layer.Find(layers[1])

    stage.SetEditTarget(Usd.EditTarget(layer1))
    stage.SetEditTarget(Usd.EditTarget(layer2))

    # Ensure that we received the same number of notices.
    assert len(received_broker) == excepted
    assert len(received_usd) == excepted
