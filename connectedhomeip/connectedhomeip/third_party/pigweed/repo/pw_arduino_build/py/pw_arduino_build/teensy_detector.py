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
"""Detects attached Teensy boards connected via usb."""

import logging
import re
import subprocess
import typing

from pathlib import Path
from typing import List

import pw_arduino_build.log

_LOG = logging.getLogger('teensy_detector')


class UnknownArduinoCore(Exception):
    """Exception raised when a given core can not be found."""


def log_subprocess_output(level, output):
    """Logs subprocess output line-by-line."""

    lines = output.decode('utf-8', errors='replace').splitlines()
    for line in lines:
        _LOG.log(level, line)


class BoardInfo(typing.NamedTuple):
    """Information about a connected dev board."""

    dev_name: str
    usb_device_path: str
    protocol: str
    label: str
    arduino_upload_tool_name: str

    def test_runner_args(self) -> List[str]:
        return [
            "--set-variable",
            f"serial.port.protocol={self.protocol}",
            "--set-variable",
            f"serial.port={self.usb_device_path}",
            "--set-variable",
            f"serial.port.label={self.dev_name}",
        ]


def detect_boards(arduino_package_path=False) -> list:
    """Detect attached boards, returning a list of Board objects."""

    teensy_core = Path()
    if arduino_package_path:
        teensy_core = Path(arduino_package_path)
    else:
        teensy_core = Path("third_party/arduino/cores/teensy")
        if not teensy_core.exists():
            teensy_core = Path(
                "third_party/pigweed/third_party/arduino/cores/teensy"
            )

    if not teensy_core.exists():
        raise UnknownArduinoCore

    teensy_device_line_regex = re.compile(
        r"^(?P<address>[^ ]+) (?P<dev_name>[^ ]+) "
        r"\((?P<label>[^)]+)\) ?(?P<rest>.*)$"
    )

    boards = []
    detect_command = [
        (teensy_core / "hardware" / "tools" / "teensy_ports")
        .absolute()
        .as_posix(),
        "-L",
    ]

    # TODO(tonymd): teensy_ports -L on windows does not return the right port
    # string Example:
    #
    #   $ teensy_ports -L
    #   Port_#0001.Hub_#0003 COM3 (Teensy 3.6) Serial
    #
    # So we get "-port=Port_#0001.Hub_#0003"
    # But it should be "-port=usb:0/140000/0/1"

    process = subprocess.run(
        detect_command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT
    )
    if process.returncode != 0:
        _LOG.error("Command failed with exit code %d.", process.returncode)
        _LOG.error("Full command:")
        _LOG.error("")
        _LOG.error("  %s", " ".join(detect_command))
        _LOG.error("")
        _LOG.error("Process output:")
        log_subprocess_output(logging.ERROR, process.stdout)
        _LOG.error('')
    for line in process.stdout.decode("utf-8", errors="replace").splitlines():
        device_match_result = teensy_device_line_regex.match(line)
        if device_match_result:
            teensy_device = device_match_result.groupdict()
            boards.append(
                BoardInfo(
                    dev_name=teensy_device["dev_name"],
                    usb_device_path=teensy_device["address"],
                    protocol="Teensy",
                    label=teensy_device["label"],
                    arduino_upload_tool_name="teensyloader",
                )
            )
    return boards


def main():
    """This detects and then displays all attached discovery boards."""

    pw_arduino_build.log.install(logging.INFO)

    boards = detect_boards()
    if not boards:
        _LOG.info("No attached boards detected")
    for idx, board in enumerate(boards):
        _LOG.info("Board %d:", idx)
        _LOG.info("  - Name: %s", board.label)
        _LOG.info("  - Port: %s", board.dev_name)
        _LOG.info("  - Address: %s", board.usb_device_path)
        _LOG.info(
            "  - Test runner args: %s", " ".join(board.test_runner_args())
        )


if __name__ == "__main__":
    main()
