# Copyright 2021 The Pigweed Authors
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
"""Prints an error message and exits unsuccessfully."""

import argparse
import logging
import os
from pathlib import Path
import subprocess
import sys

try:
    from pw_cli.log import install as setup_logging
except ImportError:
    from logging import basicConfig as setup_logging  # type: ignore

_LOG = logging.getLogger(__name__)


def _parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        '--message', required=True, help='Error message to print'
    )
    parser.add_argument(
        '--target', required=True, help='GN target in which the error occurred'
    )
    parser.add_argument('--root', required=True, type=Path, help='GN root')
    parser.add_argument('--out', required=True, type=Path, help='GN out dir')
    return parser.parse_args()


def main(message: str, target: str, root: Path, out: Path) -> int:
    """Logs the error message and returns 1."""

    _LOG.error('')
    _LOG.error('Build error for %s:', target)
    _LOG.error('')

    for line in message.split('\\n'):
        _LOG.error('  %s', line)

    _LOG.error('')

    gn_cmd = subprocess.run(
        ['gn', 'path', f'--root={root}', out, '//:default', target],
        capture_output=True,
    )
    path_info = gn_cmd.stdout.decode(errors='replace').rstrip()

    relative_out = os.path.relpath(out, root)

    if gn_cmd.returncode == 0 and 'No non-data paths found' not in path_info:
        _LOG.error('Dependency path to this target:')
        _LOG.error('')
        _LOG.error(
            '  gn path %s //:default "%s"\n%s', relative_out, target, path_info
        )
        _LOG.error('')
    else:
        _LOG.error(
            'Run this command to see the build dependency path to this target:'
        )
        _LOG.error('')
        _LOG.error('  gn path %s <target> "%s"', relative_out, target)
        _LOG.error('')
        _LOG.error('where <target> is the GN target you are building.')
        _LOG.error('')

    return 1


if __name__ == '__main__':
    setup_logging()
    sys.exit(main(**vars(_parse_args())))
