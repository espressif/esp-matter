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
"""Text formatting functions."""

import copy
import re
from typing import Iterable, List, Tuple

from prompt_toolkit.formatted_text import StyleAndTextTuples
from prompt_toolkit.formatted_text.base import OneStyleAndTextTuple
from prompt_toolkit.utils import get_cwidth

_ANSI_SEQUENCE_REGEX = re.compile(r'\x1b[^m]*m')


def strip_ansi(text: str):
    """Strip out ANSI escape sequences."""
    return _ANSI_SEQUENCE_REGEX.sub('', text)


def split_lines(
    input_fragments: StyleAndTextTuples,
) -> List[StyleAndTextTuples]:
    """Break a flattened list of StyleAndTextTuples into a list of lines.

    Ending line breaks are not preserved."""
    lines: List[StyleAndTextTuples] = []
    this_line: StyleAndTextTuples = []
    for item in input_fragments:
        if item[1].endswith('\n'):
            # If there are no elements in this line except for a linebreak add
            # an empty StyleAndTextTuple so this line isn't an empty list.
            if len(this_line) == 0 and item[1] == '\n':
                this_line.append((item[0], item[1][:-1]))
            lines.append(this_line)
            this_line = []
        else:
            this_line.append(item)
    return lines


def insert_linebreaks(
    input_fragments: StyleAndTextTuples,
    max_line_width: int,
    truncate_long_lines: bool = True,
) -> Tuple[StyleAndTextTuples, int]:
    """Add line breaks at max_line_width if truncate_long_lines is True.

    Returns input_fragments with each character as it's own formatted text
    tuple."""
    fragments: StyleAndTextTuples = []
    total_width = 0
    line_width = 0
    line_height = 0
    new_break_inserted = False

    for item in input_fragments:
        # Check for non-printable fragment; doesn't affect the width.
        if '[ZeroWidthEscape]' in item[0]:
            fragments.append(item)
            continue

        new_item_style = item[0]

        # For each character in the fragment
        for character in item[1]:
            # Get the width respecting double width characters
            width = get_cwidth(character)
            # Increment counters
            total_width += width
            line_width += width
            # Save this character as it's own fragment
            if line_width <= max_line_width:
                if not new_break_inserted or character != '\n':
                    fragments.append((new_item_style, character))
                    # Was a line break just inserted?
                    if character == '\n':
                        # Increase height
                        line_height += 1
                new_break_inserted = False

            # Reset width to zero even if we are beyond the max line width.
            if character == '\n':
                line_width = 0

            # Are we at the limit for this line?
            elif line_width == max_line_width:
                # Insert a new linebreak fragment
                fragments.append((new_item_style, '\n'))
                # Increase height
                line_height += 1
                # Set a flag for skipping the next character if it is also a
                # line break.
                new_break_inserted = True

                if not truncate_long_lines:
                    # Reset line width to zero
                    line_width = 0

    # Check if the string ends in a final line break
    last_fragment_style = fragments[-1][0]
    last_fragment_text = fragments[-1][1]
    if not last_fragment_text.endswith('\n'):
        # Add a line break if none exists
        fragments.append((last_fragment_style, '\n'))
        line_height += 1

    return fragments, line_height


def join_adjacent_style_tuples(
    fragments: StyleAndTextTuples,
) -> StyleAndTextTuples:
    """Join adjacent FormattedTextTuples if they have the same style."""
    new_fragments: StyleAndTextTuples = []

    for i, fragment in enumerate(fragments):
        # Add the first fragment
        if i == 0:
            new_fragments.append(fragment)
            continue

        # Get this style
        style = fragment[0]
        # If the previous style matches
        if style == new_fragments[-1][0]:
            # Get the previous text
            new_text = new_fragments[-1][1]
            # Append this text
            new_text += fragment[1]
            # Replace the last fragment
            new_fragments[-1] = (style, new_text)
        else:
            # Styles don't match, just append.
            new_fragments.append(fragment)

    return new_fragments


def fill_character_width(
    input_fragments: StyleAndTextTuples,
    fragment_width: int,
    window_width: int,
    line_wrapping: bool = False,
    remaining_width: int = 0,
    horizontal_scroll_amount: int = 0,
    add_cursor: bool = False,
) -> StyleAndTextTuples:
    """Fill line to the width of the window using spaces."""
    # Calculate the number of spaces to add at the end.
    empty_characters = window_width - fragment_width
    # If wrapping is on, use remaining_width
    if line_wrapping and (fragment_width > window_width):
        empty_characters = remaining_width

    # Add additional spaces for horizontal scrolling.
    empty_characters += horizontal_scroll_amount

    if empty_characters <= 0:
        # No additional spaces required
        return input_fragments

    line_fragments = copy.copy(input_fragments)

    single_space = ('', ' ')
    line_ends_in_a_break = False
    # Replace the trailing \n with a space
    if line_fragments[-1][1] == '\n':
        line_fragments[-1] = single_space
        empty_characters -= 1
        line_ends_in_a_break = True

    # Append remaining spaces
    for _i in range(empty_characters):
        line_fragments.append(single_space)

    if line_ends_in_a_break:
        # Restore the \n
        line_fragments.append(('', '\n'))

    if add_cursor:
        # Add a cursor to this line by adding SetCursorPosition fragment.
        line_fragments_remainder = line_fragments
        line_fragments = [('[SetCursorPosition]', '')]
        # Use extend to keep types happy.
        line_fragments.extend(line_fragments_remainder)

    return line_fragments


def flatten_formatted_text_tuples(
    lines: Iterable[StyleAndTextTuples],
) -> StyleAndTextTuples:
    """Flatten a list of lines of FormattedTextTuples

    This function will also remove trailing newlines to avoid displaying extra
    empty lines in prompt_toolkit containers.
    """
    fragments: StyleAndTextTuples = []

    # Return empty list if lines is empty.
    if not lines:
        return fragments

    for line_fragments in lines:
        # Append all FormattedText tuples for this line.
        for fragment in line_fragments:
            fragments.append(fragment)

    # Strip off any trailing line breaks
    last_fragment: OneStyleAndTextTuple = fragments[-1]
    style = last_fragment[0]
    text = last_fragment[1].rstrip('\n')
    fragments[-1] = (style, text)
    return fragments


def remove_formatting(formatted_text: StyleAndTextTuples) -> str:
    """Throw away style info from prompt_toolkit formatted text tuples."""
    return ''.join([formatted_tuple[1] for formatted_tuple in formatted_text])


def get_line_height(text_width, screen_width, prefix_width):
    """Calculates line height for a string with line wrapping enabled."""
    if text_width == 0:
        return 0

    # If text will fit on the screen without wrapping.
    if text_width <= screen_width:
        return 1, screen_width - text_width

    # Assume zero width prefix if it's >= width of the screen.
    if prefix_width >= screen_width:
        prefix_width = 0

    # Start with height of 1 row.
    total_height = 1

    # One screen_width of characters (with no prefix) is displayed first.
    remaining_width = text_width - screen_width

    # While we have caracters remaining to be displayed
    while remaining_width > 0:
        # Add the new indentation prefix
        remaining_width += prefix_width
        # Display this line
        remaining_width -= screen_width
        # Add a line break
        total_height += 1

    # Remaining characters is what's left below zero.
    return (total_height, abs(remaining_width))
