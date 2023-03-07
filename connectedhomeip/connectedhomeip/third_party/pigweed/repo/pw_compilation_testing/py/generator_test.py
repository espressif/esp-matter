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
"""Tests for the negative compilation test generator."""

from pathlib import Path
import re
import tempfile
import unittest

from pw_compilation_testing.generator import (
    Compiler,
    Expectation,
    ParseError,
    TestCase,
    enumerate_tests,
)

SOURCE = r'''
#if PW_NC_TEST(FirstTest)
PW_NC_EXPECT("abcdef");

SomeSourceCode();

#endif  // PW_NC_TEST

#if PW_NC_TEST(SecondTest)
PW_NC_EXPECT("\"\"abc123"    // Include " and other escapes in the string
             "def'456\""
             // Goodbye
             "ghi\n\t789"  // ???
); // abc

#endif  // PW_NC_TEST
'''

ILLEGAL_COMMENT = '''
#if PW_NC_TEST(FirstTest)
PW_NC_EXPECT("abcdef" /* illegal comment */);

#endif  // PW_NC_TEST
'''

UNTERMINATED_EXPECTATION = '#if PW_NC_TEST(FirstTest)\nPW_NC_EXPECT("abcdef"\n'


def _write_to_temp_file(contents: str) -> Path:
    file = tempfile.NamedTemporaryFile('w', delete=False)
    file.write(contents)
    file.close()
    return Path(file.name)


# pylint: disable=missing-function-docstring


class ParserTest(unittest.TestCase):
    """Tests parsing negative compilation tests from a file."""

    def test_successful(self) -> None:
        try:
            path = _write_to_temp_file(SOURCE)

            self.assertEqual(
                [
                    TestCase(
                        'TestSuite',
                        'FirstTest',
                        (Expectation(Compiler.ANY, re.compile('abcdef'), 3),),
                        path,
                        2,
                    ),
                    TestCase(
                        'TestSuite',
                        'SecondTest',
                        (
                            Expectation(
                                Compiler.ANY,
                                re.compile('""abc123def\'456"ghi\\n\\t789'),
                                10,
                            ),
                        ),
                        path,
                        9,
                    ),
                ],
                list(enumerate_tests('TestSuite', [path])),
            )
        finally:
            path.unlink()

    def test_illegal_comment(self) -> None:
        try:
            path = _write_to_temp_file(ILLEGAL_COMMENT)
            with self.assertRaises(ParseError):
                list(enumerate_tests('TestSuite', [path]))
        finally:
            path.unlink()

    def test_unterminated_expectation(self) -> None:
        try:
            path = _write_to_temp_file(UNTERMINATED_EXPECTATION)
            with self.assertRaises(ParseError) as err:
                list(enumerate_tests('TestSuite', [path]))
        finally:
            path.unlink()

        self.assertIn('Unterminated', str(err.exception))


if __name__ == '__main__':
    unittest.main()
