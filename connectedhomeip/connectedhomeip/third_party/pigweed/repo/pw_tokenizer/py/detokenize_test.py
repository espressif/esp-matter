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
"""Tests for detokenize."""

import base64
import datetime as dt
import io
import os
from pathlib import Path
import struct
import tempfile
import unittest
from unittest import mock

from pw_tokenizer import database
from pw_tokenizer import detokenize
from pw_tokenizer import elf_reader
from pw_tokenizer import tokens


# This function is not part of this test. It was used to generate the binary
# strings for EMPTY_ELF and ELF_WITH_TOKENIZER_SECTIONS. It takes a path and
# returns a Python byte string suitable for copying into Python source code.
def path_to_byte_string(path):
    with open(path, 'rb') as fd:
        data = fd.read()

    output = []
    indices = iter(range(len(data)))

    while True:
        line = ''

        while len(line) < 70:
            try:
                i = next(indices)
            except StopIteration:
                break

            line += repr(data[i : i + 1])[2:-1].replace("'", r'\'')

        if not line:
            return ''.join(output)

        output.append("    b'{}'\n".format(''.join(line)))


# This is an empty ELF file. It was created from the ELF file for
# tokenize_test.cc with the command:
#
#   arm-none-eabi-objcopy -S --only-section NO_SECTIONS_PLEASE <ELF> <OUTPUT>
#
# The resulting ELF was converted to a Python binary string using
# path_to_byte_string function above.
EMPTY_ELF = (
    b'\x7fELF\x01\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00(\x00\x01'
    b'\x00\x00\x00\xd1\x83\x00\x084\x00\x00\x00\xe0\x00\x00\x00\x00\x04\x00\x05'
    b'4\x00 \x00\x05\x00(\x00\x02\x00\x01\x00\x01\x00\x00\x00\xd4\x00\x00\x00'
    b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x07\x00'
    b'\x00\x00\x00\x00\x01\x00\x01\x00\x00\x00\xd4\x00\x00\x00\x00\x00\x00\x00'
    b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x06\x00\x00\x00\x00\x00'
    b'\x01\x00\x01\x00\x00\x00\xd4\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
    b'\x00\x00\x00\x00\x00\x00\x00\x00\x06\x00\x00\x00\x00\x00\x01\x00\x01\x00'
    b'\x00\x00\xd4\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
    b'\x00\x00\x00\x00\x06\x00\x00\x00\x00\x00\x01\x00\x01\x00\x00\x00\xd4\x00'
    b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
    b'\x06\x00\x00\x00\x00\x00\x01\x00\x00.shstrtab\x00\x00\x00\x00\x00\x00\x00'
    b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
    b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01'
    b'\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xd4\x00\x00'
    b'\x00\x0b\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00'
    b'\x00\x00\x00'
)

# This is an ELF file with only the pw_tokenizer sections. It was created
# from a tokenize_test binary built for the STM32F429i Discovery board. The
# pw_tokenizer sections were extracted with this command:
#
#   arm-none-eabi-objcopy -S --only-section ".pw_tokenizer*" <ELF> <OUTPUT>
#
ELF_WITH_TOKENIZER_SECTIONS_PATH = Path(__file__).parent.joinpath(
    'example_binary_with_tokenized_strings.elf'
)
ELF_WITH_TOKENIZER_SECTIONS = ELF_WITH_TOKENIZER_SECTIONS_PATH.read_bytes()

TOKENS_IN_ELF = 22
TOKENS_IN_ELF_WITH_TOKENIZER_SECTIONS = 26

# 0x2e668cd6 is 'Jello, world!' (which is also used in database_test.py).
JELLO_WORLD_TOKEN = b'\xd6\x8c\x66\x2e'


class DetokenizeTest(unittest.TestCase):
    """Tests the detokenize.Detokenizer."""

    def test_simple(self):
        detok = detokenize.Detokenizer(
            tokens.Database(
                [
                    tokens.TokenizedStringEntry(
                        0xCDAB, '%02d %s %c%%', date_removed=dt.datetime.now()
                    )
                ]
            )
        )
        self.assertEqual(
            str(detok.detokenize(b'\xab\xcd\0\0\x02\x03Two\x66')), '01 Two 3%'
        )

    def test_detokenize_extra_data_is_unsuccessful(self):
        detok = detokenize.Detokenizer(
            tokens.Database(
                [
                    tokens.TokenizedStringEntry(
                        1, 'no args', date_removed=dt.datetime(1, 1, 1)
                    )
                ]
            )
        )

        result = detok.detokenize(b'\x01\0\0\0\x04args')
        self.assertEqual(len(result.failures), 1)
        string, args, remaining = result.failures[0]
        self.assertEqual('no args', string)
        self.assertFalse(args)
        self.assertEqual(b'\x04args', remaining)
        self.assertEqual('no args', string)
        self.assertEqual('no args', str(result))

    def test_detokenize_zero_extend_short_token_with_no_args(self):
        detok = detokenize.Detokenizer(
            tokens.Database(
                [tokens.TokenizedStringEntry(0xCDAB, 'This token is 16 bits')]
            )
        )
        self.assertEqual(
            str(detok.detokenize(b'\xab\xcd')), 'This token is 16 bits'
        )

    def test_detokenize_missing_data_is_unsuccessful(self):
        detok = detokenize.Detokenizer(
            tokens.Database(
                [
                    tokens.TokenizedStringEntry(
                        2, '%s', date_removed=dt.datetime(1, 1, 1)
                    )
                ]
            )
        )

        result = detok.detokenize(b'\x02\0\0\0')
        string, args, remaining = result.failures[0]
        self.assertEqual('%s', string)
        self.assertEqual(len(args), 1)
        self.assertEqual(b'', remaining)
        self.assertEqual(len(result.failures), 1)
        self.assertEqual('%s', str(result))

    def test_detokenize_missing_data_with_errors_is_unsuccessful(self):
        detok = detokenize.Detokenizer(
            tokens.Database(
                [
                    tokens.TokenizedStringEntry(
                        2, '%s', date_removed=dt.datetime(1, 1, 1)
                    )
                ]
            ),
            show_errors=True,
        )

        result = detok.detokenize(b'\x02\0\0\0')
        string, args, remaining = result.failures[0]
        self.assertIn('%s MISSING', string)
        self.assertEqual(len(args), 1)
        self.assertEqual(b'', remaining)
        self.assertEqual(len(result.failures), 1)
        self.assertIn('%s MISSING', str(result))

    def test_unparsed_data(self):
        detok = detokenize.Detokenizer(
            tokens.Database(
                [
                    tokens.TokenizedStringEntry(
                        1, 'no args', date_removed=dt.datetime(100, 1, 1)
                    ),
                ]
            )
        )
        result = detok.detokenize(b'\x01\0\0\0o_o')
        self.assertFalse(result.ok())
        self.assertEqual('no args', str(result))
        self.assertIn('o_o', repr(result))
        self.assertIn('decoding failed', result.error_message())

    def test_empty_db(self):
        detok = detokenize.Detokenizer(io.BytesIO(EMPTY_ELF))
        self.assertFalse(detok.detokenize(b'\x12\x34\0\0').ok())
        self.assertIn(
            'unknown token', detok.detokenize(b'1234').error_message()
        )
        self.assertIn('unknown token', repr(detok.detokenize(b'1234')))

        self.assertEqual(
            '$' + base64.b64encode(b'1234').decode(),
            str(detok.detokenize(b'1234')),
        )

        self.assertIsNone(detok.detokenize(b'').token)

    def test_empty_db_show_errors(self):
        detok = detokenize.Detokenizer(io.BytesIO(EMPTY_ELF), show_errors=True)
        self.assertFalse(detok.detokenize(b'\x12\x34\0\0').ok())
        self.assertIn(
            'unknown token', detok.detokenize(b'1234').error_message()
        )
        self.assertIn('unknown token', repr(detok.detokenize(b'1234')))
        self.assertIn('unknown token', str(detok.detokenize(b'1234')))

        self.assertIsNone(detok.detokenize(b'').token)

    def test_missing_token_show_errors(self):
        detok = detokenize.Detokenizer(io.BytesIO(EMPTY_ELF), show_errors=True)
        self.assertIn('missing token', detok.detokenize(b'').error_message())
        self.assertIn('missing token', str(detok.detokenize(b'')))

    def test_missing_token(self):
        detok = detokenize.Detokenizer(io.BytesIO(EMPTY_ELF))
        self.assertIn('missing token', detok.detokenize(b'').error_message())
        self.assertEqual('$', str(detok.detokenize(b'')))

    def test_unknown_shorter_token_show_error(self):
        detok = detokenize.Detokenizer(io.BytesIO(EMPTY_ELF), show_errors=True)

        self.assertIn('unknown token', detok.detokenize(b'1').error_message())
        self.assertIn('unknown token', str(detok.detokenize(b'1')))
        self.assertIn('unknown token', repr(detok.detokenize(b'1')))

        self.assertIn('unknown token', detok.detokenize(b'123').error_message())
        self.assertIn('unknown token', str(detok.detokenize(b'123')))
        self.assertIn('unknown token', repr(detok.detokenize(b'123')))

    def test_unknown_shorter_token(self):
        detok = detokenize.Detokenizer(io.BytesIO(EMPTY_ELF))

        self.assertEqual(
            'unknown token 00000001', detok.detokenize(b'\1').error_message()
        )
        self.assertEqual(
            '$' + base64.b64encode(b'\1\0\0\0').decode(),
            str(detok.detokenize(b'\1')),
        )
        self.assertIn('unknown token 00000001', repr(detok.detokenize(b'\1')))

        self.assertEqual(
            'unknown token 00030201',
            detok.detokenize(b'\1\2\3').error_message(),
        )
        self.assertEqual(
            '$' + base64.b64encode(b'\1\2\3\0').decode(),
            str(detok.detokenize(b'\1\2\3')),
        )
        self.assertIn(
            'unknown token 00030201', repr(detok.detokenize(b'\1\2\3'))
        )

    def test_decode_from_elf_data(self):
        detok = detokenize.Detokenizer(io.BytesIO(ELF_WITH_TOKENIZER_SECTIONS))

        self.assertTrue(detok.detokenize(JELLO_WORLD_TOKEN).ok())
        self.assertEqual(
            str(detok.detokenize(JELLO_WORLD_TOKEN)), 'Jello, world!'
        )

        undecoded_args = detok.detokenize(JELLO_WORLD_TOKEN + b'some junk')
        self.assertFalse(undecoded_args.ok())
        self.assertEqual(str(undecoded_args), 'Jello, world!')

        self.assertTrue(detok.detokenize(b'\0\0\0\0').ok())
        self.assertEqual(str(detok.detokenize(b'\0\0\0\0')), '')

    def test_decode_from_elf_file(self):
        """Test decoding from an elf file."""
        detok = detokenize.Detokenizer(io.BytesIO(ELF_WITH_TOKENIZER_SECTIONS))
        expected_tokens = frozenset(detok.database.token_to_entries.keys())

        with tempfile.NamedTemporaryFile('wb', delete=False) as elf:
            try:
                elf.write(ELF_WITH_TOKENIZER_SECTIONS)
                elf.close()

                # Open ELF by file object
                with open(elf.name, 'rb') as fd:
                    detok = detokenize.Detokenizer(fd)

                self.assertEqual(
                    expected_tokens,
                    frozenset(detok.database.token_to_entries.keys()),
                )

                # Open ELF by path
                detok = detokenize.Detokenizer(elf.name)
                self.assertEqual(
                    expected_tokens,
                    frozenset(detok.database.token_to_entries.keys()),
                )

                # Open ELF by elf_reader.Elf
                with open(elf.name, 'rb') as fd:
                    detok = detokenize.Detokenizer(elf_reader.Elf(fd))

                self.assertEqual(
                    expected_tokens,
                    frozenset(detok.database.token_to_entries.keys()),
                )
            finally:
                os.unlink(elf.name)

    def test_decode_from_csv_file(self):
        detok = detokenize.Detokenizer(io.BytesIO(ELF_WITH_TOKENIZER_SECTIONS))
        expected_tokens = frozenset(detok.database.token_to_entries.keys())

        csv_database = str(detok.database)
        self.assertEqual(len(csv_database.splitlines()), TOKENS_IN_ELF)

        with tempfile.NamedTemporaryFile('w', delete=False) as csv_file:
            try:
                csv_file.write(csv_database)
                csv_file.close()

                # Open CSV by path
                detok = detokenize.Detokenizer(csv_file.name)
                self.assertEqual(
                    expected_tokens,
                    frozenset(detok.database.token_to_entries.keys()),
                )

                # Open CSV by file object
                with open(csv_file.name) as fd:
                    detok = detokenize.Detokenizer(fd)

                self.assertEqual(
                    expected_tokens,
                    frozenset(detok.database.token_to_entries.keys()),
                )
            finally:
                os.unlink(csv_file.name)

    def test_create_detokenizer_with_token_database(self):
        detok = detokenize.Detokenizer(io.BytesIO(ELF_WITH_TOKENIZER_SECTIONS))
        expected_tokens = frozenset(detok.database.token_to_entries.keys())

        detok = detokenize.Detokenizer(detok.database)
        self.assertEqual(
            expected_tokens, frozenset(detok.database.token_to_entries.keys())
        )


class DetokenizeWithCollisions(unittest.TestCase):
    """Tests collision resolution."""

    def setUp(self):
        super().setUp()
        token = 0xBAAD

        # Database with several conflicting tokens.
        self.detok = detokenize.Detokenizer(
            tokens.Database(
                [
                    tokens.TokenizedStringEntry(
                        token, 'REMOVED', date_removed=dt.datetime(9, 1, 1)
                    ),
                    tokens.TokenizedStringEntry(token, 'newer'),
                    tokens.TokenizedStringEntry(
                        token, 'A: %d', date_removed=dt.datetime(30, 5, 9)
                    ),
                    tokens.TokenizedStringEntry(
                        token, 'B: %c', date_removed=dt.datetime(30, 5, 10)
                    ),
                    tokens.TokenizedStringEntry(token, 'C: %s'),
                    tokens.TokenizedStringEntry(token, '%d%u'),
                    tokens.TokenizedStringEntry(token, '%s%u %d'),
                    tokens.TokenizedStringEntry(1, '%s'),
                    tokens.TokenizedStringEntry(1, '%d'),
                    tokens.TokenizedStringEntry(2, 'Three %s %s %s'),
                    tokens.TokenizedStringEntry(2, 'Five %d %d %d %d %s'),
                ]
            )
        )

    def test_collision_no_args_favors_most_recently_present(self):
        no_args = self.detok.detokenize(b'\xad\xba\0\0')
        self.assertFalse(no_args.ok())
        self.assertEqual(len(no_args.successes), 2)
        self.assertEqual(len(no_args.failures), 5)
        self.assertEqual(len(no_args.matches()), 7)
        self.assertEqual(str(no_args), 'newer')
        self.assertEqual(len(no_args.best_result()[1]), 0)
        self.assertEqual(no_args.best_result()[0], 'newer')

    def test_collision_one_integer_arg_favors_most_recently_present(self):
        multiple_correct = self.detok.detokenize(b'\xad\xba\0\0\x7a')
        self.assertFalse(multiple_correct.ok())
        self.assertIn('ERROR', repr(multiple_correct))
        self.assertEqual(len(multiple_correct.successes), 2)
        self.assertEqual(len(multiple_correct.failures), 5)
        self.assertEqual(len(multiple_correct.matches()), 7)
        self.assertEqual(str(multiple_correct), 'B: =')

    def test_collision_one_integer_arg_favor_successful_decode(self):
        # One string decodes successfully, since the arg is out of range for %c.
        int_arg = self.detok.detokenize(b'\xad\xba\0\0\xfe\xff\xff\xff\x0f')
        self.assertTrue(int_arg.ok())
        self.assertEqual(str(int_arg), 'A: 2147483647')

    def test_collision_one_string_arg_favors_successful_decode(self):
        # One string decodes successfully, since decoding the argument as an
        # integer does not decode all the data.
        string_arg = self.detok.detokenize(b'\xad\xba\0\0\x02Hi')
        self.assertTrue(string_arg.ok())
        self.assertEqual(str(string_arg), 'C: Hi')

    def test_collision_one_string_arg_favors_decoding_all_data(self):
        result = self.detok.detokenize(b'\1\0\0\0\x83hi')
        self.assertEqual(len(result.failures), 2)
        # Should resolve to the string since %d would leave one byte behind.
        self.assertEqual(str(result), '%s')

    def test_collision_multiple_args_favors_decoding_more_arguments(self):
        result = self.detok.detokenize(b'\2\0\0\0\1\2\1\4\5')
        self.assertEqual(len(result.matches()), 2)
        self.assertEqual(result.matches()[0][0], 'Five -1 1 -1 2 %s')
        self.assertEqual(result.matches()[1][0], 'Three \2 \4 %s')

    def test_collision_multiple_args_favors_decoding_all_arguments(self):
        unambiguous = self.detok.detokenize(b'\xad\xba\0\0\x01#\x00\x01')
        self.assertTrue(unambiguous.ok())
        self.assertEqual(len(unambiguous.matches()), 7)
        self.assertEqual('#0 -1', str(unambiguous))
        self.assertIn('#0 -1', repr(unambiguous))


@mock.patch('os.path.getmtime')
class AutoUpdatingDetokenizerTest(unittest.TestCase):
    """Tests the AutoUpdatingDetokenizer class."""

    def test_update(self, mock_getmtime):
        """Tests the update command."""

        db = database.load_token_database(
            io.BytesIO(ELF_WITH_TOKENIZER_SECTIONS)
        )
        self.assertEqual(len(db), TOKENS_IN_ELF)

        the_time = [100]

        def move_back_time_if_file_exists(path):
            if os.path.exists(path):
                the_time[0] -= 1
                return the_time[0]

            raise FileNotFoundError

        mock_getmtime.side_effect = move_back_time_if_file_exists

        with tempfile.NamedTemporaryFile('wb', delete=False) as file:
            try:
                file.close()

                detok = detokenize.AutoUpdatingDetokenizer(
                    file.name, min_poll_period_s=0
                )
                self.assertFalse(detok.detokenize(JELLO_WORLD_TOKEN).ok())

                with open(file.name, 'wb') as fd:
                    tokens.write_binary(db, fd)

                self.assertTrue(detok.detokenize(JELLO_WORLD_TOKEN).ok())
            finally:
                os.unlink(file.name)

        # The database stays around if the file is deleted.
        self.assertTrue(detok.detokenize(JELLO_WORLD_TOKEN).ok())

    def test_no_update_if_time_is_same(self, mock_getmtime):
        mock_getmtime.return_value = 100

        with tempfile.NamedTemporaryFile('wb', delete=False) as file:
            try:
                tokens.write_csv(
                    database.load_token_database(
                        io.BytesIO(ELF_WITH_TOKENIZER_SECTIONS)
                    ),
                    file,
                )
                file.close()

                detok = detokenize.AutoUpdatingDetokenizer(
                    file, min_poll_period_s=0
                )
                self.assertTrue(detok.detokenize(JELLO_WORLD_TOKEN).ok())

                # Empty the database, but keep the mock modified time the same.
                with open(file.name, 'wb'):
                    pass

                self.assertTrue(detok.detokenize(JELLO_WORLD_TOKEN).ok())
                self.assertTrue(detok.detokenize(JELLO_WORLD_TOKEN).ok())

                # Move back time so the now-empty file is reloaded.
                mock_getmtime.return_value = 50
                self.assertFalse(detok.detokenize(JELLO_WORLD_TOKEN).ok())
            finally:
                os.unlink(file.name)

    def test_token_domains(self, _):
        """Tests that token domains can be parsed from input filename"""
        filename_and_domain = f'{ELF_WITH_TOKENIZER_SECTIONS_PATH}#.*'
        detok_with_domain = detokenize.AutoUpdatingDetokenizer(
            filename_and_domain, min_poll_period_s=0
        )
        self.assertEqual(
            len(detok_with_domain.database),
            TOKENS_IN_ELF_WITH_TOKENIZER_SECTIONS,
        )
        detok = detokenize.AutoUpdatingDetokenizer(
            str(ELF_WITH_TOKENIZER_SECTIONS_PATH), min_poll_period_s=0
        )
        self.assertEqual(len(detok.database), TOKENS_IN_ELF)


def _next_char(message: bytes) -> bytes:
    return bytes(b + 1 for b in message)


class PrefixedMessageDecoderTest(unittest.TestCase):
    def setUp(self):
        super().setUp()
        self.decode = detokenize.PrefixedMessageDecoder('$', 'abcdefg')

    def test_transform_single_message(self):
        self.assertEqual(
            b'%bcde',
            b''.join(self.decode.transform(io.BytesIO(b'$abcd'), _next_char)),
        )

    def test_transform_message_amidst_other_only_affects_message(self):
        self.assertEqual(
            b'%%WHAT?%bcd%WHY? is this %ok %',
            b''.join(
                self.decode.transform(
                    io.BytesIO(b'$$WHAT?$abc$WHY? is this $ok $'), _next_char
                )
            ),
        )

    def test_transform_empty_message(self):
        self.assertEqual(
            b'%1%',
            b''.join(self.decode.transform(io.BytesIO(b'$1$'), _next_char)),
        )

    def test_transform_sequential_messages(self):
        self.assertEqual(
            b'%bcd%efghh',
            b''.join(
                self.decode.transform(io.BytesIO(b'$abc$defgh'), _next_char)
            ),
        )


class DetokenizeBase64(unittest.TestCase):
    """Tests detokenizing Base64 messages."""

    JELLO = b'$' + base64.b64encode(JELLO_WORLD_TOKEN)

    RECURSION_STRING = f'The secret message is "{JELLO.decode()}"'
    RECURSION = b'$' + base64.b64encode(
        struct.pack('I', tokens.c_hash(RECURSION_STRING))
    )

    RECURSION_STRING_2 = f"'{RECURSION.decode()}', said the spy."
    RECURSION_2 = b'$' + base64.b64encode(
        struct.pack('I', tokens.c_hash(RECURSION_STRING_2))
    )

    TEST_CASES = (
        (b'', b''),
        (b'nothing here', b'nothing here'),
        (JELLO, b'Jello, world!'),
        (JELLO + b'a', b'Jello, world!a'),
        (JELLO + b'abc', b'Jello, world!abc'),
        (JELLO + b'abc=', b'Jello, world!abc='),
        (b'$a' + JELLO + b'a', b'$aJello, world!a'),
        (b'Hello ' + JELLO + b'?', b'Hello Jello, world!?'),
        (b'$' + JELLO, b'$Jello, world!'),
        (JELLO + JELLO, b'Jello, world!Jello, world!'),
        (JELLO + b'$' + JELLO, b'Jello, world!$Jello, world!'),
        (JELLO + b'$a' + JELLO + b'bcd', b'Jello, world!$aJello, world!bcd'),
        (b'$3141', b'$3141'),
        (JELLO + b'$3141', b'Jello, world!$3141'),
        (RECURSION, b'The secret message is "Jello, world!"'),
        (
            RECURSION_2,
            b'\'The secret message is "Jello, world!"\', said the spy.',
        ),
    )

    def setUp(self):
        super().setUp()
        db = database.load_token_database(
            io.BytesIO(ELF_WITH_TOKENIZER_SECTIONS)
        )
        db.add(
            tokens.TokenizedStringEntry(tokens.c_hash(s), s)
            for s in [self.RECURSION_STRING, self.RECURSION_STRING_2]
        )
        self.detok = detokenize.Detokenizer(db)

    def test_detokenize_base64_live(self):
        for data, expected in self.TEST_CASES:
            output = io.BytesIO()
            self.detok.detokenize_base64_live(io.BytesIO(data), output, '$')

            self.assertEqual(expected, output.getvalue())

    def test_detokenize_base64_to_file(self):
        for data, expected in self.TEST_CASES:
            output = io.BytesIO()
            self.detok.detokenize_base64_to_file(data, output, '$')

            self.assertEqual(expected, output.getvalue())

    def test_detokenize_base64(self):
        for data, expected in self.TEST_CASES:
            self.assertEqual(expected, self.detok.detokenize_base64(data, b'$'))

    def test_detokenize_base64_str(self):
        for data, expected in self.TEST_CASES:
            self.assertEqual(
                expected.decode(), self.detok.detokenize_base64(data.decode())
            )


class DetokenizeBase64InfiniteRecursion(unittest.TestCase):
    """Tests that infinite Bas64 token recursion resolves."""

    def setUp(self):
        super().setUp()
        self.detok = detokenize.Detokenizer(
            tokens.Database(
                [
                    tokens.TokenizedStringEntry(0, '$AAAAAA=='),  # token for 0
                    tokens.TokenizedStringEntry(1, '$AgAAAA=='),  # token for 2
                    tokens.TokenizedStringEntry(2, '$AwAAAA=='),  # token for 3
                    tokens.TokenizedStringEntry(3, '$AgAAAA=='),  # token for 2
                ]
            )
        )

    def test_detokenize_self_recursion(self):
        for depth in range(5):
            self.assertEqual(
                self.detok.detokenize_base64(
                    b'This one is deep: $AAAAAA==', recursion=depth
                ),
                b'This one is deep: $AAAAAA==',
            )

    def test_detokenize_self_recursion_default(self):
        self.assertEqual(
            self.detok.detokenize_base64(b'This one is deep: $AAAAAA=='),
            b'This one is deep: $AAAAAA==',
        )

    def test_detokenize_cyclic_recursion_even(self):
        self.assertEqual(
            self.detok.detokenize_base64(b'I said "$AQAAAA=="', recursion=2),
            b'I said "$AgAAAA=="',
        )

    def test_detokenize_cyclic_recursion_odd(self):
        self.assertEqual(
            self.detok.detokenize_base64(b'I said "$AQAAAA=="', recursion=3),
            b'I said "$AwAAAA=="',
        )


if __name__ == '__main__':
    unittest.main()
