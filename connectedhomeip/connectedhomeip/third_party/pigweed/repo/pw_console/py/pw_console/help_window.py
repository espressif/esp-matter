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
"""Help window container class."""

import functools
import importlib.resources
import inspect
import logging
from typing import Dict, TYPE_CHECKING

from prompt_toolkit.document import Document
from prompt_toolkit.filters import Condition
from prompt_toolkit.key_binding import KeyBindings, KeyPressEvent
from prompt_toolkit.layout import (
    ConditionalContainer,
    DynamicContainer,
    FormattedTextControl,
    HSplit,
    VSplit,
    Window,
    WindowAlign,
)
from prompt_toolkit.layout.dimension import Dimension
from prompt_toolkit.lexers import PygmentsLexer
from prompt_toolkit.widgets import Box, TextArea

from pygments.lexers.markup import RstLexer  # type: ignore
from pygments.lexers.data import YamlLexer  # type: ignore

from pw_console.style import (
    get_pane_indicator,
)
from pw_console.widgets import (
    mouse_handlers,
    to_keybind_indicator,
)

if TYPE_CHECKING:
    from pw_console.console_app import ConsoleApp

_LOG = logging.getLogger(__package__)

_PW_CONSOLE_MODULE = 'pw_console'


def _longest_line_length(text):
    """Return the longest line in the given text."""
    max_line_length = 0
    for line in text.splitlines():
        if len(line) > max_line_length:
            max_line_length = len(line)
    return max_line_length


class HelpWindow(ConditionalContainer):
    """Help window container for displaying keybindings."""

    # pylint: disable=too-many-instance-attributes

    def _create_help_text_area(self, **kwargs):
        help_text_area = TextArea(
            focusable=True,
            focus_on_click=True,
            scrollbar=True,
            style='class:help_window_content',
            wrap_lines=False,
            **kwargs,
        )

        # Additional keybindings for the text area.
        key_bindings = KeyBindings()
        register = self.application.prefs.register_keybinding

        @register('help-window.close', key_bindings)
        def _close_window(_event: KeyPressEvent) -> None:
            """Close the current dialog window."""
            self.toggle_display()

        @register('help-window.copy-all', key_bindings)
        def _copy_all(_event: KeyPressEvent) -> None:
            """Close the current dialog window."""
            self.copy_all_text()

        help_text_area.control.key_bindings = key_bindings
        return help_text_area

    def __init__(
        self,
        application: 'ConsoleApp',
        preamble: str = '',
        additional_help_text: str = '',
        title: str = '',
    ) -> None:
        # Dict containing key = section title and value = list of key bindings.
        self.application: 'ConsoleApp' = application
        self.show_window: bool = False
        self.help_text_sections: Dict[str, Dict] = {}
        self._pane_title: str = title

        # Tracks the last focused container, to enable restoring focus after
        # closing the dialog.
        self.last_focused_pane = None

        # Generated keybinding text
        self.preamble: str = preamble
        self.additional_help_text: str = additional_help_text
        self.help_text: str = ''

        self.max_additional_help_text_width: int = (
            _longest_line_length(self.additional_help_text)
            if additional_help_text
            else 0
        )
        self.max_description_width: int = 0
        self.max_key_list_width: int = 0
        self.max_line_length: int = 0

        self.help_text_area: TextArea = self._create_help_text_area()

        close_mouse_handler = functools.partial(
            mouse_handlers.on_click, self.toggle_display
        )
        copy_mouse_handler = functools.partial(
            mouse_handlers.on_click, self.copy_all_text
        )

        toolbar_padding = 1
        toolbar_title = ' ' * toolbar_padding
        toolbar_title += self.pane_title()

        buttons = []
        buttons.extend(
            to_keybind_indicator(
                'Ctrl-c',
                'Copy All',
                copy_mouse_handler,
                base_style='class:toolbar-button-active',
            )
        )
        buttons.append(('', '  '))
        buttons.extend(
            to_keybind_indicator(
                'q',
                'Close',
                close_mouse_handler,
                base_style='class:toolbar-button-active',
            )
        )
        top_toolbar = VSplit(
            [
                Window(
                    content=FormattedTextControl(
                        # [('', toolbar_title)]
                        functools.partial(
                            get_pane_indicator,
                            self,
                            toolbar_title,
                        )
                    ),
                    align=WindowAlign.LEFT,
                    dont_extend_width=True,
                ),
                Window(
                    content=FormattedTextControl([]),
                    align=WindowAlign.LEFT,
                    dont_extend_width=False,
                ),
                Window(
                    content=FormattedTextControl(buttons),
                    align=WindowAlign.RIGHT,
                    dont_extend_width=True,
                ),
            ],
            height=1,
            style='class:toolbar_active',
        )

        self.container = HSplit(
            [
                top_toolbar,
                Box(
                    body=DynamicContainer(lambda: self.help_text_area),
                    padding=Dimension(preferred=1, max=1),
                    padding_bottom=0,
                    padding_top=0,
                    char=' ',
                    style='class:frame.border',  # Same style used for Frame.
                ),
            ]
        )

        super().__init__(
            self.container,
            filter=Condition(lambda: self.show_window),
        )

    def pane_title(self):
        return self._pane_title

    def menu_title(self):
        """Return the title to display in the Window menu."""
        return self.pane_title()

    def __pt_container__(self):
        """Return the prompt_toolkit container for displaying this HelpWindow.

        This allows self to be used wherever prompt_toolkit expects a container
        object."""
        return self.container

    def copy_all_text(self):
        """Copy all text in the Python input to the system clipboard."""
        self.application.application.clipboard.set_text(
            self.help_text_area.buffer.text
        )

    def toggle_display(self):
        """Toggle visibility of this help window."""
        # Toggle state variable.
        self.show_window = not self.show_window

        if self.show_window:
            # Save previous focus
            self.last_focused_pane = self.application.focused_window()
            # Set the help window in focus.
            self.application.layout.focus(self.help_text_area)
        else:
            # Restore original focus if possible.
            if self.last_focused_pane:
                self.application.layout.focus(self.last_focused_pane)
            else:
                # Fallback to focusing on the first window pane.
                self.application.focus_main_menu()

    def content_width(self) -> int:
        """Return total width of help window."""
        # Widths of UI elements
        frame_width = 1
        padding_width = 1
        left_side_frame_and_padding_width = frame_width + padding_width
        right_side_frame_and_padding_width = frame_width + padding_width
        scrollbar_padding = 1
        scrollbar_width = 1

        desired_width = self.max_line_length + (
            left_side_frame_and_padding_width
            + right_side_frame_and_padding_width
            + scrollbar_padding
            + scrollbar_width
        )
        desired_width = max(60, desired_width)

        window_manager_width = (
            self.application.window_manager.current_window_manager_width
        )
        if not window_manager_width:
            window_manager_width = 80
        return min(desired_width, window_manager_width)

    def load_user_guide(self):
        rstdoc_text = importlib.resources.read_text(
            f'{_PW_CONSOLE_MODULE}.docs', 'user_guide.rst'
        )
        max_line_length = 0
        rst_text = ''
        for line in rstdoc_text.splitlines():
            if 'https://' not in line and len(line) > max_line_length:
                max_line_length = len(line)
            rst_text += line + '\n'
        self.max_line_length = max_line_length

        self.help_text_area = self._create_help_text_area(
            lexer=PygmentsLexer(RstLexer),
            text=rst_text,
        )

    def load_yaml_text(self, content: str):
        max_line_length = 0
        for line in content.splitlines():
            if 'https://' not in line and len(line) > max_line_length:
                max_line_length = len(line)
        self.max_line_length = max_line_length

        self.help_text_area = self._create_help_text_area(
            lexer=PygmentsLexer(YamlLexer),
            text=content,
        )

    def generate_help_text(self):
        """Generate help text based on added key bindings."""

        template = self.application.get_template('keybind_list.jinja')

        self.help_text = template.render(
            sections=self.help_text_sections,
            max_additional_help_text_width=self.max_additional_help_text_width,
            max_description_width=self.max_description_width,
            max_key_list_width=self.max_key_list_width,
            preamble=self.preamble,
            additional_help_text=self.additional_help_text,
        )

        # Find the longest line in the rendered template.
        self.max_line_length = _longest_line_length(self.help_text)

        # Replace the TextArea content.
        self.help_text_area.buffer.document = Document(
            text=self.help_text, cursor_position=0
        )

        return self.help_text

    def add_custom_keybinds_help_text(self, section_name, key_bindings: Dict):
        """Add hand written key_bindings."""
        self.help_text_sections[section_name] = key_bindings

    def add_keybind_help_text(self, section_name, key_bindings: KeyBindings):
        """Append formatted key binding text to this help window."""

        # Create a new keybind section, erasing any old section with thesame
        # title.
        self.help_text_sections[section_name] = {}

        # Loop through passed in prompt_toolkit key_bindings.
        for binding in key_bindings.bindings:
            # Skip this keybind if the method name ends in _hidden.
            if binding.handler.__name__.endswith('_hidden'):
                continue

            # Get the key binding description from the function doctstring.
            docstring = binding.handler.__doc__
            if not docstring:
                docstring = ''
            description = inspect.cleandoc(docstring)
            description = description.replace('\n', ' ')

            # Save the length of the description.
            if len(description) > self.max_description_width:
                self.max_description_width = len(description)

            # Get the existing list of keys for this function or make a new one.
            key_list = self.help_text_sections[section_name].get(
                description, list()
            )

            # Save the name of the key e.g. F1, q, ControlQ, ControlUp
            key_name = ' '.join(
                [getattr(key, 'name', str(key)) for key in binding.keys]
            )
            key_name = key_name.replace('Control', 'Ctrl-')
            key_name = key_name.replace('Shift', 'Shift-')
            key_name = key_name.replace('Escape ', 'Alt-')
            key_name = key_name.replace('Alt-Ctrl-', 'Ctrl-Alt-')
            key_name = key_name.replace('BackTab', 'Shift-Tab')
            key_list.append(key_name)

            key_list_width = len(', '.join(key_list))
            # Save the length of the key list.
            if key_list_width > self.max_key_list_width:
                self.max_key_list_width = key_list_width

            # Update this functions key_list
            self.help_text_sections[section_name][description] = key_list
