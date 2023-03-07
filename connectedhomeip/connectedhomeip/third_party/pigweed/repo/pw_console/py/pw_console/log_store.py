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
"""LogStore saves logs and acts as a Python logging handler."""

from __future__ import annotations
import collections
import logging
from datetime import datetime
from typing import Dict, List, Optional, TYPE_CHECKING

from pw_cli.color import colors as pw_cli_colors

from pw_console.console_prefs import ConsolePrefs
from pw_console.log_line import LogLine
from pw_console.text_formatting import strip_ansi
from pw_console.widgets.table import TableView

if TYPE_CHECKING:
    from pw_console.log_view import LogView


class LogStore(logging.Handler):
    """Pigweed Console logging handler.

    This is a `Python logging.Handler
    <https://docs.python.org/3/library/logging.html#handler-objects>`_ class
    used to store logs for display in the pw_console user interface.

    You may optionally add this as a handler to an existing logger
    instances. This will be required if logs need to be captured for display in
    the pw_console UI before the user interface is running.

    Example usage:

    .. code-block:: python

        import logging

        from pw_console import PwConsoleEmbed, LogStore

        _DEVICE_LOG = logging.getLogger('usb_gadget')

        # Create a log store and add as a handler.
        device_log_store = LogStore()
        _DEVICE_LOG.addHander(device_log_store)

        # Start communication with your device here, before embedding
        # pw_console.

        # Create the pw_console embed instance
        console = PwConsoleEmbed(
            global_vars=globals(),
            local_vars=locals(),
            loggers={
                'Host Logs': [
                    logging.getLogger(__package__),
                    logging.getLogger(__name__),
                ],
                # Set the LogStore as the value of this logger window.
                'Device Logs': device_log_store,
            },
            app_title='My Awesome Console',
        )

        console.setup_python_logging()
        console.embed()
    """

    def __init__(self, prefs: Optional[ConsolePrefs] = None):
        """Initializes the LogStore instance."""

        # ConsolePrefs may not be passed on init. For example, if the user is
        # creating a LogStore to capture log messages before console startup.
        if not prefs:
            prefs = ConsolePrefs(
                project_file=False, project_user_file=False, user_file=False
            )
        self.prefs = prefs
        # Log storage deque for fast addition and deletion from the beginning
        # and end of the iterable.
        self.logs: collections.deque = collections.deque()

        # Only allow this many log lines in memory.
        self.max_history_size: int = 1000000

        # Counts of logs per python logger name
        self.channel_counts: Dict[str, int] = {}
        # Widths of each logger prefix string. For example: the character length
        # of the timestamp string.
        self.channel_formatted_prefix_widths: Dict[str, int] = {}
        # Longest of the above prefix widths.
        self.longest_channel_prefix_width = 0

        self.table: TableView = TableView(prefs=self.prefs)

        # Erase existing logs.
        self.clear_logs()

        # List of viewers that should be notified on new log line arrival.
        self.registered_viewers: List['LogView'] = []

        super().__init__()

        # Set formatting after logging.Handler init.
        self.set_formatting()

    def set_prefs(self, prefs: ConsolePrefs) -> None:
        """Set the ConsolePrefs for this LogStore."""
        self.prefs = prefs
        self.table.set_prefs(prefs)

    def register_viewer(self, viewer: 'LogView') -> None:
        """Register this LogStore with a LogView."""
        self.registered_viewers.append(viewer)

    def set_formatting(self) -> None:
        """Setup log formatting."""
        # Copy of pw_cli log formatter
        colors = pw_cli_colors(True)
        timestamp_prefix = colors.black_on_white('%(asctime)s') + ' '
        timestamp_format = '%Y%m%d %H:%M:%S'
        format_string = timestamp_prefix + '%(levelname)s %(message)s'
        formatter = logging.Formatter(format_string, timestamp_format)

        self.setLevel(logging.DEBUG)
        self.setFormatter(formatter)

        # Update log time character width.
        example_time_string = datetime.now().strftime(timestamp_format)
        self.table.column_widths['time'] = len(example_time_string)

    def clear_logs(self):
        """Erase all stored pane lines."""
        self.logs = collections.deque()
        self.channel_counts = {}
        self.channel_formatted_prefix_widths = {}
        self.line_index = 0

    def get_channel_counts(self):
        """Return the seen channel log counts for this conatiner."""
        return ', '.join(
            [f'{name}: {count}' for name, count in self.channel_counts.items()]
        )

    def get_total_count(self):
        """Total size of the logs store."""
        return len(self.logs)

    def get_last_log_index(self):
        """Last valid index of the logs."""
        # Subtract 1 since self.logs is zero indexed.
        total = self.get_total_count()
        return 0 if total < 0 else total - 1

    def _update_log_prefix_width(self, record: logging.LogRecord):
        """Save the formatted prefix width if this is a new logger channel
        name."""
        if self.formatter and (
            record.name not in self.channel_formatted_prefix_widths.keys()
        ):
            # Find the width of the formatted timestamp and level
            format_string = (
                self.formatter._fmt  # pylint: disable=protected-access
            )

            # There may not be a _fmt defined.
            if not format_string:
                return

            format_without_message = format_string.replace('%(message)s', '')
            # If any other style parameters are left, get the width of them.
            if (
                format_without_message
                and 'asctime' in format_without_message
                and 'levelname' in format_without_message
            ):
                formatted_time_and_level = format_without_message % dict(
                    asctime=record.asctime, levelname=record.levelname
                )

                # Delete ANSI escape sequences.
                ansi_stripped_time_and_level = strip_ansi(
                    formatted_time_and_level
                )

                self.channel_formatted_prefix_widths[record.name] = len(
                    ansi_stripped_time_and_level
                )
            else:
                self.channel_formatted_prefix_widths[record.name] = 0

            # Set the max width of all known formats so far.
            self.longest_channel_prefix_width = max(
                self.channel_formatted_prefix_widths.values()
            )

    def _append_log(self, record: logging.LogRecord):
        """Add a new log event."""
        # Format incoming log line.
        formatted_log = self.format(record)
        ansi_stripped_log = strip_ansi(formatted_log)
        # Save this log.
        self.logs.append(
            LogLine(
                record=record,
                formatted_log=formatted_log,
                ansi_stripped_log=ansi_stripped_log,
            )
        )
        # Increment this logger count
        self.channel_counts[record.name] = (
            self.channel_counts.get(record.name, 0) + 1
        )

        # TODO(b/235271486): Revisit calculating prefix widths automatically
        # when line wrapping indentation is supported.
        # Set the prefix width to 0
        self.channel_formatted_prefix_widths[record.name] = 0

        # Parse metadata fields
        self.logs[-1].update_metadata()

        # Check for bigger column widths.
        self.table.update_metadata_column_widths(self.logs[-1])

    def emit(self, record) -> None:
        """Process a new log record.

        This defines the logging.Handler emit() fuction which is called by
        logging.Handler.handle() We don't implement handle() as it is done in
        the parent class with thread safety and filters applied.
        """
        self._append_log(record)
        # Notify viewers of new logs
        for viewer in self.registered_viewers:
            viewer.new_logs_arrived()

    def render_table_header(self):
        """Get pre-formatted table header."""
        return self.table.formatted_header()
