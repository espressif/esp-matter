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
"""Tests for pw_console.log_view"""

import logging
import re
import unittest
from parameterized import parameterized  # type: ignore

from prompt_toolkit.document import Document
from prompt_toolkit.validation import ValidationError

from pw_console.log_line import LogLine
from pw_console.log_filter import (
    LogFilter,
    RegexValidator,
    SearchMatcher,
    preprocess_search_regex,
)


class TestLogFilter(unittest.TestCase):
    """Tests for LogFilter."""

    def setUp(self):
        self.maxDiff = None  # pylint: disable=invalid-name

    # pylint: disable=anomalous-backslash-in-string
    @parameterized.expand(
        [
            (
                'raw string',
                SearchMatcher.STRING,
                'f(x)',
                'f\(x\)',
                re.IGNORECASE,
            ),
            (
                'simple regex',
                SearchMatcher.REGEX,
                'f(x)',
                'f(x)',
                re.IGNORECASE,
            ),
            (
                'regex with case sensitivity',
                SearchMatcher.REGEX,
                'f(X)',
                'f(X)',
                re.RegexFlag(0),
            ),
            (
                'regex with error',
                SearchMatcher.REGEX,
                'f of (x',  # Un-terminated open paren
                'f of (x',
                re.IGNORECASE,
                True,  # fails_validation
            ),
            (
                'simple fuzzy',
                SearchMatcher.FUZZY,
                'f x y',
                '(f)(.*?)(x)(.*?)(y)',
                re.IGNORECASE,
            ),
            (
                'fuzzy with case sensitivity',
                SearchMatcher.FUZZY,
                'f X y',
                '(f)(.*?)(X)(.*?)(y)',
                re.RegexFlag(0),
            ),
        ]
    )
    def test_preprocess_search_regex(
        self,
        _name,
        input_matcher,
        input_text,
        expected_regex,
        expected_re_flag,
        should_fail_validation=False,
    ) -> None:
        """Test preprocess_search_regex returns the expected regex settings."""
        result_text, re_flag = preprocess_search_regex(
            input_text, input_matcher
        )
        self.assertEqual(expected_regex, result_text)
        self.assertEqual(expected_re_flag, re_flag)

        if should_fail_validation:
            document = Document(text=input_text)
            with self.assertRaisesRegex(
                ValidationError, r'Regex Error.*at position [0-9]+'
            ):
                RegexValidator().validate(document)

    def _create_logs(self, log_messages):
        test_log = logging.getLogger('log_filter.test')
        with self.assertLogs(test_log, level='DEBUG') as log_context:
            for log, extra_arg in log_messages:
                test_log.debug('%s', log, extra=extra_arg)

        return log_context

    @parameterized.expand(
        [
            (
                'simple fuzzy',
                SearchMatcher.FUZZY,
                'log item',
                [
                    ('Log some item', {'planet': 'Jupiter'}),
                    ('Log another item', {'planet': 'Earth'}),
                    ('Some exception', {'planet': 'Earth'}),
                ],
                [
                    'Log some item',
                    'Log another item',
                ],
                None,  # field
                False,  # invert
            ),
            (
                'simple fuzzy inverted',
                SearchMatcher.FUZZY,
                'log item',
                [
                    ('Log some item', dict()),
                    ('Log another item', dict()),
                    ('Some exception', dict()),
                ],
                [
                    'Some exception',
                ],
                None,  # field
                True,  # invert
            ),
            (
                'regex with field',
                SearchMatcher.REGEX,
                'earth',
                [
                    (
                        'Log some item',
                        dict(extra_metadata_fields={'planet': 'Jupiter'}),
                    ),
                    (
                        'Log another item',
                        dict(extra_metadata_fields={'planet': 'Earth'}),
                    ),
                    (
                        'Some exception',
                        dict(extra_metadata_fields={'planet': 'Earth'}),
                    ),
                ],
                [
                    'Log another item',
                    'Some exception',
                ],
                'planet',  # field
                False,  # invert
            ),
            (
                'regex with field inverted',
                SearchMatcher.REGEX,
                'earth',
                [
                    (
                        'Log some item',
                        dict(extra_metadata_fields={'planet': 'Jupiter'}),
                    ),
                    (
                        'Log another item',
                        dict(extra_metadata_fields={'planet': 'Earth'}),
                    ),
                    (
                        'Some exception',
                        dict(extra_metadata_fields={'planet': 'Earth'}),
                    ),
                ],
                [
                    'Log some item',
                ],
                'planet',  # field
                True,  # invert
            ),
        ]
    )
    def test_log_filter_matches(
        self,
        _name,
        input_matcher,
        input_text,
        input_lines,
        expected_matched_lines,
        field=None,
        invert=False,
    ) -> None:
        """Test log filter matches expected lines."""
        result_text, re_flag = preprocess_search_regex(
            input_text, input_matcher
        )
        log_filter = LogFilter(
            regex=re.compile(result_text, re_flag),
            input_text=input_text,
            invert=invert,
            field=field,
        )

        matched_lines = []
        logs = self._create_logs(input_lines)

        for record in logs.records:
            if log_filter.matches(
                LogLine(record, record.message, record.message)
            ):
                matched_lines.append(record.message)

        self.assertEqual(expected_matched_lines, matched_lines)


if __name__ == '__main__':
    unittest.main()
