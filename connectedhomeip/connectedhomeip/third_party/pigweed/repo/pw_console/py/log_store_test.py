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
"""Tests for pw_console.log_store"""

import logging
import unittest
from unittest.mock import MagicMock

from pw_console.log_store import LogStore
from pw_console.console_prefs import ConsolePrefs


def _create_log_store():
    log_store = LogStore(
        prefs=ConsolePrefs(
            project_file=False, project_user_file=False, user_file=False
        )
    )

    assert not log_store.table.prefs.show_python_file
    viewer = MagicMock()
    viewer.new_logs_arrived = MagicMock()
    log_store.register_viewer(viewer)
    return log_store, viewer


class TestLogStore(unittest.TestCase):
    """Tests for LogStore."""

    def setUp(self):
        self.maxDiff = None  # pylint: disable=invalid-name

    def test_get_total_count(self) -> None:
        log_store, viewer = _create_log_store()
        test_log = logging.getLogger('log_store.test')
        # Must use the assertLogs context manager and the addHandler call.
        with self.assertLogs(test_log, level='DEBUG') as log_context:
            test_log.addHandler(log_store)
            for i in range(5):
                test_log.debug('Test log %s', i)

        # Expected log message content
        self.assertEqual(
            ['DEBUG:log_store.test:Test log {}'.format(i) for i in range(5)],
            log_context.output,
        )
        # LogStore state checks
        viewer.new_logs_arrived.assert_called()
        self.assertEqual(5, log_store.get_total_count())
        self.assertEqual(4, log_store.get_last_log_index())

        log_store.clear_logs()
        self.assertEqual(0, log_store.get_total_count())

    def test_channel_counts_and_prefix_width(self) -> None:
        """Test logger names and prefix width calculations."""
        log_store, _viewer = _create_log_store()

        # Log some messagse on 3 separate logger instances
        for i, logger_name in enumerate(
            [
                'log_store.test',
                'log_store.dev',
                'log_store.production',
            ]
        ):
            test_log = logging.getLogger(logger_name)
            with self.assertLogs(test_log, level='DEBUG') as _log_context:
                test_log.addHandler(log_store)
                test_log.debug('Test log message')
                for j in range(i):
                    test_log.debug('%s', j)

        self.assertEqual(
            {
                'log_store.test': 1,
                'log_store.dev': 2,
                'log_store.production': 3,
            },
            log_store.channel_counts,
        )
        self.assertEqual(
            'log_store.test: 1, log_store.dev: 2, log_store.production: 3',
            log_store.get_channel_counts(),
        )

        self.assertRegex(
            log_store.logs[0].ansi_stripped_log,
            r'[0-9]{8} [0-9]{2}:[0-9]{2}:[0-9]{2} DEBUG Test log message',
        )
        self.assertEqual(
            {
                'log_store.test': 0,
                'log_store.dev': 0,
                'log_store.production': 0,
            },
            log_store.channel_formatted_prefix_widths,
        )

    def test_render_table_header_with_metadata(self) -> None:
        log_store, _viewer = _create_log_store()
        test_log = logging.getLogger('log_store.test')

        # Log table with extra columns
        with self.assertLogs(test_log, level='DEBUG') as _log_context:
            test_log.addHandler(log_store)
            test_log.debug(
                'Test log %s',
                extra=dict(
                    extra_metadata_fields={
                        'planet': 'Jupiter',
                        'galaxy': 'Milky Way',
                    }
                ),
            )

        self.assertEqual(
            [
                ('bold', 'Time             '),
                ('', '  '),
                ('bold', 'Level'),
                ('', '  '),
                ('bold', 'Planet '),
                ('', '  '),
                ('bold', 'Galaxy   '),
                ('', '  '),
                ('bold', 'Message'),
            ],
            log_store.render_table_header(),
        )


if __name__ == '__main__':
    unittest.main()
