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
"""Cross-language pw_transfer tests that do several transfers per session.

Usage:

   bazel run pw_transfer/integration_test:multi_transfer_test

Command-line arguments must be provided after a double-dash:

   bazel run pw_transfer/integration_test:multi_transfer_test -- \
       --server-port 3304

Which tests to run can be specified as command-line arguments:

  bazel run pw_transfer/integration_test:multi_transfer_test -- \
      MultiTransferIntegrationTest.test_write_to_same_id_1_java

"""

from parameterized import parameterized
import random
from typing import List

import test_fixture
from test_fixture import TransferIntegrationTestHarness, BasicTransfer
from pigweed.pw_transfer.integration_test import config_pb2


class MultiTransferIntegrationTest(test_fixture.TransferIntegrationTest):
    # Each set of transfer tests uses a different client/server port pair to
    # allow tests to be run in parallel.
    HARNESS_CONFIG = TransferIntegrationTestHarness.Config(
        server_port=3308, client_port=3309
    )

    @parameterized.expand(
        [
            ("cpp"),
            ("java"),
            ("python"),
        ]
    )
    def test_write_to_same_id(self, client_type):
        rng = random.Random(1533659510898)
        config = self.default_config()
        resource_id = 5
        transfers: List[BasicTransfer] = []
        for i in range(1, 6):
            transfers.append(
                BasicTransfer(
                    id=resource_id,
                    type=config_pb2.TransferAction.TransferType.WRITE_TO_SERVER,
                    data=rng.randbytes(rng.randrange(213, 1111)),
                )
            )

        self.do_basic_transfer_sequence(client_type, config, transfers)

    @parameterized.expand(
        [
            ("cpp"),
            ("java"),
            ("python"),
        ]
    )
    def test_read_from_same_id(self, client_type):
        rng = random.Random(1533659510898)
        config = self.default_config()
        resource_id = 5
        transfers: List[BasicTransfer] = []
        for i in range(1, 6):
            transfers.append(
                BasicTransfer(
                    id=resource_id,
                    type=config_pb2.TransferAction.TransferType.READ_FROM_SERVER,
                    data=rng.randbytes(rng.randrange(213, 1111)),
                )
            )

        self.do_basic_transfer_sequence(client_type, config, transfers)

    @parameterized.expand(
        [
            ("cpp"),
            ("java"),
            ("python"),
        ]
    )
    def test_read_write_with_same_id(self, client_type):
        rng = random.Random(1533659510898)
        config = self.default_config()
        resource_id = 53333333
        transfers: List[BasicTransfer] = []
        for i in range(1, 6):
            transfer_type = (
                config_pb2.TransferAction.TransferType.READ_FROM_SERVER
                if i % 2 == 0
                else config_pb2.TransferAction.TransferType.WRITE_TO_SERVER
            )
            transfers.append(
                BasicTransfer(
                    id=resource_id,
                    type=config_pb2.TransferAction.TransferType.READ_FROM_SERVER,
                    data=rng.randbytes(rng.randrange(213, 1111)),
                )
            )

        self.do_basic_transfer_sequence(client_type, config, transfers)


if __name__ == '__main__':
    test_fixture.run_tests_for(MultiTransferIntegrationTest)
