#!/usr/bin/env python3
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
"""Tests decoding a proto with tokenized fields."""

import base64
import unittest

from pw_tokenizer_tests.detokenize_proto_test_pb2 import TheMessage

from pw_tokenizer import detokenize, encode, tokens
from pw_tokenizer.proto import detokenize_fields, decode_optionally_tokenized

_DATABASE = tokens.Database(
    [
        tokens.TokenizedStringEntry(0xAABBCCDD, "Luke, we're gonna have %s"),
        tokens.TokenizedStringEntry(0x12345678, "This string has a $oeQAAA=="),
        tokens.TokenizedStringEntry(0x0000E4A1, "recursive token"),
    ]
)
_DETOKENIZER = detokenize.Detokenizer(_DATABASE)


class TestDetokenizeProtoFields(unittest.TestCase):
    """Tests detokenizing optionally tokenized proto fields."""

    def test_plain_text(self) -> None:
        proto = TheMessage(message=b'boring conversation anyway!')
        detokenize_fields(_DETOKENIZER, proto)
        self.assertEqual(proto.message, b'boring conversation anyway!')

    def test_binary(self) -> None:
        proto = TheMessage(message=b'\xDD\xCC\xBB\xAA\x07company')
        detokenize_fields(_DETOKENIZER, proto)
        self.assertEqual(proto.message, b"Luke, we're gonna have company")

    def test_binary_missing_arguments(self) -> None:
        proto = TheMessage(message=b'\xDD\xCC\xBB\xAA')
        detokenize_fields(_DETOKENIZER, proto)
        self.assertEqual(proto.message, b"Luke, we're gonna have %s")

    def test_recursive_binary(self) -> None:
        proto = TheMessage(message=b'\x78\x56\x34\x12')
        detokenize_fields(_DETOKENIZER, proto)
        self.assertEqual(proto.message, b"This string has a recursive token")

    def test_base64(self) -> None:
        base64_msg = encode.prefixed_base64(b'\xDD\xCC\xBB\xAA\x07company')
        proto = TheMessage(message=base64_msg.encode())
        detokenize_fields(_DETOKENIZER, proto)
        self.assertEqual(proto.message, b"Luke, we're gonna have company")

    def test_recursive_base64(self) -> None:
        base64_msg = encode.prefixed_base64(b'\x78\x56\x34\x12')
        proto = TheMessage(message=base64_msg.encode())
        detokenize_fields(_DETOKENIZER, proto)
        self.assertEqual(proto.message, b"This string has a recursive token")

    def test_plain_text_with_prefixed_base64(self) -> None:
        base64_msg = encode.prefixed_base64(b'\xDD\xCC\xBB\xAA\x09pancakes!')
        proto = TheMessage(message=f'Good morning, {base64_msg}'.encode())
        detokenize_fields(_DETOKENIZER, proto)
        self.assertEqual(
            proto.message, b"Good morning, Luke, we're gonna have pancakes!"
        )

    def test_unknown_token_not_utf8(self) -> None:
        proto = TheMessage(message=b'\xFE\xED\xF0\x0D')
        detokenize_fields(_DETOKENIZER, proto)
        self.assertEqual(
            proto.message.decode(), encode.prefixed_base64(b'\xFE\xED\xF0\x0D')
        )

    def test_only_control_characters(self) -> None:
        proto = TheMessage(message=b'\1\2\3\4')
        detokenize_fields(_DETOKENIZER, proto)
        self.assertEqual(
            proto.message.decode(), encode.prefixed_base64(b'\1\2\3\4')
        )


class TestDecodeOptionallyTokenized(unittest.TestCase):
    """Tests optional detokenization directly."""

    def setUp(self):
        self.detok = detokenize.Detokenizer(
            tokens.Database(
                [
                    tokens.TokenizedStringEntry(0, 'cheese'),
                    tokens.TokenizedStringEntry(1, 'on pizza'),
                    tokens.TokenizedStringEntry(2, 'is quite good'),
                    tokens.TokenizedStringEntry(3, 'they say'),
                ]
            )
        )

    def test_found_binary_token(self):
        self.assertEqual(
            'on pizza',
            decode_optionally_tokenized(self.detok, b'\x01\x00\x00\x00'),
        )

    def test_missing_binary_token(self):
        self.assertEqual(
            '$' + base64.b64encode(b'\xD5\x8A\xF9\x2A\x8A').decode(),
            decode_optionally_tokenized(self.detok, b'\xD5\x8A\xF9\x2A\x8A'),
        )

    def test_found_b64_token(self):
        b64_bytes = b'$' + base64.b64encode(b'\x03\x00\x00\x00')
        self.assertEqual(
            'they say', decode_optionally_tokenized(self.detok, b64_bytes)
        )

    def test_missing_b64_token(self):
        b64_bytes = b'$' + base64.b64encode(b'\xD5\x8A\xF9\x2A\x8A')
        self.assertEqual(
            b64_bytes.decode(),
            decode_optionally_tokenized(self.detok, b64_bytes),
        )

    def test_found_alternate_prefix(self):
        b64_bytes = b'~' + base64.b64encode(b'\x00\x00\x00\x00')
        self.assertEqual(
            'cheese', decode_optionally_tokenized(self.detok, b64_bytes, b'~')
        )

    def test_missing_alternate_prefix(self):
        b64_bytes = b'~' + base64.b64encode(b'\x02\x00\x00\x00')
        self.assertEqual(
            b64_bytes.decode(),
            decode_optionally_tokenized(self.detok, b64_bytes, b'^'),
        )


if __name__ == '__main__':
    unittest.main()
