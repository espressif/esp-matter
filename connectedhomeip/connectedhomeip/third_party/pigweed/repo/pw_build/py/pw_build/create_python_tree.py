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
"""Build a Python Source tree."""

import argparse
import configparser
from datetime import datetime
import io
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
from typing import Iterable, Optional

import setuptools  # type: ignore

try:
    from pw_build.python_package import (
        PythonPackage,
        load_packages,
        change_working_dir,
    )
    from pw_build.generate_python_package import PYPROJECT_FILE

except ImportError:
    # Load from python_package from this directory if pw_build is not available.
    from python_package import (  # type: ignore
        PythonPackage,
        load_packages,
        change_working_dir,
    )
    from generate_python_package import PYPROJECT_FILE  # type: ignore


def _parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        '--repo-root',
        type=Path,
        help='Path to the root git repo.',
    )
    parser.add_argument(
        '--tree-destination-dir', type=Path, help='Path to output directory.'
    )
    parser.add_argument(
        '--include-tests',
        action='store_true',
        help='Include tests in the tests dir.',
    )

    parser.add_argument(
        '--setupcfg-common-file',
        type=Path,
        help='A file containing the common set of options for'
        'incluing in the merged setup.cfg provided version.',
    )
    parser.add_argument(
        '--setupcfg-version-append-git-sha',
        action='store_true',
        help='Append the current git SHA to the setup.cfg ' 'version.',
    )
    parser.add_argument(
        '--setupcfg-version-append-date',
        action='store_true',
        help='Append the current date to the setup.cfg ' 'version.',
    )
    parser.add_argument(
        '--setupcfg-override-name', help='Override metadata.name in setup.cfg'
    )
    parser.add_argument(
        '--setupcfg-override-version',
        help='Override metadata.version in setup.cfg',
    )
    parser.add_argument(
        '--create-default-pyproject-toml',
        action='store_true',
        help='Generate a default pyproject.toml file',
    )

    parser.add_argument(
        '--extra-files',
        nargs='+',
        help='Paths to extra files that should be included in the output dir.',
    )

    parser.add_argument(
        '--input-list-files',
        nargs='+',
        type=Path,
        help='Paths to text files containing lists of Python package metadata '
        'json files.',
    )

    return parser.parse_args()


class UnknownGitSha(Exception):
    """Exception thrown when the current git SHA cannot be found."""


def get_current_git_sha(repo_root: Optional[Path] = None) -> str:
    if not repo_root:
        repo_root = Path.cwd()
    git_command = [
        'git',
        '-C',
        str(repo_root),
        'log',
        '-1',
        '--pretty=format:%h',
    ]

    process = subprocess.run(
        git_command,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )
    gitsha = process.stdout.decode()
    if process.returncode != 0 or not gitsha:
        error_output = f'\n"{git_command}" failed with:' f'\n{gitsha}'
        if process.stderr:
            error_output += f'\n{process.stderr.decode()}'
        raise UnknownGitSha(
            'Could not determine the current git SHA.' + error_output
        )
    return gitsha.strip()


def get_current_date() -> str:
    return datetime.now().strftime('%Y%m%d%H%M%S')


class UnexpectedConfigSection(Exception):
    """Exception thrown when the common config contains unexpected values."""


def load_common_config(
    common_config: Optional[Path] = None,
    package_name_override: Optional[str] = None,
    package_version_override: Optional[str] = None,
    append_git_sha: bool = False,
    append_date: bool = False,
    repo_root: Optional[Path] = None,
) -> configparser.ConfigParser:
    """Load an existing ConfigParser file and update metadata.version."""
    config = configparser.ConfigParser()
    if common_config:
        config.read(common_config)

    # Metadata and option sections need to exist.
    if not config.has_section('metadata'):
        config['metadata'] = {}
    if not config.has_section('options'):
        config['options'] = {}

    if package_name_override:
        config['metadata']['name'] = package_name_override
    if package_version_override:
        config['metadata']['version'] = package_version_override

    # Check for existing values that should not be present
    if config.has_option('options', 'packages'):
        value = str(config['options']['packages'])
        raise UnexpectedConfigSection(
            f'[options] packages already defined as: {value}'
        )

    # Append build metadata if applicable.
    build_metadata = []
    if append_date:
        build_metadata.append(get_current_date())
    if append_git_sha:
        build_metadata.append(get_current_git_sha(repo_root))
    if build_metadata:
        version_prefix = config['metadata']['version']
        build_metadata_text = '.'.join(build_metadata)
        config['metadata'][
            'version'
        ] = f'{version_prefix}+{build_metadata_text}'
    return config


def update_config_with_packages(
    config: configparser.ConfigParser,
    python_packages: Iterable[PythonPackage],
) -> None:
    """Merge setup.cfg files from a set of python packages."""
    config['options']['packages'] = 'find:'
    if not config.has_section('options.package_data'):
        config['options.package_data'] = {}
    if not config.has_section('options.entry_points'):
        config['options.entry_points'] = {}

    # Save a list of packages being bundled.
    included_packages = [pkg.package_name for pkg in python_packages]

    for pkg in python_packages:
        # Skip this package if no setup.cfg is defined.
        if not pkg.config:
            continue

        # Collect install_requires
        if pkg.config.has_option('options', 'install_requires'):
            existing_requires = config['options'].get('install_requires', '\n')

            new_requires = existing_requires.splitlines()
            new_requires += pkg.install_requires_entries()
            # Remove requires already included in this merged config.
            new_requires = [
                line
                for line in new_requires
                if line and line not in included_packages
            ]
            # Remove duplictes and sort require list.
            new_requires_text = '\n' + '\n'.join(sorted(set(new_requires)))
            config['options']['install_requires'] = new_requires_text

        # Collect package_data
        if pkg.config.has_section('options.package_data'):
            for key, value in pkg.config['options.package_data'].items():
                existing_values = (
                    config['options.package_data'].get(key, '').splitlines()
                )
                new_value = '\n'.join(
                    sorted(set(existing_values + value.splitlines()))
                )
                # Remove any empty lines
                new_value = new_value.replace('\n\n', '\n')
                config['options.package_data'][key] = new_value

        # Collect entry_points
        if pkg.config.has_section('options.entry_points'):
            for key, value in pkg.config['options.entry_points'].items():
                existing_entry_points = config['options.entry_points'].get(
                    key, ''
                )
                new_entry_points = '\n'.join([existing_entry_points, value])
                # Remove any empty lines
                new_entry_points = new_entry_points.replace('\n\n', '\n')
                config['options.entry_points'][key] = new_entry_points


def write_config(
    final_config: configparser.ConfigParser,
    tree_destination_dir: Path,
    common_config: Optional[Path] = None,
) -> None:
    """Write a the final setup.cfg file with license comment block."""
    comment_block_text = ''
    if common_config:
        # Get the license comment block from the common_config.
        comment_block_match = re.search(
            r'((^#.*?[\r\n])*)([^#])', common_config.read_text(), re.MULTILINE
        )
        if comment_block_match:
            comment_block_text = comment_block_match.group(1)

    setup_cfg_file = tree_destination_dir.resolve() / 'setup.cfg'
    setup_cfg_text = io.StringIO()
    final_config.write(setup_cfg_text)
    setup_cfg_file.write_text(comment_block_text + setup_cfg_text.getvalue())


def setuptools_build_with_base(
    pkg: PythonPackage, build_base: Path, include_tests: bool = False
) -> Path:
    """Run setuptools build for this package."""

    # If there is no setup_dir or setup_sources, just copy this packages
    # source files.
    if not pkg.setup_dir:
        pkg.copy_sources_to(build_base)
        return build_base
    # Create the lib install dir in case it doesn't exist.
    lib_dir_path = build_base / 'lib'
    lib_dir_path.mkdir(parents=True, exist_ok=True)

    starting_directory = Path.cwd()
    # cd to the location of setup.py
    with change_working_dir(pkg.setup_dir):
        # Run build with temp build-base location
        # Note: New files will be placed inside lib_dir_path
        setuptools.setup(
            script_args=[
                'build',
                '--force',
                '--build-base',
                str(build_base),
            ]
        )

        new_pkg_dir = lib_dir_path / pkg.package_name
        # If tests should be included, copy them to the tests dir
        if include_tests and pkg.tests:
            test_dir_path = new_pkg_dir / 'tests'
            test_dir_path.mkdir(parents=True, exist_ok=True)

            for test_source_path in pkg.tests:
                shutil.copy(
                    starting_directory / test_source_path, test_dir_path
                )

    return lib_dir_path


def build_python_tree(
    python_packages: Iterable[PythonPackage],
    tree_destination_dir: Path,
    include_tests: bool = False,
) -> None:
    """Install PythonPackages to a destination directory."""

    # Create the root destination directory.
    destination_path = tree_destination_dir.resolve()
    # Delete any existing files
    shutil.rmtree(destination_path, ignore_errors=True)
    destination_path.mkdir(exist_ok=True)

    for pkg in python_packages:
        # Define a temporary location to run setup.py build in.
        with tempfile.TemporaryDirectory() as build_base_name:
            build_base = Path(build_base_name)

            lib_dir_path = setuptools_build_with_base(
                pkg, build_base, include_tests=include_tests
            )

            # Move installed files from the temp build-base into
            # destination_path.
            shutil.copytree(lib_dir_path, destination_path, dirs_exist_ok=True)

            # Clean build base lib folder for next install
            shutil.rmtree(lib_dir_path, ignore_errors=True)


def copy_extra_files(extra_file_strings: Iterable[str]) -> None:
    """Copy extra files to their destinations."""
    if not extra_file_strings:
        return

    for extra_file_string in extra_file_strings:
        # Convert 'source > destination' strings to Paths.
        input_output = re.split(r' *> *', extra_file_string)
        source_file = Path(input_output[0])
        dest_file = Path(input_output[1])

        if not source_file.exists():
            raise FileNotFoundError(
                f'extra_file "{source_file}" not found.\n'
                f'  Defined by: "{extra_file_string}"'
            )

        # Copy files and make parent directories.
        dest_file.parent.mkdir(parents=True, exist_ok=True)
        # Raise an error if the destination file already exists.
        if dest_file.exists():
            raise FileExistsError(
                f'Copying "{source_file}" would overwrite "{dest_file}"'
            )

        shutil.copy(source_file, dest_file)


def _main():
    args = _parse_args()

    # Check the common_config file exists if provided.
    if args.setupcfg_common_file:
        assert args.setupcfg_common_file.is_file()

    py_packages = load_packages(args.input_list_files)

    build_python_tree(
        python_packages=py_packages,
        tree_destination_dir=args.tree_destination_dir,
        include_tests=args.include_tests,
    )
    copy_extra_files(args.extra_files)

    if args.create_default_pyproject_toml:
        pyproject_path = args.tree_destination_dir / 'pyproject.toml'
        pyproject_path.write_text(PYPROJECT_FILE)

    if args.setupcfg_common_file or (
        args.setupcfg_override_name and args.setupcfg_override_version
    ):

        config = load_common_config(
            common_config=args.setupcfg_common_file,
            package_name_override=args.setupcfg_override_name,
            package_version_override=args.setupcfg_override_version,
            append_git_sha=args.setupcfg_version_append_git_sha,
            append_date=args.setupcfg_version_append_date,
            repo_root=args.repo_root,
        )

        update_config_with_packages(config=config, python_packages=py_packages)

        write_config(
            common_config=args.setupcfg_common_file,
            final_config=config,
            tree_destination_dir=args.tree_destination_dir,
        )


if __name__ == '__main__':
    _main()
