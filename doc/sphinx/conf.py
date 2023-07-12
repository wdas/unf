# -*- coding: utf-8 -*-

"""Configuration file for the Sphinx documentation builder."""

import os
import re

extensions = ["sphinxcontrib.doxylink", "lowdown"]

# This deployment mode exists so that documentation can be built within a
# Github runner without requiring USD/TBB/Boost to be installed. We would be
# able to simplify the process if we could rely on pre-built dependencies which
# will be quick to install.
if os.environ.get("BUILD_DOCS_WITHOUT_CMAKE"):
    import doxygen

    doxygen.create_cmake_config()
    build_path = doxygen.build()
    source_path = os.path.join(os.path.dirname(__file__), "..", "..")

    with open(os.path.join(source_path, "CMakeLists.txt")) as stream:
        pattern = r"project\(.* VERSION ([\d\\.]+)"
        version = re.search(pattern, stream.read(), re.DOTALL).group(1)

    html_extra_path = ["./api"]

else:
    build_path = "@CMAKE_CURRENT_BINARY_DIR@/doc"
    source_path = "@PROJECT_SOURCE_DIR@"
    version = "@CMAKE_PROJECT_VERSION@"

source_suffix = ".rst"
master_doc = "index"

project = u"USD Notice Framework"
copyright = u"2023, Walt Disney Animation Studio"
release = version

doxylink = {
    "usd-cpp": (
        os.path.join(source_path, "doc", "doxygen", "USD.tag"),
        "https://graphics.pixar.com/usd/release/api"
    ),
    "unf-cpp": (
        os.path.join(build_path, "UNF.tag"),
        "./doxygen"
    )
}

html_theme = "sphinx_rtd_theme"
html_favicon = "favicon.png"

