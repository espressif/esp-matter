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
"""Tests for the pw_log_tokenized packed metadata class."""

import unittest

from pw_log_tokenized import Metadata


class TestMetadata(unittest.TestCase):
    """Tests extracting fields from a pw_log_tokenized packed metadata value."""

    def test_zero(self):
        metadata = Metadata(0)
        self.assertEqual(metadata.log_level, 0)
        self.assertEqual(metadata.line, 0)
        self.assertEqual(metadata.flags, 0)
        self.assertEqual(metadata.module_token, 0)

    def test_various(self):
        metadata = Metadata(
            0xABCD << 16 | 1 << 14 | 1234 << 3 | 5,
            log_bits=3,
            line_bits=11,
            flag_bits=2,
            module_bits=16,
        )
        self.assertEqual(metadata.log_level, 5)
        self.assertEqual(metadata.line, 1234)
        self.assertEqual(metadata.flags, 1)
        self.assertEqual(metadata.module_token, 0xABCD)

    def test_max(self):
        metadata = Metadata(
            0xFFFFFFFF, log_bits=3, line_bits=11, flag_bits=2, module_bits=16
        )
        self.assertEqual(metadata.log_level, 7)
        self.assertEqual(metadata.line, 2047)
        self.assertEqual(metadata.flags, 3)
        self.assertEqual(metadata.module_token, 0xFFFF)


if __name__ == '__main__':
    unittest.main()
