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
"""Unit tests for pw_software_update/dev_sign.py."""

import unittest

from pw_software_update.dev_sign import sign_root_metadata, sign_update_bundle
from pw_software_update.root_metadata import (
    gen_root_metadata,
    RootKeys,
    TargetsKeys,
)
from pw_software_update.tuf_pb2 import SignedRootMetadata, SignedTargetsMetadata
from pw_software_update.update_bundle_pb2 import UpdateBundle


class RootMetadataSigningTest(unittest.TestCase):
    """Test Root Metadata signing."""

    def setUp(self) -> None:
        self.root_key = (
            b'-----BEGIN PRIVATE KEY-----\n'
            b'MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgyk3DEQdl346MS5N/'
            b'quNEneJa4HxkJBETGzlEEKkCmZOhRANCAAThdY5PejbtM2p6HtgXs/7YSsvPMWZz'
            b'9Ui1gAEKrDseHnPzC02MbKjQadRIFZ4hKDcsyz9aM6QKLCNrCOqYjw6t'
            b'\n-----END PRIVATE KEY-----\n'
        )

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

        root_metadata = gen_root_metadata(
            RootKeys([self.root_key_public]),
            TargetsKeys([self.targets_key_public]),
        )
        self.root_metadata = SignedRootMetadata(
            serialized_root_metadata=root_metadata.SerializeToString()
        )

        self.another_signing_key = (
            b'-----BEGIN PRIVATE KEY-----\n'
            b'MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQg5OIalt8DcZYeEf/4'
            b'5/iIX6jqM0I5t4dScAdcmgNF9vKhRANCAAQdMBqcn//pXIwss9nLEVjz+4Mz4oVt'
            b'hKTFLqlzwKHZngL/0IyH6FC+4bq5CnR43WADPAyJHo18V0NCO/8QFQ2c'
            b'\n-----END PRIVATE KEY-----\n'
        )

    def test_typical_signing(self):
        signed = sign_root_metadata(self.root_metadata, self.root_key)
        self.assertEqual(len(signed.signatures), 1)
        signed = sign_root_metadata(signed, self.another_signing_key)
        self.assertEqual(len(signed.signatures), 2)


class BundleSigningTest(unittest.TestCase):
    """Test UpdateBundle signing."""

    def setUp(self):
        self.targets_key = (
            b'-----BEGIN PRIVATE KEY-----\n'
            b'MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgkMEZ0u84HzC51nhh'
            b'f2ZykPj6WfAjBxXVWndjVdn6bh6hRANCAAT1QzqpFknSAhbAuOjy2NuusFOUpeC6'
            b'TBWM6WeC5JKJgys3gwOoyU0OdomAu9wK6I1Qoe706PUMbWLpyQ10ThVM'
            b'\n-----END PRIVATE KEY-----\n'
        )

        self.update_bundle = UpdateBundle(
            targets_metadata=dict(
                targets=SignedTargetsMetadata(
                    serialized_targets_metadata=b'blahblah'
                )
            )
        )

    def test_typical_signing(self):
        signed = sign_update_bundle(self.update_bundle, self.targets_key)
        self.assertEqual(len(signed.targets_metadata['targets'].signatures), 1)


if __name__ == '__main__':
    unittest.main()
