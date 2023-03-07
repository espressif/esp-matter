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
"""Tests the python_packages module."""

import importlib.metadata
from pathlib import Path
import tempfile
import unittest
from unittest import mock

from pw_env_setup import python_packages


class TestPythonPackages(unittest.TestCase):
    """Tests the python_packages module."""

    def setUp(self):
        self.existing_pkgs_minus_toml = '\n'.join(
            pkg
            for pkg in python_packages._installed_packages()  # pylint: disable=protected-access
            if not pkg.startswith('toml==')
        )
        self.temp_dir = tempfile.TemporaryDirectory()
        self.temp_path = Path(self.temp_dir.name)

    def tearDown(self):
        self.temp_dir.cleanup()

    def test_list(self):
        # pylint: disable=protected-access
        pkgs = list(python_packages._installed_packages())
        # pylint: enable=protected-access
        toml_version = importlib.metadata.version('toml')

        self.assertIn(f'toml=={toml_version}', pkgs)
        self.assertNotIn('pw-foo', pkgs)

    @mock.patch('pw_env_setup.python_packages._stderr')
    def test_diff_removed(self, stderr_mock):
        expected = 'foo==1.0\nbar==2.0\nbaz==3.0\n'
        expected_file = self.temp_path / 'test_diff_removed_expected'
        expected_file.write_text(expected, encoding='utf-8')

        # Removed packages should trigger a failure.
        self.assertEqual(-1, python_packages.diff(expected_file))

        stderr_mock.assert_any_call('Removed packages')
        stderr_mock.assert_any_call('  foo==1.0')
        stderr_mock.assert_any_call('  bar==2.0')
        stderr_mock.assert_any_call('  baz==3.0')

    @mock.patch('pw_env_setup.python_packages._stderr')
    def test_diff_updated(self, stderr_mock):
        expected = 'toml>=0.0.1\n' + self.existing_pkgs_minus_toml
        expected_file = self.temp_path / 'test_diff_updated_expected'
        expected_file.write_text(expected, encoding='utf-8')

        toml_version = importlib.metadata.version('toml')

        # Updated packages should trigger a failure.
        self.assertEqual(-1, python_packages.diff(expected_file))

        stderr_mock.assert_any_call('Updated packages')
        stderr_mock.assert_any_call(
            f'  toml=={toml_version} (from toml>=0.0.1)'
        )

    @mock.patch('pw_env_setup.python_packages._stderr')
    def test_diff_new(self, stderr_mock):
        expected = self.existing_pkgs_minus_toml
        expected_file = self.temp_path / 'test_diff_new_expected'
        expected_file.write_text(expected, encoding='utf-8')

        toml_version = importlib.metadata.version('toml')

        # New packages should trigger a failure.
        self.assertEqual(-1, python_packages.diff(expected_file))

        stderr_mock.assert_any_call('New packages')
        stderr_mock.assert_any_call(f'  toml=={toml_version}')
        stderr_mock.assert_any_call("Package versions don't match!")


if __name__ == '__main__':
    unittest.main()
