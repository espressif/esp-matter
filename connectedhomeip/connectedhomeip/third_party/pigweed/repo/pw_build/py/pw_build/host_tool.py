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
"""Copies built host tools into Pigweed's host_tools directory."""

import argparse
import logging
from pathlib import Path
import shutil
import sys
from typing import Optional

import pw_cli.log

_LOG = logging.getLogger(__name__)


def argument_parser(
    parser: Optional[argparse.ArgumentParser] = None,
) -> argparse.ArgumentParser:
    """Registers the script's arguments on an argument parser."""

    if parser is None:
        parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument(
        '--dst', type=Path, required=True, help='Path to host tools directory'
    )
    parser.add_argument('--name', help='Name for the installed tool')
    parser.add_argument(
        '--out-root',
        type=Path,
        required=True,
        help='Root of Ninja out directory',
    )
    parser.add_argument(
        '--src', type=Path, required=True, help='Path to host tool executable'
    )

    return parser


def main() -> int:
    """Copies a host tool into a destination directory."""
    args = argument_parser().parse_args()

    if not args.src.is_file():
        _LOG.error('%s is not a file', args.src)
        return 1

    args.dst.mkdir(parents=True, exist_ok=True)

    if args.name is not None:
        if '/' in args.name:
            _LOG.error('Host tool name cannot contain "/"')
            return 1
        name = args.name
    else:
        name = args.src.name

    try:
        shutil.copy2(args.src, args.dst.joinpath(name))
    except OSError as err:
        _LOG.error('%s', err)

        # Errno 26 (text file busy) indicates that a host tool binary is
        # currently running.
        # TODO(frolv): Check if this works on Windows.
        if err.errno == 26:
            _LOG.error('')
            _LOG.error('  %s has been rebuilt but cannot be', name)
            _LOG.error('  copied into the host tools directory:')
            _LOG.error('')
            _LOG.error(
                '    %s', args.dst.relative_to(args.out_root).joinpath(name)
            )
            _LOG.error('')
            _LOG.error('  This can occur if the program is already running.')
            _LOG.error(
                '  If it is running, exit it and try re-running the build.'
            )
            _LOG.error('')

        return 1

    return 0


if __name__ == '__main__':
    pw_cli.log.install()
    sys.exit(main())
