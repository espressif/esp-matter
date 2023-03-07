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
"""LogLine storage class."""

import logging
from dataclasses import dataclass
from datetime import datetime
from typing import Dict, Optional

from prompt_toolkit.formatted_text import ANSI, StyleAndTextTuples

from pw_log_tokenized import FormatStringWithMetadata


@dataclass
class LogLine:
    """Class to hold a single log event."""

    record: logging.LogRecord
    formatted_log: str
    ansi_stripped_log: str

    def __post_init__(self):
        self.metadata = None
        self.fragment_cache = None

    def time(self):
        """Return a datetime object for the log record."""
        return datetime.fromtimestamp(self.record.created)

    def update_metadata(self, extra_fields: Optional[Dict] = None):
        """Parse log metadata fields from various sources."""

        # 1. Parse any metadata from the message itself.
        self.metadata = FormatStringWithMetadata(str(self.record.message))
        self.formatted_log = self.formatted_log.replace(
            self.metadata.raw_string, self.metadata.message
        )
        # Remove any trailing line breaks.
        self.formatted_log = self.formatted_log.rstrip()

        # 2. Check for a metadata Dict[str, str] stored in the log record in the
        # `extra_metadata_fields` attribute. This should be set using the
        # extra={} kwarg. For example:
        # LOGGER.log(
        #     level,
        #     '%s',
        #     message,
        #     extra=dict(
        #         extra_metadata_fields={
        #             'Field1': 'Value1',
        #             'Field2': 'Value2',
        #         }))
        # See:
        # https://docs.python.org/3/library/logging.html#logging.debug
        if hasattr(self.record, 'extra_metadata_fields') and (
            self.record.extra_metadata_fields  # type: ignore
        ):
            fields = self.record.extra_metadata_fields  # type: ignore
            for key, value in fields.items():
                self.metadata.fields[key] = value

        # 3. Check for additional passed in metadata.
        if extra_fields:
            for key, value in extra_fields.items():
                self.metadata.fields[key] = value

        lineno = self.record.lineno
        file_name = str(self.record.filename)
        self.metadata.fields['py_file'] = f'{file_name}:{lineno}'
        self.metadata.fields['py_logger'] = str(self.record.name)

        return self.metadata

    def get_fragments(self) -> StyleAndTextTuples:
        """Return this log line as a list of FormattedText tuples."""
        # Parse metadata if any.
        if self.metadata is None:
            self.update_metadata()

        # Create prompt_toolkit FormattedText tuples based on the log ANSI
        # escape sequences.
        if self.fragment_cache is None:
            self.fragment_cache = ANSI(
                self.formatted_log + '\n'  # Add a trailing linebreak
            ).__pt_formatted_text__()

        return self.fragment_cache
