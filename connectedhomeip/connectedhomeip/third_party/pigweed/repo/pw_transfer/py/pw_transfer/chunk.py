# Copyright 2022 The Pigweed Authors
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
"""Protocol version-aware chunk message wrapper."""

import enum
from typing import Any, Optional

from pw_status import Status

try:
    from pw_transfer import transfer_pb2
except ImportError:
    # For the bazel build, which puts generated protos in a different location.
    from pigweed.pw_transfer import transfer_pb2  # type: ignore


class ProtocolVersion(enum.Enum):
    """Supported versions of pw_transfer's RPC data transfer protocol."""

    # Protocol version not known or not set.
    UNKNOWN = 0

    # The original transfer protocol, prior to transfer start/end handshakes.
    LEGACY = 1

    # Second version of the transfer protocol. Guarantees type fields on all
    # chunks, deprecates pending_bytes in favor of window_end_offset, splits
    # transfer resource IDs from ephemeral session IDs, and adds a handshake
    # to the start and end of all transfer sessions.
    VERSION_TWO = 2

    # Alias to the most up-to-date version of the transfer protocol.
    LATEST = VERSION_TWO


_ChunkType = transfer_pb2.Chunk.Type


class Chunk:
    """A chunk exchanged in a pw_transfer stream.

    Wraps the generated protobuf Chunk class with protocol-aware field encoding
    and decoding.
    """

    Type = transfer_pb2.Chunk.Type

    # TODO(frolv): Figure out how to make the chunk type annotation work.
    # pylint: disable=too-many-arguments
    def __init__(
        self,
        protocol_version: ProtocolVersion,
        chunk_type: Any,
        session_id: int = 0,
        resource_id: Optional[int] = None,
        offset: int = 0,
        window_end_offset: int = 0,
        data: bytes = b'',
        remaining_bytes: Optional[int] = None,
        max_chunk_size_bytes: Optional[int] = None,
        min_delay_microseconds: Optional[int] = None,
        status: Optional[Status] = None,
    ):
        self.protocol_version = protocol_version
        self.type = chunk_type
        self.session_id = session_id
        self.resource_id = resource_id
        self.offset = offset
        self.window_end_offset = window_end_offset
        self.data = data
        self.remaining_bytes = remaining_bytes
        self.max_chunk_size_bytes = max_chunk_size_bytes
        self.min_delay_microseconds = min_delay_microseconds
        self.status = status

    @classmethod
    def from_message(cls, message: transfer_pb2.Chunk) -> 'Chunk':
        """Parses a Chunk from a protobuf message."""

        # Some very old versions of transfer don't always encode chunk types,
        # so they must be deduced.
        #
        # The type-less legacy transfer protocol doesn't support handshakes or
        # continuation parameters. Therefore, there are only three possible
        # types: start, data, and retransmit.
        if message.HasField('type'):
            chunk_type = message.type
        elif (
            message.offset == 0
            and not message.data
            and not message.HasField('status')
        ):
            chunk_type = Chunk.Type.START
        elif message.data:
            chunk_type = Chunk.Type.DATA
        else:
            chunk_type = Chunk.Type.PARAMETERS_RETRANSMIT

        chunk = cls(
            ProtocolVersion.UNKNOWN,
            chunk_type,
            offset=message.offset,
            window_end_offset=message.window_end_offset,
            data=message.data,
        )

        if message.HasField('session_id'):
            chunk.protocol_version = ProtocolVersion.VERSION_TWO
            chunk.session_id = message.session_id
        else:
            chunk.protocol_version = ProtocolVersion.LEGACY
            chunk.session_id = message.transfer_id

        if message.HasField('resource_id'):
            chunk.resource_id = message.resource_id

        if message.HasField('protocol_version'):
            # An explicitly specified protocol version overrides any inferred
            # one.
            chunk.protocol_version = ProtocolVersion(message.protocol_version)

        if message.HasField('pending_bytes'):
            chunk.window_end_offset = message.offset + message.pending_bytes

        if message.HasField('remaining_bytes'):
            chunk.remaining_bytes = message.remaining_bytes

        if message.HasField('max_chunk_size_bytes'):
            chunk.max_chunk_size_bytes = message.max_chunk_size_bytes

        if message.HasField('min_delay_microseconds'):
            chunk.min_delay_microseconds = message.min_delay_microseconds

        if message.HasField('status'):
            chunk.status = Status(message.status)

        if chunk.protocol_version is ProtocolVersion.UNKNOWN:
            # If no fields in the chunk specified its protocol version,
            # assume it is a legacy chunk.
            chunk.protocol_version = ProtocolVersion.LEGACY

        return chunk

    def to_message(self) -> transfer_pb2.Chunk:
        """Converts the chunk to a protobuf message."""
        message = transfer_pb2.Chunk(
            offset=self.offset,
            window_end_offset=self.window_end_offset,
            type=self.type,
        )

        if self.resource_id is not None:
            message.resource_id = self.resource_id

        if self.protocol_version is ProtocolVersion.VERSION_TWO:
            if self.session_id != 0:
                message.session_id = self.session_id

        if self._should_encode_legacy_fields():
            if self.resource_id is not None:
                message.transfer_id = self.resource_id
            else:
                assert self.session_id != 0
                message.transfer_id = self.session_id

            # In the legacy protocol, the pending_bytes field must be set
            # alongside window_end_offset, as some transfer implementations
            # require it.
            if self.window_end_offset != 0:
                message.pending_bytes = self.window_end_offset - self.offset

        if self.data:
            message.data = self.data

        if self.remaining_bytes is not None:
            message.remaining_bytes = self.remaining_bytes

        if self.max_chunk_size_bytes is not None:
            message.max_chunk_size_bytes = self.max_chunk_size_bytes

        if self.min_delay_microseconds is not None:
            message.min_delay_microseconds = self.min_delay_microseconds

        if self.status is not None:
            message.status = self.status.value

        if self._is_initial_handshake_chunk():
            # During the initial handshake, the desired protocol version is
            # explictly encoded.
            message.protocol_version = self.protocol_version.value

        return message

    def id(self) -> int:
        """Returns the transfer context identifier for a chunk.

        Depending on the protocol version and type of chunk, this may correspond
        to one of several proto fields.
        """
        if self.resource_id is not None:
            # Always prioritize a resource_id over a session_id.
            return self.resource_id

        return self.session_id

    def requests_transmission_from_offset(self) -> bool:
        """Returns True if this chunk is requesting a retransmission."""
        return (
            self.type is Chunk.Type.PARAMETERS_RETRANSMIT
            or self.type is Chunk.Type.START
            or self.type is Chunk.Type.START_ACK_CONFIRMATION
        )

    def _is_initial_handshake_chunk(self) -> bool:
        return self.protocol_version is ProtocolVersion.VERSION_TWO and (
            self.type is Chunk.Type.START
            or self.type is Chunk.Type.START_ACK
            or self.type is Chunk.Type.START_ACK_CONFIRMATION
        )

    def _should_encode_legacy_fields(self) -> bool:
        return (
            self.protocol_version is ProtocolVersion.LEGACY
            or self.type is Chunk.Type.START
        )
