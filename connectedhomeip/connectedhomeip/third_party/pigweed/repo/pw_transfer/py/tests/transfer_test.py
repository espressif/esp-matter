#!/usr/bin/env python3
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
"""Tests for the transfer service client."""

import enum
import math
import unittest
from typing import Iterable, List

from pw_status import Status
from pw_rpc import callback_client, client, ids, packets
from pw_rpc.internal import packet_pb2

import pw_transfer
from pw_transfer import ProtocolVersion

try:
    from pw_transfer import transfer_pb2
except ImportError:
    # For the bazel build, which puts generated protos in a different location.
    from pigweed.pw_transfer import transfer_pb2  # type: ignore

_TRANSFER_SERVICE_ID = ids.calculate('pw.transfer.Transfer')

# If the default timeout is too short, some tests become flaky on Windows.
DEFAULT_TIMEOUT_S = 0.3


class _Method(enum.Enum):
    READ = ids.calculate('Read')
    WRITE = ids.calculate('Write')


# pylint: disable=missing-function-docstring, missing-class-docstring


class TransferManagerTest(unittest.TestCase):
    # pylint: disable=too-many-public-methods
    """Tests for the transfer manager."""

    def setUp(self) -> None:
        self._client = client.Client.from_modules(
            callback_client.Impl(),
            [client.Channel(1, self._handle_request)],
            (transfer_pb2,),
        )
        self._service = self._client.channel(1).rpcs.pw.transfer.Transfer

        self._sent_chunks: List[transfer_pb2.Chunk] = []
        self._packets_to_send: List[List[bytes]] = []

    def _enqueue_server_responses(
        self, method: _Method, responses: Iterable[Iterable[transfer_pb2.Chunk]]
    ) -> None:
        for group in responses:
            serialized_group = []
            for response in group:
                serialized_group.append(
                    packet_pb2.RpcPacket(
                        type=packet_pb2.PacketType.SERVER_STREAM,
                        channel_id=1,
                        service_id=_TRANSFER_SERVICE_ID,
                        method_id=method.value,
                        status=Status.OK.value,
                        payload=response.SerializeToString(),
                    ).SerializeToString()
                )
            self._packets_to_send.append(serialized_group)

    def _enqueue_server_error(self, method: _Method, error: Status) -> None:
        self._packets_to_send.append(
            [
                packet_pb2.RpcPacket(
                    type=packet_pb2.PacketType.SERVER_ERROR,
                    channel_id=1,
                    service_id=_TRANSFER_SERVICE_ID,
                    method_id=method.value,
                    status=error.value,
                ).SerializeToString()
            ]
        )

    def _handle_request(self, data: bytes) -> None:
        packet = packets.decode(data)
        if packet.type is not packet_pb2.PacketType.CLIENT_STREAM:
            return

        chunk = transfer_pb2.Chunk()
        chunk.MergeFromString(packet.payload)
        self._sent_chunks.append(chunk)

        if self._packets_to_send:
            responses = self._packets_to_send.pop(0)
            for response in responses:
                self._client.process_packet(response)

    def _received_data(self) -> bytearray:
        data = bytearray()
        for chunk in self._sent_chunks:
            data.extend(chunk.data)
        return data

    def test_read_transfer_basic(self):
        manager = pw_transfer.Manager(
            self._service, default_response_timeout_s=DEFAULT_TIMEOUT_S
        )

        self._enqueue_server_responses(
            _Method.READ,
            (
                (
                    transfer_pb2.Chunk(
                        transfer_id=3, offset=0, data=b'abc', remaining_bytes=0
                    ),
                ),
            ),
        )

        data = manager.read(3)
        self.assertEqual(data, b'abc')
        self.assertEqual(len(self._sent_chunks), 2)
        self.assertTrue(self._sent_chunks[-1].HasField('status'))
        self.assertEqual(self._sent_chunks[-1].status, 0)

    def test_read_transfer_multichunk(self) -> None:
        manager = pw_transfer.Manager(
            self._service, default_response_timeout_s=DEFAULT_TIMEOUT_S
        )

        self._enqueue_server_responses(
            _Method.READ,
            (
                (
                    transfer_pb2.Chunk(
                        transfer_id=3, offset=0, data=b'abc', remaining_bytes=3
                    ),
                    transfer_pb2.Chunk(
                        transfer_id=3, offset=3, data=b'def', remaining_bytes=0
                    ),
                ),
            ),
        )

        data = manager.read(3)
        self.assertEqual(data, b'abcdef')
        self.assertEqual(len(self._sent_chunks), 2)
        self.assertTrue(self._sent_chunks[-1].HasField('status'))
        self.assertEqual(self._sent_chunks[-1].status, 0)

    def test_read_transfer_progress_callback(self) -> None:
        manager = pw_transfer.Manager(
            self._service, default_response_timeout_s=DEFAULT_TIMEOUT_S
        )

        self._enqueue_server_responses(
            _Method.READ,
            (
                (
                    transfer_pb2.Chunk(
                        transfer_id=3, offset=0, data=b'abc', remaining_bytes=3
                    ),
                    transfer_pb2.Chunk(
                        transfer_id=3, offset=3, data=b'def', remaining_bytes=0
                    ),
                ),
            ),
        )

        progress: List[pw_transfer.ProgressStats] = []

        data = manager.read(3, progress.append)
        self.assertEqual(data, b'abcdef')
        self.assertEqual(len(self._sent_chunks), 2)
        self.assertTrue(self._sent_chunks[-1].HasField('status'))
        self.assertEqual(self._sent_chunks[-1].status, 0)
        self.assertEqual(
            progress,
            [
                pw_transfer.ProgressStats(3, 3, 6),
                pw_transfer.ProgressStats(6, 6, 6),
            ],
        )

    def test_read_transfer_retry_bad_offset(self) -> None:
        """Server responds with an unexpected offset in a read transfer."""
        manager = pw_transfer.Manager(
            self._service, default_response_timeout_s=DEFAULT_TIMEOUT_S
        )

        self._enqueue_server_responses(
            _Method.READ,
            (
                (
                    transfer_pb2.Chunk(
                        transfer_id=3, offset=0, data=b'123', remaining_bytes=6
                    ),
                    # Incorrect offset; expecting 3.
                    transfer_pb2.Chunk(
                        transfer_id=3, offset=1, data=b'456', remaining_bytes=3
                    ),
                ),
                (
                    transfer_pb2.Chunk(
                        transfer_id=3, offset=3, data=b'456', remaining_bytes=3
                    ),
                    transfer_pb2.Chunk(
                        transfer_id=3, offset=6, data=b'789', remaining_bytes=0
                    ),
                ),
            ),
        )

        data = manager.read(3)
        self.assertEqual(data, b'123456789')

        # Two transfer parameter requests should have been sent.
        self.assertEqual(len(self._sent_chunks), 3)
        self.assertTrue(self._sent_chunks[-1].HasField('status'))
        self.assertEqual(self._sent_chunks[-1].status, 0)

    def test_read_transfer_recovery_sends_parameters_on_retry(self) -> None:
        """Server sends the same chunk twice (retry) in a read transfer."""
        manager = pw_transfer.Manager(
            self._service, default_response_timeout_s=DEFAULT_TIMEOUT_S
        )

        self._enqueue_server_responses(
            _Method.READ,
            (
                (
                    # Bad offset, enter recovery state. Only one parameters
                    # chunk should be sent.
                    transfer_pb2.Chunk(
                        transfer_id=3, offset=1, data=b'234', remaining_bytes=5
                    ),
                    transfer_pb2.Chunk(
                        transfer_id=3, offset=4, data=b'567', remaining_bytes=2
                    ),
                    transfer_pb2.Chunk(
                        transfer_id=3, offset=7, data=b'8', remaining_bytes=1
                    ),
                ),
                (
                    # Only one parameters chunk should be sent after the server
                    # retries the same offset twice.
                    transfer_pb2.Chunk(
                        transfer_id=3, offset=1, data=b'234', remaining_bytes=5
                    ),
                    transfer_pb2.Chunk(
                        transfer_id=3, offset=4, data=b'567', remaining_bytes=2
                    ),
                    transfer_pb2.Chunk(
                        transfer_id=3, offset=7, data=b'8', remaining_bytes=1
                    ),
                    transfer_pb2.Chunk(
                        transfer_id=3, offset=7, data=b'8', remaining_bytes=1
                    ),
                ),
                (
                    transfer_pb2.Chunk(
                        transfer_id=3,
                        offset=0,
                        data=b'123456789',
                        remaining_bytes=0,
                    ),
                ),
            ),
        )

        data = manager.read(3)
        self.assertEqual(data, b'123456789')

        self.assertEqual(len(self._sent_chunks), 4)
        self.assertEqual(
            self._sent_chunks[0].type, transfer_pb2.Chunk.Type.START
        )
        self.assertEqual(self._sent_chunks[0].offset, 0)
        self.assertEqual(
            self._sent_chunks[1].type,
            transfer_pb2.Chunk.Type.PARAMETERS_RETRANSMIT,
        )
        self.assertEqual(self._sent_chunks[1].offset, 0)
        self.assertEqual(
            self._sent_chunks[2].type,
            transfer_pb2.Chunk.Type.PARAMETERS_RETRANSMIT,
        )
        self.assertEqual(self._sent_chunks[2].offset, 0)
        self.assertEqual(
            self._sent_chunks[3].type, transfer_pb2.Chunk.Type.COMPLETION
        )

    def test_read_transfer_retry_timeout(self) -> None:
        """Server doesn't respond to read transfer parameters."""
        manager = pw_transfer.Manager(
            self._service, default_response_timeout_s=DEFAULT_TIMEOUT_S
        )

        self._enqueue_server_responses(
            _Method.READ,
            (
                (),  # Send nothing in response to the initial parameters.
                (
                    transfer_pb2.Chunk(
                        transfer_id=3, offset=0, data=b'xyz', remaining_bytes=0
                    ),
                ),
            ),
        )

        data = manager.read(3)
        self.assertEqual(data, b'xyz')

        # Two transfer parameter requests should have been sent.
        self.assertEqual(len(self._sent_chunks), 3)
        self.assertTrue(self._sent_chunks[-1].HasField('status'))
        self.assertEqual(self._sent_chunks[-1].status, 0)

    def test_read_transfer_lifetime_retries(self) -> None:
        """Server doesn't respond several times during the transfer."""
        manager = pw_transfer.Manager(
            self._service,
            default_response_timeout_s=DEFAULT_TIMEOUT_S,
            max_retries=2**32 - 1,
            max_lifetime_retries=4,
        )

        self._enqueue_server_responses(
            _Method.READ,
            (
                (),  # Retry 1
                (),  # Retry 2
                (
                    transfer_pb2.Chunk(  # Expected chunk.
                        transfer_id=43, offset=0, data=b'xyz'
                    ),
                ),
                # Don't send anything else. The maximum lifetime retry count
                # should be hit.
            ),
        )

        with self.assertRaises(pw_transfer.Error) as context:
            manager.read(43)

        self.assertEqual(len(self._sent_chunks), 5)

        exception = context.exception
        self.assertEqual(exception.resource_id, 43)
        self.assertEqual(exception.status, Status.DEADLINE_EXCEEDED)

    def test_read_transfer_timeout(self) -> None:
        manager = pw_transfer.Manager(
            self._service, default_response_timeout_s=DEFAULT_TIMEOUT_S
        )

        with self.assertRaises(pw_transfer.Error) as context:
            manager.read(27)

        exception = context.exception
        self.assertEqual(exception.resource_id, 27)
        self.assertEqual(exception.status, Status.DEADLINE_EXCEEDED)

        # The client should have sent four transfer parameters requests: one
        # initial, and three retries.
        self.assertEqual(len(self._sent_chunks), 4)

    def test_read_transfer_error(self) -> None:
        manager = pw_transfer.Manager(
            self._service, default_response_timeout_s=DEFAULT_TIMEOUT_S
        )

        self._enqueue_server_responses(
            _Method.READ,
            (
                (
                    transfer_pb2.Chunk(
                        transfer_id=31, status=Status.NOT_FOUND.value
                    ),
                ),
            ),
        )

        with self.assertRaises(pw_transfer.Error) as context:
            manager.read(31)

        exception = context.exception
        self.assertEqual(exception.resource_id, 31)
        self.assertEqual(exception.status, Status.NOT_FOUND)

    def test_read_transfer_server_error(self) -> None:
        manager = pw_transfer.Manager(
            self._service, default_response_timeout_s=DEFAULT_TIMEOUT_S
        )

        self._enqueue_server_error(_Method.READ, Status.NOT_FOUND)

        with self.assertRaises(pw_transfer.Error) as context:
            manager.read(31)

        exception = context.exception
        self.assertEqual(exception.resource_id, 31)
        self.assertEqual(exception.status, Status.INTERNAL)

    def test_write_transfer_basic(self) -> None:
        manager = pw_transfer.Manager(
            self._service, default_response_timeout_s=DEFAULT_TIMEOUT_S
        )

        self._enqueue_server_responses(
            _Method.WRITE,
            (
                (
                    transfer_pb2.Chunk(
                        transfer_id=4,
                        offset=0,
                        pending_bytes=32,
                        max_chunk_size_bytes=8,
                    ),
                ),
                (transfer_pb2.Chunk(transfer_id=4, status=Status.OK.value),),
            ),
        )

        manager.write(4, b'hello')
        self.assertEqual(len(self._sent_chunks), 2)
        self.assertEqual(self._received_data(), b'hello')

    def test_write_transfer_max_chunk_size(self) -> None:
        manager = pw_transfer.Manager(
            self._service, default_response_timeout_s=DEFAULT_TIMEOUT_S
        )

        self._enqueue_server_responses(
            _Method.WRITE,
            (
                (
                    transfer_pb2.Chunk(
                        transfer_id=4,
                        offset=0,
                        pending_bytes=32,
                        max_chunk_size_bytes=8,
                    ),
                ),
                (),
                (transfer_pb2.Chunk(transfer_id=4, status=Status.OK.value),),
            ),
        )

        manager.write(4, b'hello world')
        self.assertEqual(len(self._sent_chunks), 3)
        self.assertEqual(self._received_data(), b'hello world')
        self.assertEqual(self._sent_chunks[1].data, b'hello wo')
        self.assertEqual(self._sent_chunks[2].data, b'rld')

    def test_write_transfer_multiple_parameters(self) -> None:
        manager = pw_transfer.Manager(
            self._service, default_response_timeout_s=DEFAULT_TIMEOUT_S
        )

        self._enqueue_server_responses(
            _Method.WRITE,
            (
                (
                    transfer_pb2.Chunk(
                        transfer_id=4,
                        offset=0,
                        pending_bytes=8,
                        max_chunk_size_bytes=8,
                    ),
                ),
                (
                    transfer_pb2.Chunk(
                        transfer_id=4,
                        offset=8,
                        pending_bytes=8,
                        max_chunk_size_bytes=8,
                    ),
                ),
                (transfer_pb2.Chunk(transfer_id=4, status=Status.OK.value),),
            ),
        )

        manager.write(4, b'data to write')
        self.assertEqual(len(self._sent_chunks), 3)
        self.assertEqual(self._received_data(), b'data to write')
        self.assertEqual(self._sent_chunks[1].data, b'data to ')
        self.assertEqual(self._sent_chunks[2].data, b'write')

    def test_write_transfer_progress_callback(self) -> None:
        manager = pw_transfer.Manager(
            self._service, default_response_timeout_s=DEFAULT_TIMEOUT_S
        )

        self._enqueue_server_responses(
            _Method.WRITE,
            (
                (
                    transfer_pb2.Chunk(
                        transfer_id=4,
                        offset=0,
                        pending_bytes=8,
                        max_chunk_size_bytes=8,
                    ),
                ),
                (
                    transfer_pb2.Chunk(
                        transfer_id=4,
                        offset=8,
                        pending_bytes=8,
                        max_chunk_size_bytes=8,
                    ),
                ),
                (transfer_pb2.Chunk(transfer_id=4, status=Status.OK.value),),
            ),
        )

        progress: List[pw_transfer.ProgressStats] = []

        manager.write(4, b'data to write', progress.append)
        self.assertEqual(len(self._sent_chunks), 3)
        self.assertEqual(self._received_data(), b'data to write')
        self.assertEqual(self._sent_chunks[1].data, b'data to ')
        self.assertEqual(self._sent_chunks[2].data, b'write')
        self.assertEqual(
            progress,
            [
                pw_transfer.ProgressStats(8, 0, 13),
                pw_transfer.ProgressStats(13, 8, 13),
                pw_transfer.ProgressStats(13, 13, 13),
            ],
        )

    def test_write_transfer_rewind(self) -> None:
        """Write transfer in which the server re-requests an earlier offset."""
        manager = pw_transfer.Manager(
            self._service, default_response_timeout_s=DEFAULT_TIMEOUT_S
        )

        self._enqueue_server_responses(
            _Method.WRITE,
            (
                (
                    transfer_pb2.Chunk(
                        transfer_id=4,
                        offset=0,
                        pending_bytes=8,
                        max_chunk_size_bytes=8,
                    ),
                ),
                (
                    transfer_pb2.Chunk(
                        transfer_id=4,
                        offset=8,
                        pending_bytes=8,
                        max_chunk_size_bytes=8,
                    ),
                ),
                (
                    transfer_pb2.Chunk(
                        transfer_id=4,
                        offset=4,  # rewind
                        pending_bytes=8,
                        max_chunk_size_bytes=8,
                    ),
                ),
                (
                    transfer_pb2.Chunk(
                        transfer_id=4,
                        offset=12,
                        pending_bytes=16,  # update max size
                        max_chunk_size_bytes=16,
                    ),
                ),
                (transfer_pb2.Chunk(transfer_id=4, status=Status.OK.value),),
            ),
        )

        manager.write(4, b'pigweed data transfer')
        self.assertEqual(len(self._sent_chunks), 5)
        self.assertEqual(self._sent_chunks[1].data, b'pigweed ')
        self.assertEqual(self._sent_chunks[2].data, b'data tra')
        self.assertEqual(self._sent_chunks[3].data, b'eed data')
        self.assertEqual(self._sent_chunks[4].data, b' transfer')

    def test_write_transfer_bad_offset(self) -> None:
        manager = pw_transfer.Manager(
            self._service, default_response_timeout_s=DEFAULT_TIMEOUT_S
        )

        self._enqueue_server_responses(
            _Method.WRITE,
            (
                (
                    transfer_pb2.Chunk(
                        transfer_id=4,
                        offset=0,
                        pending_bytes=8,
                        max_chunk_size_bytes=8,
                    ),
                ),
                (
                    transfer_pb2.Chunk(
                        transfer_id=4,
                        offset=100,  # larger offset than data
                        pending_bytes=8,
                        max_chunk_size_bytes=8,
                    ),
                ),
                (transfer_pb2.Chunk(transfer_id=4, status=Status.OK.value),),
            ),
        )

        with self.assertRaises(pw_transfer.Error) as context:
            manager.write(4, b'small data')

        exception = context.exception
        self.assertEqual(exception.resource_id, 4)
        self.assertEqual(exception.status, Status.OUT_OF_RANGE)

    def test_write_transfer_error(self) -> None:
        manager = pw_transfer.Manager(
            self._service, default_response_timeout_s=DEFAULT_TIMEOUT_S
        )

        self._enqueue_server_responses(
            _Method.WRITE,
            (
                (
                    transfer_pb2.Chunk(
                        transfer_id=21, status=Status.UNAVAILABLE.value
                    ),
                ),
            ),
        )

        with self.assertRaises(pw_transfer.Error) as context:
            manager.write(21, b'no write')

        exception = context.exception
        self.assertEqual(exception.resource_id, 21)
        self.assertEqual(exception.status, Status.UNAVAILABLE)

    def test_write_transfer_server_error(self) -> None:
        manager = pw_transfer.Manager(
            self._service, default_response_timeout_s=DEFAULT_TIMEOUT_S
        )

        self._enqueue_server_error(_Method.WRITE, Status.NOT_FOUND)

        with self.assertRaises(pw_transfer.Error) as context:
            manager.write(21, b'server error')

        exception = context.exception
        self.assertEqual(exception.resource_id, 21)
        self.assertEqual(exception.status, Status.INTERNAL)

    def test_write_transfer_timeout_after_initial_chunk(self) -> None:
        manager = pw_transfer.Manager(
            self._service,
            default_response_timeout_s=0.001,
            max_retries=2,
            default_protocol_version=ProtocolVersion.LEGACY,
        )

        with self.assertRaises(pw_transfer.Error) as context:
            manager.write(22, b'no server response!')

        self.assertEqual(
            self._sent_chunks,
            [
                transfer_pb2.Chunk(
                    transfer_id=22,
                    resource_id=22,
                    type=transfer_pb2.Chunk.Type.START,
                ),  # initial chunk
                transfer_pb2.Chunk(
                    transfer_id=22,
                    resource_id=22,
                    type=transfer_pb2.Chunk.Type.START,
                ),  # retry 1
                transfer_pb2.Chunk(
                    transfer_id=22,
                    resource_id=22,
                    type=transfer_pb2.Chunk.Type.START,
                ),  # retry 2
            ],
        )

        exception = context.exception
        self.assertEqual(exception.resource_id, 22)
        self.assertEqual(exception.status, Status.DEADLINE_EXCEEDED)

    def test_write_transfer_timeout_after_intermediate_chunk(self) -> None:
        """Tests write transfers that timeout after the initial chunk."""
        manager = pw_transfer.Manager(
            self._service,
            default_response_timeout_s=DEFAULT_TIMEOUT_S,
            max_retries=2,
            default_protocol_version=ProtocolVersion.LEGACY,
        )

        self._enqueue_server_responses(
            _Method.WRITE,
            [
                [
                    transfer_pb2.Chunk(
                        transfer_id=22, pending_bytes=10, max_chunk_size_bytes=5
                    )
                ]
            ],
        )

        with self.assertRaises(pw_transfer.Error) as context:
            manager.write(22, b'0123456789')

        last_data_chunk = transfer_pb2.Chunk(
            transfer_id=22,
            data=b'56789',
            offset=5,
            remaining_bytes=0,
            type=transfer_pb2.Chunk.Type.DATA,
        )

        self.assertEqual(
            self._sent_chunks,
            [
                transfer_pb2.Chunk(
                    transfer_id=22,
                    resource_id=22,
                    type=transfer_pb2.Chunk.Type.START,
                ),
                transfer_pb2.Chunk(
                    transfer_id=22,
                    data=b'01234',
                    type=transfer_pb2.Chunk.Type.DATA,
                ),
                last_data_chunk,  # last chunk
                last_data_chunk,  # retry 1
                last_data_chunk,  # retry 2
            ],
        )

        exception = context.exception
        self.assertEqual(exception.resource_id, 22)
        self.assertEqual(exception.status, Status.DEADLINE_EXCEEDED)

    def test_write_zero_pending_bytes_is_internal_error(self) -> None:
        manager = pw_transfer.Manager(
            self._service, default_response_timeout_s=DEFAULT_TIMEOUT_S
        )

        self._enqueue_server_responses(
            _Method.WRITE,
            ((transfer_pb2.Chunk(transfer_id=23, pending_bytes=0),),),
        )

        with self.assertRaises(pw_transfer.Error) as context:
            manager.write(23, b'no write')

        exception = context.exception
        self.assertEqual(exception.resource_id, 23)
        self.assertEqual(exception.status, Status.INTERNAL)

    def test_v2_read_transfer_basic(self) -> None:
        """Tests a simple protocol version 2 read transfer."""
        manager = pw_transfer.Manager(
            self._service,
            default_response_timeout_s=DEFAULT_TIMEOUT_S,
            default_protocol_version=ProtocolVersion.VERSION_TWO,
        )

        self._enqueue_server_responses(
            _Method.READ,
            (
                (
                    transfer_pb2.Chunk(
                        resource_id=39,
                        session_id=280,
                        type=transfer_pb2.Chunk.Type.START_ACK,
                        protocol_version=ProtocolVersion.VERSION_TWO.value,
                    ),
                ),
                (
                    transfer_pb2.Chunk(
                        session_id=280,
                        type=transfer_pb2.Chunk.Type.DATA,
                        offset=0,
                        data=b'version two',
                        remaining_bytes=0,
                    ),
                ),
                (
                    transfer_pb2.Chunk(
                        session_id=280,
                        type=transfer_pb2.Chunk.Type.COMPLETION_ACK,
                    ),
                ),
            ),
        )

        data = manager.read(39)

        self.assertEqual(
            self._sent_chunks,
            [
                transfer_pb2.Chunk(
                    transfer_id=39,
                    resource_id=39,
                    pending_bytes=8192,
                    max_chunk_size_bytes=1024,
                    window_end_offset=8192,
                    type=transfer_pb2.Chunk.Type.START,
                    protocol_version=ProtocolVersion.VERSION_TWO.value,
                ),
                transfer_pb2.Chunk(
                    session_id=280,
                    type=transfer_pb2.Chunk.Type.START_ACK_CONFIRMATION,
                    max_chunk_size_bytes=1024,
                    window_end_offset=8192,
                    # pending_bytes should no longer exist as server and client
                    # have agreed on v2.
                    protocol_version=ProtocolVersion.VERSION_TWO.value,
                ),
                transfer_pb2.Chunk(
                    session_id=280,
                    type=transfer_pb2.Chunk.Type.COMPLETION,
                    status=Status.OK.value,
                ),
            ],
        )

        self.assertEqual(data, b'version two')

    def test_v2_read_transfer_legacy_fallback(self) -> None:
        """Tests a v2 read transfer when the server only supports legacy."""
        manager = pw_transfer.Manager(
            self._service,
            default_response_timeout_s=DEFAULT_TIMEOUT_S,
            default_protocol_version=ProtocolVersion.VERSION_TWO,
        )

        # Respond to the START chunk with a legacy data transfer chunk instead
        # of a START_ACK.
        self._enqueue_server_responses(
            _Method.READ,
            (
                (
                    transfer_pb2.Chunk(
                        transfer_id=40,
                        type=transfer_pb2.Chunk.Type.DATA,
                        offset=0,
                        data=b'sorry, legacy only',
                        remaining_bytes=0,
                    ),
                ),
            ),
        )

        data = manager.read(40)

        self.assertEqual(
            self._sent_chunks,
            [
                transfer_pb2.Chunk(
                    transfer_id=40,
                    resource_id=40,
                    pending_bytes=8192,
                    max_chunk_size_bytes=1024,
                    window_end_offset=8192,
                    type=transfer_pb2.Chunk.Type.START,
                    protocol_version=ProtocolVersion.VERSION_TWO.value,
                ),
                transfer_pb2.Chunk(
                    transfer_id=40,
                    type=transfer_pb2.Chunk.Type.COMPLETION,
                    status=Status.OK.value,
                ),
            ],
        )

        self.assertEqual(data, b'sorry, legacy only')

    def test_v2_write_transfer_basic(self) -> None:
        """Tests a simple protocol version 2 write transfer."""
        manager = pw_transfer.Manager(
            self._service,
            default_response_timeout_s=DEFAULT_TIMEOUT_S,
            default_protocol_version=ProtocolVersion.VERSION_TWO,
        )

        self._enqueue_server_responses(
            _Method.WRITE,
            (
                (
                    transfer_pb2.Chunk(
                        resource_id=72,
                        session_id=880,
                        type=transfer_pb2.Chunk.Type.START_ACK,
                        protocol_version=ProtocolVersion.VERSION_TWO.value,
                    ),
                ),
                (
                    transfer_pb2.Chunk(
                        session_id=880,
                        type=transfer_pb2.Chunk.Type.PARAMETERS_RETRANSMIT,
                        offset=0,
                        window_end_offset=32,
                        max_chunk_size_bytes=8,
                    ),
                ),
                (),  # In response to the first data chunk.
                (
                    transfer_pb2.Chunk(
                        session_id=880,
                        type=transfer_pb2.Chunk.Type.COMPLETION,
                        status=Status.OK.value,
                    ),
                ),
            ),
        )

        manager.write(72, b'write version 2')

        self.assertEqual(
            self._sent_chunks,
            [
                transfer_pb2.Chunk(
                    transfer_id=72,
                    resource_id=72,
                    type=transfer_pb2.Chunk.Type.START,
                    protocol_version=ProtocolVersion.VERSION_TWO.value,
                ),
                transfer_pb2.Chunk(
                    session_id=880,
                    type=transfer_pb2.Chunk.Type.START_ACK_CONFIRMATION,
                    protocol_version=ProtocolVersion.VERSION_TWO.value,
                ),
                transfer_pb2.Chunk(
                    session_id=880,
                    type=transfer_pb2.Chunk.Type.DATA,
                    offset=0,
                    data=b'write ve',
                ),
                transfer_pb2.Chunk(
                    session_id=880,
                    type=transfer_pb2.Chunk.Type.DATA,
                    offset=8,
                    data=b'rsion 2',
                    remaining_bytes=0,
                ),
                transfer_pb2.Chunk(
                    session_id=880, type=transfer_pb2.Chunk.Type.COMPLETION_ACK
                ),
            ],
        )

        self.assertEqual(self._received_data(), b'write version 2')

    def test_v2_write_transfer_legacy_fallback(self) -> None:
        """Tests a v2 write transfer when the server only supports legacy."""
        manager = pw_transfer.Manager(
            self._service,
            default_response_timeout_s=DEFAULT_TIMEOUT_S,
            default_protocol_version=ProtocolVersion.VERSION_TWO,
        )

        self._enqueue_server_responses(
            _Method.WRITE,
            (
                # Send a parameters chunk immediately per the legacy protocol.
                (
                    transfer_pb2.Chunk(
                        transfer_id=76,
                        type=transfer_pb2.Chunk.Type.PARAMETERS_RETRANSMIT,
                        offset=0,
                        pending_bytes=32,
                        window_end_offset=32,
                        max_chunk_size_bytes=8,
                    ),
                ),
                (),  # In response to the first data chunk.
                (
                    transfer_pb2.Chunk(
                        transfer_id=76,
                        type=transfer_pb2.Chunk.Type.COMPLETION,
                        status=Status.OK.value,
                    ),
                ),
            ),
        )

        manager.write(76, b'write v... NOPE')

        self.assertEqual(
            self._sent_chunks,
            [
                transfer_pb2.Chunk(
                    transfer_id=76,
                    resource_id=76,
                    type=transfer_pb2.Chunk.Type.START,
                    protocol_version=ProtocolVersion.VERSION_TWO.value,
                ),
                transfer_pb2.Chunk(
                    transfer_id=76,
                    type=transfer_pb2.Chunk.Type.DATA,
                    offset=0,
                    data=b'write v.',
                ),
                transfer_pb2.Chunk(
                    transfer_id=76,
                    type=transfer_pb2.Chunk.Type.DATA,
                    offset=8,
                    data=b'.. NOPE',
                    remaining_bytes=0,
                ),
            ],
        )

        self.assertEqual(self._received_data(), b'write v... NOPE')

    def test_v2_server_error(self) -> None:
        """Tests a timeout occurring during the opening handshake."""

        manager = pw_transfer.Manager(
            self._service,
            default_response_timeout_s=DEFAULT_TIMEOUT_S,
            default_protocol_version=ProtocolVersion.VERSION_TWO,
        )

        self._enqueue_server_responses(
            _Method.READ,
            (
                (
                    transfer_pb2.Chunk(
                        resource_id=43,
                        session_id=680,
                        type=transfer_pb2.Chunk.Type.START_ACK,
                        protocol_version=ProtocolVersion.VERSION_TWO.value,
                    ),
                ),
                (
                    transfer_pb2.Chunk(
                        session_id=680,
                        type=transfer_pb2.Chunk.Type.COMPLETION,
                        status=Status.DATA_LOSS.value,
                    ),
                ),
            ),
        )

        with self.assertRaises(pw_transfer.Error) as context:
            manager.read(43)

        self.assertEqual(
            self._sent_chunks,
            [
                transfer_pb2.Chunk(
                    transfer_id=43,
                    resource_id=43,
                    pending_bytes=8192,
                    max_chunk_size_bytes=1024,
                    window_end_offset=8192,
                    type=transfer_pb2.Chunk.Type.START,
                    protocol_version=ProtocolVersion.VERSION_TWO.value,
                ),
                transfer_pb2.Chunk(
                    session_id=680,
                    type=transfer_pb2.Chunk.Type.START_ACK_CONFIRMATION,
                    max_chunk_size_bytes=1024,
                    window_end_offset=8192,
                    protocol_version=ProtocolVersion.VERSION_TWO.value,
                ),
                # Client sends a COMPLETION_ACK in response to the server.
                transfer_pb2.Chunk(
                    session_id=680, type=transfer_pb2.Chunk.Type.COMPLETION_ACK
                ),
            ],
        )

        exception = context.exception
        self.assertEqual(exception.resource_id, 43)
        self.assertEqual(exception.status, Status.DATA_LOSS)

    def test_v2_timeout_during_opening_handshake(self) -> None:
        """Tests a timeout occurring during the opening handshake."""
        manager = pw_transfer.Manager(
            self._service,
            default_response_timeout_s=DEFAULT_TIMEOUT_S,
            default_protocol_version=ProtocolVersion.VERSION_TWO,
        )

        # Don't enqueue any server responses.

        with self.assertRaises(pw_transfer.Error) as context:
            manager.read(41)

        start_chunk = transfer_pb2.Chunk(
            transfer_id=41,
            resource_id=41,
            pending_bytes=8192,
            max_chunk_size_bytes=1024,
            window_end_offset=8192,
            type=transfer_pb2.Chunk.Type.START,
            protocol_version=ProtocolVersion.VERSION_TWO.value,
        )

        # The opening chunk should be sent initially, then retried three times.
        self.assertEqual(self._sent_chunks, [start_chunk] * 4)

        exception = context.exception
        self.assertEqual(exception.resource_id, 41)
        self.assertEqual(exception.status, Status.DEADLINE_EXCEEDED)

    def test_v2_timeout_recovery_during_opening_handshake(self) -> None:
        """Tests a timeout during the opening handshake which recovers."""
        manager = pw_transfer.Manager(
            self._service,
            default_response_timeout_s=DEFAULT_TIMEOUT_S,
            default_protocol_version=ProtocolVersion.VERSION_TWO,
        )

        self._enqueue_server_responses(
            _Method.WRITE,
            (
                (
                    transfer_pb2.Chunk(
                        resource_id=73,
                        session_id=101,
                        type=transfer_pb2.Chunk.Type.START_ACK,
                        protocol_version=ProtocolVersion.VERSION_TWO.value,
                    ),
                ),
                (),  # Don't respond to the START_ACK_CONFIRMATION.
                (),  # Don't respond to the first START_ACK_CONFIRMATION retry.
                (
                    transfer_pb2.Chunk(
                        session_id=101,
                        type=transfer_pb2.Chunk.Type.PARAMETERS_RETRANSMIT,
                        offset=0,
                        window_end_offset=32,
                        max_chunk_size_bytes=8,
                    ),
                ),
                (),  # In response to the first data chunk.
                (
                    transfer_pb2.Chunk(
                        session_id=101,
                        type=transfer_pb2.Chunk.Type.COMPLETION,
                        status=Status.OK.value,
                    ),
                ),
            ),
        )

        manager.write(73, b'write timeout 2')

        start_ack_confirmation = transfer_pb2.Chunk(
            session_id=101,
            type=transfer_pb2.Chunk.Type.START_ACK_CONFIRMATION,
            protocol_version=ProtocolVersion.VERSION_TWO.value,
        )

        self.assertEqual(
            self._sent_chunks,
            [
                transfer_pb2.Chunk(
                    transfer_id=73,
                    resource_id=73,
                    type=transfer_pb2.Chunk.Type.START,
                    protocol_version=ProtocolVersion.VERSION_TWO.value,
                ),
                start_ack_confirmation,  # Initial transmission
                start_ack_confirmation,  # Retry 1
                start_ack_confirmation,  # Retry 2
                transfer_pb2.Chunk(
                    session_id=101,
                    type=transfer_pb2.Chunk.Type.DATA,
                    offset=0,
                    data=b'write ti',
                ),
                transfer_pb2.Chunk(
                    session_id=101,
                    type=transfer_pb2.Chunk.Type.DATA,
                    offset=8,
                    data=b'meout 2',
                    remaining_bytes=0,
                ),
                transfer_pb2.Chunk(
                    session_id=101, type=transfer_pb2.Chunk.Type.COMPLETION_ACK
                ),
            ],
        )

        self.assertEqual(self._received_data(), b'write timeout 2')

    def test_v2_closing_handshake_bad_chunk(self) -> None:
        """Tests an unexpected chunk response during the closing handshake."""
        manager = pw_transfer.Manager(
            self._service,
            default_response_timeout_s=DEFAULT_TIMEOUT_S,
            default_protocol_version=ProtocolVersion.VERSION_TWO,
        )

        self._enqueue_server_responses(
            _Method.READ,
            (
                (
                    transfer_pb2.Chunk(
                        resource_id=47,
                        session_id=580,
                        type=transfer_pb2.Chunk.Type.START_ACK,
                        protocol_version=ProtocolVersion.VERSION_TWO.value,
                    ),
                ),
                (
                    transfer_pb2.Chunk(
                        session_id=580,
                        type=transfer_pb2.Chunk.Type.DATA,
                        offset=0,
                        data=b'version two',
                        remaining_bytes=0,
                    ),
                ),
                # In response to the COMPLETION, re-send the last chunk instead
                # of a COMPLETION_ACK.
                (
                    transfer_pb2.Chunk(
                        session_id=580,
                        type=transfer_pb2.Chunk.Type.DATA,
                        offset=0,
                        data=b'version two',
                        remaining_bytes=0,
                    ),
                ),
                (
                    transfer_pb2.Chunk(
                        session_id=580,
                        type=transfer_pb2.Chunk.Type.COMPLETION_ACK,
                    ),
                ),
            ),
        )

        data = manager.read(47)

        self.assertEqual(
            self._sent_chunks,
            [
                transfer_pb2.Chunk(
                    transfer_id=47,
                    resource_id=47,
                    pending_bytes=8192,
                    max_chunk_size_bytes=1024,
                    window_end_offset=8192,
                    type=transfer_pb2.Chunk.Type.START,
                    protocol_version=ProtocolVersion.VERSION_TWO.value,
                ),
                transfer_pb2.Chunk(
                    session_id=580,
                    type=transfer_pb2.Chunk.Type.START_ACK_CONFIRMATION,
                    max_chunk_size_bytes=1024,
                    window_end_offset=8192,
                    protocol_version=ProtocolVersion.VERSION_TWO.value,
                ),
                transfer_pb2.Chunk(
                    session_id=580,
                    type=transfer_pb2.Chunk.Type.COMPLETION,
                    status=Status.OK.value,
                ),
                # Completion should be re-sent following the repeated chunk.
                transfer_pb2.Chunk(
                    session_id=580,
                    type=transfer_pb2.Chunk.Type.COMPLETION,
                    status=Status.OK.value,
                ),
            ],
        )

        self.assertEqual(data, b'version two')

    def test_v2_timeout_during_closing_handshake(self) -> None:
        """Tests a timeout occurring during the closing handshake."""
        manager = pw_transfer.Manager(
            self._service,
            default_response_timeout_s=DEFAULT_TIMEOUT_S,
            default_protocol_version=ProtocolVersion.VERSION_TWO,
        )

        self._enqueue_server_responses(
            _Method.READ,
            (
                (
                    transfer_pb2.Chunk(
                        resource_id=47,
                        session_id=980,
                        type=transfer_pb2.Chunk.Type.START_ACK,
                        protocol_version=ProtocolVersion.VERSION_TWO.value,
                    ),
                ),
                (
                    transfer_pb2.Chunk(
                        session_id=980,
                        type=transfer_pb2.Chunk.Type.DATA,
                        offset=0,
                        data=b'dropped completion',
                        remaining_bytes=0,
                    ),
                ),
                # Never send the expected COMPLETION_ACK chunk.
            ),
        )

        data = manager.read(47)

        self.assertEqual(
            self._sent_chunks,
            [
                transfer_pb2.Chunk(
                    transfer_id=47,
                    resource_id=47,
                    pending_bytes=8192,
                    max_chunk_size_bytes=1024,
                    window_end_offset=8192,
                    type=transfer_pb2.Chunk.Type.START,
                    protocol_version=ProtocolVersion.VERSION_TWO.value,
                ),
                transfer_pb2.Chunk(
                    session_id=980,
                    type=transfer_pb2.Chunk.Type.START_ACK_CONFIRMATION,
                    max_chunk_size_bytes=1024,
                    window_end_offset=8192,
                    protocol_version=ProtocolVersion.VERSION_TWO.value,
                ),
                transfer_pb2.Chunk(
                    session_id=980,
                    type=transfer_pb2.Chunk.Type.COMPLETION,
                    status=Status.OK.value,
                ),
                # The completion should be retried per the usual retry flow.
                transfer_pb2.Chunk(
                    session_id=980,
                    type=transfer_pb2.Chunk.Type.COMPLETION,
                    status=Status.OK.value,
                ),
                transfer_pb2.Chunk(
                    session_id=980,
                    type=transfer_pb2.Chunk.Type.COMPLETION,
                    status=Status.OK.value,
                ),
                transfer_pb2.Chunk(
                    session_id=980,
                    type=transfer_pb2.Chunk.Type.COMPLETION,
                    status=Status.OK.value,
                ),
            ],
        )

        # Despite timing out following several retries, the transfer should
        # still conclude successfully, as failing to receive a COMPLETION_ACK
        # is not fatal.
        self.assertEqual(data, b'dropped completion')


class ProgressStatsTest(unittest.TestCase):
    def test_received_percent_known_total(self) -> None:
        self.assertEqual(
            pw_transfer.ProgressStats(75, 0, 100).percent_received(), 0.0
        )
        self.assertEqual(
            pw_transfer.ProgressStats(75, 50, 100).percent_received(), 50.0
        )
        self.assertEqual(
            pw_transfer.ProgressStats(100, 100, 100).percent_received(), 100.0
        )

    def test_received_percent_unknown_total(self) -> None:
        self.assertTrue(
            math.isnan(
                pw_transfer.ProgressStats(75, 50, None).percent_received()
            )
        )
        self.assertTrue(
            math.isnan(
                pw_transfer.ProgressStats(100, 100, None).percent_received()
            )
        )

    def test_str_known_total(self) -> None:
        stats = str(pw_transfer.ProgressStats(75, 50, 100))
        self.assertIn('75', stats)
        self.assertIn('50', stats)
        self.assertIn('100', stats)

    def test_str_unknown_total(self) -> None:
        stats = str(pw_transfer.ProgressStats(75, 50, None))
        self.assertIn('75', stats)
        self.assertIn('50', stats)
        self.assertIn('unknown', stats)


if __name__ == '__main__':
    unittest.main()
