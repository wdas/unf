******************
Notice.StageNotice
******************

.. py:class:: Notice.StageNotice

    Interface class for autonomous Usd Stage notices.

    This notice type is the autonomous equivalent of the
    `Usd.Notice.StageNotice`_ notice type.

    .. py:method:: IsMergeable()

        Indicate whether notice from the same type can be consolidated
        during a transaction.

        :return: Boolean value.

    .. py:method:: GetTypeId()

        Return unique type identifier.

        :return: String value.
