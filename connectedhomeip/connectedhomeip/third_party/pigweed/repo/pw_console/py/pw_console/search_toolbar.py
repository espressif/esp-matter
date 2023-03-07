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
"""SearchToolbar class used by LogPanes."""

from __future__ import annotations
import functools
from typing import TYPE_CHECKING

from prompt_toolkit.buffer import Buffer
from prompt_toolkit.filters import Condition, has_focus
from prompt_toolkit.formatted_text import StyleAndTextTuples
from prompt_toolkit.key_binding import (
    KeyBindings,
    KeyBindingsBase,
    KeyPressEvent,
)
from prompt_toolkit.layout import (
    ConditionalContainer,
    FormattedTextControl,
    HSplit,
    VSplit,
    Window,
    WindowAlign,
)
from prompt_toolkit.widgets import TextArea
from prompt_toolkit.validation import DynamicValidator

from pw_console.log_view import RegexValidator, SearchMatcher
from pw_console.widgets import (
    mouse_handlers,
    to_checkbox_with_keybind_indicator,
    to_keybind_indicator,
)

if TYPE_CHECKING:
    from pw_console.log_pane import LogPane


class SearchToolbar(ConditionalContainer):
    """Toolbar for entering search text and viewing match counts."""

    TOOLBAR_HEIGHT = 2

    def __init__(self, log_pane: 'LogPane'):
        self.log_pane = log_pane
        self.log_view = log_pane.log_view
        self.search_validator = RegexValidator()
        self._search_successful = False
        self._search_invert = False
        self._search_field = None

        self.input_field = TextArea(
            prompt=[
                (
                    'class:search-bar-setting',
                    '/',
                    functools.partial(
                        mouse_handlers.on_click,
                        self.focus_self,
                    ),
                )
            ],
            focusable=True,
            focus_on_click=True,
            scrollbar=False,
            multiline=False,
            height=1,
            dont_extend_height=True,
            dont_extend_width=False,
            accept_handler=self._search_accept_handler,
            validator=DynamicValidator(self.get_search_matcher),
            history=self.log_pane.application.search_history,
        )

        self.input_field.control.key_bindings = self._create_key_bindings()

        match_count_window = Window(
            content=FormattedTextControl(self.get_match_count_fragments),
            height=1,
            align=WindowAlign.LEFT,
            dont_extend_width=True,
            style='class:search-match-count-dialog',
        )

        match_buttons_window = Window(
            content=FormattedTextControl(self.get_button_fragments),
            height=1,
            align=WindowAlign.LEFT,
            dont_extend_width=False,
            style='class:search-match-count-dialog',
        )

        input_field_buttons_window = Window(
            content=FormattedTextControl(self.get_search_help_fragments),
            height=1,
            align=WindowAlign.RIGHT,
            dont_extend_width=True,
        )

        settings_bar_window = Window(
            content=FormattedTextControl(self.get_search_settings_fragments),
            height=1,
            align=WindowAlign.LEFT,
            dont_extend_width=False,
        )

        super().__init__(
            HSplit(
                [
                    # Top row
                    VSplit(
                        [
                            # Search Settings toggles, only show if the search
                            # input field is in focus.
                            ConditionalContainer(
                                settings_bar_window,
                                filter=has_focus(self.input_field),
                            ),
                            # Match count numbers and buttons, only show if the
                            # search input is NOT in focus.
                            # pylint: disable=invalid-unary-operand-type
                            ConditionalContainer(
                                match_count_window,
                                filter=~has_focus(self.input_field),
                            ),
                            ConditionalContainer(
                                match_buttons_window,
                                filter=~has_focus(self.input_field),
                            ),
                            # pylint: enable=invalid-unary-operand-type
                        ]
                    ),
                    # Bottom row
                    VSplit(
                        [
                            self.input_field,
                            ConditionalContainer(
                                input_field_buttons_window,
                                filter=has_focus(self),
                            ),
                        ]
                    ),
                ],
                height=SearchToolbar.TOOLBAR_HEIGHT,
                style='class:search-bar',
            ),
            filter=Condition(lambda: log_pane.search_bar_active),
        )

    def _create_key_bindings(self) -> KeyBindingsBase:
        """Create additional key bindings for the search input."""
        # Clear filter keybind is handled by the parent log_pane.

        key_bindings = KeyBindings()
        register = self.log_pane.application.prefs.register_keybinding

        @register('search-toolbar.cancel', key_bindings)
        def _close_search_bar(_event: KeyPressEvent) -> None:
            """Close search bar."""
            self.cancel_search()

        @register('search-toolbar.toggle-matcher', key_bindings)
        def _select_next_search_matcher(_event: KeyPressEvent) -> None:
            """Select the next search matcher."""
            self.log_pane.log_view.select_next_search_matcher()

        @register('search-toolbar.create-filter', key_bindings)
        def _create_filter(_event: KeyPressEvent) -> None:
            """Create a filter."""
            self.create_filter()

        @register('search-toolbar.toggle-invert', key_bindings)
        def _toggle_search_invert(_event: KeyPressEvent) -> None:
            """Toggle inverted search matching."""
            self._invert_search()

        @register('search-toolbar.toggle-column', key_bindings)
        def _select_next_field(_event: KeyPressEvent) -> None:
            """Select next search field/column."""
            self._next_field()

        return key_bindings

    def focus_self(self) -> None:
        self.log_pane.application.application.layout.focus(self)

    def focus_log_pane(self) -> None:
        self.log_pane.application.focus_on_container(self.log_pane)

    def _create_filter(self) -> None:
        self.input_field.buffer.reset()
        self.close_search_bar()
        self.log_view.apply_filter()

    def _next_match(self) -> None:
        self.log_view.search_forwards()

    def _previous_match(self) -> None:
        self.log_view.search_backwards()

    def cancel_search(self) -> None:
        self.input_field.buffer.reset()
        self.close_search_bar()
        self.log_view.clear_search()

    def close_search_bar(self) -> None:
        """Close search bar."""
        # Reset invert setting for the next search
        self._search_invert = False
        self.log_view.follow_search_match = False
        # Hide the search bar
        self.log_pane.search_bar_active = False
        # Focus on the log_pane.
        self.log_pane.application.focus_on_container(self.log_pane)
        self.log_pane.redraw_ui()

    def _start_search(self) -> None:
        self.input_field.buffer.validate_and_handle()

    def _invert_search(self) -> None:
        self._search_invert = not self._search_invert

    def _toggle_search_follow(self) -> None:
        self.log_view.follow_search_match = (
            not self.log_view.follow_search_match
        )
        # If automatically jumping to the next search match, disable normal
        # follow mode.
        if self.log_view.follow_search_match:
            self.log_view.follow = False

    def _next_field(self) -> None:
        fields = self.log_pane.log_view.log_store.table.all_column_names()
        fields.append(None)
        current_index = fields.index(self._search_field)
        next_index = (current_index + 1) % len(fields)
        self._search_field = fields[next_index]

    def create_filter(self) -> None:
        self._start_search()
        if self._search_successful:
            self.log_pane.log_view.apply_filter()

    def _search_accept_handler(self, buff: Buffer) -> bool:
        """Function run when hitting Enter in the search bar."""
        self._search_successful = False
        if len(buff.text) == 0:
            self.close_search_bar()
            # Don't apply an empty search.
            return False

        if self.log_pane.log_view.new_search(
            buff.text, invert=self._search_invert, field=self._search_field
        ):
            self._search_successful = True

            # Don't close the search bar, instead focus on the log content.
            self.log_pane.application.focus_on_container(
                self.log_pane.log_display_window
            )
            # Keep existing search text.
            return True

        # Keep existing text if regex error
        return True

    def get_search_help_fragments(self):
        """Return FormattedText with search general help keybinds."""
        focus = functools.partial(mouse_handlers.on_click, self.focus_self)
        start_search = functools.partial(
            mouse_handlers.on_click, self._start_search
        )
        close_search = functools.partial(
            mouse_handlers.on_click, self.cancel_search
        )

        # Search toolbar is darker than pane toolbars, use the darker button
        # style here.
        button_style = 'class:toolbar-button-inactive'

        separator_text = [('', '  ', focus)]

        # Empty text matching the width of the search bar title.
        fragments = [
            ('', '        ', focus),
        ]
        fragments.extend(separator_text)

        fragments.extend(
            to_keybind_indicator(
                'Enter', 'Search', start_search, base_style=button_style
            )
        )
        fragments.extend(separator_text)

        fragments.extend(
            to_keybind_indicator(
                'Ctrl-c', 'Cancel', close_search, base_style=button_style
            )
        )

        return fragments

    def get_search_settings_fragments(self):
        """Return FormattedText with current search settings and keybinds."""
        focus = functools.partial(mouse_handlers.on_click, self.focus_self)
        next_field = functools.partial(
            mouse_handlers.on_click, self._next_field
        )
        toggle_invert = functools.partial(
            mouse_handlers.on_click, self._invert_search
        )
        next_matcher = functools.partial(
            mouse_handlers.on_click,
            self.log_pane.log_view.select_next_search_matcher,
        )

        separator_text = [('', '  ', focus)]

        # Search toolbar is darker than pane toolbars, use the darker button
        # style here.
        button_style = 'class:toolbar-button-inactive'

        fragments = [
            # Title
            ('class:search-bar-title', ' Search ', focus),
        ]
        fragments.extend(separator_text)

        selected_column_text = [
            (
                button_style + ' class:search-bar-setting',
                (self._search_field.title() if self._search_field else 'All'),
                next_field,
            ),
        ]
        fragments.extend(
            to_keybind_indicator(
                'Ctrl-t',
                'Column:',
                next_field,
                middle_fragments=selected_column_text,
                base_style=button_style,
            )
        )
        fragments.extend(separator_text)

        fragments.extend(
            to_checkbox_with_keybind_indicator(
                self._search_invert,
                'Ctrl-v',
                'Invert',
                toggle_invert,
                base_style=button_style,
            )
        )
        fragments.extend(separator_text)

        # Matching Method
        current_matcher_text = [
            (
                button_style + ' class:search-bar-setting',
                str(self.log_pane.log_view.search_matcher.name),
                next_matcher,
            )
        ]
        fragments.extend(
            to_keybind_indicator(
                'Ctrl-n',
                'Matcher:',
                next_matcher,
                middle_fragments=current_matcher_text,
                base_style=button_style,
            )
        )
        fragments.extend(separator_text)

        return fragments

    def get_search_matcher(self):
        if self.log_pane.log_view.search_matcher == SearchMatcher.REGEX:
            return self.log_pane.log_view.search_validator
        return False

    def get_match_count_fragments(self):
        """Return formatted text for the match count indicator."""
        focus = functools.partial(mouse_handlers.on_click, self.focus_log_pane)
        two_spaces = ('', '  ', focus)

        # Check if this line is a search match
        match_number = self.log_view.search_matched_lines.get(
            self.log_view.log_index, -1
        )

        # If valid, increment the zero indexed value by one for better human
        # readability.
        if match_number >= 0:
            match_number += 1
        # If no match, mark as zero
        else:
            match_number = 0

        return [
            ('class:search-match-count-dialog-title', ' Match ', focus),
            (
                '',
                '{} / {}'.format(
                    match_number, len(self.log_view.search_matched_lines)
                ),
                focus,
            ),
            two_spaces,
        ]

    def get_button_fragments(self) -> StyleAndTextTuples:
        """Return formatted text for the action buttons."""
        focus = functools.partial(mouse_handlers.on_click, self.focus_log_pane)

        one_space = ('', ' ', focus)
        two_spaces = ('', '  ', focus)
        cancel = functools.partial(mouse_handlers.on_click, self.cancel_search)
        create_filter = functools.partial(
            mouse_handlers.on_click, self._create_filter
        )
        next_match = functools.partial(
            mouse_handlers.on_click, self._next_match
        )
        previous_match = functools.partial(
            mouse_handlers.on_click, self._previous_match
        )
        toggle_search_follow = functools.partial(
            mouse_handlers.on_click,
            self._toggle_search_follow,
        )

        button_style = 'class:toolbar-button-inactive'

        fragments = []
        fragments.extend(
            to_keybind_indicator(
                key='n',
                description='Next',
                mouse_handler=next_match,
                base_style=button_style,
            )
        )
        fragments.append(two_spaces)

        fragments.extend(
            to_keybind_indicator(
                key='N',
                description='Previous',
                mouse_handler=previous_match,
                base_style=button_style,
            )
        )
        fragments.append(two_spaces)

        fragments.extend(
            to_keybind_indicator(
                key='Ctrl-c',
                description='Cancel',
                mouse_handler=cancel,
                base_style=button_style,
            )
        )
        fragments.append(two_spaces)

        fragments.extend(
            to_keybind_indicator(
                key='Ctrl-Alt-f',
                description='Add Filter',
                mouse_handler=create_filter,
                base_style=button_style,
            )
        )
        fragments.append(two_spaces)

        fragments.extend(
            to_checkbox_with_keybind_indicator(
                checked=self.log_view.follow_search_match,
                key='',
                description='Jump to new matches',
                mouse_handler=toggle_search_follow,
                base_style=button_style,
            )
        )
        fragments.append(one_space)

        return fragments
