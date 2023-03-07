#!/usr/bin/env python3
# Copyright 2020 The Pigweed Authors
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
"""Tests for general purpose tools."""

import unittest

from pw_presubmit import tools


class FlattenTest(unittest.TestCase):
    """Tests the flatten function, which flattens iterables."""

    def test_empty(self):
        self.assertEqual([], list(tools.flatten()))
        self.assertEqual([], list(tools.flatten([])))
        self.assertEqual([], list(tools.flatten([], ())))
        self.assertEqual([], list(tools.flatten([[], (), [[]]], ((), []))))

    def test_no_nesting(self):
        self.assertEqual(
            ['a', 'bcd', 123, 45.6], list(tools.flatten('a', 'bcd', 123, 45.6))
        )
        self.assertEqual(
            ['a', 'bcd', 123, 45.6],
            list(tools.flatten(['a', 'bcd', 123, 45.6])),
        )
        self.assertEqual(
            ['a', 'bcd', 123, 45.6],
            list(tools.flatten(['a', 'bcd'], [123, 45.6])),
        )

    def test_nesting(self):
        self.assertEqual(
            ['a', 'bcd', 123, 45.6],
            list(tools.flatten('a', ['bcd'], [123], 45.6)),
        )
        self.assertEqual(
            ['a', 'bcd', 123, 45.6],
            list(tools.flatten([['a', ('bcd', [123])], 45.6])),
        )
        self.assertEqual(
            ['a', 'bcd', 123, 45.6],
            list(tools.flatten([('a', 'bcd')], [[[[123]]], 45.6])),
        )


if __name__ == '__main__':
    unittest.main()
