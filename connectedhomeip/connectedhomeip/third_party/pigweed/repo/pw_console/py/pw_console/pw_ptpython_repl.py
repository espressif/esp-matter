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
"""PwPtPythonPane class."""

import asyncio
import functools
import io
import logging
import os
import sys
import shlex
import subprocess
from typing import Iterable, Optional, TYPE_CHECKING
from unittest.mock import patch

from prompt_toolkit.buffer import Buffer
from prompt_toolkit.layout.controls import BufferControl
from prompt_toolkit.completion import merge_completers
from prompt_toolkit.filters import (
    Condition,
    has_focus,
    to_filter,
)
from ptpython.completer import (  # type: ignore
    CompletePrivateAttributes,
    PythonCompleter,
)
import ptpython.repl  # type: ignore
from ptpython.layout import (  # type: ignore
    CompletionVisualisation,
    Dimension,
)
import pygments.plugin

from pw_console.pigweed_code_style import (
    PigweedCodeStyle,
    PigweedCodeLightStyle,
)
from pw_console.text_formatting import remove_formatting

if TYPE_CHECKING:
    from pw_console.repl_pane import ReplPane

_LOG = logging.getLogger(__package__)
_SYSTEM_COMMAND_LOG = logging.getLogger('pw_console_system_command')

_original_find_plugin_styles = pygments.plugin.find_plugin_styles


def _wrapped_find_plugin_styles():
    """Patch pygment find_plugin_styles to also include Pigweed codes styles

    This allows using these themes without requiring Python entrypoints.
    """
    for style in [
        ('pigweed-code', PigweedCodeStyle),
        ('pigweed-code-light', PigweedCodeLightStyle),
    ]:
        yield style
    yield from _original_find_plugin_styles()


class MissingPtpythonBufferControl(Exception):
    """Exception for a missing ptpython BufferControl object."""


def _user_input_is_a_shell_command(text: str) -> bool:
    return text.startswith('!')


class PwPtPythonRepl(
    ptpython.repl.PythonRepl
):  # pylint: disable=too-many-instance-attributes
    """A ptpython repl class with changes to code execution and output related
    methods."""

    @patch(
        'pygments.styles.find_plugin_styles', new=_wrapped_find_plugin_styles
    )
    def __init__(
        self,
        *args,
        # pw_console specific kwargs
        extra_completers: Optional[Iterable] = None,
        **ptpython_kwargs,
    ):

        completer = None
        if extra_completers:
            # Create the default python completer used by
            # ptpython.repl.PythonRepl
            python_completer = PythonCompleter(
                # No self.get_globals yet so this must be a lambda
                # pylint: disable=unnecessary-lambda
                lambda: self.get_globals(),
                lambda: self.get_locals(),
                lambda: self.enable_dictionary_completion,  # type: ignore
            )

            all_completers = [python_completer]
            all_completers.extend(extra_completers)
            # Merge default Python completer with the new custom one.
            completer = merge_completers(all_completers)

        super().__init__(
            *args,
            create_app=False,
            # Absolute minimum height of 1
            _input_buffer_height=Dimension(min=1),
            _completer=completer,
            **ptpython_kwargs,
        )

        self.enable_mouse_support: bool = True
        self.enable_history_search: bool = True
        self.enable_dictionary_completion: bool = True
        self._set_pt_python_input_buffer_control_focusable()

        # Change some ptpython.repl defaults.
        self.show_status_bar = False
        self.show_exit_confirmation = False
        self.complete_private_attributes = (
            CompletePrivateAttributes.IF_NO_PUBLIC
        )

        # Function signature that shows args, kwargs, and types under the cursor
        # of the input window.
        self.show_signature: bool = True
        # Docstring of the current completed function that appears at the bottom
        # of the input window.
        self.show_docstring: bool = False

        # Turn off the completion menu in ptpython. The CompletionsMenu in
        # ConsoleApp.root_container will handle this.
        self.completion_visualisation: CompletionVisualisation = (
            CompletionVisualisation.NONE
        )

        # Additional state variables.
        self.repl_pane: 'Optional[ReplPane]' = None
        self._last_result = None
        self._last_exception = None

    def _set_pt_python_input_buffer_control_focusable(self) -> None:
        """Enable focus_on_click for ptpython's input buffer."""
        error_message = (
            'Unable to find ptpythons BufferControl input object.\n'
            '  For the last known position see:\n'
            '  https://github.com/prompt-toolkit/ptpython/'
            'blob/6072174eace5b645b0cfd5b21b4c237e2539f577/'
            'ptpython/layout.py#L598\n'
            '\n'
            'The installed version of ptpython may not be compatible with'
            ' pw console; please try re-running environment setup.'
        )

        try:
            # Fetch the Window's BufferControl object.
            # From ptpython/layout.py:
            #   self.root_container = HSplit([
            #     VSplit([
            #       HSplit([
            #         FloatContainer(
            #           content=HSplit(
            #             [create_python_input_window()] + extra_body
            #           ), ...
            ptpython_buffer_control = (
                self.ptpython_layout.root_container.children[0]  # type: ignore
                .children[0]
                .children[0]
                .content.children[0]
                .content
            )
            # This should be a BufferControl instance
            if not isinstance(ptpython_buffer_control, BufferControl):
                raise MissingPtpythonBufferControl(error_message)
            # Enable focus options
            ptpython_buffer_control.focusable = to_filter(True)
            ptpython_buffer_control.focus_on_click = to_filter(True)
        except IndexError as _error:
            raise MissingPtpythonBufferControl(error_message)

    def __pt_container__(self):
        """Return the prompt_toolkit root container for class.

        This allows self to be used wherever prompt_toolkit expects a container
        object."""
        return self.ptpython_layout.root_container

    def set_repl_pane(self, repl_pane):
        """Update the parent pw_console.ReplPane reference."""
        self.repl_pane = repl_pane

    def _save_result(self, formatted_text):
        """Save the last repl execution result."""
        unformatted_result = remove_formatting(formatted_text)
        self._last_result = unformatted_result

    def _save_exception(self, formatted_text):
        """Save the last repl exception."""
        unformatted_result = remove_formatting(formatted_text)
        self._last_exception = unformatted_result

    def clear_last_result(self):
        """Erase the last repl execution result."""
        self._last_result = None
        self._last_exception = None

    def show_result(self, result):
        """Format and save output results.

        This function is called from the _run_user_code() function which is
        always run from the user code thread, within
        .run_and_show_expression_async().
        """
        formatted_result = self._format_result_output(result)
        self._save_result(formatted_result)

    def _handle_exception(self, e: BaseException) -> None:
        """Format and save output results.

        This function is called from the _run_user_code() function which is
        always run from the user code thread, within
        .run_and_show_expression_async().
        """
        formatted_result = self._format_exception_output(e)
        self._save_exception(formatted_result.__pt_formatted_text__())

    def user_code_complete_callback(self, input_text, future):
        """Callback to run after user repl code is finished."""
        # If there was an exception it will be saved in self._last_result
        result_text = self._last_result
        result_object = None
        exception_text = self._last_exception

        # _last_results consumed, erase for the next run.
        self.clear_last_result()

        stdout_contents = None
        stderr_contents = None
        if future.result():
            future_result = future.result()
            stdout_contents = future_result['stdout']
            stderr_contents = future_result['stderr']
            result_object = future_result['result']

            if result_object is not None:
                # Use ptpython formatted results:
                formatted_result = self._format_result_output(result_object)
                result_text = remove_formatting(formatted_result)

        # Job is finished, append the last result.
        self.repl_pane.append_result_to_executed_code(
            input_text,
            future,
            result_text,
            stdout_contents,
            stderr_contents,
            exception_text=exception_text,
            result_object=result_object,
        )

        # Rebuild output buffer.
        self.repl_pane.update_output_buffer(
            'pw_ptpython_repl.user_code_complete_callback'
        )

        # Trigger a prompt_toolkit application redraw.
        self.repl_pane.application.application.invalidate()

    async def _run_system_command(
        self, text, stdout_proxy, _stdin_proxy
    ) -> int:
        """Run a shell command and print results to the repl."""
        command = shlex.split(text)
        returncode = None
        env = os.environ.copy()
        # Force colors in Pigweed subcommands and some terminal apps.
        env['PW_USE_COLOR'] = '1'
        env['CLICOLOR_FORCE'] = '1'

        def _handle_output(output):
            # Force tab characters to 8 spaces to prevent \t from showing in
            # prompt_toolkit.
            output = output.replace('\t', '        ')
            # Strip some ANSI sequences that don't render.
            output = output.replace('\x1b(B\x1b[m', '')
            output = output.replace('\x1b[1m', '')
            stdout_proxy.write(output)
            _SYSTEM_COMMAND_LOG.info(output.rstrip())

        with subprocess.Popen(
            command,
            env=env,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            errors='replace',
        ) as proc:
            # Print the command
            _SYSTEM_COMMAND_LOG.info('')
            _SYSTEM_COMMAND_LOG.info('$ %s', text)
            while returncode is None:
                if not proc.stdout:
                    continue

                # Check for one line and update.
                output = proc.stdout.readline()
                _handle_output(output)

                returncode = proc.poll()

            # Print any remaining lines.
            for output in proc.stdout.readlines():
                _handle_output(output)

        return returncode

    async def _run_user_code(self, text, stdout_proxy, stdin_proxy):
        """Run user code and capture stdout+err.

        This fuction should be run in a separate thread from the main
        prompt_toolkit application."""
        # NOTE: This function runs in a separate thread using the asyncio event
        # loop defined by self.repl_pane.application.user_code_loop. Patching
        # stdout here will not effect the stdout used by prompt_toolkit and the
        # main user interface.

        # Patch stdout and stderr to capture repl print() statements.
        original_stdout = sys.stdout
        original_stderr = sys.stderr

        sys.stdout = stdout_proxy
        sys.stderr = stdin_proxy

        # Run user repl code
        try:
            if _user_input_is_a_shell_command(text):
                result = await self._run_system_command(
                    text[1:], stdout_proxy, stdin_proxy
                )
            else:
                result = await self.run_and_show_expression_async(text)
        finally:
            # Always restore original stdout and stderr
            sys.stdout = original_stdout
            sys.stderr = original_stderr

        # Save the captured output
        stdout_contents = stdout_proxy.getvalue()
        stderr_contents = stdin_proxy.getvalue()

        return {
            'stdout': stdout_contents,
            'stderr': stderr_contents,
            'result': result,
        }

    def _accept_handler(self, buff: Buffer) -> bool:
        """Function executed when pressing enter in the ptpython.repl.PythonRepl
        input buffer."""
        # Do nothing if no text is entered.
        if len(buff.text) == 0:
            return False
        if self.repl_pane is None:
            return False

        repl_input_text = buff.text
        # Exit if quit or exit
        if repl_input_text.strip() in ['quit', 'quit()', 'exit', 'exit()']:
            self.repl_pane.application.application.exit()  # type: ignore

        # Create stdout and stderr proxies
        temp_stdout = io.StringIO()
        temp_stderr = io.StringIO()

        # The help() command with no args uses it's own interactive prompt which
        # will not work if prompt_toolkit is running.
        if repl_input_text.strip() in ['help()']:
            # Run nothing
            repl_input_text = ''
            # Override stdout
            temp_stdout.write(
                'Error: Interactive help() is not compatible with this repl.'
            )

        # Pop open the system command log pane for shell commands.
        if _user_input_is_a_shell_command(repl_input_text):
            self.repl_pane.application.setup_command_runner_log_pane()

        # Execute the repl code in the the separate user_code thread loop.
        future = asyncio.run_coroutine_threadsafe(
            # This function will be executed in a separate thread.
            self._run_user_code(repl_input_text, temp_stdout, temp_stderr),
            # Using this asyncio event loop.
            self.repl_pane.application.user_code_loop,
        )  # type: ignore

        # Save the input text and future object.
        self.repl_pane.append_executed_code(
            repl_input_text, future, temp_stdout, temp_stderr
        )  # type: ignore

        # Run user_code_complete_callback() when done.
        done_callback = functools.partial(
            self.user_code_complete_callback, repl_input_text
        )
        future.add_done_callback(done_callback)

        # Rebuild the parent ReplPane output buffer.
        self.repl_pane.update_output_buffer('pw_ptpython_repl._accept_handler')

        # TODO(tonymd): Return True if exception is found?
        # Don't keep input for now. Return True to keep input text.
        return False

    def line_break_count(self) -> int:
        return self.default_buffer.text.count('\n')

    def input_empty_if_in_focus_condition(self) -> Condition:
        @Condition
        def test() -> bool:
            if has_focus(self)() and len(self.default_buffer.text) == 0:
                return True
            return not has_focus(self)()

        return test
