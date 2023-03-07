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
"""Module for low-level HDLC protocol features."""

from typing import Tuple

import zlib

# Special flag character for delimiting HDLC frames.
FLAG = 0x7E

# Special character for escaping other special characters in a frame.
ESCAPE = 0x7D

# Characters allowed after a 0x7d escape character.
VALID_ESCAPED_BYTES = 0x5D, 0x5E

# Maximum allowed HDLC address (uint64_t in C++).
MAX_ADDRESS = 2**64 - 1


def escape(byte: int) -> int:
    """Escapes or unescapes a byte, which should have been preceeded by 0x7d."""
    return byte ^ 0x20


def frame_check_sequence(data: bytes) -> bytes:
    return zlib.crc32(data).to_bytes(4, 'little')


def encode_address(address: int) -> bytes:
    """Encodes an HDLC address as a one-terminated LSB varint."""
    result = bytearray()

    while True:
        result += bytes([(address & 0x7F) << 1])

        address >>= 7
        if address == 0:
            break

    result[-1] |= 0x1
    return result


def decode_address(frame: bytes) -> Tuple[int, int]:
    """Decodes an HDLC address from a frame, returning it and its size."""
    result = 0
    length = 0

    while length < len(frame):
        byte = frame[length]
        result |= (byte >> 1) << (length * 7)
        length += 1

        if byte & 0x1 == 0x1:
            break

    if result > MAX_ADDRESS:
        return -1, 0

    return result, length


class UFrameControl:
    def __init__(self, frame_type: int):
        self._data: bytes = bytes([0x03 | frame_type])

    @property
    def data(self):
        return self._data

    @classmethod
    def unnumbered_information(cls):
        return UFrameControl(0x00)
