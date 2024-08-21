.. _release/release_notes:

*************
Release Notes
*************

.. release:: 0.7.0
    :date: 2024-08-20

    .. change:: changed

        Updated to use the 'Tomorrow Open Source Technology' license, adopted
        by OpenUSD in the v24.08 release.

        .. seealso:: https://forum.aousd.org/t/upcoming-openusd-license-update/1561

    .. change:: changed

        Updated documentation building logic to discover the :term:`Sphinx`
        configuration for :term:`CMake` from the `Sphinx CMake
        <https://python-cmake.github.io/sphinx-cmake>`_ package for simplicity.

    .. change:: changed

        Updated logic to build the :ref:`release/release_notes` page.

    .. change:: changed

        Added compatibility with CMake 3.30.

.. release:: 0.6.4
    :date: 2024-08-08

    .. change:: changed

        Updated Github workflow configuration to test library against
        :term:`USD` v24.08.

    .. change:: fixed

        Fixed Windows CI test errors by reverting to 'windows-2019' image.

.. release:: 0.6.3
    :date: 2024-07-23

    .. change:: new

        Added build metadata for use at WDAS.

.. release:: 0.6.1
    :date: 2024-05-09

    .. change:: fixed

        Updated TBB CMake module finder to ensure proper resolution of linker
        scripts on older versions of GNU LD (prior to 2.35).

.. release:: 0.6.0
    :date: 2024-03-12

    .. change:: new

        Added support for Windows.

    .. change:: changed

        Updated C++ standard version to C++17 per VFX Reference Platform CY2023.

        .. seealso:: https://vfxplatform.com/

    .. change:: changed

        Updated Python testing logic to discover the :term:`Pytest`
        configuration for :term:`CMake` from the `Pytest CMake
        <https://python-cmake.github.io/pytest-cmake>`_ package for simplicity.

.. release:: 0.5.6
    :date: 2023-07-13

    .. change:: changed

        Updated Github workflow configuration to build USD using the Python
        script instead of :term:`CMake` to be consistent with Pixar's process.

    .. change:: changed

        Added Github workflow configuration to release documentation to
        Github Pages instead of ReadTheDocs.org.

.. release:: 0.5.5
    :date: 2023-06-19

    .. change:: fixed

        Removed unnecessary usage of the ``TF_REFPTR_CONST_VOLATILE_GET`` macro,
        which is no longer available in :term:`USD` v23.08.

.. release:: 0.5.4
    :date: 2023-05-23

    .. change:: fixed

        Updated :unf-cpp:`Broker::AddDispatcher` to explicitly define the error
        message raised when the template parameter provided is incorrect. It is
        necessary as the message option cannot be ommitted from a `static_assert
        <https://en.cppreference.com/w/cpp/language/static_assert>`_ declaration
        before C++17.

    .. change:: fixed

        Updated :term:`CMake` configuration to ensure the C++ API
        documentation generated via :term:`Doxygen` is created before the
        :term:`Sphinx` documentation. It was necessary as the
        `sphinxcontrib-doxylink
        <https://pypi.org/project/sphinxcontrib-doxylink/>`_
        plugin depends on TAG files created by :term:`Doxygen`.

    .. change:: changed

        Re-enabled the documentation generation by default.

        .. seealso:: :ref:`installing/documentation`.

.. release:: 0.5.3
    :date: 2023-05-19

    .. change:: changed

        Disabled the documentation generation by default.

        .. seealso:: :ref:`installing/documentation`.

.. release:: 0.5.2
    :date: 2023-05-15

    .. change:: changed

        The project's license has been updated to `Apache 2.0 Modified
        <https://github.com/wdas/unf/blob/main/LICENSE.txt>`_.

.. release:: 0.5.1
    :date: 2023-05-12

    .. change:: fixed

        Fixed :term:`Sphinx` configuration to generate links to the C++ symbols
        documentation using the `sphinxcontrib-doxylink
        <https://pypi.org/project/sphinxcontrib-doxylink/>`_ plugin.

.. release:: 0.5.0
    :date: 2023-05-10

    .. change:: changed

        Updated Github workflow configuration to test library against
        :term:`USD` v23.05.

    .. change:: changed

        Updated :ref:`documentation <main>`.

.. release:: 0.4.5
    :date: 2023-03-15

    .. change:: fixed

        Fixed merging logic for :unf-cpp:`UnfNotice::ObjectsChanged` to prevent
        recording prim paths in the "changeInfo" list when they are already
        recorded in the "resyncPaths" list.

.. release:: 0.4.4
    :date: 2023-03-07

    .. change:: fixed

        Fixed merging logic for :unf-cpp:`UnfNotice::ObjectsChanged` to prevent
        recording empty changed fields.

.. release:: 0.4.3
    :date: 2023-02-14

    .. change:: fixed

        Fixed issue with the :term:`CMake` module for running :term:`Pytest`.

.. release:: 0.4.2
    :date: 2023-01-18

    .. change:: fixed

        Updated logic to hash :usd-cpp:`UsdStageWeakPtr` object.

.. release:: 0.4.1
    :date: 2023-01-17

    .. change:: fixed

        Ensure that :ref:`notices <notices>` implementing consolidation
        logic also expose :meth:`Merge` methods from
        :unf-cpp:`UnfNotice::StageNoticeImpl` to prevent errors when the
        ``overloaded-virtual`` flag is used.

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
