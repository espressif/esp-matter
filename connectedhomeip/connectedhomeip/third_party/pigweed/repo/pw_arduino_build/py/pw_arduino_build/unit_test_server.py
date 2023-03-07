#!/usr/bin/env python3
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
"""Launch a pw_target_runner server to use for multi-device testing."""

import argparse
import logging
import sys
import tempfile
from typing import IO, List, Optional

import pw_cli.process

import pw_arduino_build.log
from pw_arduino_build import teensy_detector
from pw_arduino_build.file_operations import decode_file_json
from pw_arduino_build.unit_test_runner import ArduinoCoreNotSupported

_LOG = logging.getLogger('unit_test_server')

_TEST_RUNNER_COMMAND = 'arduino_unit_test_runner'

_TEST_SERVER_COMMAND = 'pw_target_runner_server'


class UnknownArduinoCore(Exception):
    """Exception raised when no Arduino core can be found."""


def parse_args():
    """Parses command-line arguments."""

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        '--server-port',
        type=int,
        default=8081,
        help='Port to launch the pw_target_runner_server on',
    )
    parser.add_argument(
        '--server-config',
        type=argparse.FileType('r'),
        help='Path to server config file',
    )
    parser.add_argument(
        '--verbose',
        '-v',
        dest='verbose',
        action="store_true",
        help='Output additional logs as the script runs',
    )
    parser.add_argument(
        "-c",
        "--config-file",
        required=True,
        help="Path to an arduino_builder config file.",
    )
    # TODO(tonymd): Explicitly split args using "--". See example in:
    # //pw_unit_test/py/pw_unit_test/test_runner.py:326
    parser.add_argument(
        'runner_args',
        nargs=argparse.REMAINDER,
        help='Arguments to forward to the test runner',
    )

    return parser.parse_args()


def generate_runner(command: str, arguments: List[str]) -> str:
    """Generates a text-proto style pw_target_runner_server configuration."""
    # TODO(amontanez): Use a real proto library to generate this when we have
    # one set up.
    for i, arg in enumerate(arguments):
        arguments[i] = f'  args: "{arg}"'
    runner = ['runner {', f'  command:"{command}"']
    runner.extend(arguments)
    runner.append('}\n')
    return '\n'.join(runner)


def generate_server_config(
    runner_args: Optional[List[str]], arduino_package_path: str
) -> IO[bytes]:
    """Returns a temporary generated file for use as the server config."""

    if "teensy" not in arduino_package_path:
        raise ArduinoCoreNotSupported(arduino_package_path)

    boards = teensy_detector.detect_boards(arduino_package_path)
    if not boards:
        _LOG.critical('No attached boards detected')
        sys.exit(1)
    config_file = tempfile.NamedTemporaryFile()
    _LOG.debug('Generating test server config at %s', config_file.name)
    _LOG.debug('Found %d attached devices', len(boards))
    for board in boards:
        test_runner_args = []
        if runner_args:
            test_runner_args += runner_args
        test_runner_args += ["-v"] + board.test_runner_args()
        test_runner_args += ["--port", board.dev_name]
        test_runner_args += ["--upload-tool", board.arduino_upload_tool_name]
        config_file.write(
            generate_runner(_TEST_RUNNER_COMMAND, test_runner_args).encode(
                'utf-8'
            )
        )
    config_file.flush()
    return config_file


def launch_server(
    server_config: Optional[IO[bytes]],
    server_port: Optional[int],
    runner_args: Optional[List[str]],
    arduino_package_path: str,
) -> int:
    """Launch a device test server with the provided arguments."""
    if server_config is None:
        # Auto-detect attached boards if no config is provided.
        server_config = generate_server_config(
            runner_args, arduino_package_path
        )

    cmd = [_TEST_SERVER_COMMAND, '-config', server_config.name]

    if server_port is not None:
        cmd.extend(['-port', str(server_port)])

    return pw_cli.process.run(*cmd, log_output=True).returncode


def main():
    """Launch a device test server with the provided arguments."""
    args = parse_args()

    if "--" in args.runner_args:
        args.runner_args.remove("--")

    log_level = logging.DEBUG if args.verbose else logging.INFO
    pw_arduino_build.log.install(log_level)

    # Get arduino_package_path from either the config file or command line args.
    arduino_package_path = None
    if args.config_file:
        json_file_options, unused_config_path = decode_file_json(
            args.config_file
        )
        arduino_package_path = json_file_options.get(
            "arduino_package_path", None
        )
        # Must pass --config-file option in the runner_args.
        if "--config-file" not in args.runner_args:
            args.runner_args.append("--config-file")
            args.runner_args.append(args.config_file)

    # Check for arduino_package_path in the runner_args
    try:
        arduino_package_path = args.runner_args[
            args.runner_args.index("--arduino-package-path") + 1
        ]
    except (ValueError, IndexError):
        # Only raise an error if arduino_package_path not set from the json.
        if arduino_package_path is None:
            raise UnknownArduinoCore(
                "Test runner arguments: '{}'".format(" ".join(args.runner_args))
            )

    exit_code = launch_server(
        args.server_config,
        args.server_port,
        args.runner_args,
        arduino_package_path,
    )
    sys.exit(exit_code)


if __name__ == '__main__':
    main()
