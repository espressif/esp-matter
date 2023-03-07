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
"""LogPane FilterToolbar class."""

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
from prompt_toolkit.mouse_events import MouseEvent, MouseEventType

from pw_console.style import (
    get_button_style,
    get_toolbar_style,
)
from pw_console.widgets import (
    mouse_handlers,
    to_keybind_indicator,
)

if TYPE_CHECKING:
    from pw_console.log_pane import LogPane


class FilterToolbar(ConditionalContainer):
    """Container showing each filter applied in order."""

    TOOLBAR_HEIGHT = 1

    def mouse_handler_delete_filter(self, filter_text, mouse_event: MouseEvent):
        """Delete the given log filter."""
        if mouse_event.event_type == MouseEventType.MOUSE_UP:
            self.log_pane.log_view.delete_filter(filter_text)
            return None
        return NotImplemented

    def get_left_fragments(self):
        """Return formatted text tokens for display."""
        separator = ('', '  ')
        space = ('', ' ')
        fragments = [('class:filter-bar-title', ' Filters '), separator]

        button_style = get_button_style(self.log_pane)

        for filter_text, log_filter in self.log_pane.log_view.filters.items():
            fragments.append(('class:filter-bar-delimiter', '<'))

            if log_filter.invert:
                fragments.append(('class:filter-bar-setting', 'NOT '))

            if log_filter.field:
                fragments.append(('class:filter-bar-setting', log_filter.field))
                fragments.append(space)

            fragments.append(('', filter_text))
            fragments.append(space)

            fragments.append(
                (
                    button_style + ' class:filter-bar-delete',
                    ' (X) ',
                    functools.partial(
                        self.mouse_handler_delete_filter, filter_text
                    ),
                )
            )  # type: ignore
            fragments.append(('class:filter-bar-delimiter', '>'))

            fragments.append(separator)
        return fragments

    def get_center_fragments(self):
        """Return formatted text tokens for display."""
        clear_filters = functools.partial(
            mouse_handlers.on_click,
            self.log_pane.log_view.clear_filters,
        )

        button_style = get_button_style(self.log_pane)

        return to_keybind_indicator(
            'Ctrl-Alt-r',
            'Clear Filters',
            clear_filters,
            base_style=button_style,
        )

    def __init__(self, log_pane: 'LogPane'):
        self.log_pane = log_pane
        left_bar_control = FormattedTextControl(self.get_left_fragments)
        left_bar_window = Window(
            content=left_bar_control,
            align=WindowAlign.LEFT,
            dont_extend_width=True,
        )
        center_bar_control = FormattedTextControl(self.get_center_fragments)
        center_bar_window = Window(
            content=center_bar_control,
            align=WindowAlign.LEFT,
            dont_extend_width=False,
        )
        super().__init__(
            VSplit(
                [
                    left_bar_window,
                    center_bar_window,
                ],
                style=functools.partial(
                    get_toolbar_style, self.log_pane, dim=True
                ),
                height=1,
                align=HorizontalAlign.LEFT,
            ),
            # Only show if filtering is enabled.
            filter=Condition(lambda: self.log_pane.log_view.filtering_on),
        )
