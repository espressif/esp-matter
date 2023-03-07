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
"""Python wrapper that runs a program. For use in GN."""

import argparse
import logging
import os
import re
import shlex
import subprocess
import sys
import pathlib
from typing import Dict, Optional

# Need to be able to run without pw_cli installed in the virtualenv.
try:
    import pw_cli.log
except ImportError:
    pass

_LOG = logging.getLogger(__name__)


def argument_parser(
    parser: Optional[argparse.ArgumentParser] = None,
) -> argparse.ArgumentParser:
    """Registers the script's arguments on an argument parser."""

    if parser is None:
        parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument(
        '--args-file',
        type=argparse.FileType('r'),
        help='File containing extra positional arguments to the program',
    )
    parser.add_argument(
        '--capture-output',
        action='store_true',
        help='Hide output from the program unless it fails',
    )
    parser.add_argument(
        '-e',
        '--env',
        action='append',
        default=[],
        help='key=value environment pair for the process',
    )
    parser.add_argument(
        '--env-file',
        type=argparse.FileType('r'),
        help='File defining environment variables for the process',
    )
    parser.add_argument(
        '--skip-empty-args',
        action='store_true',
        help='Don\'t run the program if --args-file is empty',
    )
    parser.add_argument(
        '--target',
        help='GN build target that runs the program',
    )
    parser.add_argument(
        '--working-directory',
        type=pathlib.Path,
        help='Directory to execute program in',
    )
    parser.add_argument(
        'command',
        nargs=argparse.REMAINDER,
        help='Program to run with arguments',
    )

    return parser


_ENV_REGEX = re.compile(r'(\w+)(\+)?=(.+)')


def apply_env_var(string: str, env: Dict[str, str]) -> None:
    """Update an environment map with provided a key-value pair.

    Pairs are accepted in two forms:

      KEY=value    sets environment variable "KEY" to "value"
      KEY+=value   appends OS-specific PATH separator and "value" to
                   environment variable "KEY"
    """
    result = _ENV_REGEX.search(string.strip())
    if not result:
        return

    key, append, val = result.groups()
    if append is not None:
        curr = env.get(key)
        val = f'{curr}{os.path.pathsep}{val}' if curr else val

    env[key] = val


def main() -> int:
    """Runs a program specified by command-line arguments."""
    args = argument_parser().parse_args()
    if not args.command or args.command[0] != '--':
        return 1

    env = os.environ.copy()

    # Command starts after the "--".
    command = args.command[1:]
    # command[0] is the invoker.prog from gn and gn will escape
    # the various spaces in the command which means when argparse
    # gets the string argparse believes this as a single argument
    # and cannot correctly break the string into a list that
    # subprocess can handle.  By splitting the first element
    # in the command list, if there is a space, all of the
    # command[0] elements will be made into a list and if not
    # then split won't do everything and the old behavior
    # will continue.
    front_command = command[0].split(' ')
    del command[0]
    command = front_command + command
    extra_kw_args = {}

    if args.args_file is not None:
        empty = True
        for line in args.args_file:
            empty = False
            command.append(line.strip())

        if args.skip_empty_args and empty:
            return 0

    if args.env_file is not None:
        for line in args.env_file:
            apply_env_var(line, env)

    # Apply command-line overrides at a higher priority than the env file.
    for string in args.env:
        apply_env_var(string, env)

    if args.capture_output:
        extra_kw_args['stdout'] = subprocess.PIPE
        extra_kw_args['stderr'] = subprocess.STDOUT

    if args.working_directory:
        extra_kw_args['cwd'] = args.working_directory

    process = subprocess.run(command, env=env, **extra_kw_args)  # type: ignore

    if process.returncode != 0 and args.capture_output:
        _LOG.error('')
        _LOG.error(
            'Command failed with exit code %d in GN build.', process.returncode
        )
        _LOG.error('')
        _LOG.error('Build target:')
        _LOG.error('')
        _LOG.error('  %s', args.target)
        _LOG.error('')
        _LOG.error('Full command:')
        _LOG.error('')
        _LOG.error('  %s', ' '.join(shlex.quote(arg) for arg in command))
        _LOG.error('')
        _LOG.error('Process output:')
        print(flush=True)
        sys.stdout.buffer.write(process.stdout)
        print(flush=True)
        _LOG.error('')

    return process.returncode


if __name__ == '__main__':
    # If pw_cli is not yet installed in the virtualenv just skip it.
    if 'pw_cli' in globals():
        pw_cli.log.install()
    sys.exit(main())
