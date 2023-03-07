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
"""LogFilters define how to search log lines in LogViews."""

from __future__ import annotations
import logging
import re
from dataclasses import dataclass
from enum import Enum
from typing import Optional

from prompt_toolkit.formatted_text import StyleAndTextTuples
from prompt_toolkit.formatted_text.utils import fragment_list_to_text
from prompt_toolkit.layout.utils import explode_text_fragments
from prompt_toolkit.validation import ValidationError, Validator

from pw_console.log_line import LogLine

_LOG = logging.getLogger(__package__)

_UPPERCASE_REGEX = re.compile(r'[A-Z]')


class SearchMatcher(Enum):
    """Possible search match methods."""

    FUZZY = 'FUZZY'
    REGEX = 'REGEX'
    STRING = 'STRING'


DEFAULT_SEARCH_MATCHER = SearchMatcher.REGEX


def preprocess_search_regex(
    text, matcher: SearchMatcher = DEFAULT_SEARCH_MATCHER
):
    # Ignorecase unless the text has capital letters in it.
    regex_flags = re.IGNORECASE
    if _UPPERCASE_REGEX.search(text):
        regex_flags = re.RegexFlag(0)

    if matcher == SearchMatcher.FUZZY:
        # Fuzzy match replace spaces with .*
        text_tokens = text.split(' ')
        if len(text_tokens) > 1:
            text = '(.*?)'.join(
                ['({})'.format(re.escape(text)) for text in text_tokens]
            )
    elif matcher == SearchMatcher.STRING:
        # Escape any regex specific characters to match the string literal.
        text = re.escape(text)
    elif matcher == SearchMatcher.REGEX:
        # Don't modify search text input.
        pass

    return text, regex_flags


class RegexValidator(Validator):
    """Validation of regex input."""

    def validate(self, document):
        """Check search input for regex syntax errors."""
        regex_text, regex_flags = preprocess_search_regex(document.text)
        try:
            re.compile(regex_text, regex_flags)
        except re.error as error:
            raise ValidationError(
                error.pos, "Regex Error: %s" % error
            ) from error


@dataclass
class LogFilter:
    """Log Filter Dataclass."""

    regex: re.Pattern
    input_text: Optional[str] = None
    invert: bool = False
    field: Optional[str] = None

    def pattern(self):
        return self.regex.pattern

    def matches(self, log: LogLine):
        field = log.ansi_stripped_log
        if self.field:
            if hasattr(log, 'metadata') and hasattr(log.metadata, 'fields'):
                field = log.metadata.fields.get(
                    self.field, log.ansi_stripped_log
                )
            if hasattr(log.record, 'extra_metadata_fields'):  # type: ignore
                field = log.record.extra_metadata_fields.get(  # type: ignore
                    self.field, log.ansi_stripped_log
                )
            if self.field == 'lvl':
                field = log.record.levelname
            elif self.field == 'time':
                field = log.record.asctime

        match = self.regex.search(field)

        if self.invert:
            return not match
        return match

    def highlight_search_matches(
        self, line_fragments, selected=False
    ) -> StyleAndTextTuples:
        """Highlight search matches in the current line_fragment."""
        line_text = fragment_list_to_text(line_fragments)
        exploded_fragments = explode_text_fragments(line_fragments)

        def apply_highlighting(fragments, i):
            # Expand all fragments and apply the highlighting style.
            old_style, _text, *_ = fragments[i]
            if selected:
                fragments[i] = (
                    old_style + ' class:search.current ',
                    fragments[i][1],
                )
            else:
                fragments[i] = (
                    old_style + ' class:search ',
                    fragments[i][1],
                )

        if self.invert:
            # Highlight the whole line
            for i, _fragment in enumerate(exploded_fragments):
                apply_highlighting(exploded_fragments, i)
        else:
            # Highlight each non-overlapping search match.
            for match in self.regex.finditer(line_text):
                for fragment_i in range(match.start(), match.end()):
                    apply_highlighting(exploded_fragments, fragment_i)

        return exploded_fragments
