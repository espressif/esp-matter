#!/usr/bin/env python3
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
"""Command line interface for mcuxpresso_builder."""

import argparse
import pathlib
import sys

try:
    from pw_build_mcuxpresso import components
except ImportError:
    # Load from this directory if pw_build_mcuxpresso is not available.
    import components  # type: ignore


def _parse_args() -> argparse.Namespace:
    """Setup argparse and parse command line args."""
    parser = argparse.ArgumentParser()

    subparsers = parser.add_subparsers(
        dest='command', metavar='<command>', required=True
    )

    project_parser = subparsers.add_parser(
        'project', help='output components of an MCUXpresso project'
    )
    project_parser.add_argument('manifest_filename', type=pathlib.Path)
    project_parser.add_argument('--include', type=str, action='append')
    project_parser.add_argument('--exclude', type=str, action='append')
    project_parser.add_argument('--prefix', dest='path_prefix', type=str)

    return parser.parse_args()


def main():
    """Main command line function."""
    args = _parse_args()

    if args.command == 'project':
        components.project(
            args.manifest_filename,
            include=args.include,
            exclude=args.exclude,
            path_prefix=args.path_prefix,
        )

    sys.exit(0)


if __name__ == '__main__':
    main()
