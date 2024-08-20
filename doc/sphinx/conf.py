"""Configuration file for the Sphinx documentation builder."""

import re
import pathlib
import os
import sys

root = pathlib.Path(__file__).parent.resolve()
sys.path.insert(0, str(root / "_extensions"))

extensions = ["changelog", "sphinxcontrib.doxylink"]

# This deployment mode exists so that documentation can be built within a
# Github runner without requiring USD/TBB/Boost to be installed. We would be
# able to simplify the process if we could rely on pre-built dependencies which
# will be quick to install.
if os.environ.get("BUILD_DOCS_WITHOUT_CMAKE"):
    import doxygen

    doxygen.create_cmake_config()
    build_path = doxygen.build()
    source_path = root.parent.parent.resolve()

    path = source_path / "CMakeLists.txt"
    data = path.read_text()

    pattern = r"project\(.* VERSION ([\d\\.]+)"
    version = re.search(pattern, data, re.DOTALL).group(1)

    html_extra_path = ["./api"]

else:
    build_path = pathlib.Path("@CMAKE_CURRENT_BINARY_DIR@") / "doc"
    source_path = pathlib.Path("@PROJECT_SOURCE_DIR@")

    version = "@CMAKE_PROJECT_VERSION@"

source_suffix = ".rst"
master_doc = "index"

project = u"USD Notice Framework"
copyright = u"2023, Walt Disney Animation Studio"
release = version

doxylink = {
    "usd-cpp": (
        str(source_path / "doc" / "doxygen" / "USD.tag"),
        "https://graphics.pixar.com/usd/release/api"
    ),
    "unf-cpp": (
        str(build_path / "UNF.tag"),
        "./doxygen"
    )
}

html_theme = "sphinx_rtd_theme"
html_favicon = "favicon.png"
html_static_path = ["_static"]
html_css_files = ["style.css"]

