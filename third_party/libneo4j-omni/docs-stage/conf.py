# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'libneo4j-client'
copyright = '2016-2023, Chris Leishman; portions 2023, Mark A. Jensen'
author = 'Chris Leishman'
release = '5.0.4'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration
source_suffix = {
  '.rst': 'restructuredtext',
  '.md': 'markdown',
  }
extensions = ['breathe', 'myst_parser']
templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']
breathe_projects = {"libneo4j-client":"../lib/doc/xml"}
breathe_default_project = 'libneo4j-client'


# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

#modindex_common_prefix = ['neo4j_']
cpp_index_common_prefix = ['neo4j_']
html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
html_js_files = ['https://code.jquery.com/jquery-3.6.4.min.js']
