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
"""Tests for file_operations module."""

import os
import shutil
import tempfile
import unittest
from pathlib import Path
from parameterized import parameterized  # type: ignore

from pw_arduino_build import file_operations


def file_set():
    return [
        "app.ino",
        "core/asm.S",
        "core/asm.s",
        "core/pwm/pulse.c",
        "core/pwm/pulse.h",
        "libraries/a.c",
        "libraries/b.cpp",
        "libraries/c.cc",
        "libraries/c.h",
    ]


def create_files(root_dir, file_names):
    for file_name in file_names:
        folder_path = Path(root_dir) / Path(os.path.dirname(file_name))
        folder_path.mkdir(parents=True, exist_ok=True)
        file_path = Path(root_dir) / Path(file_name)
        file_path.touch(exist_ok=True)


class TestFileOperations(unittest.TestCase):
    """Tests to ensure arduino core library source files can be found."""

    def setUp(self):
        self.test_dir = tempfile.mkdtemp()

    def tearDown(self):
        shutil.rmtree(self.test_dir)

    @parameterized.expand(
        [
            (
                "sources recursive",
                file_set(),
                ["**/*.ino", "**/*.h", "**/*.cpp"],
                [
                    "app.ino",
                    os.path.join("core", "pwm", "pulse.h"),
                    os.path.join("libraries", "b.cpp"),
                    os.path.join("libraries", "c.h"),
                ],
            ),
            (
                "directories recursive",
                file_set(),
                ["**"],
                [
                    "core",
                    os.path.join("core", "pwm"),
                    "libraries",
                ],
            ),
            (
                "directories one level deep",
                file_set(),
                ["*"],
                [
                    "core",
                    "libraries",
                ],
            ),
            (
                "items one level deep",
                file_set(),
                ["*"],
                [
                    "app.ino",
                    "core",
                    "libraries",
                ],
            ),
        ]
    )
    def test_find_files(
        self, test_case, base_fileset, patterns, expected_results
    ):
        """Test find_files on source files and directories."""
        create_files(self.test_dir, base_fileset)
        result = file_operations.find_files(
            self.test_dir,
            patterns,
            directories_only=("directories" in test_case),
        )
        self.assertSequenceEqual(expected_results, result)


if __name__ == '__main__':
    unittest.main()
