# -*- coding: utf-8 -*-

from pxr import Usd, Tf, Sdf
import usd_notice_framework as unf


def test_layer_muting_changed():
    """Test whether LayerMutingChanged notice is as expected."""
    stage = Usd.Stage.CreateInMemory()
    unf.Broker.Create(stage)

    root_layer = stage.GetRootLayer()
    layer = Sdf.Layer.CreateAnonymous(".usda")
    root_layer.subLayerPaths.append(layer.identifier)

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        assert notice.IsMergeable() is True
        assert notice.GetTypeId() == "unf::UnfNotice::LayerMutingChanged"
        received.append(notice)

    key = Tf.Notice.Register(unf.Notice.LayerMutingChanged, _validate, stage)

    stage.MuteLayer(layer.identifier)

    # Ensure that one notice was received.
    assert len(received) == 1


def test_layer_muting_changed_get_muted_layers():
    """Ensure that expected muted layers are returned."""
    stage = Usd.Stage.CreateInMemory()
    unf.Broker.Create(stage)

    root_layer = stage.GetRootLayer()
    layer1 = Sdf.Layer.CreateAnonymous(".usda")
    layer2 = Sdf.Layer.CreateAnonymous(".usda")
    root_layer.subLayerPaths.append(layer1.identifier)
    root_layer.subLayerPaths.append(layer2.identifier)

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        assert notice.GetMutedLayers() == [layer1.identifier, layer2.identifier]
        received.append(notice)

    key = Tf.Notice.Register(unf.Notice.LayerMutingChanged, _validate, stage)

    stage.MuteAndUnmuteLayers([layer1.identifier, layer2.identifier], [])

    # Ensure that one notice was received.
    assert len(received) == 1


def test_layer_muting_changed_get_unmuted_layers():
    """Ensure that expected unmuted layers are returned."""
    stage = Usd.Stage.CreateInMemory()
    unf.Broker.Create(stage)

    root_layer = stage.GetRootLayer()
    layer1 = Sdf.Layer.CreateAnonymous(".usda")
    layer2 = Sdf.Layer.CreateAnonymous(".usda")
    root_layer.subLayerPaths.append(layer1.identifier)
    root_layer.subLayerPaths.append(layer2.identifier)

    stage.MuteAndUnmuteLayers([layer1.identifier, layer2.identifier], [])

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        assert notice.GetUnmutedLayers() == [layer1.identifier, layer2.identifier]
        received.append(notice)

    key = Tf.Notice.Register(unf.Notice.LayerMutingChanged, _validate, stage)

    stage.MuteAndUnmuteLayers([], [layer1.identifier, layer2.identifier])

    # Ensure that one notice was received.
    assert len(received) == 1

