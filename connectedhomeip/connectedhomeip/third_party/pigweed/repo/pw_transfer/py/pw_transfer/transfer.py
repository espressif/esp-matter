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
"""Classes for read and write transfers."""

import abc
import asyncio
from dataclasses import dataclass
import enum
import logging
import math
import threading
from typing import Any, Callable, Optional

from pw_status import Status
from pw_transfer.chunk import Chunk, ProtocolVersion

_LOG = logging.getLogger(__package__)


@dataclass(frozen=True)
class ProgressStats:
    bytes_sent: int
    bytes_confirmed_received: int
    total_size_bytes: Optional[int]

    def percent_received(self) -> float:
        if self.total_size_bytes is None:
            return math.nan

        return self.bytes_confirmed_received / self.total_size_bytes * 100

    def __str__(self) -> str:
        total = (
            str(self.total_size_bytes) if self.total_size_bytes else 'unknown'
        )
        return (
            f'{self.percent_received():5.1f}% ({self.bytes_sent} B sent, '
            f'{self.bytes_confirmed_received} B received of {total} B)'
        )


ProgressCallback = Callable[[ProgressStats], Any]


class _Timer:
    """A timer which invokes a callback after a certain timeout."""

    def __init__(self, timeout_s: float, callback: Callable[[], Any]):
        self.timeout_s = timeout_s
        self._callback = callback
        self._task: Optional[asyncio.Task[Any]] = None

    def start(self, timeout_s: Optional[float] = None) -> None:
        """Starts a new timer.

        If a timer is already running, it is stopped and a new timer started.
        This can be used to implement watchdog-like behavior, where a callback
        is invoked after some time without a kick.
        """
        self.stop()
        timeout_s = self.timeout_s if timeout_s is None else timeout_s
        self._task = asyncio.create_task(self._run(timeout_s))

    def stop(self) -> None:
        """Terminates a running timer."""
        if self._task is not None:
            self._task.cancel()
            self._task = None

    async def _run(self, timeout_s: float) -> None:
        await asyncio.sleep(timeout_s)
        self._task = None
        self._callback()


class Transfer(abc.ABC):
    """A client-side data transfer through a Manager.

    Subclasses are responsible for implementing all of the logic for their type
    of transfer, receiving messages from the server and sending the appropriate
    messages in response.
    """

    # pylint: disable=too-many-instance-attributes

    class _State(enum.Enum):
        # Transfer is starting. The server and client are performing an initial
        # handshake and negotiating protocol and feature flags.
        INITIATING = 0

        # Waiting for the other end to send a chunk.
        WAITING = 1

        # Transmitting a window of data to a receiver.
        TRANSMITTING = 2

        # Recovering after one or more chunks was dropped in an active transfer.
        RECOVERY = 3

        # Transfer has completed locally and is waiting for the peer to
        # acknowledge its final status. Only entered by the terminating side of
        # the transfer.
        #
        # The context remains in a TERMINATING state until it receives an
        # acknowledgement from the peer or times out.
        TERMINATING = 4

        # A transfer has fully completed.
        COMPLETE = 5

    _UNASSIGNED_SESSION_ID = 0

    def __init__(  # pylint: disable=too-many-arguments
        self,
        resource_id: int,
        send_chunk: Callable[[Chunk], None],
        end_transfer: Callable[['Transfer'], None],
        response_timeout_s: float,
        initial_response_timeout_s: float,
        max_retries: int,
        max_lifetime_retries: int,
        protocol_version: ProtocolVersion,
        progress_callback: Optional[ProgressCallback] = None,
    ):
        self.status = Status.OK
        self.done = threading.Event()

        self._session_id = self._UNASSIGNED_SESSION_ID
        self._resource_id = resource_id

        self._send_chunk_fn = send_chunk
        self._end_transfer = end_transfer

        self._desired_protocol_version = protocol_version
        self._configured_protocol_version = ProtocolVersion.UNKNOWN

        if self._desired_protocol_version is ProtocolVersion.LEGACY:
            # In a legacy transfer, there is no protocol negotiation stage.
            # Automatically configure the context to run the legacy protocol and
            # proceed to waiting for a chunk.
            self._configured_protocol_version = ProtocolVersion.LEGACY
            self._state = Transfer._State.WAITING
            self._session_id = self._resource_id
        else:
            self._state = Transfer._State.INITIATING

        self._last_chunk: Optional[Chunk] = None

        self._retries = 0
        self._max_retries = max_retries
        self._lifetime_retries = 0
        self._max_lifetime_retries = max_lifetime_retries
        self._response_timer = _Timer(response_timeout_s, self._on_timeout)
        self._initial_response_timeout_s = initial_response_timeout_s

        self._progress_callback = progress_callback

    async def begin(self) -> None:
        """Sends the initial chunk of the transfer."""

        if (
            self._desired_protocol_version is ProtocolVersion.UNKNOWN
            or self._desired_protocol_version.value
            > ProtocolVersion.LATEST.value
        ):
            _LOG.error(
                'Cannot start a transfer with unsupported protocol version %d',
                self._desired_protocol_version.value,
            )
            self.finish(Status.INVALID_ARGUMENT)
            return

        initial_chunk = Chunk(
            self._desired_protocol_version,
            Chunk.Type.START,
            resource_id=self._resource_id,
        )

        # Regardless of the desired protocol version, set any additional fields
        # on the opening chunk, in case the server only runs legacy.
        self._set_initial_chunk_fields(initial_chunk)

        self._send_chunk(initial_chunk)
        self._response_timer.start(self._initial_response_timeout_s)

    @property
    def id(self) -> int:
        """Returns the identifier for the active transfer."""
        if self._session_id != self._UNASSIGNED_SESSION_ID:
            return self._session_id
        return self._resource_id

    @property
    def resource_id(self) -> int:
        """Returns the identifier of the resource being transferred."""
        return self._resource_id

    @property
    @abc.abstractmethod
    def data(self) -> bytes:
        """Returns the data read or written in this transfer."""

    @abc.abstractmethod
    def _set_initial_chunk_fields(self, chunk: Chunk) -> None:
        """Sets fields for the initial non-handshake chunk of the transfer."""

    def _send_chunk(self, chunk: Chunk) -> None:
        """Sends a chunk to the server, keeping track of the last chunk sent."""
        self._send_chunk_fn(chunk)
        self._last_chunk = chunk

    async def handle_chunk(self, chunk: Chunk) -> None:
        """Processes an incoming chunk from the server.

        Handles terminating chunks (i.e. those with a status) and forwards
        non-terminating chunks to handle_data_chunk.
        """
        self._response_timer.stop()
        self._retries = 0  # Received data from service, so reset the retries.

        _LOG.debug('Received chunk\n%s', str(chunk.to_message()).rstrip())

        # Status chunks are only used to terminate a transfer. They do not
        # contain any data that requires processing.
        if chunk.status is not None:
            if self._configured_protocol_version is ProtocolVersion.VERSION_TWO:
                self._send_chunk(
                    Chunk(
                        self._configured_protocol_version,
                        Chunk.Type.COMPLETION_ACK,
                        session_id=self._session_id,
                    )
                )

            self.finish(Status(chunk.status))
            return

        if self._state is Transfer._State.INITIATING:
            await self._perform_initial_handshake(chunk)
        elif self._state is Transfer._State.TERMINATING:
            if chunk.type is Chunk.Type.COMPLETION_ACK:
                self.finish(self.status)
            else:
                # Expecting a completion ACK but didn't receive one. Go through
                # the retry process.
                self._on_timeout()
        else:
            await self._handle_data_chunk(chunk)

        # Start the timeout for the server to send a chunk in response.
        self._response_timer.start()

    async def _perform_initial_handshake(self, chunk: Chunk) -> None:
        """Progresses the initial handshake phase of a v2+ transfer."""
        assert self._state is Transfer._State.INITIATING

        # If a non-handshake chunk is received during an INITIATING state, the
        # transfer server is running a legacy protocol version, which does not
        # perform a handshake. End the handshake, revert to the legacy protocol,
        # and process the chunk appropriately.
        if chunk.type is not Chunk.Type.START_ACK:
            _LOG.debug(
                'Transfer %d got non-handshake chunk, reverting to legacy',
                self.id,
            )

            self._configured_protocol_version = ProtocolVersion.LEGACY
            self._state = Transfer._State.WAITING

            # Update the transfer's session ID in case it was expecting one to
            # be assigned by the server.
            self._session_id = chunk.session_id

            await self._handle_data_chunk(chunk)
            return

        self._session_id = chunk.session_id

        self._configured_protocol_version = ProtocolVersion(
            min(
                self._desired_protocol_version.value,
                chunk.protocol_version.value,
            )
        )
        _LOG.debug(
            'Transfer %d negotiating protocol version: ours=%d, theirs=%d',
            self.id,
            self._desired_protocol_version.value,
            chunk.protocol_version.value,
        )

        # Send a confirmation chunk to the server accepting the assigned session
        # ID and protocol version. Tag any initial transfer parameters onto the
        # chunk to begin the data transfer.
        start_ack_confirmation = Chunk(
            self._configured_protocol_version,
            Chunk.Type.START_ACK_CONFIRMATION,
            session_id=self._session_id,
        )
        self._set_initial_chunk_fields(start_ack_confirmation)

        self._state = Transfer._State.WAITING
        self._send_chunk(start_ack_confirmation)

    @abc.abstractmethod
    async def _handle_data_chunk(self, chunk: Chunk) -> None:
        """Handles a chunk that contains or requests data."""

    @abc.abstractmethod
    def _retry_after_data_timeout(self) -> None:
        """Retries after a timeout occurs during the data transfer phase.

        Only invoked when in the data transfer phase (i.e. state is in
        {WAITING, TRANSMITTING, RECOVERY}). Timeouts occurring during an
        opening or closing handshake are handled by the base Transfer.
        """

    def _on_timeout(self) -> None:
        """Handles a timeout while waiting for a chunk."""
        if self._state is Transfer._State.COMPLETE:
            return

        self._retries += 1
        self._lifetime_retries += 1

        if (
            self._retries > self._max_retries
            or self._lifetime_retries > self._max_lifetime_retries
        ):
            if self._state is Transfer._State.TERMINATING:
                # If the server never responded to the sent completion chunk,
                # simply end the transfer locally with its original status.
                self.finish(self.status)
            else:
                self.finish(Status.DEADLINE_EXCEEDED)
            return

        _LOG.debug(
            'Received no responses for %.3fs; retrying %d/%d',
            self._response_timer.timeout_s,
            self._retries,
            self._max_retries,
        )

        retry_handshake_chunk = self._state in (
            Transfer._State.INITIATING,
            Transfer._State.TERMINATING,
        ) or (
            self._last_chunk is not None
            and self._last_chunk.type is Chunk.Type.START_ACK_CONFIRMATION
        )

        if retry_handshake_chunk:
            assert self._last_chunk is not None
            self._send_chunk(self._last_chunk)
        else:
            self._retry_after_data_timeout()

        self._response_timer.start()

    def finish(self, status: Status, skip_callback: bool = False) -> None:
        """Ends the transfer with the specified status."""
        self._response_timer.stop()
        self.status = status

        if status.ok():
            total_size = len(self.data)
            self._update_progress(total_size, total_size, total_size)

        if not skip_callback:
            self._end_transfer(self)

        # Set done last so that the transfer has been fully cleaned up.
        self._state = Transfer._State.COMPLETE
        self.done.set()

    def _update_progress(
        self,
        bytes_sent: int,
        bytes_confirmed_received: int,
        total_size_bytes: Optional[int],
    ) -> None:
        """Invokes the provided progress callback, if any, with the progress."""

        stats = ProgressStats(
            bytes_sent, bytes_confirmed_received, total_size_bytes
        )
        _LOG.debug('Transfer %d progress: %s', self.id, stats)

        if self._progress_callback:
            self._progress_callback(stats)

    def _send_final_chunk(self, status: Status) -> None:
        """Sends a status chunk to the server and finishes the transfer."""
        self._send_chunk(
            Chunk(
                self._configured_protocol_version,
                Chunk.Type.COMPLETION,
                session_id=self.id,
                status=status,
            )
        )

        if self._configured_protocol_version is ProtocolVersion.VERSION_TWO:
            # Wait for a completion ACK from the server.
            self.status = status
            self._state = Transfer._State.TERMINATING
            self._response_timer.start()
        else:
            self.finish(status)


class WriteTransfer(Transfer):
    """A client -> server write transfer."""

    def __init__(  # pylint: disable=too-many-arguments
        self,
        resource_id: int,
        data: bytes,
        send_chunk: Callable[[Chunk], None],
        end_transfer: Callable[[Transfer], None],
        response_timeout_s: float,
        initial_response_timeout_s: float,
        max_retries: int,
        max_lifetime_retries: int,
        protocol_version: ProtocolVersion,
        progress_callback: Optional[ProgressCallback] = None,
    ):
        super().__init__(
            resource_id,
            send_chunk,
            end_transfer,
            response_timeout_s,
            initial_response_timeout_s,
            max_retries,
            max_lifetime_retries,
            protocol_version,
            progress_callback,
        )
        self._data = data

        self._offset = 0
        self._window_end_offset = 0
        self._max_chunk_size = 0
        self._chunk_delay_us: Optional[int] = None

        # The window ID increments for each parameters update.
        self._window_id = 0

        self._bytes_confirmed_received = 0

    @property
    def data(self) -> bytes:
        return self._data

    def _set_initial_chunk_fields(self, chunk: Chunk) -> None:
        # Nothing to tag onto the initial chunk in a write transfer.
        pass

    async def _handle_data_chunk(self, chunk: Chunk) -> None:
        """Processes an incoming chunk from the server.

        In a write transfer, the server only sends transfer parameter updates
        to the client. When a message is received, update local parameters and
        send data accordingly.
        """

        if self._state is Transfer._State.TRANSMITTING:
            self._state = Transfer._State.WAITING

        assert self._state is Transfer._State.WAITING

        if not self._handle_parameters_update(chunk):
            return

        self._bytes_confirmed_received = chunk.offset
        self._state = Transfer._State.TRANSMITTING

        self._window_id += 1
        asyncio.create_task(self._transmit_next_chunk(self._window_id))

    async def _transmit_next_chunk(
        self, window_id: int, timeout_us: Optional[int] = None
    ) -> None:
        """Transmits a single data chunk to the server.

        If the chunk completes the active window, returns to a WAITING state.
        Otherwise, schedules another transmission for the next chunk.
        """
        if timeout_us is not None:
            await asyncio.sleep(timeout_us / 1e6)

        if self._state is not Transfer._State.TRANSMITTING:
            return

        if window_id != self._window_id:
            _LOG.debug('Transfer %d: Skipping stale window', self.id)
            return

        chunk = self._next_chunk()
        self._offset += len(chunk.data)

        sent_requested_bytes = self._offset == self._window_end_offset

        self._send_chunk(chunk)
        self._update_progress(
            self._offset, self._bytes_confirmed_received, len(self.data)
        )

        if sent_requested_bytes:
            self._state = Transfer._State.WAITING
        else:
            asyncio.create_task(
                self._transmit_next_chunk(
                    window_id, timeout_us=self._chunk_delay_us
                )
            )

    def _handle_parameters_update(self, chunk: Chunk) -> bool:
        """Updates transfer state based on a transfer parameters update."""

        if chunk.offset > len(self.data):
            # Bad offset; terminate the transfer.
            _LOG.error(
                'Transfer %d: server requested invalid offset %d (size %d)',
                self.id,
                chunk.offset,
                len(self.data),
            )

            self._send_final_chunk(Status.OUT_OF_RANGE)
            return False

        if chunk.offset == chunk.window_end_offset:
            _LOG.error(
                'Transfer %d: service requested 0 bytes (invalid); aborting',
                self.id,
            )
            self._send_final_chunk(Status.INTERNAL)
            return False

        # Extend the window to the new end offset specified by the server.
        self._window_end_offset = min(chunk.window_end_offset, len(self.data))

        if chunk.requests_transmission_from_offset():
            # Check whether the client has sent a previous data offset, which
            # indicates that some chunks were lost in transmission.
            if chunk.offset < self._offset:
                _LOG.debug(
                    'Write transfer %d rolling back: offset %d from %d',
                    self.id,
                    chunk.offset,
                    self._offset,
                )

            self._offset = chunk.offset

        if chunk.max_chunk_size_bytes is not None:
            self._max_chunk_size = chunk.max_chunk_size_bytes

        if chunk.min_delay_microseconds is not None:
            self._chunk_delay_us = chunk.min_delay_microseconds

        return True

    def _retry_after_data_timeout(self) -> None:
        if (
            self._state is Transfer._State.WAITING
            and self._last_chunk is not None
        ):
            self._send_chunk(self._last_chunk)

    def _next_chunk(self) -> Chunk:
        """Returns the next Chunk message to send in the data transfer."""
        chunk = Chunk(
            self._configured_protocol_version,
            Chunk.Type.DATA,
            session_id=self.id,
            offset=self._offset,
        )

        max_bytes_in_chunk = min(
            self._max_chunk_size, self._window_end_offset - self._offset
        )
        chunk.data = self.data[self._offset : self._offset + max_bytes_in_chunk]

        # Mark the final chunk of the transfer.
        if len(self.data) - self._offset <= max_bytes_in_chunk:
            chunk.remaining_bytes = 0

        return chunk


class ReadTransfer(Transfer):
    """A client <- server read transfer.

    Although Python can effectively handle an unlimited transfer window, this
    client sets a conservative window and chunk size to avoid overloading the
    device. These are configurable in the constructor.
    """

    # The fractional position within a window at which a receive transfer should
    # extend its window size to minimize the amount of time the transmitter
    # spends blocked.
    #
    # For example, a divisor of 2 will extend the window when half of the
    # requested data has been received, a divisor of three will extend at a
    # third of the window, and so on.
    EXTEND_WINDOW_DIVISOR = 2

    def __init__(  # pylint: disable=too-many-arguments
        self,
        resource_id: int,
        send_chunk: Callable[[Chunk], None],
        end_transfer: Callable[[Transfer], None],
        response_timeout_s: float,
        initial_response_timeout_s: float,
        max_retries: int,
        max_lifetime_retries: int,
        protocol_version: ProtocolVersion,
        max_bytes_to_receive: int = 8192,
        max_chunk_size: int = 1024,
        chunk_delay_us: Optional[int] = None,
        progress_callback: Optional[ProgressCallback] = None,
    ):
        super().__init__(
            resource_id,
            send_chunk,
            end_transfer,
            response_timeout_s,
            initial_response_timeout_s,
            max_retries,
            max_lifetime_retries,
            protocol_version,
            progress_callback,
        )
        self._max_bytes_to_receive = max_bytes_to_receive
        self._max_chunk_size = max_chunk_size
        self._chunk_delay_us = chunk_delay_us

        self._remaining_transfer_size: Optional[int] = None
        self._data = bytearray()
        self._offset = 0
        self._window_end_offset = max_bytes_to_receive
        self._last_chunk_offset: Optional[int] = None

    @property
    def data(self) -> bytes:
        """Returns an immutable copy of the data that has been read."""
        return bytes(self._data)

    def _set_initial_chunk_fields(self, chunk: Chunk) -> None:
        self._set_transfer_parameters(chunk)

    async def _handle_data_chunk(self, chunk: Chunk) -> None:
        """Processes an incoming chunk from the server.

        In a read transfer, the client receives data chunks from the server.
        Once all pending data is received, the transfer parameters are updated.
        """

        if self._state is Transfer._State.RECOVERY:
            if chunk.offset != self._offset:
                if self._last_chunk_offset == chunk.offset:
                    _LOG.debug(
                        'Transfer %d received repeated offset %d: '
                        'retry detected, resending transfer parameters',
                        self.id,
                        chunk.offset,
                    )
                    self._send_chunk(
                        self._transfer_parameters(
                            Chunk.Type.PARAMETERS_RETRANSMIT
                        )
                    )
                else:
                    _LOG.debug(
                        'Transfer %d waiting for offset %d, ignoring %d',
                        self.id,
                        self._offset,
                        chunk.offset,
                    )
                    self._last_chunk_offset = chunk.offset
                return

            _LOG.info(
                'Transfer %d received expected offset %d, resuming transfer',
                self.id,
                chunk.offset,
            )
            self._state = Transfer._State.WAITING

        assert self._state is Transfer._State.WAITING

        if chunk.offset != self._offset:
            # Initially, the transfer service only supports in-order transfers.
            # If data is received out of order, request that the server
            # retransmit from the previous offset.
            _LOG.debug(
                'Transfer %d expected offset %d, received %d: '
                'entering recovery state',
                self.id,
                self._offset,
                chunk.offset,
            )
            self._state = Transfer._State.RECOVERY

            self._send_chunk(
                self._transfer_parameters(Chunk.Type.PARAMETERS_RETRANSMIT)
            )
            return

        self._data += chunk.data
        self._offset += len(chunk.data)

        # Update the last offset seen so that retries can be detected.
        self._last_chunk_offset = chunk.offset

        if chunk.remaining_bytes is not None:
            if chunk.remaining_bytes == 0:
                # No more data to read. Acknowledge receipt and finish.
                self._send_final_chunk(Status.OK)
                return

            # The server may indicate if the amount of remaining data is known.
            self._remaining_transfer_size = chunk.remaining_bytes
        elif self._remaining_transfer_size is not None:
            # Update the remaining transfer size, if it is known.
            self._remaining_transfer_size -= len(chunk.data)

            # If the transfer size drops to zero, the estimate was inaccurate.
            if self._remaining_transfer_size <= 0:
                self._remaining_transfer_size = None

        total_size = (
            None
            if self._remaining_transfer_size is None
            else (self._remaining_transfer_size + self._offset)
        )
        self._update_progress(self._offset, self._offset, total_size)

        if chunk.window_end_offset != 0:
            if chunk.window_end_offset < self._offset:
                _LOG.error(
                    'Transfer %d: transmitter sent invalid earlier end offset '
                    '%d (receiver offset %d)',
                    self.id,
                    chunk.window_end_offset,
                    self._offset,
                )
                self._send_final_chunk(Status.INTERNAL)
                return

            if chunk.window_end_offset > self._window_end_offset:
                _LOG.error(
                    'Transfer %d: transmitter sent invalid later end offset '
                    '%d (receiver end offset %d)',
                    self.id,
                    chunk.window_end_offset,
                    self._window_end_offset,
                )
                self._send_final_chunk(Status.INTERNAL)
                return

            self._window_end_offset = chunk.window_end_offset

        remaining_window_size = self._window_end_offset - self._offset
        extend_window = (
            remaining_window_size
            <= self._max_bytes_to_receive / ReadTransfer.EXTEND_WINDOW_DIVISOR
        )

        if self._offset == self._window_end_offset:
            # All pending data was received. Send out a new parameters chunk for
            # the next block.
            self._send_chunk(
                self._transfer_parameters(Chunk.Type.PARAMETERS_RETRANSMIT)
            )
        elif extend_window:
            self._send_chunk(
                self._transfer_parameters(Chunk.Type.PARAMETERS_CONTINUE)
            )

    def _retry_after_data_timeout(self) -> None:
        if (
            self._state is Transfer._State.WAITING
            or self._state is Transfer._State.RECOVERY
        ):
            self._send_chunk(
                self._transfer_parameters(Chunk.Type.PARAMETERS_RETRANSMIT)
            )

    def _set_transfer_parameters(self, chunk: Chunk) -> None:
        self._window_end_offset = self._offset + self._max_bytes_to_receive

        chunk.offset = self._offset
        chunk.window_end_offset = self._window_end_offset
        chunk.max_chunk_size_bytes = self._max_chunk_size

        if self._chunk_delay_us:
            chunk.min_delay_microseconds = self._chunk_delay_us

    def _transfer_parameters(self, chunk_type: Any) -> Chunk:
        """Returns an updated transfer parameters chunk."""

        chunk = Chunk(
            self._configured_protocol_version, chunk_type, session_id=self.id
        )
        self._set_transfer_parameters(chunk)

        return chunk
