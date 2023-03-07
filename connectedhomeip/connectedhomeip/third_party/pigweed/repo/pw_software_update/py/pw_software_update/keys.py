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
"""Facilities for keys generation, importing, signing and verification.

IMPORTANT: THESE FACILITIES ARE FOR LOCAL NON-PRODUCTION USE ONLY!!

These are not suited for production use because:

1. The private keys are not generated without ANY supervision or authorization.
2. The private keys are not stored securely.
3. The underlying crypto library is not audited.
"""

import argparse
import hashlib
from pathlib import Path

from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.primitives.asymmetric.utils import (
    decode_dss_signature,
    encode_dss_signature,
)
from cryptography.hazmat.primitives.serialization import (
    Encoding,
    NoEncryption,
    PrivateFormat,
    PublicFormat,
    load_pem_private_key,
    load_pem_public_key,
)

from pw_software_update.tuf_pb2 import (
    Key,
    KeyMapping,
    KeyScheme,
    KeyType,
    Signature,
)


def parse_args():
    """Parse CLI arguments."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        '-o',
        '--out',
        type=Path,
        required=True,
        help='Output path for the generated key',
    )
    return parser.parse_args()


def gen_ecdsa_keypair(out: Path) -> None:
    """Generates and writes to disk a NIST-P256 EC key pair.

    Args:
      out: The path to write the private key to. The public key is written
        to the same path as the private key using the suffix '.pub'.
    """
    private_key = ec.generate_private_key(ec.SECP256R1())
    public_key = private_key.public_key()
    private_pem = private_key.private_bytes(
        encoding=Encoding.PEM,
        format=PrivateFormat.PKCS8,
        encryption_algorithm=NoEncryption(),
    )
    public_pem = public_key.public_bytes(
        encoding=Encoding.PEM, format=PublicFormat.SubjectPublicKeyInfo
    )

    out.write_bytes(private_pem)
    public_out = out.parent / f'{out.name}.pub'
    public_out.write_bytes(public_pem)


def gen_key_id(key: Key) -> bytes:
    """Computes the key ID of a Key object."""
    sha = hashlib.sha256()
    sha.update(key.key_type.to_bytes(1, 'big'))
    sha.update(key.scheme.to_bytes(1, 'big'))
    sha.update(key.keyval)
    return sha.digest()


def import_ecdsa_public_key(pem: bytes) -> KeyMapping:
    """Imports an EC NIST-P256 public key in pem format."""
    ec_key = load_pem_public_key(pem)

    if not isinstance(ec_key, ec.EllipticCurvePublicKey):
        raise TypeError(
            f'Not an elliptic curve public key type: {type(ec_key)}.'
            'Try generate a key with gen_ecdsa_keypair()?'
        )

    # pylint: disable=no-member
    if not (ec_key.curve.name == 'secp256r1' and ec_key.key_size == 256):
        raise TypeError(
            f'Unsupported curve: {ec_key.curve.name}.'
            'Try generate a key with gen_ecdsa_keypair()?'
        )
    # pylint: enable=no-member

    tuf_key = Key(
        key_type=KeyType.ECDSA_SHA2_NISTP256,
        scheme=KeyScheme.ECDSA_SHA2_NISTP256_SCHEME,
        keyval=ec_key.public_bytes(
            Encoding.X962, PublicFormat.UncompressedPoint
        ),
    )
    return KeyMapping(key_id=gen_key_id(tuf_key), key=tuf_key)


def create_ecdsa_signature(data: bytes, key: bytes) -> Signature:
    """Creates an ECDSA-SHA2-NISTP256 signature."""
    ec_key = load_pem_private_key(key, password=None)
    if not isinstance(ec_key, ec.EllipticCurvePrivateKey):
        raise TypeError(
            f'Not an elliptic curve private key: {type(ec_key)}.'
            'Try generate a key with gen_ecdsa_keypair()?'
        )

    tuf_key = Key(
        key_type=KeyType.ECDSA_SHA2_NISTP256,
        scheme=KeyScheme.ECDSA_SHA2_NISTP256_SCHEME,
        keyval=ec_key.public_key().public_bytes(
            Encoding.X962, PublicFormat.UncompressedPoint
        ),
    )

    der_signature = ec_key.sign(
        data, ec.ECDSA(hashes.SHA256())
    )  # pylint: disable=no-value-for-parameter
    int_r, int_s = decode_dss_signature(der_signature)
    sig_bytes = int_r.to_bytes(32, 'big') + int_s.to_bytes(32, 'big')

    return Signature(key_id=gen_key_id(tuf_key), sig=sig_bytes)


def verify_ecdsa_signature(sig: bytes, data: bytes, key: Key) -> bool:
    """Verifies an ECDSA-SHA2-NISTP256 signature with a given public key.

    Args:
      sig: the ECDSA signature as raw bytes (r||s).
      data: the message as plain text.
      key: the ECDSA-NISTP256 public key.

    Returns:
      True if the signature is verified. False otherwise.
    """
    ec_key = ec.EllipticCurvePublicKey.from_encoded_point(
        ec.SECP256R1(), key.keyval
    )
    try:
        dss_sig = encode_dss_signature(
            int.from_bytes(sig[:32], 'big'), int.from_bytes(sig[-32:], 'big')
        )
        ec_key.verify(dss_sig, data, ec.ECDSA(hashes.SHA256()))
    except:  # pylint: disable=bare-except
        return False

    return True


def main(out: Path) -> None:
    """Generates and writes to disk key pairs for development use."""

    # Currently only supports the "ecdsa-sha2-nistp256" key scheme.
    #
    # TODO(alizhang): Add support for "rsassa-pss-sha256" and "ed25519" key
    # schemes.
    gen_ecdsa_keypair(out)


if __name__ == '__main__':
    main(**vars(parse_args()))
