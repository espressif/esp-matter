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

import inspect
import logging
import unittest
from unittest.mock import MagicMock

from jinja2 import Environment, PackageLoader, make_logging_undefined
from prompt_toolkit.key_binding import KeyBindings

from pw_console.help_window import HelpWindow

_PW_CONSOLE_MODULE = 'pw_console'


_jinja_env = Environment(
    loader=PackageLoader(_PW_CONSOLE_MODULE),
    undefined=make_logging_undefined(logger=logging.getLogger('pw_console')),
    trim_blocks=True,
    lstrip_blocks=True,
)


def _create_app_mock():
    template = _jinja_env.get_template('keybind_list.jinja')
    mock_app = MagicMock()
    mock_app.get_template = MagicMock(return_value=template)
    return mock_app


class TestHelpWindow(unittest.TestCase):
    """Tests for HelpWindow text and keybind lists."""

    def setUp(self):
        self.maxDiff = None  # pylint: disable=invalid-name

    def test_instantiate(self) -> None:
        app = _create_app_mock()
        help_window = HelpWindow(app)
        self.assertIsNotNone(help_window)

    # pylint: disable=unused-variable,unused-argument
    def test_add_keybind_help_text(self) -> None:
        key_bindings = KeyBindings()

        @key_bindings.add('f1')
        def show_help(event):
            """Toggle help window."""

        @key_bindings.add('c-w')
        @key_bindings.add('c-q')
        def exit_(event):
            """Quit the application."""

        app = _create_app_mock()

        help_window = HelpWindow(app)
        help_window.add_keybind_help_text('Global', key_bindings)

        self.assertEqual(
            help_window.help_text_sections,
            {
                'Global': {
                    'Quit the application.': ['Ctrl-Q', 'Ctrl-W'],
                    'Toggle help window.': ['F1'],
                }
            },
        )

    def test_generate_help_text(self) -> None:
        """Test keybind list template generation."""
        global_bindings = KeyBindings()

        @global_bindings.add('f1')
        def show_help(event):
            """Toggle help window."""

        @global_bindings.add('c-w')
        @global_bindings.add('c-q')
        def exit_(event):
            """Quit the application."""

        focus_bindings = KeyBindings()

        @focus_bindings.add('s-tab')
        @focus_bindings.add('c-right')
        @focus_bindings.add('c-down')
        def app_focus_next(event):
            """Move focus to the next widget."""

        @focus_bindings.add('c-left')
        @focus_bindings.add('c-up')
        def app_focus_previous(event):
            """Move focus to the previous widget."""

        app = _create_app_mock()

        help_window = HelpWindow(
            app,
            preamble='Pigweed CLI v0.1',
            additional_help_text=inspect.cleandoc(
                """
                Welcome to the Pigweed Console!
                Please enjoy this extra help text.
            """
            ),
        )
        help_window.add_keybind_help_text('Global', global_bindings)
        help_window.add_keybind_help_text('Focus', focus_bindings)
        help_window.generate_help_text()

        self.assertIn(
            inspect.cleandoc(
                """
            Welcome to the Pigweed Console!
            Please enjoy this extra help text.
            """
            ),
            help_window.help_text,
        )
        self.assertIn(
            inspect.cleandoc(
                """
            ==== Global Keys ====
            """
            ),
            help_window.help_text,
        )
        self.assertIn(
            inspect.cleandoc(
                """
            Toggle help window. -----------------  F1
            Quit the application. ---------------  Ctrl-Q
                                                   Ctrl-W
            """
            ),
            help_window.help_text,
        )
        self.assertIn(
            inspect.cleandoc(
                """
            ==== Focus Keys ====
            """
            ),
            help_window.help_text,
        )
        self.assertIn(
            inspect.cleandoc(
                """
            Move focus to the next widget. ------  Ctrl-Down
                                                   Ctrl-Right
                                                   Shift-Tab
            Move focus to the previous widget. --  Ctrl-Left
                                                   Ctrl-Up
            """
            ),
            help_window.help_text,
        )


if __name__ == '__main__':
    unittest.main()
