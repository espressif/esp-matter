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
"""Build a Pigweed Project.

Usage examples:

  # Build pw_run_tests.modules in the out/cmake directory
  pw build -C out/cmake pw_run_tests.modules

  # Build the default target in out/ and pw_apps in out/cmake
  pw build -C out -C out/cmake pw_apps

  # Find a directory and build python.tests, and build pw_apps in out/cmake
  pw build python.tests -C out/cmake pw_apps
"""

import argparse
import os
import logging
import shlex
import sys
import subprocess
from typing import (
    Callable,
    Dict,
    Generator,
    List,
    NoReturn,
    Optional,
    Sequence,
    NamedTuple,
)

import pw_cli.log
import pw_cli.env
from pw_build.build_recipe import BuildRecipe, create_build_recipes
from pw_build.project_builder_prefs import ProjectBuilderPrefs

from pw_build.project_builder_argparse import add_project_builder_arguments

_COLOR = pw_cli.color.colors()
_LOG = logging.getLogger('pw_build')

PASS_MESSAGE = """
  ██████╗  █████╗ ███████╗███████╗██╗
  ██╔══██╗██╔══██╗██╔════╝██╔════╝██║
  ██████╔╝███████║███████╗███████╗██║
  ██╔═══╝ ██╔══██║╚════██║╚════██║╚═╝
  ██║     ██║  ██║███████║███████║██╗
  ╚═╝     ╚═╝  ╚═╝╚══════╝╚══════╝╚═╝
"""

# Pick a visually-distinct font from "PASS" to ensure that readers can't
# possibly mistake the difference between the two states.
FAIL_MESSAGE = """
   ▄██████▒░▄▄▄       ██▓  ░██▓
  ▓█▓     ░▒████▄    ▓██▒  ░▓██▒
  ▒████▒   ░▒█▀  ▀█▄  ▒██▒ ▒██░
  ░▓█▒    ░░██▄▄▄▄██ ░██░  ▒██░
  ░▒█░      ▓█   ▓██▒░██░░ ████████▒
   ▒█░      ▒▒   ▓▒█░░▓  ░  ▒░▓  ░
   ░▒        ▒   ▒▒ ░ ▒ ░░  ░ ▒  ░
   ░ ░       ░   ▒    ▒ ░   ░ ░
                 ░  ░ ░       ░  ░
"""


class ProjectBuilderCharset(NamedTuple):
    slug_ok: str
    slug_fail: str


ASCII_CHARSET = ProjectBuilderCharset(_COLOR.green('OK  '), _COLOR.red('FAIL'))
EMOJI_CHARSET = ProjectBuilderCharset('✔️ ', '💥')


def _exit(*args) -> NoReturn:
    _LOG.critical(*args)
    sys.exit(1)


def _exit_due_to_interrupt() -> NoReturn:
    # To keep the log lines aligned with each other in the presence of
    # a '^C' from the keyboard interrupt, add a newline before the log.
    print()
    _LOG.info('Got Ctrl-C; exiting...')
    sys.exit(1)


def _execute_command(command: list, env: dict) -> bool:
    print()
    current_build = subprocess.run(command, env=env, errors='replace')
    print()
    return current_build.returncode == 0


class ProjectBuilder:
    """Pigweed Project Builder"""

    def __init__(
        self,
        build_recipes: Sequence[BuildRecipe],
        jobs: Optional[int] = None,
        banners: bool = True,
        keep_going: bool = False,
        abort_callback: Callable = _exit,
        execute_command: Callable[[List, Dict], bool] = _execute_command,
        charset: ProjectBuilderCharset = ASCII_CHARSET,
        colors: bool = True,
    ):

        self.colors = colors
        self.charset: ProjectBuilderCharset = charset
        self.abort_callback = abort_callback
        self.execute_command = execute_command
        self.banners = banners
        self.build_recipes = build_recipes
        self.extra_ninja_args = [] if jobs is None else ['-j', f'{jobs}']
        if keep_going:
            self.extra_ninja_args.extend(['-k', '0'])

    def __len__(self) -> int:
        return len(self.build_recipes)

    def __getitem__(self, index: int) -> BuildRecipe:
        return self.build_recipes[index]

    def __iter__(self) -> Generator[BuildRecipe, None, None]:
        return (build_recipe for build_recipe in self.build_recipes)

    def run_build(
        self, cfg: BuildRecipe, env: Dict, index_message: Optional[str] = ''
    ) -> bool:
        """Run a single build config."""
        if self.colors:
            # Force colors in Pigweed subcommands run through the watcher.
            env['PW_USE_COLOR'] = '1'
            # Force Ninja to output ANSI colors
            env['CLICOLOR_FORCE'] = '1'

        build_succeded = False
        for command_step in cfg.steps:
            command_args = command_step.get_args(
                additional_build_args=self.extra_ninja_args
            )

            _LOG.info(
                '%s Running ==> %s',
                index_message,
                ' '.join(shlex.quote(arg) for arg in command_args),
            )

            # Verify that the build output directories exist.
            if command_step.build_system_command is not None and (
                not cfg.build_dir.is_dir()
            ):
                self.abort_callback(
                    'Build directory does not exist: %s', cfg.build_dir
                )

            build_succeded = self.execute_command(command_args, env)
            # Don't run further steps if a command fails.
            if not build_succeded:
                break

        return build_succeded

    def print_build_summary(
        self, builds_succeeded: List[bool], cancelled: bool = False
    ) -> None:
        """Print build results summary table."""
        if not cancelled:
            _LOG.info('')
            _LOG.info(' .------------------------------------')
            _LOG.info(' |')
            for (succeeded, cmd) in zip(
                builds_succeeded, [str(cfg) for cfg in self]
            ):
                slug = (
                    self.charset.slug_ok
                    if succeeded
                    else self.charset.slug_fail
                )
                _LOG.info(' |   %s  %s', slug, cmd)
            _LOG.info(' |')
            _LOG.info(" '------------------------------------")
        else:
            # Build was interrupted.
            _LOG.info('')
            _LOG.info(' .------------------------------------')
            _LOG.info(' |')
            _LOG.info(' |  %s- interrupted', self.charset.slug_fail)
            _LOG.info(' |')
            _LOG.info(" '------------------------------------")

        # Show a large color banner for the overall result.
        if self.banners:
            if all(builds_succeeded) and not cancelled:
                for line in PASS_MESSAGE.splitlines():
                    _LOG.info(_COLOR.green(line))
            else:
                for line in FAIL_MESSAGE.splitlines():
                    _LOG.info(_COLOR.red(line))


def main() -> None:
    """Build a Pigweed Project."""
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser = add_project_builder_arguments(parser)
    args = parser.parse_args()

    pw_env = pw_cli.env.pigweed_environment()
    if pw_env.PW_EMOJI:
        charset = EMOJI_CHARSET
    else:
        charset = ASCII_CHARSET

    prefs = ProjectBuilderPrefs(
        load_argparse_arguments=add_project_builder_arguments
    )
    prefs.apply_command_line_args(args)
    build_recipes = create_build_recipes(prefs)

    project_builder = ProjectBuilder(
        build_recipes=build_recipes,
        jobs=args.jobs,
        banners=args.banners,
        keep_going=args.keep_going,
        charset=charset,
    )

    pw_cli.log.install(
        level=logging.DEBUG if args.debug_logging else logging.INFO,
        use_color=args.colors,
        hide_timestamp=False,
    )

    builds_succeeded = []
    num_builds = len(project_builder)
    _LOG.info('Starting build with %d directories', num_builds)

    env = os.environ.copy()

    for i, cfg in enumerate(project_builder, 1):
        index = f'[{i}/{num_builds}]'
        build_start_msg = '{} Starting {}'.format(index, cfg)
        _LOG.info(build_start_msg)

        try:
            builds_succeeded.append(
                project_builder.run_build(cfg, env, index_message=index)
            )
        # Ctrl-C on Unix generates KeyboardInterrupt
        # Ctrl-Z on Windows generates EOFError
        except (KeyboardInterrupt, EOFError):
            _exit_due_to_interrupt()

        if builds_succeeded[-1]:
            level = logging.INFO
            tag = '(OK)'
        else:
            level = logging.ERROR
            tag = '(FAIL)'

        _LOG.log(level, '%s Finished %s %s', index, cfg, tag)

    project_builder.print_build_summary(builds_succeeded)


if __name__ == '__main__':
    main()
