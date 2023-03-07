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
"""Facilities to verify an update bundle."""

import argparse
import inspect
import logging
from pathlib import Path
import sys
from typing import Iterable

from pw_software_update import keys, metadata
from pw_software_update.tuf_pb2 import (
    RootMetadata,
    SignedRootMetadata,
    SignedTargetsMetadata,
    TargetsMetadata,
)
from pw_software_update.update_bundle_pb2 import UpdateBundle

_LOG = logging.getLogger(__package__)


def log_progress(message: str, indent_offset: int = -5, indent_str: str = '  '):
    """Logs verification progress.

    The default indent offset is chosen per actual output of 'python -m verify'.
    """
    indentation = 2 * (len(inspect.stack(0)) + indent_offset)
    _LOG.info('%s%s', indent_str * indentation, message)


class VerificationError(Exception):
    """Raised upon any verification error."""


def lint_root_metadata(root: RootMetadata) -> Iterable[str]:
    """Checks a RootMetadata for content or format errors.

    Returns:
      A list of all errors found.
    """
    errors = []

    # Check role type first-thing to deter chosen-ciphertext attacks.
    log_progress('Checking role type')
    if root.common_metadata.role != metadata.RoleType.ROOT.value:
        errors.append('Role type is not "root"')

    # Check keys database.
    log_progress('Checking keys database')
    for entry in root.keys:
        if not entry.key_id:
            errors.append('Missing key_id in keys list')
        elif not entry.key.keyval:
            errors.append(f'Key {entry.key_id.hex()} does not have a value')
        elif not entry.key_id == keys.gen_key_id(entry.key):
            errors.append(
                f'Key id "{entry.key_id.hex()}" cannot be derived'
                f'from key content'
            )

    # Check root signature requirement.
    log_progress('Checking root signature requirement')
    root_sig_req = root.root_signature_requirement
    if not root_sig_req.threshold:
        errors.append('Root signature threshold not set')

    if len(root_sig_req.key_ids) < root_sig_req.threshold:
        errors.append(
            f'Insufficient root keys: '
            f'{len(root_sig_req.key_ids)} < {root_sig_req.threshold}'
        )

    for key_id in root_sig_req.key_ids:
        if key_id not in [km.key_id for km in root.keys]:
            errors.append(f'Unregistered root key: {key_id.hex()}')

    # Check targets signature requirement.
    log_progress('Checking targets signature requirement')
    targets_sig_req = root.targets_signature_requirement
    if not targets_sig_req.threshold:
        errors.append('Targets signature threshold not set')

    if len(targets_sig_req.key_ids) < targets_sig_req.threshold:
        errors.append(
            f'Insufficient Targets keys: '
            f'{len(targets_sig_req.key_ids)} < {targets_sig_req.threshold}'
        )

    for key_id in targets_sig_req.key_ids:
        if key_id not in [km.key_id for km in root.keys]:
            errors.append(f'Unregistered targets key: {key_id.hex()}')

    # Make sure no two roles share the same key.
    log_progress('Checking for key sharing')
    for key_id in targets_sig_req.key_ids:
        if key_id in root_sig_req.key_ids:
            errors.append(f'Targets shares the same key: "{key_id.hex()}"')

    return errors


def verify_root_metadata_signatures(
    incoming: SignedRootMetadata, trusted: RootMetadata
) -> None:
    """Verifies the signatures of an incoming root metadata.

    Verifies the signatures of an incoming root metadata against signature
    requirements from the trusted root metadata.

    Raises:
      VerificationError if `incoming` is incorrectly or insufficiently signed.
    """
    sig_requirement = trusted.root_signature_requirement

    log_progress(
        f'Total={len(incoming.signatures)}, '
        f'threshold={sig_requirement.threshold}'
    )
    good_signature_count = 0
    for sig in incoming.signatures:
        if sig.key_id not in sig_requirement.key_ids:
            continue

        key = None
        for key_mapping in trusted.keys:
            if key_mapping.key_id == sig.key_id:
                key = key_mapping.key
                break
        if not key:
            raise VerificationError(f'Invalid key_id: {sig.key_id.hex()}.')

        if not keys.verify_ecdsa_signature(
            sig.sig, incoming.serialized_root_metadata, key
        ):
            raise VerificationError('Invalid signature, key_id={sig.key_id}.')

        good_signature_count += 1

    log_progress(f'Verified: {good_signature_count}')
    if good_signature_count < sig_requirement.threshold:
        raise VerificationError('Not enough good signatures.')


def verify_root_metadata(
    incoming: SignedRootMetadata, trusted: RootMetadata
) -> bool:
    """Verifies an incoming root metadata against a trusted root metadata.

    Returns:
      A boolean flag indicating if the targets metadata has been rotated.

    Raises:
      VerificationError if the incoming root is incorrectly formatted,
      insufficiently signed, or rolling back to an older version.
    """
    # Verify the incoming is signed with a threshold of keys specified in the
    # trusted root metadata.
    log_progress('Checking signatures against current root')
    verify_root_metadata_signatures(incoming, trusted)

    # Now that we've verified the signer of the incoming root, check its content
    # before parsing it to guard against chosen-ciphertext attacks.
    log_progress('Checking content')
    lint_errors = lint_root_metadata(
        RootMetadata.FromString(incoming.serialized_root_metadata)
    )
    if lint_errors:
        log_progress(f'Lint errors: {lint_errors}')
        raise VerificationError('Malformed root metadata.')

    # Verify the target is signed with a threshold of keys specified in the
    # target root metadata.
    log_progress('Checking signatures against current root')
    verify_root_metadata_signatures(
        incoming, RootMetadata.FromString(incoming.serialized_root_metadata)
    )

    # Check rollback attack.
    log_progress('Checking for version rollback')
    incoming_meta = RootMetadata.FromString(incoming.serialized_root_metadata)
    new_ver = incoming_meta.common_metadata.version
    cur_ver = trusted.common_metadata.version
    if new_ver < cur_ver:
        raise VerificationError(
            f'Root metadata version rollback ({cur_ver}->{new_ver}) detected!'
        )

    # Any signature requirement change indicates a targets key rotation.
    new_sig_req = incoming_meta.targets_signature_requirement
    cur_sig_req = trusted.targets_signature_requirement
    targets_key_rotated = not (
        set(new_sig_req.key_ids) == set(cur_sig_req.key_ids)
        and new_sig_req.threshold == cur_sig_req.threshold
    )
    log_progress(f'Targets key rotation: {targets_key_rotated}')
    return targets_key_rotated


def lint_targets_metadata(meta: TargetsMetadata) -> Iterable[str]:
    """Checks a targets metadata for format errors.

    Returns:
      A list of all errors found.
    """
    errors = []

    # Always check the role type first to guard against chosen-ciphertext
    # attacks.
    log_progress("Checking role type")
    if meta.common_metadata.role != metadata.RoleType.TARGETS.value:
        errors.append(
            f'Role type is not "targets" but "{meta.common_metadata.role}"'
        )

    for file in meta.target_files:
        if not file.file_name:
            errors.append('Target file missing a name')
        if not file.hashes:
            errors.append('Target file missing hashes')

    return errors


def verify_targets_metadata(
    signed: SignedTargetsMetadata, root: RootMetadata
) -> None:
    """Verifies a targets metadata is sufficiently signed and well-formed.

    Raises:
      VerificationError if the targets metadata is insufficiently signed or
        malformed.
    """
    sig_requirement = root.targets_signature_requirement
    log_progress(
        f'Checking signatures: total={len(signed.signatures)}, '
        f'threshold={sig_requirement.threshold}'
    )
    good_signatures_count = 0
    for sig in signed.signatures:
        # Ignore extraneous signatures.
        if sig.key_id not in sig_requirement.key_ids:
            continue

        # Extract the public key associated with sig.key_id. There is one and
        # only one way to derive a key_id from a key object, which has been
        # previously verified as part of root metadata verification.
        key = None
        for key_mapping in root.keys:
            if key_mapping.key_id == sig.key_id:
                key = key_mapping.key
                break
        if not key:
            raise VerificationError(
                f'No such key_id in root: {sig.key_id.hex()}.'
            )

        if not keys.verify_ecdsa_signature(
            sig=sig.sig, data=signed.serialized_targets_metadata, key=key
        ):
            raise VerificationError(
                f'Invalid signature, key_id={sig.key_id.hex()}.'
            )

        good_signatures_count += 1

    log_progress(f'Verified signatures: {good_signatures_count}')
    if good_signatures_count < sig_requirement.threshold:
        raise VerificationError(
            f'Not enough good signatures: {good_signatures_count} < '
            f'{sig_requirement.threshold}.'
        )

    log_progress('Checking content')
    lint_errors = lint_targets_metadata(
        TargetsMetadata.FromString(signed.serialized_targets_metadata)
    )
    if lint_errors:
        log_progress(f'Lint errors: {lint_errors}')
        raise VerificationError('Malformed targets metadata.')


def verify_bundle(incoming: UpdateBundle, trusted: UpdateBundle) -> None:
    """Verifies an incoming TUF bundle against metadata in `trusted`.

    Raises VerificationError upon the first verification failure.
    """

    # Root metadata in `trusted` is our trust anchor.
    if not trusted.HasField('root_metadata'):
        raise VerificationError('Trusted bundle missing root metadata')
    trusted_root = RootMetadata.FromString(
        trusted.root_metadata.serialized_root_metadata
    )

    # Check the contents of the trusted root metadata. This is optional
    # in practice as we generally trust what is provisioned in the factory.
    log_progress('Checking content of the trusted root metadata')
    lint_errors = lint_root_metadata(trusted_root)
    if lint_errors:
        log_progress(f'Lint errors: {lint_errors}')
        raise VerificationError('Malformed root metadata.')

    # If the incoming bundle includes a root metadata, verify it using the
    # current trusted root metadata and set the current trusted root to the
    # new root upon successful verification.

    # Record whether the new root metadata rotates the targets key. This
    # information is used later to perform or skip targets metadata version
    # rollback check.
    targets_key_rotated = False
    incoming_root = incoming.root_metadata
    if incoming_root:
        log_progress('Verifying incoming root metadata')
        targets_key_rotated = verify_root_metadata(
            incoming=incoming_root, trusted=trusted_root
        )
        log_progress('Upgrading trust to the incoming root metadata')
        trusted_root = RootMetadata.FromString(
            incoming_root.serialized_root_metadata
        )

    log_progress('Verifying targets metadata')
    signed_targets_metadata = incoming.targets_metadata['targets']
    verify_targets_metadata(signed_targets_metadata, trusted_root)

    # Unless the targets signing key has been rotated, check for version
    # rollback attack.
    targets_metadata = TargetsMetadata.FromString(
        signed_targets_metadata.serialized_targets_metadata
    )
    if not targets_key_rotated:
        log_progress('Checking targets metadata for version rollback')
        new_ver = targets_metadata.common_metadata.version
        cur_ver = TargetsMetadata.FromString(
            trusted.targets_metadata['targets'].serialized_targets_metadata
        ).common_metadata.version
        if new_ver < cur_ver:
            raise VerificationError(
                f'Targets metadata rolling back: {cur_ver} ' f'-> {new_ver}.'
            )

    # Verify all files listed in the targets metadata exist along with the
    # correct sizes and hashes.
    for file in targets_metadata.target_files:
        log_progress(f'Verifying target file: "{file.file_name}"')

        payload = incoming.target_payloads[file.file_name]
        if file.length != len(payload):
            raise VerificationError(
                f'Wrong file size for {file.file_name}: '
                f'expected: {file.length}, '
                f'got: {len(payload)}.'
            )

        if not file.hashes:
            raise VerificationError(f'Missing hashes for: {file.file_name}.')
        calculated_hashes = metadata.gen_hashes(
            payload, [h.function for h in file.hashes]
        )
        if list(calculated_hashes) != list(file.hashes):
            raise VerificationError(f'Mismatched hashes for: {file.file_name}.')


def parse_args():
    """Parse CLI arguments."""
    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument(
        '--incoming',
        type=Path,
        required=True,
        help='Path to the TUF bundle to be verified',
    )

    parser.add_argument(
        '--trusted',
        type=Path,
        help=(
            'Path to the TUF bundle to be trusted; '
            'defaults to the value of `--incoming` '
            'if unspecified.'
        ),
    )

    return parser.parse_args()


def main(incoming: Path, trusted: Path) -> int:
    """Verifies an incoming TUF bundle against metadata in `trusted`.

    Verifies an incoming TUF bundle against metadata from a given trusted
    bundle. If `trusted` is not specified, the target bundle itself will
    be used as the trusted bundle.

    Returns:
      0 on success, non-zero otherwise.
    """
    log_progress(f'Verifying: {incoming}')
    incoming_bundle = UpdateBundle.FromString(incoming.read_bytes())

    is_self_verification = not trusted
    if is_self_verification:
        trusted_bundle = incoming_bundle
        log_progress('(self-verification)')
    else:
        trusted_bundle = UpdateBundle.FromString(trusted.read_bytes())

    try:
        verify_bundle(incoming_bundle, trusted_bundle)
    except VerificationError as error:
        log_progress(f'Verification failed: {error}')
        return 1

    log_progress('Verification passed.')
    return 0


if __name__ == '__main__':
    logging.basicConfig(format='%(message)s', level=logging.INFO)
    exit_code = main(**vars(parse_args()))
    sys.exit(exit_code)
