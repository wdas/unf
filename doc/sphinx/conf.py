# -*- coding: utf-8 -*-

"""Configuration file for the Sphinx documentation builder."""

import os
import re

# -- General ------------------------------------------------------------------

extensions = ["lowdown"]

if os.environ.get("READTHEDOCS"):
    import doxygen

    doxygen.create_cmake_config()
    doxygen.build()

    html_extra_path = ["./api"]

# The suffix of src filenames.
source_suffix = ".rst"

# The master toctree document.
master_doc = "index"

# General information about the project.
project = u"Usd Notice Framework"
copyright = u"2022, Walt Disney Animation Studio"

# Version
with open(
    os.path.join(
        os.path.dirname(__file__), "..", "..", "CMakeLists.txt",
    )
) as _version_file:
    _version = re.search(
        r"project\(.* VERSION ([\d\\.]+)", _version_file.read(), re.DOTALL
    ).group(1)

version = _version
release = _version

# -- HTML output --------------------------------------------------------------

html_theme = "sphinx_rtd_theme"

# If True, copy src rst files to output for reference.
html_copy_source = True

# Ensure that common links are available.
with open("links.rst") as stream:
     rst_epilog = stream.read().replace(":orphan:", "")
