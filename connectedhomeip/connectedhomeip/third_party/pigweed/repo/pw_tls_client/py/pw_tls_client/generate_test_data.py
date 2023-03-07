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
"""Generate test data

Generate data needed for unit tests, i.e. certificates, keys, and CRLSet.
"""

import argparse
import subprocess
import sys
from datetime import datetime, timedelta
from typing import List, Tuple

from cryptography import x509
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.x509.oid import NameOID

CERTS_AND_KEYS_HEADER = """// Copyright 2021 The Pigweed Authors
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


class Subject:
    """A subject wraps a name, private key and extensions for issuers
    to issue its certificate"""

    def __init__(
        self, name: str, extensions: List[Tuple[x509.ExtensionType, bool]]
    ):
        self._subject_name = x509.Name(
            [
                x509.NameAttribute(NameOID.COUNTRY_NAME, u"US"),
                x509.NameAttribute(
                    NameOID.STATE_OR_PROVINCE_NAME, u"California"
                ),
                x509.NameAttribute(NameOID.LOCALITY_NAME, u"Mountain View"),
                x509.NameAttribute(NameOID.ORGANIZATION_NAME, name),
                x509.NameAttribute(NameOID.COMMON_NAME, u"Google-Pigweed"),
            ]
        )
        self._private_key = rsa.generate_private_key(
            public_exponent=65537, key_size=2048
        )
        self._extensions = extensions

    def subject_name(self) -> x509.Name:
        """Returns the subject name"""
        return self._subject_name

    def public_key(self) -> rsa.RSAPublicKey:
        """Returns the public key of this subject"""
        return self._private_key.public_key()

    def private_key(self) -> rsa.RSAPrivateKey:
        """Returns the private key of this subject"""
        return self._private_key

    def extensions(self) -> List[Tuple[x509.ExtensionType, bool]]:
        """Returns the requested extensions for issuer"""
        return self._extensions


class CA(Subject):
    """A CA/Sub-ca that issues certificates"""

    def __init__(self, *args, **kwargs):
        ext = [
            (x509.BasicConstraints(True, None), True),
            (
                x509.KeyUsage(
                    digital_signature=False,
                    content_commitment=False,
                    key_encipherment=False,
                    data_encipherment=False,
                    key_agreement=False,
                    crl_sign=False,
                    encipher_only=False,
                    decipher_only=False,
                    key_cert_sign=True,
                ),
                True,
            ),
        ]
        super().__init__(*args, extensions=ext, **kwargs)

    def sign(
        self, subject: Subject, not_before: datetime, not_after: datetime
    ) -> x509.Certificate:
        """Issues a certificate for another CA/Sub-ca/Server"""
        builder = x509.CertificateBuilder()

        # Subject name is the target's subject name
        builder = builder.subject_name(subject.subject_name())

        # Issuer name is this CA/sub-ca's subject name
        builder = builder.issuer_name(self._subject_name)

        # Public key is the target's public key.
        builder = builder.public_key(subject.public_key())

        # Validity period.
        builder = builder.not_valid_before(not_before).not_valid_after(
            not_after
        )

        # Uses a random serial number
        builder = builder.serial_number(x509.random_serial_number())

        # Add extensions
        for extension, critical in subject.extensions():
            builder = builder.add_extension(extension, critical)

        # Sign and returns the certificate.
        return builder.sign(self._private_key, hashes.SHA256())

    def self_sign(
        self, not_before: datetime, not_after: datetime
    ) -> x509.Certificate:
        """Issues a self sign certificate"""
        return self.sign(self, not_before, not_after)


class Server(Subject):
    """The end-entity server"""

    def __init__(self, *args, **kwargs):
        ext = [
            (x509.BasicConstraints(False, None), True),
            (
                x509.KeyUsage(
                    digital_signature=True,
                    content_commitment=False,
                    key_encipherment=False,
                    data_encipherment=False,
                    key_agreement=False,
                    crl_sign=False,
                    encipher_only=False,
                    decipher_only=False,
                    key_cert_sign=False,
                ),
                True,
            ),
            (
                x509.ExtendedKeyUsage([x509.ExtendedKeyUsageOID.SERVER_AUTH]),
                True,
            ),
        ]
        super().__init__(*args, extensions=ext, **kwargs)


def c_escaped_string(data: bytes):
    """Generates a C byte string representation for a byte array

    For example, given a byte sequence of [0x12, 0x34, 0x56]. The function
    generates the following byte string code:

            {"\x12\x34\x56", 3}
    """
    body = ''.join([f'\\x{b:02x}' for b in data])
    return f'{{\"{body}\", {len(data)}}}'


def byte_array_declaration(data: bytes, name: str) -> str:
    """Generates a ConstByteSpan declaration for a byte array"""
    type_name = '[[maybe_unused]] const pw::ConstByteSpan'
    array_body = f'pw::as_bytes(pw::span{c_escaped_string(data)})'
    return f'{type_name} {name} = {array_body};'


class Codegen:
    """Base helper class for code generation"""

    def generate_code(self) -> str:
        """Generates C++ code for this object"""


class PrivateKeyGen(Codegen):
    """Codegen class for a private key"""

    def __init__(self, key: rsa.RSAPrivateKey, name: str):
        self._key = key
        self._name = name

    def generate_code(self) -> str:
        """Code generation"""
        return byte_array_declaration(
            self._key.private_bytes(
                serialization.Encoding.DER,
                serialization.PrivateFormat.TraditionalOpenSSL,
                serialization.NoEncryption(),
            ),
            self._name,
        )


class CertificateGen(Codegen):
    """Codegen class for a single certificate"""

    def __init__(self, cert: x509.Certificate, name: str):
        self._cert = cert
        self._name = name

    def generate_code(self) -> str:
        """Code generation"""
        return byte_array_declaration(
            self._cert.public_bytes(serialization.Encoding.DER), self._name
        )


def generate_test_data() -> str:
    """Generates test data"""
    subjects: List[Codegen] = []

    # Working valid period.
    # Start from yesterday, to make sure we are in the valid period.
    not_before = datetime.utcnow() - timedelta(days=1)
    # Valid for 1 year.
    not_after = not_before + timedelta(days=365)

    # Generate a root-A CA certificates
    root_a = CA("root-A")
    subjects.append(
        CertificateGen(root_a.self_sign(not_before, not_after), "kRootACert")
    )

    # Generate a sub CA certificate signed by root-A.
    sub = CA("sub")
    subjects.append(
        CertificateGen(root_a.sign(sub, not_before, not_after), "kSubCACert")
    )

    # Generate a valid server certificate signed by sub
    server = Server("server")
    subjects.append(
        CertificateGen(sub.sign(server, not_before, not_after), "kServerCert")
    )
    subjects.append(PrivateKeyGen(server.private_key(), "kServerKey"))

    root_b = CA("root-B")
    subjects.append(
        CertificateGen(root_b.self_sign(not_before, not_after), "kRootBCert")
    )

    code = 'namespace {\n\n'
    for subject in subjects:
        code += subject.generate_code() + '\n\n'
    code += '}\n'

    return code


def clang_format(file):
    subprocess.run(
        [
            "clang-format",
            "-i",
            file,
        ],
        check=True,
    )


def parse_args():
    """Setup argparse."""
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "certs_and_keys_header",
        help="output header file for test certificates and keys",
    )
    return parser.parse_args()


def main() -> int:
    """Main"""
    args = parse_args()

    certs_and_keys = generate_test_data()

    with open(args.certs_and_keys_header, 'w') as header:
        header.write(CERTS_AND_KEYS_HEADER)
        header.write(certs_and_keys)

    clang_format(args.certs_and_keys_header)
    return 0


if __name__ == "__main__":
    sys.exit(main())
