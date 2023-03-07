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
"""LogScreen tracks lines to display on screen with a set of ScreenLines."""

from __future__ import annotations
import collections
import dataclasses
import logging
from typing import Callable, List, Optional, Tuple, TYPE_CHECKING

from prompt_toolkit.formatted_text import (
    to_formatted_text,
    StyleAndTextTuples,
)

from pw_console.log_filter import LogFilter
from pw_console.text_formatting import (
    fill_character_width,
    insert_linebreaks,
    split_lines,
)

if TYPE_CHECKING:
    from pw_console.log_line import LogLine
    from pw_console.log_pane import LogPane

_LOG = logging.getLogger(__package__)


@dataclasses.dataclass
class ScreenLine:
    """A single line of text for displaying on screen.

    Instances of ScreenLine are stored in a LogScreen's line_buffer deque. When
    a new log message is added it may be converted into multiple ScreenLine
    instances if the text is wrapped across multiple lines.

    For example: say our screen is 80 characters wide and a log message 240
    characters long needs to be displayed. With line wrapping on we will need
    240/80 = 3 lines to show the full message. Say also that this single log
    message is at index #5 in the LogStore classes deque, this is the log_index
    value. This single log message will then be split into 3 separate
    ScreenLine instances:

    ::
        ScreenLine(fragments=[('', 'Log message text line one')],
                   log_index=5, subline=0, height=3)
        ScreenLine(fragments=[('', 'Log message text line two')],
                   log_index=5, subline=1, height=3)
        ScreenLine(fragments=[('', 'Log message text line three')],
                   log_index=5, subline=2, height=3)

    Each `fragments` attribute will store the formatted text indended to be
    direcly drawn to the screen. Since these three lines are all displaying the
    same log message their `log_index` reference will be the same. The `subline`
    attribute is the zero-indexed number for this log's wrapped line count and
    `height` is the total ScreenLines needed to show this log message.

    Continuing with this example say the next two log messages to display both
    fit on screen with no wrapping. They will both be represented with one
    ScreenLine each:

    ::
        ScreenLine(fragments=[('', 'Another log message')],
                   log_index=6, subline=0, height=1)
        ScreenLine(fragments=[('', 'Yet another log message')],
                   log_index=7, subline=0, height=1)

    The `log_index` is different for each since these are both separate
    logs. The subline is 0 since each line is the first one for this log. Both
    have a height of 1 since no line wrapping was performed.
    """

    # The StyleAndTextTuples for this line ending with a '\n'. These are the raw
    # prompt_toolkit formatted text tuples to display on screen. The colors and
    # spacing can change depending on the formatters used in the
    # LogScreen._get_fragments_per_line() function.
    fragments: StyleAndTextTuples

    # Log index reference for this screen line. This is the index to where the
    # log message resides in the parent LogStore.logs deque. It is set to None
    # if this is an empty ScreenLine. If a log message requires line wrapping
    # then each resulting ScreenLine instance will have the same log_index
    # value.
    #
    # This log_index may also be the integer index into a LogView.filtered_logs
    # deque depending on if log messages are being filtered by the user. The
    # LogScreen class below doesn't need to do anything different in either
    # case. It's the responsibility of LogScreen.get_log_source() to return the
    # correct source.
    #
    # Note this is NOT an index into LogScreen.line_buffer.
    log_index: Optional[int] = None

    # Keep track the total height and subline number for this log message.
    # For example this line could be subline (0, 1, or 2) of a log message with
    # a total height 3.

    # Subline index.
    subline: int = 0
    # Total height in lines of text (also ScreenLine count) that the log message
    # referred to by log_index requires. When a log message is split across
    # multiple lines height will be set to the same value for each ScreenLine
    # instance.
    height: int = 1

    # Empty lines will have no log_index
    def empty(self) -> bool:
        return self.log_index is None


@dataclasses.dataclass
class LogScreen:
    """LogScreen maintains the state of visible logs on screen.

    It is responsible for moving the cursor_position, prepending and appending
    log lines as the user moves the cursor."""

    # Callable functions to retrieve logs and display formatting.
    get_log_source: Callable[[], Tuple[int, collections.deque[LogLine]]]
    get_line_wrapping: Callable[[], bool]
    get_log_formatter: Callable[
        [], Optional[Callable[[LogLine], StyleAndTextTuples]]
    ]
    get_search_filter: Callable[[], Optional[LogFilter]]
    get_search_highlight: Callable[[], bool]

    # Window row of the current cursor position
    cursor_position: int = 0
    # Screen width and height in number of characters.
    width: int = 0
    height: int = 0
    # Buffer of literal text lines to be displayed on screen. Each visual line
    # is represented by a ScreenLine instance and will have a max width equal
    # to the screen's width. If any given whole log message requires line
    # wrapping to be displayed it will be represented by multiple ScreenLine
    # instances in this deque.
    line_buffer: collections.deque[ScreenLine] = dataclasses.field(
        default_factory=collections.deque
    )

    def __post_init__(self) -> None:
        # Empty screen flag. Will be true if the screen contains only newlines.
        self._empty: bool = True
        # Save the last log index when appending. Useful for tracking how many
        # new lines need appending in follow mode.
        self.last_appended_log_index: int = 0

    def _fill_top_with_empty_lines(self) -> None:
        """Add empty lines to fill the remaining empty screen space."""
        for _ in range(self.height - len(self.line_buffer)):
            self.line_buffer.appendleft(ScreenLine([('', '')]))

    def clear_screen(self) -> None:
        """Erase all lines and fill with empty lines."""
        self.line_buffer.clear()
        self._fill_top_with_empty_lines()
        self._empty = True

    def empty(self) -> bool:
        """Return True if the screen has no lines with content."""
        return self._empty

    def reset_logs(
        self,
        log_index: int = 0,
    ) -> None:
        """Erase the screen and append logs starting from log_index."""
        self.clear_screen()

        start_log_index, log_source = self.get_log_source()
        if len(log_source) == 0:
            return

        # Append at most at most the window height number worth of logs. If the
        # number of available logs is less, use that amount.
        max_log_messages_to_fetch = min(self.height, len(log_source))

        # Including the target log_index, fetch the desired logs.
        # For example if we are rendering log_index 10 and the window height is
        # 6 the range below will be:
        # >>> list(i for i in range((10 - 6) + 1, 10 + 1))
        # [5, 6, 7, 8, 9, 10]
        for i in range(
            (log_index - max_log_messages_to_fetch) + 1, log_index + 1
        ):
            # If i is < 0 it's an invalid log, skip to the next line. The next
            # index could be 0 or higher since we are traversing in increasing
            # order.
            if i < start_log_index:
                continue
            self.append_log(i)
        # Make sure the bottom line is highlighted.
        self.move_cursor_to_bottom()

    def resize(self, width, height) -> None:
        """Update screen width and height.

        Following a resize the caller should run reset_logs()."""
        self.width = width
        self.height = height

    def get_lines(
        self,
        marked_logs_start: Optional[int] = None,
        marked_logs_end: Optional[int] = None,
    ) -> List[StyleAndTextTuples]:
        """Return lines for final display.

        Styling is added for the line under the cursor."""
        if not marked_logs_start:
            marked_logs_start = -1
        if not marked_logs_end:
            marked_logs_end = -1

        all_lines: List[StyleAndTextTuples] = []
        # Loop through a copy of the line_buffer in case it is mutated before
        # this function is complete.
        for i, line in enumerate(list(self.line_buffer)):

            # Is this line the cursor_position? Apply line highlighting
            if (
                i == self.cursor_position
                and (self.cursor_position < len(self.line_buffer))
                and not self.line_buffer[self.cursor_position].empty()
            ):
                # Fill in empty charaters to the width of the screen. This
                # ensures the backgound is highlighted to the edge of the
                # screen.
                new_fragments = fill_character_width(
                    line.fragments,
                    len(line.fragments) - 1,  # -1 for the ending line break
                    self.width,
                )

                # Apply a style to highlight this line.
                all_lines.append(
                    to_formatted_text(
                        new_fragments, style='class:selected-log-line'
                    )
                )
            elif line.log_index is not None and (
                marked_logs_start <= line.log_index <= marked_logs_end
            ):
                new_fragments = fill_character_width(
                    line.fragments,
                    len(line.fragments) - 1,  # -1 for the ending line break
                    self.width,
                )

                # Apply a style to highlight this line.
                all_lines.append(
                    to_formatted_text(
                        new_fragments, style='class:marked-log-line'
                    )
                )

            else:
                all_lines.append(line.fragments)

        return all_lines

    def _prepend_line(self, line: ScreenLine) -> None:
        """Add a line to the top of the screen."""
        self.line_buffer.appendleft(line)
        self._empty = False

    def _append_line(self, line: ScreenLine) -> None:
        """Add a line to the bottom of the screen."""
        self.line_buffer.append(line)
        self._empty = False

    def _trim_top_lines(self) -> None:
        """Remove lines from the top if larger than the screen height."""
        overflow_amount = len(self.line_buffer) - self.height
        for _ in range(overflow_amount):
            self.line_buffer.popleft()

    def _trim_bottom_lines(self) -> None:
        """Remove lines from the bottom if larger than the screen height."""
        overflow_amount = len(self.line_buffer) - self.height
        for _ in range(overflow_amount):
            self.line_buffer.pop()

    def move_cursor_up(self, line_count: int) -> int:
        """Move the cursor up as far as it can go without fetching new lines.

        Args:
            line_count: A negative number of lines to move the cursor by.

        Returns:
            int: The remaining line count that was not moved. This is the number
            of new lines that need to be fetched and prepended to the screen
            line buffer."""
        remaining_lines = line_count

        # Loop from a negative line_count value to zero.
        # For example if line_count is -5 the loop will traverse:
        # >>> list(i for i in range(-5, 0, 1))
        # [-5, -4, -3, -2, -1]
        for _ in range(line_count, 0, 1):
            new_index = self.cursor_position - 1
            if new_index < 0:
                break
            if (
                new_index < len(self.line_buffer)
                and self.line_buffer[new_index].empty()
            ):
                # The next line is empty and has no content.
                break
            self.cursor_position -= 1
            remaining_lines += 1
        return remaining_lines

    def move_cursor_down(self, line_count: int) -> int:
        """Move the cursor down as far as it can go without fetching new lines.

        Args:
            line_count: A positive number of lines to move the cursor down by.

        Returns:
            int: The remaining line count that was not moved. This is the number
            of new lines that need to be fetched and appended to the screen line
            buffer."""
        remaining_lines = line_count
        for _ in range(line_count):
            new_index = self.cursor_position + 1
            if new_index >= self.height:
                break
            if (
                new_index < len(self.line_buffer)
                and self.line_buffer[new_index].empty()
            ):
                # The next line is empty and has no content.
                break
            self.cursor_position += 1
            remaining_lines -= 1
        return remaining_lines

    def move_cursor_to_bottom(self) -> None:
        """Move the cursor to the bottom of the screen.

        Only use this for movement not initiated by users. For example if new
        logs were just added to the bottom of the screen in follow
        mode. The LogScreen class does not allow scrolling beyond the bottom of
        the content so the cursor will fall on a log message as long as there
        are some log messages. If there are no log messages the line is not
        highlighted by get_lines()."""
        self.cursor_position = self.height - 1

    def move_cursor_to_position(self, window_row: int) -> None:
        """Move the cursor to a line if there is a log message there."""
        if window_row >= len(self.line_buffer):
            return
        if 0 <= window_row < self.height:
            current_line = self.line_buffer[window_row]
            if current_line.log_index is not None:
                self.cursor_position = window_row

    def _move_selection_to_log(self, log_index: int, subline: int) -> None:
        """Move the cursor to the location of log_index."""
        for i, line in enumerate(self.line_buffer):
            if line.log_index == log_index and line.subline == subline:
                self.cursor_position = i
                return

    def shift_selected_log_to_top(self) -> None:
        """Shift the selected line to the top.

        This moves the lines on screen and keeps the originally selected line
        highlighted. Example use case: when jumping to a search match the
        matched line will be shown at the top of the screen."""
        if not 0 <= self.cursor_position < len(self.line_buffer):
            return

        current_line = self.line_buffer[self.cursor_position]
        amount = max(self.cursor_position, current_line.height)
        amount -= current_line.subline
        remaining_lines = self.scroll_subline(amount)
        if remaining_lines != 0 and current_line.log_index is not None:
            # Restore original selected line.
            self._move_selection_to_log(
                current_line.log_index, current_line.subline
            )
            return
        # Lines scrolled as expected, set cursor_position to top.
        self.cursor_position = 0

    def shift_selected_log_to_center(self) -> None:
        """Shift the selected line to the center.

        This moves the lines on screen and keeps the originally selected line
        highlighted. Example use case: when jumping to a search match the
        matched line will be shown at the center of the screen."""
        if not 0 <= self.cursor_position < len(self.line_buffer):
            return

        half_height = int(self.height / 2)
        current_line = self.line_buffer[self.cursor_position]

        amount = max(self.cursor_position - half_height, current_line.height)
        amount -= current_line.subline

        remaining_lines = self.scroll_subline(amount)
        if remaining_lines != 0 and current_line.log_index is not None:
            # Restore original selected line.
            self._move_selection_to_log(
                current_line.log_index, current_line.subline
            )
            return

        # Lines scrolled as expected, set cursor_position to center.
        self.cursor_position -= amount
        self.cursor_position -= current_line.height - 1

    def scroll_subline(self, line_count: int = 1) -> int:
        """Move the cursor down or up by positive or negative lines.

        Args:
            line_count: A positive or negative number of lines the cursor should
                move. Positive for down, negative for up.

        Returns:
            int: The remaining line count that was not moved. This is the number
            of new lines that could not be fetched in the case that the top or
            bottom of available log message lines was reached."""
        # Move self.cursor_position as far as it can go on screen without
        # fetching new log message lines.
        if line_count > 0:
            remaining_lines = self.move_cursor_down(line_count)
        else:
            remaining_lines = self.move_cursor_up(line_count)

        if remaining_lines == 0:
            # No more lines needed, return
            return remaining_lines

        # Top or bottom of the screen was reached, fetch and add new log lines.
        if remaining_lines < 0:
            return self.fetch_subline_up(remaining_lines)
        return self.fetch_subline_down(remaining_lines)

    def fetch_subline_up(self, line_count: int = -1) -> int:
        """Fetch new lines from the top in order of decreasing log_indexes.

        Args:
            line_count: A negative number of lines that should be fetched and
                added to the top of the screen.

        Returns:
            int: The number of lines that were not fetched. Returns 0 if the
                desired number of lines were fetched successfully."""
        start_log_index, _log_source = self.get_log_source()
        remaining_lines = line_count
        for _ in range(line_count, 0, 1):
            current_line = self.get_line_at_cursor_position()
            if current_line.log_index is None:
                return remaining_lines + 1

            target_log_index: int
            target_subline: int

            # If the current subline is at the start of this log, fetch the
            # previous log message's last subline.
            if current_line.subline == 0:
                target_log_index = current_line.log_index - 1
                # Set -1 to signal fetching the previous log's last subline
                target_subline = -1
            else:
                # Get previous sub line of current log
                target_log_index = current_line.log_index
                target_subline = current_line.subline - 1

            if target_log_index < start_log_index:
                # Invalid log_index, don't scroll further
                return remaining_lines + 1

            self.prepend_log(target_log_index, subline=target_subline)
            remaining_lines += 1

        return remaining_lines

    def get_line_at_cursor_position(self) -> ScreenLine:
        """Returns the ScreenLine under the cursor."""
        if (
            self.cursor_position >= len(self.line_buffer)
            or self.cursor_position < 0
        ):
            return ScreenLine([('', '')])
        return self.line_buffer[self.cursor_position]

    def fetch_subline_down(self, line_count: int = 1) -> int:
        """Fetch new lines from the bottom in order of increasing log_indexes.

        Args:
            line_count: A positive number of lines that should be fetched and
                added to the bottom of the screen.

        Returns:
            int: The number of lines that were not fetched. Returns 0 if the
                desired number of lines were fetched successfully."""
        _start_log_index, log_source = self.get_log_source()
        remaining_lines = line_count
        for _ in range(line_count):
            # Skip this line if not at the bottom
            if self.cursor_position < self.height - 1:
                self.cursor_position += 1
                continue

            current_line = self.get_line_at_cursor_position()
            if current_line.log_index is None:
                return remaining_lines - 1

            target_log_index: int
            target_subline: int

            # If the current subline is at the height of this log, fetch the
            # next log message.
            if current_line.subline == current_line.height - 1:
                # Get next log's first subline
                target_log_index = current_line.log_index + 1
                target_subline = 0
            else:
                # Get next sub line of current log
                target_log_index = current_line.log_index
                target_subline = current_line.subline + 1

            if target_log_index >= len(log_source):
                # Invalid log_index, don't scroll further
                return remaining_lines - 1

            self.append_log(target_log_index, subline=target_subline)
            remaining_lines -= 1

        return remaining_lines

    def first_rendered_log_index(self) -> Optional[int]:
        """Scan the screen for the first valid log_index and return it."""
        log_index = None
        for i in range(self.height):
            if i >= len(self.line_buffer):
                break
            if self.line_buffer[i].log_index is not None:
                log_index = self.line_buffer[i].log_index
                break
        return log_index

    def last_rendered_log_index(self) -> Optional[int]:
        """Return the last log_index shown on screen."""
        log_index = None
        if len(self.line_buffer) == 0:
            return None
        if self.line_buffer[-1].log_index is not None:
            log_index = self.line_buffer[-1].log_index
        return log_index

    def _get_fragments_per_line(
        self, log_index: int
    ) -> List[StyleAndTextTuples]:
        """Return a list of lines wrapped to the screen width for a log.

        Before fetching the log message this function updates the log_source and
        formatting options."""
        _start_log_index, log_source = self.get_log_source()
        if log_index >= len(log_source):
            return []
        log = log_source[log_index]
        table_formatter = self.get_log_formatter()
        truncate_lines = not self.get_line_wrapping()
        search_filter = self.get_search_filter()
        search_highlight = self.get_search_highlight()

        # Select the log display formatter; table or standard.
        fragments: StyleAndTextTuples = []
        if table_formatter:
            fragments = table_formatter(log)
        else:
            fragments = log.get_fragments()

        # Apply search term highlighting.
        if search_filter and search_highlight and search_filter.matches(log):
            fragments = search_filter.highlight_search_matches(fragments)

        # Word wrap the log message or truncate to screen width
        line_fragments, _log_line_height = insert_linebreaks(
            fragments,
            max_line_width=self.width,
            truncate_long_lines=truncate_lines,
        )
        # Convert the existing flattened fragments to a list of lines.
        fragments_per_line = split_lines(line_fragments)

        return fragments_per_line

    def prepend_log(
        self,
        log_index: int,
        subline: Optional[int] = None,
    ) -> None:
        """Add a log message or a single line to the top of the screen.

        Args:
            log_index: The index of the log message to fetch.
            subline: The desired subline of the log message. When displayed on
                screen the log message may take up more than one line. If
                subline is 0 or higher that line will be added. If subline is -1
                the last subline will be prepended regardless of the total log
                message height.
        """
        fragments_per_line = self._get_fragments_per_line(log_index)

        # Target the last subline if the subline arg is set to -1.
        fetch_last_subline = subline == -1

        for line_index, line in enumerate(fragments_per_line):
            # If we are looking for a specific subline and this isn't it, skip.
            if subline is not None:
                # If subline is set to -1 we need to append the last subline of
                # this log message. Skip this line if it isn't the last one.
                if fetch_last_subline and (
                    line_index != len(fragments_per_line) - 1
                ):
                    continue
                # If subline is not -1 (0 or higher) and this isn't the desired
                # line, skip to the next one.
                if not fetch_last_subline and line_index != subline:
                    continue

            self._prepend_line(
                ScreenLine(
                    fragments=line,
                    log_index=log_index,
                    subline=line_index,
                    height=len(fragments_per_line),
                )
            )

        # Remove lines from the bottom if over the screen height.
        if len(self.line_buffer) > self.height:
            self._trim_bottom_lines()

    def append_log(
        self,
        log_index: int,
        subline: Optional[int] = None,
    ) -> None:
        """Add a log message or a single line to the bottom of the screen."""
        # Save this log_index
        self.last_appended_log_index = log_index
        fragments_per_line = self._get_fragments_per_line(log_index)

        for line_index, line in enumerate(fragments_per_line):
            # If we are looking for a specific subline and this isn't it, skip.
            if subline is not None and line_index != subline:
                continue

            self._append_line(
                ScreenLine(
                    fragments=line,
                    log_index=log_index,
                    subline=line_index,
                    height=len(fragments_per_line),
                )
            )

        # Remove lines from the top if over the screen height.
        if len(self.line_buffer) > self.height:
            self._trim_top_lines()
