# :coding: utf-8

import contextlib

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

@contextlib.contextmanager
def listen_and_validate_notices(
    stage, notice_type, expected_usd, expected_broker
):
    """Edit stage and validate that USD and Broker notices are received."""
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

    broker.BeginTransaction()

    assert broker.IsInTransaction() is True

    # Edit the stage...
    yield

    # Ensure that broker notices are not sent during a transaction.
    assert len(received_broker) == 0

    # While USD Notices are being sent as expected.
    assert len(received_usd) == expected_usd

    broker.EndTransaction()

    assert broker.IsInTransaction() is False

    # Ensure that consolidated broker notices are sent after a transaction.
    assert len(received_broker) == expected_broker

@contextlib.contextmanager
def listen_and_call(stage, notice_type, callback):
    """Add one prim to the stage and analyze ObjectsChanged notice.
    """
    broker = NoticeBroker.Create(stage)

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        callback(notice)
        received.append(notice)

    key = Tf.Notice.Register(notice_type, _validate, stage)

    broker.BeginTransaction()

    # Edit the stage...
    yield

    broker.EndTransaction()

    # Ensure that one notice was received.
    assert len(received) == 1

@pytest.mark.parametrize("notice_type, expected_usd, expected_broker", [
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
def test_add_prim(notice_type, expected_usd, expected_broker):
    """Add one prim to the stage and listen to notices.
    """
    stage = Usd.Stage.CreateInMemory()

    with listen_and_validate_notices(
        stage, notice_type, expected_usd, expected_broker
    ):
        stage.DefinePrim("/Foo")

def test_add_prim_objectschanged():
    """Add one prim to the stage and analyze ObjectsChanged notice.
    """
    stage = Usd.Stage.CreateInMemory()

    def _validate(notice):
        """Validate notice received."""
        assert len(notice.GetResyncedPaths()) == 1
        assert len(notice.GetChangedInfoOnlyPaths()) == 0
        assert notice.GetResyncedPaths()[0] == "/Foo"

    with listen_and_call(
        stage, BrokerNotice.ObjectsChanged, _validate
    ):
        stage.DefinePrim("/Foo")

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
def test_add_prims(notice_type, expected_usd, expected_broker):
    """Add several prims to the stage and listen to notices.
    """
    stage = Usd.Stage.CreateInMemory()

    with listen_and_validate_notices(
        stage, notice_type, expected_usd, expected_broker
    ):
        stage.DefinePrim("/Foo")
        stage.DefinePrim("/Bar")
        stage.DefinePrim("/Baz")

def test_add_prims_objectschanged():
    """Add several prims to the stage and analyze ObjectsChanged notice.
    """
    stage = Usd.Stage.CreateInMemory()

    def _validate(notice):
        """Validate notice received."""
        assert len(notice.GetResyncedPaths()) == 3
        assert len(notice.GetChangedInfoOnlyPaths()) == 0
        assert notice.GetResyncedPaths()[0] == "/Foo"
        assert notice.GetResyncedPaths()[1] == "/Bar"
        assert notice.GetResyncedPaths()[2] == "/Baz"

    with listen_and_call(
        stage, BrokerNotice.ObjectsChanged, _validate
    ):
        stage.DefinePrim("/Foo")
        stage.DefinePrim("/Bar")
        stage.DefinePrim("/Baz")

@pytest.mark.parametrize("notice_type, expected_usd, expected_broker", [
    ("StageNotice", 3, 3),
    ("StageContentsChanged", 1, 1),
    ("ObjectsChanged", 1, 1),
    ("StageEditTargetChanged", 0, 0),
    ("LayerMutingChanged", 1, 1),
], ids=[
    "StageNotice",
    "StageContentsChanged",
    "ObjectsChanged",
    "StageEditTargetChanged",
    "LayerMutingChanged",
])
def test_mute_layer(
    notice_type, expected_usd, expected_broker, stage_with_layers
):
    """Mute one layer and listen to notices.
    """
    stage = stage_with_layers

    with listen_and_validate_notices(
        stage, notice_type, expected_usd, expected_broker
    ):
        layers = stage.GetRootLayer().subLayerPaths
        stage.MuteLayer(layers[0])

def test_mute_layer_objectschanged(stage_with_layers):
    """Mute one layer and analyze ObjectsChanged notice.
    """
    stage = stage_with_layers

    def _validate(notice):
        """Validate notice received."""
        assert len(notice.GetResyncedPaths()) == 1
        assert len(notice.GetChangedInfoOnlyPaths()) == 0
        assert notice.GetResyncedPaths()[0] == "/"

    # Create prim before caching to trigger resync path when muting.
    layers = stage.GetRootLayer().subLayerPaths
    layer = Sdf.Layer.Find(layers[0])
    stage.SetEditTarget(Usd.EditTarget(layer))
    stage.DefinePrim("/Foo")

    with listen_and_call(
        stage, BrokerNotice.ObjectsChanged, _validate
    ):
        layers = stage.GetRootLayer().subLayerPaths
        stage.MuteLayer(layers[0])

def test_mute_layer_layermutingchanged(stage_with_layers):
    """Mute one layer and analyze LayerMutingChanged notice.
    """
    stage = stage_with_layers
    layers = stage.GetRootLayer().subLayerPaths

    def _validate(notice):
        """Validate notice received."""
        assert len(notice.GetMutedLayers()) == 1
        assert len(notice.GetUnmutedLayers()) == 0
        assert notice.GetMutedLayers()[0] == layers[0]

    with listen_and_call(
        stage, BrokerNotice.LayerMutingChanged, _validate
    ):
        stage.MuteLayer(layers[0])

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
def test_mute_layers(
    notice_type, expected_usd, expected_broker, stage_with_layers
):
    """Mute several layers and listen to notices.
    """
    stage = stage_with_layers

    with listen_and_validate_notices(
        stage, notice_type, expected_usd, expected_broker
    ):
        layers = stage.GetRootLayer().subLayerPaths

        # Keep ref pointer to the layer we try to mute and unmute to
        # prevent it for being destroyed when it is muted.
        _layer = Sdf.Layer.FindOrOpen(layers[1])

        stage.MuteLayer(layers[0])
        stage.MuteLayer(layers[1])
        stage.UnmuteLayer(layers[1])
        stage.MuteAndUnmuteLayers([layers[2], layers[1]], [])

def test_mute_layers_objectschanged(stage_with_layers):
    """Mute several layers and analyze ObjectsChanged notice.
    """
    stage = stage_with_layers

    def _validate(notice):
        """Validate notice received."""
        assert len(notice.GetResyncedPaths()) == 1
        assert len(notice.GetChangedInfoOnlyPaths()) == 0
        assert notice.GetResyncedPaths()[0] == "/"

    # Create prim before caching to trigger resync path when muting.
    layers = stage.GetRootLayer().subLayerPaths
    layer = Sdf.Layer.Find(layers[0])
    stage.SetEditTarget(Usd.EditTarget(layer))
    stage.DefinePrim("/Foo")

    layer2 = Sdf.Layer.Find(layers[1])
    stage.SetEditTarget(Usd.EditTarget(layer2))
    stage.DefinePrim("/Bar")

    with listen_and_call(
        stage, BrokerNotice.ObjectsChanged, _validate
    ):
        stage.MuteLayer(layers[0])
        stage.MuteLayer(layers[1])
        stage.UnmuteLayer(layers[1])
        stage.MuteAndUnmuteLayers([layers[2], layers[1]], [])

def test_mute_layers_layermutingchanged(stage_with_layers):
    """Mute several layers and analyze LayerMutingChanged notice.
    """
    stage = stage_with_layers
    layers = stage.GetRootLayer().subLayerPaths

    def _validate(notice):
        """Validate notice received."""
        assert len(notice.GetMutedLayers()) == 3
        assert len(notice.GetUnmutedLayers()) == 0
        assert notice.GetMutedLayers()[0] == layers[0]
        assert notice.GetMutedLayers()[1] == layers[2]
        assert notice.GetMutedLayers()[2] == layers[1]

    with listen_and_call(
        stage, BrokerNotice.LayerMutingChanged, _validate
    ):
        layers = stage.GetRootLayer().subLayerPaths

        # Keep ref pointer to the layer we try to mute and unmute to
        # prevent it for being destroyed when it is muted.
        _layer = Sdf.Layer.FindOrOpen(layers[1])

        stage.MuteLayer(layers[0])
        stage.MuteLayer(layers[1])
        stage.UnmuteLayer(layers[1])
        stage.MuteAndUnmuteLayers([layers[2], layers[1]], [])

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
def test_change_edit_target(
    notice_type, expected_usd, expected_broker, stage_with_layers
):
    """Mute several layers and listen to notices.
    """
    stage = stage_with_layers

    with listen_and_validate_notices(
        stage, notice_type, expected_usd, expected_broker
    ):
        layers = stage.GetRootLayer().subLayerPaths
        layer1 = Sdf.Layer.Find(layers[0])
        layer2 = Sdf.Layer.Find(layers[1])

        stage.SetEditTarget(Usd.EditTarget(layer1))
        stage.SetEditTarget(Usd.EditTarget(layer2))
