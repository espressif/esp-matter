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

from pw_stm32cube_build import inject_init


class AddPreMainInitTest(unittest.TestCase):
    """add_pre_main_init tests."""

    def test_spaces(self):
        startup = '\n'.join(
            [
                '/* Call the clock system intitialization function.*/',
                '  bl  SystemInit   ',
                '/* Call static constructors */',
                '    bl __libc_init_array',
                '/* Call the application\'s entry point.*/',
                '  bl  main',
                '  bx  lr    ',
                '.size  Reset_Handler, .-Reset_Handler',
            ]
        )

        new_startup = inject_init.add_pre_main_init(startup)

        self.assertEqual(
            new_startup,
            '\n'.join(
                [
                    '/* Call the clock system intitialization function.*/',
                    '  bl  SystemInit   ',
                    '/* Call static constructors */',
                    '    bl __libc_init_array',
                    '/* Call the application\'s entry point.*/',
                    'bl pw_stm32cube_Init',
                    '  bl  main',
                    '  bx  lr    ',
                    '.size  Reset_Handler, .-Reset_Handler',
                ]
            ),
        )

    def test_tabs(self):
        startup = '\n'.join(
            [
                'LoopFillZerobss:',
                '	ldr	r3, = _ebss',
                '	cmp	r2, r3',
                '	bcc	FillZerobss',
                '/* Call static constructors */',
                '  bl __libc_init_array',
                '/* Call the application\'s entry point.*/',
                '	bl	main',
                '',
                'LoopForever:',
                '    b LoopForever',
            ]
        )

        new_startup = inject_init.add_pre_main_init(startup)

        self.assertEqual(
            new_startup,
            '\n'.join(
                [
                    'LoopFillZerobss:',
                    '	ldr	r3, = _ebss',
                    '	cmp	r2, r3',
                    '	bcc	FillZerobss',
                    '/* Call static constructors */',
                    '  bl __libc_init_array',
                    '/* Call the application\'s entry point.*/',
                    'bl pw_stm32cube_Init',
                    '	bl	main',
                    '',
                    'LoopForever:',
                    '    b LoopForever',
                ]
            ),
        )

    def test_main_not_found(self):
        startup = '\n'.join(
            [
                '/* Call the clock system intitialization function.*/',
                '  bl  SystemInit   ',
                '/* Call static constructors */',
                '    bl __libc_init_array',
                '/* Call the application\'s entry point.*/',
                '  bl  application_entry',
                '  bx  lr    ',
                '.size  Reset_Handler, .-Reset_Handler',
            ]
        )

        with self.assertRaises(ValueError):
            inject_init.add_pre_main_init(startup)


if __name__ == '__main__':
    unittest.main()
