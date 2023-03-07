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
"""Generates nanopb_pb2.py by importing the Nanopb proto module.

The Nanopb repository generates nanopb_pb2.py dynamically when its Python
package is imported if it does not exist. If multiple processes try to use
Nanopb to compile simultaneously on a clean build, they can interfere with each
other. One process might rewrite nanopb_pb2.py as another process is trying to
access it, resulting in import errors.

This script imports the Nanopb module so that nanopb_pb2.py is generated if it
doesn't exist. All Nanopb proto compilation targets depend on this script so
that nanopb_pb2.py is guaranteed to exist before they need it.
"""

import argparse
import importlib.util
from pathlib import Path
import sys


def generate_nanopb_proto(root: Path) -> None:
    sys.path.append(str(root / 'generator'))

    spec = importlib.util.spec_from_file_location(
        'proto', root / 'generator' / 'proto' / '__init__.py'
    )
    assert spec is not None
    proto_module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(proto_module)  # type: ignore[union-attr]


def _parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('root', type=Path, help='Nanopb root')
    return parser.parse_args()


if __name__ == '__main__':
    generate_nanopb_proto(**vars(_parse_args()))
