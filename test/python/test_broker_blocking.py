# :coding: utf-8

import contextlib

from pxr import Usd, Sdf, Tf
from usd_notice_broker import NoticeBroker, BrokerNotice

import pytest


@contextlib.contextmanager
def listen_and_validate_notices(stage, notice_type, excepted_usd):
    """Edit stage and validate that USD notices are received
    while Broker notices are blocked.
    """
    broker = NoticeBroker.Create(stage)

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

    assert broker.IsInTransaction() is True

    # Edit the stage...
    yield

    # Ensure that broker notices are not sent during a transaction.
    assert len(received_broker) == 0

    # While USD Notices are being sent as expected.
    assert len(received_usd) == excepted_usd

    broker.EndTransaction()

    assert broker.IsInTransaction() is False

    # Ensure that no broker notices have been received.
    assert len(received_broker) == 0

@pytest.mark.parametrize("notice_type, excepted_usd", [
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
def test_add_prim(notice_type, excepted_usd):
    """Add one prim to the stage and listen to notices.
    """
    stage = Usd.Stage.CreateInMemory()

    with listen_and_validate_notices(
        stage, notice_type, excepted_usd
    ):
        stage.DefinePrim("/Foo")

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
def test_add_prims(notice_type, expected_usd):
    """Add several prims to the stage and listen to notices.
    """
    stage = Usd.Stage.CreateInMemory()

    with listen_and_validate_notices(
        stage, notice_type, expected_usd
    ):
        stage.DefinePrim("/Foo")
        stage.DefinePrim("/Bar")
        stage.DefinePrim("/Baz")

@pytest.mark.parametrize("notice_type, expected_usd", [
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
def test_mute_layer(
    notice_type, expected_usd, stage_with_layers
):
    """Mute one layer and listen to notices.
    """
    stage = stage_with_layers

    with listen_and_validate_notices(
        stage, notice_type, expected_usd
    ):
        layers = stage.GetRootLayer().subLayerPaths
        stage.MuteLayer(layers[0])

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
def test_mute_layers(
    notice_type, expected_usd, stage_with_layers
):
    """Mute several layers and listen to notices.
    """
    stage = stage_with_layers

    with listen_and_validate_notices(
        stage, notice_type, expected_usd
    ):
        layers = stage.GetRootLayer().subLayerPaths

        # Keep ref pointer to the layer we try to mute and unmute to
        # prevent it for being destroyed when it is muted.
        _layer = Sdf.Layer.FindOrOpen(layers[1])

        stage.MuteLayer(layers[0])
        stage.MuteLayer(layers[1])
        stage.UnmuteLayer(layers[1])
        stage.MuteAndUnmuteLayers([layers[2], layers[1]], [])

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
def test_change_edit_target(
    notice_type, expected_usd, stage_with_layers
):
    """Mute several layers and listen to notices.
    """
    stage = stage_with_layers

    with listen_and_validate_notices(
        stage, notice_type, expected_usd
    ):
        layers = stage.GetRootLayer().subLayerPaths
        layer1 = Sdf.Layer.Find(layers[0])
        layer2 = Sdf.Layer.Find(layers[1])

        stage.SetEditTarget(Usd.EditTarget(layer1))
        stage.SetEditTarget(Usd.EditTarget(layer2))
