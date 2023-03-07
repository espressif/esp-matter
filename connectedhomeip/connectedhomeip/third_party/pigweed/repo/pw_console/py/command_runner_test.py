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
"""Tests for pw_console.command_runner dialog."""

import logging
import re
import unittest
from typing import Callable, List, Tuple

from unittest.mock import MagicMock

from prompt_toolkit.application import create_app_session
from prompt_toolkit.output import ColorDepth

# inclusive-language: ignore
from prompt_toolkit.output import DummyOutput as FakeOutput

from pw_console.console_app import ConsoleApp
from pw_console.console_prefs import ConsolePrefs
from pw_console.text_formatting import (
    flatten_formatted_text_tuples,
    join_adjacent_style_tuples,
)


def _create_console_app(log_pane_count=2):
    prefs = ConsolePrefs(
        project_file=False, project_user_file=False, user_file=False
    )
    prefs.set_code_theme('default')
    console_app = ConsoleApp(color_depth=ColorDepth.DEPTH_8_BIT, prefs=prefs)

    console_app.prefs.reset_config()

    # Setup log panes
    loggers = {}
    for i in range(log_pane_count):
        loggers['LogPane-{}'.format(i)] = [
            logging.getLogger('test_log{}'.format(i))
        ]
    for window_title, logger_instances in loggers.items():
        console_app.add_log_handler(window_title, logger_instances)

    return console_app


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


class TestCommandRunner(unittest.TestCase):
    """Tests for CommandRunner."""

    def setUp(self):
        self.maxDiff = None  # pylint: disable=invalid-name

    def test_flatten_menu_items(self) -> None:
        with create_app_session(output=FakeOutput()):
            console_app = _create_console_app(log_pane_count=2)
            flattened_menu_items = [
                text
                # pylint: disable=line-too-long
                for text, handler in console_app.command_runner.load_menu_items()
                # pylint: enable=line-too-long
            ]

            # Check some common menu items exist.
            self.assertIn('[File] > Open Logger', flattened_menu_items)
            self.assertIn(
                '[File] > Themes > UI Themes > High Contrast',
                flattened_menu_items,
            )
            self.assertIn('[Help] > User Guide', flattened_menu_items)
            self.assertIn('[Help] > Keyboard Shortcuts', flattened_menu_items)
            # Check for log windows
            self.assertRegex(
                '\n'.join(flattened_menu_items),
                re.compile(
                    r'^\[Windows\] > .* LogPane-[0-9]+ > .*$', re.MULTILINE
                ),
            )

    def test_filter_and_highlight_matches(self) -> None:
        """Check filtering matches and highlighting works correctly."""
        with create_app_session(output=FakeOutput()):
            console_app = _create_console_app(log_pane_count=2)
            command_runner = console_app.command_runner

            command_runner.filter_completions = MagicMock(
                wraps=command_runner.filter_completions
            )
            command_runner.width = 20

            # Define custom completion items
            def empty_handler() -> None:
                return None

            def get_completions() -> List[Tuple[str, Callable]]:
                return [
                    ('[File] > Open Logger', empty_handler),
                    ('[Windows] > 1: Host Logs > Show/Hide', empty_handler),
                    ('[Windows] > 2: Device Logs > Show/Hide', empty_handler),
                    ('[Help] > User Guide', empty_handler),
                ]

            command_runner.filter_completions.assert_not_called()
            command_runner.set_completions(
                window_title='Test Completions',
                load_completions=get_completions,
            )
            command_runner.filter_completions.assert_called_once()
            command_runner.filter_completions.reset_mock()

            # Input field should be empty
            self.assertEqual(command_runner.input_field.buffer.text, '')
            # Flatten resulting formatted text
            result_items = join_adjacent_style_tuples(
                flatten_formatted_text_tuples(
                    command_runner.completion_fragments
                )
            )

            # index 0: the selected line
            # index 1: the rest of the completions with line breaks
            self.assertEqual(len(result_items), 2)
            first_item_style = result_items[0][0]
            first_item_text = result_items[0][1]
            second_item_text = result_items[1][1]
            # Check expected number of lines are present
            self.assertEqual(len(first_item_text.splitlines()), 1)
            self.assertEqual(len(second_item_text.splitlines()), 3)
            # First line is highlighted as a selected item
            self.assertEqual(
                first_item_style, 'class:command-runner-selected-item'
            )
            self.assertIn('[File] > Open Logger', first_item_text)

            # Type: file open
            command_runner.input_field.buffer.text = 'file open'
            self.assertEqual(
                command_runner.input_field.buffer.text, 'file open'
            )
            # Run the filter
            command_runner.filter_completions()
            # Flatten resulting formatted text
            result_items = join_adjacent_style_tuples(
                flatten_formatted_text_tuples(
                    command_runner.completion_fragments
                )
            )
            # Check file and open are highlighted
            self.assertEqual(
                result_items[:4],
                [
                    ('class:command-runner-selected-item', '['),
                    (
                        'class:command-runner-selected-item '
                        'class:command-runner-fuzzy-highlight-0 ',
                        'File',
                    ),
                    ('class:command-runner-selected-item', '] > '),
                    (
                        'class:command-runner-selected-item '
                        'class:command-runner-fuzzy-highlight-1 ',
                        'Open',
                    ),
                ],
            )

            # Type: open file
            command_runner.input_field.buffer.text = 'open file'
            # Run the filter
            command_runner.filter_completions()
            result_items = join_adjacent_style_tuples(
                flatten_formatted_text_tuples(
                    command_runner.completion_fragments
                )
            )
            # Check file and open are highlighted, the fuzzy-highlight class
            # should be swapped.
            self.assertEqual(
                result_items[:4],
                [
                    ('class:command-runner-selected-item', '['),
                    (
                        'class:command-runner-selected-item '
                        'class:command-runner-fuzzy-highlight-1 ',
                        'File',
                    ),
                    ('class:command-runner-selected-item', '] > '),
                    (
                        'class:command-runner-selected-item '
                        'class:command-runner-fuzzy-highlight-0 ',
                        'Open',
                    ),
                ],
            )

            # Clear input
            command_runner._reset_selected_item()  # pylint: disable=protected-access
            command_runner.filter_completions()
            result_items = join_adjacent_style_tuples(
                flatten_formatted_text_tuples(
                    command_runner.completion_fragments
                )
            )
            self.assertEqual(len(first_item_text.splitlines()), 1)
            self.assertEqual(len(second_item_text.splitlines()), 3)

            # Press down (select the next item)
            command_runner._next_item()  # pylint: disable=protected-access
            # Filter and check results
            command_runner.filter_completions()
            result_items = join_adjacent_style_tuples(
                flatten_formatted_text_tuples(
                    command_runner.completion_fragments
                )
            )
            self.assertEqual(len(result_items), 3)
            # First line - not selected
            self.assertEqual(result_items[0], ('', '[File] > Open Logger\n'))
            # Second line - is selected
            self.assertEqual(
                result_items[1],
                (
                    'class:command-runner-selected-item',
                    '[Windows] > 1: Host Logs > Show/Hide\n',
                ),
            )
            # Third and fourth lines separated by \n - not selected
            self.assertEqual(
                result_items[2],
                (
                    '',
                    '[Windows] > 2: Device Logs > Show/Hide\n'
                    '[Help] > User Guide',
                ),
            )

    def test_run_action(self) -> None:
        """Check running an action works correctly."""
        with create_app_session(output=FakeOutput()):
            console_app = _create_console_app(log_pane_count=2)
            command_runner = console_app.command_runner
            self.assertEqual(
                window_pane_titles(console_app.window_manager),
                [
                    # Split 1
                    [
                        'LogPane-1 - test_log1',
                        'LogPane-0 - test_log0',
                        'Python Repl - ',
                    ],
                ],
            )
            command_runner.open_dialog()
            # Set LogPane-1 as the focused window pane
            target_list_and_pane(console_app.window_manager, 0, 0)

            command_runner.input_field.buffer.text = 'move right'

            # pylint: disable=protected-access
            command_runner._make_regexes = MagicMock(
                wraps=command_runner._make_regexes
            )
            # pylint: enable=protected-access
            command_runner.filter_completions()
            # Filter should only be re-run if input text changed
            command_runner.filter_completions()
            command_runner._make_regexes.assert_called_once()  # pylint: disable=protected-access

            self.assertIn(
                '[View] > Move Window Right', command_runner.selected_item_text
            )
            # Run the Move Window Right action
            command_runner._run_selected_item()  # pylint: disable=protected-access
            # Dialog should be closed
            self.assertFalse(command_runner.show_dialog)
            # LogPane-1 should be moved to the right in it's own split
            self.assertEqual(
                window_pane_titles(console_app.window_manager),
                [
                    # Split 1
                    [
                        'LogPane-0 - test_log0',
                        'Python Repl - ',
                    ],
                    # Split 2
                    [
                        'LogPane-1 - test_log1',
                    ],
                ],
            )


if __name__ == '__main__':
    unittest.main()
