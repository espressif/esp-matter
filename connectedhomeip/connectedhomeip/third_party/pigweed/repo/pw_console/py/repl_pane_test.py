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
"""Tests for pw_console.console_app"""

import asyncio
import builtins
import inspect
import io
import sys
import threading
import unittest
from unittest.mock import MagicMock, call

from prompt_toolkit.application import create_app_session
from prompt_toolkit.output import (
    ColorDepth,
    # inclusive-language: ignore
    DummyOutput as FakeOutput,
)

from pw_console.console_app import ConsoleApp
from pw_console.console_prefs import ConsolePrefs
from pw_console.repl_pane import ReplPane
from pw_console.pw_ptpython_repl import PwPtPythonRepl

_PYTHON_3_8 = sys.version_info >= (
    3,
    8,
)

if _PYTHON_3_8:
    from unittest import IsolatedAsyncioTestCase  # type: ignore # pylint: disable=no-name-in-module

    class TestReplPane(IsolatedAsyncioTestCase):
        """Tests for ReplPane."""

        def setUp(self):  # pylint: disable=invalid-name
            self.maxDiff = None  # pylint: disable=invalid-name

        def test_repl_code_return_values(self) -> None:
            """Test stdout, return values, and exceptions can be returned from
            running user repl code."""
            app = MagicMock()

            global_vars = {
                '__name__': '__main__',
                '__package__': None,
                '__doc__': None,
                '__builtins__': builtins,
            }

            pw_ptpython_repl = PwPtPythonRepl(
                get_globals=lambda: global_vars,
                get_locals=lambda: global_vars,
                color_depth=ColorDepth.DEPTH_8_BIT,
            )
            repl_pane = ReplPane(
                application=app,
                python_repl=pw_ptpython_repl,
            )
            # Check pw_ptpython_repl has a reference to the parent repl_pane.
            self.assertEqual(repl_pane, pw_ptpython_repl.repl_pane)

            # Define a function, should return nothing.
            code = inspect.cleandoc(
                """
                def run():
                    print('The answer is ', end='')
                    return 1+1+4+16+20
            """
            )
            temp_stdout = io.StringIO()
            temp_stderr = io.StringIO()
            # pylint: disable=protected-access
            result = asyncio.run(
                pw_ptpython_repl._run_user_code(code, temp_stdout, temp_stderr)
            )
            self.assertEqual(
                result, {'stdout': '', 'stderr': '', 'result': None}
            )

            temp_stdout = io.StringIO()
            temp_stderr = io.StringIO()
            # Check stdout and return value
            result = asyncio.run(
                pw_ptpython_repl._run_user_code(
                    'run()', temp_stdout, temp_stderr
                )
            )
            self.assertEqual(
                result, {'stdout': 'The answer is ', 'stderr': '', 'result': 42}
            )

            temp_stdout = io.StringIO()
            temp_stderr = io.StringIO()
            # Check for repl exception
            result = asyncio.run(
                pw_ptpython_repl._run_user_code(
                    'return "blah"', temp_stdout, temp_stderr
                )
            )
            self.assertIn(
                "SyntaxError: 'return' outside function",
                pw_ptpython_repl._last_exception,  # type: ignore
            )

        async def test_user_thread(self) -> None:
            """Test user code thread."""

            with create_app_session(output=FakeOutput()):
                # Setup Mocks
                prefs = ConsolePrefs(
                    project_file=False, project_user_file=False, user_file=False
                )
                prefs.set_code_theme('default')
                app = ConsoleApp(
                    color_depth=ColorDepth.DEPTH_8_BIT, prefs=prefs
                )

                app.start_user_code_thread()

                pw_ptpython_repl = app.pw_ptpython_repl
                repl_pane = app.repl_pane

                # Mock update_output_buffer to track number of update calls
                repl_pane.update_output_buffer = MagicMock(  # type: ignore
                    wraps=repl_pane.update_output_buffer
                )

                # Mock complete callback
                pw_ptpython_repl.user_code_complete_callback = (  # type: ignore
                    MagicMock(
                        wraps=pw_ptpython_repl.user_code_complete_callback
                    )
                )

                # Repl done flag for tests
                user_code_done = threading.Event()

                # Run some code
                code = inspect.cleandoc(
                    """
                    import time
                    def run():
                        for i in range(2):
                            time.sleep(0.5)
                            print(i)
                        print('The answer is ', end='')
                        return 1+1+4+16+20
                """
                )
                input_buffer = MagicMock(text=code)
                # pylint: disable=protected-access
                pw_ptpython_repl._accept_handler(input_buffer)
                # pylint: enable=protected-access

                # Get last executed code object.
                user_code1 = repl_pane.executed_code[-1]
                # Wait for repl code to finish.
                user_code1.future.add_done_callback(
                    lambda future: user_code_done.set()
                )
                # Wait for stdout monitoring to complete.
                if user_code1.stdout_check_task:
                    await user_code1.stdout_check_task
                # Wait for test done callback.
                user_code_done.wait()

                # Check user_code1 results
                # NOTE: Avoid using assert_has_calls. Thread timing can make the
                # test flaky.
                expected_calls = [
                    # Initial exec start
                    call('pw_ptpython_repl._accept_handler'),
                    # Code finishes
                    call('repl_pane.append_result_to_executed_code'),
                    # Complete callback
                    call('pw_ptpython_repl.user_code_complete_callback'),
                ]
                for expected_call in expected_calls:
                    self.assertIn(
                        expected_call, repl_pane.update_output_buffer.mock_calls
                    )

                user_code_complete_callback = (
                    pw_ptpython_repl.user_code_complete_callback
                )
                user_code_complete_callback.assert_called_once()

                self.assertIsNotNone(user_code1)
                self.assertTrue(user_code1.future.done())
                self.assertEqual(user_code1.input, code)
                self.assertEqual(user_code1.output, None)
                # stdout / stderr may be '' or None
                self.assertFalse(user_code1.stdout)
                self.assertFalse(user_code1.stderr)

                # Reset mocks
                user_code_done.clear()
                pw_ptpython_repl.user_code_complete_callback.reset_mock()
                repl_pane.update_output_buffer.reset_mock()

                # Run some code
                input_buffer = MagicMock(text='run()')
                # pylint: disable=protected-access
                pw_ptpython_repl._accept_handler(input_buffer)
                # pylint: enable=protected-access

                # Get last executed code object.
                user_code2 = repl_pane.executed_code[-1]
                # Wait for repl code to finish.
                user_code2.future.add_done_callback(
                    lambda future: user_code_done.set()
                )
                # Wait for stdout monitoring to complete.
                if user_code2.stdout_check_task:
                    await user_code2.stdout_check_task
                # Wait for test done callback.
                user_code_done.wait()

                # Check user_code2 results
                # NOTE: Avoid using assert_has_calls. Thread timing can make the
                # test flaky.
                expected_calls = [
                    # Initial exec start
                    call('pw_ptpython_repl._accept_handler'),
                    # Periodic checks, should be a total of 4:
                    #   Code should take 1.0 second to run.
                    #   Periodic checks every 0.3 seconds
                    #   1.0 / 0.3 = 3.33 (4) checks
                    call('repl_pane.periodic_check'),
                    call('repl_pane.periodic_check'),
                    call('repl_pane.periodic_check'),
                    # Code finishes
                    call('repl_pane.append_result_to_executed_code'),
                    # Complete callback
                    call('pw_ptpython_repl.user_code_complete_callback'),
                    # Final periodic check
                    call('repl_pane.periodic_check'),
                ]
                for expected_call in expected_calls:
                    self.assertIn(
                        expected_call, repl_pane.update_output_buffer.mock_calls
                    )

                # pylint: disable=line-too-long
                pw_ptpython_repl.user_code_complete_callback.assert_called_once()
                # pylint: enable=line-too-long
                self.assertIsNotNone(user_code2)
                self.assertTrue(user_code2.future.done())
                self.assertEqual(user_code2.input, 'run()')
                self.assertEqual(user_code2.output, '42')
                self.assertEqual(user_code2.stdout, '0\n1\nThe answer is ')
                self.assertFalse(user_code2.stderr)

                # Reset mocks
                user_code_done.clear()
                pw_ptpython_repl.user_code_complete_callback.reset_mock()
                repl_pane.update_output_buffer.reset_mock()


if __name__ == '__main__':
    unittest.main()
