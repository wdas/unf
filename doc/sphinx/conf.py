# -*- coding: utf-8 -*-

"""Configuration file for the Sphinx documentation builder."""

# -- General ------------------------------------------------------------------

# Extensions.
extensions = [
    "sphinx.ext.extlinks",
    "sphinx.ext.intersphinx",
]

# The suffix of src filenames.
source_suffix = ".rst"

# The master toctree document.
master_doc = "index"

# General information about the project.
project = u"Usd Notice Framework"
copyright = u"2022, Walt Disney Animation Studio"

# -- HTML output --------------------------------------------------------------

html_theme = "sphinx_rtd_theme"

# If True, copy src rst files to output for reference.
html_copy_source = True

# -- Intersphinx --------------------------------------------------------------

intersphinx_mapping = {
    "python": ("http://docs.python.org/", None),
    "usd": ("https://graphics.pixar.com/usd/release", None)
}
