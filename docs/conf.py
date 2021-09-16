# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import os
import sys
sys.path.insert(0, os.path.abspath(".."))

import apbs  # noqa: E402
from datetime import date

# Start TODO: This is part of issue https://github.com/Electrostatics/apbs/issues/41
# Based on https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/

# import subprocess, os

# # Check if we're running on Read the Docs' servers
# read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'

# breathe_default_project = "src"
# breathe_projects = {}

# if read_the_docs_build:
#     subprocess.call('doxygen', shell=True)
#     breathe_projects['src'] = output_dir + '/xml'
# End TODO:

# -- Project information -----------------------------------------------------

project = "APBS"
copyright = f"{date.today().year}, Nathan Baker and many others"

author = "Nathan Baker and many others"

# The full version, including alpha/beta/rc tags
release = apbs._version.__version__


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'sphinx.ext.autodoc', 'sphinx.ext.intersphinx', 'sphinx.ext.viewcode',
    'sphinx.ext.napoleon', 'sphinx.ext.todo', 'sphinx.ext.autosummary',
    'sphinx_rtd_theme', 'sphinx.ext.mathjax', 'sphinx_sitemap'
]

mathjax_path = 'https://cdnjs.cloudflare.com'
mathjax_path += '/ajax/libs/mathjax/2.7.0/MathJax.js'
mathjax_path += '?config=TeX-AMS-MML_HTMLorMML'
site_url = "https://apbs.readthedocs.io"

autosummary_generate = True
autosummary_imported_members = False
autosummary_generate_overwrite = True

master_doc = 'index'

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

# Enable intersphinx mapping
intersphinx_mapping = {
    'python': ('https://docs.python.org/3', None),
}
