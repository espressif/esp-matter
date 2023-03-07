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
"""Tests for pw_console.console_app"""

import logging
import unittest
from unittest.mock import MagicMock

from prompt_toolkit.application import create_app_session
from prompt_toolkit.output import ColorDepth

# inclusive-language: ignore
from prompt_toolkit.output import DummyOutput as FakeOutput

from pw_console.console_app import ConsoleApp
from pw_console.console_prefs import ConsolePrefs
from pw_console.window_manager import _WINDOW_SPLIT_ADJUST
from pw_console.window_list import _WINDOW_HEIGHT_ADJUST, DisplayMode


def _create_console_app(logger_count=2):
    prefs = ConsolePrefs(
        project_file=False, project_user_file=False, user_file=False
    )
    prefs.set_code_theme('default')
    console_app = ConsoleApp(color_depth=ColorDepth.DEPTH_8_BIT, prefs=prefs)
    console_app.focus_on_container = MagicMock()

    loggers = {}
    for i in range(logger_count):
        loggers['Log{}'.format(i)] = [logging.getLogger('test_log{}'.format(i))]
    for window_title, logger_instances in loggers.items():
        console_app.add_log_handler(window_title, logger_instances)
    return console_app


_WINDOW_MANAGER_WIDTH = 80
_WINDOW_MANAGER_HEIGHT = 30
_DEFAULT_WINDOW_WIDTH = 10
_DEFAULT_WINDOW_HEIGHT = 10


def _window_list_widths(window_manager):
    window_manager.update_window_manager_size(
        _WINDOW_MANAGER_WIDTH, _WINDOW_MANAGER_HEIGHT
    )

    return [
        window_list.width.preferred
        for window_list in window_manager.window_lists
    ]


def _window_list_heights(window_manager):
    window_manager.update_window_manager_size(
        _WINDOW_MANAGER_WIDTH, _WINDOW_MANAGER_HEIGHT
    )

    return [
        window_list.height.preferred
        for window_list in window_manager.window_lists
    ]


def _window_pane_widths(window_manager, window_list_index=0):
    window_manager.update_window_manager_size(
        _WINDOW_MANAGER_WIDTH, _WINDOW_MANAGER_HEIGHT
    )

    return [
        pane.width.preferred
        for pane in window_manager.window_lists[window_list_index].active_panes
    ]


def _window_pane_heights(window_manager, window_list_index=0):
    window_manager.update_window_manager_size(
        _WINDOW_MANAGER_WIDTH, _WINDOW_MANAGER_HEIGHT
    )

    return [
        pane.height.preferred
        for pane in window_manager.window_lists[window_list_index].active_panes
    ]


def _window_pane_counts(window_manager):
    return [
        len(window_list.active_panes)
        for window_list in window_manager.window_lists
    ]


def window_pane_titles(window_manager):
    return [
        [
            pane.pane_title() + ' - ' + pane.pane_subtitle()
            for pane in window_list.active_panes
        ]
        for window_list in window_manager.window_lists
    ]


def target_list_and_pane(window_manager, list_index, pane_index):
    # pylint: disable=protected-access
    # Bypass prompt_toolkit has_focus()
    pane = window_manager.window_lists[list_index].active_panes[pane_index]
    # If the pane is in focus it will be visible.
    pane.show_pane = True
    window_manager._get_active_window_list_and_pane = MagicMock(  # type: ignore
        return_value=(
            window_manager.window_lists[list_index],
            window_manager.window_lists[list_index].active_panes[pane_index],
        )
    )


class TestWindowManager(unittest.TestCase):
    # pylint: disable=protected-access
    """Tests for window management functions."""

    def setUp(self):
        self.maxDiff = None  # pylint: disable=invalid-name

    def test_find_window_list_and_pane(self) -> None:
        """Test getting the window list for a given pane."""
        with create_app_session(output=FakeOutput()):
            console_app = _create_console_app(logger_count=3)

            window_manager = console_app.window_manager
            self.assertEqual([4], _window_pane_counts(window_manager))

            # Move 2 windows to the right into their own splits
            target_list_and_pane(window_manager, 0, 0)
            window_manager.move_pane_right()
            target_list_and_pane(window_manager, 0, 0)
            window_manager.move_pane_right()
            target_list_and_pane(window_manager, 1, 0)
            window_manager.move_pane_right()
            # 3 splits, first split has 2 windows
            self.assertEqual([2, 1, 1], _window_pane_counts(window_manager))

            # Move the first window in the first split left
            target_list_and_pane(window_manager, 0, 0)
            window_manager.move_pane_left()
            # 4 splits, each with their own window
            self.assertEqual([1, 1, 1, 1], _window_pane_counts(window_manager))

            # Move the first window to the right
            target_list_and_pane(window_manager, 0, 0)
            window_manager.move_pane_right()
            # 3 splits, first split has 2 windows
            self.assertEqual([2, 1, 1], _window_pane_counts(window_manager))

            target_pane = window_manager.window_lists[2].active_panes[0]

            (
                result_window_list,
                result_pane_index,
            ) = window_manager._find_window_list_and_pane_index(target_pane)
            self.assertEqual(
                (result_window_list, result_pane_index),
                (window_manager.window_lists[2], 0),
            )
            window_manager.remove_pane(target_pane)
            self.assertEqual([2, 1], _window_pane_counts(window_manager))

    def test_window_list_moving_and_resizing(self) -> None:
        """Test window split movement resizing."""
        with create_app_session(output=FakeOutput()):
            console_app = _create_console_app(logger_count=3)

            window_manager = console_app.window_manager

            target_list_and_pane(window_manager, 0, 0)
            # Should have one window list split of size 50.
            self.assertEqual(
                _window_list_widths(window_manager),
                [_WINDOW_MANAGER_WIDTH],
            )

            # Move one pane to the right, creating a new window_list split.
            window_manager.move_pane_right()

            self.assertEqual(
                _window_list_widths(window_manager),
                [
                    int(_WINDOW_MANAGER_WIDTH / 2),
                    int(_WINDOW_MANAGER_WIDTH / 2),
                ],
            )

            # Move another pane to the right twice, creating a third
            # window_list split.
            target_list_and_pane(window_manager, 0, 0)
            window_manager.move_pane_right()

            # Above window pane is at a new location
            target_list_and_pane(window_manager, 1, 0)
            window_manager.move_pane_right()

            # Should have 3 splits now
            self.assertEqual(
                _window_list_widths(window_manager),
                [
                    int(_WINDOW_MANAGER_WIDTH / 3),
                    int(_WINDOW_MANAGER_WIDTH / 3),
                    int(_WINDOW_MANAGER_WIDTH / 3),
                ],
            )

            # Total of 4 active panes
            self.assertEqual(len(list(window_manager.active_panes())), 4)

            # Target the middle split
            target_list_and_pane(window_manager, 1, 0)
            # Shrink the middle split twice
            window_manager.shrink_split()
            window_manager.shrink_split()
            self.assertEqual(
                _window_list_widths(window_manager),
                [
                    int(_WINDOW_MANAGER_WIDTH / 3),
                    int(_WINDOW_MANAGER_WIDTH / 3) - (2 * _WINDOW_SPLIT_ADJUST),
                    int(_WINDOW_MANAGER_WIDTH / 3) + (2 * _WINDOW_SPLIT_ADJUST),
                ],
            )

            # Target the first split
            target_list_and_pane(window_manager, 0, 0)
            window_manager.reset_split_sizes()
            # Shrink the first split twice
            window_manager.shrink_split()
            self.assertEqual(
                _window_list_widths(window_manager),
                [
                    int(_WINDOW_MANAGER_WIDTH / 3) - (1 * _WINDOW_SPLIT_ADJUST),
                    int(_WINDOW_MANAGER_WIDTH / 3) + (1 * _WINDOW_SPLIT_ADJUST),
                    int(_WINDOW_MANAGER_WIDTH / 3),
                ],
            )

            # Target the third (last) split
            target_list_and_pane(window_manager, 2, 0)
            window_manager.reset_split_sizes()
            # Shrink the third split once
            window_manager.shrink_split()
            self.assertEqual(
                _window_list_widths(window_manager),
                [
                    int(_WINDOW_MANAGER_WIDTH / 3),
                    int(_WINDOW_MANAGER_WIDTH / 3) + (1 * _WINDOW_SPLIT_ADJUST),
                    int(_WINDOW_MANAGER_WIDTH / 3) - (1 * _WINDOW_SPLIT_ADJUST),
                ],
            )

            window_manager.reset_split_sizes()
            # Enlarge the third split a few times.
            window_manager.enlarge_split()
            window_manager.enlarge_split()
            window_manager.enlarge_split()
            self.assertEqual(
                _window_list_widths(window_manager),
                [
                    int(_WINDOW_MANAGER_WIDTH / 3),
                    int(_WINDOW_MANAGER_WIDTH / 3) - (3 * _WINDOW_SPLIT_ADJUST),
                    int(_WINDOW_MANAGER_WIDTH / 3) + (3 * _WINDOW_SPLIT_ADJUST),
                ],
            )

            # Target the middle split
            target_list_and_pane(window_manager, 1, 0)
            # Move the middle window pane left
            window_manager.move_pane_left()
            # This is called on the next render pass
            window_manager.rebalance_window_list_sizes()
            # Middle split should be removed
            self.assertEqual(
                _window_list_widths(window_manager),
                [
                    int(_WINDOW_MANAGER_WIDTH / 2) - (3 * _WINDOW_SPLIT_ADJUST),
                    # This split is removed
                    int(_WINDOW_MANAGER_WIDTH / 2) + (2 * _WINDOW_SPLIT_ADJUST),
                ],
            )

            # Revert sizes to default
            window_manager.reset_split_sizes()
            self.assertEqual(
                _window_list_widths(window_manager),
                [
                    int(_WINDOW_MANAGER_WIDTH / 2),
                    int(_WINDOW_MANAGER_WIDTH / 2),
                ],
            )

    def test_get_pane_titles(self) -> None:
        """Test window resizing."""
        with create_app_session(output=FakeOutput()):
            console_app = _create_console_app(logger_count=3)

            window_manager = console_app.window_manager
            list_pane_titles = [
                # Remove mouse click handler partials in tup[2]
                [(tup[0], tup[1]) for tup in window_list.get_pane_titles()]
                for window_list in window_manager.window_lists
            ]
            self.assertEqual(
                list_pane_titles[0],
                [
                    ('', ' '),
                    ('class:window-tab-inactive', ' Log2 test_log2 '),
                    ('', ' '),
                    ('class:window-tab-inactive', ' Log1 test_log1 '),
                    ('', ' '),
                    ('class:window-tab-inactive', ' Log0 test_log0 '),
                    ('', ' '),
                    ('class:window-tab-inactive', ' Python Repl  '),
                    ('', ' '),
                ],
            )

    def test_window_pane_movement_resizing(self) -> None:
        """Test window resizing."""
        with create_app_session(output=FakeOutput()):
            console_app = _create_console_app(logger_count=3)

            window_manager = console_app.window_manager

            # 4 panes, 3 for the loggers and 1 for the repl.
            self.assertEqual(
                len(window_manager.first_window_list().active_panes), 4
            )

            def target_window_pane(index: int):
                # Bypass prompt_toolkit has_focus()
                window_manager._get_active_window_list_and_pane = (
                    MagicMock(  # type: ignore
                        return_value=(
                            window_manager.window_lists[0],
                            window_manager.window_lists[0].active_panes[index],
                        )
                    )
                )
                window_list = console_app.window_manager.first_window_list()
                window_list.get_current_active_pane = MagicMock(  # type: ignore
                    return_value=window_list.active_panes[index]
                )

            # Target the first window pane
            target_window_pane(0)

            # Shrink the first pane
            window_manager.shrink_pane()
            self.assertEqual(
                _window_pane_heights(window_manager),
                [
                    _DEFAULT_WINDOW_HEIGHT - (1 * _WINDOW_HEIGHT_ADJUST),
                    _DEFAULT_WINDOW_HEIGHT + (1 * _WINDOW_HEIGHT_ADJUST),
                    _DEFAULT_WINDOW_HEIGHT,
                    _DEFAULT_WINDOW_HEIGHT,
                ],
            )

            # Reset pane sizes
            window_manager.window_lists[0].current_window_list_height = (
                4 * _DEFAULT_WINDOW_HEIGHT
            )
            window_manager.reset_pane_sizes()
            self.assertEqual(
                _window_pane_heights(window_manager),
                [
                    _DEFAULT_WINDOW_HEIGHT,
                    _DEFAULT_WINDOW_HEIGHT,
                    _DEFAULT_WINDOW_HEIGHT,
                    _DEFAULT_WINDOW_HEIGHT,
                ],
            )

            # Shrink last pane
            target_window_pane(3)

            window_manager.shrink_pane()
            self.assertEqual(
                _window_pane_heights(window_manager),
                [
                    _DEFAULT_WINDOW_HEIGHT,
                    _DEFAULT_WINDOW_HEIGHT,
                    _DEFAULT_WINDOW_HEIGHT + (1 * _WINDOW_HEIGHT_ADJUST),
                    _DEFAULT_WINDOW_HEIGHT - (1 * _WINDOW_HEIGHT_ADJUST),
                ],
            )

            # Enlarge second pane
            target_window_pane(1)
            window_manager.reset_pane_sizes()

            window_manager.enlarge_pane()
            window_manager.enlarge_pane()
            self.assertEqual(
                _window_pane_heights(window_manager),
                [
                    _DEFAULT_WINDOW_HEIGHT,
                    _DEFAULT_WINDOW_HEIGHT + (2 * _WINDOW_HEIGHT_ADJUST),
                    _DEFAULT_WINDOW_HEIGHT - (2 * _WINDOW_HEIGHT_ADJUST),
                    _DEFAULT_WINDOW_HEIGHT,
                ],
            )

            # Check window pane ordering
            self.assertEqual(
                window_pane_titles(window_manager),
                [
                    [
                        'Log2 - test_log2',
                        'Log1 - test_log1',
                        'Log0 - test_log0',
                        'Python Repl - ',
                    ],
                ],
            )

            target_window_pane(0)
            window_manager.move_pane_down()
            self.assertEqual(
                window_pane_titles(window_manager),
                [
                    [
                        'Log1 - test_log1',
                        'Log2 - test_log2',
                        'Log0 - test_log0',
                        'Python Repl - ',
                    ],
                ],
            )
            target_window_pane(2)
            window_manager.move_pane_up()
            target_window_pane(1)
            window_manager.move_pane_up()
            self.assertEqual(
                window_pane_titles(window_manager),
                [
                    [
                        'Log0 - test_log0',
                        'Log1 - test_log1',
                        'Log2 - test_log2',
                        'Python Repl - ',
                    ],
                ],
            )
            target_window_pane(0)
            window_manager.move_pane_up()
            self.assertEqual(
                window_pane_titles(window_manager),
                [
                    [
                        'Log0 - test_log0',
                        'Log1 - test_log1',
                        'Log2 - test_log2',
                        'Python Repl - ',
                    ],
                ],
            )

    def test_focus_next_and_previous_pane(self) -> None:
        """Test switching focus to next and previous window panes."""
        with create_app_session(output=FakeOutput()):
            console_app = _create_console_app(logger_count=4)

            window_manager = console_app.window_manager
            window_manager.window_lists[0].set_display_mode(DisplayMode.STACK)
            self.assertEqual(
                window_pane_titles(window_manager),
                [
                    [
                        'Log3 - test_log3',
                        'Log2 - test_log2',
                        'Log1 - test_log1',
                        'Log0 - test_log0',
                        'Python Repl - ',
                    ],
                ],
            )

            # Scenario: Move between panes with a single stacked window list.

            # Set the first pane in focus.
            target_list_and_pane(window_manager, 0, 0)
            # Switch focus to the next pane
            window_manager.focus_next_pane()
            # Pane index 1 should now be focused.
            console_app.focus_on_container.assert_called_once_with(
                window_manager.window_lists[0].active_panes[1]
            )
            console_app.focus_on_container.reset_mock()

            # Set the first pane in focus.
            target_list_and_pane(window_manager, 0, 0)
            # Switch focus to the previous pane
            window_manager.focus_previous_pane()
            # Previous pane should wrap around to the last pane in the first
            # window_list.
            console_app.focus_on_container.assert_called_once_with(
                window_manager.window_lists[0].active_panes[-1]
            )
            console_app.focus_on_container.reset_mock()

            # Set the last pane in focus.
            target_list_and_pane(window_manager, 0, 4)
            # Switch focus to the next pane
            window_manager.focus_next_pane()
            # Next pane should wrap around to the first pane in the first
            # window_list.
            console_app.focus_on_container.assert_called_once_with(
                window_manager.window_lists[0].active_panes[0]
            )
            console_app.focus_on_container.reset_mock()

            # Scenario: Move between panes with a single tabbed window list.

            # Switch to Tabbed view mode
            window_manager.window_lists[0].set_display_mode(DisplayMode.TABBED)
            # The set_display_mode call above will call focus_on_container once.
            console_app.focus_on_container.reset_mock()

            # Setup the switch_to_tab mock
            window_manager.window_lists[0].switch_to_tab = MagicMock(
                wraps=window_manager.window_lists[0].switch_to_tab
            )

            # Set the first pane/tab in focus.
            target_list_and_pane(window_manager, 0, 0)
            # Switch focus to the next pane/tab
            window_manager.focus_next_pane()
            # Check switch_to_tab is called
            window_manager.window_lists[
                0
            ].switch_to_tab.assert_called_once_with(1)
            # And that focus_on_container is called only once
            console_app.focus_on_container.assert_called_once_with(
                window_manager.window_lists[0].active_panes[1]
            )
            console_app.focus_on_container.reset_mock()
            window_manager.window_lists[0].switch_to_tab.reset_mock()

            # Set the last pane/tab in focus.
            target_list_and_pane(window_manager, 0, 4)
            # Switch focus to the next pane/tab
            window_manager.focus_next_pane()
            # Check switch_to_tab is called
            window_manager.window_lists[
                0
            ].switch_to_tab.assert_called_once_with(0)
            # And that focus_on_container is called only once
            console_app.focus_on_container.assert_called_once_with(
                window_manager.window_lists[0].active_panes[0]
            )
            console_app.focus_on_container.reset_mock()
            window_manager.window_lists[0].switch_to_tab.reset_mock()

            # Set the first pane/tab in focus.
            target_list_and_pane(window_manager, 0, 0)
            # Switch focus to the prev pane/tab
            window_manager.focus_previous_pane()
            # Check switch_to_tab is called
            window_manager.window_lists[
                0
            ].switch_to_tab.assert_called_once_with(4)
            # And that focus_on_container is called only once
            console_app.focus_on_container.assert_called_once_with(
                window_manager.window_lists[0].active_panes[4]
            )
            console_app.focus_on_container.reset_mock()
            window_manager.window_lists[0].switch_to_tab.reset_mock()

            # Scenario: Move between multiple window lists with mixed stacked
            # and tabbed view modes.

            # Setup: Move two panes to the right into their own stacked
            # window_list.
            target_list_and_pane(window_manager, 0, 4)
            window_manager.move_pane_right()
            target_list_and_pane(window_manager, 0, 3)
            window_manager.move_pane_right()
            self.assertEqual(
                window_pane_titles(window_manager),
                [
                    [
                        'Log3 - test_log3',
                        'Log2 - test_log2',
                        'Log1 - test_log1',
                    ],
                    [
                        'Log0 - test_log0',
                        'Python Repl - ',
                    ],
                ],
            )

            # Setup the switch_to_tab mock on the second window_list
            window_manager.window_lists[1].switch_to_tab = MagicMock(
                wraps=window_manager.window_lists[1].switch_to_tab
            )

            # Set Log1 in focus
            target_list_and_pane(window_manager, 0, 2)
            window_manager.focus_next_pane()
            # Log0 should now have focus
            console_app.focus_on_container.assert_called_once_with(
                window_manager.window_lists[1].active_panes[0]
            )
            console_app.focus_on_container.reset_mock()

            # Set Log0 in focus
            target_list_and_pane(window_manager, 1, 0)
            window_manager.focus_previous_pane()
            # Log1 should now have focus
            console_app.focus_on_container.assert_called_once_with(
                window_manager.window_lists[0].active_panes[2]
            )
            # The first window list is in tabbed mode so switch_to_tab should be
            # called once.
            window_manager.window_lists[
                0
            ].switch_to_tab.assert_called_once_with(2)
            # Reset
            window_manager.window_lists[0].switch_to_tab.reset_mock()
            console_app.focus_on_container.reset_mock()

            # Set Python Repl in focus
            target_list_and_pane(window_manager, 1, 1)
            window_manager.focus_next_pane()
            # Log3 should now have focus
            console_app.focus_on_container.assert_called_once_with(
                window_manager.window_lists[0].active_panes[0]
            )
            window_manager.window_lists[
                0
            ].switch_to_tab.assert_called_once_with(0)
            # Reset
            window_manager.window_lists[0].switch_to_tab.reset_mock()
            console_app.focus_on_container.reset_mock()

            # Set Log3 in focus
            target_list_and_pane(window_manager, 0, 0)
            window_manager.focus_next_pane()
            # Log2 should now have focus
            console_app.focus_on_container.assert_called_once_with(
                window_manager.window_lists[0].active_panes[1]
            )
            window_manager.window_lists[
                0
            ].switch_to_tab.assert_called_once_with(1)
            # Reset
            window_manager.window_lists[0].switch_to_tab.reset_mock()
            console_app.focus_on_container.reset_mock()

            # Set Python Repl in focus
            target_list_and_pane(window_manager, 1, 1)
            window_manager.focus_previous_pane()
            # Log0 should now have focus
            console_app.focus_on_container.assert_called_once_with(
                window_manager.window_lists[1].active_panes[0]
            )
            # The second window list is in stacked mode so switch_to_tab should
            # not be called.
            window_manager.window_lists[1].switch_to_tab.assert_not_called()
            # Reset
            window_manager.window_lists[1].switch_to_tab.reset_mock()
            console_app.focus_on_container.reset_mock()

    def test_resize_vertical_splits(self) -> None:
        """Test resizing window splits."""
        with create_app_session(output=FakeOutput()):
            console_app = _create_console_app(logger_count=4)
            window_manager = console_app.window_manager

            # Required before moving windows
            window_manager.update_window_manager_size(
                _WINDOW_MANAGER_WIDTH, _WINDOW_MANAGER_HEIGHT
            )
            window_manager.create_root_container()

            # Vertical split by default
            self.assertTrue(window_manager.vertical_window_list_spliting())

            # Move windows to create 3 splits
            target_list_and_pane(window_manager, 0, 0)
            window_manager.move_pane_right()
            target_list_and_pane(window_manager, 0, 0)
            window_manager.move_pane_right()
            target_list_and_pane(window_manager, 1, 1)
            window_manager.move_pane_right()

            # Check windows are where expected
            self.assertEqual(
                window_pane_titles(window_manager),
                [
                    [
                        'Log1 - test_log1',
                        'Log0 - test_log0',
                        'Python Repl - ',
                    ],
                    [
                        'Log2 - test_log2',
                    ],
                    [
                        'Log3 - test_log3',
                    ],
                ],
            )

            # Check initial split widths
            widths = [
                int(_WINDOW_MANAGER_WIDTH / 3),
                int(_WINDOW_MANAGER_WIDTH / 3),
                int(_WINDOW_MANAGER_WIDTH / 3),
            ]
            self.assertEqual(_window_list_widths(window_manager), widths)

            # Decrease size of first split
            window_manager.adjust_split_size(window_manager.window_lists[0], -4)
            widths = [
                widths[0] - (4 * _WINDOW_SPLIT_ADJUST),
                widths[1] + (4 * _WINDOW_SPLIT_ADJUST),
                widths[2],
            ]
            self.assertEqual(_window_list_widths(window_manager), widths)

            # Increase size of last split
            widths = [
                widths[0],
                widths[1] - (4 * _WINDOW_SPLIT_ADJUST),
                widths[2] + (4 * _WINDOW_SPLIT_ADJUST),
            ]
            window_manager.adjust_split_size(window_manager.window_lists[2], 4)
            self.assertEqual(_window_list_widths(window_manager), widths)

            # Check heights are all the same
            window_manager.rebalance_window_list_sizes()
            heights = [
                int(_WINDOW_MANAGER_HEIGHT),
                int(_WINDOW_MANAGER_HEIGHT),
                int(_WINDOW_MANAGER_HEIGHT),
            ]
            self.assertEqual(_window_list_heights(window_manager), heights)

    def test_resize_horizontal_splits(self) -> None:
        """Test resizing window splits."""
        with create_app_session(output=FakeOutput()):
            console_app = _create_console_app(logger_count=4)
            window_manager = console_app.window_manager

            # We want horizontal window splits
            window_manager.vertical_window_list_spliting = MagicMock(
                return_value=False
            )
            self.assertFalse(window_manager.vertical_window_list_spliting())

            # Required before moving windows
            window_manager.update_window_manager_size(
                _WINDOW_MANAGER_WIDTH, _WINDOW_MANAGER_HEIGHT
            )
            window_manager.create_root_container()

            # Move windows to create 3 splits
            target_list_and_pane(window_manager, 0, 0)
            window_manager.move_pane_right()
            target_list_and_pane(window_manager, 0, 0)
            window_manager.move_pane_right()
            target_list_and_pane(window_manager, 1, 1)
            window_manager.move_pane_right()

            # Check windows are where expected
            self.assertEqual(
                window_pane_titles(window_manager),
                [
                    [
                        'Log1 - test_log1',
                        'Log0 - test_log0',
                        'Python Repl - ',
                    ],
                    [
                        'Log2 - test_log2',
                    ],
                    [
                        'Log3 - test_log3',
                    ],
                ],
            )

            # Check initial split widths
            heights = [
                int(_WINDOW_MANAGER_HEIGHT / 3),
                int(_WINDOW_MANAGER_HEIGHT / 3),
                int(_WINDOW_MANAGER_HEIGHT / 3),
            ]
            self.assertEqual(_window_list_heights(window_manager), heights)

            # Decrease size of first split
            window_manager.adjust_split_size(window_manager.window_lists[0], -4)
            heights = [
                heights[0] - (4 * _WINDOW_SPLIT_ADJUST),
                heights[1] + (4 * _WINDOW_SPLIT_ADJUST),
                heights[2],
            ]
            self.assertEqual(_window_list_heights(window_manager), heights)

            # Increase size of last split
            heights = [
                heights[0],
                heights[1] - (4 * _WINDOW_SPLIT_ADJUST),
                heights[2] + (4 * _WINDOW_SPLIT_ADJUST),
            ]
            window_manager.adjust_split_size(window_manager.window_lists[2], 4)
            self.assertEqual(_window_list_heights(window_manager), heights)

            # Check widths are all the same
            window_manager.rebalance_window_list_sizes()
            widths = [
                int(_WINDOW_MANAGER_WIDTH),
                int(_WINDOW_MANAGER_WIDTH),
                int(_WINDOW_MANAGER_WIDTH),
            ]
            self.assertEqual(_window_list_widths(window_manager), widths)


if __name__ == '__main__':
    unittest.main()
