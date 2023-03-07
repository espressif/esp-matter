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
"""LogPane Info Toolbar classes."""

from __future__ import annotations
import functools
from typing import TYPE_CHECKING

from prompt_toolkit.filters import Condition
from prompt_toolkit.layout import (
    ConditionalContainer,
    FormattedTextControl,
    VSplit,
    Window,
    WindowAlign,
    HorizontalAlign,
)

from pw_console.style import get_toolbar_style

if TYPE_CHECKING:
    from pw_console.log_pane import LogPane


class LineInfoBar(ConditionalContainer):
    """One line bar for showing current and total log lines."""

    def get_tokens(self):
        """Return formatted text tokens for display."""
        tokens = ' {} / {} '.format(
            self.log_pane.log_view.get_current_line() + 1,
            self.log_pane.log_view.get_total_count(),
        )
        return [('', tokens)]

    def __init__(self, log_pane: 'LogPane'):
        self.log_pane = log_pane
        info_bar_control = FormattedTextControl(self.get_tokens)
        info_bar_window = Window(
            content=info_bar_control,
            align=WindowAlign.RIGHT,
            dont_extend_width=True,
        )

        super().__init__(
            VSplit(
                [info_bar_window],
                height=1,
                style=functools.partial(
                    get_toolbar_style, self.log_pane, dim=True
                ),
                align=HorizontalAlign.RIGHT,
            ),
            # Only show current/total line info if not auto-following
            # logs. Similar to tmux behavior.
            filter=Condition(lambda: not self.log_pane.log_view.follow),
        )


class TableToolbar(ConditionalContainer):
    """One line toolbar for showing table headers."""

    TOOLBAR_HEIGHT = 1

    def __init__(self, log_pane: 'LogPane'):
        # FormattedText of the table column headers.
        table_header_bar_control = FormattedTextControl(
            log_pane.log_view.render_table_header
        )
        # Left justify the header content.
        table_header_bar_window = Window(
            content=table_header_bar_control,
            align=WindowAlign.LEFT,
            dont_extend_width=False,
        )
        super().__init__(
            VSplit(
                [table_header_bar_window],
                height=1,
                style=functools.partial(get_toolbar_style, log_pane, dim=True),
                align=HorizontalAlign.LEFT,
            ),
            filter=Condition(
                lambda: log_pane.table_view
                and log_pane.log_view.get_total_count() > 0
            ),
        )
