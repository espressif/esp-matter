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
"""Preconfigured checks for Python code.

These checks assume that they are running in a preconfigured Python environment.
"""

import json
import logging
import os
from pathlib import Path
import subprocess
import sys
from typing import Optional

try:
    import pw_presubmit
except ImportError:
    # Append the pw_presubmit package path to the module search path to allow
    # running this module without installing the pw_presubmit package.
    sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    import pw_presubmit

from pw_env_setup import python_packages
from pw_presubmit import (
    build,
    call,
    Check,
    filter_paths,
    PresubmitContext,
    PresubmitFailure,
)

_LOG = logging.getLogger(__name__)

_PYTHON_EXTENSIONS = ('.py', '.gn', '.gni')

_PYTHON_IS_3_9_OR_HIGHER = sys.version_info >= (
    3,
    9,
)


@filter_paths(endswith=_PYTHON_EXTENSIONS)
def gn_python_check(ctx: PresubmitContext):
    build.gn_gen(ctx)
    build.ninja(ctx, 'python.tests', 'python.lint')


def _transform_lcov_file_paths(lcov_file: Path, repo_root: Path) -> str:
    """Modify file paths in an lcov file to be relative to the repo root.

    See `man geninfo` for info on the lcov format."""

    lcov_input = lcov_file.read_text()
    lcov_output = ''

    if not _PYTHON_IS_3_9_OR_HIGHER:
        return lcov_input

    for line in lcov_input.splitlines():
        if not line.startswith('SF:'):
            lcov_output += line + '\n'
            continue

        # Get the file path after SF:
        file_string = line[3:].rstrip()
        source_file_path = Path(file_string)

        # TODO(b/248257406) Remove once we drop support for Python 3.8.
        def is_relative_to(path: Path, other: Path) -> bool:
            try:
                path.relative_to(other)
                return True
            except ValueError:
                return False

        # Attempt to map a generated Python package source file to the root
        # source tree.
        # pylint: disable=no-member
        if not is_relative_to(
            source_file_path, repo_root  # type: ignore[attr-defined]
        ):
            # pylint: enable=no-member
            source_file_path = repo_root / str(source_file_path).replace(
                'python/gen/', ''
            ).replace('py.generated_python_package/', '')

        # If mapping fails don't modify this line.
        # pylint: disable=no-member
        if not is_relative_to(
            source_file_path, repo_root  # type: ignore[attr-defined]
        ):
            # pylint: enable=no-member
            lcov_output += line + '\n'
            continue

        source_file_path = source_file_path.relative_to(repo_root)
        lcov_output += f'SF:{source_file_path}\n'

    return lcov_output


@filter_paths(endswith=_PYTHON_EXTENSIONS)
def gn_python_test_coverage(ctx: PresubmitContext):
    """Run Python tests with coverage and create reports."""
    build.gn_gen(ctx, pw_build_PYTHON_TEST_COVERAGE=True)
    build.ninja(ctx, 'python.tests')

    # Find coverage data files
    coverage_data_files = list(ctx.output_dir.glob('**/*.coverage'))
    if not coverage_data_files:
        return

    # Merge coverage data files to out/.coverage
    call(
        'coverage',
        'combine',
        # Leave existing coverage files in place; by default they are deleted.
        '--keep',
        *coverage_data_files,
        cwd=ctx.output_dir,
    )
    combined_data_file = ctx.output_dir / '.coverage'
    _LOG.info('Coverage data saved to: %s', combined_data_file.resolve())

    # Always ignore generated proto python and setup.py files.
    coverage_omit_patterns = '--omit=*_pb2.py,*/setup.py'

    # Output coverage percentage summary to the terminal of changed files.
    changed_python_files = list(
        str(p) for p in ctx.paths if str(p).endswith('.py')
    )
    report_args = [
        'coverage',
        'report',
        '--ignore-errors',
        coverage_omit_patterns,
    ]
    report_args += changed_python_files
    subprocess.run(report_args, check=False, cwd=ctx.output_dir)

    # Generate a json report
    call('coverage', 'lcov', coverage_omit_patterns, cwd=ctx.output_dir)
    lcov_data_file = ctx.output_dir / 'coverage.lcov'
    lcov_data_file.write_text(
        _transform_lcov_file_paths(lcov_data_file, repo_root=ctx.root)
    )
    _LOG.info('Coverage lcov saved to: %s', lcov_data_file.resolve())

    # Generate an html report
    call('coverage', 'html', coverage_omit_patterns, cwd=ctx.output_dir)
    html_report = ctx.output_dir / 'htmlcov' / 'index.html'
    _LOG.info('Coverage html report saved to: %s', html_report.resolve())


@filter_paths(endswith=_PYTHON_EXTENSIONS + ('.pylintrc',))
def gn_python_lint(ctx: pw_presubmit.PresubmitContext) -> None:
    build.gn_gen(ctx)
    build.ninja(ctx, 'python.lint')


@Check
def check_python_versions(ctx: PresubmitContext):
    """Checks that the list of installed packages is as expected."""

    build.gn_gen(ctx)
    constraint_file: Optional[str] = None
    requirement_file: Optional[str] = None
    try:
        for arg in build.get_gn_args(ctx.output_dir):
            if arg['name'] == 'pw_build_PIP_CONSTRAINTS':
                constraint_file = json.loads(arg['current']['value'])[0].strip(
                    '/'
                )
            if arg['name'] == 'pw_build_PIP_REQUIREMENTS':
                requirement_file = json.loads(arg['current']['value'])[0].strip(
                    '/'
                )
    except json.JSONDecodeError:
        _LOG.warning('failed to parse GN args json')
        return

    if not constraint_file:
        _LOG.warning('could not find pw_build_PIP_CONSTRAINTS GN arg')
        return
    ignored_requirements_arg = None
    if requirement_file:
        ignored_requirements_arg = [(ctx.root / requirement_file)]

    if (
        python_packages.diff(
            expected=(ctx.root / constraint_file),
            ignore_requirements_file=ignored_requirements_arg,
        )
        != 0
    ):
        raise PresubmitFailure
