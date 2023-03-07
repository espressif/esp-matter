#!/usr/bin/env python3
# Copyright 2022 The Pigweed Authors
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
"""Tests the tokenized string decode module."""

from datetime import datetime
import unittest

import tokenized_string_decoding_test_data as tokenized_string
import varint_test_data
from pw_tokenizer import decode


def error(msg, value=None) -> str:
    """Formats msg as the message for an argument that failed to parse."""
    if value is None:
        return '<[{}]>'.format(msg)
    return '<[{} ({})]>'.format(msg, value)


class TestDecodeTokenized(unittest.TestCase):
    """Tests decoding tokenized strings with various arguments."""

    def test_decode_generated_data(self) -> None:
        self.assertGreater(len(tokenized_string.TEST_DATA), 100)

        for fmt, decoded, encoded in tokenized_string.TEST_DATA:
            self.assertEqual(decode.decode(fmt, encoded, True), decoded)

    def test_unicode_decode_errors(self) -> None:
        """Tests unicode errors, which do not occur in the C++ decoding code."""
        self.assertEqual(
            decode.decode('Why, %c', b'\x01', True),
            'Why, ' + error('%c ERROR', -1),
        )

        self.assertEqual(
            decode.decode('%sXY%+ldxy%u', b'\x83N\x80!\x01\x02', True),
            '{}XY{}xy{}'.format(
                error('%s ERROR', "'N\\x80!'"),
                error('%+ld SKIPPED', -1),
                error('%u SKIPPED', 1),
            ),
        )

        self.assertEqual(
            decode.decode('%s%lld%9u', b'\x82$\x80\x80', True),
            '{0}{1}{2}'.format(
                error("%s ERROR ('$\\x80')"),
                error('%lld SKIPPED'),
                error('%9u SKIPPED'),
            ),
        )

        self.assertEqual(
            decode.decode('%c', b'\xff\xff\xff\xff\x0f', True),
            error('%c ERROR', -2147483648),
        )

    def test_ignore_errors(self) -> None:
        self.assertEqual(decode.decode('Why, %c', b'\x01'), 'Why, %c')

        self.assertEqual(decode.decode('%s %d', b'\x01!'), '! %d')

    def test_pointer(self) -> None:
        """Tests pointer args, which are not natively supported in Python."""
        self.assertEqual(
            decode.decode('Hello: %p', b'\x00', True), 'Hello: 0x00000000'
        )
        self.assertEqual(
            decode.decode('%p%d%d', b'\x02\x80', True),
            '0x00000001<[%d ERROR]><[%d SKIPPED]>',
        )


class TestIntegerDecoding(unittest.TestCase):
    """Tests decoding variable-length integers."""

    def test_decode_generated_data(self) -> None:
        test_data = varint_test_data.TEST_DATA
        self.assertGreater(len(test_data), 100)

        for signed_spec, signed, unsigned_spec, unsigned, encoded in test_data:
            self.assertEqual(
                int(signed),
                decode.FormatSpec.from_string(signed_spec)
                .decode(bytearray(encoded))
                .value,
            )

            self.assertEqual(
                int(unsigned),
                decode.FormatSpec.from_string(unsigned_spec)
                .decode(bytearray(encoded))
                .value,
            )


class TestFormattedString(unittest.TestCase):
    """Tests scoring how successfully a formatted string decoded."""

    def test_no_args(self) -> None:
        result = decode.FormatString('string').format(b'')

        self.assertTrue(result.ok())
        self.assertEqual(result.score(), (True, True, 0, 0, datetime.max))

    def test_one_arg(self) -> None:
        result = decode.FormatString('%d').format(b'\0')

        self.assertTrue(result.ok())
        self.assertEqual(result.score(), (True, True, 0, 1, datetime.max))

    def test_missing_args(self) -> None:
        result = decode.FormatString('%p%d%d').format(b'\x02\x80')

        self.assertFalse(result.ok())
        self.assertEqual(result.score(), (False, True, -2, 3, datetime.max))
        self.assertGreater(result.score(), result.score(datetime.now()))
        self.assertGreater(
            result.score(datetime.now()), result.score(datetime.min)
        )

    def test_compare_score(self) -> None:
        all_args_ok = decode.FormatString('%d%d%d').format(b'\0\0\0')
        missing_one_arg = decode.FormatString('%d%d%d').format(b'\0\0')
        missing_two_args = decode.FormatString('%d%d%d').format(b'\0')
        all_args_extra_data = decode.FormatString('%d%d%d').format(b'\0\0\0\1')
        missing_one_arg_extra_data = decode.FormatString('%d%d%d').format(
            b'\0' + b'\x80' * 100
        )

        self.assertGreater(all_args_ok.score(), missing_one_arg.score())
        self.assertGreater(missing_one_arg.score(), missing_two_args.score())
        self.assertGreater(
            missing_two_args.score(), all_args_extra_data.score()
        )
        self.assertGreater(
            all_args_extra_data.score(), missing_one_arg_extra_data.score()
        )


if __name__ == '__main__':
    unittest.main()
