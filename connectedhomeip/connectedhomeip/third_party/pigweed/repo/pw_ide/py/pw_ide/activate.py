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
# pylint: disable=line-too-long
"""Pigweed shell activation script.

Aside from importing it, this script can be used in three ways:

1. Activate the Pigweed environment in your current shell (i.e., modify your
   interactive shell's environment with Pigweed environment variables).

   Using bash (assuming a global Python 3 is in $PATH):
       source <(python3 ./pw_ide/activate.py -s bash)

   Using bash (using the environment Python):
       source <({environment}/pigweed-venv/bin/python ./pw_ide/activate.py -s bash)

2. Run a shell command or executable in an activated shell (i.e. apply a
   modified environment to a subprocess without affecting your current
   interactive shell).

    Example (assuming a global Python 3 is in $PATH):
        python3 ./pw_ide/activate.py -x 'pw ide cpp --list'

    Example (using the environment Python):
        {environment}/pigweed-venv/bin/python ./pw_ide/activate.py -x 'pw ide cpp --list'

    Example (using the environment Python on Windows):
        {environment}/pigweed-venv/Scripts/pythonw.exe ./pw_ide/activate.py -x 'pw ide cpp --list'

3. Produce a JSON representation of the Pigweed activated environment (-O) or
   the diff against your current environment that produces an activated
   environment (-o). See the help for more detailed information on the options
   available.

    Example (assuming a global Python 3 is in $PATH):
        python3 ./pw_ide/activate.py -o

    Example (using the environment Python):
        {environment}/pigweed-venv/bin/python ./pw_ide/activate.py -o

    Example (using the environment Python on Windows):
        {environment}/pigweed-venv/Scripts/pythonw.exe ./pw_ide/activate.py -o
"""
# pylint: enable=line-too-long

from abc import abstractmethod, ABC
import argparse
from collections import defaultdict
from inspect import cleandoc
import json
import os
from pathlib import Path
import shlex
import subprocess
import sys
from typing import Dict, Optional

# This expects this file to be in the Python module. If it ever moves
# (e.g. to the root of the repository), this will need to change.
_PW_PROJECT_PATH = Path(
    os.environ.get(
        'PW_PROJECT_ROOT', os.environ.get('PW_ROOT', Path(__file__).parents[3])
    )
)


def assumed_environment_root() -> Path:
    actual_environment_root = os.environ.get('_PW_ACTUAL_ENVIRONMENT_ROOT')
    if (
        actual_environment_root is not None
        and (root_path := Path(actual_environment_root)).exists()
    ):
        return root_path.absolute()

    default_environment = _PW_PROJECT_PATH / 'environment'
    if default_environment.exists():
        return default_environment.absolute()

    default_dot_environment = _PW_PROJECT_PATH / '.environment'
    if default_dot_environment.exists():
        return default_dot_environment.absolute()

    raise RuntimeError(
        'This must be run from a bootstrapped Pigweed directory!'
    )


_DEFAULT_CONFIG_FILE_PATH = assumed_environment_root() / 'actions.json'


def _sanitize_path(
    path: str, project_root_prefix: str, user_home_prefix: str
) -> str:
    """Given a path, return a sanitized path.

    By default, environment variable paths are usually absolute. If we want
    those paths to work across multiple systems, we need to sanitize them. This
    takes a string that may be a path, and if it is indeed a path, it returns
    the sanitized path, which is relative to either the repository root or the
    user's home directory. If it's not a path, it just returns the input.

    You can provide the strings that should be substituted for the project root
    and the user's home directory. This may be useful for applications that have
    their own way of representing those directories.

    Note that this is intended to work on Pigweed environment variables, which
    should all be relative to either of those two locations. Paths that aren't
    (e.g. the path to a system binary) won't really be sanitized.
    """
    # Return the argument if it's not actually a path.
    # This strategy relies on the fact that env_setup outputs absolute paths for
    # all path env vars. So if we get a variable that's not an absolute path, it
    # must not be a path at all.
    if not Path(path).is_absolute():
        return path

    project_root = _PW_PROJECT_PATH.resolve()
    user_home = Path.home().resolve()
    resolved_path = Path(path).resolve()

    # TODO(b/248257406) Remove once we drop support for Python 3.8.
    def is_relative_to(path: Path, other: Path) -> bool:
        try:
            path.relative_to(other)
            return True
        except ValueError:
            return False

    if is_relative_to(resolved_path, project_root):
        return f'{project_root_prefix}/' + str(
            resolved_path.relative_to(project_root)
        )

    if is_relative_to(resolved_path, user_home):
        return f'{user_home_prefix}/' + str(
            resolved_path.relative_to(user_home)
        )

    # Path is not in the project root or user home, so just return it as is.
    return path


class ShellModifier(ABC):
    """Abstract class for shell modifiers.

    A shell modifier provides an interface for modifying the environment
    variables in various shells. You can pass in a current environment state
    as a dictionary during instantiation and modify it and/or modify shell state
    through other side effects.
    """

    separator = ':'
    comment = '# '

    def __init__(
        self,
        env: Optional[Dict[str, str]] = None,
        env_only: bool = False,
        path_var: str = '$PATH',
        project_root: str = '.',
        user_home: str = '~',
    ):
        # This will contain only the modifications to the environment, with
        # no elements of the existing environment aside from variables included
        # here. In that sense, it's like a diff against the existing
        # environment, or a structured form of the shell modification side
        # effects.
        default_env_mod = {'PATH': path_var}
        self.env_mod = default_env_mod.copy()

        # This is seeded with the existing environment, and then is modified.
        # So it contains the complete new environment after modifications.
        # If no existing environment is provided, this is identical to env_mod.
        env = env if env is not None else default_env_mod.copy()
        self.env: Dict[str, str] = defaultdict(str, env)

        # Will contain the side effects, i.e. commands executed in the shell to
        # modify its environment.
        self.side_effects = ''

        # Set this to not do any side effects, but just modify the environment
        # stored in this class.
        self.env_only = env_only

        self.project_root = project_root
        self.user_home = user_home

    def do_effect(self, effect: str):
        """Add to the commands that will affect the shell's environment.

        This is a no-op if the shell modifier is set to only store shell
        modification data rather than doing the side effects.
        """
        if not self.env_only:
            self.side_effects += f'{effect}\n'

    def modify_env(
        self,
        config_file_path: Path = _DEFAULT_CONFIG_FILE_PATH,
        sanitize: bool = False,
    ) -> 'ShellModifier':
        """Modify the current shell state per the actions.json file provided."""
        json_file_options = {}

        with config_file_path.open('r') as json_file:
            json_file_options = json.loads(json_file.read())

        root = self.project_root
        home = self.user_home

        # Set env vars
        for var_name, value in json_file_options.get('set', dict()).items():
            if value is not None:
                value = _sanitize_path(value, root, home) if sanitize else value
                self.set_variable(var_name, value)

        # Prepend & append env vars
        for var_name, mode_changes in json_file_options.get(
            'modify', dict()
        ).items():
            for mode_name, values in mode_changes.items():
                if mode_name in ['prepend', 'append']:
                    modify_variable = self.prepend_variable

                    if mode_name == 'append':
                        modify_variable = self.append_variable

                    for value in values:
                        value = (
                            _sanitize_path(value, root, home)
                            if sanitize
                            else value
                        )
                        modify_variable(var_name, value)

        return self

    @abstractmethod
    def set_variable(self, var_name: str, value: str) -> None:
        pass

    @abstractmethod
    def prepend_variable(self, var_name: str, value: str) -> None:
        pass

    @abstractmethod
    def append_variable(self, var_name: str, value: str) -> None:
        pass


class BashShellModifier(ShellModifier):
    """Shell modifier for bash."""

    def set_variable(self, var_name: str, value: str):
        self.env[var_name] = value
        self.env_mod[var_name] = value
        quoted_value = shlex.quote(value)
        self.do_effect(f'export {var_name}={quoted_value}')

    def prepend_variable(self, var_name: str, value: str) -> None:
        self.env[var_name] = f'{value}{self.separator}{self.env[var_name]}'
        self.env_mod[
            var_name
        ] = f'{value}{self.separator}{self.env_mod[var_name]}'
        quoted_value = shlex.quote(value)
        self.do_effect(
            f'export {var_name}={quoted_value}{self.separator}${var_name}'
        )

    def append_variable(self, var_name: str, value: str) -> None:
        self.env[var_name] = f'{self.env[var_name]}{self.separator}{value}'
        self.env_mod[
            var_name
        ] = f'{self.env_mod[var_name]}{self.separator}{value}'
        quoted_value = shlex.quote(value)
        self.do_effect(
            f'export {var_name}=${var_name}{self.separator}{quoted_value}'
        )


def _build_argument_parser() -> argparse.ArgumentParser:
    """Set up `argparse`."""
    doc = __doc__

    try:
        env_root = assumed_environment_root()
    except RuntimeError:
        env_root = None

    # Substitute in the actual environment path in the help text, if we can
    # find it. If not, leave the placeholder text.
    if env_root is not None:
        doc = doc.replace(
            '{environment}', str(env_root.relative_to(Path.cwd()))
        )

    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description=doc,
    )

    parser.add_argument(
        '-c',
        '--config-file',
        default=_DEFAULT_CONFIG_FILE_PATH,
        type=Path,
        help='Path to actions.json config file, which defines '
        'the modifications to the shell environment '
        'needed to activate Pigweed. '
        f'Default: {_DEFAULT_CONFIG_FILE_PATH.relative_to(Path.cwd())}',
    )

    default_shell = Path(os.environ['SHELL']).name
    parser.add_argument(
        '-s',
        '--shell-mode',
        default=default_shell,
        help='Which shell is being used. ' f'Default: {default_shell}',
    )

    parser.add_argument(
        '-o',
        '--out',
        action='store_true',
        help='Write only the modifications to the environment ' 'out to JSON.',
    )

    parser.add_argument(
        '-O',
        '--out-all',
        action='store_true',
        help='Write the complete modified environment to ' 'JSON.',
    )

    parser.add_argument(
        '-n',
        '--sanitize',
        action='store_true',
        help='Sanitize paths that are relative to the repo '
        'root or user home directory so that they are portable '
        'to other workstations.',
    )

    parser.add_argument(
        '--path-var',
        default='$PATH',
        help='The string to substitute for the existing $PATH. Default: $PATH',
    )

    parser.add_argument(
        '--project-root',
        default='.',
        help='The string to substitute for the project root when sanitizing '
        'paths. Default: .',
    )

    parser.add_argument(
        '--user-home',
        default='~',
        help='The string to substitute for the user\'s home when sanitizing '
        'paths. Default: ~',
    )

    parser.add_argument(
        '-x',
        '--exec',
        help='A command to execute in the activated shell.',
        metavar='COMMAND',
    )

    return parser


def main() -> int:
    """The main CLI script."""
    args, _unused_extra_args = _build_argument_parser().parse_known_args()
    env = os.environ.copy()
    config_file_path = args.config_file

    if not config_file_path.exists():
        sys.stderr.write(f'File not found! {config_file_path}')
        sys.stderr.write(
            'This must be run from a bootstrapped Pigweed ' 'project directory.'
        )
        sys.exit(1)

    # If we're executing a command in a subprocess, don't modify the current
    # shell's state. Instead, apply the modified state to the subprocess.
    env_only = args.exec is not None

    # Assume bash by default.
    shell_modifier = BashShellModifier

    # TODO(chadnorvell): if args.shell_mode == 'zsh', 'ksh', 'fish'...
    try:
        modified_env = shell_modifier(
            env=env,
            env_only=env_only,
            path_var=args.path_var,
            project_root=args.project_root,
            user_home=args.user_home,
        ).modify_env(config_file_path, args.sanitize)
    except (FileNotFoundError, json.JSONDecodeError):
        sys.stderr.write(
            'Unable to read file: {}\n'
            'Please run this in bash or zsh:\n'
            '  . ./bootstrap.sh\n'.format(str(config_file_path))
        )

        sys.exit(1)

    if args.out_all:
        print(json.dumps(modified_env.env, sort_keys=True, indent=2))
        return 0

    if args.out:
        print(json.dumps(modified_env.env_mod, sort_keys=True, indent=2))
        return 0

    if args.exec is not None:
        # We're executing a command in a subprocess with the modified env.
        return subprocess.run(
            args.exec, env=modified_env.env, shell=True
        ).returncode

    # If we got here, we're trying to modify the current shell's env.
    print(modified_env.side_effects)

    # Let's warn the user if the output is going to stdout instead of being
    # executed by the shell.
    python_path = Path(sys.executable).relative_to(os.getcwd())
    c = shell_modifier.comment  # pylint: disable=invalid-name
    print(
        cleandoc(
            f"""
        {c}
        {c}Can you see these commands? If so, you probably wanted to
        {c}source this script instead of running it. Try this instead:
        {c}
        {c}    . <({str(python_path)} {' '.join(sys.argv)})
        {c}
        {c}Run this script with `-h` for more help."""
        )
    )
    return 0


if __name__ == '__main__':
    sys.exit(main())
