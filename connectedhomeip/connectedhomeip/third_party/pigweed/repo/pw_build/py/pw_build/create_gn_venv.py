# Copyright 2022 The Pigweed Authors
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
"""Crate a venv."""

import argparse
import venv
from pathlib import Path


def _parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        '--destination-dir',
        type=Path,
        required=True,
        help='Path to venv directory.',
    )
    return parser.parse_args()


def main(destination_dir: Path) -> None:
    if not destination_dir.is_dir():
        venv.create(destination_dir, symlinks=True, with_pip=True)


if __name__ == '__main__':
    main(**vars(_parse_args()))
