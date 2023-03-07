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
"""The encode module supports encoding HDLC frames."""

from pw_hdlc import protocol

_ESCAPE_BYTE = bytes([protocol.ESCAPE])
_FLAG_BYTE = bytes([protocol.FLAG])


def ui_frame(address: int, data: bytes) -> bytes:
    """Encodes an HDLC UI-frame with a CRC-32 frame check sequence."""
    frame = (
        protocol.encode_address(address)
        + protocol.UFrameControl.unnumbered_information().data
        + data
    )
    frame += protocol.frame_check_sequence(frame)
    frame = frame.replace(_ESCAPE_BYTE, b'\x7d\x5d')
    frame = frame.replace(_FLAG_BYTE, b'\x7d\x5e')
    return b''.join([_FLAG_BYTE, frame, _FLAG_BYTE])
