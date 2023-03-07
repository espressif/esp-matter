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
"""Pip install Pigweed Python packages."""

import argparse
from pathlib import Path
import subprocess
import sys
from typing import List, Tuple

try:
    from pw_build.python_package import load_packages
except ImportError:
    # Load from python_package from this directory if pw_build is not available.
    from python_package import load_packages  # type: ignore


def _parse_args() -> Tuple[argparse.Namespace, List[str]]:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        '--python-dep-list-files',
        type=Path,
        required=True,
        help=(
            'Path to a text file containing the list of Python package '
            'metadata json files.'
        ),
    )
    parser.add_argument(
        '--gn-packages',
        required=True,
        help=(
            'Comma separated list of GN python package ' 'targets to install.'
        ),
    )
    parser.add_argument(
        '--editable-pip-install',
        action='store_true',
        help=(
            'If true run the pip install command with the '
            '\'--editable\' option.'
        ),
    )
    return parser.parse_known_args()


class NoMatchingGnPythonDependency(Exception):
    """An error occurred while processing a Python dependency."""


def main(
    python_dep_list_files: Path,
    editable_pip_install: bool,
    gn_targets: List[str],
    pip_args: List[str],
) -> int:
    """Find matching python packages to pip install."""
    pip_target_dirs: List[str] = []

    py_packages = load_packages([python_dep_list_files], ignore_missing=True)
    for pkg in py_packages:
        valid_target = [target in pkg.gn_target_name for target in gn_targets]
        if not any(valid_target):
            continue
        top_level_source_dir = pkg.package_dir
        pip_target_dirs.append(str(top_level_source_dir.parent.resolve()))

    if not pip_target_dirs:
        raise NoMatchingGnPythonDependency(
            'No matching GN Python dependency found to install.\n'
            'GN Targets to pip install:\n' + '\n'.join(gn_targets) + '\n\n'
            'Declared Python Dependencies:\n'
            + '\n'.join(pkg.gn_target_name for pkg in py_packages)
            + '\n\n'
        )

    for target in pip_target_dirs:
        command_args = [sys.executable, "-m", "pip"]
        command_args += pip_args
        if editable_pip_install:
            command_args.append('--editable')
        command_args.append(target)

        process = subprocess.run(
            command_args, stdout=subprocess.PIPE, stderr=subprocess.STDOUT
        )
        pip_output = process.stdout.decode()
        if process.returncode != 0:
            print(pip_output)
            return process.returncode
    return 0


if __name__ == '__main__':
    # Parse this script's args and pass any remaining args to pip.
    argparse_args, remaining_args_for_pip = _parse_args()

    # Split the comma separated string and remove leading slashes.
    gn_target_names = [
        target.lstrip('/')
        for target in argparse_args.gn_packages.split(',')
        if target  # The last target may be an empty string.
    ]

    result = main(
        python_dep_list_files=argparse_args.python_dep_list_files,
        editable_pip_install=argparse_args.editable_pip_install,
        gn_targets=gn_target_names,
        pip_args=remaining_args_for_pip,
    )
    sys.exit(result)
