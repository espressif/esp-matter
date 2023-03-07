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
"""Proxy for transfer integration testing.

This module contains a proxy for transfer intergation testing.  It is capable
of introducing various link failures into the connection between the client and
server.
"""

import abc
import argparse
import asyncio
from enum import Enum
import logging
import random
import socket
import sys
import time
from typing import Any, Awaitable, Callable, Iterable, List, Optional

from google.protobuf import text_format

from pigweed.pw_rpc.internal import packet_pb2
from pigweed.pw_transfer import transfer_pb2
from pigweed.pw_transfer.integration_test import config_pb2
from pw_hdlc import decode
from pw_transfer.chunk import Chunk

_LOG = logging.getLogger('pw_transfer_intergration_test_proxy')

# This is the maximum size of the socket receive buffers. Ideally, this is set
# to the lowest allowed value to minimize buffering between the proxy and
# clients so rate limiting causes the client to block and wait for the
# integration test proxy to drain rather than allowing OS buffers to backlog
# large quantities of data.
#
# Note that the OS may chose to not strictly follow this requested buffer size.
# Still, setting this value to be relatively small does reduce bufer sizes
# significantly enough to better reflect typical inter-device communication.
#
# For this to be effective, clients should also configure their sockets to a
# smaller send buffer size.
_RECEIVE_BUFFER_SIZE = 2048


class Event(Enum):
    TRANSFER_START = 1
    PARAMETERS_RETRANSMIT = 2
    PARAMETERS_CONTINUE = 3
    START_ACK_CONFIRMATION = 4


class Filter(abc.ABC):
    """An abstract interface for manipulating a stream of data.

    ``Filter``s are used to implement various transforms to simulate real
    world link properties.  Some examples include: data corruption,
    packet loss, packet reordering, rate limiting, latency modeling.

    A ``Filter`` implementation should implement the ``process`` method
    and call ``self.send_data()`` when it has data to send.
    """

    def __init__(self, send_data: Callable[[bytes], Awaitable[None]]):
        self.send_data = send_data
        pass

    @abc.abstractmethod
    async def process(self, data: bytes) -> None:
        """Processes incoming data.

        Implementations of this method may send arbitrary data, or none, using
        the ``self.send_data()`` handler.
        """

    async def __call__(self, data: bytes) -> None:
        await self.process(data)


class HdlcPacketizer(Filter):
    """A filter which aggregates data into complete HDLC packets.

    Since the proxy transport (SOCK_STREAM) has no framing and we want some
    filters to operates on whole frames, this filter can be used so that
    downstream filters see whole frames.
    """

    def __init__(self, send_data: Callable[[bytes], Awaitable[None]]):
        super().__init__(send_data)
        self.decoder = decode.FrameDecoder()

    async def process(self, data: bytes) -> None:
        for frame in self.decoder.process(data):
            await self.send_data(frame.raw_encoded)


class DataDropper(Filter):
    """A filter which drops some data.

    DataDropper will drop data passed through ``process()`` at the
    specified ``rate``.
    """

    def __init__(
        self,
        send_data: Callable[[bytes], Awaitable[None]],
        name: str,
        rate: float,
        seed: Optional[int] = None,
    ):
        super().__init__(send_data)
        self._rate = rate
        self._name = name
        if seed == None:
            seed = time.time_ns()
        self._rng = random.Random(seed)
        _LOG.info(f'{name} DataDropper initialized with seed {seed}')

    async def process(self, data: bytes) -> None:
        if self._rng.uniform(0.0, 1.0) < self._rate:
            _LOG.info(f'{self._name} dropped {len(data)} bytes of data')
        else:
            await self.send_data(data)


class KeepDropQueue(Filter):
    """A filter which alternates between sending packets and dropping packets.

    A KeepDropQueue filter will alternate between keeping packets and dropping
    chunks of data based on a keep/drop queue provided during its creation. The
    queue is looped over unless a negative element is found. A negative number
    is effectively the same as a value of infinity.

     This filter is typically most pratical when used with a packetizer so data
     can be dropped as distinct packets.

    Examples:

      keep_drop_queue = [3, 2]:
        Keeps 3 packets,
        Drops 2 packets,
        Keeps 3 packets,
        Drops 2 packets,
        ... [loops indefinitely]

      keep_drop_queue = [5, 99, 1, -1]:
        Keeps 5 packets,
        Drops 99 packets,
        Keeps 1 packet,
        Drops all further packets.
    """

    def __init__(
        self,
        send_data: Callable[[bytes], Awaitable[None]],
        name: str,
        keep_drop_queue: Iterable[int],
    ):
        super().__init__(send_data)
        self._keep_drop_queue = list(keep_drop_queue)
        self._loop_idx = 0
        self._current_count = self._keep_drop_queue[0]
        self._keep = True
        self._name = name

    async def process(self, data: bytes) -> None:
        # Move forward through the queue if neeeded.
        while self._current_count == 0:
            self._loop_idx += 1
            self._current_count = self._keep_drop_queue[
                self._loop_idx % len(self._keep_drop_queue)
            ]
            self._keep = not self._keep

        if self._current_count > 0:
            self._current_count -= 1

        if self._keep:
            await self.send_data(data)
            _LOG.info(f'{self._name} forwarded {len(data)} bytes of data')
        else:
            _LOG.info(f'{self._name} dropped {len(data)} bytes of data')


class RateLimiter(Filter):
    """A filter which limits transmission rate.

    This filter delays transmission of data by len(data)/rate.
    """

    def __init__(
        self, send_data: Callable[[bytes], Awaitable[None]], rate: float
    ):
        super().__init__(send_data)
        self._rate = rate

    async def process(self, data: bytes) -> None:
        delay = len(data) / self._rate
        await asyncio.sleep(delay)
        await self.send_data(data)


class DataTransposer(Filter):
    """A filter which occasionally transposes two chunks of data.

    This filter transposes data at the specified rate.  It does this by
    holding a chunk to transpose until another chunk arrives. The filter
    will not hold a chunk longer than ``timeout`` seconds.
    """

    def __init__(
        self,
        send_data: Callable[[bytes], Awaitable[None]],
        name: str,
        rate: float,
        timeout: float,
        seed: int,
    ):
        super().__init__(send_data)
        self._name = name
        self._rate = rate
        self._timeout = timeout
        self._data_queue = asyncio.Queue()
        self._rng = random.Random(seed)
        self._transpose_task = asyncio.create_task(self._transpose_handler())

        _LOG.info(f'{name} DataTranspose initialized with seed {seed}')

    def __del__(self):
        _LOG.info(f'{self._name} cleaning up transpose task.')
        self._transpose_task.cancel()

    async def _transpose_handler(self):
        """Async task that handles the packet transposition and timeouts"""
        held_data: Optional[bytes] = None
        while True:
            # Only use timeout if we have data held for transposition
            timeout = None if held_data is None else self._timeout
            try:
                data = await asyncio.wait_for(
                    self._data_queue.get(), timeout=timeout
                )

                if held_data is not None:
                    # If we have held data, send it out of order.
                    await self.send_data(data)
                    await self.send_data(held_data)
                    held_data = None
                else:
                    # Otherwise decide if we should transpose the current data.
                    if self._rng.uniform(0.0, 1.0) < self._rate:
                        _LOG.info(
                            f'{self._name} transposing {len(data)} bytes of data'
                        )
                        held_data = data
                    else:
                        await self.send_data(data)

            except asyncio.TimeoutError:
                _LOG.info(f'{self._name} sending data in order due to timeout')
                await self.send_data(held_data)
                held_data = None

    async def process(self, data: bytes) -> None:
        # Queue data for processing by the transpose task.
        await self._data_queue.put(data)


class ServerFailure(Filter):
    """A filter to simulate the server stopping sending packets.

    ServerFailure takes a list of numbers of packets to send before
    dropping all subsequent packets until a TRANSFER_START packet
    is seen.  This process is repeated for each element in
    packets_before_failure.  After that list is exhausted, ServerFailure
    will send all packets.

    This filter should be instantiated in the same filter stack as an
    HdlcPacketizer so that EventFilter can decode complete packets.
    """

    def __init__(
        self,
        send_data: Callable[[bytes], Awaitable[None]],
        name: str,
        packets_before_failure_list: List[int],
    ):
        super().__init__(send_data)
        self._name = name
        self._relay_packets = True
        self._packets_before_failure_list = packets_before_failure_list
        self.advance_packets_before_failure()

    def advance_packets_before_failure(self):
        if len(self._packets_before_failure_list) > 0:
            self._packets_before_failure = (
                self._packets_before_failure_list.pop(0)
            )
        else:
            self._packets_before_failure = None

    async def process(self, data: bytes) -> None:
        if self._packets_before_failure is None:
            await self.send_data(data)
        elif self._packets_before_failure > 0:
            self._packets_before_failure -= 1
            await self.send_data(data)

    def handle_event(self, event: Event) -> None:
        if event is Event.TRANSFER_START:
            self.advance_packets_before_failure()


class WindowPacketDropper(Filter):
    """A filter to allow the same packet in each window to be dropped

    WindowPacketDropper with drop the nth packet in each window as
    specified by window_packet_to_drop.  This process will happend
    indefinitely for each window.

    This filter should be instantiated in the same filter stack as an
    HdlcPacketizer so that EventFilter can decode complete packets.
    """

    def __init__(
        self,
        send_data: Callable[[bytes], Awaitable[None]],
        name: str,
        window_packet_to_drop: int,
    ):
        super().__init__(send_data)
        self._name = name
        self._relay_packets = True
        self._window_packet_to_drop = window_packet_to_drop
        self._window_packet = 0

    async def process(self, data: bytes) -> None:
        try:
            is_data_chunk = (
                _extract_transfer_chunk(data).type is Chunk.Type.DATA
            )
        except Exception:
            # Invalid / non-chunk data (e.g. text logs); ignore.
            is_data_chunk = False

        # Only count transfer data chunks as part of a window.
        if is_data_chunk:
            if self._window_packet != self._window_packet_to_drop:
                await self.send_data(data)

            self._window_packet += 1
        else:
            await self.send_data(data)

    def handle_event(self, event: Event) -> None:
        if event in (
            Event.PARAMETERS_RETRANSMIT,
            Event.PARAMETERS_CONTINUE,
            Event.START_ACK_CONFIRMATION,
        ):
            self._window_packet = 0


class EventFilter(Filter):
    """A filter that inspects packets and send events to other filters.

    This filter should be instantiated in the same filter stack as an
    HdlcPacketizer so that it can decode complete packets.
    """

    def __init__(
        self,
        send_data: Callable[[bytes], Awaitable[None]],
        name: str,
        event_queue: asyncio.Queue,
    ):
        super().__init__(send_data)
        self._queue = event_queue

    async def process(self, data: bytes) -> None:
        try:
            chunk = _extract_transfer_chunk(data)
            if chunk.type is Chunk.Type.START:
                await self._queue.put(Event.TRANSFER_START)
            if chunk.type is Chunk.Type.START_ACK_CONFIRMATION:
                await self._queue.put(Event.START_ACK_CONFIRMATION)
            elif chunk.type is Chunk.Type.PARAMETERS_RETRANSMIT:
                await self._queue.put(Event.PARAMETERS_RETRANSMIT)
            elif chunk.type is Chunk.Type.PARAMETERS_CONTINUE:
                await self._queue.put(Event.PARAMETERS_CONTINUE)
        except:
            # Silently ignore invalid packets
            pass

        await self.send_data(data)


def _extract_transfer_chunk(data: bytes) -> Chunk:
    """Gets a transfer Chunk from an HDLC frame containing an RPC packet.

    Raises an exception if a valid chunk does not exist.
    """

    decoder = decode.FrameDecoder()
    for frame in decoder.process(data):
        packet = packet_pb2.RpcPacket()
        packet.ParseFromString(frame.data)
        raw_chunk = transfer_pb2.Chunk()
        raw_chunk.ParseFromString(packet.payload)
        return Chunk.from_message(raw_chunk)

    raise ValueError("Invalid transfer frame")


async def _handle_simplex_events(
    event_queue: asyncio.Queue, handlers: List[Callable[[Event], None]]
):
    while True:
        event = await event_queue.get()
        for handler in handlers:
            handler(event)


async def _handle_simplex_connection(
    name: str,
    filter_stack_config: List[config_pb2.FilterConfig],
    reader: asyncio.StreamReader,
    writer: asyncio.StreamWriter,
    inbound_event_queue: asyncio.Queue,
    outbound_event_queue: asyncio.Queue,
) -> None:
    """Handle a single direction of a bidirectional connection between
    server and client."""

    async def send(data: bytes):
        writer.write(data)
        await writer.drain()

    filter_stack = EventFilter(send, name, outbound_event_queue)

    event_handlers: List[Callable[[Event], None]] = []

    # Build the filter stack from the bottom up
    for config in reversed(filter_stack_config):
        filter_name = config.WhichOneof("filter")
        if filter_name == "hdlc_packetizer":
            filter_stack = HdlcPacketizer(filter_stack)
        elif filter_name == "data_dropper":
            data_dropper = config.data_dropper
            filter_stack = DataDropper(
                filter_stack, name, data_dropper.rate, data_dropper.seed
            )
        elif filter_name == "rate_limiter":
            filter_stack = RateLimiter(filter_stack, config.rate_limiter.rate)
        elif filter_name == "data_transposer":
            transposer = config.data_transposer
            filter_stack = DataTransposer(
                filter_stack,
                name,
                transposer.rate,
                transposer.timeout,
                transposer.seed,
            )
        elif filter_name == "server_failure":
            server_failure = config.server_failure
            filter_stack = ServerFailure(
                filter_stack, name, server_failure.packets_before_failure
            )
            event_handlers.append(filter_stack.handle_event)
        elif filter_name == "keep_drop_queue":
            keep_drop_queue = config.keep_drop_queue
            filter_stack = KeepDropQueue(
                filter_stack, name, keep_drop_queue.keep_drop_queue
            )
        elif filter_name == "window_packet_dropper":
            window_packet_dropper = config.window_packet_dropper
            filter_stack = WindowPacketDropper(
                filter_stack, name, window_packet_dropper.window_packet_to_drop
            )
            event_handlers.append(filter_stack.handle_event)
        else:
            sys.exit(f'Unknown filter {filter_name}')

    event_task = asyncio.create_task(
        _handle_simplex_events(inbound_event_queue, event_handlers)
    )

    while True:
        # Arbitrarily chosen "page sized" read.
        data = await reader.read(4096)

        # An empty data indicates that the connection is closed.
        if not data:
            _LOG.info(f'{name} connection closed.')
            return

        await filter_stack.process(data)


async def _handle_connection(
    server_port: int,
    config: config_pb2.ProxyConfig,
    client_reader: asyncio.StreamReader,
    client_writer: asyncio.StreamWriter,
) -> None:
    """Handle a connection between server and client."""

    client_addr = client_writer.get_extra_info('peername')
    _LOG.info(f'New client connection from {client_addr}')

    # Open a new connection to the server for each client connection.
    #
    # TODO(konkers): catch exception and close client writer
    server_reader, server_writer = await asyncio.open_connection(
        'localhost', server_port
    )
    _LOG.info(f'New connection opened to server')

    # Queues for the simplex connections to pass events to each other.
    server_event_queue = asyncio.Queue()
    client_event_queue = asyncio.Queue()

    # Instantiate two simplex handler one for each direction of the connection.
    _, pending = await asyncio.wait(
        [
            asyncio.create_task(
                _handle_simplex_connection(
                    "client",
                    config.client_filter_stack,
                    client_reader,
                    server_writer,
                    server_event_queue,
                    client_event_queue,
                )
            ),
            asyncio.create_task(
                _handle_simplex_connection(
                    "server",
                    config.server_filter_stack,
                    server_reader,
                    client_writer,
                    client_event_queue,
                    server_event_queue,
                )
            ),
        ],
        return_when=asyncio.FIRST_COMPLETED,
    )

    # When one side terminates the connection, also terminate the other side
    for task in pending:
        task.cancel()

    for stream in [client_writer, server_writer]:
        stream.close()


def _parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )

    parser.add_argument(
        '--server-port',
        type=int,
        required=True,
        help='Port of the integration test server.  The proxy will forward connections to this port',
    )
    parser.add_argument(
        '--client-port',
        type=int,
        required=True,
        help='Port on which to listen for connections from integration test client.',
    )

    return parser.parse_args()


def _init_logging(level: int) -> None:
    _LOG.setLevel(logging.DEBUG)
    log_to_stderr = logging.StreamHandler()
    log_to_stderr.setLevel(level)
    log_to_stderr.setFormatter(
        logging.Formatter(
            fmt='%(asctime)s.%(msecs)03d-%(levelname)s: %(message)s',
            datefmt='%H:%M:%S',
        )
    )

    _LOG.addHandler(log_to_stderr)


async def _main(server_port: int, client_port: int) -> None:
    _init_logging(logging.DEBUG)

    # Load config from stdin using synchronous IO
    text_config = sys.stdin.buffer.read()

    config = text_format.Parse(text_config, config_pb2.ProxyConfig())

    # Instantiate the TCP server.
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(
        socket.SOL_SOCKET, socket.SO_RCVBUF, _RECEIVE_BUFFER_SIZE
    )
    server_socket.bind(('localhost', client_port))
    server = await asyncio.start_server(
        lambda reader, writer: _handle_connection(
            server_port, config, reader, writer
        ),
        limit=_RECEIVE_BUFFER_SIZE,
        sock=server_socket,
    )

    addrs = ', '.join(str(sock.getsockname()) for sock in server.sockets)
    _LOG.info(f'Listening for client connection on {addrs}')

    # Run the TCP server.
    async with server:
        await server.serve_forever()


if __name__ == '__main__':
    asyncio.run(_main(**vars(_parse_args())))
