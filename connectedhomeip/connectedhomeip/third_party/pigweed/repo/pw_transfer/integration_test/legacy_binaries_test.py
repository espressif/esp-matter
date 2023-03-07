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
"""A variety of transfer tests that validate backwards compatibility.

Usage:

   bazel run pw_transfer/integration_test:legacy_binaries_test

Command-line arguments must be provided after a double-dash:

   bazel run pw_transfer/integration_test:legacy_binaries_test -- \
       --server-port 3304

Which tests to run can be specified as command-line arguments:

  bazel run pw_transfer/integration_test:legacy_binaries_test -- \
      LegacyClientTransferIntegrationTests.test_small_client_write_0_cpp

"""

import itertools
from parameterized import parameterized
import random

from pigweed.pw_transfer.integration_test import config_pb2
import test_fixture
from test_fixture import TransferIntegrationTestHarness
from rules_python.python.runfiles import runfiles

# Each set of transfer tests uses a different client/server port pair to
# allow tests to be run in parallel.
_SERVER_PORT = 3314
_CLIENT_PORT = 3315


# NOTE: These backwards compatibility tests DO NOT include tests that verify
# expected error cases (e.g. timeouts, unknown resource ID) because legacy
# integration test clients did not support the ability to check those.
# Additionally, there are deliberately NOT back-to-back read/write transfers
# because of known issues with transfer cleanup in the legacy transfer protocol.
class LegacyTransferIntegrationTest(test_fixture.TransferIntegrationTest):
    """This base class defines the tests to run, but isn't run directly."""

    # Explicitly use UNKNOWN_VERSION (the default value of
    # TransferAction.protocol_version), as that will cause protocol_version to
    # be omitted from the generated text proto, which is critical for the legacy
    # client that doesn't support transfer version specifications (and will
    # cause a proto parse error).
    PROTOCOL_VERSION = config_pb2.TransferAction.ProtocolVersion.UNKNOWN_VERSION
    LEGACY_SERVER = False
    LEGACY_CLIENT = False

    def default_config(self) -> test_fixture.TransferConfig:
        # The legacy binaries aren't aware of the max_lifetime_retries field,
        # which was added more recently. Clear it so it isn't encoded into the
        # serialized message.
        config = super().default_config()
        config.client.max_lifetime_retries = 0
        return config

    @parameterized.expand(
        [
            ("cpp"),
            ("java"),
            ("python"),
        ]
    )
    def test_single_byte_client_write(self, client_type):
        if not (self.LEGACY_SERVER or self.LEGACY_CLIENT):
            self.skipTest("No legacy binary in use, skipping")

        if not self.LEGACY_SERVER and (
            client_type == "java" or client_type == "python"
        ):
            self.skipTest("Java and Python legacy clients not yet set up")

        payload = b"?"
        config = self.default_config()
        resource_id = 5
        self.do_single_write(
            "cpp", config, resource_id, payload, self.PROTOCOL_VERSION
        )

    @parameterized.expand(
        [
            ("cpp"),
            ("java"),
            ("python"),
        ]
    )
    def test_small_client_write(self, client_type):
        if not (self.LEGACY_SERVER or self.LEGACY_CLIENT):
            self.skipTest("No legacy binary in use, skipping")

        if not self.LEGACY_SERVER and (
            client_type == "java" or client_type == "python"
        ):
            self.skipTest("Java and Python legacy clients not yet set up")

        payload = b"some data"
        config = self.default_config()
        resource_id = 5
        self.do_single_write(
            "cpp", config, resource_id, payload, self.PROTOCOL_VERSION
        )

    @parameterized.expand(
        [
            ("cpp"),
            ("java"),
            ("python"),
        ]
    )
    def test_medium_hdlc_escape_client_write(self, client_type):
        if not (self.LEGACY_SERVER or self.LEGACY_CLIENT):
            self.skipTest("No legacy binary in use, skipping")

        if not self.LEGACY_SERVER and (
            client_type == "java" or client_type == "python"
        ):
            self.skipTest("Java and Python legacy clients not yet set up")

        payload = b"~" * 8731
        config = self.default_config()
        resource_id = 12345678
        self.do_single_write(
            "cpp", config, resource_id, payload, self.PROTOCOL_VERSION
        )

    @parameterized.expand(
        [
            ("cpp"),
            ("java"),
            ("python"),
        ]
    )
    def test_medium_random_data_client_write(self, client_type):
        if not (self.LEGACY_SERVER or self.LEGACY_CLIENT):
            self.skipTest("No legacy binary in use, skipping")

        if not self.LEGACY_SERVER and (
            client_type == "java" or client_type == "python"
        ):
            self.skipTest("Java and Python legacy clients not yet set up")

        rng = random.Random(1533659510898)
        payload = rng.randbytes(13713)
        config = self.default_config()
        resource_id = 12345678
        self.do_single_write(
            "cpp", config, resource_id, payload, self.PROTOCOL_VERSION
        )

    @parameterized.expand(
        [
            ("cpp"),
            ("java"),
            ("python"),
        ]
    )
    def test_single_byte_client_read(self, client_type):
        if not (self.LEGACY_SERVER or self.LEGACY_CLIENT):
            self.skipTest("No legacy binary in use, skipping")

        if not self.LEGACY_SERVER and (
            client_type == "java" or client_type == "python"
        ):
            self.skipTest("Java and Python legacy clients not yet set up")

        payload = b"?"
        config = self.default_config()
        resource_id = 5
        self.do_single_read(
            "cpp", config, resource_id, payload, self.PROTOCOL_VERSION
        )

    @parameterized.expand(
        [
            ("cpp"),
            ("java"),
            ("python"),
        ]
    )
    def test_small_client_read(self, client_type):
        if not (self.LEGACY_SERVER or self.LEGACY_CLIENT):
            self.skipTest("No legacy binary in use, skipping")

        if not self.LEGACY_SERVER and (
            client_type == "java" or client_type == "python"
        ):
            self.skipTest("Java and Python legacy clients not yet set up")

        payload = b"some data"
        config = self.default_config()
        resource_id = 5
        self.do_single_read(
            "cpp", config, resource_id, payload, self.PROTOCOL_VERSION
        )

    @parameterized.expand(
        [
            ("cpp"),
            ("java"),
            ("python"),
        ]
    )
    def test_medium_hdlc_escape_client_read(self, client_type):
        if not (self.LEGACY_SERVER or self.LEGACY_CLIENT):
            self.skipTest("No legacy binary in use, skipping")

        if self.LEGACY_SERVER:
            self.skipTest("Legacy server has HDLC buffer sizing issues")

        payload = b"~" * 8731
        config = self.default_config()
        resource_id = 5
        self.do_single_read(
            "cpp", config, resource_id, payload, self.PROTOCOL_VERSION
        )

    @parameterized.expand(
        [
            ("cpp"),
            ("java"),
            ("python"),
        ]
    )
    def test_medium_random_data_client_read(self, client_type):
        if not (self.LEGACY_SERVER or self.LEGACY_CLIENT):
            self.skipTest("No legacy binary in use, skipping")

        if self.LEGACY_SERVER:
            self.skipTest("Legacy server has HDLC buffer sizing issues")

        rng = random.Random(1533659510898)
        payload = rng.randbytes(13713)
        config = self.default_config()
        resource_id = 5
        self.do_single_read(
            "cpp", config, resource_id, payload, self.PROTOCOL_VERSION
        )


class LegacyClientTransferIntegrationTests(LegacyTransferIntegrationTest):
    r = runfiles.Create()
    client_binary = r.Rlocation("pw_transfer_test_binaries/cpp_client_528098d5")
    HARNESS_CONFIG = TransferIntegrationTestHarness.Config(
        cpp_client_binary=client_binary,
        server_port=_SERVER_PORT,
        client_port=_CLIENT_PORT,
    )
    LEGACY_CLIENT = True


class LegacyServerTransferIntegrationTests(LegacyTransferIntegrationTest):
    r = runfiles.Create()
    server_binary = r.Rlocation("pw_transfer_test_binaries/server_528098d5")
    HARNESS_CONFIG = TransferIntegrationTestHarness.Config(
        server_binary=server_binary,
        server_port=_SERVER_PORT,
        client_port=_CLIENT_PORT,
    )
    LEGACY_SERVER = True


if __name__ == '__main__':
    test_fixture.run_tests_for(LegacyClientTransferIntegrationTests)
    test_fixture.run_tests_for(LegacyServerTransferIntegrationTests)
