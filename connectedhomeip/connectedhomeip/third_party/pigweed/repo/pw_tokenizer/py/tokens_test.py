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
"""Tests for the tokens module."""

from datetime import datetime
import io
import logging
from pathlib import Path
import shutil
import tempfile
from typing import Iterator
import unittest

from pw_tokenizer import tokens
from pw_tokenizer.tokens import c_hash, DIR_DB_SUFFIX, _LOG

CSV_DATABASE = '''\
00000000,2019-06-10,""
141c35d5,          ,"The answer: ""%s"""
2db1515f,          ,"%u%d%02x%X%hu%hhu%d%ld%lu%lld%llu%c%c%c"
2e668cd6,2019-06-11,"Jello, world!"
31631781,          ,"%d"
61fd1e26,          ,"%ld"
68ab92da,          ,"%s there are %x (%.2f) of them%c"
7b940e2a,          ,"Hello %s! %hd %e"
851beeb6,          ,"%u %d"
881436a0,          ,"The answer is: %s"
ad002c97,          ,"%llx"
b3653e13,2019-06-12,"Jello!"
b912567b,          ,"%x%lld%1.2f%s"
cc6d3131,2020-01-01,"Jello?"
e13b0f94,          ,"%llu"
e65aefef,2019-06-10,"Won't fit : %s%d"
'''

# The date 2019-06-10 is 07E3-06-0A in hex. In database order, it's 0A 06 E3 07.
BINARY_DATABASE = (
    b'TOKENS\x00\x00\x10\x00\x00\x00\0\0\0\0'  # header (0x10 entries)
    b'\x00\x00\x00\x00\x0a\x06\xe3\x07'  # 0x01
    b'\xd5\x35\x1c\x14\xff\xff\xff\xff'  # 0x02
    b'\x5f\x51\xb1\x2d\xff\xff\xff\xff'  # 0x03
    b'\xd6\x8c\x66\x2e\x0b\x06\xe3\x07'  # 0x04
    b'\x81\x17\x63\x31\xff\xff\xff\xff'  # 0x05
    b'\x26\x1e\xfd\x61\xff\xff\xff\xff'  # 0x06
    b'\xda\x92\xab\x68\xff\xff\xff\xff'  # 0x07
    b'\x2a\x0e\x94\x7b\xff\xff\xff\xff'  # 0x08
    b'\xb6\xee\x1b\x85\xff\xff\xff\xff'  # 0x09
    b'\xa0\x36\x14\x88\xff\xff\xff\xff'  # 0x0a
    b'\x97\x2c\x00\xad\xff\xff\xff\xff'  # 0x0b
    b'\x13\x3e\x65\xb3\x0c\x06\xe3\x07'  # 0x0c
    b'\x7b\x56\x12\xb9\xff\xff\xff\xff'  # 0x0d
    b'\x31\x31\x6d\xcc\x01\x01\xe4\x07'  # 0x0e
    b'\x94\x0f\x3b\xe1\xff\xff\xff\xff'  # 0x0f
    b'\xef\xef\x5a\xe6\x0a\x06\xe3\x07'  # 0x10
    b'\x00'
    b'The answer: "%s"\x00'
    b'%u%d%02x%X%hu%hhu%d%ld%lu%lld%llu%c%c%c\x00'
    b'Jello, world!\x00'
    b'%d\x00'
    b'%ld\x00'
    b'%s there are %x (%.2f) of them%c\x00'
    b'Hello %s! %hd %e\x00'
    b'%u %d\x00'
    b'The answer is: %s\x00'
    b'%llx\x00'
    b'Jello!\x00'
    b'%x%lld%1.2f%s\x00'
    b'Jello?\x00'
    b'%llu\x00'
    b'Won\'t fit : %s%d\x00'
)

INVALID_CSV = """\
1,,"Whoa there!"
2,this is totally invalid,"Whoa there!"
3,,"This one's OK"
,,"Also broken"
5,1845-2-2,"I'm %s fine"
6,"Missing fields"
"""

CSV_DATABASE_2 = '''\
00000000,          ,""
141c35d5,          ,"The answer: ""%s"""
29aef586,          ,"1234"
2b78825f,          ,"[:-)"
2e668cd6,          ,"Jello, world!"
31631781,          ,"%d"
61fd1e26,          ,"%ld"
68ab92da,          ,"%s there are %x (%.2f) of them%c"
7b940e2a,          ,"Hello %s! %hd %e"
7da55d52,          ,">:-[]"
7f35a9a5,          ,"TestName"
851beeb6,          ,"%u %d"
881436a0,          ,"The answer is: %s"
88808930,          ,"%u%d%02x%X%hu%hhd%d%ld%lu%lld%llu%c%c%c"
92723f44,          ,"???"
a09d6698,          ,"won-won-won-wonderful"
aa9ffa66,          ,"void pw::tokenizer::{anonymous}::TestName()"
ad002c97,          ,"%llx"
b3653e13,          ,"Jello!"
cc6d3131,          ,"Jello?"
e13b0f94,          ,"%llu"
e65aefef,          ,"Won't fit : %s%d"
'''

CSV_DATABASE_3 = """\
17fa86d3,          ,"hello"
18c5017c,          ,"yes"
59b2701c,          ,"The answer was: %s"
881436a0,          ,"The answer is: %s"
d18ada0f,          ,"something"
"""

CSV_DATABASE_4 = '''\
00000000,          ,""
141c35d5,          ,"The answer: ""%s"""
17fa86d3,          ,"hello"
18c5017c,          ,"yes"
29aef586,          ,"1234"
2b78825f,          ,"[:-)"
2e668cd6,          ,"Jello, world!"
31631781,          ,"%d"
59b2701c,          ,"The answer was: %s"
61fd1e26,          ,"%ld"
68ab92da,          ,"%s there are %x (%.2f) of them%c"
7b940e2a,          ,"Hello %s! %hd %e"
7da55d52,          ,">:-[]"
7f35a9a5,          ,"TestName"
851beeb6,          ,"%u %d"
881436a0,          ,"The answer is: %s"
88808930,          ,"%u%d%02x%X%hu%hhd%d%ld%lu%lld%llu%c%c%c"
92723f44,          ,"???"
a09d6698,          ,"won-won-won-wonderful"
aa9ffa66,          ,"void pw::tokenizer::{anonymous}::TestName()"
ad002c97,          ,"%llx"
b3653e13,          ,"Jello!"
cc6d3131,          ,"Jello?"
d18ada0f,          ,"something"
e13b0f94,          ,"%llu"
e65aefef,          ,"Won't fit : %s%d"
'''


def read_db_from_csv(csv_str: str) -> tokens.Database:
    with io.StringIO(csv_str) as csv_db:
        return tokens.Database(tokens.parse_csv(csv_db))


def _entries(*strings: str) -> Iterator[tokens.TokenizedStringEntry]:
    for string in strings:
        yield tokens.TokenizedStringEntry(c_hash(string), string)


class TokenDatabaseTest(unittest.TestCase):
    """Tests the token database class."""

    def test_csv(self) -> None:
        db = read_db_from_csv(CSV_DATABASE)
        self.assertEqual(str(db), CSV_DATABASE)

        db = read_db_from_csv('')
        self.assertEqual(str(db), '')

    def test_csv_formatting(self) -> None:
        db = read_db_from_csv('')
        self.assertEqual(str(db), '')

        db = read_db_from_csv('abc123,2048-4-1,Fake string\n')
        self.assertEqual(str(db), '00abc123,2048-04-01,"Fake string"\n')

        db = read_db_from_csv(
            '1,1990-01-01,"Quotes"""\n' '0,1990-02-01,"Commas,"",,"\n'
        )
        self.assertEqual(
            str(db),
            (
                '00000000,1990-02-01,"Commas,"",,"\n'
                '00000001,1990-01-01,"Quotes"""\n'
            ),
        )

    def test_bad_csv(self) -> None:
        with self.assertLogs(_LOG, logging.ERROR) as logs:
            db = read_db_from_csv(INVALID_CSV)

        self.assertGreaterEqual(len(logs.output), 3)
        self.assertEqual(len(db.token_to_entries), 3)

        self.assertEqual(db.token_to_entries[1][0].string, 'Whoa there!')
        self.assertFalse(db.token_to_entries[2])
        self.assertEqual(db.token_to_entries[3][0].string, "This one's OK")
        self.assertFalse(db.token_to_entries[4])
        self.assertEqual(db.token_to_entries[5][0].string, "I'm %s fine")
        self.assertFalse(db.token_to_entries[6])

    def test_lookup(self) -> None:
        db = read_db_from_csv(CSV_DATABASE)
        self.assertEqual(db.token_to_entries[0x9999], [])

        matches = db.token_to_entries[0x2E668CD6]
        self.assertEqual(len(matches), 1)
        jello = matches[0]

        self.assertEqual(jello.token, 0x2E668CD6)
        self.assertEqual(jello.string, 'Jello, world!')
        self.assertEqual(jello.date_removed, datetime(2019, 6, 11))

        matches = db.token_to_entries[0xE13B0F94]
        self.assertEqual(len(matches), 1)
        llu = matches[0]
        self.assertEqual(llu.token, 0xE13B0F94)
        self.assertEqual(llu.string, '%llu')
        self.assertIsNone(llu.date_removed)

        (answer,) = db.token_to_entries[0x141C35D5]
        self.assertEqual(answer.string, 'The answer: "%s"')

    def test_collisions(self) -> None:
        hash_1 = tokens.c_hash('o000', 96)
        hash_2 = tokens.c_hash('0Q1Q', 96)
        self.assertEqual(hash_1, hash_2)

        db = tokens.Database.from_strings(['o000', '0Q1Q'])

        self.assertEqual(len(db.token_to_entries[hash_1]), 2)
        self.assertCountEqual(
            [entry.string for entry in db.token_to_entries[hash_1]],
            ['o000', '0Q1Q'],
        )

    def test_purge(self) -> None:
        db = read_db_from_csv(CSV_DATABASE)
        original_length = len(db.token_to_entries)

        self.assertEqual(db.token_to_entries[0][0].string, '')
        self.assertEqual(db.token_to_entries[0x31631781][0].string, '%d')
        self.assertEqual(
            db.token_to_entries[0x2E668CD6][0].string, 'Jello, world!'
        )
        self.assertEqual(db.token_to_entries[0xB3653E13][0].string, 'Jello!')
        self.assertEqual(db.token_to_entries[0xCC6D3131][0].string, 'Jello?')
        self.assertEqual(
            db.token_to_entries[0xE65AEFEF][0].string, "Won't fit : %s%d"
        )

        db.purge(datetime(2019, 6, 11))
        self.assertLess(len(db.token_to_entries), original_length)

        self.assertFalse(db.token_to_entries[0])
        self.assertEqual(db.token_to_entries[0x31631781][0].string, '%d')
        self.assertFalse(db.token_to_entries[0x2E668CD6])
        self.assertEqual(db.token_to_entries[0xB3653E13][0].string, 'Jello!')
        self.assertEqual(db.token_to_entries[0xCC6D3131][0].string, 'Jello?')
        self.assertFalse(db.token_to_entries[0xE65AEFEF])

    def test_merge(self) -> None:
        """Tests the tokens.Database merge method."""

        db = tokens.Database()

        # Test basic merging into an empty database.
        db.merge(
            tokens.Database(
                [
                    tokens.TokenizedStringEntry(
                        1, 'one', date_removed=datetime.min
                    ),
                    tokens.TokenizedStringEntry(
                        2, 'two', date_removed=datetime.min
                    ),
                ]
            )
        )
        self.assertEqual({str(e) for e in db.entries()}, {'one', 'two'})
        self.assertEqual(db.token_to_entries[1][0].date_removed, datetime.min)
        self.assertEqual(db.token_to_entries[2][0].date_removed, datetime.min)

        # Test merging in an entry with a removal date.
        db.merge(
            tokens.Database(
                [
                    tokens.TokenizedStringEntry(3, 'three'),
                    tokens.TokenizedStringEntry(
                        4, 'four', date_removed=datetime.min
                    ),
                ]
            )
        )
        self.assertEqual(
            {str(e) for e in db.entries()}, {'one', 'two', 'three', 'four'}
        )
        self.assertIsNone(db.token_to_entries[3][0].date_removed)
        self.assertEqual(db.token_to_entries[4][0].date_removed, datetime.min)

        # Test merging in one entry.
        db.merge(
            tokens.Database(
                [
                    tokens.TokenizedStringEntry(5, 'five'),
                ]
            )
        )
        self.assertEqual(
            {str(e) for e in db.entries()},
            {'one', 'two', 'three', 'four', 'five'},
        )
        self.assertEqual(db.token_to_entries[4][0].date_removed, datetime.min)
        self.assertIsNone(db.token_to_entries[5][0].date_removed)

        # Merge in repeated entries different removal dates.
        db.merge(
            tokens.Database(
                [
                    tokens.TokenizedStringEntry(
                        4, 'four', date_removed=datetime.max
                    ),
                    tokens.TokenizedStringEntry(
                        5, 'five', date_removed=datetime.max
                    ),
                ]
            )
        )
        self.assertEqual(len(db.entries()), 5)
        self.assertEqual(
            {str(e) for e in db.entries()},
            {'one', 'two', 'three', 'four', 'five'},
        )
        self.assertEqual(db.token_to_entries[4][0].date_removed, datetime.max)
        self.assertIsNone(db.token_to_entries[5][0].date_removed)

        # Merge in the same repeated entries now without removal dates.
        db.merge(
            tokens.Database(
                [
                    tokens.TokenizedStringEntry(4, 'four'),
                    tokens.TokenizedStringEntry(5, 'five'),
                ]
            )
        )
        self.assertEqual(len(db.entries()), 5)
        self.assertEqual(
            {str(e) for e in db.entries()},
            {'one', 'two', 'three', 'four', 'five'},
        )
        self.assertIsNone(db.token_to_entries[4][0].date_removed)
        self.assertIsNone(db.token_to_entries[5][0].date_removed)

        # Merge in an empty databsse.
        db.merge(tokens.Database([]))
        self.assertEqual(
            {str(e) for e in db.entries()},
            {'one', 'two', 'three', 'four', 'five'},
        )

    def test_merge_multiple_datbases_in_one_call(self) -> None:
        """Tests the merge and merged methods with multiple databases."""
        db = tokens.Database.merged(
            tokens.Database(
                [
                    tokens.TokenizedStringEntry(
                        1, 'one', date_removed=datetime.max
                    )
                ]
            ),
            tokens.Database(
                [
                    tokens.TokenizedStringEntry(
                        2, 'two', date_removed=datetime.min
                    )
                ]
            ),
            tokens.Database(
                [
                    tokens.TokenizedStringEntry(
                        1, 'one', date_removed=datetime.min
                    )
                ]
            ),
        )
        self.assertEqual({str(e) for e in db.entries()}, {'one', 'two'})

        db.merge(
            tokens.Database(
                [
                    tokens.TokenizedStringEntry(
                        4, 'four', date_removed=datetime.max
                    )
                ]
            ),
            tokens.Database(
                [
                    tokens.TokenizedStringEntry(
                        2, 'two', date_removed=datetime.max
                    )
                ]
            ),
            tokens.Database(
                [
                    tokens.TokenizedStringEntry(
                        3, 'three', date_removed=datetime.min
                    )
                ]
            ),
        )
        self.assertEqual(
            {str(e) for e in db.entries()}, {'one', 'two', 'three', 'four'}
        )

    def test_entry_counts(self) -> None:
        self.assertEqual(len(CSV_DATABASE.splitlines()), 16)

        db = read_db_from_csv(CSV_DATABASE)
        self.assertEqual(len(db.entries()), 16)
        self.assertEqual(len(db.token_to_entries), 16)

        # Add two strings with the same hash.
        db.add(_entries('o000', '0Q1Q'))

        self.assertEqual(len(db.entries()), 18)
        self.assertEqual(len(db.token_to_entries), 17)

    def test_mark_removed(self) -> None:
        """Tests that date_removed field is set by mark_removed."""
        db = tokens.Database.from_strings(
            ['MILK', 'apples', 'oranges', 'CHEESE', 'pears']
        )

        self.assertTrue(
            all(entry.date_removed is None for entry in db.entries())
        )
        date_1 = datetime(1, 2, 3)

        db.mark_removed(_entries('apples', 'oranges', 'pears'), date_1)

        self.assertEqual(
            db.token_to_entries[c_hash('MILK')][0].date_removed, date_1
        )
        self.assertEqual(
            db.token_to_entries[c_hash('CHEESE')][0].date_removed, date_1
        )

        now = datetime.now()
        db.mark_removed(_entries('MILK', 'CHEESE', 'pears'))

        # New strings are not added or re-added in mark_removed().
        self.assertGreaterEqual(
            db.token_to_entries[c_hash('MILK')][0].date_removed, date_1
        )
        self.assertGreaterEqual(
            db.token_to_entries[c_hash('CHEESE')][0].date_removed, date_1
        )

        # These strings were removed.
        self.assertGreaterEqual(
            db.token_to_entries[c_hash('apples')][0].date_removed, now
        )
        self.assertGreaterEqual(
            db.token_to_entries[c_hash('oranges')][0].date_removed, now
        )
        self.assertIsNone(db.token_to_entries[c_hash('pears')][0].date_removed)

    def test_add(self) -> None:
        db = tokens.Database()
        db.add(_entries('MILK', 'apples'))
        self.assertEqual({e.string for e in db.entries()}, {'MILK', 'apples'})

        db.add(_entries('oranges', 'CHEESE', 'pears'))
        self.assertEqual(len(db.entries()), 5)

        db.add(_entries('MILK', 'apples', 'only this one is new'))
        self.assertEqual(len(db.entries()), 6)

        db.add(_entries('MILK'))
        self.assertEqual(
            {e.string for e in db.entries()},
            {
                'MILK',
                'apples',
                'oranges',
                'CHEESE',
                'pears',
                'only this one is new',
            },
        )

    def test_add_duplicate_entries_keeps_none_as_removal_date(self) -> None:
        db = tokens.Database()
        db.add(
            [
                tokens.TokenizedStringEntry(1, 'Spam', '', datetime.now()),
                tokens.TokenizedStringEntry(1, 'Spam', ''),
                tokens.TokenizedStringEntry(1, 'Spam', '', datetime.min),
            ]
        )
        self.assertEqual(len(db), 1)
        self.assertIsNone(db.token_to_entries[1][0].date_removed)

    def test_add_duplicate_entries_keeps_newest_removal_date(self) -> None:
        db = tokens.Database()
        db.add(
            [
                tokens.TokenizedStringEntry(1, 'Spam', '', datetime.now()),
                tokens.TokenizedStringEntry(1, 'Spam', '', datetime.max),
                tokens.TokenizedStringEntry(1, 'Spam', '', datetime.now()),
                tokens.TokenizedStringEntry(1, 'Spam', '', datetime.min),
            ]
        )
        self.assertEqual(len(db), 1)
        self.assertEqual(db.token_to_entries[1][0].date_removed, datetime.max)

    def test_difference(self) -> None:
        first = tokens.Database(
            [
                tokens.TokenizedStringEntry(1, 'one'),
                tokens.TokenizedStringEntry(2, 'two'),
                tokens.TokenizedStringEntry(3, 'three'),
            ]
        )
        second = tokens.Database(
            [
                tokens.TokenizedStringEntry(1, 'one'),
                tokens.TokenizedStringEntry(3, 'three'),
                tokens.TokenizedStringEntry(4, 'four'),
            ]
        )
        difference = first.difference(second)
        self.assertEqual({e.string for e in difference.entries()}, {'two'})

    def test_binary_format_write(self) -> None:
        db = read_db_from_csv(CSV_DATABASE)

        with io.BytesIO() as fd:
            tokens.write_binary(db, fd)
            binary_db = fd.getvalue()

        self.assertEqual(BINARY_DATABASE, binary_db)

    def test_binary_format_parse(self) -> None:
        with io.BytesIO(BINARY_DATABASE) as binary_db:
            db = tokens.Database(tokens.parse_binary(binary_db))

        self.assertEqual(str(db), CSV_DATABASE)


class TestDatabaseFile(unittest.TestCase):
    """Tests the DatabaseFile class."""

    def setUp(self) -> None:
        file = tempfile.NamedTemporaryFile(delete=False)
        file.close()
        self._path = Path(file.name)

    def tearDown(self) -> None:
        self._path.unlink()

    def test_update_csv_file(self) -> None:
        self._path.write_text(CSV_DATABASE)
        db = tokens.DatabaseFile.load(self._path)
        self.assertEqual(str(db), CSV_DATABASE)

        db.add([tokens.TokenizedStringEntry(0xFFFFFFFF, 'New entry!')])

        db.write_to_file()

        self.assertEqual(
            self._path.read_text(),
            CSV_DATABASE + 'ffffffff,          ,"New entry!"\n',
        )

    def test_csv_file_too_short_raises_exception(self) -> None:
        self._path.write_text('1234')

        with self.assertRaises(tokens.DatabaseFormatError):
            tokens.DatabaseFile.load(self._path)

    def test_csv_invalid_format_raises_exception(self) -> None:
        self._path.write_text('MK34567890')

        with self.assertRaises(tokens.DatabaseFormatError):
            tokens.DatabaseFile.load(self._path)

    def test_csv_not_utf8(self) -> None:
        self._path.write_bytes(b'\x80' * 20)

        with self.assertRaises(tokens.DatabaseFormatError):
            tokens.DatabaseFile.load(self._path)


class TestFilter(unittest.TestCase):
    """Tests the filtering functionality."""

    def setUp(self) -> None:
        self.db = tokens.Database(
            [
                tokens.TokenizedStringEntry(1, 'Luke'),
                tokens.TokenizedStringEntry(2, 'Leia'),
                tokens.TokenizedStringEntry(2, 'Darth Vader'),
                tokens.TokenizedStringEntry(2, 'Emperor Palpatine'),
                tokens.TokenizedStringEntry(3, 'Han'),
                tokens.TokenizedStringEntry(4, 'Chewbacca'),
                tokens.TokenizedStringEntry(5, 'Darth Maul'),
                tokens.TokenizedStringEntry(6, 'Han Solo'),
            ]
        )

    def test_filter_include_single_regex(self) -> None:
        self.db.filter(include=[' '])  # anything with a space
        self.assertEqual(
            set(e.string for e in self.db.entries()),
            {'Darth Vader', 'Emperor Palpatine', 'Darth Maul', 'Han Solo'},
        )

    def test_filter_include_multiple_regexes(self) -> None:
        self.db.filter(include=['Darth', 'cc', '^Han$'])
        self.assertEqual(
            set(e.string for e in self.db.entries()),
            {'Darth Vader', 'Darth Maul', 'Han', 'Chewbacca'},
        )

    def test_filter_include_no_matches(self) -> None:
        self.db.filter(include=['Gandalf'])
        self.assertFalse(self.db.entries())

    def test_filter_exclude_single_regex(self) -> None:
        self.db.filter(exclude=['^[^L]'])
        self.assertEqual(
            set(e.string for e in self.db.entries()), {'Luke', 'Leia'}
        )

    def test_filter_exclude_multiple_regexes(self) -> None:
        self.db.filter(exclude=[' ', 'Han', 'Chewbacca'])
        self.assertEqual(
            set(e.string for e in self.db.entries()), {'Luke', 'Leia'}
        )

    def test_filter_exclude_no_matches(self) -> None:
        self.db.filter(exclude=['.*'])
        self.assertFalse(self.db.entries())

    def test_filter_include_and_exclude(self) -> None:
        self.db.filter(include=[' '], exclude=['Darth', 'Emperor'])
        self.assertEqual(set(e.string for e in self.db.entries()), {'Han Solo'})

    def test_filter_neither_include_nor_exclude(self) -> None:
        self.db.filter()
        self.assertEqual(
            set(e.string for e in self.db.entries()),
            {
                'Luke',
                'Leia',
                'Darth Vader',
                'Emperor Palpatine',
                'Han',
                'Chewbacca',
                'Darth Maul',
                'Han Solo',
            },
        )


class TestDirectoryDatabase(unittest.TestCase):
    """Test DirectoryDatabase class is properly loaded."""

    def setUp(self) -> None:
        self._dir = Path(tempfile.mkdtemp('_pw_tokenizer_test'))
        self._db_dir = self._dir / '_dir_database_test'
        self._db_dir.mkdir(exist_ok=True)
        self._db_csv = self._db_dir / f'first{DIR_DB_SUFFIX}'

    def tearDown(self) -> None:
        shutil.rmtree(self._dir)

    def test_loading_empty_directory(self) -> None:
        self.assertFalse(tokens.DatabaseFile.load(self._db_dir).entries())

    def test_loading_a_single_file(self) -> None:
        self._db_csv.write_text(CSV_DATABASE)
        csv = tokens.DatabaseFile.load(self._db_csv)
        directory_db = tokens.DatabaseFile.load(self._db_dir)
        self.assertEqual(1, len(list(self._db_dir.iterdir())))
        self.assertEqual(str(csv), str(directory_db))

    def test_loading_multiples_files(self) -> None:
        self._db_csv.write_text(CSV_DATABASE_3)
        first_csv = tokens.DatabaseFile.load(self._db_csv)

        path_to_second_csv = self._db_dir / f'second{DIR_DB_SUFFIX}'
        path_to_second_csv.write_text(CSV_DATABASE_2)
        second_csv = tokens.DatabaseFile.load(path_to_second_csv)

        path_to_third_csv = self._db_dir / f'third{DIR_DB_SUFFIX}'
        path_to_third_csv.write_text(CSV_DATABASE_4)
        third_csv = tokens.DatabaseFile.load(path_to_third_csv)

        all_databases_merged = tokens.Database.merged(
            first_csv, second_csv, third_csv
        )
        directory_db = tokens.DatabaseFile.load(self._db_dir)
        self.assertEqual(3, len(list(self._db_dir.iterdir())))
        self.assertEqual(str(all_databases_merged), str(directory_db))

    def test_loading_multiples_files_with_removal_dates(self) -> None:
        self._db_csv.write_text(CSV_DATABASE)
        first_csv = tokens.DatabaseFile.load(self._db_csv)

        path_to_second_csv = self._db_dir / f'second{DIR_DB_SUFFIX}'
        path_to_second_csv.write_text(CSV_DATABASE_2)
        second_csv = tokens.DatabaseFile.load(path_to_second_csv)

        path_to_third_csv = self._db_dir / f'third{DIR_DB_SUFFIX}'
        path_to_third_csv.write_text(CSV_DATABASE_3)
        third_csv = tokens.DatabaseFile.load(path_to_third_csv)

        all_databases_merged = tokens.Database.merged(
            first_csv, second_csv, third_csv
        )
        directory_db = tokens.DatabaseFile.load(self._db_dir)
        self.assertEqual(3, len(list(self._db_dir.iterdir())))
        self.assertEqual(str(all_databases_merged), str(directory_db))

    def test_rewrite(self) -> None:
        self._db_dir.joinpath('junk_file').write_text('should be ignored')

        self._db_csv.write_text(CSV_DATABASE_3)
        first_csv = tokens.DatabaseFile.load(self._db_csv)

        path_to_second_csv = self._db_dir / f'second{DIR_DB_SUFFIX}'
        path_to_second_csv.write_text(CSV_DATABASE_2)
        second_csv = tokens.DatabaseFile.load(path_to_second_csv)

        path_to_third_csv = self._db_dir / f'third{DIR_DB_SUFFIX}'
        path_to_third_csv.write_text(CSV_DATABASE_4)
        third_csv = tokens.DatabaseFile.load(path_to_third_csv)

        all_databases_merged = tokens.Database.merged(
            first_csv, second_csv, third_csv
        )

        directory_db = tokens.DatabaseFile.load(self._db_dir)
        directory_db.write_to_file(rewrite=True)

        self.assertEqual(1, len(list(self._db_dir.glob(f'*{DIR_DB_SUFFIX}'))))
        self.assertEqual(
            self._db_dir.joinpath('junk_file').read_text(), 'should be ignored'
        )

        directory_db = tokens.DatabaseFile.load(self._db_dir)
        self.assertEqual(str(all_databases_merged), str(directory_db))


if __name__ == '__main__':
    unittest.main()
