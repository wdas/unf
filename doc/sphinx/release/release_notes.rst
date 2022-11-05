.. _release/release_notes:

*************
Release Notes
*************

.. release:: 0.4.0
    :date: 2022-11-04

    .. change:: changed

        Renamed python package to :mod:`unf` for consistency.

.. release:: 0.3.0
    :date: 2022-11-04

    .. change:: changed

        Updated :term:`CMake` configuration to update C++ standard via the
        :envvar:`CXXFLAGS_STD` environment variable.

    .. change:: changed

        Set `CMAKE_EXPORT_COMPILE_COMMANDS
        <https://cmake.org/cmake/help/latest/variable/CMAKE_EXPORT_COMPILE_COMMANDS.html>`_
        to generates a :file:`compile_commands.json` file for use by editors and
        other developer tools.

.. release:: 0.2.0
    :date: 2022-11-04

    .. change:: changed

        Updated :term:`CMake` configuration to export target with a ``unf::``
        namespace for safety.

    .. change:: changed

        Updated :ref:`building <installing/building>` documentation.

    .. change:: changed

        Updated exported :term:`CMake` configuration file names to use
        kebab-case instead of CamelCase.

    .. change:: changed

        Updated :term:`CMake` configuration to ensure that the
        ``CMAKE_BUILD_TYPE`` environment variable can be used to specify the
        build type when :term:`CMake` is used under version 3.22 which
        provides this feature.

        .. seealso::

            https://gitlab.kitware.com/cmake/cmake/-/merge_requests/6291

    .. change:: fixed

        Updated :term:`CMake` configuration to use
        :func:`configure_package_config_file` which ensures a that the package
        file contains required data.

        .. seealso::

            `CMakePackageConfigHelpers
            <https://cmake.org/cmake/help/latest/module/CMakePackageConfigHelpers.html>`_

    .. change:: fixed

        Updated :term:`CMake` configuration to ensure that
        ``CMAKE_CXX_STANDARD`` is defaulted to C++ 14 but can be overwritten by
        user.

.. release:: 0.1.1
    :date: 2022-11-03

    .. change:: changed

        Updated :term:`CMake` configuration to install documentation using
        the ``CMAKE_INSTALL_DOCDIR`` variable defined by the
        `GNUInstallDirs module
        <https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html>`_.

.. release:: 0.1.0
    :date: 2022-11-02

    .. change:: new

        Initial release.
