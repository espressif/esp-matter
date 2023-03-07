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
"""Finds files for a given product."""

import unittest

from pw_stm32cube_build import icf_to_ld


class ParseIcfTest(unittest.TestCase):
    """parse_icf tests."""

    TEST_ICF_1 = """
/*test comments*/
// some other comments
define symbol __ICFEDIT_intvec_start__ = 0x08000000;
/*-Memory Regions-*/
define symbol __ICFEDIT_region_ROM_start__   = 0x08000000;
define symbol __ICFEDIT_region_ROM_end__     = 0x0807FFFF;
define symbol __ICFEDIT_region_RAM_start__   = 0x20000000;
define symbol __ICFEDIT_region_RAM_end__     = 0x2002FFFF;

/*-Sizes-*/
define symbol __ICFEDIT_size_cstack__ = 0x400;
define symbol __ICFEDIT_size_heap__   = 0x200;
/**** End of ICF editor section. ###ICF###*/

define symbol __region_SRAM1_start__  = 0x20000000;
define symbol __region_SRAM1_end__    = 0x2002FFFF;
define symbol __region_SRAM2_start__  = 0x20030000;
define symbol __region_SRAM2_end__    = 0x2003FFFF;

define memory mem with size = 4G;
define region ROM_region      = mem:[from __ICFEDIT_region_ROM_start__   to __ICFEDIT_region_ROM_end__];
define region RAM_region      = mem:[from __ICFEDIT_region_RAM_start__   to __ICFEDIT_region_RAM_end__];
define region SRAM1_region    = mem:[from __region_SRAM1_start__   to __region_SRAM1_end__];
define region SRAM2_region    = mem:[from __region_SRAM2_start__   to __region_SRAM2_end__];

define block CSTACK    with alignment = 8, size = __ICFEDIT_size_cstack__   { };
define block HEAP      with alignment = 8, size = __ICFEDIT_size_heap__     { };

initialize by copy { readwrite };
do not initialize  { section .noinit };

place at address mem:__ICFEDIT_intvec_start__ { readonly section .intvec };

place in ROM_region   { readonly };
place in RAM_region   { readwrite,
                        block CSTACK, block HEAP };
place in SRAM1_region { };
place in SRAM2_region { };
"""

    TEST_ICF_2 = """
/*test comments*/
// some other comments
/*-Specials-*/
define symbol __ICFEDIT_intvec_start__ = 0x08000000;
/*-Memory Regions-*/
define symbol __ICFEDIT_region_ROM_start__    = 0x08000000;
define symbol __ICFEDIT_region_ROM_end__      = 0x081FFFFF;
define symbol __ICFEDIT_region_RAM_start__    = 0x20000000;
define symbol __ICFEDIT_region_RAM_end__      = 0x2002FFFF;
define symbol __ICFEDIT_region_CCMRAM_start__ = 0x10000000;
define symbol __ICFEDIT_region_CCMRAM_end__   = 0x1000FFFF;
/*-Sizes-*/
define symbol __ICFEDIT_size_cstack__ = 0x400;
define symbol __ICFEDIT_size_heap__   = 0x200;
/**** End of ICF editor section. ###ICF###*/


define memory mem with size = 4G;
define region ROM_region      = mem:[from __ICFEDIT_region_ROM_start__   to __ICFEDIT_region_ROM_end__];
define region RAM_region      = mem:[from __ICFEDIT_region_RAM_start__   to __ICFEDIT_region_RAM_end__];
define region CCMRAM_region   = mem:[from __ICFEDIT_region_CCMRAM_start__   to __ICFEDIT_region_CCMRAM_end__];

define block CSTACK    with alignment = 8, size = __ICFEDIT_size_cstack__   { };
define block HEAP      with alignment = 8, size = __ICFEDIT_size_heap__     { };

initialize by copy { readwrite };
do not initialize  { section .noinit };

place at address mem:__ICFEDIT_intvec_start__ { readonly section .intvec };

place in ROM_region   { readonly };
place in RAM_region   { readwrite,
                        block CSTACK, block HEAP };
"""

    def test_parse_icf_2(self):
        regions, blocks = icf_to_ld.parse_icf(self.TEST_ICF_2)

        self.assertEqual(
            {
                'ROM': ('0x08000000', '0x081FFFFF'),
                'RAM': ('0x20000000', '0x2002FFFF'),
                'CCMRAM': ('0x10000000', '0x1000FFFF'),
            },
            regions,
        )

        self.assertEqual(
            {
                'CSTACK': {'alignment': '8', 'size': '0x400'},
                'HEAP': {'alignment': '8', 'size': '0x200'},
            },
            blocks,
        )


class IcfRegionsToLdRegionsTest(unittest.TestCase):
    """icf_regions_to_ld_regions tests."""

    def test_icf_region(self):
        ld_regions = icf_to_ld.icf_regions_to_ld_regions(
            {
                'ROM': ('0x08000000', '0x081FFFFF'),
                'RAM': ('0x20000000', '0x2002FFFF'),
                'CCMRAM': ('0x10000000', '0x1000FFFF'),
            }
        )

        self.assertEqual(
            {
                'FLASH': ('0x08000000', '2048K'),
                'RAM': ('0x20000000', '192K'),
                'CCMRAM': ('0x10000000', '64K'),
            },
            ld_regions,
        )

    def test_icf_region_off_by_one(self):
        ld_regions = icf_to_ld.icf_regions_to_ld_regions(
            {
                'ROM': ('0x08000000', '0x080FFFFF'),
                'RAM': ('0x20000000', '0x20020000'),
            }
        )

        self.assertEqual(
            {
                'FLASH': ('0x08000000', '1024K'),
                'RAM': ('0x20000000', '128K'),
            },
            ld_regions,
        )


class CreateLdTest(unittest.TestCase):
    """create_ld tests."""

    def test_create_ld(self):
        ld_str = icf_to_ld.create_ld(
            {
                'FLASH': ('0x08000000', '2048K'),
                'RAM': ('0x20000000', '192K'),
                'CCMRAM': ('0x10000000', '64K'),
            },
            {
                'CSTACK': {'alignment': '8', 'size': '0x400'},
                'HEAP': {'alignment': '8', 'size': '0x200'},
            },
        )

        self.assertTrue(
            'RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 192K' in ld_str
        )
        self.assertTrue(
            'FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 2048K' in ld_str
        )


if __name__ == '__main__':
    unittest.main()
