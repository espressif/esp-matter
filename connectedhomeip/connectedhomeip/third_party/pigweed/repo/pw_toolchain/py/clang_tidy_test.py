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
"""Tests for clang_tidy."""

import pathlib
import unittest
from unittest import mock

from pw_toolchain import clang_tidy


class ClangTidyTest(unittest.TestCase):
    """Unit tests for the clang-tidy wrapper."""

    @mock.patch('subprocess.run', autospec=True)
    def test_source_exclude_filters(self, mock_run):
        # Build the path using joinpath to use OS-appropriate separators on both
        # Windows and Linux.
        source_file = (
            pathlib.Path('..').joinpath('third_party').joinpath('somefile.cc')
        )
        source_root = pathlib.Path('..')
        source_exclude = ['third_party.*']
        extra_args = ['END_OF_INVOKER']
        got = clang_tidy.main(
            False,
            'clang-tidy',
            source_file,
            source_root,
            None,
            source_exclude,
            list(),
            extra_args,
        )

        # Return code is zero.
        self.assertEqual(got, 0)
        # No calls to subprocess: we filtered out the source file.
        self.assertEqual(len(mock_run.mock_calls), 0)

    @mock.patch('subprocess.run', autospec=True)
    def test_source_exclude_does_not_filter(self, mock_run):
        mock_run.return_value.returncode = 0
        source_file = (
            pathlib.Path('..').joinpath('third_party').joinpath('somefile.cc')
        )
        source_root = pathlib.Path('..')
        source_exclude = ['someotherdir.*']
        extra_args = ['END_OF_INVOKER']
        got = clang_tidy.main(
            False,
            'clang-tidy',
            source_file,
            source_root,
            None,
            source_exclude,
            list(),
            extra_args,
        )

        # Return code is zero.
        self.assertEqual(got, 0)
        # One call to subprocess: we did not filter out the source file.
        # There will be more than one mock call because accessing return value
        # attributes also produces mock_calls.
        self.assertGreater(len(mock_run.mock_calls), 0)


if __name__ == '__main__':
    unittest.main()
