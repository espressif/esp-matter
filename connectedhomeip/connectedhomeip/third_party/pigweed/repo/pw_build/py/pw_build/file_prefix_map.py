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
"""Transforms a JSON list of paths using -ffile-prefix-map style rules."""

import argparse
import json
from typing import Iterator, List, TextIO

# Note: This should be List[Tuple[str, str]], but using string.split()
# produces Tuple[Any,...], so this permits that typing for convenience.
PrefixMaps = List[tuple]


def _parse_args() -> argparse.Namespace:
    """Parses and returns the command line arguments."""

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        'in_json',
        type=argparse.FileType('r'),
        help='The JSON file containing a list of file names '
        'that the prefix map operations should be applied to',
    )
    parser.add_argument(
        '--prefix-map-json',
        type=argparse.FileType('r'),
        required=True,
        help=(
            'JSON file containing an array of prefix map transformations to '
            'apply to the strings before tokenizing. These string literal '
            'transformations are of the form "from=to". All strings with the '
            'prefix `from` will have the prefix replaced with `to`. '
            'Transformations are applied in the order they are listed in the '
            'JSON file.'
        ),
    )

    parser.add_argument(
        '--output',
        type=argparse.FileType('w'),
        help='File path to write transformed paths to.',
    )
    return parser.parse_args()


def remap_paths(paths: List[str], prefix_maps: PrefixMaps) -> Iterator[str]:
    for path in paths:
        for from_prefix, to_prefix in prefix_maps:
            if path.startswith(from_prefix):
                path = path.replace(from_prefix, to_prefix, 1)
        yield path


def remap_json_paths(
    in_json: TextIO, output: TextIO, prefix_map_json: TextIO
) -> None:
    paths = json.load(in_json)
    prefix_maps: PrefixMaps = [
        tuple(m.split('=', maxsplit=1)) for m in json.load(prefix_map_json)
    ]

    json.dump(list(remap_paths(paths, prefix_maps)), output)


if __name__ == '__main__':
    remap_json_paths(**vars(_parse_args()))
