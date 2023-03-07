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
"""Check Python package install_requires are covered."""

import argparse
import configparser
from pathlib import Path
import sys

try:
    from pw_build.python_package import load_packages
    from pw_build.create_python_tree import update_config_with_packages
except ImportError:
    # Load from python_package from this directory if pw_build is not available.
    from python_package import load_packages  # type: ignore
    from create_python_tree import update_config_with_packages  # type: ignore


def _parse_args() -> argparse.Namespace:
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
        '--requirement',
        type=Path,
        required=True,
        help='requirement file to generate',
    )
    parser.add_argument(
        '--gn-packages',
        required=True,
        help=(
            'Comma separated list of GN python package '
            'targets to check for requirements.'
        ),
    )
    parser.add_argument(
        '--exclude-transitive-deps',
        action='store_true',
        help='Exclude checking transitive deps of the specified --gn-packages',
    )
    return parser.parse_args()


class NoMatchingGnPythonDependency(Exception):
    """An error occurred while processing a Python dependency."""


def main(
    python_dep_list_files: Path,
    requirement: Path,
    gn_packages: str,
    exclude_transitive_deps: bool,
) -> int:
    """Check Python package setup.cfg correctness."""

    # Split the comma separated string and remove leading slashes.
    gn_target_names = [
        target.lstrip('/')
        for target in gn_packages.split(',')
        if target  # The last target may be an empty string.
    ]
    for i, gn_target in enumerate(gn_target_names):
        # Remove metadata subtarget if present.
        python_package_target = gn_target.replace('._package_metadata(', '(', 1)
        # Split on the first paren to ignore the toolchain.
        gn_target_names[i] = python_package_target.split('(')[0]

    py_packages = load_packages([python_dep_list_files], ignore_missing=False)

    target_py_packages = py_packages
    if exclude_transitive_deps:
        target_py_packages = []
        for pkg in py_packages:
            valid_target = [
                target in pkg.gn_target_name for target in gn_target_names
            ]
            if not any(valid_target):
                continue
            target_py_packages.append(pkg)

    if not target_py_packages:
        gn_targets_to_include = '\n'.join(gn_target_names)
        declared_py_deps = '\n'.join(pkg.gn_target_name for pkg in py_packages)
        raise NoMatchingGnPythonDependency(
            'No matching GN Python dependency found.\n'
            'GN Targets to include:\n'
            f'{gn_targets_to_include}\n\n'
            'Declared Python Dependencies:\n'
            f'{declared_py_deps}\n\n'
        )

    config = configparser.ConfigParser()
    config['options'] = {}
    update_config_with_packages(
        config=config, python_packages=target_py_packages
    )

    output = (
        '# Auto-generated requirements.txt from the following packages:\n#\n'
    )
    output += '\n'.join(
        '# ' + pkg.gn_target_name
        for pkg in sorted(
            target_py_packages, key=lambda pkg: pkg.gn_target_name
        )
    )

    output += config['options']['install_requires']
    output += '\n'
    requirement.write_text(output)

    return 0


if __name__ == '__main__':
    sys.exit(main(**vars(_parse_args())))
