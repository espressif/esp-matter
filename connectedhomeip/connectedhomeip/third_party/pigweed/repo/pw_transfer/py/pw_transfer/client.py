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
"""Client for the pw_transfer service, which transmits data over pw_rpc."""

import asyncio
import logging
import threading
from typing import Any, Dict, Optional, Union

from pw_rpc.callback_client import BidirectionalStreamingCall
from pw_status import Status

from pw_transfer.transfer import (
    ProgressCallback,
    ProtocolVersion,
    ReadTransfer,
    Transfer,
    WriteTransfer,
)
from pw_transfer.chunk import Chunk

try:
    from pw_transfer import transfer_pb2
except ImportError:
    # For the bazel build, which puts generated protos in a different location.
    from pigweed.pw_transfer import transfer_pb2  # type: ignore

_LOG = logging.getLogger(__package__)

_TransferDict = Dict[int, Transfer]


class Manager:  # pylint: disable=too-many-instance-attributes
    """A manager for transmitting data through an RPC TransferService.

    This should be initialized with an active Manager over an RPC channel. Only
    one instance of this class should exist for a configured RPC TransferService
    -- the Manager supports multiple simultaneous transfers.

    When created, a Manager starts a separate thread in which transfer
    communications and events are handled.
    """

    def __init__(
        self,
        rpc_transfer_service,
        *,
        default_response_timeout_s: float = 2.0,
        initial_response_timeout_s: float = 4.0,
        max_retries: int = 3,
        max_lifetime_retries: int = 1500,
        default_protocol_version=ProtocolVersion.LATEST,
    ):
        """Initializes a Manager on top of a TransferService.

        Args:
          rpc_transfer_service: the pw_rpc transfer service client
          default_response_timeout_s: max time to wait between receiving packets
          initial_response_timeout_s: timeout for the first packet; may be
              longer to account for transfer handler initialization
          max_retires: number of times to retry a single package after a timeout
          max_lifetime_retires: Cumulative maximum number of times to retry over
              the course of the transfer before giving up.
        """
        self._service: Any = rpc_transfer_service
        self._default_response_timeout_s = default_response_timeout_s
        self._initial_response_timeout_s = initial_response_timeout_s
        self.max_retries = max_retries
        self.max_lifetime_retries = max_lifetime_retries
        self._default_protocol_version = default_protocol_version

        # Ongoing transfers in the service by resource ID.
        self._read_transfers: _TransferDict = {}
        self._write_transfers: _TransferDict = {}

        # RPC streams for read and write transfers. These are shareable by
        # multiple transfers of the same type.
        self._read_stream: Optional[BidirectionalStreamingCall] = None
        self._write_stream: Optional[BidirectionalStreamingCall] = None

        self._loop = asyncio.new_event_loop()
        # Set the event loop for the current thread.
        asyncio.set_event_loop(self._loop)

        # Queues are used for communication between the Manager context and the
        # dedicated asyncio transfer thread.
        self._new_transfer_queue: asyncio.Queue = asyncio.Queue()
        self._read_chunk_queue: asyncio.Queue = asyncio.Queue()
        self._write_chunk_queue: asyncio.Queue = asyncio.Queue()
        self._quit_event = asyncio.Event()

        self._thread = threading.Thread(
            target=self._start_event_loop_thread, daemon=True
        )

        self._thread.start()

    def __del__(self):
        # Notify the thread that the transfer manager is being destroyed and
        # wait for it to exit.
        if self._thread.is_alive():
            self._loop.call_soon_threadsafe(self._quit_event.set)
            self._thread.join()

    def read(
        self,
        resource_id: int,
        progress_callback: Optional[ProgressCallback] = None,
        protocol_version: Optional[ProtocolVersion] = None,
    ) -> bytes:
        """Receives ("downloads") data from the server.

        Args:
          resource_id: ID of the resource from which to read.
          progress_callback: Optional callback periodically invoked throughout
              the transfer with the transfer state. Can be used to provide user-
              facing status updates such as progress bars.

        Raises:
          Error: the transfer failed to complete
        """

        if resource_id in self._read_transfers:
            raise ValueError(
                f'Read transfer for resource {resource_id} already exists'
            )

        if protocol_version is None:
            protocol_version = self._default_protocol_version

        transfer = ReadTransfer(
            resource_id,
            self._send_read_chunk,
            self._end_read_transfer,
            self._default_response_timeout_s,
            self._initial_response_timeout_s,
            self.max_retries,
            self.max_lifetime_retries,
            protocol_version,
            progress_callback=progress_callback,
        )
        self._start_read_transfer(transfer)

        transfer.done.wait()

        if not transfer.status.ok():
            raise Error(transfer.resource_id, transfer.status)

        return transfer.data

    def write(
        self,
        resource_id: int,
        data: Union[bytes, str],
        progress_callback: Optional[ProgressCallback] = None,
        protocol_version: Optional[ProtocolVersion] = None,
    ) -> None:
        """Transmits ("uploads") data to the server.

        Args:
          resource_id: ID of the resource to which to write.
          data: Data to send to the server.
          progress_callback: Optional callback periodically invoked throughout
              the transfer with the transfer state. Can be used to provide user-
              facing status updates such as progress bars.

        Raises:
          Error: the transfer failed to complete
        """

        if isinstance(data, str):
            data = data.encode()

        if resource_id in self._write_transfers:
            raise ValueError(
                f'Write transfer for resource {resource_id} already exists'
            )

        if protocol_version is None:
            protocol_version = self._default_protocol_version

        transfer = WriteTransfer(
            resource_id,
            data,
            self._send_write_chunk,
            self._end_write_transfer,
            self._default_response_timeout_s,
            self._initial_response_timeout_s,
            self.max_retries,
            self.max_lifetime_retries,
            protocol_version,
            progress_callback=progress_callback,
        )
        self._start_write_transfer(transfer)

        transfer.done.wait()

        if not transfer.status.ok():
            raise Error(transfer.resource_id, transfer.status)

    def _send_read_chunk(self, chunk: Chunk) -> None:
        assert self._read_stream is not None
        self._read_stream.send(chunk.to_message())

    def _send_write_chunk(self, chunk: Chunk) -> None:
        assert self._write_stream is not None
        self._write_stream.send(chunk.to_message())

    def _start_event_loop_thread(self):
        """Entry point for event loop thread that starts an asyncio context."""
        asyncio.set_event_loop(self._loop)

        # Recreate the async communication channels in the context of the
        # running event loop.
        self._new_transfer_queue = asyncio.Queue()
        self._read_chunk_queue = asyncio.Queue()
        self._write_chunk_queue = asyncio.Queue()
        self._quit_event = asyncio.Event()

        self._loop.create_task(self._transfer_event_loop())
        self._loop.run_forever()

    async def _transfer_event_loop(self):
        """Main async event loop."""
        exit_thread = self._loop.create_task(self._quit_event.wait())
        new_transfer = self._loop.create_task(self._new_transfer_queue.get())
        read_chunk = self._loop.create_task(self._read_chunk_queue.get())
        write_chunk = self._loop.create_task(self._write_chunk_queue.get())

        while not self._quit_event.is_set():
            # Perform a select(2)-like wait for one of several events to occur.
            done, _ = await asyncio.wait(
                (exit_thread, new_transfer, read_chunk, write_chunk),
                return_when=asyncio.FIRST_COMPLETED,
            )

            if exit_thread in done:
                break

            if new_transfer in done:
                await new_transfer.result().begin()
                new_transfer = self._loop.create_task(
                    self._new_transfer_queue.get()
                )

            if read_chunk in done:
                self._loop.create_task(
                    self._handle_chunk(
                        self._read_transfers, read_chunk.result()
                    )
                )
                read_chunk = self._loop.create_task(
                    self._read_chunk_queue.get()
                )

            if write_chunk in done:
                self._loop.create_task(
                    self._handle_chunk(
                        self._write_transfers, write_chunk.result()
                    )
                )
                write_chunk = self._loop.create_task(
                    self._write_chunk_queue.get()
                )

        self._loop.stop()

    @staticmethod
    async def _handle_chunk(
        transfers: _TransferDict, message: transfer_pb2.Chunk
    ) -> None:
        """Processes an incoming chunk from a stream.

        The chunk is dispatched to an active transfer based on its ID. If the
        transfer indicates that it is complete, the provided completion callback
        is invoked.
        """

        chunk = Chunk.from_message(message)

        # Find a transfer for the chunk in the list of active transfers.
        try:
            if chunk.resource_id is not None:
                # Prioritize a resource_id if one is set.
                transfer = transfers[chunk.resource_id]
            else:
                # Otherwise, match against either resource or session ID.
                transfer = next(
                    t for t in transfers.values() if t.id == chunk.id()
                )
        except (KeyError, StopIteration):
            _LOG.error(
                'TransferManager received chunk for unknown transfer %d',
                chunk.id(),
            )
            # TODO(frolv): What should be done here, if anything?
            return

        await transfer.handle_chunk(chunk)

    def _open_read_stream(self) -> None:
        self._read_stream = self._service.Read.invoke(
            lambda _, chunk: self._loop.call_soon_threadsafe(
                self._read_chunk_queue.put_nowait, chunk
            ),
            on_error=lambda _, status: self._on_read_error(status),
        )

    def _on_read_error(self, status: Status) -> None:
        """Callback for an RPC error in the read stream."""

        if status is Status.FAILED_PRECONDITION:
            # FAILED_PRECONDITION indicates that the stream packet was not
            # recognized as the stream is not open. This could occur if the
            # server resets during an active transfer. Re-open the stream to
            # allow pending transfers to continue.
            self._open_read_stream()
        else:
            # Other errors are unrecoverable. Clear the stream and cancel any
            # pending transfers with an INTERNAL status as this is a system
            # error.
            self._read_stream = None

            for transfer in self._read_transfers.values():
                transfer.finish(Status.INTERNAL, skip_callback=True)
            self._read_transfers.clear()

            _LOG.error('Read stream shut down: %s', status)

    def _open_write_stream(self) -> None:
        self._write_stream = self._service.Write.invoke(
            lambda _, chunk: self._loop.call_soon_threadsafe(
                self._write_chunk_queue.put_nowait, chunk
            ),
            on_error=lambda _, status: self._on_write_error(status),
        )

    def _on_write_error(self, status: Status) -> None:
        """Callback for an RPC error in the write stream."""

        if status is Status.FAILED_PRECONDITION:
            # FAILED_PRECONDITION indicates that the stream packet was not
            # recognized as the stream is not open. This could occur if the
            # server resets during an active transfer. Re-open the stream to
            # allow pending transfers to continue.
            self._open_write_stream()
        else:
            # Other errors are unrecoverable. Clear the stream and cancel any
            # pending transfers with an INTERNAL status as this is a system
            # error.
            self._write_stream = None

            for transfer in self._write_transfers.values():
                transfer.finish(Status.INTERNAL, skip_callback=True)
            self._write_transfers.clear()

            _LOG.error('Write stream shut down: %s', status)

    def _start_read_transfer(self, transfer: Transfer) -> None:
        """Begins a new read transfer, opening the stream if it isn't."""

        self._read_transfers[transfer.resource_id] = transfer

        if not self._read_stream:
            self._open_read_stream()

        _LOG.debug('Starting new read transfer %d', transfer.id)
        self._loop.call_soon_threadsafe(
            self._new_transfer_queue.put_nowait, transfer
        )

    def _end_read_transfer(self, transfer: Transfer) -> None:
        """Completes a read transfer."""
        del self._read_transfers[transfer.resource_id]

        if not transfer.status.ok():
            _LOG.error(
                'Read transfer %d terminated with status %s',
                transfer.id,
                transfer.status,
            )

        # TODO(frolv): This doesn't seem to work. Investigate why.
        # If no more transfers are using the read stream, close it.
        # if not self._read_transfers and self._read_stream:
        #     self._read_stream.cancel()
        #     self._read_stream = None

    def _start_write_transfer(self, transfer: Transfer) -> None:
        """Begins a new write transfer, opening the stream if it isn't."""

        self._write_transfers[transfer.resource_id] = transfer

        if not self._write_stream:
            self._open_write_stream()

        _LOG.debug('Starting new write transfer %d', transfer.id)
        self._loop.call_soon_threadsafe(
            self._new_transfer_queue.put_nowait, transfer
        )

    def _end_write_transfer(self, transfer: Transfer) -> None:
        """Completes a write transfer."""
        del self._write_transfers[transfer.resource_id]

        if not transfer.status.ok():
            _LOG.error(
                'Write transfer %d terminated with status %s',
                transfer.id,
                transfer.status,
            )

        # TODO(frolv): This doesn't seem to work. Investigate why.
        # If no more transfers are using the write stream, close it.
        # if not self._write_transfers and self._write_stream:
        #     self._write_stream.cancel()
        #     self._write_stream = None


class Error(Exception):
    """Exception raised when a transfer fails.

    Stores the ID of the failed transfer resource and the error that occurred.
    """

    def __init__(self, resource_id: int, status: Status):
        super().__init__(f'Transfer {resource_id} failed with status {status}')
        self.resource_id = resource_id
        self.status = status
