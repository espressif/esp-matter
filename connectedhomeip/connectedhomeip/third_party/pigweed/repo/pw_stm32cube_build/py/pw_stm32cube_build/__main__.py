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
"""Command line interface for stm32cube_builder."""

import argparse
import pathlib
import sys

try:
    from pw_stm32cube_build import (
        find_files,
        gen_file_list,
        icf_to_ld,
        inject_init,
    )
except ImportError:
    # Load from this directory if pw_stm32cube_build is not available.
    import find_files  # type: ignore
    import gen_file_list  # type: ignore
    import icf_to_ld  # type: ignore
    import inject_init  # type: ignore


def _parse_args() -> argparse.Namespace:
    """Setup argparse and parse command line args."""
    parser = argparse.ArgumentParser()

    subparsers = parser.add_subparsers(
        dest='command', metavar='<command>', required=True
    )

    gen_file_list_parser = subparsers.add_parser(
        'gen_file_list', help='generate files.txt for stm32cube directory'
    )
    gen_file_list_parser.add_argument('stm32cube_dir', type=pathlib.Path)

    find_files_parser = subparsers.add_parser(
        'find_files', help='find files in stm32cube directory'
    )
    find_files_parser.add_argument('stm32cube_dir', type=pathlib.Path)
    find_files_parser.add_argument('product_str')
    find_files_parser.add_argument('--init', default=False, action='store_true')

    icf_to_ld_parser = subparsers.add_parser(
        'icf_to_ld', help='convert stm32cube .icf linker files to .ld'
    )
    icf_to_ld_parser.add_argument('icf_path', type=pathlib.Path)
    icf_to_ld_parser.add_argument(
        '--ld-path', nargs=1, default=None, type=pathlib.Path
    )

    inject_init_parser = subparsers.add_parser(
        'inject_init', help='inject `pw_stm32cube_Init()` into startup_*.s'
    )
    inject_init_parser.add_argument('in_startup_path', type=pathlib.Path)
    inject_init_parser.add_argument(
        '--out-startup-path', nargs=1, default=None, type=pathlib.Path
    )

    return parser.parse_args()


def main():
    """Main command line function."""
    args = _parse_args()

    if args.command == 'gen_file_list':
        gen_file_list.gen_file_list(args.stm32cube_dir)
    elif args.command == 'find_files':
        find_files.find_files(args.stm32cube_dir, args.product_str, args.init)
    elif args.command == 'icf_to_ld':
        icf_to_ld.icf_to_ld(
            args.icf_path, args.ld_path[0] if args.ld_path else None
        )
    elif args.command == 'inject_init':
        inject_init.inject_init(
            args.in_startup_path,
            args.out_startup_path[0] if args.out_startup_path else None,
        )

    sys.exit(0)


if __name__ == '__main__':
    main()
