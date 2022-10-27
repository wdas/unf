*****************
NoticeTransaction
*****************

.. py:class:: NoticeTransaction

    Context manager object which consolidates and filter notices derived from
    :class:`Notice.StageNotice` within a specific scope.

    A transaction can be started with a :class:`Broker` instance, or with
    a Usd Stage instance. If the later option is chosen, a :class:`Broker`
    instance will be automatically created if none is associated with the
    incoming stage.

    .. code-block:: python

        # Create a transaction from a broker.
        with NoticeTransaction(broker) as transaction:
            ...

        # Create a transaction from a stage.
        with NoticeTransaction(stage) as transaction:
            broker = transaction.GetBroker()


    By default, all :class:`Notice.StageNotice` notices will be
    captured during the entire scope of the transaction. A function
    *predicate* or a :class:`CapturePredicate` instance can be passed to
    influence which notices are captured.

    Notices that are not captured will not be emitted.

    .. code-block:: python

        # Block all notices emitted within the transaction.
        with NoticeTransaction(
            broker, predicate=CapturePredicate.BlockAll()
        ) as transaction:
            ...

        # Block all notices from type 'Foo' emitted during transaction.
        with NoticeTransaction(
            broker, predicate=lambda n: not isinstance(n, Foo)
        ) as transaction:
            ...

    .. py:method:: __init__(target, predicate=CapturePredicate.Default())

        :param target: Instance of :class:`Broker` or Usd Stage.

        :param predicate: Instance of :class:`CapturePredicate` or function
            taking a :class:`Notice.StageNotice` instance and returning a
            boolean value. By default, the :meth:`CapturePredicate.Default`
            predicate is used.

        :return: Instance of :class:`NoticeTransaction`.

    .. py:method:: GetBroker()

        Return associated :class:`Broker` instance.

        :return: Instance of :class:`Broker`.
