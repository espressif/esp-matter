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
"""Decodes and detokenizes Base64-encoded strings in serial output.

The output is printed or saved to a file. Input is not supported.
"""

import argparse
import sys
from typing import BinaryIO, Iterable

import serial  # type: ignore
from pw_tokenizer import database, detokenize, tokens


def _parse_args():
    """Parses and return command line arguments."""

    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
        parents=[database.token_databases_parser()],
    )
    parser.add_argument(
        '-d',
        '--device',
        required=True,
        help='The serial device from which to read',
    )
    parser.add_argument(
        '-b',
        '--baudrate',
        type=int,
        default=115200,
        help='The baud rate for the serial device',
    )
    parser.add_argument(
        '-o',
        '--output',
        type=argparse.FileType('wb'),
        default=sys.stdout.buffer,
        help=(
            'The file to which to write the output; '
            'provide - or omit for stdout.'
        ),
    )
    parser.add_argument(
        '-p',
        '--prefix',
        default=detokenize.BASE64_PREFIX,
        help=(
            'The one-character prefix that signals the start of a '
            'Base64-encoded message. (default: $)'
        ),
    )
    parser.add_argument(
        '-s',
        '--show_errors',
        action='store_true',
        help=(
            'Show error messages instead of conversion specifiers when '
            'arguments cannot be decoded.'
        ),
    )

    return parser.parse_args()


def _detokenize_serial(
    databases: Iterable,
    device: serial.Serial,
    baudrate: int,
    show_errors: bool,
    output: BinaryIO,
    prefix: str,
) -> None:
    if output is sys.stdout:
        output = sys.stdout.buffer

    detokenizer = detokenize.Detokenizer(
        tokens.Database.merged(*databases), show_errors=show_errors
    )
    serial_device = serial.Serial(port=device, baudrate=baudrate)

    try:
        detokenizer.detokenize_base64_live(serial_device, output, prefix)
    except KeyboardInterrupt:
        output.flush()


def main():
    _detokenize_serial(**vars(_parse_args()))
    return 0


if __name__ == '__main__':
    sys.exit(main())
