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
"""Script for generating test bundles"""

import argparse
import subprocess
import sys
from typing import Dict, Optional

from pw_software_update import dev_sign, keys, metadata, root_metadata
from pw_software_update.update_bundle_pb2 import Manifest, UpdateBundle
from pw_software_update.tuf_pb2 import (
    RootMetadata,
    SignedRootMetadata,
    TargetsMetadata,
    SignedTargetsMetadata,
)

from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.primitives import serialization

HEADER = """// Copyright 2021 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy
// of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

#pragma once

#include "pw_bytes/span.h"

"""

TEST_DEV_KEY = """-----BEGIN PRIVATE KEY-----
MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgVgMQBOTJyx1xOafy
WTs2VkACf7Uo3RbP9Vun+oKXtMihRANCAATV7XJljxeUs2z2wqM5Q/kohAra1620
zXT90N9a3UR+IHksTd1OA7wFq220IQB/e4eVzbcOprN0MMMuSgXMxL8p
-----END PRIVATE KEY-----"""

TEST_PROD_KEY = """-----BEGIN PRIVATE KEY-----
MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQg73MLNmB/fPNX75Pl
YdynPtJkM2gGOWfIcHDuwuxSQmqhRANCAARpvjrXkjG2Fp+ZgREtxeTBBmJmWGS9
8Ny2tXY+Qggzl77G7wvCNF5+koz7ecsV6sKjK+dFiAXOIdqlga7p2j0A
-----END PRIVATE KEY-----"""

TEST_TARGETS_DEV_KEY = """-----BEGIN PRIVATE KEY-----
MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQggRCrido5vZOnkULH
sxQDt9Qoe/TlEKoqa1bhO1HFbi6hRANCAASVwdXbGWM7+f/r+Z2W6Dbd7CQA0Cbb
pkBv5PnA+DZnCkFhLW2kTn89zQv8W1x4m9maoINp9QPXQ4/nXlrVHqDg
-----END PRIVATE KEY-----"""

TEST_TARGETS_PROD_KEY = """-----BEGIN PRIVATE KEY-----
MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgx2VdB2EsUKghuLMG
RmxzqX2jnLTq5pxsFgO5Rrf5jlehRANCAASVijeDpemxVSlgZOOW0yvwE5QkXkq0
geWonkusMP0+MXopnmN0QlpgaCnG40TSr/W+wFjRmNCklL4dXk01oCwD
-----END PRIVATE KEY-----"""

TEST_ROOT_VERSION = 2
TEST_TARGETS_VERSION = 2

USER_MANIFEST_FILE_NAME = 'user_manifest'

TARGET_FILES = {
    'file1': 'file 1 content'.encode(),
    'file2': 'file 2 content'.encode(),
    USER_MANIFEST_FILE_NAME: 'user manfiest content'.encode(),
}


def byte_array_declaration(data: bytes, name: str) -> str:
    """Generates a byte C array declaration for a byte array"""
    type_name = '[[maybe_unused]] const std::byte'
    byte_str = ''.join([f'std::byte{{0x{b:02x}}},' for b in data])
    array_body = f'{{{byte_str}}}'
    return f'{type_name} {name}[] = {array_body};'


def proto_array_declaration(proto, name: str) -> str:
    """Generates a byte array declaration for a proto"""
    return byte_array_declaration(proto.SerializeToString(), name)


def private_key_public_pem_bytes(key: ec.EllipticCurvePrivateKey) -> bytes:
    """Serializes the public part of a private key in PEM format"""
    return key.public_key().public_bytes(
        serialization.Encoding.PEM,
        serialization.PublicFormat.SubjectPublicKeyInfo,
    )


def private_key_private_pem_bytes(key: ec.EllipticCurvePrivateKey) -> bytes:
    """Serializes the private part of a private key in PEM format"""
    return key.private_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PrivateFormat.PKCS8,
        encryption_algorithm=serialization.NoEncryption(),
    )


class Bundle:
    """A helper for test UpdateBundle generation"""

    def __init__(self):
        self._root_dev_key = serialization.load_pem_private_key(
            TEST_DEV_KEY.encode(), None
        )
        self._root_prod_key = serialization.load_pem_private_key(
            TEST_PROD_KEY.encode(), None
        )
        self._targets_dev_key = serialization.load_pem_private_key(
            TEST_TARGETS_DEV_KEY.encode(), None
        )
        self._targets_prod_key = serialization.load_pem_private_key(
            TEST_TARGETS_PROD_KEY.encode(), None
        )
        self._payloads: Dict[str, bytes] = {}
        # Adds some update files.
        for key, value in TARGET_FILES.items():
            self.add_payload(key, value)

    def add_payload(self, name: str, payload: bytes) -> None:
        """Adds a payload to the bundle"""
        self._payloads[name] = payload

    def generate_dev_root_metadata(self) -> RootMetadata:
        """Generates a root metadata with the dev key"""
        # The dev root metadata contains both the prod and the dev public key,
        # so that it can rotate to prod. But it will only use a dev targets
        # key.
        return root_metadata.gen_root_metadata(
            root_metadata.RootKeys(
                [
                    private_key_public_pem_bytes(self._root_dev_key),
                    private_key_public_pem_bytes(self._root_prod_key),
                ]
            ),
            root_metadata.TargetsKeys(
                [private_key_public_pem_bytes(self._targets_dev_key)]
            ),
            TEST_ROOT_VERSION,
        )

    def generate_prod_root_metadata(self) -> RootMetadata:
        """Generates a root metadata with the prod key"""
        # The prod root metadta contains only the prod public key and uses the
        # prod targets key
        return root_metadata.gen_root_metadata(
            root_metadata.RootKeys(
                [private_key_public_pem_bytes(self._root_prod_key)]
            ),
            root_metadata.TargetsKeys(
                [private_key_public_pem_bytes(self._targets_prod_key)]
            ),
            TEST_ROOT_VERSION,
        )

    def generate_dev_signed_root_metadata(self) -> SignedRootMetadata:
        """Generates a dev signed root metadata"""
        signed_root = SignedRootMetadata()
        root_metadata_proto = self.generate_dev_root_metadata()
        signed_root.serialized_root_metadata = (
            root_metadata_proto.SerializeToString()
        )
        return dev_sign.sign_root_metadata(
            signed_root, private_key_private_pem_bytes(self._root_dev_key)
        )

    def generate_prod_signed_root_metadata(
        self, root_metadata_proto: Optional[RootMetadata] = None
    ) -> SignedRootMetadata:
        """Generates a root metadata signed by the prod key"""
        if not root_metadata_proto:
            root_metadata_proto = self.generate_prod_root_metadata()

        signed_root = SignedRootMetadata(
            serialized_root_metadata=root_metadata_proto.SerializeToString()
        )

        return dev_sign.sign_root_metadata(
            signed_root, private_key_private_pem_bytes(self._root_prod_key)
        )

    def generate_targets_metadata(self) -> TargetsMetadata:
        """Generates the targets metadata"""
        targets = metadata.gen_targets_metadata(
            self._payloads, metadata.DEFAULT_HASHES, TEST_TARGETS_VERSION
        )
        return targets

    def generate_unsigned_bundle(
        self,
        targets_metadata: Optional[TargetsMetadata] = None,
        signed_root_metadata: Optional[SignedRootMetadata] = None,
    ) -> UpdateBundle:
        """Generate an unsigned (targets metadata) update bundle"""
        bundle = UpdateBundle()

        if not targets_metadata:
            targets_metadata = self.generate_targets_metadata()

        if signed_root_metadata:
            bundle.root_metadata.CopyFrom(signed_root_metadata)

        bundle.targets_metadata['targets'].CopyFrom(
            SignedTargetsMetadata(
                serialized_targets_metadata=targets_metadata.SerializeToString()
            )
        )

        for name, payload in self._payloads.items():
            bundle.target_payloads[name] = payload

        return bundle

    def generate_dev_signed_bundle(
        self,
        targets_metadata_override: Optional[TargetsMetadata] = None,
        signed_root_metadata: Optional[SignedRootMetadata] = None,
    ) -> UpdateBundle:
        """Generate a dev signed update bundle"""
        return dev_sign.sign_update_bundle(
            self.generate_unsigned_bundle(
                targets_metadata_override, signed_root_metadata
            ),
            private_key_private_pem_bytes(self._targets_dev_key),
        )

    def generate_prod_signed_bundle(
        self,
        targets_metadata_override: Optional[TargetsMetadata] = None,
        signed_root_metadata: Optional[SignedRootMetadata] = None,
    ) -> UpdateBundle:
        """Generate a prod signed update bundle"""
        # The targets metadata in a prod signed bundle can only be verified
        # by a prod signed root. Because it is signed by the prod targets key.
        # The prod signed root however, can be verified by a dev root.
        return dev_sign.sign_update_bundle(
            self.generate_unsigned_bundle(
                targets_metadata_override, signed_root_metadata
            ),
            private_key_private_pem_bytes(self._targets_prod_key),
        )

    def generate_manifest(self) -> Manifest:
        """Generates the manifest"""
        manifest = Manifest()
        manifest.targets_metadata['targets'].CopyFrom(
            self.generate_targets_metadata()
        )
        if USER_MANIFEST_FILE_NAME in self._payloads:
            manifest.user_manifest = self._payloads[USER_MANIFEST_FILE_NAME]
        return manifest


def parse_args():
    """Setup argparse."""
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "output_header", help="output path of the generated C header"
    )
    return parser.parse_args()


def main() -> int:
    """Main"""
    # TODO(b/237580538): Refactor the code so that each test bundle generation
    # is done in a separate function or script.
    # pylint: disable=too-many-locals
    args = parse_args()

    test_bundle = Bundle()

    dev_signed_root = test_bundle.generate_dev_signed_root_metadata()
    dev_signed_bundle = test_bundle.generate_dev_signed_bundle()
    dev_signed_bundle_with_root = test_bundle.generate_dev_signed_bundle(
        signed_root_metadata=dev_signed_root
    )
    unsigned_bundle_with_root = test_bundle.generate_unsigned_bundle(
        signed_root_metadata=dev_signed_root
    )
    manifest_proto = test_bundle.generate_manifest()
    prod_signed_root = test_bundle.generate_prod_signed_root_metadata()
    prod_signed_bundle = test_bundle.generate_prod_signed_bundle(
        None, prod_signed_root
    )
    dev_signed_bundle_with_prod_root = test_bundle.generate_dev_signed_bundle(
        signed_root_metadata=prod_signed_root
    )

    # Generates a prod root metadata that fails signature verification against
    # the dev root (i.e. it has a bad prod signature). This is done by making
    # a bad prod signature.
    bad_prod_signature = test_bundle.generate_prod_root_metadata()
    signed_bad_prod_signature = test_bundle.generate_prod_signed_root_metadata(
        bad_prod_signature
    )
    # Compromises the signature.
    signed_bad_prod_signature.signatures[0].sig = b'1' * 64
    signed_bad_prod_signature_bundle = test_bundle.generate_prod_signed_bundle(
        None, signed_bad_prod_signature
    )

    # Generates a prod root metadtata that fails to verify itself. Specifically,
    # the prod signature cannot be verified by the key in the incoming root
    # metadata. This is done by dev signing a prod root metadata.
    # pylint: disable=line-too-long
    signed_mismatched_root_key_and_signature = SignedRootMetadata(
        serialized_root_metadata=test_bundle.generate_prod_root_metadata().SerializeToString()
    )
    # pylint: enable=line-too-long
    dev_root_key = serialization.load_pem_private_key(
        TEST_DEV_KEY.encode(), None
    )
    signature = keys.create_ecdsa_signature(
        signed_mismatched_root_key_and_signature.serialized_root_metadata,
        private_key_private_pem_bytes(dev_root_key),  # type: ignore
    )
    signed_mismatched_root_key_and_signature.signatures.append(signature)
    mismatched_root_key_and_signature_bundle = (
        test_bundle.generate_prod_signed_bundle(
            None, signed_mismatched_root_key_and_signature
        )
    )

    # Generates a prod root metadata with rollback attempt.
    root_rollback = test_bundle.generate_prod_root_metadata()
    root_rollback.common_metadata.version = TEST_ROOT_VERSION - 1
    signed_root_rollback = test_bundle.generate_prod_signed_root_metadata(
        root_rollback
    )
    root_rollback_bundle = test_bundle.generate_prod_signed_bundle(
        None, signed_root_rollback
    )

    # Generates a bundle with a bad target signature.
    bad_targets_siganture = test_bundle.generate_prod_signed_bundle(
        None, prod_signed_root
    )
    # Compromises the signature.
    bad_targets_siganture.targets_metadata['targets'].signatures[0].sig = (
        b'1' * 64
    )

    # Generates a bundle with rollback attempt
    targets_rollback = test_bundle.generate_targets_metadata()
    targets_rollback.common_metadata.version = TEST_TARGETS_VERSION - 1
    targets_rollback_bundle = test_bundle.generate_prod_signed_bundle(
        targets_rollback, prod_signed_root
    )

    # Generate bundles with mismatched hash
    mismatched_hash_targets_bundles = []
    # Generate bundles with mismatched file length
    mismatched_length_targets_bundles = []
    # Generate bundles with missing hash
    missing_hash_targets_bundles = []
    # Generate bundles with personalized out payload
    personalized_out_bundles = []
    # For each of the two files in `TARGET_FILES`, we generate a number of
    # bundles each of which modify the target in the following way
    # respectively:
    # 1. Compromise its sha256 hash value in the targets metadata, so as to
    #    test hash verification logic.
    # 2. Remove the hashes, to trigger verification failure cause by missing
    #    hashes.
    # 3. Compromise the file length in the targets metadata.
    # 4. Remove the payload to emulate being personalized out, so as to test
    #    that it does not cause verification failure.
    for idx, payload_file in enumerate(TARGET_FILES.items()):
        mismatched_hash_targets = test_bundle.generate_targets_metadata()
        mismatched_hash_targets.target_files[idx].hashes[0].hash = b'0' * 32
        mismatched_hash_targets_bundle = (
            test_bundle.generate_prod_signed_bundle(
                mismatched_hash_targets, prod_signed_root
            )
        )
        mismatched_hash_targets_bundles.append(mismatched_hash_targets_bundle)

        mismatched_length_targets = test_bundle.generate_targets_metadata()
        mismatched_length_targets.target_files[idx].length = 1
        mismatched_length_targets_bundle = (
            test_bundle.generate_prod_signed_bundle(
                mismatched_length_targets, prod_signed_root
            )
        )
        mismatched_length_targets_bundles.append(
            mismatched_length_targets_bundle
        )

        missing_hash_targets = test_bundle.generate_targets_metadata()
        missing_hash_targets.target_files[idx].hashes.pop()
        missing_hash_targets_bundle = test_bundle.generate_prod_signed_bundle(
            missing_hash_targets, prod_signed_root
        )
        missing_hash_targets_bundles.append(missing_hash_targets_bundle)

        file_name, _ = payload_file
        personalized_out_bundle = test_bundle.generate_prod_signed_bundle(
            None, prod_signed_root
        )
        personalized_out_bundle.target_payloads.pop(file_name)
        personalized_out_bundles.append(personalized_out_bundle)

    with open(args.output_header, 'w') as header:
        header.write(HEADER)
        header.write(
            proto_array_declaration(dev_signed_bundle, 'kTestDevBundle')
        )
        header.write(
            proto_array_declaration(
                dev_signed_bundle_with_root, 'kTestDevBundleWithRoot'
            )
        )
        header.write(
            proto_array_declaration(
                unsigned_bundle_with_root, 'kTestUnsignedBundleWithRoot'
            )
        )
        header.write(
            proto_array_declaration(
                dev_signed_bundle_with_prod_root, 'kTestDevBundleWithProdRoot'
            )
        )
        header.write(
            proto_array_declaration(manifest_proto, 'kTestBundleManifest')
        )
        header.write(proto_array_declaration(dev_signed_root, 'kDevSignedRoot'))
        header.write(
            proto_array_declaration(prod_signed_bundle, 'kTestProdBundle')
        )
        header.write(
            proto_array_declaration(
                mismatched_root_key_and_signature_bundle,
                'kTestMismatchedRootKeyAndSignature',
            )
        )
        header.write(
            proto_array_declaration(
                signed_bad_prod_signature_bundle, 'kTestBadProdSignature'
            )
        )
        header.write(
            proto_array_declaration(
                bad_targets_siganture, 'kTestBadTargetsSignature'
            )
        )
        header.write(
            proto_array_declaration(
                targets_rollback_bundle, 'kTestTargetsRollback'
            )
        )
        header.write(
            proto_array_declaration(root_rollback_bundle, 'kTestRootRollback')
        )

        for idx, mismatched_hash_bundle in enumerate(
            mismatched_hash_targets_bundles
        ):
            header.write(
                proto_array_declaration(
                    mismatched_hash_bundle,
                    f'kTestBundleMismatchedTargetHashFile{idx}',
                )
            )

        for idx, missing_hash_bundle in enumerate(missing_hash_targets_bundles):
            header.write(
                proto_array_declaration(
                    missing_hash_bundle,
                    f'kTestBundleMissingTargetHashFile{idx}',
                )
            )

        for idx, mismatched_length_bundle in enumerate(
            mismatched_length_targets_bundles
        ):
            header.write(
                proto_array_declaration(
                    mismatched_length_bundle,
                    f'kTestBundleMismatchedTargetLengthFile{idx}',
                )
            )

        for idx, personalized_out_bundle in enumerate(personalized_out_bundles):
            header.write(
                proto_array_declaration(
                    personalized_out_bundle,
                    f'kTestBundlePersonalizedOutFile{idx}',
                )
            )
    subprocess.run(
        [
            'clang-format',
            '-i',
            args.output_header,
        ],
        check=True,
    )
    # TODO(b/237580538): Refactor the code so that each test bundle generation
    # is done in a separate function or script.
    # pylint: enable=too-many-locals
    return 0


if __name__ == "__main__":
    sys.exit(main())
