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
"""Argument parsing code for presubmit checks."""

import argparse
import logging
import os
from pathlib import Path
import re
import shutil
import textwrap
from typing import Callable, Collection, List, Optional, Sequence

from pw_presubmit import git_repo, presubmit

_LOG = logging.getLogger(__name__)
DEFAULT_PATH = Path('out', 'presubmit')

_OUTPUT_PATH_README = '''\
This directory was created by pw_presubmit to run presubmit checks for the
{repo} repository. This directory is not used by the regular GN or CMake Ninja
builds. It may be deleted safely.
'''


def add_path_arguments(parser) -> None:
    """Adds common presubmit check options to an argument parser."""

    parser.add_argument(
        'paths',
        metavar='pathspec',
        nargs='*',
        help=(
            'Paths or patterns to which to restrict the checks. These are '
            'interpreted as Git pathspecs. If --base is provided, only '
            'paths changed since that commit are checked.'
        ),
    )

    base = parser.add_mutually_exclusive_group()
    base.add_argument(
        '-b',
        '--base',
        metavar='commit',
        default=git_repo.TRACKING_BRANCH_ALIAS,
        help=(
            'Git revision against which to diff for changed files. '
            'Default is the tracking branch of the current branch.'
        ),
    )

    base.add_argument(
        '--all',
        '--full',
        dest='base',
        action='store_const',
        const=None,
        help='Run actions for all files, not just changed files.',
    )

    parser.add_argument(
        '-e',
        '--exclude',
        metavar='regular_expression',
        default=[],
        action='append',
        type=re.compile,
        help=(
            'Exclude paths matching any of these regular expressions, '
            "which are interpreted relative to each Git repository's root."
        ),
    )


def _add_programs_arguments(
    parser: argparse.ArgumentParser, programs: presubmit.Programs, default: str
):
    def presubmit_program(arg: str) -> presubmit.Program:
        if arg not in programs:
            all_program_names = ', '.join(sorted(programs.keys()))
            raise argparse.ArgumentTypeError(
                f'{arg} is not the name of a presubmit program\n\n'
                f'Valid Programs:\n{all_program_names}'
            )

        return programs[arg]

    # This argument is used to copy the default program into the argparse
    # namespace argument. It's not intended to be set by users.
    parser.add_argument(
        '--default-program',
        default=[presubmit_program(default)],
        help=argparse.SUPPRESS,
    )

    parser.add_argument(
        '-p',
        '--program',
        choices=programs.values(),
        type=presubmit_program,
        action='append',
        default=[],
        help='Which presubmit program to run',
    )

    parser.add_argument(
        '--list-steps-file',
        dest='list_steps_file',
        type=Path,
        help=argparse.SUPPRESS,
    )

    all_steps = programs.all_steps()

    def list_steps() -> None:
        """List all available presubmit steps and their docstrings."""
        for step in sorted(all_steps.values(), key=str):
            _LOG.info('%s', step)
            if step.doc:
                first, *rest = step.doc.split('\n', 1)
                _LOG.info('  %s', first)
                if rest and _LOG.isEnabledFor(logging.DEBUG):
                    for line in textwrap.dedent(*rest).splitlines():
                        _LOG.debug('  %s', line)

    parser.add_argument(
        '--list-steps',
        action='store_const',
        const=list_steps,
        default=None,
        help='List all the available steps.',
    )

    def presubmit_step(arg: str) -> presubmit.Check:
        if arg not in all_steps:
            all_step_names = ', '.join(sorted(all_steps.keys()))
            raise argparse.ArgumentTypeError(
                f'{arg} is not the name of a presubmit step\n\n'
                f'Valid Steps:\n{all_step_names}'
            )
        return all_steps[arg]

    parser.add_argument(
        '--step',
        action='append',
        choices=all_steps.values(),
        default=[],
        help='Run specific steps instead of running a full program.',
        type=presubmit_step,
    )

    def gn_arg(argument):
        key, value = argument.split('=', 1)
        return (key, value)

    # Recipe code for handling builds with pre-release toolchains requires the
    # ability to pass through GN args. This ability is not expected to be used
    # directly outside of this case, so the option is hidden. Values passed in
    # to this argument should be of the form 'key=value'.
    parser.add_argument(
        '--override-gn-arg',
        dest='override_gn_args',
        action='append',
        type=gn_arg,
        help=argparse.SUPPRESS,
    )


def add_arguments(
    parser: argparse.ArgumentParser,
    programs: Optional[presubmit.Programs] = None,
    default: str = '',
) -> None:
    """Adds common presubmit check options to an argument parser."""

    add_path_arguments(parser)
    parser.add_argument(
        '-k',
        '--keep-going',
        action='store_true',
        help='Continue running presubmit steps after a failure.',
    )
    parser.add_argument(
        '--continue-after-build-error',
        action='store_true',
        help=(
            'Within presubmit steps, continue running build steps after a '
            'failure.'
        ),
    )
    parser.add_argument(
        '--output-directory',
        type=Path,
        help=f'Output directory (default: {"<repo root>" / DEFAULT_PATH})',
    )
    parser.add_argument(
        '--package-root',
        type=Path,
        help='Package root directory (default: <env directory>/packages)',
    )

    exclusive = parser.add_mutually_exclusive_group()
    exclusive.add_argument(
        '--clear',
        '--clean',
        action='store_true',
        help='Delete the presubmit output directory and exit.',
    )

    if programs:
        if not default:
            raise ValueError('A default must be provided with programs')

        _add_programs_arguments(parser, programs, default)

        # LUCI builders extract the list of steps from the program and run them
        # individually for a better UX in MILO.
        parser.add_argument(
            '--only-list-steps',
            action='store_true',
            help=argparse.SUPPRESS,
        )


def run(
    default_program: Optional[presubmit.Program],
    program: Sequence[presubmit.Program],
    step: Sequence[presubmit.Check],
    output_directory: Optional[Path],
    package_root: Path,
    clear: bool,
    root: Optional[Path] = None,
    repositories: Collection[Path] = (),
    only_list_steps=False,
    list_steps: Optional[Callable[[], None]] = None,
    **other_args,
) -> int:
    """Processes arguments from add_arguments and runs the presubmit.

    Args:
      default_program: program to use if neither --program nor --step is used
      program: from the --program option
      step: from the --step option
      output_directory: from --output-directory option
      package_root: from --package-root option
      clear: from the --clear option
      root: base path from which to run presubmit checks; defaults to the root
          of the current directory's repository
      repositories: roots of Git repositories on which to run presubmit checks;
          defaults to the root of the current directory's repository
      only_list_steps: list the steps that would be executed, one per line,
          instead of executing them
      list_steps: list the steps that would be executed with their docstrings
      **other_args: remaining arguments defined by by add_arguments

    Returns:
      exit code for sys.exit; 0 if successful, 1 if an error occurred
    """
    if root is None:
        root = git_repo.root()

    if not repositories:
        repositories = [root]

    if output_directory is None:
        output_directory = root / DEFAULT_PATH

    output_directory.mkdir(parents=True, exist_ok=True)
    output_directory.joinpath('README.txt').write_text(
        _OUTPUT_PATH_README.format(repo=root)
    )

    if not package_root:
        package_root = Path(os.environ['PW_PACKAGE_ROOT'])

    _LOG.debug('Using environment at %s', output_directory)

    if clear:
        _LOG.info('Clearing presubmit output directory')

        if output_directory.exists():
            shutil.rmtree(output_directory)
            _LOG.info('Deleted %s', output_directory)

        return 0

    if list_steps:
        list_steps()
        return 0

    final_program: Optional[presubmit.Program] = None
    if not program and not step:
        assert default_program  # Cast away Optional[].
        final_program = default_program
    elif len(program) == 1 and not step:
        final_program = program[0]
    else:
        steps: List[presubmit.Check] = []
        for prog in program:
            steps.extend(prog)
        steps.extend(step)
        final_program = presubmit.Program('', steps)

    if presubmit.run(
        final_program,
        root,
        repositories,
        only_list_steps=only_list_steps,
        output_directory=output_directory,
        package_root=package_root,
        **other_args,
    ):
        return 0

    return 1
