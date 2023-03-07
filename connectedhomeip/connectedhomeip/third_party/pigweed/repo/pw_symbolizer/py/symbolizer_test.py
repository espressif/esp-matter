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
"""Tests for pw_symbolizer's python tooling."""

import unittest
import pw_symbolizer


class TestSymbolFormatting(unittest.TestCase):
    """Tests Symbol objects to validate formatted output."""

    def test_blank_symbol(self):
        sym = pw_symbolizer.Symbol(address=0x00000000, name='', file='', line=0)
        self.assertEqual('??:?', sym.file_and_line())
        self.assertEqual('0x00000000 (??:?)', str(sym))

    def test_default_symbol(self):
        sym = pw_symbolizer.Symbol(address=0x0000A400)
        self.assertEqual('??:?', sym.file_and_line())
        self.assertEqual('0x0000A400 (??:?)', str(sym))

    def test_to_str(self):
        sym = pw_symbolizer.Symbol(
            address=0x12345678,
            name='idle_thread_context',
            file='device/system/threads.cc',
            line=59,
        )
        self.assertEqual('device/system/threads.cc:59', sym.file_and_line())
        self.assertEqual(
            'idle_thread_context (device/system/threads.cc:59)', str(sym)
        )

    def test_truncated_filename(self):
        sym = pw_symbolizer.Symbol(
            address=0x12345678,
            name='idle_thread_context',
            file='device/system/threads.cc',
            line=59,
        )
        self.assertEqual(
            'idle_thread_context ([...]stem/threads.cc:59)',
            sym.to_string(max_filename_len=15),
        )


class TestFakeSymbolizer(unittest.TestCase):
    """Tests the FakeSymbolizer class."""

    def test_empty_db(self):
        symbolizer = pw_symbolizer.FakeSymbolizer()
        symbol = symbolizer.symbolize(0x404)
        self.assertEqual(symbol.address, 0x404)
        self.assertEqual(symbol.name, '')
        self.assertEqual(symbol.file, '')

    def test_db_with_entries(self):
        known_symbols = (
            pw_symbolizer.Symbol(
                0x404, 'do_a_flip(int n)', 'source/tricks.cc', 1403
            ),
            pw_symbolizer.Symbol(
                0xFFFFFFFF,
                'a_variable_here_would_be_funny',
                'source/globals.cc',
                21,
            ),
        )
        symbolizer = pw_symbolizer.FakeSymbolizer(known_symbols)

        symbol = symbolizer.symbolize(0x404)
        self.assertEqual(symbol.address, 0x404)
        self.assertEqual(symbol.name, 'do_a_flip(int n)')
        self.assertEqual(symbol.file, 'source/tricks.cc')
        self.assertEqual(symbol.line, 1403)

        symbol = symbolizer.symbolize(0xFFFFFFFF)
        self.assertEqual(symbol.address, 0xFFFFFFFF)
        self.assertEqual(symbol.name, 'a_variable_here_would_be_funny')
        self.assertEqual(symbol.file, 'source/globals.cc')
        self.assertEqual(symbol.line, 21)


if __name__ == '__main__':
    unittest.main()
