*****************************
unf.Notice.LayerMutingChanged
*****************************

.. py:class:: unf.Notice.LayerMutingChanged

    Base: :py:class:`unf.Notice.StageNotice`

    Notice sent after a set of layers have been newly muted or unmuted.

    This notice type is the autonomous equivalent of the
    `Usd.Notice.LayerMutingChanged`_ notice type.

    .. py:method:: GetMutedLayers()

        Returns identifiers of the layers that were muted.

        :return: List of layer identifiers.

    .. py:method:: GetUnmutedLayers()

        Returns identifiers of the layers that were unmuted.

        :return: List of layer identifiers.
