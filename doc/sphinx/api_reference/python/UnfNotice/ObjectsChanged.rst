************************
UnfNotice.ObjectsChanged
************************

.. py:class:: UnfNotice.ObjectsChanged

    Base: :py:class:`UnfNotice.StageNotice`

    Notice sent in response to authored changes that affect any
    Usd Object.

    This notice type is the autonomous equivalent of the
    :class:`Usd.Notice.ObjectsChanged` notice type.

    .. py:method:: AffectedObject(object)

        Indicate whether object was affected by the change that generated
        this notice.

        :param object: Instance of Usd Object.

        :return: Boolean value.

    .. py:method:: ResyncedObject(object)

        Indicate whether object was resynced by the change that generated
        this notice.

        :param object: Instance of Usd Object.

        :return: Boolean value.

    .. py:method:: ChangedInfoOnly(object)

        Indicate whether object was modified but not resynced by the
        change that generated this notice.

        :param object: Instance of Usd Object.

        :return: Boolean value.

    .. py:method:: GetResyncedPaths()

        Return list of paths that are resynced in lexicographical order.

        :return: List of instances of Sdf Path.

    .. py:method:: GetChangedInfoOnlyPaths()

        Return list of paths that are modified but not resynced in
        lexicographical order.

        :return: List of instances of Sdf Path.

    .. py:method:: GetChangedFields(target)

        Return the set of changed fields in layers that affected the *target*.

        :param target: Instance of Usd Object or Sdf Path.

        :return: List of field name.

    .. py:method:: HasChangedFields(target)

        Indicate whether any changed fields affected the *target*.

        :param target: Instance of Usd Object or Sdf Path.

        :return: Boolean value.
