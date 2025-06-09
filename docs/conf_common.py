from esp_docs.conf_docs import *  # noqa: F403,F401
import subprocess

languages = ['en']
idf_targets = ['esp32', 'esp32s3', 'esp32c2', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32h2', 'esp32p4']

extensions += ['sphinx_copybutton',
               # Needed as a trigger for running doxygen
               'esp_docs.esp_extensions.dummy_build_system',
               'esp_docs.esp_extensions.run_doxygen',
               ]

# link roles config
github_repo = 'espressif/esp-matter'

# Get current commit hash
try:
    current_commit = subprocess.check_output(['git', 'rev-parse', '--short','HEAD']).decode('utf-8').strip()
except:
    current_commit = 'main'  # Fallback if git command fails

# do not check for anchors
linkcheck_anchors = False
# Add linkcheck configurations
linkcheck_ignore = [
    # Private repositories
    'https://github.com/CHIP-Specifications/.*',

    # Internal documentation links
    './developing.html#.*',
    './production.html#.*',

    # Ignore self-referential GitHub links with commit hashes
    f'https://github.com/{github_repo}/blob/{current_commit}/.*',  # Current commit
]

# Add timeout and HTTP status codes to ignore
linkcheck_timeout = 30
linkcheck_retries = 3
linkcheck_ignore_codes = [
    403,    # Forbidden
    429,    # Too many requests
    500,    # Server errors
    502,    # Bad Gateway
    503,    # Service unavailable
    504,    # Gateway timeout
]

# context used by sphinx_idf_theme
html_context['github_user'] = 'espressif'
html_context['github_repo'] = 'esp-matter'

html_static_path = ['../_static']

# Extra options required by sphinx_idf_theme
project_slug = 'esp-matter'

# Contains info used for constructing target and version selector
# Can also be hosted externally, see esp-idf for example
versions_url = '_static/esp_sdk_matter_version.js'

# Final PDF filename will contains target and version
pdf_file_prefix = u'esp-docs'
