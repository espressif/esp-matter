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
"""Collect Python wheels from a build into a central directory."""

import argparse
import logging
from pathlib import Path
import shutil
import sys
from typing import Dict

_LOG = logging.getLogger(__name__)


def _parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        '--prefix',
        type=Path,
        help='Root search path to use in conjunction with --wheels_file',
    )
    parser.add_argument(
        '--suffix_file',
        type=argparse.FileType('r'),
        help=(
            'File that lists subdirs relative to --prefix, one per line,'
            'to search for .whl files to copy into --out_dir'
        ),
    )
    parser.add_argument(
        '--out_dir',
        type=Path,
        help='Path where all the built and collected .whl files should be put',
    )

    return parser.parse_args()


def copy_wheels(prefix, suffix_file, out_dir):
    if not out_dir.exists():
        out_dir.mkdir()

    copied_files: Dict[str, Path] = dict()
    for suffix in suffix_file.readlines():
        path = prefix / suffix.strip()
        _LOG.debug('Searching for wheels in %s', path)
        if path == out_dir:
            continue
        for wheel in path.glob('**/*.whl'):
            if wheel.name in copied_files:
                _LOG.error(
                    'Attempting to override %s with %s',
                    copied_files[wheel.name],
                    wheel,
                )
                raise FileExistsError(
                    f'{wheel.name} conflict: '
                    f'{copied_files[wheel.name]} and {wheel}'
                )
            copied_files[wheel.name] = wheel
            _LOG.debug('Copying %s to %s', wheel, out_dir)
            shutil.copy(wheel, out_dir)


def main():
    copy_wheels(**vars(_parse_args()))


if __name__ == '__main__':
    logging.basicConfig()
    main()
    sys.exit(0)
