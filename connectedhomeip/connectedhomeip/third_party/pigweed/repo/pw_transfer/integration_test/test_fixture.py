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
"""Test fixture for pw_transfer integration tests."""

import argparse
import asyncio
from dataclasses import dataclass
import logging
import pathlib
from pathlib import Path
import sys
import tempfile
from typing import BinaryIO, Iterable, List, NamedTuple, Optional
import unittest

from google.protobuf import text_format

from pigweed.pw_protobuf.pw_protobuf_protos import status_pb2
from pigweed.pw_transfer.integration_test import config_pb2
from rules_python.python.runfiles import runfiles

_LOG = logging.getLogger('pw_transfer_intergration_test_proxy')
_LOG.level = logging.DEBUG
_LOG.addHandler(logging.StreamHandler(sys.stdout))


class LogMonitor:
    """Monitors lines read from the reader, and logs them."""

    class Error(Exception):
        """Raised if wait_for_line reaches EOF before expected line."""

        pass

    def __init__(self, prefix: str, reader: asyncio.StreamReader):
        """Initializer.

        Args:
          prefix: Prepended to read lines before they are logged.
          reader: StreamReader to read lines from.
        """
        self._prefix = prefix
        self._reader = reader

        # Queue of messages waiting to be monitored.
        self._queue = asyncio.Queue()
        # Relog any messages read from the reader, and enqueue them for
        # monitoring.
        self._relog_and_enqueue_task = asyncio.create_task(
            self._relog_and_enqueue()
        )

    async def wait_for_line(self, msg: str):
        """Wait for a line containing msg to be read from the reader."""
        while True:
            line = await self._queue.get()
            if not line:
                raise LogMonitor.Error(
                    f"Reached EOF before getting line matching {msg}"
                )
            if msg in line.decode():
                return

    async def wait_for_eof(self):
        """Wait for the reader to reach EOF, relogging any lines read."""
        # Drain the queue, since we're not monitoring it any more.
        drain_queue = asyncio.create_task(self._drain_queue())
        await asyncio.gather(drain_queue, self._relog_and_enqueue_task)

    async def _relog_and_enqueue(self):
        """Reads lines from the reader, logs them, and puts them in queue."""
        while True:
            line = await self._reader.readline()
            await self._queue.put(line)
            if line:
                _LOG.info(f"{self._prefix} {line.decode().rstrip()}")
            else:
                # EOF. Note, we still put the EOF in the queue, so that the
                # queue reader can process it appropriately.
                return

    async def _drain_queue(self):
        while True:
            line = await self._queue.get()
            if not line:
                # EOF.
                return


class MonitoredSubprocess:
    """A subprocess with monitored asynchronous communication."""

    @staticmethod
    async def create(cmd: List[str], prefix: str, stdinput: bytes):
        """Starts the subprocess and writes stdinput to stdin.

        This method returns once stdinput has been written to stdin. The
        MonitoredSubprocess continues to log the process's stderr and stdout
        (with the prefix) until it terminates.

        Args:
          cmd: Command line to execute.
          prefix: Prepended to process logs.
          stdinput: Written to stdin on process startup.
        """
        self = MonitoredSubprocess()
        self._process = await asyncio.create_subprocess_exec(
            *cmd,
            stdin=asyncio.subprocess.PIPE,
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE,
        )

        self._stderr_monitor = LogMonitor(
            f"{prefix} ERR:", self._process.stderr
        )
        self._stdout_monitor = LogMonitor(
            f"{prefix} OUT:", self._process.stdout
        )

        self._process.stdin.write(stdinput)
        await self._process.stdin.drain()
        self._process.stdin.close()
        await self._process.stdin.wait_closed()
        return self

    async def wait_for_line(self, stream: str, msg: str, timeout: float):
        """Wait for a line containing msg to be read on the stream."""
        if stream == "stdout":
            monitor = self._stdout_monitor
        elif stream == "stderr":
            monitor = self._stderr_monitor
        else:
            raise ValueError(
                "Stream must be 'stdout' or 'stderr', got {stream}"
            )

        await asyncio.wait_for(monitor.wait_for_line(msg), timeout)

    def returncode(self):
        return self._process.returncode

    def terminate(self):
        """Terminate the process."""
        self._process.terminate()

    async def wait_for_termination(self, timeout: float):
        """Wait for the process to terminate."""
        await asyncio.wait_for(
            asyncio.gather(
                self._process.wait(),
                self._stdout_monitor.wait_for_eof(),
                self._stderr_monitor.wait_for_eof(),
            ),
            timeout,
        )

    async def terminate_and_wait(self, timeout: float):
        """Terminate the process and wait for it to exit."""
        if self.returncode() is not None:
            # Process already terminated
            return
        self.terminate()
        await self.wait_for_termination(timeout)


class TransferConfig(NamedTuple):
    """A simple tuple to collect configs for test binaries."""

    server: config_pb2.ServerConfig
    client: config_pb2.ClientConfig
    proxy: config_pb2.ProxyConfig


class TransferIntegrationTestHarness:
    """A class to manage transfer integration tests"""

    # Prefix for log messages coming from the harness (as opposed to the server,
    # client, or proxy processes). Padded so that the length is the same as
    # "SERVER OUT:".
    _PREFIX = "HARNESS:   "

    @dataclass
    class Config:
        server_port: int = 3300
        client_port: int = 3301
        java_client_binary: Optional[Path] = None
        cpp_client_binary: Optional[Path] = None
        python_client_binary: Optional[Path] = None
        proxy_binary: Optional[Path] = None
        server_binary: Optional[Path] = None

    class TransferExitCodes(NamedTuple):
        client: int
        server: int

    def __init__(self, harness_config: Config) -> None:
        # TODO(tpudlik): This is Bazel-only. Support gn, too.
        r = runfiles.Create()

        # Set defaults.
        self._JAVA_CLIENT_BINARY = r.Rlocation(
            "pigweed/pw_transfer/integration_test/java_client"
        )
        self._CPP_CLIENT_BINARY = r.Rlocation(
            "pigweed/pw_transfer/integration_test/cpp_client"
        )
        self._PYTHON_CLIENT_BINARY = r.Rlocation(
            "pigweed/pw_transfer/integration_test/python_client"
        )
        self._PROXY_BINARY = r.Rlocation(
            "pigweed/pw_transfer/integration_test/proxy"
        )
        self._SERVER_BINARY = r.Rlocation(
            "pigweed/pw_transfer/integration_test/server"
        )

        # Server/client ports are non-optional, so use those.
        self._CLIENT_PORT = harness_config.client_port
        self._SERVER_PORT = harness_config.server_port

        # If the harness configuration specifies overrides, use those.
        if harness_config.java_client_binary is not None:
            self._JAVA_CLIENT_BINARY = harness_config.java_client_binary
        if harness_config.cpp_client_binary is not None:
            self._CPP_CLIENT_BINARY = harness_config.cpp_client_binary
        if harness_config.python_client_binary is not None:
            self._PYTHON_CLIENT_BINARY = harness_config.python_client_binary
        if harness_config.proxy_binary is not None:
            self._PROXY_BINARY = harness_config.proxy_binary
        if harness_config.server_binary is not None:
            self._SERVER_BINARY = harness_config.server_binary

        self._CLIENT_BINARY = {
            "cpp": self._CPP_CLIENT_BINARY,
            "java": self._JAVA_CLIENT_BINARY,
            "python": self._PYTHON_CLIENT_BINARY,
        }
        pass

    async def _start_client(
        self, client_type: str, config: config_pb2.ClientConfig
    ):
        _LOG.info(f"{self._PREFIX} Starting client with config\n{config}")
        self._client = await MonitoredSubprocess.create(
            [self._CLIENT_BINARY[client_type], str(self._CLIENT_PORT)],
            "CLIENT",
            str(config).encode('ascii'),
        )

    async def _start_server(self, config: config_pb2.ServerConfig):
        _LOG.info(f"{self._PREFIX} Starting server with config\n{config}")
        self._server = await MonitoredSubprocess.create(
            [self._SERVER_BINARY, str(self._SERVER_PORT)],
            "SERVER",
            str(config).encode('ascii'),
        )

    async def _start_proxy(self, config: config_pb2.ProxyConfig):
        _LOG.info(f"{self._PREFIX} Starting proxy with config\n{config}")
        self._proxy = await MonitoredSubprocess.create(
            [
                self._PROXY_BINARY,
                "--server-port",
                str(self._SERVER_PORT),
                "--client-port",
                str(self._CLIENT_PORT),
            ],
            # Extra space in "PROXY " so that it lines up with "SERVER".
            "PROXY ",
            str(config).encode('ascii'),
        )

    async def perform_transfers(
        self,
        server_config: config_pb2.ServerConfig,
        client_type: str,
        client_config: config_pb2.ClientConfig,
        proxy_config: config_pb2.ProxyConfig,
    ) -> TransferExitCodes:
        """Performs a pw_transfer write.

        Args:
          server_config: Server configuration.
          client_type: Either "cpp", "java", or "python".
          client_config: Client configuration.
          proxy_config: Proxy configuration.

        Returns:
          Exit code of the client and server as a tuple.
        """
        # Timeout for components (server, proxy) to come up or shut down after
        # write is finished or a signal is sent. Approximately arbitrary. Should
        # not be too long so that we catch bugs in the server that prevent it
        # from shutting down.
        TIMEOUT = 5  # seconds

        try:
            await self._start_proxy(proxy_config)
            await self._proxy.wait_for_line(
                "stderr", "Listening for client connection", TIMEOUT
            )

            await self._start_server(server_config)
            await self._server.wait_for_line(
                "stderr", "Starting pw_rpc server on port", TIMEOUT
            )

            await self._start_client(client_type, client_config)
            # No timeout: the client will only exit once the transfer
            # completes, and this can take a long time for large payloads.
            await self._client.wait_for_termination(None)

            # Wait for the server to exit.
            await self._server.wait_for_termination(TIMEOUT)

        finally:
            # Stop the server, if still running. (Only expected if the
            # wait_for above timed out.)
            if self._server:
                await self._server.terminate_and_wait(TIMEOUT)
            # Stop the proxy. Unlike the server, we expect it to still be
            # running at this stage.
            if self._proxy:
                await self._proxy.terminate_and_wait(TIMEOUT)

            return self.TransferExitCodes(
                self._client.returncode(), self._server.returncode()
            )


class BasicTransfer(NamedTuple):
    id: int
    type: config_pb2.TransferAction.TransferType.ValueType
    data: bytes


class TransferIntegrationTest(unittest.TestCase):
    """A base class for transfer integration tests.

    This significantly reduces the boiler plate required for building
    integration test cases for pw_transfer. This class does not include any
    tests itself, but instead bundles together much of the boiler plate required
    for making an integration test for pw_transfer using this test fixture.
    """

    HARNESS_CONFIG = TransferIntegrationTestHarness.Config()

    @classmethod
    def setUpClass(cls):
        cls.harness = TransferIntegrationTestHarness(cls.HARNESS_CONFIG)

    @staticmethod
    def default_server_config() -> config_pb2.ServerConfig:
        return config_pb2.ServerConfig(
            chunk_size_bytes=216,
            pending_bytes=32 * 1024,
            chunk_timeout_seconds=5,
            transfer_service_retries=4,
            extend_window_divisor=32,
        )

    @staticmethod
    def default_client_config() -> config_pb2.ClientConfig:
        return config_pb2.ClientConfig(
            max_retries=5,
            max_lifetime_retries=1500,
            initial_chunk_timeout_ms=4000,
            chunk_timeout_ms=4000,
        )

    @staticmethod
    def default_proxy_config() -> config_pb2.ProxyConfig:
        return text_format.Parse(
            """
                client_filter_stack: [
                    { hdlc_packetizer: {} },
                    { data_dropper: {rate: 0.01, seed: 1649963713563718435} }
                ]

                server_filter_stack: [
                    { hdlc_packetizer: {} },
                    { data_dropper: {rate: 0.01, seed: 1649963713563718436} }
            ]""",
            config_pb2.ProxyConfig(),
        )

    @staticmethod
    def default_config() -> TransferConfig:
        """Returns a new transfer config with default options."""
        return TransferConfig(
            TransferIntegrationTest.default_server_config(),
            TransferIntegrationTest.default_client_config(),
            TransferIntegrationTest.default_proxy_config(),
        )

    def do_single_write(
        self,
        client_type: str,
        config: TransferConfig,
        resource_id: int,
        data: bytes,
        protocol_version=config_pb2.TransferAction.ProtocolVersion.LATEST,
        permanent_resource_id=False,
        expected_status=status_pb2.StatusCode.OK,
    ) -> None:
        """Performs a single client-to-server write of the provided data."""
        with tempfile.NamedTemporaryFile() as f_payload, tempfile.NamedTemporaryFile() as f_server_output:
            if permanent_resource_id:
                config.server.resources[
                    resource_id
                ].default_destination_path = f_server_output.name
            else:
                config.server.resources[resource_id].destination_paths.append(
                    f_server_output.name
                )
            config.client.transfer_actions.append(
                config_pb2.TransferAction(
                    resource_id=resource_id,
                    file_path=f_payload.name,
                    transfer_type=config_pb2.TransferAction.TransferType.WRITE_TO_SERVER,
                    protocol_version=protocol_version,
                    expected_status=int(expected_status),
                )
            )

            f_payload.write(data)
            f_payload.flush()  # Ensure contents are there to read!
            exit_codes = asyncio.run(
                self.harness.perform_transfers(
                    config.server, client_type, config.client, config.proxy
                )
            )

            self.assertEqual(exit_codes.client, 0)
            self.assertEqual(exit_codes.server, 0)
            if expected_status == status_pb2.StatusCode.OK:
                self.assertEqual(f_server_output.read(), data)

    def do_single_read(
        self,
        client_type: str,
        config: TransferConfig,
        resource_id: int,
        data: bytes,
        protocol_version=config_pb2.TransferAction.ProtocolVersion.LATEST,
        permanent_resource_id=False,
        expected_status=status_pb2.StatusCode.OK,
    ) -> None:
        """Performs a single server-to-client read of the provided data."""
        with tempfile.NamedTemporaryFile() as f_payload, tempfile.NamedTemporaryFile() as f_client_output:
            if permanent_resource_id:
                config.server.resources[
                    resource_id
                ].default_source_path = f_payload.name
            else:
                config.server.resources[resource_id].source_paths.append(
                    f_payload.name
                )
            config.client.transfer_actions.append(
                config_pb2.TransferAction(
                    resource_id=resource_id,
                    file_path=f_client_output.name,
                    transfer_type=config_pb2.TransferAction.TransferType.READ_FROM_SERVER,
                    protocol_version=protocol_version,
                    expected_status=int(expected_status),
                )
            )

            f_payload.write(data)
            f_payload.flush()  # Ensure contents are there to read!
            exit_codes = asyncio.run(
                self.harness.perform_transfers(
                    config.server, client_type, config.client, config.proxy
                )
            )
            self.assertEqual(exit_codes.client, 0)
            self.assertEqual(exit_codes.server, 0)
            if expected_status == status_pb2.StatusCode.OK:
                self.assertEqual(f_client_output.read(), data)

    def do_basic_transfer_sequence(
        self,
        client_type: str,
        config: TransferConfig,
        transfers: Iterable[BasicTransfer],
    ) -> None:
        """Performs multiple reads/writes in a single client/server session."""

        class ReadbackSet(NamedTuple):
            server_file: BinaryIO
            client_file: BinaryIO
            expected_data: bytes

        transfer_results: List[ReadbackSet] = []
        for transfer in transfers:
            server_file = tempfile.NamedTemporaryFile()
            client_file = tempfile.NamedTemporaryFile()

            if (
                transfer.type
                == config_pb2.TransferAction.TransferType.READ_FROM_SERVER
            ):
                server_file.write(transfer.data)
                server_file.flush()
                config.server.resources[transfer.id].source_paths.append(
                    server_file.name
                )
            elif (
                transfer.type
                == config_pb2.TransferAction.TransferType.WRITE_TO_SERVER
            ):
                client_file.write(transfer.data)
                client_file.flush()
                config.server.resources[transfer.id].destination_paths.append(
                    server_file.name
                )
            else:
                raise ValueError('Unknown TransferType')

            config.client.transfer_actions.append(
                config_pb2.TransferAction(
                    resource_id=transfer.id,
                    file_path=client_file.name,
                    transfer_type=transfer.type,
                )
            )

            transfer_results.append(
                ReadbackSet(server_file, client_file, transfer.data)
            )

        exit_codes = asyncio.run(
            self.harness.perform_transfers(
                config.server, client_type, config.client, config.proxy
            )
        )

        for i, result in enumerate(transfer_results):
            with self.subTest(i=i):
                # Need to seek to the beginning of the file to read written
                # data.
                result.client_file.seek(0, 0)
                result.server_file.seek(0, 0)
                self.assertEqual(
                    result.client_file.read(), result.expected_data
                )
                self.assertEqual(
                    result.server_file.read(), result.expected_data
                )

        # Check exit codes at the end as they provide less useful info.
        self.assertEqual(exit_codes.client, 0)
        self.assertEqual(exit_codes.server, 0)


def run_tests_for(test_class_name):
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--server-port',
        type=int,
        help='Port of the integration test server.  The proxy will forward connections to this port',
    )
    parser.add_argument(
        '--client-port',
        type=int,
        help='Port on which to listen for connections from integration test client.',
    )
    parser.add_argument(
        '--java-client-binary',
        type=pathlib.Path,
        default=None,
        help='Path to the Java transfer client to use in tests',
    )
    parser.add_argument(
        '--cpp-client-binary',
        type=pathlib.Path,
        default=None,
        help='Path to the C++ transfer client to use in tests',
    )
    parser.add_argument(
        '--python-client-binary',
        type=pathlib.Path,
        default=None,
        help='Path to the Python transfer client to use in tests',
    )
    parser.add_argument(
        '--server-binary',
        type=pathlib.Path,
        default=None,
        help='Path to the transfer server to use in tests',
    )
    parser.add_argument(
        '--proxy-binary',
        type=pathlib.Path,
        default=None,
        help=(
            'Path to the proxy binary to use in tests to allow interception '
            'of client/server data'
        ),
    )

    (args, passthrough_args) = parser.parse_known_args()

    # Inherrit the default configuration from the class being tested, and only
    # override provided arguments.
    for arg in vars(args):
        val = getattr(args, arg)
        if val:
            setattr(test_class_name.HARNESS_CONFIG, arg, val)

    unittest_args = [sys.argv[0]] + passthrough_args
    unittest.main(argv=unittest_args)
