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
"""Detects attached stm32f429i-disc1 boards connected via mini usb."""

import logging
import typing

import coloredlogs  # type: ignore
import serial.tools.list_ports  # type: ignore

# Vendor/device ID to search for in USB devices.
_ST_VENDOR_ID = 0x0483
_DISCOVERY_MODEL_ID = 0x374B

_LOG = logging.getLogger('stm32f429i_detector')


class BoardInfo(typing.NamedTuple):
    """Information about a connected dev board."""

    dev_name: str
    serial_number: str


def detect_boards() -> list:
    """Detect attached boards, returning a list of Board objects."""
    boards = []
    all_devs = serial.tools.list_ports.comports()
    for dev in all_devs:
        if dev.vid == _ST_VENDOR_ID and dev.pid == _DISCOVERY_MODEL_ID:
            boards.append(
                BoardInfo(dev_name=dev.device, serial_number=dev.serial_number)
            )
    return boards


def main():
    """This detects and then displays all attached discovery boards."""

    # Try to use pw_cli logs, else default to something reasonable.
    try:
        import pw_cli.log  # pylint: disable=import-outside-toplevel

        pw_cli.log.install()
    except ImportError:
        coloredlogs.install(
            level='INFO',
            level_styles={'debug': {'color': 244}, 'error': {'color': 'red'}},
            fmt='%(asctime)s %(levelname)s | %(message)s',
        )

    boards = detect_boards()
    if not boards:
        _LOG.info('No attached boards detected')
    for idx, board in enumerate(boards):
        _LOG.info('Board %d:', idx)
        _LOG.info('  - Port: %s', board.dev_name)
        _LOG.info('  - Serial #: %s', board.serial_number)


if __name__ == '__main__':
    main()
