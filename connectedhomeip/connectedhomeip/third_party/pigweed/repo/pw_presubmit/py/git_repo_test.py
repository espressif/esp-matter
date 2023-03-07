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
"""git repo module tests"""

from unittest import mock
import re
import pathlib
import unittest

from pw_presubmit import git_repo


class TestGitRepo(unittest.TestCase):
    """Tests for git_repo.py"""

    GIT_ROOT = pathlib.Path("/dev/null/test")
    SUBMODULES = [
        pathlib.Path("third_party/pigweed"),
        pathlib.Path("vendor/anycom/p1"),
        pathlib.Path("vendor/anycom/p2"),
    ]
    GIT_SUBMODULES_OUT = "\n".join([str(x) for x in SUBMODULES])

    def setUp(self) -> None:
        self.git_stdout = mock.patch.object(
            git_repo, "git_stdout", autospec=True
        ).start()
        self.git_stdout.return_value = self.GIT_SUBMODULES_OUT
        self.root = mock.patch.object(git_repo, "root", autospec=True).start()
        self.root.return_value = self.GIT_ROOT
        super().setUp()

    def tearDown(self) -> None:
        mock.patch.stopall()
        super().tearDown()

    def test_mock_root(self):
        """Ensure our mock works since so many of our tests depend upon it."""
        self.assertEqual(git_repo.root(), self.GIT_ROOT)

    def test_discover_submodules_1(self):
        paths = git_repo.discover_submodules(superproject_dir=self.GIT_ROOT)
        self.assertIn(self.GIT_ROOT, paths)

    def test_discover_submodules_2(self):
        paths = git_repo.discover_submodules(superproject_dir=self.GIT_ROOT)
        self.assertIn(self.SUBMODULES[2], paths)

    def test_discover_submodules_with_exclude_str(self):
        paths = git_repo.discover_submodules(
            superproject_dir=self.GIT_ROOT,
            excluded_paths=(self.GIT_ROOT.as_posix(),),
        )
        self.assertNotIn(self.GIT_ROOT, paths)

    def test_discover_submodules_with_exclude_regex(self):
        paths = git_repo.discover_submodules(
            superproject_dir=self.GIT_ROOT,
            excluded_paths=(re.compile("third_party/.*"),),
        )
        self.assertNotIn(self.SUBMODULES[0], paths)

    def test_discover_submodules_with_exclude_str_miss(self):
        paths = git_repo.discover_submodules(
            superproject_dir=self.GIT_ROOT,
            excluded_paths=(re.compile("pigweed"),),
        )
        self.assertIn(self.SUBMODULES[-1], paths)

    def test_discover_submodules_with_exclude_regex_miss_1(self):
        paths = git_repo.discover_submodules(
            superproject_dir=self.GIT_ROOT,
            excluded_paths=(re.compile("foo/.*"),),
        )
        self.assertIn(self.GIT_ROOT, paths)
        for module in self.SUBMODULES:
            self.assertIn(module, paths)

    def test_discover_submodules_with_exclude_regex_miss_2(self):
        paths = git_repo.discover_submodules(
            superproject_dir=self.GIT_ROOT,
            excluded_paths=(re.compile("pigweed"),),
        )
        self.assertIn(self.GIT_ROOT, paths)
        for module in self.SUBMODULES:
            self.assertIn(module, paths)


if __name__ == '__main__':
    unittest.main()
