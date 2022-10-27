******
Broker
******

.. py:class:: Broker

    Intermediate object between the Usd Stage and any clients that needs
    asynchronous handling and upstream filtering of notices.

    .. py:staticmethod:: Create(stage)

        Create a broker from a Usd Stage.

        If a broker has already been created from this *stage*, it will be
        returned. Otherwise, a new one will be created and returned.

        Example:

        .. code-block:: python

            stage = Usd.Stage.CreateInMemory()
            broker = Broker(stage)

        :param stage: Instance of Usd Stage.

        :return: Instance of :class:`Broker`.

    .. py:method:: GetStage()

        Return Usd Stage associated with the broker.

        :return: Instance of Usd Stage.

    .. py:method:: IsInTransaction()

        Indicate whether a notice transaction has been started.

        :return: Boolean value.

    .. py:method:: BeginTransaction(predicate=CapturePredicate.Default())

        Start a notice transaction.

        Notices derived from :class:`Notice.StageNotice` will be held during
        the transaction and emitted at the end.

        By default, all :class:`Notice.StageNotice` notices will be
        captured during the entire scope of the transaction. A function
        *predicate* or a :class:`CapturePredicate` instance can be passed to
        influence which notices are captured.

        Notices that are not captured will not be emitted.

        Example:

        .. code-block:: python

            # Block all notices emitted within the transaction.
            broker.BeginTransaction(predicate=CapturePredicate.BlockAll())

            # Block all notices from type 'Foo' emitted during transaction.
            broker.BeginTransaction(predicate=lambda n: not isinstance(n, Foo))

        .. warning::

            Each transaction started must be closed with :meth:`EndTransaction`.
            It is preferrable to use :class:`NoticeTransaction` over this API
            to safely manage transactions.

        :param predicate: Instance of :class:`CapturePredicate` or function
            taking a :class:`Notice.StageNotice` instance and returning a
            boolean value. By default, the :meth:`CapturePredicate.Default`
            predicate is used.

    .. py:method:: EndTransaction()

        Stop a notice transaction.

        This will trigger the emission of all captured
        :class:`Notice.StageNotice` notices. Each notice type will be
        consolidated before emission if applicable.

        .. warning::

            It is preferrable to use :class:`NoticeTransaction` over this API
            to safely manage transactions.
