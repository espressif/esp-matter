#!/usr/bin/env python3
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
"""This script flashes and runs unit tests on stm32f429i-disc1 boards."""

import argparse
import logging
import os
import subprocess
import sys
import threading
from typing import List

import coloredlogs  # type: ignore
import serial  # type: ignore
from stm32f429i_disc1_utils import stm32f429i_detector

# Path used to access non-python resources in this python module.
_DIR = os.path.dirname(__file__)

# Path to default openocd configuration file.
_OPENOCD_CONFIG = os.path.join(_DIR, 'openocd_stm32f4xx.cfg')

# Path to scripts provided by openocd.
_OPENOCD_SCRIPTS_DIR = os.path.join(
    os.getenv('PW_PIGWEED_CIPD_INSTALL_DIR', ''), 'share', 'openocd', 'scripts'
)

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


def parse_args():
    """Parses command-line arguments."""

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('binary', help='The target test binary to run')
    parser.add_argument(
        '--openocd-config',
        default=_OPENOCD_CONFIG,
        help='Path to openocd configuration file',
    )
    parser.add_argument(
        '--stlink-serial',
        default=None,
        help='The serial number of the stlink to use when '
        'flashing the target device',
    )
    parser.add_argument(
        '--port',
        default=None,
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
        action="store_true",
        help='Output additional logs as the script runs',
    )

    return parser.parse_args()


def log_subprocess_output(level, output):
    """Logs subprocess output line-by-line."""

    lines = output.decode('utf-8', errors='replace').splitlines()
    for line in lines:
        _LOG.log(level, line)


def reset_device(openocd_config, stlink_serial):
    """Uses openocd to reset the attached device."""

    # Name/path of openocd.
    default_flasher = 'openocd'
    flash_tool = os.getenv('OPENOCD_PATH', default_flasher)

    cmd = [
        flash_tool,
        '-s',
        _OPENOCD_SCRIPTS_DIR,
        '-f',
        openocd_config,
        '-c',
        'init',
        '-c',
        'reset run',
        '-c',
        'exit',
    ]
    _LOG.debug('Resetting device')

    env = os.environ.copy()
    if stlink_serial:
        env['PW_STLINK_SERIAL'] = stlink_serial

    # Disable GDB port to support multi-device testing.
    env['PW_GDB_PORT'] = 'disabled'
    process = subprocess.run(
        cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, env=env
    )
    if process.returncode:
        log_subprocess_output(logging.ERROR, process.stdout)
        raise TestingFailure('Failed to reset target device')

    log_subprocess_output(logging.DEBUG, process.stdout)

    _LOG.debug('Successfully reset device')


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


def flash_device(binary, openocd_config, stlink_serial):
    """Flash binary to a connected device using the provided configuration."""

    # Name/path of openocd.
    default_flasher = 'openocd'
    flash_tool = os.getenv('OPENOCD_PATH', default_flasher)

    openocd_command = ' '.join(['program', binary, 'reset', 'exit'])
    cmd = [
        flash_tool,
        '-s',
        _OPENOCD_SCRIPTS_DIR,
        '-f',
        openocd_config,
        '-c',
        openocd_command,
    ]
    _LOG.info('Flashing firmware to device')

    env = os.environ.copy()
    if stlink_serial:
        env['PW_STLINK_SERIAL'] = stlink_serial

    # Disable GDB port to support multi-device testing.
    env['PW_GDB_PORT'] = 'disabled'
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


def _threaded_test_reader(dest, port, baud_rate, test_timeout):
    """Parses test output to the mutable "dest" passed to this function."""
    dest.append(read_serial(port, baud_rate, test_timeout))


def run_device_test(
    binary, test_timeout, openocd_config, baud, stlink_serial=None, port=None
) -> bool:
    """Flashes, runs, and checks an on-device test binary.

    Returns true on test pass.
    """

    if stlink_serial is None and port is None:
        _LOG.debug('Attempting to automatically detect dev board')
        boards = stm32f429i_detector.detect_boards()
        if not boards:
            error = 'Could not find an attached device'
            _LOG.error(error)
            raise DeviceNotFound(error)
        stlink_serial = boards[0].serial_number
        port = boards[0].dev_name

    _LOG.debug('Launching test binary %s', binary)
    try:
        # Begin capturing test output via another thread BEFORE flashing the
        # device since the test will automatically run after the image is
        # flashed. This reduces flake since there isn't a need to time a reset
        # correctly relative to the start of capturing device output.
        result: List[bytes] = []
        threaded_reader_args = (result, port, baud, test_timeout)
        read_thread = threading.Thread(
            target=_threaded_test_reader, args=threaded_reader_args
        )
        read_thread.start()
        _LOG.info('Running test')
        flash_device(binary, openocd_config, stlink_serial)
        read_thread.join()
        if result:
            handle_test_results(result[0])
    except TestingFailure as err:
        _LOG.error(err)
        return False

    return True


def main():
    """Set up runner, and then flash/run device test."""
    args = parse_args()

    # Try to use pw_cli logs, else default to something reasonable.
    try:
        import pw_cli.log  # pylint: disable=import-outside-toplevel

        log_level = logging.DEBUG if args.verbose else logging.INFO
        pw_cli.log.install(level=log_level)
    except ImportError:
        coloredlogs.install(
            level='DEBUG' if args.verbose else 'INFO',
            level_styles={'debug': {'color': 244}, 'error': {'color': 'red'}},
            fmt='%(asctime)s %(levelname)s | %(message)s',
        )

    if run_device_test(
        args.binary,
        args.test_timeout,
        args.openocd_config,
        args.baud,
        args.stlink_serial,
        args.port,
    ):
        sys.exit(0)
    else:
        sys.exit(1)


if __name__ == '__main__':
    main()
