.. _introduction:

************
Introduction
************

The Usd Notice Framework (UNF) is built over :term:`USD`'s
:term:`Tf Notification System`. It provides a :ref:`C++ <api_reference/cpp>` and
:ref:`Python <api_reference/python>` API to efficiently manage the flow of
notifications emitted when authoring the :term:`USD` stage.

While :term:`USD` notices are delivered synchronously and tightly coupled with
the sender, UNF introduces :ref:`standalone notices <notices>` that can be used
for deferred delivery and can be aggregated per notice type, when applicable.

What does this solve?
=====================

Pixar designed :term:`USD` as an open and extensible framework for composable
data interchange across different tools.  As such, it is highly optimized for
that purpose. Born out of Pixar's :term:`Presto Animation` package, some
application-level features were intentionally omitted to maintain speed,
scalability, and robustness to support its core usage.

Given that, there are challenges when using :term:`USD` "out of the box" when
building interactive applications directly on top of :term:`USD` as a data
model. UNF is designed as a framework for use in higher-level APIs and
application logic to help mitigate those issues.

One of the challenges a developer might face when developing an application
on top of :term:`USD` data is the interactive editing performance when using
:term:`USD` as a data model. When editing :term:`USD` data, the stage andor
layers produce a high volume of change notifications that can be hard to manage
when crafting a performant user experience. This high volume of notices can
cause frequent and costly cache invalidation overhead, leading to sluggish
performance and overly complicated code.

UNF provides a framework to aggregate and even simplify change notifications
across a series of edits on a :term:`USD` stage. In doing so, it introduces
*some* overhead on top of :term:`USD` but allows developers to write more
performant and sustainable tools to observe and author the :term:`USD` stage
directly.

Note that UNF does not affect the *internal* performance of :term:`USD` and
therefore, will not affect composition performance or the results they
generate.

.. seealso:: :ref:`getting_started`
