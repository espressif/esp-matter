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
"""The Pigweed command line interface."""

import logging
import os
import sys
from typing import NoReturn

import pw_cli.log
from pw_cli import arguments, plugins, pw_command_plugins

_LOG = logging.getLogger(__name__)


def main() -> NoReturn:
    """Entry point for the pw command."""

    args = arguments.parse_args()

    pw_cli.log.install(level=args.loglevel, debug_log=args.debug_log)

    # Start with the most critical part of the Pigweed command line tool.
    if not args.no_banner:
        arguments.print_banner()

    _LOG.debug('Executing the pw command from %s', args.directory)
    os.chdir(args.directory)

    pw_command_plugins.register(args.directory)

    if args.help or args.command is None:
        print(pw_command_plugins.format_help(), file=sys.stderr)
        sys.exit(0)

    try:
        sys.exit(pw_command_plugins.run(args.command, args.plugin_args))
    except (plugins.Error, KeyError) as err:
        _LOG.critical('Cannot run command %s.', args.command)
        _LOG.critical('%s', err)
        sys.exit(2)


if __name__ == '__main__':
    main()
