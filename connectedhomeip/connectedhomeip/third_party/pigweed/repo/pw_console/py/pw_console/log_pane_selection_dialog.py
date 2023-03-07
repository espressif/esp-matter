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
"""Dialog box for log selection functions."""

from __future__ import annotations
import functools
from typing import TYPE_CHECKING

from prompt_toolkit.filters import Condition
from prompt_toolkit.layout import (
    ConditionalContainer,
    FormattedTextControl,
    Window,
    WindowAlign,
)

from pw_console.widgets import (
    create_border,
    mouse_handlers,
    to_checkbox_with_keybind_indicator,
    to_keybind_indicator,
)

if TYPE_CHECKING:
    from pw_console.log_pane import LogPane


class LogPaneSelectionDialog(ConditionalContainer):
    """Dialog box for showing log selection functions.

    Displays number of lines selected, buttons for copying to the clipboar or
    saving to a file, and buttons to select all or cancel (clear) the
    selection."""

    # Height of the dialog box contens in lines of text.
    DIALOG_HEIGHT = 3

    def __init__(self, log_pane: 'LogPane'):
        self.log_pane = log_pane
        self.log_view = log_pane.log_view

        self._markdown_flag: bool = False
        self._table_flag: bool = True

        selection_bar_control = FormattedTextControl(self.get_fragments)
        selection_bar_window = Window(
            content=selection_bar_control,
            height=1,
            align=WindowAlign.LEFT,
            dont_extend_width=False,
            style='class:selection-dialog',
        )

        super().__init__(
            create_border(
                selection_bar_window,
                (LogPaneSelectionDialog.DIALOG_HEIGHT - 1),
                border_style='class:selection-dialog-border',
                base_style='class:selection-dialog-default-fg',
                top=False,
                right=False,
            ),
            filter=Condition(lambda: self.log_view.visual_select_mode),
        )

    def focus_log_pane(self):
        self.log_pane.application.focus_on_container(self.log_pane)

    def _toggle_markdown_flag(self) -> None:
        self._markdown_flag = not self._markdown_flag

    def _toggle_table_flag(self) -> None:
        self._table_flag = not self._table_flag

    def _select_all(self) -> None:
        self.log_view.visual_select_all()

    def _select_none(self) -> None:
        self.log_view.clear_visual_selection()

    def _copy_selection(self) -> None:
        if self.log_view.export_logs(
            to_clipboard=True,
            use_table_formatting=self._table_flag,
            selected_lines_only=True,
            add_markdown_fence=self._markdown_flag,
        ):
            self._select_none()

    def _saveas_file(self) -> None:
        self.log_pane.start_saveas(
            table_format=self._table_flag, selected_lines_only=True
        )

    def get_fragments(self):
        """Return formatted text tuples for both rows of the selection
        dialog."""

        focus = functools.partial(mouse_handlers.on_click, self.focus_log_pane)

        one_space = ('', ' ', focus)
        two_spaces = ('', '  ', focus)
        select_all = functools.partial(
            mouse_handlers.on_click, self._select_all
        )
        select_none = functools.partial(
            mouse_handlers.on_click, self._select_none
        )

        copy_selection = functools.partial(
            mouse_handlers.on_click, self._copy_selection
        )
        saveas_file = functools.partial(
            mouse_handlers.on_click, self._saveas_file
        )
        toggle_markdown = functools.partial(
            mouse_handlers.on_click,
            self._toggle_markdown_flag,
        )
        toggle_table = functools.partial(
            mouse_handlers.on_click, self._toggle_table_flag
        )

        button_style = 'class:toolbar-button-inactive'

        # First row of text
        fragments = [
            (
                'class:selection-dialog-title',
                ' {} Selected '.format(
                    self.log_view.visual_selected_log_count()
                ),
                focus,
            ),
            one_space,
            ('class:selection-dialog-default-fg', 'Format: ', focus),
        ]

        # Table and Markdown options
        fragments.extend(
            to_checkbox_with_keybind_indicator(
                self._table_flag,
                key='',
                description='Table',
                mouse_handler=toggle_table,
                base_style='class:selection-dialog-default-bg',
            )
        )

        fragments.extend(
            to_checkbox_with_keybind_indicator(
                self._markdown_flag,
                key='',
                description='Markdown',
                mouse_handler=toggle_markdown,
                base_style='class:selection-dialog-default-bg',
            )
        )

        # Line break
        fragments.append(('', '\n'))

        # Second row of text
        fragments.append(one_space)

        fragments.extend(
            to_keybind_indicator(
                key='Ctrl-c',
                description='Cancel',
                mouse_handler=select_none,
                base_style=button_style,
            )
        )
        fragments.append(two_spaces)

        fragments.extend(
            to_keybind_indicator(
                key='Ctrl-a',
                description='Select All',
                mouse_handler=select_all,
                base_style=button_style,
            )
        )
        fragments.append(two_spaces)

        fragments.append(one_space)
        fragments.extend(
            to_keybind_indicator(
                key='',
                description='Save as File',
                mouse_handler=saveas_file,
                base_style=button_style,
            )
        )
        fragments.append(two_spaces)

        fragments.extend(
            to_keybind_indicator(
                key='',
                description='Copy',
                mouse_handler=copy_selection,
                base_style=button_style,
            )
        )
        fragments.append(one_space)

        return fragments
