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
"""Script that preprocesses a Python command then runs it.

This script evaluates expressions in the Python command's arguments then invokes
the command.
"""

import argparse
import atexit
import json
import logging
import os
from pathlib import Path
import platform
import shlex
import subprocess
import sys
import time
from typing import List, Optional, Tuple

try:
    from pw_build import gn_resolver
    from pw_build.python_package import load_packages
except (ImportError, ModuleNotFoundError):
    # Load from python_package from this directory if pw_build is not available.
    from python_package import load_packages  # type: ignore
    import gn_resolver  # type: ignore

if sys.platform != 'win32':
    import fcntl  # pylint: disable=import-error

    # TODO(b/227670947): Support Windows.

_LOG = logging.getLogger(__name__)
_LOCK_ACQUISITION_TIMEOUT = 30 * 60  # 30 minutes in seconds

# TODO(frolv): Remove these aliases once downstream projects are migrated.
GnPaths = gn_resolver.GnPaths
expand_expressions = gn_resolver.expand_expressions


def _parse_args() -> argparse.Namespace:
    """Parses arguments for this script, splitting out the command to run."""

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        '--gn-root',
        type=Path,
        required=True,
        help=(
            'Path to the root of the GN tree; '
            'value of rebase_path("//", root_build_dir)'
        ),
    )
    parser.add_argument(
        '--current-path',
        type=Path,
        required=True,
        help='Value of rebase_path(".", root_build_dir)',
    )
    parser.add_argument(
        '--default-toolchain', required=True, help='Value of default_toolchain'
    )
    parser.add_argument(
        '--current-toolchain', required=True, help='Value of current_toolchain'
    )
    parser.add_argument('--module', help='Run this module instead of a script')
    parser.add_argument(
        '--env',
        action='append',
        help='Environment variables to set as NAME=VALUE',
    )
    parser.add_argument(
        '--touch',
        type=Path,
        help='File to touch after the command is run',
    )
    parser.add_argument(
        '--capture-output',
        action='store_true',
        help='Capture subcommand output; display only on error',
    )
    parser.add_argument(
        '--working-directory',
        type=Path,
        help='Change to this working directory before running the subcommand',
    )
    parser.add_argument(
        '--python-dep-list-files',
        nargs='+',
        type=Path,
        help='Paths to text files containing lists of Python package metadata '
        'json files.',
    )
    parser.add_argument(
        '--python-virtualenv-config',
        type=Path,
        help='Path to a virtualenv json config to use for this action.',
    )
    parser.add_argument(
        'original_cmd',
        nargs=argparse.REMAINDER,
        help='Python script with arguments to run',
    )
    parser.add_argument(
        '--lockfile',
        type=Path,
        help=(
            'Path to a pip lockfile. Any pip execution will acquire an '
            'exclusive lock on it, any other module a shared lock.'
        ),
    )
    return parser.parse_args()


class LockAcquisitionTimeoutError(Exception):
    """Raised on a timeout."""


def acquire_lock(lockfile: Path, exclusive: bool):
    """Attempts to acquire the lock.

    Args:
      lockfile: pathlib.Path to the lock.
      exclusive: whether this needs to be an exclusive lock.

    Raises:
      LockAcquisitionTimeoutError: If the lock is not acquired after a
        reasonable time.
    """
    if sys.platform == 'win32':
        # No-op on Windows, which doesn't have POSIX file locking.
        # TODO(b/227670947): Get this working on Windows, too.
        return

    start_time = time.monotonic()
    if exclusive:
        lock_type = fcntl.LOCK_EX  # type: ignore[name-defined]
    else:
        lock_type = fcntl.LOCK_SH  # type: ignore[name-defined]
    fd = os.open(lockfile, os.O_RDWR | os.O_CREAT)

    # Make sure we close the file when the process exits. If we manage to
    # acquire the lock below, closing the file will release it.
    def cleanup():
        os.close(fd)

    atexit.register(cleanup)

    backoff = 1
    while time.monotonic() - start_time < _LOCK_ACQUISITION_TIMEOUT:
        try:
            fcntl.flock(  # type: ignore[name-defined]
                fd, lock_type | fcntl.LOCK_NB  # type: ignore[name-defined]
            )
            return  # Lock acquired!
        except BlockingIOError:
            pass  # Keep waiting.

        time.sleep(backoff * 0.05)
        backoff += 1

    raise LockAcquisitionTimeoutError(
        f"Failed to acquire lock {lockfile} in {_LOCK_ACQUISITION_TIMEOUT}"
    )


class MissingPythonDependency(Exception):
    """An error occurred while processing a Python dependency."""


def _load_virtualenv_config(json_file_path: Path) -> Tuple[str, str]:
    with json_file_path.open() as json_fp:
        json_dict = json.load(json_fp)
    return json_dict.get('interpreter'), json_dict.get('path')


def main(  # pylint: disable=too-many-arguments,too-many-branches,too-many-locals
    gn_root: Path,
    current_path: Path,
    original_cmd: List[str],
    default_toolchain: str,
    current_toolchain: str,
    module: Optional[str],
    env: Optional[List[str]],
    python_dep_list_files: List[Path],
    python_virtualenv_config: Optional[Path],
    capture_output: bool,
    touch: Optional[Path],
    working_directory: Optional[Path],
    lockfile: Optional[Path],
) -> int:
    """Script entry point."""

    python_paths_list = []
    if python_dep_list_files:
        py_packages = load_packages(
            python_dep_list_files,
            # If this python_action has no gn python_deps this file will be
            # empty.
            ignore_missing=True,
        )

        for pkg in py_packages:
            top_level_source_dir = pkg.package_dir
            if not top_level_source_dir:
                raise MissingPythonDependency(
                    'Unable to find top level source dir for the Python '
                    f'package "{pkg}"'
                )
            # Don't add this dir to the PYTHONPATH if no __init__.py exists.
            init_py_files = top_level_source_dir.parent.glob('*/__init__.py')
            if not any(init_py_files):
                continue
            python_paths_list.append(
                gn_resolver.abspath(top_level_source_dir.parent)
            )

        # Sort the PYTHONPATH list, it will be in a different order each build.
        python_paths_list = sorted(python_paths_list)

    if not original_cmd or original_cmd[0] != '--':
        _LOG.error('%s requires a command to run', sys.argv[0])
        return 1

    # GN build scripts are executed from the root build directory.
    root_build_dir = gn_resolver.abspath(Path.cwd())

    tool = current_toolchain if current_toolchain != default_toolchain else ''
    paths = gn_resolver.GnPaths(
        root=gn_resolver.abspath(gn_root),
        build=root_build_dir,
        cwd=gn_resolver.abspath(current_path),
        toolchain=tool,
    )

    command = [sys.executable]

    python_interpreter = None
    python_virtualenv = None
    if python_virtualenv_config:
        python_interpreter, python_virtualenv = _load_virtualenv_config(
            python_virtualenv_config
        )

    if python_interpreter is not None:
        command = [str(root_build_dir / python_interpreter)]

    if module is not None:
        command += ['-m', module]

    run_args: dict = dict()
    # Always inherit the environtment by default. If PYTHONPATH or VIRTUALENV is
    # set below then the environment vars must be copied in or subprocess.run
    # will run with only the new updated variables.
    run_args['env'] = os.environ.copy()

    if env is not None:
        environment = os.environ.copy()
        environment.update((k, v) for k, v in (a.split('=', 1) for a in env))
        run_args['env'] = environment

    script_command = original_cmd[0]
    if script_command == '--':
        script_command = original_cmd[1]

    is_pip_command = (
        module == 'pip' or 'pip_install_python_deps.py' in script_command
    )

    existing_env = run_args['env'] if 'env' in run_args else os.environ.copy()
    new_env = {}
    if python_virtualenv:
        new_env['VIRTUAL_ENV'] = str(root_build_dir / python_virtualenv)
        bin_folder = 'Scripts' if platform.system() == 'Windows' else 'bin'
        new_env['PATH'] = os.pathsep.join(
            [
                str(root_build_dir / python_virtualenv / bin_folder),
                existing_env.get('PATH', ''),
            ]
        )

    if python_virtualenv and python_paths_list and not is_pip_command:
        python_path_prepend = os.pathsep.join(
            str(p) for p in set(python_paths_list)
        )

        # Append the existing PYTHONPATH to the new one.
        new_python_path = os.pathsep.join(
            path_str
            for path_str in [
                python_path_prepend,
                existing_env.get('PYTHONPATH', ''),
            ]
            if path_str
        )

        new_env['PYTHONPATH'] = new_python_path

    if 'env' not in run_args:
        run_args['env'] = {}
    run_args['env'].update(new_env)

    if capture_output:
        # Combine stdout and stderr so that error messages are correctly
        # interleaved with the rest of the output.
        run_args['stdout'] = subprocess.PIPE
        run_args['stderr'] = subprocess.STDOUT

    # Build the command to run.
    try:
        for arg in original_cmd[1:]:
            command += gn_resolver.expand_expressions(paths, arg)
    except gn_resolver.ExpressionError as err:
        _LOG.error('%s: %s', sys.argv[0], err)
        return 1

    if working_directory:
        run_args['cwd'] = working_directory

    # TODO(b/235239674): Deprecate the --lockfile option as part of the Python
    # GN template refactor.
    if lockfile:
        try:
            acquire_lock(lockfile, is_pip_command)
        except LockAcquisitionTimeoutError as exception:
            _LOG.error('%s', exception)
            return 1

    _LOG.debug('RUN %s', ' '.join(shlex.quote(arg) for arg in command))

    completed_process = subprocess.run(command, **run_args)

    if completed_process.returncode != 0:
        _LOG.debug(
            'Command failed; exit code: %d', completed_process.returncode
        )
        if capture_output:
            sys.stdout.buffer.write(completed_process.stdout)
    elif touch:
        # If a stamp file is provided and the command executed successfully,
        # touch the stamp file to indicate a successful run of the command.
        touch = touch.resolve()
        _LOG.debug('TOUCH %s', touch)

        # Create the parent directory in case GN / Ninja hasn't created it.
        touch.parent.mkdir(parents=True, exist_ok=True)
        touch.touch()

    return completed_process.returncode


if __name__ == '__main__':
    sys.exit(main(**vars(_parse_args())))
