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
"""LogView maintains a log pane's scrolling and searching state."""

from __future__ import annotations
import asyncio
import collections
import copy
from enum import Enum
import itertools
import json
import logging
import operator
from pathlib import Path
import re
from threading import Thread
from typing import Callable, Dict, List, Optional, Tuple, TYPE_CHECKING

from prompt_toolkit.data_structures import Point
from prompt_toolkit.formatted_text import StyleAndTextTuples
import websockets

from pw_console.log_filter import (
    DEFAULT_SEARCH_MATCHER,
    LogFilter,
    RegexValidator,
    SearchMatcher,
    preprocess_search_regex,
)
from pw_console.log_screen import ScreenLine, LogScreen
from pw_console.log_store import LogStore
from pw_console.python_logging import log_record_to_json
from pw_console.text_formatting import remove_formatting

if TYPE_CHECKING:
    from pw_console.console_app import ConsoleApp
    from pw_console.log_line import LogLine
    from pw_console.log_pane import LogPane

_LOG = logging.getLogger(__package__)


class FollowEvent(Enum):
    """Follow mode scroll event types."""

    SEARCH_MATCH = 'scroll_to_bottom'
    STICKY_FOLLOW = 'scroll_to_bottom_with_sticky_follow'


class LogView:
    """Viewing window into a LogStore."""

    # pylint: disable=too-many-instance-attributes,too-many-public-methods

    def __init__(
        self,
        log_pane: 'LogPane',
        application: 'ConsoleApp',
        log_store: Optional[LogStore] = None,
    ):
        # Parent LogPane reference. Updated by calling `set_log_pane()`.
        self.log_pane = log_pane
        self.log_store = (
            log_store if log_store else LogStore(prefs=application.prefs)
        )
        self.log_store.set_prefs(application.prefs)
        self.log_store.register_viewer(self)

        self.marked_logs_start: Optional[int] = None
        self.marked_logs_end: Optional[int] = None

        # Search variables
        self.search_text: Optional[str] = None
        self.search_filter: Optional[LogFilter] = None
        self.search_highlight: bool = False
        self.search_matcher = DEFAULT_SEARCH_MATCHER
        self.search_validator = RegexValidator()

        # Container for each log_index matched by active searches.
        self.search_matched_lines: Dict[int, int] = {}
        # Background task to find historical matched lines.
        self.search_match_count_task: Optional[asyncio.Task] = None

        # Flag for automatically jumping to each new search match as they
        # appear.
        self.follow_search_match: bool = False
        self.last_search_matched_log: Optional[int] = None

        # Follow event flag. This is set to by the new_logs_arrived() function
        # as a signal that the log screen should be scrolled to the bottom.
        # This is read by render_content() whenever the screen is drawn.
        self.follow_event: Optional[FollowEvent] = None

        self.log_screen = LogScreen(
            get_log_source=self._get_log_lines,
            get_line_wrapping=self.wrap_lines_enabled,
            get_log_formatter=self._get_table_formatter,
            get_search_filter=lambda: self.search_filter,
            get_search_highlight=lambda: self.search_highlight,
        )

        # Filter
        self.filtering_on: bool = False
        self.filters: 'collections.OrderedDict[str, LogFilter]' = (
            collections.OrderedDict()
        )
        self.filtered_logs: collections.deque = collections.deque()
        self.filter_existing_logs_task: Optional[asyncio.Task] = None

        # Current log line index state variables:
        self._last_log_index = -1
        self._log_index = 0
        self._filtered_log_index = 0
        self._last_start_index = 0
        self._last_end_index = 0
        self._current_start_index = 0
        self._current_end_index = 0
        self._scrollback_start_index = 0

        # LogPane prompt_toolkit container render size.
        self._window_height = 20
        self._window_width = 80
        self._reset_log_screen_on_next_render: bool = True
        self._user_scroll_event: bool = False

        self._last_log_store_index = 0
        self._new_logs_since_last_render = True
        self._new_logs_since_last_websocket_serve = True
        self._last_served_websocket_index = -1

        # Should new log lines be tailed?
        self.follow: bool = True

        self.visual_select_mode: bool = False

        # Cache of formatted text tuples used in the last UI render.
        self._line_fragment_cache: List[StyleAndTextTuples] = []

        # websocket server variables
        self.websocket_running: bool = False
        self.websocket_server = None
        self.websocket_port = None
        self.websocket_loop = asyncio.new_event_loop()

        # Check if any logs are already in the log_store and update the view.
        self.new_logs_arrived()

    def _websocket_thread_entry(self):
        """Entry point for the user code thread."""
        asyncio.set_event_loop(self.websocket_loop)
        self.websocket_server = websockets.serve(  # type: ignore # pylint: disable=no-member
            self._send_logs_over_websockets, '127.0.0.1'
        )
        self.websocket_loop.run_until_complete(self.websocket_server)
        self.websocket_port = self.websocket_server.ws_server.sockets[
            0
        ].getsockname()[1]
        self.log_pane.application.application.clipboard.set_text(
            self.get_web_socket_url()
        )
        self.websocket_running = True
        self.websocket_loop.run_forever()

    def start_websocket_thread(self):
        """Create a thread for running user code so the UI isn't blocked."""
        thread = Thread(
            target=self._websocket_thread_entry, args=(), daemon=True
        )
        thread.start()

    def stop_websocket_thread(self):
        """Stop websocket server."""
        if self.websocket_running:
            self.websocket_loop.call_soon_threadsafe(self.websocket_loop.stop)
            self.websocket_server = None
            self.websocket_port = None
            self.websocket_running = False
            if self.filtering_on:
                self._restart_filtering()

    async def _send_logs_over_websockets(self, websocket, _path) -> None:
        formatter: Callable[[LogLine], str] = operator.attrgetter(
            'ansi_stripped_log'
        )
        formatter = lambda log: log_record_to_json(log.record)

        theme_colors = json.dumps(
            self.log_pane.application.prefs.pw_console_color_config()
        )
        # Send colors
        await websocket.send(theme_colors)

        while True:
            # Wait for new logs
            if not self._new_logs_since_last_websocket_serve:
                await asyncio.sleep(0.5)

            _start_log_index, log_source = self._get_log_lines()
            log_index_range = range(
                self._last_served_websocket_index + 1, self.get_total_count()
            )

            for i in log_index_range:
                log_text = formatter(log_source[i])
                await websocket.send(log_text)
                self._last_served_websocket_index = i

            # Flag that all logs have been served.
            self._new_logs_since_last_websocket_serve = False

    def view_mode_changed(self) -> None:
        self._reset_log_screen_on_next_render = True

    @property
    def log_index(self):
        if self.filtering_on:
            return self._filtered_log_index
        return self._log_index

    @log_index.setter
    def log_index(self, new_log_index):
        # Save the old log_index
        self._last_log_index = self.log_index
        if self.filtering_on:
            self._filtered_log_index = new_log_index
        else:
            self._log_index = new_log_index

    def _reset_log_index_changed(self) -> None:
        self._last_log_index = self.log_index

    def log_index_changed_since_last_render(self) -> bool:
        return self._last_log_index != self.log_index

    def _set_match_position(self, position: int):
        self.follow = False
        self.log_index = position
        self.save_search_matched_line(position)
        self.log_screen.reset_logs(log_index=self.log_index)
        self.log_screen.shift_selected_log_to_center()
        self._user_scroll_event = True
        self.log_pane.application.redraw_ui()

    def select_next_search_matcher(self):
        matchers = list(SearchMatcher)
        index = matchers.index(self.search_matcher)
        new_index = (index + 1) % len(matchers)
        self.search_matcher = matchers[new_index]

    def search_forwards(self):
        if not self.search_filter:
            return
        self.search_highlight = True

        log_beginning_index = self.hidden_line_count()

        starting_index = self.log_index + 1
        if starting_index > self.get_last_log_index():
            starting_index = log_beginning_index

        _, logs = self._get_log_lines()

        # From current position +1 and down
        for i in range(starting_index, self.get_last_log_index() + 1):
            if self.search_filter.matches(logs[i]):
                self._set_match_position(i)
                return

        # From the beginning to the original start
        for i in range(log_beginning_index, starting_index):
            if self.search_filter.matches(logs[i]):
                self._set_match_position(i)
                return

    def search_backwards(self):
        if not self.search_filter:
            return
        self.search_highlight = True

        log_beginning_index = self.hidden_line_count()

        starting_index = self.log_index - 1
        if starting_index < 0:
            starting_index = self.get_last_log_index()

        _, logs = self._get_log_lines()

        # From current position - 1 and up
        for i in range(starting_index, log_beginning_index - 1, -1):
            if self.search_filter.matches(logs[i]):
                self._set_match_position(i)
                return

        # From the end to the original start
        for i in range(self.get_last_log_index(), starting_index, -1):
            if self.search_filter.matches(logs[i]):
                self._set_match_position(i)
                return

    def set_search_regex(
        self, text, invert, field, matcher: Optional[SearchMatcher] = None
    ) -> bool:
        search_matcher = matcher if matcher else self.search_matcher
        _LOG.debug(search_matcher)

        regex_text, regex_flags = preprocess_search_regex(
            text, matcher=search_matcher
        )

        try:
            compiled_regex = re.compile(regex_text, regex_flags)
            self.search_filter = LogFilter(
                regex=compiled_regex,
                input_text=text,
                invert=invert,
                field=field,
            )
            _LOG.debug(self.search_filter)
        except re.error as error:
            _LOG.debug(error)
            return False

        self.search_highlight = True
        self.search_text = regex_text
        return True

    def new_search(
        self,
        text,
        invert=False,
        field: Optional[str] = None,
        search_matcher: Optional[str] = None,
        interactive: bool = True,
    ) -> bool:
        """Start a new search for the given text."""
        valid_matchers = list(s.name for s in SearchMatcher)
        selected_matcher: Optional[SearchMatcher] = None
        if (
            search_matcher is not None
            and search_matcher.upper() in valid_matchers
        ):
            selected_matcher = SearchMatcher(search_matcher.upper())

        if not self.set_search_regex(text, invert, field, selected_matcher):
            return False

        # Clear matched lines
        self.search_matched_lines = {}

        if interactive:
            # Start count historical search matches task.
            self.search_match_count_task = asyncio.create_task(
                self.count_search_matches()
            )

        # Default search direction when hitting enter in the search bar.
        if interactive:
            self.search_forwards()
        return True

    def save_search_matched_line(self, log_index: int) -> None:
        """Save the log_index at position as a matched line."""
        self.search_matched_lines[log_index] = 0
        # Keep matched lines sorted by position
        self.search_matched_lines = {
            # Save this log_index and its match number.
            log_index: match_number
            for match_number, log_index in enumerate(
                sorted(self.search_matched_lines.keys())
            )
        }

    def disable_search_highlighting(self):
        self.log_pane.log_view.search_highlight = False

    def _restart_filtering(self):
        # Turn on follow
        if not self.follow:
            self.toggle_follow()

        # Reset filtered logs.
        self.filtered_logs.clear()
        # Reset scrollback start
        self._scrollback_start_index = 0

        # Start filtering existing log lines.
        self.filter_existing_logs_task = asyncio.create_task(
            self.filter_past_logs()
        )

        # Reset existing search
        self.clear_search()

        # Trigger a main menu update to set log window menu titles.
        self.log_pane.application.update_menu_items()
        # Redraw the UI
        self.log_pane.application.redraw_ui()

    def install_new_filter(self):
        """Set a filter using the current search_regex."""
        if not self.search_filter:
            return

        self.filtering_on = True
        self.filters[self.search_text] = copy.deepcopy(self.search_filter)

        self.clear_search()

    def apply_filter(self):
        """Set new filter and schedule historical log filter asyncio task."""
        if self.websocket_running:
            return
        self.install_new_filter()
        self._restart_filtering()

    def clear_search_highlighting(self):
        self.search_highlight = False
        self._reset_log_screen_on_next_render = True

    def clear_search(self):
        self.search_matched_lines = {}
        self.search_text = None
        self.search_filter = None
        self.search_highlight = False
        self._reset_log_screen_on_next_render = True

    def _get_log_lines(self) -> Tuple[int, collections.deque[LogLine]]:
        logs = self.log_store.logs
        if self.filtering_on:
            logs = self.filtered_logs
        return self._scrollback_start_index, logs

    def _get_visible_log_lines(self):
        _, logs = self._get_log_lines()
        if self._scrollback_start_index > 0:
            return collections.deque(
                itertools.islice(logs, self.hidden_line_count(), len(logs))
            )
        return logs

    def _get_table_formatter(self) -> Optional[Callable]:
        table_formatter = None
        if self.log_pane.table_view:
            table_formatter = self.log_store.table.formatted_row
        return table_formatter

    def delete_filter(self, filter_text):
        if filter_text not in self.filters:
            return

        # Delete this filter
        del self.filters[filter_text]

        # If no filters left, stop filtering.
        if len(self.filters) == 0:
            self.clear_filters()
        else:
            # Erase existing filtered lines.
            self._restart_filtering()

    def clear_filters(self):
        if not self.filtering_on:
            return
        self.clear_search()
        self.filtering_on = False
        self.filters: 'collections.OrderedDict[str, re.Pattern]' = (
            collections.OrderedDict()
        )
        self.filtered_logs.clear()
        # Reset scrollback start
        self._scrollback_start_index = 0
        if not self.follow:
            self.toggle_follow()

    async def count_search_matches(self):
        """Count search matches and save their locations."""
        # Wait for any filter_existing_logs_task to finish.
        if self.filtering_on and self.filter_existing_logs_task:
            await self.filter_existing_logs_task

        starting_index = self.get_last_log_index()
        ending_index, logs = self._get_log_lines()

        # From the end of the log store to the beginning.
        for i in range(starting_index, ending_index - 1, -1):
            # Is this log a match?
            if self.search_filter.matches(logs[i]):
                self.save_search_matched_line(i)
            # Pause every 100 lines or so
            if i % 100 == 0:
                await asyncio.sleep(0.1)

    async def filter_past_logs(self):
        """Filter past log lines."""
        starting_index = self.log_store.get_last_log_index()
        ending_index = -1

        # From the end of the log store to the beginning.
        for i in range(starting_index, ending_index, -1):
            # Is this log a match?
            if self.filter_scan(self.log_store.logs[i]):
                # Add to the beginning of the deque.
                self.filtered_logs.appendleft(self.log_store.logs[i])
            # TODO(tonymd): Tune these values.
            # Pause every 100 lines or so
            if i % 100 == 0:
                await asyncio.sleep(0.1)

    def set_log_pane(self, log_pane: 'LogPane'):
        """Set the parent LogPane instance."""
        self.log_pane = log_pane

    def _update_log_index(self) -> ScreenLine:
        line_at_cursor = self.log_screen.get_line_at_cursor_position()
        if line_at_cursor.log_index is not None:
            self.log_index = line_at_cursor.log_index
        return line_at_cursor

    def get_current_line(self) -> int:
        """Return the currently selected log event index."""
        return self.log_index

    def get_total_count(self):
        """Total size of the logs store."""
        return (
            len(self.filtered_logs)
            if self.filtering_on
            else self.log_store.get_total_count()
        )

    def get_last_log_index(self):
        total = self.get_total_count()
        return 0 if total < 0 else total - 1

    def clear_scrollback(self):
        """Hide log lines before the max length of the stored logs."""
        # Enable follow and scroll to the bottom, then clear.
        if not self.follow:
            self.toggle_follow()
        self._scrollback_start_index = self.log_index
        self._reset_log_screen_on_next_render = True

    def hidden_line_count(self):
        """Return the number of hidden lines."""
        if self._scrollback_start_index > 0:
            return self._scrollback_start_index + 1
        return 0

    def undo_clear_scrollback(self):
        """Reset the current scrollback start index."""
        self._scrollback_start_index = 0

    def wrap_lines_enabled(self):
        """Get the parent log pane wrap lines setting."""
        if not self.log_pane:
            return False
        return self.log_pane.wrap_lines

    def toggle_follow(self):
        """Toggle auto line following."""
        self.follow = not self.follow
        if self.follow:
            # Disable search match follow mode.
            self.follow_search_match = False
            self.scroll_to_bottom()

    def filter_scan(self, log: 'LogLine'):
        filter_match_count = 0
        for _filter_text, log_filter in self.filters.items():
            if log_filter.matches(log):
                filter_match_count += 1
            else:
                break

        if filter_match_count == len(self.filters):
            return True
        return False

    def new_logs_arrived(self):
        """Check newly arrived log messages.

        Depending on where log statements occur ``new_logs_arrived`` may be in a
        separate thread since it is triggerd by the Python log handler
        ``emit()`` function. In this case the log handler is the LogStore
        instance ``self.log_store``. This function should not redraw the screen
        or scroll.
        """
        latest_total = self.log_store.get_total_count()

        if self.filtering_on:
            # Scan newly arived log lines
            for i in range(self._last_log_store_index, latest_total):
                if self.filter_scan(self.log_store.logs[i]):
                    self.filtered_logs.append(self.log_store.logs[i])

        if self.search_filter:
            last_matched_log: Optional[int] = None
            # Scan newly arived log lines
            for i in range(self._last_log_store_index, latest_total):
                if self.search_filter.matches(self.log_store.logs[i]):
                    self.save_search_matched_line(i)
                    last_matched_log = i
            if last_matched_log and self.follow_search_match:
                # Set the follow event flag for the next render_content call.
                self.follow_event = FollowEvent.SEARCH_MATCH
                self.last_search_matched_log = last_matched_log

        self._last_log_store_index = latest_total
        self._new_logs_since_last_render = True
        self._new_logs_since_last_websocket_serve = True

        if self.follow:
            # Set the follow event flag for the next render_content call.
            self.follow_event = FollowEvent.STICKY_FOLLOW

        if self.websocket_running:
            # No terminal screen redraws are required.
            return

        # Trigger a UI update if the log window is visible.
        if self.log_pane.show_pane:
            self.log_pane.application.logs_redraw()

    def get_cursor_position(self) -> Point:
        """Return the position of the cursor."""
        return Point(0, self.log_screen.cursor_position)

    def scroll_to_top(self):
        """Move selected index to the beginning."""
        # Stop following so cursor doesn't jump back down to the bottom.
        self.follow = False
        # First possible log index that should be displayed
        log_beginning_index = self.hidden_line_count()
        self.log_index = log_beginning_index
        self.log_screen.reset_logs(log_index=self.log_index)
        self.log_screen.shift_selected_log_to_top()
        self._user_scroll_event = True

    def move_selected_line_to_top(self):
        self.follow = False

        # Update selected line
        self._update_log_index()

        self.log_screen.reset_logs(log_index=self.log_index)
        self.log_screen.shift_selected_log_to_top()
        self._user_scroll_event = True

    def center_log_line(self):
        self.follow = False

        # Update selected line
        self._update_log_index()

        self.log_screen.reset_logs(log_index=self.log_index)
        self.log_screen.shift_selected_log_to_center()
        self._user_scroll_event = True

    def scroll_to_bottom(self, with_sticky_follow: bool = True):
        """Move selected index to the end."""
        # Don't change following state like scroll_to_top.
        self.log_index = max(0, self.get_last_log_index())
        self.log_screen.reset_logs(log_index=self.log_index)

        # Sticky follow mode
        if with_sticky_follow:
            self.follow = True
        self._user_scroll_event = True

    def scroll(self, lines) -> None:
        """Scroll up or down by plus or minus lines.

        This method is only called by user keybindings.
        """
        # If the user starts scrolling, stop auto following.
        self.follow = False

        self.log_screen.scroll_subline(lines)
        self._user_scroll_event = True

        # Update the current log
        current_line = self._update_log_index()

        # Don't check for sticky follow mode if selecting lines.
        if self.visual_select_mode:
            return
        # Is the last log line selected?
        if self.log_index == self.get_last_log_index():
            # Is the last line of the current log selected?
            if current_line.subline + 1 == current_line.height:
                # Sticky follow mode
                self.follow = True

    def visual_selected_log_count(self) -> int:
        if self.marked_logs_start is None or self.marked_logs_end is None:
            return 0
        return (self.marked_logs_end - self.marked_logs_start) + 1

    def clear_visual_selection(self) -> None:
        self.marked_logs_start = None
        self.marked_logs_end = None
        self.visual_select_mode = False
        self._user_scroll_event = True
        self.log_pane.application.redraw_ui()

    def visual_select_all(self) -> None:
        self.marked_logs_start = self._scrollback_start_index
        self.marked_logs_end = self.get_total_count() - 1

        self.visual_select_mode = True
        self._user_scroll_event = True
        self.log_pane.application.redraw_ui()

    def visual_select_up(self) -> None:
        # Select the current line
        self.visual_select_line(self.get_cursor_position(), autoscroll=False)
        # Move the cursor by 1
        self.scroll_up(1)
        # Select the new line
        self.visual_select_line(self.get_cursor_position(), autoscroll=False)

    def visual_select_down(self) -> None:
        # Select the current line
        self.visual_select_line(self.get_cursor_position(), autoscroll=False)
        # Move the cursor by 1
        self.scroll_down(1)
        # Select the new line
        self.visual_select_line(self.get_cursor_position(), autoscroll=False)

    def visual_select_line(
        self, mouse_position: Point, autoscroll: bool = True
    ) -> None:
        """Mark the log under mouse_position as visually selected."""
        # Check mouse_position is valid
        if not 0 <= mouse_position.y < len(self.log_screen.line_buffer):
            return
        # Update mode flags
        self.visual_select_mode = True
        self.follow = False
        # Get the ScreenLine for the cursor position
        screen_line = self.log_screen.line_buffer[mouse_position.y]
        if screen_line.log_index is None:
            return

        if self.marked_logs_start is None:
            self.marked_logs_start = screen_line.log_index
        if self.marked_logs_end is None:
            self.marked_logs_end = screen_line.log_index

        if screen_line.log_index < self.marked_logs_start:
            self.marked_logs_start = screen_line.log_index
        elif screen_line.log_index > self.marked_logs_end:
            self.marked_logs_end = screen_line.log_index

        # Update cursor position
        self.log_screen.move_cursor_to_position(mouse_position.y)

        # Autoscroll when mouse dragging on the top or bottom of the window.
        if autoscroll:
            if mouse_position.y == 0:
                self.scroll_up(1)
            elif mouse_position.y == self._window_height - 1:
                self.scroll_down(1)

        # Trigger a rerender.
        self._user_scroll_event = True
        self.log_pane.application.redraw_ui()

    def scroll_to_position(self, mouse_position: Point):
        """Set the selected log line to the mouse_position."""
        # Disable follow mode when the user clicks or mouse drags on a log line.
        self.follow = False

        self.log_screen.move_cursor_to_position(mouse_position.y)
        self._update_log_index()

        self._user_scroll_event = True

    def scroll_up_one_page(self):
        """Move the selected log index up by one window height."""
        lines = 1
        if self._window_height > 0:
            lines = self._window_height
        self.scroll(-1 * lines)

    def scroll_down_one_page(self):
        """Move the selected log index down by one window height."""
        lines = 1
        if self._window_height > 0:
            lines = self._window_height
        self.scroll(lines)

    def scroll_down(self, lines=1):
        """Move the selected log index down by one or more lines."""
        self.scroll(lines)

    def scroll_up(self, lines=1):
        """Move the selected log index up by one or more lines."""
        self.scroll(-1 * lines)

    def log_start_end_indexes_changed(self) -> bool:
        return (
            self._last_start_index != self._current_start_index
            or self._last_end_index != self._current_end_index
        )

    def render_table_header(self):
        """Get pre-formatted table header."""
        return self.log_store.render_table_header()

    def get_web_socket_url(self):
        return f'http://127.0.0.1:3000/#ws={self.websocket_port}'

    def render_content(self) -> List:
        """Return logs to display on screen as a list of FormattedText tuples.

        This function determines when the log screen requires re-rendeing based
        on user scroll events, follow mode being on, or log pane being
        empty. The FormattedText tuples passed to prompt_toolkit are cached if
        no updates are required.
        """
        screen_update_needed = False

        # Disable rendering if user is viewing logs on web
        if self.websocket_running:
            return []

        # Check window size
        if self.log_pane.pane_resized():
            self._window_width = self.log_pane.current_log_pane_width
            self._window_height = self.log_pane.current_log_pane_height
            self.log_screen.resize(self._window_width, self._window_height)
            self._reset_log_screen_on_next_render = True

        if self.follow_event is not None:
            if (
                self.follow_event == FollowEvent.SEARCH_MATCH
                and self.last_search_matched_log
            ):
                self.log_index = self.last_search_matched_log
                self.last_search_matched_log = None
                self._reset_log_screen_on_next_render = True

            elif self.follow_event == FollowEvent.STICKY_FOLLOW:
                # Jump to the last log message
                self.log_index = max(0, self.get_last_log_index())

            self.follow_event = None
            screen_update_needed = True

        if self._reset_log_screen_on_next_render or self.log_screen.empty():
            # Clear the reset flag.
            self._reset_log_screen_on_next_render = False
            self.log_screen.reset_logs(log_index=self.log_index)
            screen_update_needed = True

        elif self.follow and self._new_logs_since_last_render:
            # Follow mode is on so add new logs to the screen
            self._new_logs_since_last_render = False

            current_log_index = self.log_index
            last_rendered_log_index = self.log_screen.last_appended_log_index
            # If so many logs have arrived than can fit on the screen, redraw
            # the whole screen from the new position.
            if (
                current_log_index - last_rendered_log_index
            ) > self.log_screen.height:
                self.log_screen.reset_logs(log_index=self.log_index)
            # A small amount of logs have arrived, append them one at a time
            # without redrawing the whole screen.
            else:
                for i in range(
                    last_rendered_log_index + 1, current_log_index + 1
                ):
                    self.log_screen.append_log(i)

            screen_update_needed = True

        if self.follow:
            # Select the last line for follow mode.
            self.log_screen.move_cursor_to_bottom()
            screen_update_needed = True

        if self._user_scroll_event:
            self._user_scroll_event = False
            screen_update_needed = True

        if screen_update_needed:
            self._line_fragment_cache = self.log_screen.get_lines(
                marked_logs_start=self.marked_logs_start,
                marked_logs_end=self.marked_logs_end,
            )
        return self._line_fragment_cache

    def _logs_to_text(
        self,
        use_table_formatting: bool = True,
        selected_lines_only: bool = False,
    ) -> str:
        """Convert all or selected log messages to plaintext."""

        def get_table_string(log: LogLine) -> str:
            return remove_formatting(self.log_store.table.formatted_row(log))

        formatter: Callable[[LogLine], str] = operator.attrgetter(
            'ansi_stripped_log'
        )
        if use_table_formatting:
            formatter = get_table_string

        _start_log_index, log_source = self._get_log_lines()

        log_index_range = range(
            self._scrollback_start_index, self.get_total_count()
        )
        if (
            selected_lines_only
            and self.marked_logs_start is not None
            and self.marked_logs_end is not None
        ):
            log_index_range = range(
                self.marked_logs_start, self.marked_logs_end + 1
            )

        text_output = ''
        for i in log_index_range:
            log_text = formatter(log_source[i])
            text_output += log_text
            if not log_text.endswith('\n'):
                text_output += '\n'

        return text_output

    def export_logs(
        self,
        use_table_formatting: bool = True,
        selected_lines_only: bool = False,
        file_name: Optional[str] = None,
        to_clipboard: bool = False,
        add_markdown_fence: bool = False,
    ) -> bool:
        """Export log lines to file or clipboard."""
        text_output = self._logs_to_text(
            use_table_formatting, selected_lines_only
        )

        if file_name:
            target_path = Path(file_name).expanduser()
            with target_path.open('w') as output_file:
                output_file.write(text_output)
            _LOG.debug('Saved to file: %s', file_name)

        elif to_clipboard:
            if add_markdown_fence:
                text_output = '```\n' + text_output + '```\n'
            self.log_pane.application.application.clipboard.set_text(
                text_output
            )
            _LOG.debug('Copied logs to clipboard.')

        return True
