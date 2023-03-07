#!/usr/bin/env python3
# Copyright 2020 The Pigweed Authors
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
"""Tests the tokenized string encoder module."""

import unittest

import varint_test_data

from pw_tokenizer.encode import encode_token_and_args


class TestEncodeTokenized(unittest.TestCase):
    """Tests encoding tokenized strings with various arguments."""

    def test_no_args(self):
        self.assertEqual(b'\xab\xcd\x12\x34', encode_token_and_args(0x3412CDAB))
        self.assertEqual(b'\x00\x00\x00\x00', encode_token_and_args(0))

    def test_int(self):
        self.assertEqual(
            b'\xff\xff\xff\xff\0', encode_token_and_args(0xFFFFFFFF, 0)
        )
        self.assertEqual(
            b'\xff\xff\xff\xff\1', encode_token_and_args(0xFFFFFFFF, -1)
        )
        self.assertEqual(
            b'\xff\xff\xff\xff\2', encode_token_and_args(0xFFFFFFFF, 1)
        )

    def test_float(self):
        self.assertEqual(
            b'\xff\xff\xff\xff\0\0\0\0', encode_token_and_args(0xFFFFFFFF, 0.0)
        )
        self.assertEqual(
            b'\xff\xff\xff\xff\0\0\0\x80',
            encode_token_and_args(0xFFFFFFFF, -0.0),
        )

    def test_string(self):
        self.assertEqual(
            b'\xff\xff\xff\xff\5hello',
            encode_token_and_args(0xFFFFFFFF, 'hello'),
        )
        self.assertEqual(
            b'\xff\xff\xff\xff\x7f' + b'!' * 127,
            encode_token_and_args(0xFFFFFFFF, '!' * 127),
        )

    def test_string_too_long(self):
        self.assertEqual(
            b'\xff\xff\xff\xff\xff' + b'!' * 127,
            encode_token_and_args(0xFFFFFFFF, '!' * 128),
        )

    def test_bytes(self):
        self.assertEqual(
            b'\xff\xff\xff\xff\4\0yo\0',
            encode_token_and_args(0xFFFFFFFF, '\0yo\0'),
        )

    def test_bytes_too_long(self):
        self.assertEqual(
            b'\xff\xff\xff\xff\xff' + b'?' * 127,
            encode_token_and_args(0xFFFFFFFF, b'?' * 200),
        )

    def test_multiple_args(self):
        self.assertEqual(
            b'\xdd\xcc\xbb\xaa\0', encode_token_and_args(0xAABBCCDD, 0)
        )


class TestIntegerEncoding(unittest.TestCase):
    """Test encoding variable-length integers."""

    def test_encode_generated_data(self):
        test_data = varint_test_data.TEST_DATA
        self.assertGreater(len(test_data), 100)

        for _, signed, _, unsigned, encoded in test_data:
            # Skip numbers that are larger than 32-bits, since they aren't
            # supported currently.
            if int(unsigned).bit_length() > 32:
                continue

            # Encode the value as an arg, but skip the 4 bytes for the token.
            self.assertEqual(encode_token_and_args(0, int(signed))[4:], encoded)
            self.assertEqual(
                encode_token_and_args(0, int(unsigned))[4:], encoded
            )


if __name__ == '__main__':
    unittest.main()
