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

import logging
import unittest

from prompt_toolkit.application import create_app_session
from prompt_toolkit.output import ColorDepth

# inclusive-language: ignore
from prompt_toolkit.output import DummyOutput as FakeOutput

from pw_console.console_app import ConsoleApp
from pw_console.console_prefs import ConsolePrefs


class TestConsoleApp(unittest.TestCase):
    """Tests for ConsoleApp."""

    def test_instantiate(self) -> None:
        """Test init."""
        with create_app_session(output=FakeOutput()):
            prefs = ConsolePrefs(
                project_file=False, project_user_file=False, user_file=False
            )
            prefs.set_code_theme('default')
            console_app = ConsoleApp(
                color_depth=ColorDepth.DEPTH_8_BIT, prefs=prefs
            )

            self.assertIsNotNone(console_app)

    def test_multiple_loggers_in_one_pane(self) -> None:
        """Test window resizing."""
        # pylint: disable=protected-access
        with create_app_session(output=FakeOutput()):
            prefs = ConsolePrefs(
                project_file=False, project_user_file=False, user_file=False
            )
            prefs.set_code_theme('default')
            console_app = ConsoleApp(
                color_depth=ColorDepth.DEPTH_8_BIT, prefs=prefs
            )

            loggers = {
                'Logs': [
                    logging.getLogger('test_log1'),
                    logging.getLogger('test_log2'),
                    logging.getLogger('test_log3'),
                ]
            }
            for window_title, logger_instances in loggers.items():
                console_app.add_log_handler(window_title, logger_instances)

            # Two panes, one for the loggers and one for the repl.
            window_list = console_app.window_manager.first_window_list()
            self.assertEqual(len(window_list.active_panes), 2)

            self.assertEqual(window_list.active_panes[0].pane_title(), 'Logs')
            self.assertEqual(
                window_list.active_panes[0]._pane_subtitle,
                'test_log1, test_log2, test_log3',
            )
            self.assertEqual(
                window_list.active_panes[0].pane_subtitle(),
                'test_log1 + 3 more',
            )


if __name__ == '__main__':
    unittest.main()
