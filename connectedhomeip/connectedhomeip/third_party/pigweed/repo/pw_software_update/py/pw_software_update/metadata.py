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
"""Facilities for generating TUF target metadata."""

import enum
import hashlib
from typing import Dict, Iterable

from pw_software_update.tuf_pb2 import (
    CommonMetadata,
    Hash,
    HashFunction,
    TargetFile,
    TargetsMetadata,
)

HASH_FACTORIES = {
    HashFunction.SHA256: hashlib.sha256,
}
DEFAULT_HASHES = (HashFunction.SHA256,)
DEFAULT_SPEC_VERSION = "0.0.1"
DEFAULT_METADATA_VERSION = 0


class RoleType(enum.Enum):
    """Set of allowed TUF metadata role types."""

    ROOT = 'root'
    TARGETS = 'targets'


def gen_common_metadata(
    role: RoleType,
    spec_version: str = DEFAULT_SPEC_VERSION,
    version: int = DEFAULT_METADATA_VERSION,
) -> CommonMetadata:
    """Generates CommonMetadata."""
    return CommonMetadata(
        role=role.value, spec_version=spec_version, version=version
    )


def gen_target_file(
    file_name: str, file_contents: bytes, hash_funcs=DEFAULT_HASHES
) -> TargetFile:

    return TargetFile(
        file_name=file_name,
        length=len(file_contents),
        hashes=gen_hashes(file_contents, hash_funcs),
    )


def gen_targets_metadata(
    target_payloads: Dict[str, bytes],
    hash_funcs: Iterable['HashFunction.V'] = DEFAULT_HASHES,
    version: int = DEFAULT_METADATA_VERSION,
) -> TargetsMetadata:
    """Generates TargetsMetadata the given target payloads."""
    target_files = []
    for target_file_name, target_payload in target_payloads.items():
        new_target_file = gen_target_file(
            file_name=target_file_name,
            file_contents=target_payload,
            hash_funcs=hash_funcs,
        )

        target_files.append(new_target_file)

    common_metadata = gen_common_metadata(RoleType.TARGETS, version=version)
    return TargetsMetadata(
        common_metadata=common_metadata, target_files=target_files
    )


def gen_hashes(
    data: bytes, hash_funcs: Iterable['HashFunction.V']
) -> Iterable[Hash]:
    """Computes all the specified hashes over the data."""
    result = []
    for func in hash_funcs:
        if func == HashFunction.UNKNOWN_HASH_FUNCTION:
            raise ValueError(
                'UNKNOWN_HASH_FUNCTION cannot be used to generate hashes.'
            )
        digest = HASH_FACTORIES[func](data).digest()
        result.append(Hash(function=func, hash=digest))

    return result
