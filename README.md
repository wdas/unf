# USD Notice Framework

[![CMake](https://img.shields.io/badge/CMake-3.20...3.30-blue.svg?logo=CMake&logoColor=blue)](https://cmake.org)
[![test-linux](https://github.com/wdas/unf/actions/workflows/test-linux.yml/badge.svg?branch=main)](https://github.com/wdas/unf/actions/workflows/test-linux.yml)
[![test-windows](https://github.com/wdas/unf/actions/workflows/test-windows.yml/badge.svg?branch=main)](https://github.com/wdas/unf/actions/workflows/test-windows.yml)
[![License](https://img.shields.io/badge/License-TOST-yellow.svg)](https://github.com/wdas/unf/blob/main/LICENSE.txt)

The USD Notice Framework (UNF) is built over the native [Tf Notification System][1]
in [USD][2], an open source extensible software platform for collaboratively
constructing animated 3D scenes. It provides a C++ and Python API to efficiently
manage the flow of notifications emitted when authoring the USD stage.

While USD notices are delivered synchronously and tightly coupled with
the sender, UNF introduces standalone notices that can be used
for deferred delivery and can be aggregated per notice type, when applicable.

## What does this solve?

Pixar designed [USD][2] as an open and extensible framework for composable data
interchange across different tools. As such, it is highly optimized for that
purpose. Born out of Pixar's [Presto Animation][3] package, some
application-level features were intentionally omitted to maintain speed,
scalability, and robustness to support its core usage.

When editing USD data, the stage and layers produce a high volume of change
notifications that can be hard to manage when crafting a performant user
experience. UNF provides a framework to aggregate and even simplify change
notifications across a series of edits on a USD stage. It allows developers
to build performant and sustainable interactive applications using USD as its
native data model.

## Documentation

Full documentation, including installation and setup guides, can be found at
https://wdas.github.io/unf

[1]: https://graphics.pixar.com/usd/release/api/page_tf__notification.html
[2]: http://openusd.org
[3]: https://en.wikipedia.org/wiki/Presto_(animation_software)
