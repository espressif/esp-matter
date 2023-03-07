#!/usr/bin/env python

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
"""Save list of installed packages and versions."""

import argparse
import itertools
import sys
from pathlib import Path
from typing import Dict, Iterator, List, Optional, Union

import pkg_resources


def _installed_packages() -> Iterator[str]:
    """Run pip python_packages and write to out."""
    installed_packages = list(
        pkg.as_requirement()
        for pkg in pkg_resources.working_set  # pylint: disable=not-an-iterable
        # Non-editable packages only
        if isinstance(pkg, pkg_resources.DistInfoDistribution)  # type: ignore
        # This will skip packages with local versions.
        #   For example text after a plus sign: 1.2.3+dev456
        and not pkg.parsed_version.local
        # These are always installed by default in:
        #   pw_env_setup/py/pw_env_setup/virtualenv_setup/install.py
        and pkg.key not in ['pip', 'setuptools', 'wheel']
    )
    for req in sorted(
        installed_packages, key=lambda pkg: pkg.name.lower()  # type: ignore
    ):
        yield str(req)


def ls(output_file: Optional[Path]) -> int:  # pylint: disable=invalid-name
    """Run pip python_packages and write to output_file."""
    actual_requirements = frozenset(
        pkg_resources.Requirement.parse(line) for line in _installed_packages()
    )
    missing_requirements = set()

    # If updating an existing file, load the existing requirements to find lines
    # that are missing in the active environment.
    if output_file:
        existing_lines = output_file.read_text().splitlines()
        expected_requirements = set(
            pkg_resources.Requirement.parse(line) for line in existing_lines
        )
        missing_requirements = expected_requirements - actual_requirements

    new_requirements: List[pkg_resources.Requirement] = list(
        actual_requirements
    )

    for requirement in missing_requirements:
        # Preserve this requirement if it has a marker that doesn't apply to
        # the current environment. For example a line with ;python_version <
        # "3.9" if running on Python 3.9 or higher will be saved.
        if requirement.marker and not requirement.marker.evaluate():
            new_requirements.append(requirement)
            continue

        # If this package is in the active environment then it has a marker
        # in the existing file that should be preserved.
        try:
            found_package = pkg_resources.working_set.find(requirement)
            # If the package version doesn't match, save the new version.
        except pkg_resources.VersionConflict:
            found_package = None
        if found_package:
            # Delete the old line with no marker.
            new_requirements.remove(found_package.as_requirement())
            # Add the existing requirement line that includes the marker.
            new_requirements.append(requirement)

    out = output_file.open('w') if output_file else sys.stdout

    for package in sorted(
        new_requirements, key=lambda pkg: pkg.name.lower()  # type: ignore
    ):
        print(package, file=out)

    if output_file:
        out.close()
    return 0


class UpdateRequiredError(Exception):
    pass


def _stderr(*args, **kwargs):
    return print(*args, file=sys.stderr, **kwargs)


def _load_requirements_lines(*req_files: Path) -> Iterator[str]:
    for req_file in req_files:
        for line in req_file.read_text().splitlines():
            # Ignore comments and blank lines
            if line.startswith('#') or line == '':
                continue
            yield line


def diff(
    expected: Path, ignore_requirements_file: Optional[List[Path]] = None
) -> int:
    """Report on differences between installed and expected versions."""
    actual_lines = set(_installed_packages())
    expected_lines = set(_load_requirements_lines(expected))
    ignored_lines = set()
    if ignore_requirements_file:
        ignored_lines = set(_load_requirements_lines(*ignore_requirements_file))

    if actual_lines == expected_lines:
        _stderr('package versions are identical')
        return 0

    actual_requirements = frozenset(
        pkg_resources.Requirement.parse(line) for line in actual_lines
    )
    expected_requirements = frozenset(
        pkg_resources.Requirement.parse(line) for line in expected_lines
    )
    ignored_requirements = frozenset(
        pkg_resources.Requirement.parse(line) for line in ignored_lines
    )

    removed_requirements = expected_requirements - actual_requirements
    added_requirements = actual_requirements - expected_requirements

    removed_packages: Dict[pkg_resources.Requirement, str] = {}
    updated_packages: Dict[pkg_resources.Requirement, str] = {}
    new_packages: Dict[pkg_resources.Requirement, str] = {}
    reformatted_packages: Dict[pkg_resources.Requirement, str] = {}

    for line in expected_lines:
        requirement = pkg_resources.Requirement.parse(line)

        # Check for lines that need reformatting
        # This will catch lines that use underscores instead of dashes in the
        # name of missing spaces after specifiers.

        # Match this requirement with the original one found in
        # actual_requirements.
        #
        # Note the requirement variable may equal its counterpart in
        # actual_requirements due to the .hashCpm on the Requirement class
        # checking their normalized name. Since we are looking for formatting
        # mismatches here we need to retrieve the requirement instance from the
        # actual_requirements set.
        matching_found_requirement = {
            req: req for req in actual_requirements
        }.get(requirement)

        # If the actual requirement line doesn't match.
        if (
            matching_found_requirement
            and str(matching_found_requirement) != line
        ):
            reformatted_packages[matching_found_requirement] = line

        # If this requirement isn't in the active enviroment and the line
        # doesn't match the repr: flag for reformatting.
        if not matching_found_requirement and str(requirement) != line:
            reformatted_packages[requirement] = line

        # If a requirement specifier is used and it doesn't apply, skip this
        # line. See details for requirement specifiers at:
        # https://pip.pypa.io/en/stable/reference/requirement-specifiers/#requirement-specifiers
        if requirement.marker and not requirement.marker.evaluate():
            continue

        # Try to find this requirement in the current environment.
        try:
            found_package = pkg_resources.working_set.find(requirement)
        # If the package version doesn't match, save the new version.
        except pkg_resources.VersionConflict as err:
            found_package = None
            if err.dist:
                found_package = err.dist

        # If this requirement isn't in the environment, it was removed.
        if not found_package:
            removed_packages[requirement] = line
            continue

        # If found_package is set, the version doesn't match so it was updated.
        if requirement.specs != found_package.as_requirement().specs:
            updated_packages[found_package.as_requirement()] = line

    ignored_distributions = list(
        distribution
        for distribution in pkg_resources.working_set  # pylint: disable=not-an-iterable
        if distribution.as_requirement() in ignored_requirements
    )
    expected_distributions = list(
        distribution
        for distribution in pkg_resources.working_set  # pylint: disable=not-an-iterable
        if distribution.as_requirement() in expected_requirements
    )

    def get_requirements(
        dist_info: pkg_resources.Distribution,
    ) -> Iterator[pkg_resources.Distribution]:
        """Return requirement that are not in expected_distributions."""
        for req in dist_info.requires():
            req_dist_info = pkg_resources.working_set.find(req)
            if not req_dist_info:
                continue
            if req_dist_info in expected_distributions:
                continue
            yield req_dist_info

    def expand_requirements(
        reqs: List[pkg_resources.Distribution],
    ) -> Iterator[List[pkg_resources.Distribution]]:
        """Recursively expand requirements."""
        for dist_info in reqs:
            deps = list(get_requirements(dist_info))
            if deps:
                yield deps
            yield from expand_requirements(deps)

    ignored_transitive_deps = set(
        itertools.chain.from_iterable(
            expand_requirements(ignored_distributions)
        )
    )

    # Check for new packages
    for requirement in added_requirements - removed_requirements:
        if requirement in updated_packages:
            continue
        if requirement in ignored_requirements or ignored_transitive_deps:
            continue

        new_packages[requirement] = str(requirement)

    # Print status messages to stderr

    if reformatted_packages:
        _stderr('Requirements that need reformatting:')
        for requirement, line in reformatted_packages.items():
            _stderr(f'  {line}')
            _stderr('  should be:')
            _stderr(f'  {str(requirement)}')

    if updated_packages:
        _stderr('Updated packages')
        for requirement, line in updated_packages.items():
            _stderr(f'  {str(requirement)} (from {line})')

    if removed_packages:
        _stderr('Removed packages')
        for requirement in removed_packages:
            _stderr(f'  {requirement}')

    if new_packages:
        _stderr('New packages')
        for requirement in new_packages:
            _stderr(f'  {requirement}')

    if updated_packages or new_packages:
        _stderr("Package versions don't match!")
        _stderr(
            f"""
Please do the following:

* purge your environment directory
  * Linux/Mac: 'rm -rf "$_PW_ACTUAL_ENVIRONMENT_ROOT"'
  * Windows: 'rmdir /S %_PW_ACTUAL_ENVIRONMENT_ROOT%'
* bootstrap
  * Linux/Mac: '. ./bootstrap.sh'
  * Windows: 'bootstrap.bat'
* update the constraint file
  * 'pw python-packages list {expected.name}'
"""
        )
        return -1

    return 0


def parse(argv: Union[List[str], None] = None) -> argparse.Namespace:
    """Parse command-line arguments."""
    parser = argparse.ArgumentParser(
        prog="python -m pw_env_setup.python_packages"
    )
    subparsers = parser.add_subparsers(dest='cmd')

    list_parser = subparsers.add_parser(
        'list', aliases=('ls',), help='List installed package versions.'
    )
    list_parser.add_argument('output_file', type=Path, nargs='?')

    diff_parser = subparsers.add_parser(
        'diff',
        help='Show differences between expected and actual package versions.',
    )
    diff_parser.add_argument('expected', type=Path)
    diff_parser.add_argument(
        '--ignore-requirements-file', type=Path, action='append'
    )

    return parser.parse_args(argv)


def main() -> int:
    args = vars(parse())
    cmd = args.pop('cmd')
    if cmd == 'diff':
        return diff(**args)
    if cmd == 'list':
        return ls(**args)
    return -1


if __name__ == '__main__':
    sys.exit(main())
