# :coding: utf-8

from pxr import Usd, Sdf
from usd_notice_broker import NoticeCache, NoticeBroker, BrokerNotice

import pytest


@pytest.mark.parametrize("notice", [
    Usd.Notice.StageNotice,
    Usd.Notice.ObjectsChanged,
    Usd.Notice.StageContentsChanged,
    Usd.Notice.StageEditTargetChanged,
    Usd.Notice.LayerMutingChanged,
    BrokerNotice.StageNotice,
], ids=[
    "Usd.Notice.StageNotice",
    "Usd.Notice.ObjectsChanged",
    "Usd.Notice.StageContentsChanged",
    "Usd.Notice.StageEditTargetChanged",
    "Usd.Notice.LayerMutingChanged",
    "BrokerNotice.StageNotice",
])
def test_incorrect_notice(notice):
    """Start cache with incorrect notice."""
    with pytest.raises(RuntimeError) as error:
        NoticeCache(notice)

    assert (
        "Expecting a notice derived from UsdBrokerNotice::StageNotice."
    ) in str(error)


def test_simple_cache():
    """Start simple cache workflow."""
    stage = Usd.Stage.CreateInMemory()
    NoticeBroker.Create(stage)

    cache = NoticeCache(BrokerNotice.ObjectsChanged)
    assert cache.Size() == 0

    stage.DefinePrim("/Foo")
    assert cache.Size() == 1

    cache.Clear()
    assert cache.Size() == 0


def test_objectschanged_add_prim():
    """Add a prim to stage while caching ObjectsChanged notices.
    """
    stage = Usd.Stage.CreateInMemory()
    NoticeBroker.Create(stage)

    cache = NoticeCache(BrokerNotice.ObjectsChanged)

    # Edit the stage...
    stage.DefinePrim("/Foo")

    # Ensure that one notice has been cached.
    assert cache.Size() == 1

    # Ensure that notice data is as expected.
    assert len(cache.GetAll()[0].GetResyncedPaths()) == 1
    assert len(cache.GetAll()[0].GetChangedInfoOnlyPaths()) == 0
    assert cache.GetAll()[0].GetResyncedPaths()[0] == "/Foo"

    cache.MergeAll()

    # Ensure that we have still one notice after consolidation.
    assert cache.Size() == 1

    # Ensure that the content of the notice didn't change.
    assert len(cache.GetAll()[0].GetResyncedPaths()) == 1
    assert len(cache.GetAll()[0].GetChangedInfoOnlyPaths()) == 0
    assert cache.GetAll()[0].GetResyncedPaths()[0] == "/Foo"


def test_objectschanged_add_prims():
    """Add several prims to stage while caching ObjectsChanged notices.
    """
    stage = Usd.Stage.CreateInMemory()
    NoticeBroker.Create(stage)

    cache = NoticeCache(BrokerNotice.ObjectsChanged)

    # Edit the stage...
    stage.DefinePrim("/Foo")
    stage.DefinePrim("/Bar")
    stage.DefinePrim("/Baz")

    # Ensure that three notices have been cached.
    assert cache.Size() == 3

    # Ensure that notice data are all as expected.
    assert len(cache.GetAll()[0].GetResyncedPaths()) == 1
    assert len(cache.GetAll()[0].GetChangedInfoOnlyPaths()) == 0
    assert cache.GetAll()[0].GetResyncedPaths()[0] == "/Foo"

    assert len(cache.GetAll()[1].GetResyncedPaths()) == 1
    assert len(cache.GetAll()[1].GetChangedInfoOnlyPaths()) == 0
    assert cache.GetAll()[1].GetResyncedPaths()[0] == "/Bar"

    assert len(cache.GetAll()[2].GetResyncedPaths()) == 1
    assert len(cache.GetAll()[2].GetChangedInfoOnlyPaths()) == 0
    assert cache.GetAll()[2].GetResyncedPaths()[0] == "/Baz"

    cache.MergeAll()

    # Ensure that we have one merged notice after consolidation.
    assert cache.Size() == 1

    # Ensure that the content of the notice didn't change.
    assert len(cache.GetAll()[0].GetResyncedPaths()) == 3
    assert len(cache.GetAll()[0].GetChangedInfoOnlyPaths()) == 0
    assert cache.GetAll()[0].GetResyncedPaths()[0] == "/Foo"
    assert cache.GetAll()[0].GetResyncedPaths()[1] == "/Bar"
    assert cache.GetAll()[0].GetResyncedPaths()[2] == "/Baz"


def test_stagecontentschanged_add_prim():
    """Add a prim to stage while caching StageContentsChanged notices.
    """
    stage = Usd.Stage.CreateInMemory()
    NoticeBroker.Create(stage)

    cache = NoticeCache(BrokerNotice.StageContentsChanged)

    # Edit the stage...
    stage.DefinePrim("/Foo")

    # Ensure that one notice has been cached.
    assert cache.Size() == 1

    cache.MergeAll()

    # Ensure that we have still one notice after consolidation.
    assert cache.Size() == 1

def test_stagecontentschanged_add_prims():
    """Add several prims to stage while caching StageContentsChanged notices.
    """
    stage = Usd.Stage.CreateInMemory()
    NoticeBroker.Create(stage)

    cache = NoticeCache(BrokerNotice.StageContentsChanged)

    # Edit the stage...
    stage.DefinePrim("/Foo")
    stage.DefinePrim("/Bar")
    stage.DefinePrim("/Baz")

    # Ensure that three notices have been cached.
    assert cache.Size() == 3

    cache.MergeAll()

    # Ensure that we have one merged notice after consolidation.
    assert cache.Size() == 1

def test_objectschanged_mute_layer(stage_with_layers):
    """Mute one layer while caching ObjectsChanged notices.
    """
    stage = stage_with_layers
    NoticeBroker.Create(stage)

    # Create prim before caching to trigger resync path when muting.
    layers = stage.GetRootLayer().subLayerPaths
    layer = Sdf.Layer.Find(layers[0])
    stage.SetEditTarget(Usd.EditTarget(layer))
    stage.DefinePrim("/Foo")

    cache = NoticeCache(BrokerNotice.ObjectsChanged)

    # Edit the stage...
    stage.MuteLayer(layers[0])

    # Ensure that one notice has been cached.
    assert cache.Size() == 1

    # Ensure that notice data is as expected.
    assert len(cache.GetAll()[0].GetResyncedPaths()) == 1
    assert len(cache.GetAll()[0].GetChangedInfoOnlyPaths()) == 0
    assert cache.GetAll()[0].GetResyncedPaths()[0] == "/"

    cache.MergeAll()

    # Ensure that we have still one notice after consolidation.
    assert cache.Size() == 1

    # Ensure that the content of the notice didn't change.
    assert len(cache.GetAll()[0].GetResyncedPaths()) == 1
    assert len(cache.GetAll()[0].GetChangedInfoOnlyPaths()) == 0
    assert cache.GetAll()[0].GetResyncedPaths()[0] == "/"

def test_objectschanged_mute_layers(stage_with_layers):
    """Mute several layers while caching ObjectsChanged notices.
    """
    stage = stage_with_layers
    NoticeBroker.Create(stage)

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

def test_stagecontentschanged_mute_layer(stage_with_layers):
    """Mute one layer while caching StageContentsChanged notices.
    """
    stage = stage_with_layers
    NoticeBroker.Create(stage)

    cache = NoticeCache(BrokerNotice.StageContentsChanged)

    # Edit the stage...
    layers = stage.GetRootLayer().subLayerPaths
    stage.MuteLayer(layers[0])

    # Ensure that one notice has been cached.
    assert cache.Size() == 1

    cache.MergeAll()

    # Ensure that we have still one notice after consolidation.
    assert cache.Size() == 1

def test_stagecontentschanged_mute_layers(stage_with_layers):
    """Mute several layers while caching StageContentsChanged notices.
    """
    stage = stage_with_layers
    NoticeBroker.Create(stage)

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

def test_layermutingchanged_mute_layer(stage_with_layers):
    """Mute one layer while caching LayerMutingChanged notices.
    """
    stage = stage_with_layers
    NoticeBroker.Create(stage)

    cache = NoticeCache(BrokerNotice.LayerMutingChanged)

    # Edit the stage...
    layers = stage.GetRootLayer().subLayerPaths
    stage.MuteLayer(layers[0])

    # Ensure that one notice has been cached.
    assert cache.Size() == 1

    # Ensure that notice data is as expected.
    assert len(cache.GetAll()[0].GetMutedLayers()) == 1
    assert len(cache.GetAll()[0].GetUnmutedLayers()) == 0
    assert cache.GetAll()[0].GetMutedLayers()[0] == layers[0]

    cache.MergeAll()

    # Ensure that we have still one notice after consolidation.
    assert cache.Size() == 1

    # Ensure that the content of the notice didn't change.
    assert len(cache.GetAll()[0].GetMutedLayers()) == 1
    assert len(cache.GetAll()[0].GetUnmutedLayers()) == 0
    assert cache.GetAll()[0].GetMutedLayers()[0] == layers[0]

def test_layermutingchanged_mute_layers(stage_with_layers):
    """Mute several layers while caching LayerMutingChanged notices.
    """
    stage = stage_with_layers
    NoticeBroker.Create(stage)

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


def test_change_edit_target(stage_with_layers):
    """Change edit target while caching StageEditTargetChanged notices.
    """
    stage = stage_with_layers
    NoticeBroker.Create(stage)

    cache = NoticeCache(BrokerNotice.StageEditTargetChanged)

    # Edit the stage...
    layers = stage.GetRootLayer().subLayerPaths
    layer1 = Sdf.Layer.Find(layers[0])
    layer2 = Sdf.Layer.Find(layers[1])

    stage.SetEditTarget(Usd.EditTarget(layer1))
    stage.SetEditTarget(Usd.EditTarget(layer2))

    # Ensure that one notice has been cached.
    assert cache.Size() == 2

    cache.MergeAll()

    # Ensure that we have still one notice after consolidation.
    assert cache.Size() == 1
