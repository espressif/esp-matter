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
"""Tests for pw_console.text_formatting"""

import unittest
from parameterized import parameterized  # type: ignore

from prompt_toolkit.formatted_text import ANSI

from pw_console.text_formatting import (
    get_line_height,
    insert_linebreaks,
    split_lines,
)


class TestTextFormatting(unittest.TestCase):
    """Tests for manipulating prompt_toolkit formatted text tuples."""

    def setUp(self):
        self.maxDiff = None  # pylint: disable=invalid-name

    @parameterized.expand(
        [
            (
                'with short prefix height 2',
                len('LINE that should be wrapped'),  # text_width
                len('|                |'),  # screen_width
                len('--->'),  # prefix_width
                ('LINE that should b\n' '--->e wrapped     \n').count(
                    '\n'
                ),  # expected_height
                len('_____'),  # expected_trailing_characters
            ),
            (
                'with short prefix height 3',
                len('LINE that should be wrapped three times.'),  # text_width
                len('|                |'),  # screen_width
                len('--->'),  # prefix_width
                (
                    'LINE that should b\n'
                    '--->e wrapped thre\n'
                    '--->e times.      \n'
                ).count(
                    '\n'
                ),  # expected_height
                len('______'),  # expected_trailing_characters
            ),
            (
                'with short prefix height 4',
                len('LINE that should be wrapped even more times, say four.'),
                len('|                |'),  # screen_width
                len('--->'),  # prefix_width
                (
                    'LINE that should b\n'
                    '--->e wrapped even\n'
                    '---> more times, s\n'
                    '--->ay four.      \n'
                ).count(
                    '\n'
                ),  # expected_height
                len('______'),  # expected_trailing_characters
            ),
            (
                'no wrapping needed',
                len('LINE wrapped'),  # text_width
                len('|                |'),  # screen_width
                len('--->'),  # prefix_width
                ('LINE wrapped      \n').count('\n'),  # expected_height
                len('______'),  # expected_trailing_characters
            ),
            (
                'prefix is > screen width',
                len('LINE that should be wrapped'),  # text_width
                len('|                |'),  # screen_width
                len('------------------>'),  # prefix_width
                ('LINE that should b\n' 'e wrapped         \n').count(
                    '\n'
                ),  # expected_height
                len('_________'),  # expected_trailing_characters
            ),
            (
                'prefix is == screen width',
                len('LINE that should be wrapped'),  # text_width
                len('|                |'),  # screen_width
                len('----------------->'),  # prefix_width
                ('LINE that should b\n' 'e wrapped         \n').count(
                    '\n'
                ),  # expected_height
                len('_________'),  # expected_trailing_characters
            ),
        ]
    )
    def test_get_line_height(
        self,
        _name,
        text_width,
        screen_width,
        prefix_width,
        expected_height,
        expected_trailing_characters,
    ) -> None:
        """Test line height calculations."""
        height, remaining_width = get_line_height(
            text_width, screen_width, prefix_width
        )
        self.assertEqual(height, expected_height)
        self.assertEqual(remaining_width, expected_trailing_characters)

    # pylint: disable=line-too-long
    @parameterized.expand(
        [
            (
                'One line with ANSI escapes and no included breaks',
                12,  # screen_width
                False,  # truncate_long_lines
                'Lorem ipsum \x1b[34m\x1b[1mdolor sit amet\x1b[0m, consectetur adipiscing elit.',  # message
                ANSI(
                    # Line 1
                    'Lorem ipsum \n'
                    # Line 2
                    '\x1b[34m\x1b[1m'  # zero width
                    'dolor sit am\n'
                    # Line 3
                    'et'
                    '\x1b[0m'  # zero width
                    ', consecte\n'
                    # Line 4
                    'tur adipisci\n'
                    # Line 5
                    'ng elit.\n'
                ).__pt_formatted_text__(),
                5,  # expected_height
            ),
            (
                'One line with ANSI escapes and included breaks',
                12,  # screen_width
                False,  # truncate_long_lines
                'Lorem\n ipsum \x1b[34m\x1b[1mdolor sit amet\x1b[0m, consectetur adipiscing elit.',  # message
                ANSI(
                    # Line 1
                    'Lorem\n'
                    # Line 2
                    ' ipsum \x1b[34m\x1b[1mdolor\n'
                    # Line 3
                    ' sit amet\x1b[0m, c\n'
                    # Line 4
                    'onsectetur a\n'
                    # Line 5
                    'dipiscing el\n'
                    # Line 6
                    'it.\n'
                ).__pt_formatted_text__(),
                6,  # expected_height
            ),
            (
                'One line with ANSI escapes and included breaks; truncate lines enabled',
                12,  # screen_width
                True,  # truncate_long_lines
                'Lorem\n ipsum dolor sit amet, consectetur adipiscing \nelit.\n',  # message
                ANSI(
                    # Line 1
                    'Lorem\n'
                    # Line 2
                    ' ipsum dolor\n'
                    # Line 3
                    'elit.\n'
                ).__pt_formatted_text__(),
                3,  # expected_height
            ),
            (
                'wrapping enabled with a line break just after screen_width',
                10,  # screen_width
                False,  # truncate_long_lines
                '01234567890\nTest Log\n',  # message
                ANSI('0123456789\n' '0\n' 'Test Log\n').__pt_formatted_text__(),
                3,  # expected_height
            ),
            (
                'log message with a line break at screen_width',
                10,  # screen_width
                True,  # truncate_long_lines
                '0123456789\nTest Log\n',  # message
                ANSI('0123456789\n' 'Test Log\n').__pt_formatted_text__(),
                2,  # expected_height
            ),
        ]
    )
    # pylint: enable=line-too-long
    def test_insert_linebreaks(
        self,
        _name,
        screen_width,
        truncate_long_lines,
        raw_text,
        expected_fragments,
        expected_height,
    ) -> None:
        """Test inserting linebreaks to wrap lines."""

        formatted_text = ANSI(raw_text).__pt_formatted_text__()

        fragments, line_height = insert_linebreaks(
            formatted_text,
            max_line_width=screen_width,
            truncate_long_lines=truncate_long_lines,
        )

        self.assertEqual(fragments, expected_fragments)
        self.assertEqual(line_height, expected_height)

    @parameterized.expand(
        [
            (
                'flattened split',
                ANSI(
                    'Lorem\n' ' ipsum dolor\n' 'elit.\n'
                ).__pt_formatted_text__(),
                [
                    ANSI('Lorem').__pt_formatted_text__(),
                    ANSI(' ipsum dolor').__pt_formatted_text__(),
                    ANSI('elit.').__pt_formatted_text__(),
                ],  # expected_lines
            ),
            (
                'split fragments from insert_linebreaks',
                insert_linebreaks(
                    ANSI(
                        'Lorem\n ipsum dolor sit amet, consectetur adipiscing elit.'
                    ).__pt_formatted_text__(),
                    max_line_width=15,
                    # [0] for the fragments, [1] is line_height
                    truncate_long_lines=False,
                )[0],
                [
                    ANSI('Lorem').__pt_formatted_text__(),
                    ANSI(' ipsum dolor si').__pt_formatted_text__(),
                    ANSI('t amet, consect').__pt_formatted_text__(),
                    ANSI('etur adipiscing').__pt_formatted_text__(),
                    ANSI(' elit.').__pt_formatted_text__(),
                ],
            ),
            (
                'empty lines',
                # Each line should have at least one StyleAndTextTuple but without
                # an ending line break.
                [
                    ('', '\n'),
                    ('', '\n'),
                ],
                [
                    [('', '')],
                    [('', '')],
                ],
            ),
        ]
    )
    def test_split_lines(
        self,
        _name,
        input_fragments,
        expected_lines,
    ) -> None:
        """Test splitting flattened StyleAndTextTuples into a list of lines."""

        result_lines = split_lines(input_fragments)

        self.assertEqual(result_lines, expected_lines)


if __name__ == '__main__':
    unittest.main()
