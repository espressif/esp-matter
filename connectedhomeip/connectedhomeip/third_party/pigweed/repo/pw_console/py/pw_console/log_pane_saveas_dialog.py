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
"""LogPane Save As Dialog."""

from __future__ import annotations
import functools
from pathlib import Path
from typing import Optional, TYPE_CHECKING

from prompt_toolkit.buffer import Buffer
from prompt_toolkit.key_binding import KeyBindings, KeyPressEvent
from prompt_toolkit.completion import PathCompleter
from prompt_toolkit.filters import Condition
from prompt_toolkit.history import InMemoryHistory
from prompt_toolkit.layout import (
    ConditionalContainer,
    FormattedTextControl,
    HSplit,
    Window,
    WindowAlign,
)
from prompt_toolkit.widgets import TextArea
from prompt_toolkit.validation import (
    ValidationError,
    Validator,
)

from pw_console.widgets import (
    create_border,
    mouse_handlers,
    to_checkbox_with_keybind_indicator,
    to_keybind_indicator,
)

if TYPE_CHECKING:
    from pw_console.log_pane import LogPane


class PathValidator(Validator):
    """Validation of file path input."""

    def validate(self, document):
        """Check input path leads to a valid parent directory."""
        target_path = Path(document.text).expanduser()

        if not target_path.parent.exists():
            raise ValidationError(
                # Set cursor position to the end
                len(document.text),
                "Directory doesn't exist: %s" % document.text,
            )

        if target_path.is_dir():
            raise ValidationError(
                # Set cursor position to the end
                len(document.text),
                "File input is an existing directory: %s" % document.text,
            )


class LogPaneSaveAsDialog(ConditionalContainer):
    """Dialog box for saving logs to a file."""

    # Height of the dialog box contens in lines of text.
    DIALOG_HEIGHT = 3

    def __init__(self, log_pane: 'LogPane'):
        self.log_pane = log_pane

        self.path_validator = PathValidator()

        self._export_with_table_formatting: bool = True
        self._export_with_selected_lines_only: bool = False

        self.starting_file_path: str = str(Path.cwd())

        self.input_field = TextArea(
            prompt=[
                (
                    'class:saveas-dialog-setting',
                    'File: ',
                    functools.partial(
                        mouse_handlers.on_click,
                        self.focus_self,
                    ),
                )
            ],
            # Pre-fill the current working directory.
            text=self.starting_file_path,
            focusable=True,
            focus_on_click=True,
            scrollbar=False,
            multiline=False,
            height=1,
            dont_extend_height=True,
            dont_extend_width=False,
            accept_handler=self._saveas_accept_handler,
            validator=self.path_validator,
            history=InMemoryHistory(),
            completer=PathCompleter(expanduser=True),
        )

        self.input_field.buffer.cursor_position = len(self.starting_file_path)

        settings_bar_control = FormattedTextControl(self.get_settings_fragments)
        settings_bar_window = Window(
            content=settings_bar_control,
            height=1,
            align=WindowAlign.LEFT,
            dont_extend_width=False,
        )

        action_bar_control = FormattedTextControl(self.get_action_fragments)
        action_bar_window = Window(
            content=action_bar_control,
            height=1,
            align=WindowAlign.RIGHT,
            dont_extend_width=False,
        )

        # Add additional keybindings for the input_field text area.
        key_bindings = KeyBindings()
        register = self.log_pane.application.prefs.register_keybinding

        @register('save-as-dialog.cancel', key_bindings)
        def _close_saveas_dialog(_event: KeyPressEvent) -> None:
            """Close save as dialog."""
            self.close_dialog()

        self.input_field.control.key_bindings = key_bindings

        super().__init__(
            create_border(
                HSplit(
                    [
                        settings_bar_window,
                        self.input_field,
                        action_bar_window,
                    ],
                    height=LogPaneSaveAsDialog.DIALOG_HEIGHT,
                    style='class:saveas-dialog',
                ),
                LogPaneSaveAsDialog.DIALOG_HEIGHT,
                border_style='class:saveas-dialog-border',
                left_margin_columns=1,
            ),
            filter=Condition(lambda: self.log_pane.saveas_dialog_active),
        )

    def focus_self(self):
        self.log_pane.application.application.layout.focus(self)

    def close_dialog(self):
        """Close this dialog."""
        self.log_pane.saveas_dialog_active = False
        self.log_pane.application.focus_on_container(self.log_pane)
        self.log_pane.redraw_ui()

    def _toggle_table_formatting(self):
        self._export_with_table_formatting = (
            not self._export_with_table_formatting
        )

    def _toggle_selected_lines(self):
        self._export_with_selected_lines_only = (
            not self._export_with_selected_lines_only
        )

    def set_export_options(
        self,
        table_format: Optional[bool] = None,
        selected_lines_only: Optional[bool] = None,
    ) -> None:
        # Allows external callers such as the line selection dialog to set
        # export format options.
        if table_format is not None:
            self._export_with_table_formatting = table_format

        if selected_lines_only is not None:
            self._export_with_selected_lines_only = selected_lines_only

    def save_action(self):
        """Trigger save file execution on mouse click.

        This ultimately runs LogPaneSaveAsDialog._saveas_accept_handler()."""
        self.input_field.buffer.validate_and_handle()

    def _saveas_accept_handler(self, buff: Buffer) -> bool:
        """Function run when hitting Enter in the input_field."""
        input_text = buff.text
        if len(input_text) == 0:
            self.close_dialog()
            # Don't save anything if empty input.
            return False

        if self.log_pane.log_view.export_logs(
            file_name=input_text,
            use_table_formatting=self._export_with_table_formatting,
            selected_lines_only=self._export_with_selected_lines_only,
        ):
            self.close_dialog()
            # Reset selected_lines_only
            self.set_export_options(selected_lines_only=False)
            # Erase existing input text.
            return False

        # Keep existing text if error
        return True

    def get_settings_fragments(self):
        """Return FormattedText with current save settings."""
        # Mouse handlers
        focus = functools.partial(mouse_handlers.on_click, self.focus_self)
        toggle_table_formatting = functools.partial(
            mouse_handlers.on_click,
            self._toggle_table_formatting,
        )
        toggle_selected_lines = functools.partial(
            mouse_handlers.on_click,
            self._toggle_selected_lines,
        )

        # Separator should have the focus mouse handler so clicking on any
        # whitespace focuses the input field.
        separator_text = ('', '  ', focus)

        # Default button style
        button_style = 'class:toolbar-button-inactive'

        fragments = [('class:saveas-dialog-title', 'Save as File', focus)]
        fragments.append(separator_text)

        # Table checkbox
        fragments.extend(
            to_checkbox_with_keybind_indicator(
                checked=self._export_with_table_formatting,
                key='',  # No key shortcut help text
                description='Table Formatting',
                mouse_handler=toggle_table_formatting,
                base_style=button_style,
            )
        )

        # Two space separator
        fragments.append(separator_text)

        # Selected lines checkbox
        fragments.extend(
            to_checkbox_with_keybind_indicator(
                checked=self._export_with_selected_lines_only,
                key='',  # No key shortcut help text
                description='Selected Lines Only',
                mouse_handler=toggle_selected_lines,
                base_style=button_style,
            )
        )

        # Two space separator
        fragments.append(separator_text)

        return fragments

    def get_action_fragments(self):
        """Return FormattedText with the save action buttons."""
        # Mouse handlers
        focus = functools.partial(mouse_handlers.on_click, self.focus_self)
        cancel = functools.partial(mouse_handlers.on_click, self.close_dialog)
        save = functools.partial(mouse_handlers.on_click, self.save_action)

        # Separator should have the focus mouse handler so clicking on any
        # whitespace focuses the input field.
        separator_text = ('', '  ', focus)

        # Default button style
        button_style = 'class:toolbar-button-inactive'

        fragments = [separator_text]
        # Cancel button
        fragments.extend(
            to_keybind_indicator(
                key='Ctrl-c',
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
                key='Enter',
                description='Save',
                mouse_handler=save,
                base_style=button_style,
            )
        )

        # One space separator
        fragments.append(('', ' ', focus))

        return fragments
