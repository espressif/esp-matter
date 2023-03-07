#!/usr/bin/env python3
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
"""Invoke clang-tidy.

Implements additional features compared to directly calling
clang-tidy:
  - add option `--source-exclude` to exclude matching sources from the
    clang-tidy analysis.
  - inputs the full compile command, with the cc binary name
  - TODO(henrichataing): infer platform options from the full compile command
"""

import argparse
import logging
from pathlib import Path
import re
import shlex
import subprocess
import sys
from typing import Iterable, List, Optional, Union

_LOG = logging.getLogger(__name__)


def _parse_args() -> argparse.Namespace:
    """Parses arguments for this script, splitting out the command to run."""

    parser = argparse.ArgumentParser()
    parser.add_argument(
        '-v',
        '--verbose',
        action='store_true',
        help='Run clang_tidy with extra debug output.',
    )

    parser.add_argument(
        '--clang-tidy',
        default='clang-tidy',
        help='Path to clang-tidy executable.',
    )

    parser.add_argument(
        '--source-file',
        required=True,
        type=Path,
        help='Path to the source file to analyze with clang-tidy.',
    )
    parser.add_argument(
        '--source-root',
        required=True,
        type=Path,
        help=(
            'Path to the root source directory.'
            ' The relative path from the root directory is matched'
            ' against source filter rather than the absolute path.'
        ),
    )
    parser.add_argument(
        '--export-fixes',
        required=False,
        type=Path,
        help=(
            'YAML file to store suggested fixes in. The '
            'stored fixes can be applied to the input source '
            'code with clang-apply-replacements.'
        ),
    )

    parser.add_argument(
        '--source-exclude',
        default=[],
        action='append',
        type=str,
        help=(
            'Regular expressions matching the paths of'
            ' source files to be excluded from the'
            ' analysis.'
        ),
    )

    parser.add_argument(
        '--skip-include-path',
        default=[],
        action='append',
        type=str,
        help=(
            'Exclude include paths ending in these paths from clang-tidy. '
            'These paths are switched from -I to -isystem so clang-tidy '
            'ignores them.'
        ),
    )

    # Add a silent placeholder arg for everything that was left over.
    parser.add_argument(
        'extra_args', nargs=argparse.REMAINDER, help=argparse.SUPPRESS
    )

    parsed_args = parser.parse_args()

    if parsed_args.extra_args[0] != '--':
        parser.error('arguments not correctly split')
    parsed_args.extra_args = parsed_args.extra_args[1:]
    return parsed_args


def _filter_include_paths(
    args: Iterable[str], skip_include_paths: Iterable[str]
) -> Iterable[str]:
    filters = [f.rstrip('/') for f in skip_include_paths]

    for arg in args:
        if arg.startswith('-I'):
            path = Path(arg[2:]).as_posix()
            if any(path.endswith(f) or re.match(f, str(path)) for f in filters):
                yield '-isystem' + arg[2:]
                continue
        if arg.startswith('--sysroot'):
            path = Path(arg[9:]).as_posix()
            if any(path.endswith(f) or re.match(f, str(path)) for f in filters):
                yield '-isysroot' + arg[9:]
                continue

        yield arg


def run_clang_tidy(
    clang_tidy: str,
    verbose: bool,
    source_file: Path,
    export_fixes: Optional[Path],
    skip_include_path: List[str],
    extra_args: List[str],
) -> int:
    """Executes clang_tidy via subprocess. Returns true if no failures."""
    command: List[Union[str, Path]] = [clang_tidy, source_file, '--use-color']

    if not verbose:
        command.append('--quiet')

    if export_fixes is not None:
        command.extend(['--export-fixes', export_fixes])

    # Append extra compilation flags.  Extra args up to
    # END_OF_INVOKER are skipped.
    command.append('--')
    end_of_invoker = extra_args.index('END_OF_INVOKER')
    command.extend(
        _filter_include_paths(
            extra_args[end_of_invoker + 1 :], skip_include_path
        )
    )

    process = subprocess.run(
        command,
        stdout=subprocess.PIPE,
        # clang-tidy prints regular information on
        # stderr, even with the option --quiet.
        stderr=subprocess.PIPE,
    )
    if process.returncode != 0:
        _LOG.warning('%s', ' '.join(shlex.quote(str(arg)) for arg in command))

    if process.stdout:
        _LOG.warning(process.stdout.decode().strip())

    if process.stderr and process.returncode != 0:
        _LOG.error(process.stderr.decode().strip())

    return process.returncode


def main(
    verbose: bool,
    clang_tidy: str,
    source_file: Path,
    source_root: Path,
    export_fixes: Optional[Path],
    source_exclude: List[str],
    skip_include_path: List[str],
    extra_args: List[str],
) -> int:
    # Rebase the source file path on source_root.
    # If source_file is not relative to source_root (which may be the case for
    # generated files) stick with the original source_file.
    try:
        relative_source_file = source_file.relative_to(source_root)
    except ValueError:
        relative_source_file = source_file

    for pattern in source_exclude:
        if re.match(pattern, str(relative_source_file)):
            return 0

    source_file_path = source_file.resolve()
    export_fixes_path = (
        export_fixes.resolve() if export_fixes is not None else None
    )
    return run_clang_tidy(
        clang_tidy,
        verbose,
        source_file_path,
        export_fixes_path,
        skip_include_path,
        extra_args,
    )


if __name__ == '__main__':
    sys.exit(main(**vars(_parse_args())))
