****************
CapturePredicate
****************

.. py:class:: CapturePredicate

    Predicate functor which indicates whether a notice can be captured
    during a transaction.

    .. py:staticmethod:: Default()

        Create a predicate which return true for each notice type.

        :return: Instance of :class:`CapturePredicate`.

    .. py:staticmethod:: BlockAll()

        Create a predicate which return false for each notice type.

        :return: Instance of :class:`CapturePredicate`.
