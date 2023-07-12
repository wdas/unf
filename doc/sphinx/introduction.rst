.. _introduction:

************
Introduction
************

The USD Notice Framework (UNF) is built over the native
:term:`Tf Notification System` in :term:`USD`, an open source extensible
software platform for collaboratively constructing animated 3D scenes. It
provides a :ref:`C++ <api_reference/cpp>` and
:ref:`Python <api_reference/python>` API to efficiently manage the flow of
notifications emitted when authoring the :term:`USD` stage.

While :term:`USD` notices are delivered synchronously and tightly coupled with
the sender, UNF introduces :ref:`standalone notices <notices>` that can be used
for deferred delivery and can be aggregated per notice type, when applicable.

What does this solve?
=====================

Pixar designed :term:`USD` as an open and extensible framework for composable
data interchange across different tools. As such, it is highly optimized for
that purpose. Born out of Pixar's :term:`Presto Animation` package, some
application-level features were intentionally omitted to maintain speed,
scalability, and robustness to support its core usage.

When editing :term:`USD` data, the stage and layers produce a high volume of
change notifications that can be hard to manage when crafting a performant user
experience. UNF provides a framework to aggregate and even simplify change
notifications across a series of edits on a :term:`USD` stage. It allows
developers to build performant and sustainable interactive applications using
:term:`USD` as its native data model.

.. seealso:: :ref:`getting_started`
