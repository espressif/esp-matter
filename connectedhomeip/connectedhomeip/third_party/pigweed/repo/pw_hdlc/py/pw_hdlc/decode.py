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
"""Decoder class for decoding bytes using HDLC protocol"""

import enum
import logging
import threading
import time
from typing import Iterable, Optional, Callable, Any
import zlib

from pw_hdlc import protocol

_LOG = logging.getLogger('pw_hdlc')

NO_ADDRESS = -1
_MIN_FRAME_SIZE = 6  # 1 B address + 1 B control + 4 B CRC-32
_FLAG_BYTE = bytes([protocol.FLAG])


class FrameStatus(enum.Enum):
    """Indicates that an error occurred."""

    OK = 'OK'
    FCS_MISMATCH = 'frame check sequence failure'
    FRAMING_ERROR = 'invalid flag or escape characters'
    BAD_ADDRESS = 'address field too long'


class Frame:
    """Represents an HDLC frame."""

    def __init__(
        self,
        raw_encoded: bytes,
        raw_decoded: bytes,
        status: FrameStatus = FrameStatus.OK,
    ):
        """Parses fields from an HDLC frame.

        Arguments:
            raw_encoded: The complete HDLC-encoded frame, including any HDLC
                flag bytes.  In the case of back to back frames, the
                beginning flag byte may be omitted.
            raw_decoded: The complete decoded frame (address, control,
                information, FCS).
            status: Whether parsing the frame succeeded.
        """
        self.raw_encoded = raw_encoded
        self.raw_decoded = raw_decoded
        self.status = status

        self.address: int = NO_ADDRESS
        self.control: bytes = b''
        self.data: bytes = b''

        if status == FrameStatus.OK:
            address, address_length = protocol.decode_address(raw_decoded)
            if address_length == 0:
                self.status = FrameStatus.BAD_ADDRESS
                return

            self.address = address
            self.control = raw_decoded[address_length : address_length + 1]
            self.data = raw_decoded[address_length + 1 : -4]

    def ok(self) -> bool:
        """True if this represents a valid frame.

        If false, then parsing failed. The status is set to indicate what type
        of error occurred, and the data field contains all bytes parsed from the
        frame (including bytes parsed as address or control bytes).
        """
        return self.status is FrameStatus.OK

    def __repr__(self) -> str:
        if self.ok():
            body = (
                f'address={self.address}, control={self.control!r}, '
                f'data={self.data!r}'
            )
        else:
            body = (
                f'raw_encoded={self.raw_encoded!r}, '
                f'status={str(self.status)}'
            )

        return f'{type(self).__name__}({body})'


class _State(enum.Enum):
    INTERFRAME = 0
    FRAME = 1
    FRAME_ESCAPE = 2


def _check_frame(frame_data: bytes) -> FrameStatus:
    if len(frame_data) < _MIN_FRAME_SIZE:
        return FrameStatus.FRAMING_ERROR

    frame_crc = int.from_bytes(frame_data[-4:], 'little')
    if zlib.crc32(frame_data[:-4]) != frame_crc:
        return FrameStatus.FCS_MISMATCH

    return FrameStatus.OK


class FrameDecoder:
    """Decodes one or more HDLC frames from a stream of data."""

    def __init__(self) -> None:
        self._decoded_data = bytearray()
        self._raw_data = bytearray()
        self._state = _State.INTERFRAME

    def process(self, data: bytes) -> Iterable[Frame]:
        """Decodes and yields HDLC frames, including corrupt frames.

        The ok() method on Frame indicates whether it is valid or represents a
        frame parsing error.

        Yields:
          Frames, which may be valid (frame.ok()) or corrupt (!frame.ok())
        """
        for byte in data:
            frame = self.process_byte(byte)
            if frame:
                yield frame

    def process_valid_frames(self, data: bytes) -> Iterable[Frame]:
        """Decodes and yields valid HDLC frames, logging any errors."""
        for frame in self.process(data):
            if frame.ok():
                yield frame
            else:
                _LOG.warning(
                    'Failed to decode frame: %s; discarded %d bytes',
                    frame.status.value,
                    len(frame.raw_encoded),
                )
                _LOG.debug('Discarded data: %s', frame.raw_encoded)

    def _finish_frame(self, status: FrameStatus) -> Frame:
        # HDLC frames always start and end with a flag character, though the
        # character may be shared with other frames. Ensure the raw encoding of
        # OK frames always includes the start and end flags for consistency.
        if status is FrameStatus.OK:
            if not self._raw_data.startswith(_FLAG_BYTE):
                self._raw_data.insert(0, protocol.FLAG)

        frame = Frame(bytes(self._raw_data), bytes(self._decoded_data), status)
        self._raw_data.clear()
        self._decoded_data.clear()
        return frame

    def process_byte(self, byte: int) -> Optional[Frame]:
        """Processes a single byte and returns a frame if one was completed."""
        frame: Optional[Frame] = None

        self._raw_data.append(byte)

        if self._state is _State.INTERFRAME:
            if byte == protocol.FLAG:
                if len(self._raw_data) != 1:
                    frame = self._finish_frame(FrameStatus.FRAMING_ERROR)

                self._state = _State.FRAME
        elif self._state is _State.FRAME:
            if byte == protocol.FLAG:
                # On back to back frames, we may see a repeated FLAG byte.
                if len(self._raw_data) > 1:
                    frame = self._finish_frame(_check_frame(self._decoded_data))

                self._state = _State.FRAME
            elif byte == protocol.ESCAPE:
                self._state = _State.FRAME_ESCAPE
            else:
                self._decoded_data.append(byte)
        elif self._state is _State.FRAME_ESCAPE:
            if byte == protocol.FLAG:
                frame = self._finish_frame(FrameStatus.FRAMING_ERROR)
                self._state = _State.FRAME
            elif byte in protocol.VALID_ESCAPED_BYTES:
                self._state = _State.FRAME
                self._decoded_data.append(protocol.escape(byte))
            else:
                self._state = _State.INTERFRAME
        else:
            raise AssertionError(f'Invalid decoder state: {self._state}')

        return frame


class FrameAndNonFrameDecoder:
    """Processes both HDLC frames and non-frame data in a stream."""

    def __init__(
        self,
        non_frame_data_handler: Callable[[bytes], Any],
        *,
        mtu: Optional[int] = None,
        timeout_s: Optional[float] = None,
        handle_shared_flags: bool = True,
    ) -> None:
        """Yields valid HDLC frames and passes non-frame data to callback.

        Args:
          mtu: Maximum bytes to receive before flushing raw data. If a valid
              HDLC frame contains more than MTU bytes, the valid frame will be
              emitted, but part of the frame will be included in the raw data.
          timeout_s: How long to wait before automatically flushing raw data. If
              a timeout occurs partway through a valid frame, the frame will be
              emitted, but part of the frame will be included in the raw data.
          handle_shared_flags: Whether to permit HDLC frames to share a single
              flag byte between frames. If False, partial HDLC frames may be
              emitted as raw data when HDLC frames share a flag byte, but raw
              data won't have to wait for a timeout or full MTU to be flushed.
        """
        self._non_frame_data_handler = non_frame_data_handler
        self._mtu = mtu
        self._shared_flags = handle_shared_flags
        self._timeout_s = timeout_s

        self._raw_data = bytearray()
        self._hdlc_decoder = FrameDecoder()
        self._last_data_time = time.time()
        self._lock = threading.Lock()

        if self._timeout_s is not None:
            threading.Thread(target=self._timeout_thread, daemon=True).start()

    def flush_non_frame_data(self) -> None:
        """Flushes any data in the buffer as non-frame data.

        If a valid HDLC frame was flushed partway, the data for the first part
        of the frame will be included both in the raw data and in the frame.
        """
        with self._lock:
            self._flush_non_frame()

    def _flush_non_frame(self, to_index: Optional[int] = None):
        if self._raw_data:
            self._non_frame_data_handler(bytes(self._raw_data[:to_index]))
            del self._raw_data[:to_index]

    def _timeout_thread(self) -> None:
        assert self._timeout_s is not None

        while True:
            time.sleep(self._timeout_s)
            with self._lock:
                if time.time() - self._last_data_time > self._timeout_s:
                    self._flush_non_frame()

    def process(self, data: bytes) -> Iterable[Frame]:
        """Processes a stream of mixed HDLC and unstructured data.

        Yields OK frames and calls non_frame_data_handler with non-HDLC data.
        """
        with self._lock:
            for byte in data:
                yield from self._process_byte(byte)

            # Flush the data if it is larger than the MTU, or flag bytes are not
            # being shared and no initial flag was seen.
            if (self._mtu is not None and len(self._raw_data) > self._mtu) or (
                not self._shared_flags
                and not self._raw_data.startswith(_FLAG_BYTE)
            ):
                self._flush_non_frame()

            self._last_data_time = time.time()

    def _process_byte(self, byte: int) -> Iterable[Frame]:
        self._raw_data.append(byte)
        frame = self._hdlc_decoder.process_byte(byte)

        if frame is None:
            return

        if frame.ok():
            # Drop the valid frame from the data. Only drop matching bytes in
            # case the frame was flushed prematurely.
            for suffix_byte in reversed(frame.raw_encoded):
                if not self._raw_data or self._raw_data[-1] != suffix_byte:
                    break
                self._raw_data.pop()

            self._flush_non_frame()  # Flush the raw data before the frame.

            if self._mtu is not None and len(frame.raw_encoded) > self._mtu:
                _LOG.warning(
                    'Found a valid %d B HDLC frame, but the MTU is set to %d! '
                    'The MTU setting may be incorrect.',
                    self._mtu,
                    len(frame.raw_encoded),
                )

            yield frame
        else:
            # Don't flush a final flag byte yet because it might be the start of
            # an HDLC frame.
            to_index = -1 if self._raw_data[-1] == protocol.FLAG else None
            self._flush_non_frame(to_index)
