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
"""LogPane Info Toolbar classes."""

from __future__ import annotations
import functools
import logging
import sys
from typing import Optional, Callable, TYPE_CHECKING

from prompt_toolkit.data_structures import Point
from prompt_toolkit.key_binding import KeyBindings, KeyPressEvent
from prompt_toolkit.filters import Condition
from prompt_toolkit.layout import (
    ConditionalContainer,
    FormattedTextControl,
    HSplit,
    Window,
    WindowAlign,
)

from pw_console.widgets import (
    create_border,
    mouse_handlers,
    to_keybind_indicator,
)

if TYPE_CHECKING:
    from pw_console.console_app import ConsoleApp

_LOG = logging.getLogger(__package__)


class QuitDialog(ConditionalContainer):
    """Confirmation quit dialog box."""

    DIALOG_HEIGHT = 2

    def __init__(
        self, application: ConsoleApp, on_quit: Optional[Callable] = None
    ):
        self.application = application
        self.show_dialog = False
        # Tracks the last focused container, to enable restoring focus after
        # closing the dialog.
        self.last_focused_pane = None

        self.on_quit_function = (
            on_quit if on_quit else self._default_on_quit_function
        )

        # Quit keybindings are active when this dialog is in focus
        key_bindings = KeyBindings()
        register = self.application.prefs.register_keybinding

        @register('quit-dialog.yes', key_bindings)
        def _quit(_event: KeyPressEvent) -> None:
            """Close save as bar."""
            self.quit_action()

        @register('quit-dialog.no', key_bindings)
        def _cancel(_event: KeyPressEvent) -> None:
            """Close save as bar."""
            self.close_dialog()

        self.exit_message = 'Quit? y/n '

        action_bar_control = FormattedTextControl(
            self.get_action_fragments,
            show_cursor=True,
            focusable=True,
            key_bindings=key_bindings,
            # Cursor will appear after the exit_message
            get_cursor_position=lambda: Point(len(self.exit_message), 0),
        )

        action_bar_window = Window(
            content=action_bar_control,
            height=QuitDialog.DIALOG_HEIGHT,
            align=WindowAlign.LEFT,
            dont_extend_width=False,
        )

        super().__init__(
            create_border(
                HSplit(
                    [action_bar_window],
                    height=QuitDialog.DIALOG_HEIGHT,
                    style='class:quit-dialog',
                ),
                QuitDialog.DIALOG_HEIGHT,
                border_style='class:quit-dialog-border',
                left_margin_columns=1,
            ),
            filter=Condition(lambda: self.show_dialog),
        )

    def focus_self(self):
        self.application.layout.focus(self)

    def close_dialog(self):
        """Close this dialog box."""
        self.show_dialog = False
        # Restore original focus if possible.
        if self.last_focused_pane:
            self.application.layout.focus(self.last_focused_pane)
        else:
            # Fallback to focusing on the main menu.
            self.application.focus_main_menu()

    def open_dialog(self):
        self.show_dialog = True
        self.last_focused_pane = self.application.focused_window()
        self.focus_self()
        self.application.redraw_ui()

    def _default_on_quit_function(self):
        if hasattr(self.application, 'application'):
            self.application.application.exit()
        else:
            sys.exit()

    def quit_action(self):
        self.on_quit_function()

    def get_action_fragments(self):
        """Return FormattedText with action buttons."""

        # Mouse handlers
        focus = functools.partial(mouse_handlers.on_click, self.focus_self)
        cancel = functools.partial(mouse_handlers.on_click, self.close_dialog)
        quit_action = functools.partial(
            mouse_handlers.on_click, self.quit_action
        )

        # Separator should have the focus mouse handler so clicking on any
        # whitespace focuses the input field.
        separator_text = ('', '  ', focus)

        # Default button style
        button_style = 'class:toolbar-button-inactive'

        fragments = [('', self.exit_message), separator_text]
        fragments.append(('', '\n'))

        # Cancel button
        fragments.extend(
            to_keybind_indicator(
                key='n / Ctrl-c',
                description='Cancel',
                mouse_handler=cancel,
                base_style=button_style,
            )
        )

        # Two space separator
        fragments.append(separator_text)

        # Save button
        fragments.extend(
            to_keybind_indicator(
                key='y / Ctrl-d',
                description='Quit',
                mouse_handler=quit_action,
                base_style=button_style,
            )
        )

        # One space separator
        fragments.append(('', ' ', focus))

        return fragments
