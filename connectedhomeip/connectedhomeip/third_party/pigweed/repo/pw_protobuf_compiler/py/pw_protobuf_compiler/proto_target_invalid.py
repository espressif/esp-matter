# Copyright 2019 The Pigweed Authors
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
"""Prints an error for an incorrect usage of a pw_proto_library template."""

import argparse
import logging
import sys

from typing import Optional

_LOG = logging.getLogger(__name__)


def argument_parser(
    parser: Optional[argparse.ArgumentParser] = None,
) -> argparse.ArgumentParser:
    """Registers the script's arguments on an argument parser."""

    if parser is None:
        parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument('--dir', required=True, help='Target directory')
    parser.add_argument('--root', required=True, help='GN root')
    parser.add_argument('--target', required=True, help='Build target')
    parser.add_argument(
        'generators',
        metavar='GEN',
        nargs='+',
        help='Supported protobuf generators',
    )

    return parser


def main() -> int:
    """Prints an error message."""

    args = argument_parser().parse_args()
    relative_dir = args.dir[len(args.root) :].rstrip('/')

    _LOG.error('')
    _LOG.error('The target %s is not a compiled protobuf library.', args.target)
    _LOG.error('')
    _LOG.error('A different target is generated for each active generator.')
    _LOG.error('Depend on one of the following targets instead:')
    _LOG.error('')
    for gen in args.generators:
        _LOG.error('  //%s:%s.%s', relative_dir, args.target, gen)
    _LOG.error('')

    return 1


if __name__ == '__main__':
    try:
        # If pw_cli is available, use it to initialize logs.
        from pw_cli import log

        log.install(logging.INFO)
    except ImportError:
        # If pw_cli isn't available, display log messages like a simple print.
        logging.basicConfig(format='%(message)s', level=logging.INFO)

    sys.exit(main())
