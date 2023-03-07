#!/usr/bin/env python3
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
"""Tests decoding metadata from log strings."""

import unittest

from pw_log_tokenized import FormatStringWithMetadata


class TestFormatStringWithMetadata(unittest.TestCase):
    """Tests extracting metadata from a pw_log_tokenized-style format string."""

    def test_all_fields(self):
        log = FormatStringWithMetadata(
            '■msg♦hello %d■file♦__FILE__■module♦log module name!'
        )
        self.assertEqual(log.message, 'hello %d')
        self.assertEqual(log.module, 'log module name!')
        self.assertEqual(log.file, '__FILE__')

    def test_different_fields(self):
        log = FormatStringWithMetadata('■msg♦hello %d■module♦■THING♦abc123')
        self.assertEqual(log.message, 'hello %d')
        self.assertEqual(log.module, '')
        self.assertEqual(log.file, '')
        self.assertEqual(log.fields['THING'], 'abc123')

    def test_no_metadata(self):
        log = FormatStringWithMetadata('a■msg♦not formatted correctly')
        self.assertEqual(log.message, log.raw_string)
        self.assertEqual(log.module, '')
        self.assertEqual(log.file, '')

    def test_invalid_field_name(self):
        log = FormatStringWithMetadata('■msg♦M♦S♦G■1abc♦abc■other♦hi')
        self.assertEqual(log.message, 'M♦S♦G■1abc♦abc')
        self.assertEqual(log.fields['other'], 'hi')

    def test_delimiters_in_value(self):
        log = FormatStringWithMetadata('■msg♦♦■♦■yo■module♦M♦DU■E')
        self.assertEqual(log.message, '♦■♦■yo')
        self.assertEqual(log.module, 'M♦DU■E')


if __name__ == '__main__':
    unittest.main()
