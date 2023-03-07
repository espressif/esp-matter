#!/usr/bin/env python3
# Copyright 2022 The Pigweed Authors
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
"""Tests for bloaty configuration tooling."""

import unittest

from pw_bloat import bloaty_config


class BloatyConfigTest(unittest.TestCase):
    """Tests that the bloaty config tool produces the expected config."""

    def test_map_segments_to_memory_regions(self) -> None:
        """Ensures the mapping works correctly based on a real example."""
        segments = {
            3: (int(0x800F268), int(0x8100200)),
            5: (int(0x20004650), int(0x20020650)),
            6: (int(0x20020650), int(0x20030000)),
            1: (int(0x8000200), int(0x800F060)),
            4: (int(0x20000208), int(0x20004650)),
            2: (int(0x20000000), int(0x20000208)),
            0: (int(0x8000000), int(0x8000200)),
        }
        memory_regions = {
            'FLASH': {0: (int(0x8000200), int(0x8100200))},
            'RAM': {0: (int(0x20000000), int(0x20030000))},
            'VECTOR_TABLE': {0: (int(0x8000000), int(0x8000200))},
        }
        expected = {
            3: 'FLASH',
            5: 'RAM',
            6: 'RAM',
            1: 'FLASH',
            4: 'RAM',
            2: 'RAM',
            0: 'VECTOR_TABLE',
        }
        actual = bloaty_config.map_segments_to_memory_regions(
            segments=segments, memory_regions=memory_regions
        )
        self.assertEqual(expected, actual)

    def test_generate_memoryregions_data_source(self) -> None:
        """Ensures the formatted generation works correctly."""
        segments_to_memory_regions = {
            0: 'RAM',
            1: 'RAM',
            13: 'FLASH',
        }
        config = bloaty_config.generate_memoryregions_data_source(
            segments_to_memory_regions
        )
        expected = '\n'.join(
            (
                r'custom_data_source: {',
                r'  name: "memoryregions"',
                r'  base_data_source: "segments"',
                r'  rewrite: {',
                r'    pattern:"^LOAD #0 \\[.*\\]$"',
                r'    replacement:"RAM"',
                r'  }',
                r'  rewrite: {',
                r'    pattern:"^LOAD #1 \\[.*\\]$"',
                r'    replacement:"RAM"',
                r'  }',
                r'  rewrite: {',
                r'    pattern:"^LOAD #13 \\[.*\\]$"',
                r'    replacement:"FLASH"',
                r'  }',
                r'  rewrite: {',
                r'    pattern:".*"',
                r'    replacement:"Not resident in memory"',
                r'  }',
                r'}',
                r'',
            )
        )
        self.assertEqual(expected, config)

    def test_generate_utilization_data_source(self) -> None:
        config = bloaty_config.generate_utilization_data_source()
        expected = '\n'.join(
            (
                'custom_data_source: {',
                '  name:"utilization"',
                '  base_data_source:"sections"',
                '  rewrite: {',
                '    pattern:"unused_space"',
                '    replacement:"Free space"',
                '  }',
                '  rewrite: {',
                '    pattern:".*"',
                '    replacement:"Used space"',
                '  }',
                '}',
                '',
            )
        )
        self.assertEqual(expected, config)


if __name__ == '__main__':
    unittest.main()
