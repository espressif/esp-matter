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
"""Facilities for generating the 'root' metadata."""

import argparse
from pathlib import Path
from typing import Iterable, List, NewType

from pw_software_update import keys, metadata
from pw_software_update.tuf_pb2 import (
    RootMetadata,
    SignedRootMetadata,
    SignatureRequirement,
)

RootKeys = NewType('RootKeys', List[bytes])
TargetsKeys = NewType('TargetsKeys', List[bytes])


def gen_root_metadata(
    root_key_pems: RootKeys, targets_key_pems: TargetsKeys, version: int = 1
) -> RootMetadata:
    """Generates a RootMetadata.

    Args:
      root_key_pems: list of root public keys in PEM format.
      targets_key_pems: list of targets keys in PEM format.
      version: Version number for rollback checks.
    """
    common = metadata.gen_common_metadata(
        metadata.RoleType.ROOT, version=version
    )

    root_keys = [keys.import_ecdsa_public_key(pem) for pem in root_key_pems]
    targets_keys = [
        keys.import_ecdsa_public_key(pem) for pem in targets_key_pems
    ]

    return RootMetadata(
        common_metadata=common,
        consistent_snapshot=False,
        keys=root_keys + targets_keys,
        root_signature_requirement=SignatureRequirement(
            key_ids=[k.key_id for k in root_keys], threshold=1
        ),
        targets_signature_requirement=SignatureRequirement(
            key_ids=[k.key_id for k in targets_keys], threshold=1
        ),
    )


def parse_args():
    """Parse CLI arguments."""
    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument(
        '-o',
        '--out',
        type=Path,
        required=True,
        help='Output path for the generated root metadata',
    )

    parser.add_argument(
        '--version',
        type=int,
        default=1,
        help='Canonical version number for rollback checks',
    )

    parser.add_argument(
        '--root-key',
        type=Path,
        required=True,
        nargs='+',
        help='Public key filename for the "Root" role',
    )

    parser.add_argument(
        '--targets-key',
        type=Path,
        required=True,
        nargs='+',
        help='Public key filename for the "Targets" role',
    )
    return parser.parse_args()


def main(
    out: Path,
    root_key: Iterable[Path],
    targets_key: Iterable[Path],
    version: int,
) -> None:
    """Generates and writes to disk an unsigned SignedRootMetadata."""

    root_metadata = gen_root_metadata(
        RootKeys([k.read_bytes() for k in root_key]),
        TargetsKeys([k.read_bytes() for k in targets_key]),
        version,
    )
    signed = SignedRootMetadata(
        serialized_root_metadata=root_metadata.SerializeToString()
    )

    out.write_bytes(signed.SerializeToString())


if __name__ == '__main__':
    main(**vars(parse_args()))
