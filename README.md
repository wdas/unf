# USD Notice Framework

[![Tests](https://github.com/wdas/usd-notice-framework/actions/workflows/test.yml/badge.svg?branch=main)](https://github.com/wdas/usd-notice-framework/actions/workflows/test.yml)

The Usd Notice Framework is built over
[Usd](https://github.com/PixarAnimationStudios/USD) notices and uses the
[Tf Notification System](https://graphics.pixar.com/usd/release/api/page_tf__notification.html).
It provides a C++ and Python API to efficiently manage the flow of notifications
emitted when authoring the [Usd](https://github.com/PixarAnimationStudios/USD)
stage.

It introduces the concept of autonomous notices and notice transaction which
defers notification and consolidate notices per type when applicable:

```python
from pxr import Usd, Tf
import usd_notice_framework as unf

stage = Usd.Stage.CreateInMemory()

def updated(notice, stage):
    """Print resynced paths from the stage."""
    print(notice.GetResyncedPaths())

key = Tf.Notice.Register(unf.Notice.ObjectsChanged, updated, stage)

with unf.NoticeTransaction(stage):
    prim = stage.DefinePrim("/Foo", "Cylinder")
    prim.GetAttribute("radius").Set(5)
    prim.GetAttribute("height").Set(10)
```

A predicate can be applied during a transaction to block some or all Unf notices
emitted in this scope:

```python
with unf.NoticeTransaction(
    stage, predicate=unf.CapturePredicate.BlockAll()
):
    prim = stage.DefinePrim("/Foo", "Cylinder")
    prim.GetAttribute("radius").Set(5)
    prim.GetAttribute("height").Set(10)
```

## Documentation

Full documentation, including installation and setup guides, can be found at
https://usd-notice-framework.readthedocs.io/en/stable/
