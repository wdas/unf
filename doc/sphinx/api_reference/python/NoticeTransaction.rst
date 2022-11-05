*********************
unf.NoticeTransaction
*********************

.. py:class:: unf.NoticeTransaction

    Context manager object which consolidates and filter notices derived from
    :class:`unf.Notice.StageNotice` within a specific scope.

    A transaction can be started with a :class:`~unf.Broker` instance, or with
    a Usd Stage instance. If the later option is chosen, a :class:`~unf.Broker`
    instance will be automatically created if none is associated with the
    incoming stage.

    .. code-block:: python

        # Create a transaction from a broker.
        with NoticeTransaction(broker) as transaction:
            ...

        # Create a transaction from a stage.
        with NoticeTransaction(stage) as transaction:
            broker = transaction.GetBroker()


    By default, all :class:`unf.Notice.StageNotice` notices will be
    captured during the entire scope of the transaction. A function
    *predicate* or a :class:`unf.CapturePredicate` instance can be passed to
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

        :param target: Instance of :class:`unf.Broker` or Usd Stage.

        :param predicate: Instance of :class:`unf.CapturePredicate` or function
            taking a :class:`unf.Notice.StageNotice` instance and returning a
            boolean value. By default, the :meth:`unf.CapturePredicate.Default`
            predicate is used.

        :return: Instance of :class:`unf.NoticeTransaction`.

    .. py:method:: GetBroker()

        Return associated :class:`unf.Broker` instance.

        :return: Instance of :class:`unf.Broker`.
