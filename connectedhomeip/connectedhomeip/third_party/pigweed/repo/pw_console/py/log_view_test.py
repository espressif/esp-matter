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
"""Tests for pw_console.log_view"""

import logging
import time
import sys
import unittest
from datetime import datetime
from unittest.mock import MagicMock, patch
from parameterized import parameterized  # type: ignore
from prompt_toolkit.data_structures import Point

from pw_console.console_prefs import ConsolePrefs
from pw_console.log_view import LogView
from pw_console.log_screen import ScreenLine
from pw_console.text_formatting import (
    flatten_formatted_text_tuples,
    join_adjacent_style_tuples,
)

_PYTHON_3_8 = sys.version_info >= (
    3,
    8,
)


def _create_log_view():
    log_pane = MagicMock()
    log_pane.pane_resized = MagicMock(return_value=True)
    log_pane.current_log_pane_width = 80
    log_pane.current_log_pane_height = 10

    application = MagicMock()
    application.prefs = ConsolePrefs(
        project_file=False, project_user_file=False, user_file=False
    )
    application.prefs.reset_config()
    log_view = LogView(log_pane, application)
    return log_view, log_pane


class TestLogView(unittest.TestCase):
    """Tests for LogView."""

    # pylint: disable=invalid-name
    def setUp(self):
        self.maxDiff = None

    # pylint: enable=invalid-name

    def _create_log_view_with_logs(self, log_count=100):
        log_view, log_pane = _create_log_view()

        if log_count > 0:
            test_log = logging.getLogger('log_view.test')
            with self.assertLogs(test_log, level='DEBUG') as _log_context:
                test_log.addHandler(log_view.log_store)
                for i in range(log_count):
                    test_log.debug('Test log %s', i)

        return log_view, log_pane

    def test_follow_toggle(self) -> None:
        log_view, _pane = _create_log_view()
        self.assertTrue(log_view.follow)
        log_view.toggle_follow()
        self.assertFalse(log_view.follow)

    def test_follow_scrolls_to_bottom(self) -> None:
        log_view, _pane = _create_log_view()
        log_view.toggle_follow()
        _fragments = log_view.render_content()
        self.assertFalse(log_view.follow)
        self.assertEqual(log_view.get_current_line(), 0)

        test_log = logging.getLogger('log_view.test')

        # Log 5 messagse, current_line should stay at 0
        with self.assertLogs(test_log, level='DEBUG') as _log_context:
            test_log.addHandler(log_view.log_store)
            for i in range(5):
                test_log.debug('Test log %s', i)
        _fragments = log_view.render_content()

        self.assertEqual(log_view.get_total_count(), 5)
        self.assertEqual(log_view.get_current_line(), 0)
        # Turn follow on
        log_view.toggle_follow()
        self.assertTrue(log_view.follow)

        # Log another messagse, current_line should move to the last.
        with self.assertLogs(test_log, level='DEBUG') as _log_context:
            test_log.addHandler(log_view.log_store)
            test_log.debug('Test log')
        _fragments = log_view.render_content()

        self.assertEqual(log_view.get_total_count(), 6)
        self.assertEqual(log_view.get_current_line(), 5)

    def test_scrolling(self) -> None:
        """Test all scrolling methods."""
        log_view, log_pane = self._create_log_view_with_logs(log_count=100)

        # Page scrolling needs to know the current window height.
        log_pane.pane_resized = MagicMock(return_value=True)
        log_pane.current_log_pane_width = 80
        log_pane.current_log_pane_height = 10

        log_view.render_content()

        # Follow is on by default, current line should be at the end.
        self.assertEqual(log_view.get_current_line(), 99)

        # Move to the beginning.
        log_view.scroll_to_top()
        log_view.render_content()
        self.assertEqual(log_view.get_current_line(), 0)

        # Should not be able to scroll before the beginning.
        log_view.scroll_up()
        log_view.render_content()
        self.assertEqual(log_view.get_current_line(), 0)
        log_view.scroll_up_one_page()
        log_view.render_content()
        self.assertEqual(log_view.get_current_line(), 0)

        # Single and multi line movement.
        log_view.scroll_down()
        log_view.render_content()
        self.assertEqual(log_view.get_current_line(), 1)
        log_view.scroll(5)
        log_view.render_content()
        self.assertEqual(log_view.get_current_line(), 6)
        log_view.scroll_up()
        log_view.render_content()
        self.assertEqual(log_view.get_current_line(), 5)

        # Page down and up.
        log_view.scroll_down_one_page()
        self.assertEqual(log_view.get_current_line(), 15)

        log_view.scroll_up_one_page()
        self.assertEqual(log_view.get_current_line(), 5)

        # Move to the end.
        log_view.scroll_to_bottom()
        log_view.render_content()
        self.assertEqual(log_view.get_current_line(), 99)

        # Should not be able to scroll beyond the end.
        log_view.scroll_down()
        log_view.render_content()
        self.assertEqual(log_view.get_current_line(), 99)
        log_view.scroll_down_one_page()
        log_view.render_content()
        self.assertEqual(log_view.get_current_line(), 99)

        # Move up a bit to turn off follow
        self.assertEqual(log_view.log_screen.cursor_position, 9)
        log_view.scroll(-1)
        self.assertEqual(log_view.log_screen.cursor_position, 8)
        log_view.render_content()
        self.assertEqual(log_view.get_current_line(), 98)

        # Simulate a mouse click to scroll.
        # Click 1 lines from the top of the window.
        log_view.scroll_to_position(Point(0, 1))
        log_view.render_content()
        self.assertEqual(log_view.get_current_line(), 90)

        # Disable follow mode if mouse click on line.
        log_view.toggle_follow()
        log_view.render_content()
        self.assertTrue(log_view.follow)
        self.assertEqual(log_view.get_current_line(), 99)
        log_view.scroll_to_position(Point(0, 5))
        log_view.render_content()
        self.assertEqual(log_view.get_current_line(), 95)
        self.assertFalse(log_view.follow)

    def test_render_content_and_cursor_position(self) -> None:
        """Test render_content results and get_cursor_position

        get_cursor_position() should return the correct position depending on
        what line is selected."""

        # Mock time to always return the same value.
        mock_time = MagicMock(  # type: ignore
            return_value=time.mktime(datetime(2021, 7, 13, 0, 0, 0).timetuple())
        )
        with patch('time.time', new=mock_time):
            log_view, log_pane = self._create_log_view_with_logs(log_count=4)

        # Mock needed LogPane functions that pull info from prompt_toolkit.
        log_pane.get_horizontal_scroll_amount = MagicMock(return_value=0)
        log_pane.current_log_pane_width = 80
        log_pane.current_log_pane_height = 10

        log_view.render_content()
        log_view.scroll_to_top()
        log_view.render_content()
        # Scroll to top keeps the cursor on the bottom of the window.
        self.assertEqual(log_view.get_cursor_position(), Point(x=0, y=9))

        log_view.scroll_to_bottom()
        log_view.render_content()
        self.assertEqual(log_view.get_cursor_position(), Point(x=0, y=9))

        expected_formatted_text = [
            ('', ''),
            ('class:log-time', '20210713 00:00:00'),
            ('', '  '),
            ('class:log-level-10', 'DEBUG'),
            ('', '  Test log 0'),
            ('class:log-time', '20210713 00:00:00'),
            ('', '  '),
            ('class:log-level-10', 'DEBUG'),
            ('', '  Test log 1'),
            ('class:log-time', '20210713 00:00:00'),
            ('', '  '),
            ('class:log-level-10', 'DEBUG'),
            ('', '  Test log 2'),
            ('class:selected-log-line class:log-time', '20210713 00:00:00'),
            ('class:selected-log-line ', '  '),
            ('class:selected-log-line class:log-level-10', 'DEBUG'),
            (
                'class:selected-log-line ',
                '  Test log 3                                             ',
            ),
        ]

        # pylint: disable=protected-access
        result_text = join_adjacent_style_tuples(
            flatten_formatted_text_tuples(log_view._line_fragment_cache)
        )
        # pylint: enable=protected-access

        self.assertEqual(result_text, expected_formatted_text)

    def test_clear_scrollback(self) -> None:
        """Test various functions with clearing log scrollback history."""
        # pylint: disable=protected-access
        # Create log_view with 4 logs
        starting_log_count = 4
        log_view, _pane = self._create_log_view_with_logs(
            log_count=starting_log_count
        )
        log_view.render_content()

        # Check setup is correct
        self.assertTrue(log_view.follow)
        self.assertEqual(log_view.get_current_line(), 3)
        self.assertEqual(log_view.get_total_count(), 4)
        self.assertEqual(
            list(
                log.record.message for log in log_view._get_visible_log_lines()
            ),
            ['Test log 0', 'Test log 1', 'Test log 2', 'Test log 3'],
        )

        # Clear scrollback
        log_view.clear_scrollback()
        log_view.render_content()
        # Follow is still on
        self.assertTrue(log_view.follow)
        self.assertEqual(log_view.hidden_line_count(), 4)
        # Current line index should stay the same
        self.assertEqual(log_view.get_current_line(), 3)
        # Total count should stay the same
        self.assertEqual(log_view.get_total_count(), 4)
        # No lines returned
        self.assertEqual(
            list(
                log.record.message for log in log_view._get_visible_log_lines()
            ),
            [],
        )

        # Add Log 4 more lines
        test_log = logging.getLogger('log_view.test')
        with self.assertLogs(test_log, level='DEBUG') as _log_context:
            test_log.addHandler(log_view.log_store)
            for i in range(4):
                test_log.debug('Test log %s', i + starting_log_count)
        log_view.render_content()

        # Current line
        self.assertEqual(log_view.hidden_line_count(), 4)
        self.assertEqual(log_view.get_last_log_index(), 7)
        self.assertEqual(log_view.get_current_line(), 7)
        self.assertEqual(log_view.get_total_count(), 8)
        # Only the last 4 logs should appear
        self.assertEqual(
            list(
                log.record.message for log in log_view._get_visible_log_lines()
            ),
            ['Test log 4', 'Test log 5', 'Test log 6', 'Test log 7'],
        )

        log_view.scroll_to_bottom()
        log_view.render_content()
        self.assertEqual(log_view.get_current_line(), 7)
        # Turn follow back on
        log_view.toggle_follow()

        log_view.undo_clear_scrollback()
        # Current line and total are the same
        self.assertEqual(log_view.get_current_line(), 7)
        self.assertEqual(log_view.get_total_count(), 8)
        # All logs should appear
        self.assertEqual(
            list(
                log.record.message for log in log_view._get_visible_log_lines()
            ),
            [
                'Test log 0',
                'Test log 1',
                'Test log 2',
                'Test log 3',
                'Test log 4',
                'Test log 5',
                'Test log 6',
                'Test log 7',
            ],
        )

        log_view.scroll_to_bottom()
        log_view.render_content()
        self.assertEqual(log_view.get_current_line(), 7)

    def test_get_line_at_cursor_position(self) -> None:
        """Tests fuctions that rely on getting a log_index for the current
        cursor position.

        Including:
        - LogScreen.fetch_subline_up
        - LogScreen.fetch_subline_down
        - LogView._update_log_index
        """
        # pylint: disable=protected-access
        # Create log_view with 4 logs
        starting_log_count = 4
        log_view, _pane = self._create_log_view_with_logs(
            log_count=starting_log_count
        )
        log_view.render_content()

        # Check setup is correct
        self.assertTrue(log_view.follow)
        self.assertEqual(log_view.get_current_line(), 3)
        self.assertEqual(log_view.get_total_count(), 4)
        self.assertEqual(
            list(
                log.record.message for log in log_view._get_visible_log_lines()
            ),
            ['Test log 0', 'Test log 1', 'Test log 2', 'Test log 3'],
        )

        self.assertEqual(log_view.log_screen.cursor_position, 9)
        # Force the cursor_position to be larger than the log_screen
        # line_buffer.
        log_view.log_screen.cursor_position = 10
        # Attempt to get the current line, no exception should be raised
        result = log_view.log_screen.get_line_at_cursor_position()
        # Log index should be None
        self.assertEqual(result.log_index, None)

        # Force the cursor_position to be < 0. This won't produce an error but
        # would wrap around to the beginning.
        log_view.log_screen.cursor_position = -1
        # Attempt to get the current line, no exception should be raised
        result = log_view.log_screen.get_line_at_cursor_position()
        # Result should be a blank line
        self.assertEqual(result, ScreenLine([('', '')]))
        # Log index should be None
        self.assertEqual(result.log_index, None)

    def test_visual_select(self) -> None:
        """Test log line selection."""
        log_view, log_pane = self._create_log_view_with_logs(log_count=100)
        self.assertEqual(100, log_view.get_total_count())

        # Page scrolling needs to know the current window height.
        log_pane.pane_resized = MagicMock(return_value=True)
        log_pane.current_log_pane_width = 80
        log_pane.current_log_pane_height = 10

        log_view.log_screen.reset_logs = MagicMock(
            wraps=log_view.log_screen.reset_logs
        )
        log_view.log_screen.get_lines = MagicMock(
            wraps=log_view.log_screen.get_lines
        )

        log_view.render_content()
        log_view.log_screen.reset_logs.assert_called_once()
        log_view.log_screen.get_lines.assert_called_once_with(
            marked_logs_start=None, marked_logs_end=None
        )
        log_view.log_screen.get_lines.reset_mock()
        log_view.log_screen.reset_logs.reset_mock()

        self.assertIsNone(log_view.marked_logs_start)
        self.assertIsNone(log_view.marked_logs_end)
        log_view.visual_select_line(Point(0, 9))
        self.assertEqual(
            (99, 99), (log_view.marked_logs_start, log_view.marked_logs_end)
        )

        log_view.visual_select_line(Point(0, 8))
        log_view.visual_select_line(Point(0, 7))
        self.assertEqual(
            (97, 99), (log_view.marked_logs_start, log_view.marked_logs_end)
        )

        log_view.clear_visual_selection()
        self.assertIsNone(log_view.marked_logs_start)
        self.assertIsNone(log_view.marked_logs_end)

        log_view.visual_select_line(Point(0, 1))
        log_view.visual_select_line(Point(0, 2))
        log_view.visual_select_line(Point(0, 3))
        log_view.visual_select_line(Point(0, 4))
        self.assertEqual(
            (91, 94), (log_view.marked_logs_start, log_view.marked_logs_end)
        )

        # Make sure the log screen was not re-generated.
        log_view.log_screen.reset_logs.assert_not_called()
        log_view.log_screen.reset_logs.reset_mock()

        # Render the screen
        log_view.render_content()
        log_view.log_screen.reset_logs.assert_called_once()
        # Check the visual selection was specified
        log_view.log_screen.get_lines.assert_called_once_with(
            marked_logs_start=91, marked_logs_end=94
        )
        log_view.log_screen.get_lines.reset_mock()
        log_view.log_screen.reset_logs.reset_mock()


if _PYTHON_3_8:
    from unittest import IsolatedAsyncioTestCase  # type: ignore # pylint: disable=no-name-in-module

    class TestLogViewFiltering(
        IsolatedAsyncioTestCase
    ):  # pylint: disable=undefined-variable
        """Test LogView log filtering capabilities."""

        # pylint: disable=invalid-name
        def setUp(self):
            self.maxDiff = None

        # pylint: enable=invalid-name

        def _create_log_view_from_list(self, log_messages):
            log_view, log_pane = _create_log_view()

            test_log = logging.getLogger('log_view.test')
            with self.assertLogs(test_log, level='DEBUG') as _log_context:
                test_log.addHandler(log_view.log_store)
                for log, extra_arg in log_messages:
                    test_log.debug('%s', log, extra=extra_arg)

            return log_view, log_pane

        @parameterized.expand(
            [
                (
                    # Test name
                    'regex filter',
                    # Search input_text
                    'log.*item',
                    # input_logs
                    [
                        ('Log some item', dict()),
                        ('Log another item', dict()),
                        ('Some exception', dict()),
                    ],
                    # expected_matched_lines
                    [
                        'Log some item',
                        'Log another item',
                    ],
                    # expected_match_line_numbers
                    {0: 0, 1: 1},
                    # expected_export_text
                    ('  DEBUG  Log some item\n  DEBUG  Log another item\n'),
                    None,  # field
                    False,  # invert
                ),
                (
                    # Test name
                    'regex filter with field',
                    # Search input_text
                    'earth',
                    # input_logs
                    [
                        (
                            'Log some item',
                            dict(extra_metadata_fields={'planet': 'Jupiter'}),
                        ),
                        (
                            'Log another item',
                            dict(extra_metadata_fields={'planet': 'Earth'}),
                        ),
                        (
                            'Some exception',
                            dict(extra_metadata_fields={'planet': 'Earth'}),
                        ),
                    ],
                    # expected_matched_lines
                    [
                        'Log another item',
                        'Some exception',
                    ],
                    # expected_match_line_numbers
                    {1: 0, 2: 1},
                    # expected_export_text
                    (
                        '  DEBUG  Earth    Log another item\n'
                        '  DEBUG  Earth    Some exception\n'
                    ),
                    'planet',  # field
                    False,  # invert
                ),
                (
                    # Test name
                    'regex filter with field inverted',
                    # Search input_text
                    'earth',
                    # input_logs
                    [
                        (
                            'Log some item',
                            dict(extra_metadata_fields={'planet': 'Jupiter'}),
                        ),
                        (
                            'Log another item',
                            dict(extra_metadata_fields={'planet': 'Earth'}),
                        ),
                        (
                            'Some exception',
                            dict(extra_metadata_fields={'planet': 'Earth'}),
                        ),
                    ],
                    # expected_matched_lines
                    [
                        'Log some item',
                    ],
                    # expected_match_line_numbers
                    {0: 0},
                    # expected_export_text
                    ('  DEBUG  Jupiter  Log some item\n'),
                    'planet',  # field
                    True,  # invert
                ),
            ]
        )
        async def test_log_filtering(
            self,
            _test_name,
            input_text,
            input_logs,
            expected_matched_lines,
            expected_match_line_numbers,
            expected_export_text,
            field=None,
            invert=False,
        ) -> None:
            """Test run log view filtering."""
            log_view, _log_pane = self._create_log_view_from_list(input_logs)
            log_view.render_content()

            self.assertEqual(log_view.get_total_count(), len(input_logs))
            # Apply the search and wait for the match count background task
            log_view.new_search(input_text, invert=invert, field=field)
            await log_view.search_match_count_task
            self.assertEqual(
                log_view.search_matched_lines, expected_match_line_numbers
            )

            # Apply the filter and wait for the filter background task
            log_view.apply_filter()
            await log_view.filter_existing_logs_task

            # Do the number of logs match the expected count?
            self.assertEqual(
                log_view.get_total_count(), len(expected_matched_lines)
            )
            self.assertEqual(
                [log.record.message for log in log_view.filtered_logs],
                expected_matched_lines,
            )

            # Check exported text respects filtering
            log_text = (
                log_view._logs_to_text(  # pylint: disable=protected-access
                    use_table_formatting=True
                )
            )
            # Remove leading time from resulting logs
            log_text_no_datetime = ''
            for line in log_text.splitlines():
                log_text_no_datetime += line[17:] + '\n'
            self.assertEqual(log_text_no_datetime, expected_export_text)

            # Select the bottom log line
            log_view.render_content()
            log_view.visual_select_line(Point(0, 9))  # Window height is 10
            # Export to text
            log_text = (
                log_view._logs_to_text(  # pylint: disable=protected-access
                    selected_lines_only=True, use_table_formatting=False
                )
            )
            self.assertEqual(
                # Remove date, time, and level
                log_text[24:].strip(),
                expected_matched_lines[0].strip(),
            )

            # Clear filters and check the numbe of lines is back to normal.
            log_view.clear_filters()
            self.assertEqual(log_view.get_total_count(), len(input_logs))


if __name__ == '__main__':
    unittest.main()
