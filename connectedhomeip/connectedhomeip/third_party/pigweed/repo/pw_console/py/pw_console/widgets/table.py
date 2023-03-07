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
"""Table view renderer for LogLines."""

import collections
import copy

from prompt_toolkit.formatted_text import StyleAndTextTuples

from pw_console.console_prefs import ConsolePrefs
from pw_console.log_line import LogLine
from pw_console.text_formatting import strip_ansi


class TableView:
    """Store column information and render logs into formatted tables."""

    # TODO(tonymd): Add a method to provide column formatters externally.
    # Should allow for string format, column color, and column ordering.
    FLOAT_FORMAT = '%.3f'
    INT_FORMAT = '%s'
    LAST_TABLE_COLUMN_NAMES = ['msg', 'message']

    def __init__(self, prefs: ConsolePrefs):
        self.set_prefs(prefs)
        self.column_widths: collections.OrderedDict = collections.OrderedDict()
        self._header_fragment_cache = None

        # Assume common defaults here before recalculating in set_formatting().
        self._default_time_width: int = 17
        self.column_widths['time'] = self._default_time_width
        self.column_widths['level'] = 3
        self._year_month_day_width: int = 9

        # Width of all columns except the final message
        self.column_width_prefix_total = 0

    def set_prefs(self, prefs: ConsolePrefs) -> None:
        self.prefs = prefs
        # Max column widths of each log field
        self.column_padding = ' ' * self.prefs.spaces_between_columns

    def all_column_names(self):
        columns_names = [name for name, _width in self._ordered_column_widths()]
        return columns_names + ['message']

    def _width_of_justified_fields(self):
        """Calculate the width of all columns except LAST_TABLE_COLUMN_NAMES."""
        padding_width = len(self.column_padding)
        used_width = sum(
            [
                width + padding_width
                for key, width in self.column_widths.items()
                if key not in TableView.LAST_TABLE_COLUMN_NAMES
            ]
        )
        return used_width

    def _ordered_column_widths(self):
        """Return each column and width in the preferred order."""
        if self.prefs.column_order:
            # Get ordered_columns
            columns = copy.copy(self.column_widths)
            ordered_columns = {}

            for column_name in self.prefs.column_order:
                # If valid column name
                if column_name in columns:
                    ordered_columns[column_name] = columns.pop(column_name)

            # Add remaining columns unless user preference to hide them.
            if not self.prefs.omit_unspecified_columns:
                for column_name in columns:
                    ordered_columns[column_name] = columns[column_name]
        else:
            ordered_columns = copy.copy(self.column_widths)

        if not self.prefs.show_python_file and 'py_file' in ordered_columns:
            del ordered_columns['py_file']
        if not self.prefs.show_python_logger and 'py_logger' in ordered_columns:
            del ordered_columns['py_logger']
        if not self.prefs.show_source_file and 'file' in ordered_columns:
            del ordered_columns['file']

        return ordered_columns.items()

    def update_metadata_column_widths(self, log: LogLine) -> None:
        """Calculate the max widths for each metadata field."""
        if log.metadata is None:
            log.update_metadata()
        # If extra fields still don't exist, no need to update column widths.
        if log.metadata is None:
            return

        for field_name, value in log.metadata.fields.items():
            value_string = str(value)

            # Get width of formatted numbers
            if isinstance(value, float):
                value_string = TableView.FLOAT_FORMAT % value
            elif isinstance(value, int):
                value_string = TableView.INT_FORMAT % value

            current_width = self.column_widths.get(field_name, 0)
            if len(value_string) > current_width:
                self.column_widths[field_name] = len(value_string)

        # Update log level character width.
        ansi_stripped_level = strip_ansi(log.record.levelname)
        if len(ansi_stripped_level) > self.column_widths['level']:
            self.column_widths['level'] = len(ansi_stripped_level)

        self.column_width_prefix_total = self._width_of_justified_fields()
        self._update_table_header()

    def _update_table_header(self):
        default_style = 'bold'
        fragments: collections.deque = collections.deque()

        # Update time column width to current prefs setting
        self.column_widths['time'] = self._default_time_width
        if self.prefs.hide_date_from_log_time:
            self.column_widths['time'] = (
                self._default_time_width - self._year_month_day_width
            )

        for name, width in self._ordered_column_widths():
            # These fields will be shown at the end
            if name in TableView.LAST_TABLE_COLUMN_NAMES:
                continue
            fragments.append((default_style, name.title()[:width].ljust(width)))
            fragments.append(('', self.column_padding))

        fragments.append((default_style, 'Message'))

        self._header_fragment_cache = list(fragments)

    def formatted_header(self):
        """Get pre-formatted table header."""
        return self._header_fragment_cache

    def formatted_row(self, log: LogLine) -> StyleAndTextTuples:
        """Render a single table row."""
        # pylint: disable=too-many-locals
        padding_formatted_text = ('', self.column_padding)
        # Don't apply any background styling that would override the parent
        # window or selected-log-line style.
        default_style = ''

        table_fragments: StyleAndTextTuples = []

        # NOTE: To preseve ANSI formatting on log level use:
        # table_fragments.extend(
        #     ANSI(log.record.levelname.ljust(
        #         self.column_widths['level'])).__pt_formatted_text__())

        # Collect remaining columns to display after host time and level.
        columns = {}
        for name, width in self._ordered_column_widths():
            # Skip these modifying these fields
            if name in TableView.LAST_TABLE_COLUMN_NAMES:
                continue

            # hasattr checks are performed here since a log record may not have
            # asctime or levelname if they are not included in the formatter
            # fmt string.
            if name == 'time' and hasattr(log.record, 'asctime'):
                time_text = log.record.asctime
                if self.prefs.hide_date_from_log_time:
                    time_text = time_text[self._year_month_day_width :]
                time_style = self.prefs.column_style(
                    'time', time_text, default='class:log-time'
                )
                columns['time'] = (
                    time_style,
                    time_text.ljust(self.column_widths['time']),
                )
                continue

            if name == 'level' and hasattr(log.record, 'levelname'):
                # Remove any existing ANSI formatting and apply our colors.
                level_text = strip_ansi(log.record.levelname)
                level_style = self.prefs.column_style(
                    'level',
                    level_text,
                    default='class:log-level-{}'.format(log.record.levelno),
                )
                columns['level'] = (
                    level_style,
                    level_text.ljust(self.column_widths['level']),
                )
                continue

            value = log.metadata.fields.get(name, ' ')
            left_justify = True

            # Right justify and format numbers
            if isinstance(value, float):
                value = TableView.FLOAT_FORMAT % value
                left_justify = False
            elif isinstance(value, int):
                value = TableView.INT_FORMAT % value
                left_justify = False

            if left_justify:
                columns[name] = value.ljust(width)
            else:
                columns[name] = value.rjust(width)

        # Grab the message to appear after the justified columns with ANSI
        # escape sequences removed.
        message_text = strip_ansi(log.record.message)
        message = log.metadata.fields.get(
            'msg',
            message_text.rstrip(),  # Remove any trailing line breaks
        )
        # Alternatively ANSI formatting can be preserved with:
        #   message = ANSI(log.record.message).__pt_formatted_text__()

        # Convert to FormattedText if we have a raw string from fields.
        if isinstance(message, str):
            message_style = default_style
            if log.record.levelno >= 30:  # Warning, Error and Critical
                # Style the whole message to match it's level
                message_style = 'class:log-level-{}'.format(log.record.levelno)
            message = (message_style, message)
        # Add to columns
        columns['message'] = message

        index_modifier = 0
        # Go through columns and convert to FormattedText where needed.
        for i, column in enumerate(columns.items()):
            column_name, column_value = column
            if i in [0, 1] and column_name in ['time', 'level']:
                index_modifier -= 1
            # For raw strings that don't have their own ANSI colors, apply the
            # theme color style for this column.
            if isinstance(column_value, str):
                fallback_style = (
                    'class:log-table-column-{}'.format(i + index_modifier)
                    if 0 <= i <= 7
                    else default_style
                )

                style = self.prefs.column_style(
                    column_name, column_value.rstrip(), default=fallback_style
                )

                table_fragments.append((style, column_value))
                table_fragments.append(padding_formatted_text)
            # Add this tuple to table_fragments.
            elif isinstance(column, tuple):
                table_fragments.append(column_value)
                # Add padding if not the last column.
                if i < len(columns) - 1:
                    table_fragments.append(padding_formatted_text)

        # Add the final new line for this row.
        table_fragments.append(('', '\n'))
        return table_fragments
