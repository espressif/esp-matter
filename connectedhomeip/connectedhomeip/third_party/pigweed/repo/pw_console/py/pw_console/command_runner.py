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
"""CommandRunner dialog classes."""

from __future__ import annotations
import functools
import logging
import re
from typing import (
    Callable,
    Iterable,
    Iterator,
    List,
    Optional,
    TYPE_CHECKING,
    Tuple,
)

from prompt_toolkit.buffer import Buffer
from prompt_toolkit.filters import Condition
from prompt_toolkit.formatted_text import StyleAndTextTuples
from prompt_toolkit.formatted_text.utils import fragment_list_to_text
from prompt_toolkit.layout.utils import explode_text_fragments
from prompt_toolkit.history import InMemoryHistory
from prompt_toolkit.key_binding import (
    KeyBindings,
    KeyBindingsBase,
    KeyPressEvent,
)
from prompt_toolkit.layout import (
    AnyContainer,
    ConditionalContainer,
    DynamicContainer,
    FormattedTextControl,
    HSplit,
    VSplit,
    Window,
    WindowAlign,
)
from prompt_toolkit.widgets import MenuItem
from prompt_toolkit.widgets import TextArea

from pw_console.widgets import (
    create_border,
    mouse_handlers,
    to_keybind_indicator,
)

if TYPE_CHECKING:
    from pw_console.console_app import ConsoleApp

_LOG = logging.getLogger(__package__)


def flatten_menu_items(
    items: List[MenuItem], prefix: str = ''
) -> Iterator[Tuple[str, Callable]]:
    """Flatten nested prompt_toolkit MenuItems into text and callable tuples."""
    for item in items:
        new_text = []
        if prefix:
            new_text.append(prefix)
        new_text.append(item.text)
        new_prefix = ' > '.join(new_text)

        if item.children:
            yield from flatten_menu_items(item.children, new_prefix)
        elif item.handler:
            # Skip this item if it's a separator or disabled.
            if item.text == '-' or item.disabled:
                continue
            yield (new_prefix, item.handler)


def highlight_matches(
    regexes: Iterable[re.Pattern], line_fragments: StyleAndTextTuples
) -> StyleAndTextTuples:
    """Highlight regex matches in prompt_toolkit FormattedTextTuples."""
    line_text = fragment_list_to_text(line_fragments)
    exploded_fragments = explode_text_fragments(line_fragments)

    def apply_highlighting(
        fragments: StyleAndTextTuples, index: int, matching_regex_index: int = 0
    ) -> None:
        # Expand all fragments and apply the highlighting style.
        old_style, _text, *_ = fragments[index]
        # There are 6 fuzzy-highlight styles defined in style.py. Get an index
        # from 0-5 to use one style after the other in turn.
        style_index = matching_regex_index % 6
        fragments[index] = (
            old_style + f' class:command-runner-fuzzy-highlight-{style_index} ',
            fragments[index][1],
        )

    # Highlight each non-overlapping search match.
    for regex_i, regex in enumerate(regexes):
        for match in regex.finditer(line_text):
            for fragment_i in range(match.start(), match.end()):
                apply_highlighting(exploded_fragments, fragment_i, regex_i)

    return exploded_fragments


class CommandRunner:
    """CommandRunner dialog box."""

    # pylint: disable=too-many-instance-attributes

    def __init__(
        self,
        application: ConsoleApp,
        window_title: Optional[str] = None,
        load_completions: Optional[
            Callable[[], List[Tuple[str, Callable]]]
        ] = None,
        width: int = 80,
        height: int = 10,
    ):
        # Parent pw_console application
        self.application = application
        # Visibility toggle
        self.show_dialog = False
        # Tracks the last focused container, to enable restoring focus after
        # closing the dialog.
        self.last_focused_pane = None

        # List of all possible completion items
        self.completions: List[Tuple[str, Callable]] = []
        # Formatted text fragments of matched items
        self.completion_fragments: List[StyleAndTextTuples] = []

        # Current selected item tracking variables
        self.selected_item: int = 0
        self.selected_item_text: str = ''
        self.selected_item_handler: Optional[Callable] = None
        # Previous input text
        self.last_input_field_text: str = 'EMPTY'
        # Previous selected item
        self.last_selected_item: int = 0

        # Dialog width, height and title
        self.width = width
        self.height = height
        self.window_title: str

        # Callable to fetch completion items
        self.load_completions: Callable[[], List[Tuple[str, Callable]]]

        # Command runner text input field
        self.input_field = TextArea(
            prompt=[
                (
                    'class:command-runner-setting',
                    '> ',
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
            accept_handler=self._command_accept_handler,
            history=InMemoryHistory(),
        )
        # Set additional keybindings for the input field
        self.input_field.control.key_bindings = self._create_key_bindings()

        # Container for the Cancel and Run buttons
        input_field_buttons_container = ConditionalContainer(
            Window(
                content=FormattedTextControl(
                    self._get_input_field_button_fragments,
                    focusable=False,
                    show_cursor=False,
                ),
                height=1,
                align=WindowAlign.RIGHT,
                dont_extend_width=True,
            ),
            filter=Condition(lambda: self.content_width() > 40),
        )

        # Container for completion matches
        command_items_window = Window(
            content=FormattedTextControl(
                self.render_completion_items,
                show_cursor=False,
                focusable=False,
            ),
            align=WindowAlign.LEFT,
            dont_extend_width=False,
            height=self.height,
        )

        # Main content HSplit
        self.command_runner_content = HSplit(
            [
                # Input field and buttons on the same line
                VSplit(
                    [
                        self.input_field,
                        input_field_buttons_container,
                    ]
                ),
                # Completion items below
                command_items_window,
            ],
            style='class:command-runner class:theme-fg-default',
        )

        # Set completions if passed in.
        self.set_completions(window_title, load_completions)

        # bordered_content wraps the above command_runner_content in a border.
        self.bordered_content: AnyContainer
        # Root prompt_toolkit container
        self.container = ConditionalContainer(
            DynamicContainer(lambda: self.bordered_content),
            filter=Condition(lambda: self.show_dialog),
        )

    def _create_bordered_content(self) -> None:
        """Wrap self.command_runner_content in a border."""
        # This should be called whenever the window_title changes.
        self.bordered_content = create_border(
            self.command_runner_content,
            title=self.window_title,
            border_style='class:command-runner-border',
            left_margin_columns=1,
            right_margin_columns=1,
        )

    def __pt_container__(self) -> AnyContainer:
        """Return the prompt_toolkit root container for this dialog."""
        return self.container

    def _create_key_bindings(self) -> KeyBindingsBase:
        """Create additional key bindings for the command input field."""
        key_bindings = KeyBindings()
        register = self.application.prefs.register_keybinding

        @register('command-runner.cancel', key_bindings)
        def _cancel(_event: KeyPressEvent) -> None:
            """Clear input or close command."""
            if self._get_input_field_text() != '':
                self._reset_selected_item()
                return

            self.close_dialog()

        @register('command-runner.select-previous-item', key_bindings)
        def _select_previous_item(_event: KeyPressEvent) -> None:
            """Select previous completion item."""
            self._previous_item()

        @register('command-runner.select-next-item', key_bindings)
        def _select_next_item(_event: KeyPressEvent) -> None:
            """Select next completion item."""
            self._next_item()

        return key_bindings

    def content_width(self) -> int:
        """Return the smaller value of self.width and the available width."""
        window_manager_width = (
            self.application.window_manager.current_window_manager_width
        )
        if not window_manager_width:
            window_manager_width = self.width
        return min(self.width, window_manager_width)

    def focus_self(self) -> None:
        self.application.layout.focus(self)

    def close_dialog(self) -> None:
        """Close command runner dialog box."""
        self.show_dialog = False
        self._reset_selected_item()

        # Restore original focus if possible.
        if self.last_focused_pane:
            self.application.focus_on_container(self.last_focused_pane)
        else:
            # Fallback to focusing on the main menu.
            self.application.focus_main_menu()

    def open_dialog(self) -> None:
        self.show_dialog = True
        self.last_focused_pane = self.application.focused_window()
        self.focus_self()
        self.application.redraw_ui()

    def set_completions(
        self,
        window_title: Optional[str] = None,
        load_completions: Optional[
            Callable[[], List[Tuple[str, Callable]]]
        ] = None,
    ) -> None:
        """Set window title and callable to fetch possible completions.

        Call this function whenever new completion items need to be loaded.
        """
        self.window_title = window_title if window_title else 'Menu Items'
        self.load_completions = (
            load_completions if load_completions else self.load_menu_items
        )
        self._reset_selected_item()

        self.completions = []
        self.completion_fragments = []

        # Load and filter completions
        self.filter_completions()

        # (Re)create the bordered content with the window_title set.
        self._create_bordered_content()

    def reload_completions(self) -> None:
        self.completions = self.load_completions()

    def load_menu_items(self) -> List[Tuple[str, Callable]]:
        # pylint: disable=no-self-use
        return list(flatten_menu_items(self.application.menu_items))

    def _get_input_field_text(self) -> str:
        return self.input_field.buffer.text

    def _make_regexes(self, input_text) -> List[re.Pattern]:
        # pylint: disable=no-self-use
        regexes: List[re.Pattern] = []
        if not input_text:
            return regexes

        text_tokens = input_text.split(' ')
        if len(text_tokens) > 0:
            regexes = [
                re.compile(re.escape(text), re.IGNORECASE)
                for text in text_tokens
            ]

        return regexes

    def _matches_orderless(self, regexes: List[re.Pattern], text) -> bool:
        """Check if all supplied regexs match the input text."""
        # pylint: disable=no-self-use
        return all(regex.search(text) for regex in regexes)

    def filter_completions(self) -> None:
        """Filter completion items if new user input detected."""
        if not self.input_text_changed() and not self.selected_item_changed():
            return

        self.reload_completions()

        input_text = self._get_input_field_text()
        self.completion_fragments = []

        regexes = self._make_regexes(input_text)
        check_match = self._matches_orderless

        i = 0
        for text, handler in self.completions:
            if not (input_text == '' or check_match(regexes, text)):
                continue
            style = ''
            if i == self.selected_item:
                style = 'class:command-runner-selected-item'
                self.selected_item_text = text
                self.selected_item_handler = handler
                text = text.ljust(self.content_width())
            fragments: StyleAndTextTuples = highlight_matches(
                regexes, [(style, text + '\n')]
            )
            self.completion_fragments.append(fragments)
            i += 1

    def input_text_changed(self) -> bool:
        """Return True if text in the input field has changed."""
        input_text = self._get_input_field_text()
        if input_text != self.last_input_field_text:
            self.last_input_field_text = input_text
            self.selected_item = 0
            return True
        return False

    def selected_item_changed(self) -> bool:
        """Check if the user pressed up or down to select a different item."""
        return self.last_selected_item != self.selected_item

    def _next_item(self) -> None:
        self.last_selected_item = self.selected_item
        self.selected_item = min(
            # Don't move past the height of the window or the length of possible
            # items.
            min(self.height, len(self.completion_fragments)) - 1,
            self.selected_item + 1,
        )
        self.application.redraw_ui()

    def _previous_item(self) -> None:
        self.last_selected_item = self.selected_item
        self.selected_item = max(0, self.selected_item - 1)
        self.application.redraw_ui()

    def _get_input_field_button_fragments(self) -> StyleAndTextTuples:
        # Mouse handlers
        focus = functools.partial(mouse_handlers.on_click, self.focus_self)
        cancel = functools.partial(mouse_handlers.on_click, self.close_dialog)
        select_item = functools.partial(
            mouse_handlers.on_click, self._run_selected_item
        )

        separator_text = ('', ' ', focus)

        # Default button style
        button_style = 'class:toolbar-button-inactive'

        fragments: StyleAndTextTuples = []

        # Cancel button
        fragments.extend(
            to_keybind_indicator(
                key='Ctrl-c',
                description='Cancel',
                mouse_handler=cancel,
                base_style=button_style,
            )
        )
        fragments.append(separator_text)

        # Run button
        fragments.extend(
            to_keybind_indicator(
                'Enter', 'Run', select_item, base_style=button_style
            )
        )
        return fragments

    def render_completion_items(self) -> StyleAndTextTuples:
        """Render completion items."""
        fragments: StyleAndTextTuples = []

        # Update completions if any state change since the last render (new text
        # entered or arrow keys pressed).
        self.filter_completions()

        for completion_item in self.completion_fragments:
            fragments.extend(completion_item)

        return fragments

    def _reset_selected_item(self) -> None:
        self.selected_item = 0
        self.last_selected_item = 0
        self.selected_item_text = ''
        self.selected_item_handler = None
        self.last_input_field_text = 'EMPTY'
        self.input_field.buffer.reset()

    def _run_selected_item(self) -> None:
        """Run the selected action."""
        if not self.selected_item_handler:
            return
        # Save the selected item handler. This is reset by self.close_dialog()
        handler = self.selected_item_handler

        # Depending on what action is run, the command runner dialog may need to
        # be closed, left open, or closed before running the selected action.
        close_dialog = True
        close_dialog_first = False

        # Actions that launch new command runners, close_dialog should not run.
        for command_text in [
            '[File] > Insert Repl Snippet',
            '[File] > Insert Repl History',
            '[File] > Open Logger',
        ]:
            if command_text in self.selected_item_text:
                close_dialog = False
                break

        # Actions that change what is in focus should be run after closing the
        # command runner dialog.
        for command_text in [
            '[File] > Games > ',
            '[View] > Focus Next Window/Tab',
            '[View] > Focus Prev Window/Tab',
            # All help menu entries open popup windows.
            '[Help] > ',
            # This focuses on a save dialog bor.
            'Save/Export a copy',
            '[Windows] > Floating ',
        ]:
            if command_text in self.selected_item_text:
                close_dialog_first = True
                break

        # Close first if needed
        if close_dialog and close_dialog_first:
            self.close_dialog()

        # Run the selected item handler
        handler()

        # If not already closed earlier.
        if close_dialog and not close_dialog_first:
            self.close_dialog()

    def _command_accept_handler(self, _buff: Buffer) -> bool:
        """Function run when pressing Enter in the command runner input box."""
        # If at least one match is available
        if len(self.completion_fragments) > 0:
            self._run_selected_item()
            # Erase input text
            return False
        # Keep input text
        return True
