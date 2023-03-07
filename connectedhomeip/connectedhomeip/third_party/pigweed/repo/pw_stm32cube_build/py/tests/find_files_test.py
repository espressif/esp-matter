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
"""Generate File List Tests."""

import pathlib
import unittest

from pw_stm32cube_build import find_files


class ParseProductStringTest(unittest.TestCase):
    """parse_product_str tests."""

    def test_start_with_stm32(self):
        with self.assertRaises(ValueError):
            find_files.parse_product_str('f439zit')

    def test_specify_chip(self):
        with self.assertRaises(ValueError):
            find_files.parse_product_str('stm32f43')

    def test_stm32f412zx(self):
        (family, defines, name) = find_files.parse_product_str('stm32f412zx')

        self.assertEqual(family, 'stm32f4xx')
        self.assertEqual(defines, {'STM32F412xx', 'STM32F412Zx'})
        self.assertEqual(name, 'stm32f412zx')

    def test_stm32f439xx(self):
        (family, defines, name) = find_files.parse_product_str('STM32F439xx')

        self.assertEqual(family, 'stm32f4xx')
        self.assertEqual(defines, {'STM32F439xx'})
        self.assertEqual(name, 'stm32f439xx')

    def test_stm32f439(self):
        (family, defines, name) = find_files.parse_product_str('STM32F439')

        self.assertEqual(family, 'stm32f4xx')
        self.assertEqual(defines, {'STM32F439xx'})
        self.assertEqual(name, 'stm32f439xx')

    def test_stm32f439xi(self):
        (family, defines, name) = find_files.parse_product_str('STM32F439xI')

        self.assertEqual(family, 'stm32f4xx')
        self.assertEqual(defines, {'STM32F439xx', 'STM32F439xI'})
        self.assertEqual(name, 'stm32f439xi')

    def test_stm32f439zit6u(self):
        (family, defines, name) = find_files.parse_product_str('stm32f439zit6u')

        self.assertEqual(family, 'stm32f4xx')
        self.assertEqual(
            defines,
            {'STM32F439xx', 'STM32F439Zx', 'STM32F439xI', 'STM32F439ZI'},
        )
        self.assertEqual(name, 'stm32f439zit6u')

    def test_stm32l552zet(self):
        (family, defines, name) = find_files.parse_product_str('stm32l552zet')

        self.assertEqual(family, 'stm32l5xx')
        self.assertEqual(
            defines,
            {'STM32L552xx', 'STM32L552Zx', 'STM32L552xE', 'STM32L552ZE'},
        )
        self.assertEqual(name, 'stm32l552zet')

    def test_stm32l552xc(self):
        (family, defines, name) = find_files.parse_product_str('stm32l552xc')

        self.assertEqual(family, 'stm32l5xx')
        self.assertEqual(defines, {'STM32L552xx', 'STM32L552xC'})
        self.assertEqual(name, 'stm32l552xc')

    def test_stm32wb5m(self):
        (family, defines, name) = find_files.parse_product_str('stm32wb5m')

        self.assertEqual(family, 'stm32wbxx')
        self.assertEqual(defines, {'STM32WB5Mxx'})
        self.assertEqual(name, 'stm32wb5mxx')


class SelectDefineTest(unittest.TestCase):
    """select_define tests."""

    def test_stm32f412zx_not_found(self):
        with self.assertRaises(ValueError):
            find_files.select_define({'STM32F412xx', 'STM32F412Zx'}, "")

    def test_stm32f412zx_found(self):
        define = find_files.select_define(
            {'STM32F412xx', 'STM32F412Zx'},
            "asdf\nfdas\n#if defined(STM32F412Zx)\n",
        )
        self.assertEqual(define, 'STM32F412Zx')

    def test_stm32f412zx_multiple_found(self):
        with self.assertRaises(ValueError):
            find_files.select_define(
                {'STM32F412xx', 'STM32F412Zx'},
                "asdf\n#if defined (STM32F412xx)\n#elif defined(STM32F412Zx)\n",
            )


class MatchFilenameTest(unittest.TestCase):
    """match_filename tests."""

    def test_stm32f412zx(self):
        # Match should fail if product name is not specific enough
        self.assertTrue(
            find_files.match_filename('stm32f412zx', 'stm32f412zx_flash.icf')
        )
        self.assertFalse(
            find_files.match_filename('stm32f412xx', 'stm32f412zx_flash.icf')
        )
        self.assertTrue(
            find_files.match_filename('stm32f412zx', 'startup_stm32f412zx.s')
        )
        self.assertFalse(
            find_files.match_filename('stm32f412xx', 'startup_stm32f429zx.s')
        )

    def test_stm32f439xx(self):
        self.assertTrue(
            find_files.match_filename('stm32f439xx', 'stm32f439xx_flash.icf')
        )
        self.assertFalse(
            find_files.match_filename('stm32f439xx', 'stm32f429xx_flash.icf')
        )
        self.assertTrue(
            find_files.match_filename('stm32f439xx', 'startup_stm32f439xx.s')
        )
        self.assertFalse(
            find_files.match_filename('stm32f439xx', 'startup_stm32f429xx.s')
        )

    def test_stm32f439xi(self):
        self.assertTrue(
            find_files.match_filename('stm32f439xi', 'stm32f439xx_flash.icf')
        )
        self.assertFalse(
            find_files.match_filename('stm32f439xi', 'stm32f429xx_flash.icf')
        )
        self.assertTrue(
            find_files.match_filename('stm32f439xi', 'startup_stm32f439xx.s')
        )
        self.assertFalse(
            find_files.match_filename('stm32f439xi', 'startup_stm32f429xx.s')
        )

    def test_stm32l552zet(self):
        self.assertTrue(
            find_files.match_filename('stm32l552zet', 'STM32L552xE_FLASH.ld')
        )
        self.assertTrue(
            find_files.match_filename('stm32l552zet', 'STM32L552xx_FLASH.ld')
        )
        self.assertFalse(
            find_files.match_filename('stm32l552zet', 'STM32L552xC_FLASH.ld')
        )
        self.assertTrue(
            find_files.match_filename('stm32l552zet', 'stm32l552xe_flash.icf')
        )
        self.assertFalse(
            find_files.match_filename('stm32l552zet', 'stm32l552xc_flash.icf')
        )
        self.assertTrue(
            find_files.match_filename('stm32l552zet', 'startup_stm32l552xx.s')
        )
        self.assertFalse(
            find_files.match_filename('stm32l552zet', 'startup_stm32l562xx.s')
        )


class FindLinkerFilesTest(unittest.TestCase):
    """find_linker_files tests."""

    TEST_PATH = pathlib.Path('/test/path')

    def test_stm32f439xx(self):
        files = [
            'path/to/stm32f439xx_flash.icf',
            'other/path/to/stm32f439xx_sram.icf',
        ]
        gcc_linker, iar_linker = find_files.find_linker_files(
            'stm32f439xx', files, self.TEST_PATH
        )

        self.assertEqual(gcc_linker, None)
        self.assertEqual(iar_linker, self.TEST_PATH / files[0])

    def test_stm32f439xx_find_ld(self):
        files = [
            'path/to/stm32f439xx_flash.icf',
            'other/path/to/stm32f439xx_sram.icf',
            'path/to/STM32F439xx_FLASH.ld',
        ]
        gcc_linker, iar_linker = find_files.find_linker_files(
            'stm32f439xx', files, self.TEST_PATH
        )

        self.assertEqual(gcc_linker, self.TEST_PATH / files[2])
        self.assertEqual(iar_linker, self.TEST_PATH / files[0])

    def test_stm32f439xc_error_multiple_matching_ld(self):
        files = [
            'path/to/stm32f439xx_flash.icf',
            'other/path/to/stm32f439xx_sram.icf',
            'other/path/to/STM32F439xI_FLASH.ld',
            'path/to/STM32F439xx_FLASH.ld',
        ]
        with self.assertRaises(ValueError):
            find_files.find_linker_files('stm32f439xi', files, self.TEST_PATH)

    def test_stm32f439xc_error_multiple_matching_icf(self):
        files = [
            'path/to/stm32f439xx_flash.icf',
            'other/path/to/stm32f439xc_flash.icf',
        ]
        with self.assertRaises(ValueError):
            find_files.find_linker_files('stm32f439xc', files, self.TEST_PATH)

    def test_stm32f439xc_error_none_found(self):
        files = [
            'path/to/stm32f439xc_flash.icf',
            'other/path/to/stm32f439xc_flash.icf',
        ]
        with self.assertRaises(ValueError):
            find_files.find_linker_files('stm32f439xx', files, self.TEST_PATH)

    # ignore secure and nonsecure variants for the M33 boards
    def test_stm32l552xe_ignore_s_ns(self):
        files = [
            'iar/linker/stm32l552xe_flash_ns.icf',
            'iar/linker/stm32l552xe_flash_s.icf',
            'iar/linker/stm32l552xe_flash.icf',
            'gcc/linker/STM32L552xE_FLASH_ns.ld',
            'gcc/linker/STM32L552xE_FLASH_s.ld',
            'gcc/linker/STM32L552xE_FLASH.ld',
        ]
        gcc_linker, iar_linker = find_files.find_linker_files(
            'stm32l552xe', files, self.TEST_PATH
        )

        self.assertEqual(gcc_linker, self.TEST_PATH / files[-1])
        self.assertEqual(iar_linker, self.TEST_PATH / files[2])


class FindStartupFileTest(unittest.TestCase):
    """find_startup_file tests."""

    TEST_PATH = pathlib.Path('/test/path')

    def test_stm32f439xx_none_found(self):
        files = [
            'path/to/stm32f439xx_flash.icf',
            'other/path/to/stm32f439xx_sram.icf',
            'path/iar/startup_stm32f439xx.s',
        ]
        with self.assertRaises(ValueError):
            find_files.find_startup_file('stm32f439xx', files, self.TEST_PATH)

    def test_stm32f439xx(self):
        files = [
            'path/to/stm32f439xx_flash.icf',
            'other/path/to/stm32f439xx_sram.icf',
            'path/iar/startup_stm32f439xx.s',
            'path/gcc/startup_stm32f439xx.s',
        ]
        startup_file = find_files.find_startup_file(
            'stm32f439xx', files, self.TEST_PATH
        )

        self.assertEqual(startup_file, self.TEST_PATH / files[3])

    def test_stm32f439xx_multiple_found(self):
        files = [
            'path/to/stm32f439xx_flash.icf',
            'other/path/to/stm32f439xx_sram.icf',
            'path/gcc/startup_stm32f439xc.s',
            'path/gcc/startup_stm32f439xx.s',
        ]
        with self.assertRaises(ValueError):
            find_files.find_startup_file('stm32f439xc', files, self.TEST_PATH)


class GetSourceAndHeadersTest(unittest.TestCase):
    """test_sources_and_headers tests."""

    def test_sources_and_headers(self):
        files = [
            'random/header.h',
            'random/source.c',
            'cmsis_core/Include/core_cm4.h',
            'cmsis_device/Include/stm32f4xx.h',
            'cmsis_device/Include/stm32f439xx.h',
            'hal_driver/Inc/stm32f4xx_hal_eth.h',
            'hal_driver/Src/stm32f4xx_hal_adc.c',
            'hal_driver/Inc/stm32f4xx_hal.h',
            'hal_driver/Src/stm32f4xx_hal_timebase_tim_template.c',
            'hal_driver/Src/stm32f4xx_hal_eth.c',
        ]
        path = pathlib.Path('/test/path/to/stm32cube')
        sources, headers = find_files.get_sources_and_headers(files, path)
        self.assertSetEqual(
            set(
                [
                    str(path / 'hal_driver/Src/stm32f4xx_hal_adc.c'),
                    str(path / 'hal_driver/Src/stm32f4xx_hal_eth.c'),
                ]
            ),
            set(sources),
        )
        self.assertSetEqual(
            set(
                [
                    str(path / 'cmsis_core/Include/core_cm4.h'),
                    str(path / 'cmsis_device/Include/stm32f4xx.h'),
                    str(path / 'cmsis_device/Include/stm32f439xx.h'),
                    str(path / 'hal_driver/Inc/stm32f4xx_hal_eth.h'),
                    str(path / 'hal_driver/Inc/stm32f4xx_hal.h'),
                ]
            ),
            set(headers),
        )


if __name__ == '__main__':
    unittest.main()
