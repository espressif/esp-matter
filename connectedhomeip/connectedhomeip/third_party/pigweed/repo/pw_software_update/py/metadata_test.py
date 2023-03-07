# Copyright 2021 The Pigweed Authors
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
"""Unit tests for pw_software_update/metadata.py."""

import unittest

from pw_software_update import metadata
from pw_software_update.tuf_pb2 import HashFunction


class GenTargetsMetadataTest(unittest.TestCase):
    """Test the generation of targets metadata."""

    def test_multiple_targets(self):
        """Checks that multiple targets generates multiple TargetFiles."""
        target_payloads = {
            'foo': b'\x1e\xe7',
            'bar': b'\x12\x34',
        }
        targets_metadata = metadata.gen_targets_metadata(
            target_payloads, (HashFunction.SHA256,), version=42
        )
        self.assertEqual(2, len(targets_metadata.target_files))
        self.assertEqual(
            metadata.RoleType.TARGETS.value,
            targets_metadata.common_metadata.role,
        )
        self.assertEqual(42, targets_metadata.common_metadata.version)


class GenHashesTest(unittest.TestCase):
    """Test the generation of hashes."""

    def test_sha256(self):
        """Checks that SHA256 hashes are computed and stored properly."""
        data = b'\x1e\xe7'
        sha256_hash = metadata.gen_hashes(data, (HashFunction.SHA256,))[0]
        self.assertEqual(
            '9f36ce605a3b28110d2a25ec36bdfff86059086cbd53c9efc1428ef01070515d',
            sha256_hash.hash.hex(),
        )


if __name__ == '__main__':
    unittest.main()
