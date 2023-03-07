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
"""ReplPane class."""

import asyncio
import concurrent
import functools
import logging
import pprint
from dataclasses import dataclass
from typing import (
    Any,
    Callable,
    Dict,
    List,
    Optional,
    Tuple,
    TYPE_CHECKING,
    Union,
)

from prompt_toolkit.filters import (
    Condition,
    has_focus,
)
from prompt_toolkit.document import Document
from prompt_toolkit.key_binding import KeyBindings, KeyPressEvent
from prompt_toolkit.layout.dimension import AnyDimension
from prompt_toolkit.widgets import TextArea
from prompt_toolkit.layout import (
    ConditionalContainer,
    DynamicContainer,
    Dimension,
    FloatContainer,
    HSplit,
    Window,
)
from prompt_toolkit.lexers import PygmentsLexer  # type: ignore
from pygments.lexers.python import PythonConsoleLexer  # type: ignore

# Alternative Formatting
# from IPython.lib.lexers import IPythonConsoleLexer  # type: ignore

from pw_console.progress_bar.progress_bar_state import TASKS_CONTEXTVAR
from pw_console.pw_ptpython_repl import PwPtPythonRepl
from pw_console.style import get_pane_style
from pw_console.widgets import (
    ToolbarButton,
    WindowPane,
    WindowPaneHSplit,
    WindowPaneToolbar,
)

if TYPE_CHECKING:
    from pw_console.console_app import ConsoleApp

_LOG = logging.getLogger(__package__)

_Namespace = Dict[str, Any]
_GetNamespace = Callable[[], _Namespace]

_REPL_OUTPUT_SCROLL_AMOUNT = 5


@dataclass
class UserCodeExecution:
    """Class to hold a single user repl execution event."""

    input: str
    future: concurrent.futures.Future
    output: str
    stdout: str
    stderr: str
    stdout_check_task: Optional[concurrent.futures.Future] = None
    result_object: Optional[Any] = None
    exception_text: Optional[str] = None

    @property
    def is_running(self):
        return not self.future.done()

    def update_stdout(self, text: Optional[str]):
        if text:
            self.stdout = text

    def update_stderr(self, text: Optional[str]):
        if text:
            self.stderr = text


class ReplPane(WindowPane):
    """Pane for reading Python input."""

    # pylint: disable=too-many-instance-attributes,too-many-public-methods
    def __init__(
        self,
        application: 'ConsoleApp',
        python_repl: PwPtPythonRepl,
        pane_title: str = 'Python Repl',
        startup_message: Optional[str] = None,
    ) -> None:
        super().__init__(application, pane_title)

        self.executed_code: List = []
        self.application = application

        self.pw_ptpython_repl = python_repl
        self.pw_ptpython_repl.set_repl_pane(self)

        self.wrap_output_lines = True

        self.startup_message = startup_message if startup_message else ''

        self.output_field = TextArea(
            text=self.startup_message,
            focusable=True,
            focus_on_click=True,
            scrollbar=True,
            wrap_lines=Condition(lambda: self.wrap_output_lines),
            lexer=PygmentsLexer(PythonConsoleLexer),
        )

        # Additional keybindings for the text area.
        key_bindings = KeyBindings()
        register = self.application.prefs.register_keybinding

        @register('python-repl.copy-output-selection', key_bindings)
        def _copy_selection(_event: KeyPressEvent) -> None:
            """Copy selected text."""
            self.copy_output_selection()

        self.output_field.control.key_bindings = key_bindings

        # Override output buffer mouse wheel scroll
        self.output_field.window._scroll_up = (  # type: ignore
            self.scroll_output_up
        )
        self.output_field.window._scroll_down = (  # type: ignore
            self.scroll_output_down
        )

        self.bottom_toolbar = self._create_input_toolbar()
        self.results_toolbar = self._create_output_toolbar()

        self.progress_state = TASKS_CONTEXTVAR.get()

        # ReplPane root container
        self.container = ConditionalContainer(
            FloatContainer(
                # Horizontal split of all Repl pane sections.
                WindowPaneHSplit(
                    self,
                    [
                        HSplit(
                            [
                                # 1. Repl Output
                                self.output_field,
                                # 2. Progress bars if any
                                ConditionalContainer(
                                    DynamicContainer(
                                        self.get_progress_bar_task_container
                                    ),
                                    filter=Condition(
                                        # pylint: disable=line-too-long
                                        lambda: not self.progress_state.all_tasks_complete
                                        # pylint: enable=line-too-long
                                    ),
                                ),
                                # 3. Static separator toolbar.
                                self.results_toolbar,
                            ],
                            # Output area only dimensions
                            height=self.get_output_height,
                        ),
                        HSplit(
                            [
                                # 3. Repl Input
                                self.pw_ptpython_repl,
                                # 4. Bottom toolbar
                                self.bottom_toolbar,
                            ],
                            # Input area only dimensions
                            height=self.get_input_height,
                        ),
                    ],
                    # Repl pane dimensions
                    height=lambda: self.height,
                    width=lambda: self.width,
                    style=functools.partial(get_pane_style, self),
                ),
                floats=[],
            ),
            filter=Condition(lambda: self.show_pane),
        )

    def toggle_wrap_output_lines(self):
        """Enable or disable output line wraping/truncation."""
        self.wrap_output_lines = not self.wrap_output_lines

    def scroll_output_down(self) -> None:
        """Scroll the output buffer down on mouse wheel down events."""
        for _i in range(_REPL_OUTPUT_SCROLL_AMOUNT):
            # There is no move cursor more than one line at a time function.
            self.output_field.control.move_cursor_down()
        self.output_field.window.vertical_scroll += _REPL_OUTPUT_SCROLL_AMOUNT

    def scroll_output_up(self) -> None:
        """Scroll the output buffer up on mouse wheel up events."""
        for _i in range(_REPL_OUTPUT_SCROLL_AMOUNT):
            # There is no move cursor more than one line at a time function.
            self.output_field.control.move_cursor_up()
        self.output_field.window.vertical_scroll -= _REPL_OUTPUT_SCROLL_AMOUNT

    def focus_output(self):
        self.application.focus_on_container(self.output_field)

    def focus_input(self):
        self.application.focus_on_container(self.pw_ptpython_repl)

    def get_progress_bar_task_container(self):
        bar_container = self.progress_state.get_container()
        if bar_container:
            return bar_container
        return Window()

    def get_output_height(self) -> AnyDimension:
        # pylint: disable=no-self-use
        return Dimension(min=1)

    def get_input_height(self) -> AnyDimension:
        desired_max_height = 10
        # Check number of line breaks in the input buffer.
        input_line_count = self.pw_ptpython_repl.line_break_count()
        if input_line_count > desired_max_height:
            desired_max_height = input_line_count
        # Check if it's taller than the available space
        if desired_max_height > self.current_pane_height:
            # Leave space for minimum of
            #   1 line of content in the output
            #   + 1 for output toolbar
            #   + 1 for input toolbar
            desired_max_height = self.current_pane_height - 3

        if desired_max_height > 1:
            return Dimension(min=1, max=desired_max_height)
        # Fall back to at least a height of 1
        return Dimension(min=1)

    def _create_input_toolbar(self):
        bottom_toolbar = WindowPaneToolbar(
            self,
            focus_action_callable=self.focus_input,
            focus_check_container=self.pw_ptpython_repl,
        )
        bottom_toolbar.add_button(
            ToolbarButton(
                'Ctrl-v', 'Paste', self.paste_system_clipboard_to_input_buffer
            )
        )
        bottom_toolbar.add_button(
            ToolbarButton(
                'Ctrl-c', 'Copy / Clear', self.copy_or_clear_input_buffer
            )
        )
        bottom_toolbar.add_button(ToolbarButton('Enter', 'Run', self.run_code))
        bottom_toolbar.add_button(ToolbarButton('F2', 'Settings'))
        bottom_toolbar.add_button(ToolbarButton('F3', 'History'))
        return bottom_toolbar

    def _create_output_toolbar(self):
        results_toolbar = WindowPaneToolbar(
            self,
            title='Python Results',
            focus_action_callable=self.focus_output,
            focus_check_container=self.output_field,
            include_resize_handle=False,
        )
        results_toolbar.add_button(
            ToolbarButton(
                description='Wrap lines',
                mouse_handler=self.toggle_wrap_output_lines,
                is_checkbox=True,
                checked=lambda: self.wrap_output_lines,
            )
        )
        results_toolbar.add_button(
            ToolbarButton(
                'Ctrl-Alt-c', 'Copy All Output', self.copy_all_output_text
            )
        )
        results_toolbar.add_button(
            ToolbarButton(
                'Ctrl-c', 'Copy Selected Text', self.copy_output_selection
            )
        )

        results_toolbar.add_button(
            ToolbarButton(
                description='Clear', mouse_handler=self.clear_output_buffer
            )
        )
        results_toolbar.add_button(
            ToolbarButton('Shift+Arrows / Mouse Drag', 'Select Text')
        )

        return results_toolbar

    def copy_output_selection(self):
        """Copy highlighted output text to the system clipboard."""
        clipboard_data = self.output_field.buffer.copy_selection()
        self.application.application.clipboard.set_data(clipboard_data)

    def copy_input_selection(self):
        """Copy highlighted input text to the system clipboard."""
        clipboard_data = self.pw_ptpython_repl.default_buffer.copy_selection()
        self.application.application.clipboard.set_data(clipboard_data)

    def copy_all_output_text(self):
        """Copy all text in the Python output to the system clipboard."""
        self.application.application.clipboard.set_text(
            self.output_field.buffer.text
        )

    def copy_all_input_text(self):
        """Copy all text in the Python input to the system clipboard."""
        self.application.application.clipboard.set_text(
            self.pw_ptpython_repl.default_buffer.text
        )

    # pylint: disable=no-self-use
    def get_all_key_bindings(self) -> List:
        """Return all keybinds for this plugin."""
        # ptpython native bindings:
        # return [load_python_bindings(self.pw_ptpython_repl)]

        # Hand-crafted bindings for display in the HelpWindow:
        return [
            {
                'Execute code': ['Enter', 'Option-Enter', 'Alt-Enter'],
                'Reverse search history': ['Ctrl-r'],
                'Erase input buffer.': ['Ctrl-c'],
                'Show settings.': ['F2'],
                'Show history.': ['F3'],
            }
        ]

    def get_window_menu_options(
        self,
    ) -> List[Tuple[str, Union[Callable, None]]]:
        return [
            (
                'Python Input > Paste',
                self.paste_system_clipboard_to_input_buffer,
            ),
            ('Python Input > Copy or Clear', self.copy_or_clear_input_buffer),
            ('Python Input > Run', self.run_code),
            # Menu separator
            ('-', None),
            (
                'Python Output > Toggle Wrap lines',
                self.toggle_wrap_output_lines,
            ),
            ('Python Output > Copy All', self.copy_all_output_text),
            ('Python Output > Copy Selection', self.copy_output_selection),
            ('Python Output > Clear', self.clear_output_buffer),
        ]

    def run_code(self):
        """Trigger a repl code execution on mouse click."""
        self.pw_ptpython_repl.default_buffer.validate_and_handle()

    def ctrl_c(self):
        """Ctrl-C keybinding behavior."""
        # If there is text in the input buffer
        if self.pw_ptpython_repl.default_buffer.text:
            self.copy_or_clear_input_buffer()
        else:
            self.interrupt_last_code_execution()

    def insert_text_into_input_buffer(self, text: str) -> None:
        self.pw_ptpython_repl.default_buffer.insert_text(text)

    def paste_system_clipboard_to_input_buffer(self, erase_buffer=False):
        if erase_buffer:
            self.clear_input_buffer()

        clip_data = self.application.application.clipboard.get_data()
        self.pw_ptpython_repl.default_buffer.paste_clipboard_data(clip_data)

    def clear_input_buffer(self):
        # Erase input buffer.
        self.pw_ptpython_repl.default_buffer.reset()
        # Clear any displayed function signatures.
        self.pw_ptpython_repl.on_reset()

    def clear_output_buffer(self):
        self.executed_code.clear()
        self.update_output_buffer()

    def copy_or_clear_input_buffer(self):
        # Copy selected text if a selection is active.
        if self.pw_ptpython_repl.default_buffer.selection_state:
            self.copy_input_selection()
            return
        # Otherwise, clear the input buffer
        self.clear_input_buffer()

    def interrupt_last_code_execution(self):
        code = self._get_currently_running_code()
        if code:
            code.future.cancel()
            code.output = 'Canceled'
            self.progress_state.cancel_all_tasks()
        self.pw_ptpython_repl.clear_last_result()
        self.update_output_buffer('repl_pane.interrupt_last_code_execution')

    def _get_currently_running_code(self):
        for code in self.executed_code:
            if not code.future.done():
                return code
        return None

    def _get_executed_code(self, future):
        for code in self.executed_code:
            if code.future == future:
                return code
        return None

    def _log_executed_code(self, code, prefix=''):
        """Log repl command input text along with a prefix string."""
        text = self.get_output_buffer_text([code], show_index=False)
        text = text.strip()
        for line in text.splitlines():
            _LOG.debug('[PYTHON %s]  %s', prefix, line.strip())

    async def periodically_check_stdout(
        self, user_code: UserCodeExecution, stdout_proxy, stderr_proxy
    ):
        while not user_code.future.done():
            await asyncio.sleep(0.3)
            stdout_text_so_far = stdout_proxy.getvalue()
            stderr_text_so_far = stderr_proxy.getvalue()
            if stdout_text_so_far:
                user_code.update_stdout(stdout_text_so_far)
            if stderr_text_so_far:
                user_code.update_stderr(stderr_text_so_far)

            # if stdout_text_so_far or stderr_text_so_far:
            self.update_output_buffer('repl_pane.periodic_check')

    def append_executed_code(self, text, future, temp_stdout, temp_stderr):
        user_code = UserCodeExecution(
            input=text, future=future, output=None, stdout=None, stderr=None
        )

        background_stdout_check = asyncio.create_task(
            self.periodically_check_stdout(user_code, temp_stdout, temp_stderr)
        )
        user_code.stdout_check_task = background_stdout_check
        self.executed_code.append(user_code)
        self._log_executed_code(user_code, prefix='START')

    def append_result_to_executed_code(
        self,
        _input_text,
        future,
        result_text,
        stdout_text='',
        stderr_text='',
        exception_text='',
        result_object=None,
    ):

        code = self._get_executed_code(future)
        if code:
            code.output = result_text
            code.stdout = stdout_text
            code.stderr = stderr_text
            code.exception_text = exception_text
            code.result_object = result_object
        self._log_executed_code(code, prefix='FINISH')
        self.update_output_buffer('repl_pane.append_result_to_executed_code')

    def get_output_buffer_text(self, code_items=None, show_index=True):
        content_width = (
            self.current_pane_width if self.current_pane_width else 80
        )
        pprint_respecting_width = pprint.PrettyPrinter(
            indent=2, width=content_width
        ).pformat

        executed_code = code_items or self.executed_code

        template = self.application.get_template('repl_output.jinja')
        return template.render(
            code_items=executed_code,
            result_format=pprint_respecting_width,
            show_index=show_index,
        )

    def update_output_buffer(self, *unused_args):
        text = self.get_output_buffer_text()
        # Add an extra line break so the last cursor position is in column 0
        # instead of the end of the last line.
        text += '\n'
        self.output_field.buffer.set_document(
            Document(text=text, cursor_position=len(text))
        )

        self.application.redraw_ui()

    def input_or_output_has_focus(self) -> Condition:
        @Condition
        def test() -> bool:
            if (
                has_focus(self.output_field)()
                or has_focus(self.pw_ptpython_repl)()
            ):
                return True
            return False

        return test

    def history_completions(self) -> List[Tuple[str, str]]:
        return [
            (
                ' '.join([line.lstrip() for line in text.splitlines()]),
                # Pass original text as the completion result.
                text,
            )
            for text in list(
                self.pw_ptpython_repl.history.load_history_strings()
            )
        ]
