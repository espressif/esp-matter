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
"""This script flashes and runs unit tests onto Arduino boards."""

import argparse
import logging
import os
import platform
import re
import subprocess
import sys
import time
from pathlib import Path
from typing import List

import serial  # type: ignore
import serial.tools.list_ports  # type: ignore
import pw_arduino_build.log
from pw_arduino_build import teensy_detector
from pw_arduino_build.file_operations import decode_file_json

_LOG = logging.getLogger('unit_test_runner')

# Verification of test pass/failure depends on these strings. If the formatting
# or output of the simple_printing_event_handler changes, this may need to be
# updated.
_TESTS_STARTING_STRING = b'[==========] Running all tests.'
_TESTS_DONE_STRING = b'[==========] Done running all tests.'
_TEST_FAILURE_STRING = b'[  FAILED  ]'

# How long to wait for the first byte of a test to be emitted. This is longer
# than the user-configurable timeout as there's a delay while the device is
# flashed.
_FLASH_TIMEOUT = 5.0


class TestingFailure(Exception):
    """A simple exception to be raised when a testing step fails."""


class DeviceNotFound(Exception):
    """A simple exception to be raised when unable to connect to a device."""


class ArduinoCoreNotSupported(Exception):
    """Exception raised when a given core does not support unit testing."""


def valid_file_name(arg):
    file_path = Path(os.path.expandvars(arg)).absolute()
    if not file_path.is_file():
        raise argparse.ArgumentTypeError(f"'{arg}' does not exist.")
    return file_path


def parse_args():
    """Parses command-line arguments."""

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        'binary', help='The target test binary to run', type=valid_file_name
    )
    parser.add_argument(
        '--port',
        help='The name of the serial port to connect to when ' 'running tests',
    )
    parser.add_argument(
        '--baud',
        type=int,
        default=115200,
        help='Target baud rate to use for serial communication'
        ' with target device',
    )
    parser.add_argument(
        '--test-timeout',
        type=float,
        default=5.0,
        help='Maximum communication delay in seconds before a '
        'test is considered unresponsive and aborted',
    )
    parser.add_argument(
        '--verbose',
        '-v',
        dest='verbose',
        action='store_true',
        help='Output additional logs as the script runs',
    )

    parser.add_argument(
        '--flash-only',
        action='store_true',
        help="Don't check for test output after flashing.",
    )

    # arduino_builder arguments
    # TODO(tonymd): Get these args from __main__.py or elsewhere.
    parser.add_argument(
        "-c", "--config-file", required=True, help="Path to a config file."
    )
    parser.add_argument(
        "--arduino-package-path",
        help="Path to the arduino IDE install location.",
    )
    parser.add_argument(
        "--arduino-package-name",
        help="Name of the Arduino board package to use.",
    )
    parser.add_argument(
        "--compiler-path-override",
        help="Path to arm-none-eabi-gcc bin folder. "
        "Default: Arduino core specified gcc",
    )
    parser.add_argument("--board", help="Name of the Arduino board to use.")
    parser.add_argument(
        "--upload-tool",
        required=True,
        help="Name of the Arduino upload tool to use.",
    )
    parser.add_argument(
        "--set-variable",
        action="append",
        metavar='some.variable=NEW_VALUE',
        help="Override an Arduino recipe variable. May be "
        "specified multiple times. For example: "
        "--set-variable 'serial.port.label=/dev/ttyACM0' "
        "--set-variable 'serial.port.protocol=Teensy'",
    )
    return parser.parse_args()


def log_subprocess_output(level, output):
    """Logs subprocess output line-by-line."""

    lines = output.decode('utf-8', errors='replace').splitlines()
    for line in lines:
        _LOG.log(level, line)


def read_serial(port, baud_rate, test_timeout) -> bytes:
    """Reads lines from a serial port until a line read times out.

    Returns bytes object containing the read serial data.
    """

    serial_data = bytearray()
    device = serial.Serial(
        baudrate=baud_rate, port=port, timeout=_FLASH_TIMEOUT
    )
    if not device.is_open:
        raise TestingFailure('Failed to open device')

    # Flush input buffer and reset the device to begin the test.
    device.reset_input_buffer()

    # Block and wait for the first byte.
    serial_data += device.read()
    if not serial_data:
        raise TestingFailure('Device not producing output')

    device.timeout = test_timeout

    # Read with a reasonable timeout until we stop getting characters.
    while True:
        bytes_read = device.readline()
        if not bytes_read:
            break
        serial_data += bytes_read
        if serial_data.rfind(_TESTS_DONE_STRING) != -1:
            # Set to much more aggressive timeout since the last one or two
            # lines should print out immediately. (one line if all fails or all
            # passes, two lines if mixed.)
            device.timeout = 0.01

    # Remove carriage returns.
    serial_data = serial_data.replace(b'\r', b'')

    # Try to trim captured results to only contain most recent test run.
    test_start_index = serial_data.rfind(_TESTS_STARTING_STRING)
    return (
        serial_data
        if test_start_index == -1
        else serial_data[test_start_index:]
    )


def wait_for_port(port):
    """Wait for the serial port to be available."""
    while port not in [sp.device for sp in serial.tools.list_ports.comports()]:
        time.sleep(1)


def flash_device(test_runner_args, upload_tool):
    """Flash binary to a connected device using the provided configuration."""

    # TODO(tonymd): Create a library function to call rather than launching
    # the arduino_builder script.
    flash_tool = 'arduino_builder'
    cmd = (
        [flash_tool, "--quiet"]
        + test_runner_args
        + ["--run-objcopy", "--run-postbuilds", "--run-upload", upload_tool]
    )
    _LOG.info('Flashing firmware to device')
    _LOG.debug('Running: %s', " ".join(cmd))

    env = os.environ.copy()
    process = subprocess.run(
        cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, env=env
    )
    if process.returncode:
        log_subprocess_output(logging.ERROR, process.stdout)
        raise TestingFailure('Failed to flash target device')

    log_subprocess_output(logging.DEBUG, process.stdout)

    _LOG.debug('Successfully flashed firmware to device')


def handle_test_results(test_output):
    """Parses test output to determine whether tests passed or failed."""

    if test_output.find(_TESTS_STARTING_STRING) == -1:
        raise TestingFailure('Failed to find test start')

    if test_output.rfind(_TESTS_DONE_STRING) == -1:
        log_subprocess_output(logging.INFO, test_output)
        raise TestingFailure('Tests did not complete')

    if test_output.rfind(_TEST_FAILURE_STRING) != -1:
        log_subprocess_output(logging.INFO, test_output)
        raise TestingFailure('Test suite had one or more failures')

    log_subprocess_output(logging.DEBUG, test_output)

    _LOG.info('Test passed!')


def run_device_test(
    binary,
    flash_only,
    port,
    baud,
    test_timeout,
    upload_tool,
    arduino_package_path,
    test_runner_args,
) -> bool:
    """Flashes, runs, and checks an on-device test binary.

    Returns true on test pass.
    """
    if test_runner_args is None:
        test_runner_args = []

    if "teensy" not in arduino_package_path:
        raise ArduinoCoreNotSupported(arduino_package_path)

    if port is None or "--set-variable" not in test_runner_args:
        _LOG.debug('Attempting to automatically detect dev board')
        boards = teensy_detector.detect_boards(arduino_package_path)
        if not boards:
            error = 'Could not find an attached device'
            _LOG.error(error)
            raise DeviceNotFound(error)
        test_runner_args += boards[0].test_runner_args()
        upload_tool = boards[0].arduino_upload_tool_name
        if port is None:
            port = boards[0].dev_name

    # TODO(tonymd): Remove this when teensy_ports is working in teensy_detector
    if platform.system() == "Windows":
        # Delete the incorrect serial port.
        index_of_port = [
            i
            for i, l in enumerate(test_runner_args)
            if l.startswith('serial.port=')
        ]
        if index_of_port:
            # Delete the '--set-variable' arg
            del test_runner_args[index_of_port[0] - 1]
            # Delete the 'serial.port=*' arg
            del test_runner_args[index_of_port[0] - 1]

    _LOG.debug('Launching test binary %s', binary)
    try:
        result: List[bytes] = []
        _LOG.info('Running test')
        # Warning: A race condition is possible here. This assumes the host is
        # able to connect to the port and that there isn't a test running on
        # this serial port.
        flash_device(test_runner_args, upload_tool)
        wait_for_port(port)
        if flash_only:
            return True
        result.append(read_serial(port, baud, test_timeout))
        if result:
            handle_test_results(result[0])
    except TestingFailure as err:
        _LOG.error(err)
        return False

    return True


def get_option(key, config_file_values, args, required=False):
    command_line_option = getattr(args, key, None)
    final_option = config_file_values.get(key, command_line_option)
    if required and command_line_option is None and final_option is None:
        # Print a similar error message to argparse
        executable = os.path.basename(sys.argv[0])
        option = "--" + key.replace("_", "-")
        print(
            f"{executable}: error: the following arguments are required: "
            f"{option}"
        )
        sys.exit(1)
    return final_option


def main():
    """Set up runner, and then flash/run device test."""
    args = parse_args()

    json_file_options, unused_config_path = decode_file_json(args.config_file)

    log_level = logging.DEBUG if args.verbose else logging.INFO
    pw_arduino_build.log.install(log_level)

    # Construct arduino_builder flash arguments for a given .elf binary.
    arduino_package_path = get_option(
        "arduino_package_path", json_file_options, args, required=True
    )
    # Arduino core args.
    arduino_builder_args = [
        "--arduino-package-path",
        arduino_package_path,
        "--arduino-package-name",
        get_option(
            "arduino_package_name", json_file_options, args, required=True
        ),
    ]

    # Use CIPD installed compilers.
    compiler_path_override = get_option(
        "compiler_path_override", json_file_options, args
    )
    if compiler_path_override:
        arduino_builder_args += [
            "--compiler-path-override",
            compiler_path_override,
        ]

    # Run subcommand with board selection arg.
    arduino_builder_args += [
        "run",
        "--board",
        get_option("board", json_file_options, args, required=True),
    ]

    # .elf file location args.
    binary = args.binary
    build_path = binary.parent.as_posix()
    arduino_builder_args += ["--build-path", build_path]
    build_project_name = binary.name
    # Remove '.elf' extension.
    match_result = re.match(r'(.*?)\.elf$', binary.name, re.IGNORECASE)
    if match_result:
        build_project_name = match_result[1]
        arduino_builder_args += ["--build-project-name", build_project_name]

    # USB port is passed to arduino_builder_args via --set-variable args.
    if args.set_variable:
        for var in args.set_variable:
            arduino_builder_args += ["--set-variable", var]

    if run_device_test(
        binary.as_posix(),
        args.flash_only,
        args.port,
        args.baud,
        args.test_timeout,
        args.upload_tool,
        arduino_package_path,
        test_runner_args=arduino_builder_args,
    ):
        sys.exit(0)
    else:
        sys.exit(1)


if __name__ == '__main__':
    main()
