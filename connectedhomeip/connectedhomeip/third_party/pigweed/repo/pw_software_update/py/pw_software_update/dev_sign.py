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
"""Metadata signing facilities."""

import argparse
from pathlib import Path

from pw_software_update import keys
from pw_software_update.tuf_pb2 import SignedRootMetadata
from pw_software_update.update_bundle_pb2 import UpdateBundle


def sign_root_metadata(
    root_metadata: SignedRootMetadata, root_key_pem: bytes
) -> SignedRootMetadata:
    """Signs or re-signs a Root Metadata.

    Args:
      root_metadata: A SignedRootMetadata to be signed/re-signed.
      root_key_pem: The Root signing key in PEM.
    """

    signature = keys.create_ecdsa_signature(
        root_metadata.serialized_root_metadata, root_key_pem
    )
    root_metadata.signatures.append(signature)

    return root_metadata


def sign_update_bundle(
    bundle: UpdateBundle, targets_key_pem: bytes
) -> UpdateBundle:
    """Signs or re-signs an update bundle.

    Args:
      bundle: An UpdateBundle to be signed/re-signed.
      targets_key_pem: The targets signing key in PEM.
    """
    bundle.targets_metadata['targets'].signatures.append(
        keys.create_ecdsa_signature(
            bundle.targets_metadata['targets'].serialized_targets_metadata,
            targets_key_pem,
        )
    )
    return bundle


def parse_args():
    """Parse CLI arguments."""
    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument(
        '--root-metadata',
        type=Path,
        required=False,
        help='Path to the root metadata to be signed',
    )

    parser.add_argument(
        '--bundle',
        type=Path,
        required=False,
        help='Path to the bundle to be signed',
    )

    parser.add_argument(
        '--output',
        type=Path,
        required=False,
        help=(
            'Path to save the signed root metadata or bundle '
            'to; Defaults to the input path if unspecified'
        ),
    )

    parser.add_argument(
        '--key', type=Path, required=True, help='Path to the signing key'
    )

    args = parser.parse_args()

    if not (args.root_metadata or args.bundle):
        parser.error('either "--root-metadata" or "--bundle" must be specified')
    if args.root_metadata and args.bundle:
        parser.error('"--root-metadata" and "--bundle" are mutually exclusive')

    return args


def main(root_metadata: Path, bundle: Path, key: Path, output: Path) -> None:
    """Signs or re-signs a root metadata or an update bundle."""
    if root_metadata:
        signed_root_metadata = sign_root_metadata(
            SignedRootMetadata.FromString(root_metadata.read_bytes()),
            key.read_bytes(),
        )

        if not output:
            output = root_metadata
        output.write_bytes(signed_root_metadata.SerializeToString())
    else:
        signed_bundle = sign_update_bundle(
            UpdateBundle.FromString(bundle.read_bytes()), key.read_bytes()
        )

        if not output:
            output = bundle
        output.write_bytes(signed_bundle.SerializeToString())


if __name__ == '__main__':
    main(**vars(parse_args()))
