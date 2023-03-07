# Copyright 2020 The Pigweed Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
"""Pigweed's Sphinx configuration."""

from datetime import date
import sphinx

# The suffix of source filenames.
source_suffix = ['.rst', '.md']

# The master toctree document.  # inclusive-language: ignore
master_doc = 'index'

# General information about the project.
project = 'Pigweed'
copyright = f'{date.today().year} The Pigweed Authors'  # pylint: disable=redefined-builtin

# The version info for the project you're documenting, acts as replacement for
# |version| and |release|, also used in various other places throughout the
# built documents.
#
# The short X.Y version.
version = '0.1'
# The full version, including alpha/beta/rc tags.
release = '0.1.0'

# The class of the Pygments (syntax highlighting) style to use.
pygments_style = 'pw_console.pigweed_code_style.PigweedCodeLightStyle'
pygments_dark_style = 'pw_console.pigweed_code_style.PigweedCodeStyle'

extensions = [
    'pw_docgen.sphinx.google_analytics',  # Enables optional Google Analytics
    'sphinx.ext.autodoc',  # Automatic documentation for Python code
    'sphinx.ext.napoleon',  # Parses Google-style docstrings
    'sphinxarg.ext',  # Automatic documentation of Python argparse
    'sphinxcontrib.mermaid',
    'sphinx_design',
    'myst_parser',
]

myst_enable_extensions = [
    # "amsmath",
    "colon_fence",
    # "deflist",
    "dollarmath",
    # "html_admonition",
    # "html_image",
    # "linkify",
    # "replacements",
    # "smartquotes",
    # "substitution",
    # "tasklist",
]

_DIAG_HTML_IMAGE_FORMAT = 'SVG'
blockdiag_html_image_format = _DIAG_HTML_IMAGE_FORMAT
nwdiag_html_image_format = _DIAG_HTML_IMAGE_FORMAT
seqdiag_html_image_format = _DIAG_HTML_IMAGE_FORMAT
actdiag_html_image_format = _DIAG_HTML_IMAGE_FORMAT
rackdiag_html_image_format = _DIAG_HTML_IMAGE_FORMAT
packetdiag_html_image_format = _DIAG_HTML_IMAGE_FORMAT

# Tell m2r to parse links to .md files and add them to the build.
m2r_parse_relative_links = True

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
html_theme = 'furo'

# The name for this set of Sphinx documents.  If None, it defaults to
# "<project> v<release> documentation".
html_title = 'Pigweed'

# If true, SmartyPants will be used to convert quotes and dashes to
# typographically correct entities.
html_use_smartypants = True

# If false, no module index is generated.
html_domain_indices = True

html_favicon = 'docs/_static/pw_logo.ico'
html_logo = 'docs/_static/pw_logo.svg'

# If false, no index is generated.
html_use_index = True

# If true, the index is split into individual pages for each letter.
html_split_index = False

# If true, links to the reST sources are added to the pages.
html_show_sourcelink = False

# If true, "Created using Sphinx" is shown in the HTML footer. Default is True.
html_show_sphinx = False

# These folders are copied to the documentation's HTML output
html_static_path = ['docs/_static']

# These paths are either relative to html_static_path
# or fully qualified paths (eg. https://...)
html_css_files = [
    'css/pigweed.css',
    # Needed for Inconsolata font.
    'https://fonts.googleapis.com/css2?family=Inconsolata&display=swap',
    # FontAwesome for mermaid and sphinx-design
    "https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.4/css/all.min.css",
]

html_theme_options = {
    'light_css_variables': {
        # Make the logo text more amaranth-like
        'color-sidebar-brand-text': '#b529aa',
        'color-sidebar-search-border': '#b529aa',
        'color-sidebar-link-text--top-level': '#85004d',
        'color-sidebar-link-text': '#016074',
        'color-sidebar-item-background--current': '#f0f0f0',
        'color-sidebar-item-background--hover': '#ffe2f3',
        'color-sidebar-item-expander-background--hover': '#ffe2f3',
        # Function signature colors
        'color-api-function-border': '#cccccc',
        'color-api-function-background': '#f0f0f0',
        'color-api-class-background': '#e7f2fa',
        'color-api-class-foreground': '#2980b9',
        'color-api-class-border': '#6ab0de',
        # Namespace::
        'color-api-pre-name': '#2980b9',
        # Function name
        'color-api-name': '#2980b9',
        'color-inline-code-background': '#fafafa',
        'color-inline-code-border': '#cccccc',
        'color-text-selection-background': '#1d5fad',
        'color-text-selection-foreground': '#ffffff',
        # Background color for focused headings.
        'color-highlight-on-target': '#ffffcc',
        # Background color emphasized code lines.
        'color-code-hll-background': '#ffffcc',
    },
    'dark_css_variables': {
        'color-sidebar-brand-text': '#fb71fe',
        'color-sidebar-search-border': '#e815a5',
        'color-sidebar-link-text--top-level': '#ff79c6',
        'color-sidebar-link-text': '#8be9fd',
        'color-sidebar-item-background--current': '#575757',
        'color-sidebar-item-background--hover': '#4c333f',
        'color-sidebar-item-expander-background--hover': '#4c333f',
        # Function signature colors
        'color-api-function-border': '#575757',
        'color-api-function-background': '#2b2b2b',
        'color-api-class-background': '#222c35',
        'color-api-class-foreground': '#87c1e5',
        'color-api-class-border': '#5288be',
        # Namespace::
        'color-api-pre-name': '#87c1e5',
        # Function name
        'color-api-name': '#87c1e5',
        'color-inline-code-background': '#2b2b2b',
        'color-inline-code-border': '#575757',
        'color-text-selection-background': '#2674bf',
        'color-text-selection-foreground': '#ffffff',
        # Background color for focused headings.
        'color-highlight-on-target': '#ffc55140',
        # Background color emphasized code lines.
        'color-code-hll-background': '#ffc55140',
    },
}

# Output file base name for HTML help builder.
htmlhelp_basename = 'Pigweeddoc'

# One entry per manual page. List of tuples
# (source start file, name, description, authors, manual section).
man_pages = [('index', 'pigweed', 'Pigweed', ['Google'], 1)]

# Grouping the document tree into Texinfo files. List of tuples
# (source start file, target name, title, author,
#  dir menu entry, description, category)
texinfo_documents = [
    (
        'index',
        'Pigweed',
        'Pigweed',
        'Google',
        'Pigweed',
        'Firmware framework',
        'Miscellaneous',
    ),
]


def do_not_skip_init(app, what, name, obj, would_skip, options):
    if name == "__init__":
        return False  # never skip __init__ functions
    return would_skip


# Problem: CSS files aren't copied after modifying them. Solution:
# https://github.com/sphinx-doc/sphinx/issues/2090#issuecomment-572902572
def env_get_outdated(app, env, added, changed, removed):
    return ['index']


def setup(app):
    app.add_css_file('css/pigweed.css')
    app.connect('env-get-outdated', env_get_outdated)
    app.connect("autodoc-skip-member", do_not_skip_init)
