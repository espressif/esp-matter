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
"""Tests the pw_rpc.console_tools.functions module."""

import unittest

from pw_rpc.console_tools import functions


def func(  # pylint: disable=unused-argument
    one, two: int, *a: bool, three=3, four: 'int' = 4, **kw
) -> None:
    """This is the docstring.

    More stuff.
    """


_EXPECTED_HELP = """\
func(one, two: int, *a: bool, three = 3, four: int = 4, **kw) -> None:

    This is the docstring.

    More stuff."""


class TestFunctions(unittest.TestCase):
    def test_format_no_args_function_help(self) -> None:
        def simple_function():
            pass

        self.assertEqual(
            functions.format_function_help(simple_function),
            'simple_function():\n\n    (no docstring)',
        )

    def test_format_complex_function_help(self) -> None:
        self.assertEqual(functions.format_function_help(func), _EXPECTED_HELP)

    def test_help_as_repr_with_docstring_help(self) -> None:
        wrapped = functions.help_as_repr(func)
        self.assertEqual(repr(wrapped), _EXPECTED_HELP)

    def test_help_as_repr_decorator(self) -> None:
        @functions.help_as_repr
        def no_docs():
            pass

        self.assertEqual(repr(no_docs), 'no_docs():\n\n    (no docstring)')

    def test_help_as_repr_call_no_args(self) -> None:
        self.assertEqual(functions.help_as_repr(lambda: 9876)(), 9876)

    def test_help_as_repr_call_with_arg(self) -> None:
        value = object()
        self.assertIs(functions.help_as_repr(lambda arg: arg)(value), value)


if __name__ == '__main__':
    unittest.main()
