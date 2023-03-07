# Copyright 2022 The Pigweed Authors
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
"""Tests for pw_watch.build_recipe"""

from pathlib import Path
import unittest

from parameterized import parameterized  # type: ignore

from pw_build.build_recipe import BuildCommand


# pylint: disable=line-too-long
class TestBuildRecipe(unittest.TestCase):
    """Tests for creating BuildRecipes."""

    def setUp(self):
        self.maxDiff = None  # pylint: disable=invalid-name

    @parameterized.expand(
        [
            (
                'build command using make',
                BuildCommand(
                    build_dir=Path('outmake'),
                    build_system_command='make',
                    build_system_extra_args=['-k'],
                    targets=['maketarget1', 'maketarget2'],
                ),
                # result
                ['make', '-k', '-C', 'outmake', 'maketarget1', 'maketarget2'],
            ),
            (
                'build command using bazel',
                BuildCommand(
                    build_dir=Path('outbazel'),
                    build_system_command='bazel',
                    build_system_extra_args=[],
                    targets=['build', '//pw_analog/...', '//pw_assert/...'],
                ),
                # result
                [
                    'bazel',
                    '--output_base',
                    'outbazel',
                    'build',
                    '//pw_analog/...',
                    '//pw_assert/...',
                ],
            ),
            (
                'cmake shell command',
                BuildCommand(
                    build_dir=Path('outcmake'),
                    command_string='cmake -G Ninja -S ./ -B outcmake',
                ),
                # result
                ['cmake', '-G', 'Ninja', '-S', './', '-B', 'outcmake'],
            ),
            (
                'gn shell command',
                BuildCommand(
                    build_dir=Path('out'),
                    command_string='gn gen out --export-compile-commands',
                ),
                # result
                ['gn', 'gen', 'out', '--export-compile-commands'],
            ),
            (
                'python shell command',
                BuildCommand(
                    build_dir=Path('outpytest'),
                    command_string='python pw_build/py/build_recipe_test.py',
                ),
                # result
                ['python', 'pw_build/py/build_recipe_test.py'],
            ),
        ]
    )
    def test_build_command_get_args(
        self,
        _test_name,
        build_command,
        expected_args,
    ) -> None:
        """Test BuildCommand get_args."""
        self.assertEqual(expected_args, build_command.get_args())


if __name__ == '__main__':
    unittest.main()
