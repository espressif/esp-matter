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
"""Tests for the database module."""

import json
import io
import os
from pathlib import Path
import shutil
import stat
import subprocess
import sys
import tempfile
import unittest
from unittest import mock

from pw_tokenizer import database

# This is an ELF file with only the pw_tokenizer sections. It was created
# from a tokenize_test binary built for the STM32F429i Discovery board. The
# pw_tokenizer sections were extracted with this command:
#
#   arm-none-eabi-objcopy -S --only-section ".pw_tokenize*" <ELF> <OUTPUT>
#
TOKENIZED_ENTRIES_ELF = (
    Path(__file__).parent / 'example_binary_with_tokenized_strings.elf'
)

CSV_DEFAULT_DOMAIN = '''\
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

CSV_TEST_DOMAIN = """\
17fa86d3,          ,"hello"
18c5017c,          ,"yes"
59b2701c,          ,"The answer was: %s"
881436a0,          ,"The answer is: %s"
d18ada0f,          ,"something"
"""

CSV_ALL_DOMAINS = '''\
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

JSON_SOURCE_STRINGS = '''\
[
  "pigweed/pw_polyfill/standard_library_public/pw_polyfill/standard_library/assert.h",
  "protocol_buffer/gen/pigweed/pw_protobuf/common_protos.proto_library/nanopb/pw_protobuf_protos/status.pb.h",
  "pigweed/pw_rpc/client_server.cc",
  "pigweed/pw_rpc/public/pw_rpc/client_server.h",
  "This is a very long string that will produce two tokens; one for C++ and one for C. This is because this string exceeds the default C hash length."
]
'''

CSV_STRINGS = '''\
2cbf627a,          ,"pigweed/pw_rpc/client_server.cc"
666562a1,          ,"protocol_buffer/gen/pigweed/pw_protobuf/common_protos.proto_library/nanopb/pw_protobuf_protos/status.pb.h"
6c1e6eb3,          ,"pigweed/pw_rpc/public/pw_rpc/client_server.h"
b25a9932,          ,"This is a very long string that will produce two tokens; one for C++ and one for C. This is because this string exceeds the default C hash length."
eadf017f,          ,"pigweed/pw_polyfill/standard_library_public/pw_polyfill/standard_library/assert.h"
f815dc5c,          ,"This is a very long string that will produce two tokens; one for C++ and one for C. This is because this string exceeds the default C hash length."
'''

EXPECTED_REPORT = {
    str(TOKENIZED_ENTRIES_ELF): {
        '': {
            'present_entries': 22,
            'present_size_bytes': 289,
            'total_entries': 22,
            'total_size_bytes': 289,
            'collisions': {},
        },
        'TEST_DOMAIN': {
            'present_entries': 5,
            'present_size_bytes': 57,
            'total_entries': 5,
            'total_size_bytes': 57,
            'collisions': {},
        },
    }
}


def run_cli(*args) -> None:
    original_argv = sys.argv
    sys.argv = ['database.py', *(str(a) for a in args)]
    # pylint: disable=protected-access
    try:
        database._main(*database._parse_args())
    finally:
        # Remove the log handler added by _main to avoid duplicate logs.
        if database._LOG.handlers:
            database._LOG.handlers.pop()
        # pylint: enable=protected-access

        sys.argv = original_argv


def _mock_output() -> io.TextIOWrapper:
    output = io.BytesIO()
    output.name = '<fake stdout>'
    return io.TextIOWrapper(output, write_through=True)


def _remove_readonly(  # pylint: disable=unused-argument
    func, path, excinfo
) -> None:
    """Changes file permission and recalls the calling function."""
    print('Path attempted to be deleted:', path)
    if not os.access(path, os.W_OK):
        # Change file permissions.
        os.chmod(path, stat.S_IWUSR)
        # Call the calling function again.
        func(path)


class DatabaseCommandLineTest(unittest.TestCase):
    """Tests the database.py command line interface."""

    def setUp(self) -> None:
        self._dir = Path(tempfile.mkdtemp('_pw_tokenizer_test'))
        self._csv = self._dir / 'db.csv'
        self._elf = TOKENIZED_ENTRIES_ELF

        self._csv_test_domain = CSV_TEST_DOMAIN

    def tearDown(self) -> None:
        shutil.rmtree(self._dir)

    def test_create_csv(self) -> None:
        run_cli('create', '--database', self._csv, self._elf)

        self.assertEqual(
            CSV_DEFAULT_DOMAIN.splitlines(), self._csv.read_text().splitlines()
        )

    def test_create_csv_test_domain(self) -> None:
        run_cli('create', '--database', self._csv, f'{self._elf}#TEST_DOMAIN')

        self.assertEqual(
            self._csv_test_domain.splitlines(),
            self._csv.read_text().splitlines(),
        )

    def test_create_csv_all_domains(self) -> None:
        run_cli('create', '--database', self._csv, f'{self._elf}#.*')

        self.assertEqual(
            CSV_ALL_DOMAINS.splitlines(), self._csv.read_text().splitlines()
        )

    def test_create_force(self) -> None:
        self._csv.write_text(CSV_ALL_DOMAINS)

        with self.assertRaises(FileExistsError):
            run_cli('create', '--database', self._csv, self._elf)

        run_cli('create', '--force', '--database', self._csv, self._elf)

    def test_create_binary(self) -> None:
        binary = self._dir / 'db.bin'
        run_cli('create', '--type', 'binary', '--database', binary, self._elf)

        # Write the binary database as CSV to verify its contents.
        run_cli('create', '--database', self._csv, binary)

        self.assertEqual(
            CSV_DEFAULT_DOMAIN.splitlines(), self._csv.read_text().splitlines()
        )

    def test_add_does_not_recalculate_tokens(self) -> None:
        db_with_custom_token = '01234567,          ,"hello"'

        to_add = self._dir / 'add_this.csv'
        to_add.write_text(db_with_custom_token + '\n')
        self._csv.touch()

        run_cli('add', '--database', self._csv, to_add)
        self.assertEqual(
            db_with_custom_token.splitlines(),
            self._csv.read_text().splitlines(),
        )

    def test_mark_removed(self) -> None:
        self._csv.write_text(CSV_ALL_DOMAINS)

        run_cli(
            'mark_removed',
            '--database',
            self._csv,
            '--date',
            '1998-09-04',
            self._elf,
        )

        # Add the removal date to the four tokens not in the default domain
        new_csv = CSV_ALL_DOMAINS
        new_csv = new_csv.replace(
            '17fa86d3,          ,"hello"', '17fa86d3,1998-09-04,"hello"'
        )
        new_csv = new_csv.replace(
            '18c5017c,          ,"yes"', '18c5017c,1998-09-04,"yes"'
        )
        new_csv = new_csv.replace(
            '59b2701c,          ,"The answer was: %s"',
            '59b2701c,1998-09-04,"The answer was: %s"',
        )
        new_csv = new_csv.replace(
            'd18ada0f,          ,"something"', 'd18ada0f,1998-09-04,"something"'
        )
        self.assertNotEqual(CSV_ALL_DOMAINS, new_csv)

        self.assertEqual(
            new_csv.splitlines(), self._csv.read_text().splitlines()
        )

    def test_purge(self) -> None:
        self._csv.write_text(CSV_ALL_DOMAINS)

        # Mark everything not in TEST_DOMAIN as removed.
        run_cli(
            'mark_removed', '--database', self._csv, f'{self._elf}#TEST_DOMAIN'
        )

        # Delete all entries except those in TEST_DOMAIN.
        run_cli('purge', '--database', self._csv)

        self.assertEqual(
            self._csv_test_domain.splitlines(),
            self._csv.read_text().splitlines(),
        )

    @mock.patch('sys.stdout', new_callable=_mock_output)
    def test_report(self, mock_stdout) -> None:
        run_cli('report', self._elf)

        self.assertEqual(
            json.loads(mock_stdout.buffer.getvalue()), EXPECTED_REPORT
        )

    def test_replace(self) -> None:
        sub = 'replace/ment'
        run_cli(
            'create',
            '--database',
            self._csv,
            self._elf,
            '--replace',
            r'(?i)\b[jh]ello\b/' + sub,
        )
        self.assertEqual(
            CSV_DEFAULT_DOMAIN.replace('Jello', sub).replace('Hello', sub),
            self._csv.read_text(),
        )

    def test_json_strings(self) -> None:
        strings_file = self._dir / "strings.json"

        with open(strings_file, 'w') as file:
            file.write(JSON_SOURCE_STRINGS)

        run_cli('create', '--force', '--database', self._csv, strings_file)
        self.assertEqual(
            CSV_STRINGS.splitlines(), self._csv.read_text().splitlines()
        )


class TestDirectoryDatabaseCommandLine(unittest.TestCase):
    """Tests the directory database command line interface."""

    def setUp(self) -> None:
        self._dir = Path(tempfile.mkdtemp('_pw_tokenizer_test'))
        self._db_dir = self._dir / '_dir_database_test'
        self._db_dir.mkdir(exist_ok=True)
        self._db_csv = self._db_dir / '8123913.pw_tokenizer.csv'
        self._elf = TOKENIZED_ENTRIES_ELF
        self._csv_test_domain = CSV_TEST_DOMAIN

    def _git(self, *command: str) -> None:
        """Runs git in self._dir with forced user name and email values.

        Prevents accidentally running git in the wrong directory and avoids
        errors if the name and email are not configured.
        """
        subprocess.run(
            [
                'git',
                '-c',
                'user.name=pw_tokenizer tests',
                '-c',
                'user.email=noreply@google.com',
                *command,
            ],
            cwd=self._dir,
            check=True,
        )

    def tearDown(self) -> None:
        shutil.rmtree(self._dir, onerror=_remove_readonly)

    def test_add_csv_to_dir(self) -> None:
        """Tests a CSV can be created within the database."""
        run_cli('add', '--database', self._db_dir, f'{self._elf}#TEST_DOMAIN')
        directory = list(self._db_dir.iterdir())

        self.assertEqual(1, len(directory))

        self._db_csv = directory.pop()

        self.assertEqual(
            self._csv_test_domain.splitlines(),
            self._db_csv.read_text().splitlines(),
        )

    def test_add_all_domains_to_dir(self) -> None:
        """Tests a CSV with all domains can be added to the database."""
        run_cli('add', '--database', self._db_dir, f'{self._elf}#.*')
        directory = list(self._db_dir.iterdir())

        self.assertEqual(1, len(directory))

        self._db_csv = directory.pop()

        self.assertEqual(
            CSV_ALL_DOMAINS.splitlines(), self._db_csv.read_text().splitlines()
        )

    def test_not_adding_existing_tokens(self) -> None:
        """Tests duplicate tokens are not added to the database."""
        run_cli('add', '--database', self._db_dir, f'{self._elf}#TEST_DOMAIN')
        run_cli('add', '--database', self._db_dir, f'{self._elf}#TEST_DOMAIN')
        directory = list(self._db_dir.iterdir())

        self.assertEqual(1, len(directory))

        self._db_csv = directory.pop()

        self.assertEqual(
            self._csv_test_domain.splitlines(),
            self._db_csv.read_text().splitlines(),
        )

    def test_adding_tokens_without_git_repo(self):
        """Tests creating new files with new entries when no repo exists."""
        # Add CSV_TEST_DOMAIN to a new CSV in the directory database.
        run_cli('add', '--database', self._db_dir, f'{self._elf}#TEST_DOMAIN')
        directory = list(self._db_dir.iterdir())

        self.assertEqual(1, len(directory))

        first_csv_in_db = directory.pop()

        self.assertEqual(
            self._csv_test_domain.splitlines(),
            first_csv_in_db.read_text().splitlines(),
        )
        # Add CSV_ALL_DOMAINS to a new CSV in the directory database.
        run_cli('add', '--database', self._db_dir, f'{self._elf}#.*')
        directory = list(self._db_dir.iterdir())
        # Assert two different CSVs were created to store new tokens.
        self.assertEqual(2, len(directory))
        # Retrieve the other CSV in the directory.
        second_csv_in_db = (
            directory[0] if directory[0] != first_csv_in_db else directory[1]
        )

        self.assertNotEqual(first_csv_in_db, second_csv_in_db)
        self.assertEqual(
            self._csv_test_domain.splitlines(),
            first_csv_in_db.read_text().splitlines(),
        )

        # Retrieve entries that exclusively exist in CSV_ALL_DOMAINS
        # as CSV_ALL_DOMAINS contains all entries in TEST_DOMAIN.
        entries_exclusively_in_all_domain = set(
            CSV_ALL_DOMAINS.splitlines()
        ) - set(self._csv_test_domain.splitlines())
        # Ensure only new tokens not in CSV_TEST_DOMAIN were added to
        # the second CSV added to the directory database.
        self.assertEqual(
            entries_exclusively_in_all_domain,
            set(second_csv_in_db.read_text().splitlines()),
        )

    def test_untracked_files_in_dir(self):
        """Tests untracked CSVs are reused by the database."""
        self._git('init')
        # Add CSV_TEST_DOMAIN to a new CSV in the directory database.
        run_cli(
            'add',
            '--database',
            self._db_dir,
            '--discard-temporary',
            'HEAD',
            f'{self._elf}#TEST_DOMAIN',
        )
        directory = list(self._db_dir.iterdir())

        self.assertEqual(1, len(directory))

        first_path_in_db = directory.pop()

        self.assertEqual(
            self._csv_test_domain.splitlines(),
            first_path_in_db.read_text().splitlines(),
        )
        # Retrieve the untracked CSV in the Git repository and discard
        # tokens that do not exist in CSV_DEFAULT_DOMAIN.
        run_cli(
            'add',
            '--database',
            self._db_dir,
            '--discard-temporary',
            'HEAD',
            self._elf,
        )
        directory = list(self._db_dir.iterdir())

        self.assertEqual(1, len(directory))

        reused_path_in_db = directory.pop()
        # Ensure the first path created is the same being reused. Also,
        # the CSV content is the same as CSV_DEFAULT_DOMAIN.
        self.assertEqual(first_path_in_db, reused_path_in_db)
        self.assertEqual(
            CSV_DEFAULT_DOMAIN.splitlines(),
            reused_path_in_db.read_text().splitlines(),
        )

    def test_adding_multiple_elf_files(self) -> None:
        """Tests adding multiple elf files to a file in the database."""
        # Add CSV_TEST_DOMAIN to a new CSV in the directory database.
        run_cli(
            'add',
            '--database',
            self._db_dir,
            f'{self._elf}#TEST_DOMAIN',
            self._elf,
        )
        directory = list(self._db_dir.iterdir())

        self.assertEqual(1, len(directory))
        # Combines CSV_DEFAULT_DOMAIN and TEST_DOMAIN into a unique set
        # of token entries.
        entries_from_default_and_test_domain = set(
            CSV_DEFAULT_DOMAIN.splitlines()
        ).union(set(self._csv_test_domain.splitlines()))
        # Multiple ELF files were added at once to a single CSV.
        self.assertEqual(
            entries_from_default_and_test_domain,
            set(directory.pop().read_text().splitlines()),
        )

    def test_discarding_old_entries(self) -> None:
        """Tests discarding old entries for new entries when re-adding."""
        self._git('init')
        # Add CSV_ALL_DOMAINS to a new CSV in the directory database.
        run_cli(
            'add',
            '--database',
            self._db_dir,
            '--discard-temporary',
            'HEAD',
            f'{self._elf}#.*',
        )
        directory = list(self._db_dir.iterdir())

        self.assertEqual(1, len(directory))

        untracked_path_in_db = directory.pop()

        self.assertEqual(
            CSV_ALL_DOMAINS.splitlines(),
            untracked_path_in_db.read_text().splitlines(),
        )
        # Add CSV_DEFAULT_DOMAIN and CSV_TEST_DOMAIN to a CSV in the
        # directory database, while replacing entries in CSV_ALL_DOMAINS
        # that no longer exist.
        run_cli(
            'add',
            '--database',
            self._db_dir,
            '--discard-temporary',
            'HEAD',
            f'{self._elf}#TEST_DOMAIN',
            self._elf,
        )
        directory = list(self._db_dir.iterdir())

        self.assertEqual(1, len(directory))

        reused_path_in_db = directory.pop()
        # Combines CSV_DEFAULT_DOMAIN and TEST_DOMAIN.
        entries_from_default_and_test_domain = set(
            CSV_DEFAULT_DOMAIN.splitlines()
        ).union(set(self._csv_test_domain.splitlines()))

        self.assertEqual(untracked_path_in_db, reused_path_in_db)
        self.assertEqual(
            entries_from_default_and_test_domain,
            set(reused_path_in_db.read_text().splitlines()),
        )

    def test_retrieving_csv_from_commit(self) -> None:
        """Tests retrieving a CSV from a commit and removing temp tokens."""
        self._git('init')
        self._git('commit', '--allow-empty', '-m', 'First Commit')
        # Add CSV_ALL_DOMAINS to a new CSV in the directory database.
        run_cli('add', '--database', self._db_dir, f'{self._elf}#.*')
        directory = list(self._db_dir.iterdir())

        self.assertEqual(1, len(directory))

        tracked_path_in_db = directory.pop()

        self.assertEqual(
            CSV_ALL_DOMAINS.splitlines(),
            tracked_path_in_db.read_text().splitlines(),
        )
        # Commit the CSV to avoid retrieving the CSV with the checks
        # for untracked changes.
        self._git('add', '--all')
        self._git('commit', '-m', 'Adding a CSV to a new commit.')
        # Retrieve the CSV in HEAD~ and discard tokens that exist in
        # CSV_ALL_DOMAINS and not exist in CSV_TEST_DOMAIN.
        run_cli(
            'add',
            '--database',
            self._db_dir,
            '--discard-temporary',
            'HEAD~2',
            f'{self._elf}#TEST_DOMAIN',
        )
        directory = list(self._db_dir.iterdir())

        self.assertEqual(1, len(directory))

        reused_path_in_db = directory.pop()

        self.assertEqual(
            self._csv_test_domain.splitlines(),
            reused_path_in_db.read_text().splitlines(),
        )


if __name__ == '__main__':
    unittest.main()
