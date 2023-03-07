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
"""Unit tests for pw_software_update/root_metadata.py."""

import unittest

from pw_software_update import metadata
from pw_software_update.root_metadata import (
    RootKeys,
    TargetsKeys,
    gen_root_metadata,
)


class GenRootMetadataTest(unittest.TestCase):
    """Test the generation of root metadata."""

    def setUp(self):
        self.root_key_public = (
            b'-----BEGIN PUBLIC KEY-----\n'
            b'MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAE4XWOT3o27TNqeh7YF7P+2ErLzzFm'
            b'c/VItYABCqw7Hh5z8wtNjGyo0GnUSBWeISg3LMs/WjOkCiwjawjqmI8OrQ=='
            b'\n-----END PUBLIC KEY-----\n'
        )

        self.targets_key_public = (
            b'-----BEGIN PUBLIC KEY-----\n'
            b'MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAE9UM6qRZJ0gIWwLjo8tjbrrBTlKXg'
            b'ukwVjOlnguSSiYMrN4MDqMlNDnaJgLvcCuiNUKHu9Oj1DG1i6ckNdE4VTA=='
            b'\n-----END PUBLIC KEY-----\n'
        )

    def test_multiple_keys(self) -> None:
        """Checks that multiple keys generates multiple KeyMappings and
        SignatureRequirements."""
        root_metadata = gen_root_metadata(
            RootKeys([self.root_key_public]),
            TargetsKeys([self.targets_key_public]),
            version=42,
        )

        self.assertEqual(len(root_metadata.keys), 2)
        self.assertEqual(
            len(root_metadata.root_signature_requirement.key_ids), 1
        )
        self.assertEqual(root_metadata.root_signature_requirement.threshold, 1)
        self.assertEqual(
            len(root_metadata.targets_signature_requirement.key_ids), 1
        )
        self.assertEqual(
            root_metadata.targets_signature_requirement.threshold, 1
        )
        self.assertEqual(root_metadata.common_metadata.version, 42)
        self.assertEqual(
            root_metadata.common_metadata.role, metadata.RoleType.ROOT.value
        )


if __name__ == '__main__':
    unittest.main()
