# USD Notice Framework

[![CMake](https://img.shields.io/badge/CMake-3.15...3.26-blue.svg?logo=CMake&logoColor=blue)](https://cmake.org)
[![Documentation](https://readthedocs.org/projects/usd-notice-framework/badge/?version=stable)](https://usd-notice-framework.readthedocs.io/en/stable/)
[![Tests](https://github.com/wdas/unf/actions/workflows/test.yml/badge.svg?branch=main)](https://github.com/wdas/unf/actions/workflows/test.yml)
[![License](https://img.shields.io/badge/License-Modified%20Apache%202.0-yellow.svg)](https://github.com/wdas/unf/blob/main/LICENSE.txt)

The USD Notice Framework (UNF) is built over [USD](https://github.com/PixarAnimationStudios/USD)'s
[Tf Notification System](https://graphics.pixar.com/usd/release/api/page_tf__notification.html).
It provides a C++ and Python API to efficiently manage the flow of
notifications emitted when authoring the USD stage.

While USD notices are delivered synchronously and tightly coupled with
the sender, UNF introduces standalone notices that can be used
for deferred delivery and can be aggregated per notice type, when applicable.

## What does this solve?

Pixar designed [USD](https://github.com/PixarAnimationStudios/USD) as an open
and extensible framework for composable data interchange across different tools.
As such, it is highly optimized for that purpose. Born out of Pixar's
[Presto Animation](https://en.wikipedia.org/wiki/Presto_(animation_software))
package, some application-level features were intentionally omitted to maintain
speed, scalability, and robustness to support its core usage.

Given that, there are challenges when using USD "out of the box" when building
interactive applications directly on top of USD as a data model. UNF is designed
as a framework for use in higher-level APIs and application logic to help
mitigate those issues.

One of the challenges a developer might face when developing an application on
top of USD data is the interactive editing performance when using USD as a data
model. When editing USD data, the stage andor layers produce a high volume of
change notifications that can be hard to manage when crafting a performant user
experience. This high volume of notices can cause frequent and costly cache
invalidation overhead, leading to sluggish performance and overly complicated
code.

UNF provides a framework to aggregate and even simplify change notifications
across a series of edits on a USD stage. In doing so, it introduces *some*
overhead on top of USD but allows developers to write more performant and
sustainable tools to observe and author the USD stage directly.

Note that UNF does not affect the *internal* performance of USD and therefore,
will not affect composition performance or the results they generate.

## Documentation

Full documentation, including installation and setup guides, can be found at
https://usd-notice-framework.readthedocs.io/en/stable/
