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
"""Example text input-output Plugin."""

from typing import TYPE_CHECKING

from prompt_toolkit.document import Document
from prompt_toolkit.key_binding import KeyBindings, KeyPressEvent
from prompt_toolkit.layout import Window
from prompt_toolkit.widgets import SearchToolbar, TextArea

from pw_console.widgets import ToolbarButton, WindowPane, WindowPaneToolbar

if TYPE_CHECKING:
    from pw_console.console_app import ConsoleApp


class CalcPane(WindowPane):
    """Example plugin that accepts text input and displays output.

    This plugin is similar to the full-screen calculator example provided in
    prompt_toolkit:
    https://github.com/prompt-toolkit/python-prompt-toolkit/blob/3.0.23/examples/full-screen/calculator.py

    It's a full window that can be moved around the user interface like other
    Pigweed Console window panes. An input prompt is displayed on the bottom of
    the window where the user can type in some math equation. When the enter key
    is pressed the input is processed and the result shown in the top half of
    the window.

    Both input and output fields are prompt_toolkit TextArea objects which can
    have their own options like syntax highlighting.
    """

    def __init__(self):
        # Call WindowPane.__init__ and set the title to 'Calculator'
        super().__init__(pane_title='Calculator')

        # Create a TextArea for the output-field
        # TextArea is a prompt_toolkit widget that can display editable text in
        # a buffer. See the prompt_toolkit docs for all possible options:
        # https://python-prompt-toolkit.readthedocs.io/en/latest/pages/reference.html#prompt_toolkit.widgets.TextArea
        self.output_field = TextArea(
            # Optional Styles to apply to this TextArea
            style='class:output-field',
            # Initial text to put into the buffer.
            text='Calculator Output',
            # Allow this buffer to be in focus. This lets you drag select text
            # contained inside, and edit the contents unless readonly.
            focusable=True,
            # Focus on mouse click.
            focus_on_click=True,
        )

        # This is the search toolbar and only appears if the user presses ctrl-r
        # to do reverse history search (similar to bash or zsh). Its used by the
        # input_field below.
        self.search_field = SearchToolbar()

        # Create a TextArea for the user input.
        self.input_field = TextArea(
            # The height is set to 1 line
            height=1,
            # Prompt string that appears before the cursor.
            prompt='>>> ',
            # Optional Styles to apply to this TextArea
            style='class:input-field',
            # We only allow one line input for this example but multiline is
            # supported by prompt_toolkit.
            multiline=False,
            wrap_lines=False,
            # Allow reverse history search
            search_field=self.search_field,
            # Allow this input to be focused.
            focusable=True,
            # Focus on mouse click.
            focus_on_click=True,
        )

        # The TextArea accept_handler function is called by prompt_toolkit (the
        # UI) when the user presses enter. Here we override it to our own accept
        # handler defined in this CalcPane class.
        self.input_field.accept_handler = self.accept_input

        # Create a toolbar for display at the bottom of this window. It will
        # show the window title and toolbar buttons.
        self.bottom_toolbar = WindowPaneToolbar(self)
        self.bottom_toolbar.add_button(
            ToolbarButton(
                key='Enter',  # Key binding for this function
                description='Run Calculation',  # Button name
                # Function to run when clicked.
                mouse_handler=self.run_calculation,
            )
        )
        self.bottom_toolbar.add_button(
            ToolbarButton(
                key='Ctrl-c',  # Key binding for this function
                description='Copy Output',  # Button name
                # Function to run when clicked.
                mouse_handler=self.copy_all_output,
            )
        )

        # self.container is the root container that contains objects to be
        # rendered in the UI, one on top of the other.
        self.container = self._create_pane_container(
            # Show the output_field on top
            self.output_field,
            # Draw a separator line with height=1
            Window(height=1, char='─', style='class:line'),
            # Show the input field just below that.
            self.input_field,
            # If ctrl-r reverse history is active, show the search box below the
            # input_field.
            self.search_field,
            # Lastly, show the toolbar.
            self.bottom_toolbar,
        )

    def pw_console_init(self, app: 'ConsoleApp') -> None:
        """Set the Pigweed Console application instance.

        This function is called after the Pigweed Console starts up and allows
        access to the user preferences. Prefs is required for creating new
        user-remappable keybinds."""
        self.application = app
        self.set_custom_keybinds()

    def set_custom_keybinds(self) -> None:
        # Fetch ConsoleApp preferences to load user keybindings
        prefs = self.application.prefs
        # Register a named keybind function that is user re-mappable
        prefs.register_named_key_function(
            'calc-pane.copy-selected-text',
            # default bindings
            ['c-c'],
        )

        # For setting additional keybindings to the output_field.
        key_bindings = KeyBindings()

        # Map the 'calc-pane.copy-selected-text' function keybind to the
        # _copy_all_output function below. This will set
        @prefs.register_keybinding('calc-pane.copy-selected-text', key_bindings)
        def _copy_all_output(_event: KeyPressEvent) -> None:
            """Copy selected text from the output buffer."""
            self.copy_selected_output()

        # Set the output_field controls key_bindings to the new bindings.
        self.output_field.control.key_bindings = key_bindings

    def run_calculation(self):
        """Trigger the input_field's accept_handler.

        This has the same effect as pressing enter in the input_field.
        """
        self.input_field.buffer.validate_and_handle()

    def accept_input(self, _buffer):
        """Function run when the user presses enter in the input_field.

        Takes a buffer argument that contains the user's input text.
        """
        # Evaluate the user's calculator expression as Python and format the
        # output result.
        try:
            output = "\n\nIn:  {}\nOut: {}".format(
                self.input_field.text,
                # NOTE: Don't use 'eval' in real code (this is just an example)
                eval(self.input_field.text),  # pylint: disable=eval-used
            )
        except BaseException as exception:  # pylint: disable=broad-except
            output = "\n\n{}".format(exception)

        # Append the new output result to the existing output_field contents.
        new_text = self.output_field.text + output

        # Update the output_field with the new contents and move the
        # cursor_position to the end.
        self.output_field.buffer.document = Document(
            text=new_text, cursor_position=len(new_text)
        )

    def copy_selected_output(self):
        """Copy highlighted text in the output_field to the system clipboard."""
        clipboard_data = self.output_field.buffer.copy_selection()
        self.application.application.clipboard.set_data(clipboard_data)

    def copy_all_output(self):
        """Copy all text in the output_field to the system clipboard."""
        self.application.application.clipboard.set_text(
            self.output_field.buffer.text
        )
