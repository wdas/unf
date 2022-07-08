# :coding: utf-8

from pxr import Usd, Sdf

import pytest

@pytest.fixture()
def stage_with_layers():
    """Create a Stage with initial layers."""
    stage = Usd.Stage.CreateInMemory()
    root_layer = stage.GetRootLayer()

    layers_nb = 3

    layers = []
    identifiers = []

    while len(layers) < layers_nb:
        layer = Sdf.Layer.CreateAnonymous(".usda")
        layers.append(layer)
        identifiers.append(layer.identifier)

    root_layer.subLayerPaths = identifiers
    return stage
