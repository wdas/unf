**********************
unf.Notice.StageNotice
**********************

.. py:class:: unf.Notice.StageNotice

    Interface class for standalone Usd Stage notices.

    This notice type is the standalone equivalent of the
    :usd-cpp:`UsdNotice::StageNotice` notice type.

    .. py:method:: IsMergeable()

        Indicate whether notice from the same type can be consolidated
        during a transaction.

        :return: Boolean value.

    .. py:method:: GetTypeId()

        Return unique type identifier.

        :return: String value.
