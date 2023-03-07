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
"""Tests for pw_build.create_python_tree"""

import io
import os
from pathlib import Path
import tempfile
from typing import List
import unittest

from parameterized import parameterized  # type: ignore

from pw_build.python_package import PythonPackage
from pw_build.create_python_tree import (
    build_python_tree,
    copy_extra_files,
    load_common_config,
    update_config_with_packages,
)
from pw_build.generate_python_package import PYPROJECT_FILE


def _setup_cfg(package_name: str, install_requires: str = '') -> str:
    return f'''
[metadata]
name = {package_name}
version = 0.0.1
author = Pigweed Authors
author_email = pigweed-developers@googlegroups.com
description = Pigweed swiss-army knife

[options]
packages = find:
zip_safe = False
{install_requires}

[options.package_data]
{package_name} =
    py.typed
    '''


def _create_fake_python_package(
    location: Path,
    package_name: str,
    files: List[str],
    install_requires: str = '',
) -> None:
    for file in files:
        destination = location / file
        destination.parent.mkdir(parents=True, exist_ok=True)
        text = f'"""{package_name}"""'
        if str(destination).endswith('setup.cfg'):
            text = _setup_cfg(package_name, install_requires)
        elif str(destination).endswith('pyproject.toml'):
            # Make sure pyproject.toml file has valid syntax.
            text = PYPROJECT_FILE
        destination.write_text(text)


class TestCreatePythonTree(unittest.TestCase):
    """Integration tests for create_python_tree."""

    def setUp(self):
        self.maxDiff = None  # pylint: disable=invalid-name
        # Save the starting working directory for returning to later.
        self.start_dir = Path.cwd()
        # Create a temp out directory
        self.temp_dir = tempfile.TemporaryDirectory()

    def tearDown(self):
        # cd to the starting dir before cleaning up the temp out directory
        os.chdir(self.start_dir)
        # Delete the TemporaryDirectory
        self.temp_dir.cleanup()

    def _check_result_paths_equal(self, install_dir, expected_results) -> None:
        # Normalize path strings to posix before comparing.
        expected_paths = set(Path(p).as_posix() for p in expected_results)
        actual_paths = set(
            p.relative_to(install_dir).as_posix()
            for p in install_dir.glob('**/*')
            if p.is_file()
        )
        self.assertEqual(expected_paths, actual_paths)

    def test_update_config_with_packages(self) -> None:
        """Test merging package setup.cfg files."""
        temp_root = Path(self.temp_dir.name)
        common_config = temp_root / 'common_setup.cfg'
        common_config.write_text(
            '''
[metadata]
name = megapackage
version = 0.0.1
author = Pigweed Authors
author_email = pigweed-developers@googlegroups.com
description = Pigweed swiss-army knife

[options]
zip_safe = False

[options.package_data]
megapackage =
    py.typed
'''
        )
        config = load_common_config(
            common_config=common_config, append_git_sha=False, append_date=False
        )
        config_metadata = dict(config['metadata'].items())
        self.assertIn('name', config_metadata)

        pkg1_root = temp_root / 'pkg1'
        pkg2_root = temp_root / 'pkg2'
        _create_fake_python_package(
            pkg1_root,
            'mars',
            [
                'planets/BUILD.mars_rocket',
                'planets/mars/__init__.py',
                'planets/mars/__main__.py',
                'planets/mars/moons/__init__.py',
                'planets/mars/moons/deimos.py',
                'planets/mars/moons/phobos.py',
                'planets/hohmann_transfer_test.py',
                'planets/pyproject.toml',
                'planets/setup.cfg',
            ],
            install_requires='''
install_requires =
        coloredlogs
        coverage
        cryptography
        graphlib-backport;python_version<'3.9'
        httpwatcher
''',
        )

        os.chdir(pkg1_root)
        pkg1 = PythonPackage.from_dict(
            **{
                'generate_setup': {
                    'metadata': {'name': 'mars', 'version': '0.0.1'},
                },
                'inputs': [],
                'setup_sources': [
                    'planets/pyproject.toml',
                    'planets/setup.cfg',
                ],
                'sources': [
                    'planets/mars/__init__.py',
                    'planets/mars/__main__.py',
                    'planets/mars/moons/__init__.py',
                    'planets/mars/moons/deimos.py',
                    'planets/mars/moons/phobos.py',
                ],
                'tests': [
                    'planets/hohmann_transfer_test.py',
                ],
            }
        )

        _create_fake_python_package(
            pkg2_root,
            'saturn',
            [
                'planets/BUILD.saturn_rocket',
                'planets/hohmann_transfer_test.py',
                'planets/pyproject.toml',
                'planets/saturn/__init__.py',
                'planets/saturn/__main__.py',
                'planets/saturn/misson.py',
                'planets/saturn/moons/__init__.py',
                'planets/saturn/moons/enceladus.py',
                'planets/saturn/moons/iapetus.py',
                'planets/saturn/moons/rhea.py',
                'planets/saturn/moons/titan.py',
                'planets/setup.cfg',
                'planets/setup.py',
            ],
            install_requires='''
install_requires =
        graphlib-backport;python_version<'3.9'
        httpwatcher
''',
        )
        os.chdir(pkg2_root)
        pkg2 = PythonPackage.from_dict(
            **{
                'inputs': [],
                'setup_sources': [
                    'planets/pyproject.toml',
                    'planets/setup.cfg',
                    'planets/setup.py',
                ],
                'sources': [
                    'planets/saturn/__init__.py',
                    'planets/saturn/__main__.py',
                    'planets/saturn/misson.py',
                    'planets/saturn/moons/__init__.py',
                    'planets/saturn/moons/enceladus.py',
                    'planets/saturn/moons/iapetus.py',
                    'planets/saturn/moons/rhea.py',
                    'planets/saturn/moons/titan.py',
                ],
                'tests': [
                    'planets/hohmann_transfer_test.py',
                ],
            }
        )

        update_config_with_packages(config=config, python_packages=[pkg1, pkg2])

        setup_cfg_text = io.StringIO()
        config.write(setup_cfg_text)
        expected_cfg = '''
[metadata]
name = megapackage
version = 0.0.1
author = Pigweed Authors
author_email = pigweed-developers@googlegroups.com
description = Pigweed swiss-army knife

[options]
zip_safe = False
packages = find:
install_requires =
    coloredlogs
    coverage
    cryptography
    graphlib-backport;python_version<'3.9'
    httpwatcher

[options.package_data]
megapackage =
    py.typed
mars =
    py.typed
saturn =
    py.typed

[options.entry_points]
'''
        result_cfg_lines = [
            line.rstrip().replace('\t', '    ')
            for line in setup_cfg_text.getvalue().splitlines()
            if line
        ]
        expected_cfg_lines = [
            line.rstrip() for line in expected_cfg.splitlines() if line
        ]
        self.assertEqual(expected_cfg_lines, result_cfg_lines)

    @parameterized.expand(
        [
            (
                # Test name
                'working case',
                # Package name
                'mars',
                # File list
                [
                    'planets/BUILD.mars_rocket',
                    'planets/mars/__init__.py',
                    'planets/mars/__main__.py',
                    'planets/mars/moons/__init__.py',
                    'planets/mars/moons/deimos.py',
                    'planets/mars/moons/phobos.py',
                    'planets/hohmann_transfer_test.py',
                    'planets/pyproject.toml',
                    'planets/setup.cfg',
                ],
                # Extra_files
                [],
                # Package definition
                {
                    'generate_setup': {
                        'metadata': {'name': 'mars', 'version': '0.0.1'},
                    },
                    'inputs': [],
                    'setup_sources': [
                        'planets/pyproject.toml',
                        'planets/setup.cfg',
                    ],
                    'sources': [
                        'planets/mars/__init__.py',
                        'planets/mars/__main__.py',
                        'planets/mars/moons/__init__.py',
                        'planets/mars/moons/deimos.py',
                        'planets/mars/moons/phobos.py',
                    ],
                    'tests': [
                        'planets/hohmann_transfer_test.py',
                    ],
                },
                # Output file list
                [
                    'mars/__init__.py',
                    'mars/__main__.py',
                    'mars/moons/__init__.py',
                    'mars/moons/deimos.py',
                    'mars/moons/phobos.py',
                    'mars/tests/hohmann_transfer_test.py',
                ],
            ),
            (
                # Test name
                'with extra files',
                # Package name
                'saturn',
                # File list
                [
                    'planets/BUILD.saturn_rocket',
                    'planets/hohmann_transfer_test.py',
                    'planets/pyproject.toml',
                    'planets/saturn/__init__.py',
                    'planets/saturn/__main__.py',
                    'planets/saturn/misson.py',
                    'planets/saturn/moons/__init__.py',
                    'planets/saturn/moons/enceladus.py',
                    'planets/saturn/moons/iapetus.py',
                    'planets/saturn/moons/rhea.py',
                    'planets/saturn/moons/titan.py',
                    'planets/setup.cfg',
                    'planets/setup.py',
                ],
                # Extra files
                [
                    'planets/BUILD.saturn_rocket > out/saturn/BUILD.rocket',
                ],
                # Package definition
                {
                    'inputs': [],
                    'setup_sources': [
                        'planets/pyproject.toml',
                        'planets/setup.cfg',
                        'planets/setup.py',
                    ],
                    'sources': [
                        'planets/saturn/__init__.py',
                        'planets/saturn/__main__.py',
                        'planets/saturn/misson.py',
                        'planets/saturn/moons/__init__.py',
                        'planets/saturn/moons/enceladus.py',
                        'planets/saturn/moons/iapetus.py',
                        'planets/saturn/moons/rhea.py',
                        'planets/saturn/moons/titan.py',
                    ],
                    'tests': [
                        'planets/hohmann_transfer_test.py',
                    ],
                },
                # Output file list
                [
                    'saturn/BUILD.rocket',
                    'saturn/__init__.py',
                    'saturn/__main__.py',
                    'saturn/misson.py',
                    'saturn/moons/__init__.py',
                    'saturn/moons/enceladus.py',
                    'saturn/moons/iapetus.py',
                    'saturn/moons/rhea.py',
                    'saturn/moons/titan.py',
                    'saturn/tests/hohmann_transfer_test.py',
                ],
            ),
        ]
    )
    def test_build_python_tree(
        self,
        _test_name,
        package_name,
        file_list,
        extra_files,
        package_definition,
        expected_file_list,
    ) -> None:
        """Check results of build_python_tree and copy_extra_files."""
        temp_root = Path(self.temp_dir.name)
        _create_fake_python_package(temp_root, package_name, file_list)

        os.chdir(temp_root)
        install_dir = temp_root / 'out'

        package = PythonPackage.from_dict(**package_definition)
        build_python_tree(
            python_packages=[package],
            tree_destination_dir=install_dir,
            include_tests=True,
        )
        copy_extra_files(extra_files)

        # Check expected files are in place.
        self._check_result_paths_equal(install_dir, expected_file_list)

    @parameterized.expand(
        [
            (
                # Test name
                'everything in correct locations',
                # Package name
                'planets',
                # File list
                [
                    'BUILD.mars_rocket',
                ],
                # Extra_files
                [
                    'BUILD.mars_rocket > out/mars/BUILD.rocket',
                ],
                # Output file list
                [
                    'mars/BUILD.rocket',
                ],
                # Should raise exception
                None,
            ),
            (
                # Test name
                'missing source files',
                # Package name
                'planets',
                # File list
                [
                    'BUILD.mars_rocket',
                ],
                # Extra_files
                [
                    'BUILD.venus_rocket > out/venus/BUILD.rocket',
                ],
                # Output file list
                [],
                # Should raise exception
                FileNotFoundError,
            ),
            (
                # Test name
                'existing destination files',
                # Package name
                'planets',
                # File list
                [
                    'BUILD.jupiter_rocket',
                    'out/jupiter/BUILD.rocket',
                ],
                # Extra_files
                [
                    'BUILD.jupiter_rocket > out/jupiter/BUILD.rocket',
                ],
                # Output file list
                [],
                # Should raise exception
                FileExistsError,
            ),
        ]
    )
    def test_copy_extra_files(
        self,
        _test_name,
        package_name,
        file_list,
        extra_files,
        expected_file_list,
        should_raise_exception,
    ) -> None:
        """Check results of build_python_tree and copy_extra_files."""
        temp_root = Path(self.temp_dir.name)
        _create_fake_python_package(temp_root, package_name, file_list)

        os.chdir(temp_root)
        install_dir = temp_root / 'out'

        # If exceptions should be raised
        if should_raise_exception:
            with self.assertRaises(should_raise_exception):
                copy_extra_files(extra_files)
            return

        # Do the copy
        copy_extra_files(extra_files)
        # Check expected files are in place.
        self._check_result_paths_equal(install_dir, expected_file_list)


if __name__ == '__main__':
    unittest.main()
