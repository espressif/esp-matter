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
"""Cross-language pw_transfer tests that verify failure modes.

Note: these tests DO NOT work with older integration test binaries that only
support the legacy transfer protocol.

Usage:

   bazel run pw_transfer/integration_test:expected_errors_test

Command-line arguments must be provided after a double-dash:

   bazel run pw_transfer/integration_test:expected_errors_test -- \
       --server-port 3304

Which tests to run can be specified as command-line arguments:

  bazel run pw_transfer/integration_test:expected_errors_test -- \
      ErrorTransferIntegrationTest.test_write_to_unknown_id

"""

import asyncio
from parameterized import parameterized
import random
import tempfile

from google.protobuf import text_format

from pigweed.pw_transfer.integration_test import config_pb2
from pigweed.pw_protobuf.pw_protobuf_protos import status_pb2
import test_fixture
from test_fixture import TransferIntegrationTestHarness, TransferConfig


class ErrorTransferIntegrationTest(test_fixture.TransferIntegrationTest):
    # Each set of transfer tests uses a different client/server port pair to
    # allow tests to be run in parallel.
    HARNESS_CONFIG = TransferIntegrationTestHarness.Config(
        server_port=3312, client_port=3313
    )

    @parameterized.expand(
        [
            ("cpp"),
            ("java"),
            ("python"),
        ]
    )
    def test_write_to_unknown_id(self, client_type):
        payload = b"Rabbits are the best pets"
        config = self.default_config()
        resource_id = 5

        with tempfile.NamedTemporaryFile() as f_payload, tempfile.NamedTemporaryFile() as f_server_output:
            # Add the resource at a different resource ID.
            config.server.resources[resource_id + 1].destination_paths.append(
                f_server_output.name
            )
            config.client.transfer_actions.append(
                config_pb2.TransferAction(
                    resource_id=resource_id,
                    file_path=f_payload.name,
                    transfer_type=config_pb2.TransferAction.TransferType.WRITE_TO_SERVER,
                    expected_status=status_pb2.StatusCode.NOT_FOUND,
                )
            )

            f_payload.write(payload)
            f_payload.flush()  # Ensure contents are there to read!
            exit_codes = asyncio.run(
                self.harness.perform_transfers(
                    config.server, client_type, config.client, config.proxy
                )
            )

            self.assertEqual(exit_codes.client, 0)
            self.assertEqual(exit_codes.server, 0)

    @parameterized.expand(
        [
            ("cpp"),
            ("java"),
            ("python"),
        ]
    )
    def test_client_write_timeout(self, client_type):
        payload = random.Random(67336391945).randbytes(4321)
        config = TransferConfig(
            self.default_server_config(),
            self.default_client_config(),
            text_format.Parse(
                """
                client_filter_stack: [
                    { hdlc_packetizer: {} },
                    { server_failure: {packets_before_failure: [5]} }
                ]

                server_filter_stack: [
                    { hdlc_packetizer: {} }
            ]""",
                config_pb2.ProxyConfig(),
            ),
        )
        resource_id = 987654321

        # This test deliberately tries to time out the transfer, so because of
        # the retry process the resource ID might be re-initialized multiple
        # times.
        self.do_single_write(
            client_type,
            config,
            resource_id,
            payload,
            permanent_resource_id=True,
            expected_status=status_pb2.StatusCode.DEADLINE_EXCEEDED,
        )

    @parameterized.expand(
        [
            ("cpp"),
            ("java"),
            ("python"),
        ]
    )
    def test_server_write_timeout(self, client_type):
        payload = random.Random(67336391945).randbytes(4321)
        config = TransferConfig(
            self.default_server_config(),
            self.default_client_config(),
            text_format.Parse(
                """
                client_filter_stack: [
                    { hdlc_packetizer: {} }
                ]

                server_filter_stack: [
                    { hdlc_packetizer: {} },
                    { server_failure: {packets_before_failure: [5]} }
            ]""",
                config_pb2.ProxyConfig(),
            ),
        )
        resource_id = 987654321

        # This test deliberately tries to time out the transfer, so because of
        # the retry process the resource ID might be re-initialized multiple
        # times.
        self.do_single_write(
            client_type,
            config,
            resource_id,
            payload,
            permanent_resource_id=True,
            expected_status=status_pb2.StatusCode.DEADLINE_EXCEEDED,
        )

    @parameterized.expand(
        [
            ("cpp"),
            ("java"),
            ("python"),
        ]
    )
    def test_read_from_unknown_id(self, client_type):
        payload = b"Rabbits are the best pets"
        config = self.default_config()
        resource_id = 5

        with tempfile.NamedTemporaryFile() as f_payload, tempfile.NamedTemporaryFile() as f_client_output:
            # Add the resource at a different resource ID.
            config.server.resources[resource_id + 1].source_paths.append(
                f_payload.name
            )
            config.client.transfer_actions.append(
                config_pb2.TransferAction(
                    resource_id=resource_id,
                    file_path=f_client_output.name,
                    transfer_type=config_pb2.TransferAction.TransferType.READ_FROM_SERVER,
                    expected_status=status_pb2.StatusCode.NOT_FOUND,
                )
            )

            f_payload.write(payload)
            f_payload.flush()  # Ensure contents are there to read!
            exit_codes = asyncio.run(
                self.harness.perform_transfers(
                    config.server, client_type, config.client, config.proxy
                )
            )

            self.assertEqual(exit_codes.client, 0)
            self.assertEqual(exit_codes.server, 0)

    @parameterized.expand(
        [
            ("cpp"),
            ("java"),
            ("python"),
        ]
    )
    def test_client_read_timeout(self, client_type):
        payload = random.Random(67336391945).randbytes(4321)
        config = TransferConfig(
            self.default_server_config(),
            self.default_client_config(),
            text_format.Parse(
                """
                client_filter_stack: [
                    { hdlc_packetizer: {} },
                    { server_failure: {packets_before_failure: [5]} }
                ]

                server_filter_stack: [
                    { hdlc_packetizer: {} }
            ]""",
                config_pb2.ProxyConfig(),
            ),
        )
        resource_id = 987654321

        # This test deliberately tries to time out the transfer, so because of
        # the retry process the resource ID might be re-initialized multiple
        # times.
        self.do_single_read(
            client_type,
            config,
            resource_id,
            payload,
            permanent_resource_id=True,
            expected_status=status_pb2.StatusCode.DEADLINE_EXCEEDED,
        )

    @parameterized.expand(
        [
            ("cpp"),
            ("java"),
            ("python"),
        ]
    )
    def test_server_read_timeout(self, client_type):
        payload = random.Random(67336391945).randbytes(4321)
        config = TransferConfig(
            self.default_server_config(),
            self.default_client_config(),
            text_format.Parse(
                """
                client_filter_stack: [
                    { hdlc_packetizer: {} }
                ]

                server_filter_stack: [
                    { hdlc_packetizer: {} },
                    { server_failure: {packets_before_failure: [5]} }
            ]""",
                config_pb2.ProxyConfig(),
            ),
        )
        resource_id = 987654321

        # This test deliberately tries to time out the transfer, so because of
        # the retry process the resource ID might be re-initialized multiple
        # times.
        self.do_single_read(
            client_type,
            config,
            resource_id,
            payload,
            permanent_resource_id=True,
            expected_status=status_pb2.StatusCode.DEADLINE_EXCEEDED,
        )

    @parameterized.expand(
        [
            ("cpp"),
            ("java"),
            ("python"),
        ]
    )
    def test_data_drop_client_lifetime_timeout(self, client_type):
        """Drops the first data chunk of a transfer but allows the rest."""
        payload = random.Random(67336391945).randbytes(1234)

        # This test is expected to hit the lifetime retry count, so make it
        # reasonable.
        client_config = self.default_client_config()
        client_config.max_lifetime_retries = 20
        client_config.chunk_timeout_ms = 1000

        config = TransferConfig(
            self.default_server_config(),
            client_config,
            text_format.Parse(
                """
                client_filter_stack: [
                    { hdlc_packetizer: {} },
                    { window_packet_dropper: { window_packet_to_drop: 0 } }
                ]

                server_filter_stack: [
                    { hdlc_packetizer: {} },
                    { window_packet_dropper: { window_packet_to_drop: 0 } }
            ]""",
                config_pb2.ProxyConfig(),
            ),
        )
        # Resource ID is arbitrary, but deliberately set to be >1 byte.
        resource_id = 7332

        # This test deliberately tries to time out the transfer, so because of
        # the retry process the resource ID might be re-initialized multiple
        # times.
        self.do_single_read(
            client_type,
            config,
            resource_id,
            payload,
            permanent_resource_id=True,
            expected_status=status_pb2.StatusCode.DEADLINE_EXCEEDED,
        )


if __name__ == '__main__':
    test_fixture.run_tests_for(ErrorTransferIntegrationTest)
