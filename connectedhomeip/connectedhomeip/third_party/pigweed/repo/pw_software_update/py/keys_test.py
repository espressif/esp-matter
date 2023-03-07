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
"""Unit tests for pw_software_update/keys.py."""

from pathlib import Path
import tempfile
import unittest

from pw_software_update import keys
from pw_software_update.tuf_pb2 import Key, KeyType, KeyScheme


class KeyGenTest(unittest.TestCase):
    """Test the generation of keys."""

    def test_ecdsa_keygen(self):
        """Test ECDSA key generation."""
        with tempfile.TemporaryDirectory() as tempdir_name:
            temp_root = Path(tempdir_name)
            private_key_filename = temp_root / 'test_key'
            public_key_filename = temp_root / 'test_key.pub'

            keys.gen_ecdsa_keypair(private_key_filename)

            self.assertTrue(private_key_filename.exists())
            self.assertTrue(public_key_filename.exists())
            public_key = keys.import_ecdsa_public_key(
                public_key_filename.read_bytes()
            )
            self.assertEqual(
                public_key.key.key_type, KeyType.ECDSA_SHA2_NISTP256
            )
            self.assertEqual(
                public_key.key.scheme, KeyScheme.ECDSA_SHA2_NISTP256_SCHEME
            )


class KeyIdTest(unittest.TestCase):
    """Test Key ID generations"""

    def test_256bit_length(self):
        key_id = keys.gen_key_id(
            Key(
                key_type=KeyType.ECDSA_SHA2_NISTP256,
                scheme=KeyScheme.ECDSA_SHA2_NISTP256_SCHEME,
                keyval=b'public_key bytes',
            )
        )
        self.assertEqual(len(key_id), 32)

    def test_different_keyval(self):
        key1 = Key(
            key_type=KeyType.ECDSA_SHA2_NISTP256,
            scheme=KeyScheme.ECDSA_SHA2_NISTP256_SCHEME,
            keyval=b'key 1 bytes',
        )
        key2 = Key(
            key_type=KeyType.ECDSA_SHA2_NISTP256,
            scheme=KeyScheme.ECDSA_SHA2_NISTP256_SCHEME,
            keyval=b'key 2 bytes',
        )

        key1_id, key2_id = keys.gen_key_id(key1), keys.gen_key_id(key2)
        self.assertNotEqual(key1_id, key2_id)

    def test_different_key_type(self):
        key1 = Key(
            key_type=KeyType.RSA,
            scheme=KeyScheme.ECDSA_SHA2_NISTP256_SCHEME,
            keyval=b'key bytes',
        )
        key2 = Key(
            key_type=KeyType.ECDSA_SHA2_NISTP256,
            scheme=KeyScheme.ECDSA_SHA2_NISTP256_SCHEME,
            keyval=b'key bytes',
        )

        key1_id, key2_id = keys.gen_key_id(key1), keys.gen_key_id(key2)
        self.assertNotEqual(key1_id, key2_id)

    def test_different_scheme(self):
        key1 = Key(
            key_type=KeyType.ECDSA_SHA2_NISTP256,
            scheme=KeyScheme.ECDSA_SHA2_NISTP256_SCHEME,
            keyval=b'key bytes',
        )
        key2 = Key(
            key_type=KeyType.ECDSA_SHA2_NISTP256,
            scheme=KeyScheme.ED25519_SCHEME,
            keyval=b'key bytes',
        )

        key1_id, key2_id = keys.gen_key_id(key1), keys.gen_key_id(key2)
        self.assertNotEqual(key1_id, key2_id)


class KeyImportTest(unittest.TestCase):
    """Test key importing"""

    def setUp(self):
        # Generated with:
        # $> openssl ecparam -name prime256v1 -genkey -noout -out priv.pem
        # $> openssl ec -in priv.pem -pubout -out pub.pem
        # $> cat pub.pem
        self.valid_nistp256_pem_bytes = (
            b'-----BEGIN PUBLIC KEY-----\n'
            b'MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEKmK5mJwMV7eimA6MfFQL2q6KbZDr'
            b'SnWwoeHvXB/aZBnwF422OLifuOuMjEUEHrNMmoekcua+ulHW41X3AgbvIw==\n'
            b'-----END PUBLIC KEY-----\n'
        )

        # Generated with:
        # $> openssl ecparam -name secp384r1 -genkey -noout -out priv.pem
        # $> openssl ec -in priv.pem -pubout -out pub.pem
        # $> cat pub.pem
        self.valid_secp384r1_pem_bytes = (
            b'-----BEGIN PUBLIC KEY-----\n'
            b'MHYwEAYHKoZIzj0CAQYFK4EEACIDYgAE6xs+TEjb2/vIzs4AzSm2CSUWpJMCPAts'
            b'e+gwvGwFrr2bXKHVLNCxr5/Va6rD0nDmB2NOiJwAXX1Z8CB5wqLLB31emCBFRb5i'
            b'1LjZu8Bp3hrWOL7uvXer8uExnSfTKAoT\n'
            b'-----END PUBLIC KEY-----\n'
        )

        # Replaces "MF" with "MM"
        self.tampered_nistp256_pem_bytes = (
            b'-----BEGIN PUBLIC KEY-----\n'
            b'MMkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEKmK5mJwMV7eimA6MfFQL2q6KbZDr'
            b'SnWwoeHvXB/aZBnwF422OLifuOuMjEUEHrNMmoekcua+ulHW41X3AgbvIw==\n'
            b'-----END PUBLIC KEY-----\n'
        )

        self.rsa_2048_pem_bytes = (
            b'-----BEGIN PUBLIC KEY-----\n'
            b'MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAsu0+ol90Ri2BQ5TE9ife'
            b'6aAmAUMzvAD2b3cnWaTBGXKpi7O9PKnfKbMVf/nJcWsyw2Bj8uStx3oV98U6owLO'
            b'vsQwyFKVgLZdrXo2qv0L6ljBfCLJxnDhjesEV/oG04dwdN7qyPwAZtpVBCrC7Qi8'
            b'2rkTnzTQi/1slUxRjliDDhgEdqP7dHbCr7QXNIAA0HFRiOqYmHGD7HNKl67iYmAX'
            b'd/Jv8GfZL/ykZstP6Ow1/ByP1ZKvrZvg2iXjC686hZXiMJLqmp0sIqLire82oW+8'
            b'XFc1uyr1j20m+NI5Siy0G3RbfPXrVKyXIgAYPW12+a/BXR9SrqYJYcWwuOGbHZCM'
            b'pwIDAQAB\n'
            b'-----END PUBLIC KEY-----\n'
        )

    def test_valid_nistp256_key(self):
        keys.import_ecdsa_public_key(self.valid_nistp256_pem_bytes)

    def test_tampered_nistp256_key(self):
        with self.assertRaises(ValueError):
            keys.import_ecdsa_public_key(self.tampered_nistp256_pem_bytes)

    def test_non_ec_key(self):
        with self.assertRaises(TypeError):
            keys.import_ecdsa_public_key(self.rsa_2048_pem_bytes)

    def test_wrong_curve(self):
        with self.assertRaises(TypeError):
            keys.import_ecdsa_public_key(self.valid_secp384r1_pem_bytes)


class SignatureVerificationTest(unittest.TestCase):
    """ECDSA signing and verification test."""

    def setUp(self):
        # Generated with:
        # $> openssl ecparam -name prime256v1 -genkey -noout -out priv.pem
        # $> openssl ec -in priv.pem -pubout -out pub.pem
        # $> cat priv.pem pub.pem
        self.private_key_pem = (
            b'-----BEGIN EC PRIVATE KEY-----\n'
            b'MHcCAQEEIH9u1n4qAT59f7KRRl/ZB0Y/BUfS4blba+LONlF4s3ltoAoGCCqGSM49'
            b'AwEHoUQDQgAEgKf3kY9Hi3hxIyqm2EkfqQvJkCijjlJSmEAJ1oAp0Godi5x2af+m'
            b'cSNuBjpRcC8iW8x1/gizqyWlfAVrZV0XdA==\n'
            b'-----END EC PRIVATE KEY-----\n'
        )
        self.public_key_pem = (
            b'-----BEGIN PUBLIC KEY-----\n'
            b'MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEgKf3kY9Hi3hxIyqm2EkfqQvJkCij'
            b'jlJSmEAJ1oAp0Godi5x2af+mcSNuBjpRcC8iW8x1/gizqyWlfAVrZV0XdA==\n'
            b'-----END PUBLIC KEY-----\n'
        )

        self.message = b'Hello Pigweed!'
        self.tampered_message = b'Hell0 Pigweed!'

    def test_good_signature(self):
        sig = keys.create_ecdsa_signature(self.message, self.private_key_pem)
        self.assertTrue(
            keys.verify_ecdsa_signature(
                sig.sig,
                self.message,
                keys.import_ecdsa_public_key(self.public_key_pem).key,
            )
        )

    def test_tampered_message(self):
        sig = keys.create_ecdsa_signature(self.message, self.private_key_pem)
        self.assertFalse(
            keys.verify_ecdsa_signature(
                sig.sig,
                self.tampered_message,
                keys.import_ecdsa_public_key(self.public_key_pem).key,
            )
        )

    def test_tampered_signature(self):
        sig = keys.create_ecdsa_signature(self.message, self.private_key_pem)
        tampered_sig = bytearray(sig.sig)
        tampered_sig[0] ^= 1
        self.assertFalse(
            keys.verify_ecdsa_signature(
                tampered_sig,
                self.message,
                keys.import_ecdsa_public_key(self.public_key_pem).key,
            )
        )


if __name__ == '__main__':
    unittest.main()
