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

from dataclasses import dataclass
from pathlib import Path
import tempfile
from typing import NamedTuple, Optional
import unittest

from pw_software_update import dev_sign, root_metadata, update_bundle
from pw_software_update.verify import VerificationError, verify_bundle
from pw_software_update.tuf_pb2 import SignedRootMetadata
from pw_software_update.update_bundle_pb2 import UpdateBundle


def gen_unsigned_bundle(
    signed_root_metadata: Optional[SignedRootMetadata] = None,
    targets_metadata_version: int = 0,
) -> UpdateBundle:
    """Generates an unsigned test bundle."""
    with tempfile.TemporaryDirectory() as tempdir_name:
        targets_root = Path(tempdir_name)
        foo_path = targets_root / 'foo.bin'
        bar_path = targets_root / 'bar.bin'
        baz_path = targets_root / 'baz.bin'
        qux_path = targets_root / 'subdir' / 'qux.exe'
        foo_bytes = b'\xf0\x0b\xa4'
        bar_bytes = b'\x0b\xa4\x99'
        baz_bytes = b'\xba\x59\x06'
        qux_bytes = b'\x8a\xf3\x12'
        foo_path.write_bytes(foo_bytes)
        bar_path.write_bytes(bar_bytes)
        baz_path.write_bytes(baz_bytes)
        (targets_root / 'subdir').mkdir()
        qux_path.write_bytes(qux_bytes)
        targets = {
            foo_path: 'foo',
            bar_path: 'bar',
            baz_path: 'baz',
            qux_path: 'qux',
        }
        return update_bundle.gen_unsigned_update_bundle(
            targets,
            root_metadata=signed_root_metadata,
            targets_metadata_version=targets_metadata_version,
        )


class TestKey(NamedTuple):
    """A test key pair"""

    public: bytes
    private: bytes


@dataclass
class BundleOptions:
    """Parameters used in test bundle generations."""

    root_key_version: int = 0
    root_metadata_version: int = 0
    targets_key_version: int = 0
    targets_metadata_version: int = 0


def gen_signed_bundle(options: BundleOptions) -> UpdateBundle:
    """Generates a test bundle per given options."""
    # Root keys look up table: version->TestKey
    root_keys = {
        0: TestKey(
            private=(
                b'-----BEGIN PRIVATE KEY-----\n'
                b'MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgyk3DEQdl'
                b'346MS5N/quNEneJa4HxkJBETGzlEEKkCmZOhRANCAAThdY5PejbtM2p6'
                b'HtgXs/7YSsvPMWZz9Ui1gAEKrDseHnPzC02MbKjQadRIFZ4hKDcsyz9a'
                b'M6QKLCNrCOqYjw6t'
                b'\n-----END PRIVATE KEY-----\n'
            ),
            public=(
                b'-----BEGIN PUBLIC KEY-----\n'
                b'MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAE4XWOT3o27TNqeh7YF7P+2'
                b'ErLzzFmc/VItYABCqw7Hh5z8wtNjGyo0GnUSBWeISg3LMs/WjOkCiwjaw'
                b'jqmI8OrQ=='
                b'\n-----END PUBLIC KEY-----\n'
            ),
        ),
        1: TestKey(
            private=(
                b'-----BEGIN PRIVATE KEY-----\n'
                b'MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgE3MRbMxo'
                b'Gv3I/Ok/0qE8GV/mQuIbZo9kk+AsJnYetQ6hRANCAAQ5UhycwdcfYe34'
                b'NpmG32t0klnKlrUbk3LyvYLq5uDWG2MfP3L0ciNFsEnW7vHpqqjKsoru'
                b'Qt30G10K7D+reC77'
                b'\n-----END PRIVATE KEY-----\n'
            ),
            public=(
                b'-----BEGIN PUBLIC KEY-----\n'
                b'MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEOVIcnMHXH2Ht+DaZht9rd'
                b'JJZypa1G5Ny8r2C6ubg1htjHz9y9HIjRbBJ1u7x6aqoyrKK7kLd9BtdCu'
                b'w/q3gu+w=='
                b'\n-----END PUBLIC KEY-----\n'
            ),
        ),
    }

    # Targets keys look up table: version->TestKey
    targets_keys = {
        0: TestKey(
            private=(
                b'-----BEGIN PRIVATE KEY-----\n'
                b'MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgkMEZ0u84'
                b'HzC51nhhf2ZykPj6WfAjBxXVWndjVdn6bh6hRANCAAT1QzqpFknSAhbA'
                b'uOjy2NuusFOUpeC6TBWM6WeC5JKJgys3gwOoyU0OdomAu9wK6I1Qoe70'
                b'6PUMbWLpyQ10ThVM'
                b'\n-----END PRIVATE KEY-----\n'
            ),
            public=(
                b'-----BEGIN PUBLIC KEY-----\n'
                b'MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAE9UM6qRZJ0gIWwLjo8tjbr'
                b'rBTlKXgukwVjOlnguSSiYMrN4MDqMlNDnaJgLvcCuiNUKHu9Oj1DG1i6c'
                b'kNdE4VTA=='
                b'\n-----END PUBLIC KEY-----\n'
            ),
        ),
        1: TestKey(
            private=(
                b'-----BEGIN PRIVATE KEY-----\n'
                b'MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQg+Q+u2KoO'
                b'CwpY1HEKDTIjQXmTlxhoo3gVkE7nrtHhMemhRANCAASgc+0AHCfUxoHy'
                b'+ZkSslLvMufiDqGPABvfuKzHd0wUWs2Y0eIvQc7tsBP0bcuJsFuxvL6a'
                b'8Ek7y3kUmFWVL01v'
                b'\n-----END PRIVATE KEY-----\n'
            ),
            public=(
                b'-----BEGIN PUBLIC KEY-----\n'
                b'MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEoHPtABwn1MaB8vmZErJS7'
                b'zLn4g6hjwAb37isx3dMFFrNmNHiL0HO7bAT9G3LibBbsby+mvBJO8t5FJ'
                b'hVlS9Nbw=='
                b'\n-----END PUBLIC KEY-----\n'
            ),
        ),
    }

    unsigned_root = root_metadata.gen_root_metadata(
        root_metadata.RootKeys([root_keys[options.root_key_version].public]),
        root_metadata.TargetsKeys(
            [targets_keys[options.targets_key_version].public]
        ),
        version=options.root_metadata_version,
    )

    serialized_root = unsigned_root.SerializeToString()
    signed_root = SignedRootMetadata(serialized_root_metadata=serialized_root)
    signed_root = dev_sign.sign_root_metadata(
        signed_root, root_keys[options.root_key_version].private
    )
    # Additionaly sign the root metadata with the previous version of root key
    # to enable upgrading from the previous root.
    if options.root_key_version > 0:
        signed_root = dev_sign.sign_root_metadata(
            signed_root, root_keys[options.root_key_version - 1].private
        )

    unsigned_bundle = gen_unsigned_bundle(
        signed_root_metadata=signed_root,
        targets_metadata_version=options.targets_metadata_version,
    )
    signed_bundle = dev_sign.sign_update_bundle(
        unsigned_bundle, targets_keys[options.targets_key_version].private
    )

    return signed_bundle


class VerifyBundleTest(unittest.TestCase):
    """Bundle verification test cases."""

    def test_self_verification(self):  # pylint: disable=no-self-use
        incoming = gen_signed_bundle(BundleOptions())
        verify_bundle(incoming, trusted=incoming)

    def test_root_key_rotation(self):  # pylint: disable=no-self-use
        trusted = gen_signed_bundle(BundleOptions(root_key_version=0))
        incoming = gen_signed_bundle(BundleOptions(root_key_version=1))
        verify_bundle(incoming, trusted)

    def test_root_metadata_anti_rollback(self):
        trusted = gen_signed_bundle(BundleOptions(root_metadata_version=1))
        incoming = gen_signed_bundle(BundleOptions(root_metadata_version=0))
        with self.assertRaises(VerificationError):
            verify_bundle(incoming, trusted)

    def test_root_metadata_anti_rollback_with_key_rotation(self):
        trusted = gen_signed_bundle(
            BundleOptions(root_key_version=0, root_metadata_version=1)
        )
        incoming = gen_signed_bundle(
            BundleOptions(root_key_version=1, root_metadata_version=0)
        )
        # Anti-rollback enforced regardless of key rotation.
        with self.assertRaises(VerificationError):
            verify_bundle(incoming, trusted)

    def test_missing_root(self):
        incoming = gen_signed_bundle(BundleOptions())
        incoming.ClearField('root_metadata')
        with self.assertRaises(VerificationError):
            verify_bundle(incoming, trusted=incoming)

    def test_targets_key_rotation(self):  # pylint: disable=no-self-use
        trusted = gen_signed_bundle(BundleOptions(targets_key_version=0))
        incoming = gen_signed_bundle(BundleOptions(targets_key_version=1))
        verify_bundle(incoming, trusted)

    def test_targets_metadata_anti_rollback(self):
        trusted = gen_signed_bundle(BundleOptions(targets_metadata_version=1))
        incoming = gen_signed_bundle(BundleOptions(targets_metadata_version=0))
        with self.assertRaises(VerificationError):
            verify_bundle(incoming, trusted)

    def test_targets_fastforward_recovery(self):  # pylint: disable=no-self-use
        trusted = gen_signed_bundle(
            BundleOptions(targets_key_version=0, targets_metadata_version=999)
        )
        # Revoke key and bring back the metadata version.
        incoming = gen_signed_bundle(
            BundleOptions(targets_key_version=1, targets_metadata_version=0)
        )
        # Anti-rollback is not enforced upon key rotation.
        verify_bundle(incoming, trusted)


if __name__ == '__main__':
    unittest.main()
