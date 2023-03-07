#!/usr/bin/env python
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
"""Tests for pw_watch.minimal_watch_directories."""

import unittest
import tempfile
from pathlib import Path

from pw_watch import watch


class TestMinimalWatchDirectories(unittest.TestCase):
    """Tests for pw_watch.watch.minimal_watch_directories."""

    def setUp(self):
        self._tempdir = tempfile.TemporaryDirectory()
        self._root = Path(self._tempdir.name)

    def tearDown(self):
        self._tempdir.cleanup()

    def make_tree(self, *directories: str) -> None:
        for directory in directories:
            self._root.joinpath(directory).mkdir(parents=True)

    def test_empty_directory(self):
        subdirectories_to_watch = []
        ans_subdirectories_to_watch = [(self._root, False)]
        subdirectories_to_watch = watch.minimal_watch_directories(
            self._root, 'f1'
        )

        self.assertEqual(
            set(subdirectories_to_watch), set(ans_subdirectories_to_watch)
        )

    def test_non_exist_directories_to_exclude(self):
        subdirectories_to_watch = []
        exclude_list = ['f3']
        self.make_tree('f1', 'f2')
        ans_subdirectories_to_watch = [
            (self._root / 'f1', True),
            (self._root / 'f2', True),
            (self._root, False),
        ]
        subdirectories_to_watch = watch.minimal_watch_directories(
            self._root, exclude_list
        )

        self.assertEqual(
            set(subdirectories_to_watch), set(ans_subdirectories_to_watch)
        )

    def test_one_layer_directories(self):
        subdirectories_to_watch = []
        exclude_list = ['f1']
        self.make_tree(
            'f1/f1',
            'f1/f2',
            'f2/f1',
        )
        ans_subdirectories_to_watch = [
            (self._root / 'f2', True),
            (self._root, False),
        ]
        subdirectories_to_watch = watch.minimal_watch_directories(
            self._root, exclude_list
        )

        self.assertEqual(
            set(subdirectories_to_watch), set(ans_subdirectories_to_watch)
        )

    def test_two_layers_direcories(self):
        subdirectories_to_watch = []
        exclude_list = ['f1/f2']
        self.make_tree(
            'f1/f1',
            'f1/f2',
            'f2/f1',
        )
        ans_subdirectories_to_watch = [
            (self._root / 'f2', True),
            (self._root / 'f1/f1', True),
            (self._root, False),
            (self._root / 'f1', False),
        ]
        subdirectories_to_watch = watch.minimal_watch_directories(
            self._root, exclude_list
        )

        self.assertEqual(
            set(subdirectories_to_watch), set(ans_subdirectories_to_watch)
        )

    def test_empty_exclude_list(self):
        subdirectories_to_watch = []
        exclude_list = []
        self.make_tree(
            'f1/f1',
            'f1/f2',
            'f2/f1',
        )
        ans_subdirectories_to_watch = [
            (self._root / 'f2', True),
            (self._root / 'f1', True),
            (self._root, False),
        ]
        subdirectories_to_watch = watch.minimal_watch_directories(
            self._root, exclude_list
        )

        self.assertEqual(
            set(subdirectories_to_watch), set(ans_subdirectories_to_watch)
        )

    def test_multiple_directories_in_exclude_list(self):
        """test case for multiple directories to exclude"""
        subdirectories_to_watch = []
        exclude_list = [
            'f1/f2',
            'f3/f1',
            'f3/f3',
        ]
        self.make_tree(
            'f1/f1',
            'f1/f2',
            'f2/f1',
            'f3/f1',
            'f3/f2',
            'f3/f3',
        )
        ans_subdirectories_to_watch = [
            (self._root / 'f2', True),
            (self._root / 'f1/f1', True),
            (self._root / 'f3/f2', True),
            (self._root, False),
            (self._root / 'f1', False),
            (self._root / 'f3', False),
        ]
        subdirectories_to_watch = watch.minimal_watch_directories(
            self._root, exclude_list
        )

        self.assertEqual(
            set(subdirectories_to_watch), set(ans_subdirectories_to_watch)
        )

    def test_nested_sibling_exclusion(self):
        subdirectories_to_watch = []
        exclude_list = [
            'f1/f1/f1/f1/f1',
            'f1/f1/f1/f2',
        ]
        self.make_tree(
            'f1/f1/f1/f1/f1',
            'f1/f1/f1/f1/f2',
            'f1/f1/f1/f1/f3',
            'f1/f1/f1/f2',
        )
        ans_subdirectories_to_watch = [
            (self._root / 'f1/f1/f1/f1/f2', True),
            (self._root / 'f1/f1/f1/f1/f3', True),
            (self._root, False),
            (self._root / 'f1', False),
            (self._root / 'f1/f1', False),
            (self._root / 'f1/f1/f1', False),
            (self._root / 'f1/f1/f1/f1', False),
        ]
        subdirectories_to_watch = watch.minimal_watch_directories(
            self._root, exclude_list
        )

        self.assertEqual(
            set(subdirectories_to_watch), set(ans_subdirectories_to_watch)
        )


if __name__ == '__main__':
    unittest.main()
