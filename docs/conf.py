import sphinx_rtd_theme

# -- Project information -----------------------------------------------------

project = 'R-type'
copyright = '2023'
author = ''

# The full version, including alpha/beta/rc tags
release = '1.0.0'

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'sphinx.ext.autodoc',  # Automatically generate documentation from docstrings
    'sphinx.ext.intersphinx',  # Link to other projects' documentation
    'sphinx.ext.todo',  # Support for todo items
    'sphinx.ext.viewcode',  # Include the source code in the documentation
    'breathe',  # Integration with Doxygen for C++ documentation
]

# Breathe Configuration
breathe_default_project = "R-type"
breathe_projects = {"R-type": "./xml"}
breathe_domain_by_extension = {"h": "cpp"}

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []

# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
html_theme = 'sphinx_rtd_theme'
html_theme_path = [sphinx_rtd_theme.get_html_theme_path()]

# -- Extension configuration -------------------------------------------------
