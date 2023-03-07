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
"""Wrapper for the CLI commands for Python .whl building."""

import argparse
import logging
import os
import subprocess
import sys

_LOG = logging.getLogger(__name__)


def _parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        'setup_files',
        nargs='+',
        help='Path to a setup.py file to invoke to build wheels.',
    )
    parser.add_argument(
        '--out_dir', help='Path where the build artifacts should be put.'
    )

    return parser.parse_args()


def build_wheels(setup_files, out_dir):
    """Build Python wheels by calling 'python setup.py bdist_wheel'."""
    dist_dir = os.path.abspath(out_dir)

    for filename in setup_files:
        if not (filename.endswith('setup.py') and os.path.isfile(filename)):
            raise RuntimeError(f'Unable to find setup.py file at {filename}.')

        working_dir = os.path.dirname(filename)

        cmd = [
            sys.executable,
            'setup.py',
            'bdist_wheel',
            '--dist-dir',
            dist_dir,
        ]
        _LOG.debug('Running command:\n  %s', ' '.join(cmd))
        subprocess.check_call(cmd, cwd=working_dir)


def main():
    build_wheels(**vars(_parse_args()))


if __name__ == '__main__':
    logging.basicConfig()
    main()
    sys.exit(0)
