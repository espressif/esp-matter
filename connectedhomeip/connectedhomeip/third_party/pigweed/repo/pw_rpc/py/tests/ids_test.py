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
"""Tests service and method ID calculation for Python and C++."""

from typing import Iterator
import unittest

from pw_build.generated_tests import Context, TestGenerator
from pw_build import generated_tests
from pw_rpc import ids

_TESTS = TestGenerator(
    [
        'Empty string',
        (0x00000000, ''),
        'Single character strings',
        (0x00000001, '\0'),
        (0x00010040, '\1'),
        (0x003F0F82, '?'),
        'Non-printable strings',
        (0xD3556087, '\0\0\0\1\1\1\1'),
        'General strings',
        (0x63D43D8C, 'Pigweed?'),
        (0x79AB6494, 'Pigweed!Pigweed!Pigweed!Pigweed!Pigweed!Pigweed!'),
    ]
)


def _define_py_test(ctx: Context):
    expected_id, name = ctx.test_case
    return lambda self: self.assertEqual(expected_id, ids.calculate(name))


IdsTest = _TESTS.python_tests('IdsTest', _define_py_test)

_CC_HEADER = """\
#include <string_view>

#include "gtest/gtest.h"
#include "pw_rpc/internal/hash.h"

namespace pw::rpc::internal {

using namespace std::string_view_literals;
"""

_CC_FOOTER = '}  // namespace pw::rpc::internal'


def _cc_test(ctx: Context) -> Iterator[str]:
    expected_id, name = ctx.test_case

    yield f'TEST(RpcIds, {ctx.cc_name()}) {{'
    yield f'    EXPECT_EQ(0x{expected_id:08x}u,'
    yield f'              Hash({generated_tests.cc_string(name)}sv));'
    yield '}'


if __name__ == '__main__':
    args = generated_tests.parse_test_generation_args()
    if args.generate_cc_test:
        _TESTS.cc_tests(args.generate_cc_test, _cc_test, _CC_HEADER, _CC_FOOTER)
    else:
        unittest.main()
