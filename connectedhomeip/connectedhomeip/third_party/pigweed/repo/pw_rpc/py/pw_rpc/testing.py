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
"""Utilities for testing pw_rpc."""

import argparse
import subprocess
import sys
import tempfile
import time
from typing import Optional, Sequence

TEMP_DIR_MARKER = '(pw_rpc:CREATE_TEMP_DIR)'


def parse_test_server_args(
    parser: Optional[argparse.ArgumentParser] = None,
) -> argparse.Namespace:
    """Parses arguments for running a Python-based integration test."""
    if parser is None:
        parser = argparse.ArgumentParser(
            description=sys.modules['__main__'].__doc__
        )

    parser.add_argument(
        '--test-server-command',
        nargs='+',
        required=True,
        help='Command that starts the test server.',
    )
    parser.add_argument(
        '--port',
        type=int,
        required=True,
        help=(
            'The port to use to connect to the test server. This value is '
            'passed to the test server as the last argument.'
        ),
    )
    parser.add_argument(
        'unittest_args',
        nargs=argparse.REMAINDER,
        help='Arguments after "--" are passed to unittest.',
    )

    args = parser.parse_args()

    # Append the port number to the test server command.
    args.test_server_command.append(str(args.port))

    # Make the script name argv[0] and drop the "--".
    args.unittest_args = sys.argv[:1] + args.unittest_args[1:]

    return args


def _parse_subprocess_integration_test_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description='Executes a test between two subprocesses'
    )
    parser.add_argument('--client', required=True, help='Client binary to run')
    parser.add_argument('--server', required=True, help='Server binary to run')
    parser.add_argument(
        'common_args',
        metavar='-- ...',
        nargs=argparse.REMAINDER,
        help=(
            'Arguments to pass to both the server and client; '
            f'pass {TEMP_DIR_MARKER} to generate a temporary directory'
        ),
    )

    args = parser.parse_args()

    if not args.common_args or args.common_args[0] != '--':
        parser.error('The common arguments must start with "--"')

    args.common_args.pop(0)

    return args


def execute_integration_test(
    server: str,
    client: str,
    common_args: Sequence[str],
    setup_time_s: float = 0.2,
) -> int:
    temp_dir: Optional[tempfile.TemporaryDirectory] = None

    if TEMP_DIR_MARKER in common_args:
        temp_dir = tempfile.TemporaryDirectory(prefix='pw_rpc_test_')
        common_args = [
            temp_dir.name if a == TEMP_DIR_MARKER else a for a in common_args
        ]

    try:
        server_process = subprocess.Popen([server, *common_args])
        # TODO(b/234879791): Replace this delay with some sort of IPC.
        time.sleep(setup_time_s)

        result = subprocess.run([client, *common_args]).returncode

        server_process.terminate()
        server_process.communicate()
    finally:
        if temp_dir:
            temp_dir.cleanup()

    return result


if __name__ == '__main__':
    sys.exit(
        execute_integration_test(
            **vars(_parse_subprocess_integration_test_args())
        )
    )
