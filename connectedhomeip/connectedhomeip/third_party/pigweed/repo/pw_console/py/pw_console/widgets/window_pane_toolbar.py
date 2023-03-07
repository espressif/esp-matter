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
"""Window pane toolbar base class."""

import logging
from typing import Any, Callable, List, Optional
import functools

from prompt_toolkit.filters import Condition, has_focus
from prompt_toolkit.layout import (
    ConditionalContainer,
    FormattedTextControl,
    VSplit,
    Window,
    WindowAlign,
)
from prompt_toolkit.mouse_events import MouseEvent, MouseEventType

from pw_console.get_pw_console_app import get_pw_console_app
from pw_console.style import (
    get_pane_indicator,
    get_button_style,
    get_toolbar_style,
)
from pw_console.widgets import (
    ToolbarButton,
    mouse_handlers,
    to_checkbox_with_keybind_indicator,
    to_keybind_indicator,
)

_LOG = logging.getLogger(__package__)


class WindowPaneResizeHandle(FormattedTextControl):
    """Button to initiate window pane resize drag events."""

    def __init__(self, parent_window_pane: Any, *args, **kwargs) -> None:
        self.parent_window_pane = parent_window_pane
        super().__init__(*args, **kwargs)

    def mouse_handler(self, mouse_event: MouseEvent):
        """Mouse handler for this control."""
        # Start resize mouse drag event
        if mouse_event.event_type == MouseEventType.MOUSE_DOWN:
            get_pw_console_app().window_manager.start_resize_pane(
                self.parent_window_pane
            )
            # Mouse event handled, return None.
            return None

        # Mouse event not handled, return NotImplemented.
        return NotImplemented


class WindowPaneToolbar:
    """One line toolbar for display at the bottom of of a window pane."""

    # pylint: disable=too-many-instance-attributes
    TOOLBAR_HEIGHT = 1

    def get_left_text_tokens(self):
        """Return toolbar indicator and title."""

        title = self.title
        if not title and self.parent_window_pane:
            # No title was set, fetch the parent window pane title if available.
            parent_pane_title = self.parent_window_pane.pane_title()
            title = parent_pane_title if parent_pane_title else title
        return get_pane_indicator(
            self.focus_check_container, f' {title} ', self.focus_mouse_handler
        )

    def get_center_text_tokens(self):
        """Return formatted text tokens for display in the center part of the
        toolbar."""

        button_style = get_button_style(self.focus_check_container)

        # FormattedTextTuple contents: (Style, Text, Mouse handler)
        separator_text = [('', '  ')]  # 2 spaces of separaton between keybinds.
        if self.focus_mouse_handler:
            separator_text = [('', '  ', self.focus_mouse_handler)]

        fragments = []
        fragments.extend(separator_text)

        for button in self.buttons:
            on_click_handler = None
            if button.mouse_handler:
                on_click_handler = functools.partial(
                    mouse_handlers.on_click,
                    button.mouse_handler,
                )

            if button.is_checkbox:
                fragments.extend(
                    to_checkbox_with_keybind_indicator(
                        button.checked(),
                        button.key,
                        button.description,
                        on_click_handler,
                        base_style=button_style,
                    )
                )
            else:
                fragments.extend(
                    to_keybind_indicator(
                        button.key,
                        button.description,
                        on_click_handler,
                        base_style=button_style,
                    )
                )

            fragments.extend(separator_text)

        # Remaining whitespace should focus on click.
        fragments.extend(separator_text)

        return fragments

    def get_right_text_tokens(self):
        """Return formatted text tokens for display."""
        fragments = []
        if not has_focus(self.focus_check_container.__pt_container__())():
            fragments.append(
                (
                    'class:toolbar-button-inactive '
                    'class:toolbar-button-decoration',
                    ' ',
                    self.focus_mouse_handler,
                )
            )
            fragments.append(
                (
                    'class:toolbar-button-inactive class:keyhelp',
                    'click to focus',
                    self.focus_mouse_handler,
                )
            )
            fragments.append(
                (
                    'class:toolbar-button-inactive '
                    'class:toolbar-button-decoration',
                    ' ',
                    self.focus_mouse_handler,
                )
            )
        fragments.append(
            ('', '  {} '.format(self.subtitle()), self.focus_mouse_handler)
        )
        return fragments

    def get_resize_handle(self):
        return get_pane_indicator(
            self.focus_check_container, '─══─', hide_indicator=True
        )

    def add_button(self, button: ToolbarButton):
        self.buttons.append(button)

    def __init__(
        self,
        parent_window_pane: Optional[Any] = None,
        title: Optional[str] = None,
        subtitle: Optional[Callable[[], str]] = None,
        focus_check_container: Optional[Any] = None,
        focus_action_callable: Optional[Callable] = None,
        center_section_align: WindowAlign = WindowAlign.LEFT,
        include_resize_handle: bool = True,
    ):

        self.parent_window_pane = parent_window_pane
        self.title = title
        self.subtitle = subtitle

        # Assume check this container for focus
        self.focus_check_container = self
        self.focus_action_callable = None

        # Set parent_window_pane related options
        if self.parent_window_pane:
            if not subtitle:
                self.subtitle = self.parent_window_pane.pane_subtitle
            self.focus_check_container = self.parent_window_pane
            self.focus_action_callable = self.parent_window_pane.focus_self

        # Set title overrides
        if self.subtitle is None:

            def empty_subtitle() -> str:
                return ''

            self.subtitle = empty_subtitle

        if focus_check_container:
            self.focus_check_container = focus_check_container
        if focus_action_callable:
            self.focus_action_callable = focus_action_callable

        self.focus_mouse_handler = None
        if self.focus_action_callable:
            self.focus_mouse_handler = functools.partial(
                mouse_handlers.on_click,
                self.focus_action_callable,
            )

        self.buttons: List[ToolbarButton] = []
        self.show_toolbar = True

        self.left_section_window = Window(
            content=FormattedTextControl(self.get_left_text_tokens),
            align=WindowAlign.LEFT,
            dont_extend_width=True,
        )

        self.center_section_window = Window(
            content=FormattedTextControl(self.get_center_text_tokens),
            align=center_section_align,
            dont_extend_width=False,
        )

        self.right_section_window = Window(
            content=FormattedTextControl(self.get_right_text_tokens),
            # Right side text should appear at the far right of the toolbar
            align=WindowAlign.RIGHT,
            dont_extend_width=True,
        )

        wrapped_get_toolbar_style = functools.partial(
            get_toolbar_style, self.focus_check_container
        )

        sections = [
            self.left_section_window,
            self.center_section_window,
            self.right_section_window,
        ]
        if self.parent_window_pane and include_resize_handle:
            resize_handle = Window(
                content=WindowPaneResizeHandle(
                    self.parent_window_pane,
                    self.get_resize_handle,
                ),
                # Right side text should appear at the far right of the toolbar
                align=WindowAlign.RIGHT,
                dont_extend_width=True,
            )
            sections.append(resize_handle)

        self.toolbar_vsplit = VSplit(
            sections,
            height=WindowPaneToolbar.TOOLBAR_HEIGHT,
            style=wrapped_get_toolbar_style,
        )

        self.container = self._create_toolbar_container(self.toolbar_vsplit)

    def _create_toolbar_container(self, content):
        return ConditionalContainer(
            content, filter=Condition(lambda: self.show_toolbar)
        )

    def __pt_container__(self):
        """Return the prompt_toolkit root container for this log pane.

        This allows self to be used wherever prompt_toolkit expects a container
        object."""
        return self.container  # pylint: disable=no-member
