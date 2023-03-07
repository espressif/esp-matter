#!/usr/bin/env python

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
"""Environment setup script for Pigweed.

This script installs everything and writes out a file for the user's shell
to source.

For now, this is valid Python 2 and Python 3. Once we switch to running this
with PyOxidizer it can be upgraded to recent Python 3.
"""

from __future__ import print_function

import argparse
import copy
import glob
import inspect
import json
import os
import shutil
import subprocess
import sys
import time

# If we're running oxidized, filesystem-centric import hacks won't work. In that
# case, jump straight to the imports and assume oxidation brought in the deps.
if not getattr(sys, 'oxidized', False):
    old_sys_path = copy.deepcopy(sys.path)
    filename = None
    if hasattr(sys.modules[__name__], '__file__'):
        filename = __file__
    else:
        # Try introspection in environments where __file__ is not populated.
        frame = inspect.currentframe()
        if frame is not None:
            filename = inspect.getfile(frame)
    # If none of our strategies worked, we're in a strange runtime environment.
    # The imports are almost certainly going to fail.
    if filename is None:
        raise RuntimeError(
            'Unable to locate pw_env_setup module; cannot continue.\n'
            '\n'
            'Try updating to one of the standard Python implemetations:\n'
            '  https://www.python.org/downloads/'
        )
    sys.path = [
        os.path.abspath(os.path.join(filename, os.path.pardir, os.path.pardir))
    ]
    import pw_env_setup  # pylint: disable=unused-import

    sys.path = old_sys_path

# pylint: disable=wrong-import-position
from pw_env_setup.cipd_setup import update as cipd_update
from pw_env_setup.cipd_setup import wrapper as cipd_wrapper
from pw_env_setup.colors import Color, enable_colors
from pw_env_setup import environment
from pw_env_setup import spinner
from pw_env_setup import virtualenv_setup
from pw_env_setup import windows_env_start


def _which(
    executable, pathsep=os.pathsep, use_pathext=None, case_sensitive=None
):
    if use_pathext is None:
        use_pathext = os.name == 'nt'
    if case_sensitive is None:
        case_sensitive = os.name != 'nt' and sys.platform != 'darwin'

    if not case_sensitive:
        executable = executable.lower()

    exts = None
    if use_pathext:
        exts = frozenset(os.environ['PATHEXT'].split(pathsep))
        if not case_sensitive:
            exts = frozenset(x.lower() for x in exts)
        if not exts:
            raise ValueError('empty PATHEXT')

    paths = os.environ['PATH'].split(pathsep)
    for path in paths:
        try:
            entries = frozenset(os.listdir(path))
            if not case_sensitive:
                entries = frozenset(x.lower() for x in entries)
        except OSError:
            continue

        if exts:
            for ext in exts:
                if executable + ext in entries:
                    return os.path.join(path, executable + ext)
        else:
            if executable in entries:
                return os.path.join(path, executable)

    return None


class _Result:
    class Status:
        DONE = 'done'
        SKIPPED = 'skipped'
        FAILED = 'failed'

    def __init__(self, status, *messages):
        self._status = status
        self._messages = list(messages)

    def ok(self):
        return self._status in {_Result.Status.DONE, _Result.Status.SKIPPED}

    def status_str(self, duration=None):
        if not duration:
            return self._status

        duration_parts = []
        if duration > 60:
            minutes = int(duration // 60)
            duration %= 60
            duration_parts.append('{}m'.format(minutes))
        duration_parts.append('{:.1f}s'.format(duration))
        return '{} ({})'.format(self._status, ''.join(duration_parts))

    def messages(self):
        return self._messages


class ConfigError(Exception):
    pass


def result_func(glob_warnings=()):
    def result(status, *args):
        return _Result(status, *([str(x) for x in glob_warnings] + list(args)))

    return result


class ConfigFileError(Exception):
    pass


class MissingSubmodulesError(Exception):
    pass


# TODO(mohrr) remove disable=useless-object-inheritance once in Python 3.
# pylint: disable=useless-object-inheritance
# pylint: disable=too-many-instance-attributes
# pylint: disable=too-many-arguments
class EnvSetup(object):
    """Run environment setup for Pigweed."""

    def __init__(
        self,
        pw_root,
        cipd_cache_dir,
        shell_file,
        quiet,
        install_dir,
        strict,
        virtualenv_gn_out_dir,
        json_file,
        project_root,
        config_file,
        use_existing_cipd,
        check_submodules,
        use_pinned_pip_packages,
        cipd_only,
        trust_cipd_hash,
    ):
        self._env = environment.Environment()
        self._project_root = project_root
        self._pw_root = pw_root
        self._setup_root = os.path.join(
            pw_root, 'pw_env_setup', 'py', 'pw_env_setup'
        )
        self._cipd_cache_dir = cipd_cache_dir
        self._shell_file = shell_file
        self._is_windows = os.name == 'nt'
        self._quiet = quiet
        self._install_dir = install_dir
        self._virtualenv_root = os.path.join(self._install_dir, 'pigweed-venv')
        self._strict = strict
        self._cipd_only = cipd_only
        self._trust_cipd_hash = trust_cipd_hash

        if os.path.isfile(shell_file):
            os.unlink(shell_file)

        if isinstance(self._pw_root, bytes) and bytes != str:
            self._pw_root = self._pw_root.decode()

        self._cipd_package_file = []
        self._virtualenv_requirements = []
        self._virtualenv_constraints = []
        self._virtualenv_gn_targets = []
        self._virtualenv_gn_args = []
        self._use_pinned_pip_packages = use_pinned_pip_packages
        self._optional_submodules = []
        self._required_submodules = []
        self._virtualenv_system_packages = False
        self._pw_packages = []
        self._root_variable = None

        self._check_submodules = check_submodules

        self._json_file = json_file
        self._gni_file = None

        self._config_file_name = getattr(config_file, 'name', 'config file')
        self._env.set('_PW_ENVIRONMENT_CONFIG_FILE', self._config_file_name)
        if config_file:
            self._parse_config_file(config_file)

        self._check_submodule_presence()

        self._use_existing_cipd = use_existing_cipd
        self._virtualenv_gn_out_dir = virtualenv_gn_out_dir

        if self._root_variable:
            self._env.set(self._root_variable, project_root, deactivate=False)
        self._env.set('PW_PROJECT_ROOT', project_root, deactivate=False)
        self._env.set('PW_ROOT', pw_root, deactivate=False)
        self._env.set('_PW_ACTUAL_ENVIRONMENT_ROOT', install_dir)
        self._env.add_replacement('_PW_ACTUAL_ENVIRONMENT_ROOT', install_dir)
        self._env.add_replacement('PW_ROOT', pw_root)

    def _process_globs(self, globs):
        unique_globs = []
        for pat in globs:
            if pat and pat not in unique_globs:
                unique_globs.append(pat)

        files = []
        warnings = []
        for pat in unique_globs:
            if pat:
                matches = glob.glob(pat)
                if not matches:
                    warning = 'pattern "{}" matched 0 files'.format(pat)
                    warnings.append('warning: {}'.format(warning))
                    if self._strict:
                        raise ConfigError(warning)

                files.extend(matches)

        if globs and not files:
            warnings.append('warning: matched 0 total files')
            if self._strict:
                raise ConfigError('matched 0 total files')

        return files, warnings

    def _parse_config_file(self, config_file):
        config = json.load(config_file)

        self._root_variable = config.pop('root_variable', None)

        rosetta = config.pop('rosetta', 'allow')
        if rosetta not in ('never', 'allow', 'force'):
            raise ValueError(rosetta)
        self._rosetta = rosetta in ('allow', 'force')
        self._env.set('_PW_ROSETTA', str(int(self._rosetta)))

        if 'json_file' in config:
            self._json_file = config.pop('json_file')

        self._gni_file = config.pop('gni_file', None)

        self._optional_submodules.extend(config.pop('optional_submodules', ()))
        self._required_submodules.extend(config.pop('required_submodules', ()))

        if self._optional_submodules and self._required_submodules:
            raise ValueError(
                '{} contains both "optional_submodules" and '
                '"required_submodules", but these options are mutually '
                'exclusive'.format(self._config_file_name)
            )

        self._cipd_package_file.extend(
            os.path.join(self._project_root, x)
            for x in config.pop('cipd_package_files', ())
        )

        for pkg in config.pop('pw_packages', ()):
            self._pw_packages.append(pkg)

        virtualenv = config.pop('virtualenv', {})

        if virtualenv.get('gn_root'):
            root = os.path.join(self._project_root, virtualenv.pop('gn_root'))
        else:
            root = self._project_root

        for target in virtualenv.pop('gn_targets', ()):
            self._virtualenv_gn_targets.append(
                virtualenv_setup.GnTarget('{}#{}'.format(root, target))
            )

        self._virtualenv_gn_args = virtualenv.pop('gn_args', ())

        self._virtualenv_system_packages = virtualenv.pop(
            'system_packages', False
        )

        for req_txt in virtualenv.pop('requirements', ()):
            self._virtualenv_requirements.append(
                os.path.join(self._project_root, req_txt)
            )

        for constraint_txt in virtualenv.pop('constraints', ()):
            self._virtualenv_constraints.append(
                os.path.join(self._project_root, constraint_txt)
            )

        if virtualenv:
            raise ConfigFileError(
                'unrecognized option in {}: "virtualenv.{}"'.format(
                    self._config_file_name, next(iter(virtualenv))
                )
            )

        if config:
            raise ConfigFileError(
                'unrecognized option in {}: "{}"'.format(
                    self._config_file_name, next(iter(config))
                )
            )

    def _check_submodule_presence(self):
        uninitialized = set()

        # Don't check submodule presence if using the Android Repo Tool.
        if os.path.isdir(os.path.join(self._project_root, '.repo')):
            return

        if not self._check_submodules:
            return

        cmd = ['git', 'submodule', 'status', '--recursive']

        for line in subprocess.check_output(
            cmd, cwd=self._project_root
        ).splitlines():
            if isinstance(line, bytes):
                line = line.decode()
            # Anything but an initial '-' means the submodule is initialized.
            if not line.startswith('-'):
                continue
            uninitialized.add(line.split()[1])

        missing = uninitialized - set(self._optional_submodules)
        if self._required_submodules:
            missing = set(self._required_submodules) & uninitialized

        if missing:
            print(
                'Not all submodules are initialized. Please run the '
                'following commands.',
                file=sys.stderr,
            )
            print('', file=sys.stderr)

            for miss in sorted(missing):
                print(
                    '    git submodule update --init {}'.format(miss),
                    file=sys.stderr,
                )
            print('', file=sys.stderr)

            if self._required_submodules:
                print(
                    'If these submodules are not required, remove them from '
                    'the "required_submodules"',
                    file=sys.stderr,
                )

            else:
                print(
                    'If these submodules are not required, add them to the '
                    '"optional_submodules"',
                    file=sys.stderr,
                )

            print('list in the environment config JSON file:', file=sys.stderr)
            print('    {}'.format(self._config_file_name), file=sys.stderr)
            print('', file=sys.stderr)

            raise MissingSubmodulesError(', '.join(sorted(missing)))

    def _write_gni_file(self):
        if self._cipd_only:
            return

        gni_file = os.path.join(
            self._project_root, 'build_overrides', 'pigweed_environment.gni'
        )
        if self._gni_file:
            gni_file = os.path.join(self._project_root, self._gni_file)

        with open(gni_file, 'w') as outs:
            self._env.gni(outs, self._project_root)

    def _log(self, *args, **kwargs):
        # Not using logging module because it's awkward to flush a log handler.
        if self._quiet:
            return
        flush = kwargs.pop('flush', False)
        print(*args, **kwargs)
        if flush:
            sys.stdout.flush()

    def setup(self):
        """Runs each of the env_setup steps."""

        if os.name == 'nt':
            windows_env_start.print_banner(bootstrap=True, no_shell_file=False)
        else:
            enable_colors()

        steps = [
            ('CIPD package manager', self.cipd),
            ('Python environment', self.virtualenv),
            ('pw packages', self.pw_package),
            ('Host tools', self.host_tools),
        ]

        if self._is_windows:
            steps.append(("Windows scripts", self.win_scripts))

        if self._cipd_only:
            steps = [('CIPD package manager', self.cipd)]

        self._log(
            Color.bold(
                'Downloading and installing packages into local '
                'source directory:\n'
            )
        )

        max_name_len = max(len(name) for name, _ in steps)

        self._env.comment(
            '''
This file is automatically generated. DO NOT EDIT!
For details, see $PW_ROOT/pw_env_setup/py/pw_env_setup/env_setup.py and
$PW_ROOT/pw_env_setup/py/pw_env_setup/environment.py.
'''.strip()
        )

        if not self._is_windows:
            self._env.comment(
                '''
For help debugging errors in this script, uncomment the next line.
set -x
Then use `set +x` to go back to normal.
'''.strip()
            )

        self._env.echo(
            Color.bold(
                'Activating environment (setting environment variables):'
            )
        )
        self._env.echo('')

        for name, step in steps:
            self._log(
                '  Setting up {name:.<{width}}...'.format(
                    name=name, width=max_name_len
                ),
                end='',
                flush=True,
            )
            self._env.echo(
                '  Setting environment variables for '
                '{name:.<{width}}...'.format(name=name, width=max_name_len),
                newline=False,
            )

            start = time.time()
            spin = spinner.Spinner(self._quiet)
            with spin():
                result = step(spin)
            stop = time.time()

            self._log(result.status_str(stop - start))

            self._env.echo(result.status_str())
            for message in result.messages():
                sys.stderr.write('{}\n'.format(message))
                self._env.echo(message)

            if not result.ok():
                return -1

            # Log the environment state at the end of each step for debugging.
            log_dir = os.path.join(self._install_dir, 'logs')
            if not os.path.isdir(log_dir):
                os.makedirs(log_dir)
            actions_json = os.path.join(
                log_dir, 'post-{}.json'.format(name.replace(' ', '_'))
            )
            with open(actions_json, 'w') as outs:
                self._env.json(outs)

            # This file needs to be written after the CIPD step and before the
            # Python virtualenv step. It also needs to be rewritten after the
            # Python virtualenv step, so it's easiest to just write it after
            # every step.
            self._write_gni_file()

        self._log('')
        self._env.echo('')

        self._env.finalize()

        self._env.echo(Color.bold('Checking the environment:'))
        self._env.echo()

        self._env.doctor()
        self._env.echo()

        self._env.echo(
            Color.bold('Environment looks good, you are ready to go!')
        )
        self._env.echo()

        # Don't write new files if all we did was update CIPD packages.
        if self._cipd_only:
            return 0

        with open(self._shell_file, 'w') as outs:
            self._env.write(outs)

        deactivate = os.path.join(
            self._install_dir,
            'deactivate{}'.format(os.path.splitext(self._shell_file)[1]),
        )
        with open(deactivate, 'w') as outs:
            self._env.write_deactivate(outs)

        config = {
            # Skipping sysname and nodename in os.uname(). nodename could change
            # based on the current network. sysname won't change, but is
            # redundant because it's contained in release or version, and
            # skipping it here simplifies logic.
            'uname': ' '.join(getattr(os, 'uname', lambda: ())()[2:]),
            'os': os.name,
        }

        with open(os.path.join(self._install_dir, 'config.json'), 'w') as outs:
            outs.write(
                json.dumps(config, indent=4, separators=(',', ': ')) + '\n'
            )

        json_file = self._json_file or os.path.join(
            self._install_dir, 'actions.json'
        )
        with open(json_file, 'w') as outs:
            self._env.json(outs)

        return 0

    def cipd(self, spin):
        """Set up cipd and install cipd packages."""

        install_dir = os.path.join(self._install_dir, 'cipd')

        # There's no way to get to the UnsupportedPlatform exception if this
        # flag is set, but this flag should only be set in LUCI builds which
        # will always have CIPD.
        if self._use_existing_cipd:
            cipd_client = 'cipd'

        else:
            try:
                cipd_client = cipd_wrapper.init(
                    install_dir,
                    silent=True,
                    rosetta=self._rosetta,
                )
            except cipd_wrapper.UnsupportedPlatform as exc:
                return result_func(('    {!r}'.format(exc),))(
                    _Result.Status.SKIPPED,
                    '    abandoning CIPD setup',
                )

        package_files, glob_warnings = self._process_globs(
            self._cipd_package_file
        )
        result = result_func(glob_warnings)

        if not package_files:
            return result(_Result.Status.SKIPPED)

        if not cipd_update.update(
            cipd=cipd_client,
            root_install_dir=install_dir,
            package_files=package_files,
            cache_dir=self._cipd_cache_dir,
            env_vars=self._env,
            rosetta=self._rosetta,
            spin=spin,
            trust_hash=self._trust_cipd_hash,
        ):
            return result(_Result.Status.FAILED)

        return result(_Result.Status.DONE)

    def virtualenv(self, unused_spin):
        """Setup virtualenv."""

        requirements, req_glob_warnings = self._process_globs(
            self._virtualenv_requirements
        )

        constraints, constraint_glob_warnings = self._process_globs(
            self._virtualenv_constraints
        )

        result = result_func(req_glob_warnings + constraint_glob_warnings)

        orig_python3 = _which('python3')
        with self._env():
            new_python3 = _which('python3')

        # There is an issue with the virtualenv module on Windows where it
        # expects sys.executable to be called "python.exe" or it fails to
        # properly execute. If we installed Python 3 in the CIPD step we need
        # to address this. Detect if we did so and if so create a copy of
        # python3.exe called python.exe so that virtualenv works.
        if orig_python3 != new_python3 and self._is_windows:
            python3_copy = os.path.join(
                os.path.dirname(new_python3), 'python.exe'
            )
            if not os.path.exists(python3_copy):
                shutil.copyfile(new_python3, python3_copy)
            new_python3 = python3_copy

        if not requirements and not self._virtualenv_gn_targets:
            return result(_Result.Status.SKIPPED)

        if not virtualenv_setup.install(
            project_root=self._project_root,
            venv_path=self._virtualenv_root,
            requirements=requirements,
            constraints=constraints,
            gn_args=self._virtualenv_gn_args,
            gn_targets=self._virtualenv_gn_targets,
            gn_out_dir=self._virtualenv_gn_out_dir,
            python=new_python3,
            env=self._env,
            system_packages=self._virtualenv_system_packages,
            use_pinned_pip_packages=self._use_pinned_pip_packages,
        ):
            return result(_Result.Status.FAILED)

        return result(_Result.Status.DONE)

    def pw_package(self, unused_spin):
        """Install "default" pw packages."""

        result = result_func()

        pkg_dir = os.path.join(self._install_dir, 'packages')
        self._env.set('PW_PACKAGE_ROOT', pkg_dir)

        if not os.path.isdir(pkg_dir):
            os.makedirs(pkg_dir)

        if not self._pw_packages:
            return result(_Result.Status.SKIPPED)

        for pkg in self._pw_packages:
            print('installing {}'.format(pkg))
            cmd = ['pw', 'package', 'install', pkg]

            log = os.path.join(pkg_dir, '{}.log'.format(pkg))
            try:
                with open(log, 'w') as outs, self._env():
                    print(*cmd, file=outs)
                    subprocess.check_call(
                        cmd,
                        cwd=self._project_root,
                        stdout=outs,
                        stderr=subprocess.STDOUT,
                    )
            except subprocess.CalledProcessError:
                with open(log, 'r') as ins:
                    sys.stderr.write(ins.read())
                    raise

        return result(_Result.Status.DONE)

    def host_tools(self, unused_spin):
        # The host tools are grabbed from CIPD, at least initially. If the
        # user has a current host build, that build will be used instead.
        # TODO(mohrr) find a way to do stuff like this for all projects.
        host_dir = os.path.join(self._pw_root, 'out', 'host')
        self._env.prepend('PATH', os.path.join(host_dir, 'host_tools'))
        return _Result(_Result.Status.DONE)

    def win_scripts(self, unused_spin):
        # These scripts act as a compatibility layer for windows.
        env_setup_dir = os.path.join(self._pw_root, 'pw_env_setup')
        self._env.prepend(
            'PATH', os.path.join(env_setup_dir, 'windows_scripts')
        )
        return _Result(_Result.Status.DONE)


def parse(argv=None):
    """Parse command-line arguments."""
    parser = argparse.ArgumentParser(prog="python -m pw_env_setup.env_setup")

    pw_root = os.environ.get('PW_ROOT', None)
    if not pw_root:
        try:
            with open(os.devnull, 'w') as outs:
                pw_root = subprocess.check_output(
                    ['git', 'rev-parse', '--show-toplevel'], stderr=outs
                ).strip()
        except subprocess.CalledProcessError:
            pw_root = None

    parser.add_argument(
        '--pw-root',
        default=pw_root,
        required=not pw_root,
    )

    project_root = os.environ.get('PW_PROJECT_ROOT', None) or pw_root

    parser.add_argument(
        '--project-root',
        default=project_root,
        required=not project_root,
    )

    parser.add_argument(
        '--cipd-cache-dir',
        default=os.environ.get(
            'CIPD_CACHE_DIR', os.path.expanduser('~/.cipd-cache-dir')
        ),
    )

    parser.add_argument(
        '--trust-cipd-hash',
        action='store_true',
        help='Only run the cipd executable if the ensure file or command-line '
        'has changed. Defaults to false since files could have been deleted '
        'from the installation directory and cipd would add them back.',
    )

    parser.add_argument(
        '--shell-file',
        help='Where to write the file for shells to source.',
        required=True,
    )

    parser.add_argument(
        '--quiet',
        help='Reduce output.',
        action='store_true',
        default='PW_ENVSETUP_QUIET' in os.environ,
    )

    parser.add_argument(
        '--install-dir',
        help='Location to install environment.',
        required=True,
    )

    parser.add_argument(
        '--config-file',
        help='JSON file describing CIPD and virtualenv requirements.',
        type=argparse.FileType('r'),
        required=True,
    )

    parser.add_argument(
        '--virtualenv-gn-out-dir',
        help=(
            'Output directory to use when building and installing Python '
            'packages with GN; defaults to a unique path in the environment '
            'directory.'
        ),
    )

    parser.add_argument('--json-file', help=argparse.SUPPRESS, default=None)

    parser.add_argument(
        '--use-existing-cipd',
        help='Use cipd executable from the environment instead of fetching it.',
        action='store_true',
    )

    parser.add_argument(
        '--strict',
        help='Fail if there are any warnings.',
        action='store_true',
    )

    parser.add_argument(
        '--unpin-pip-packages',
        dest='use_pinned_pip_packages',
        help='Do not use pins of pip packages.',
        action='store_false',
    )

    parser.add_argument(
        '--cipd-only',
        help='Skip non-CIPD steps.',
        action='store_true',
    )

    parser.add_argument(
        '--skip-submodule-check',
        help='Skip checking for submodule presence.',
        dest='check_submodules',
        action='store_false',
    )

    args = parser.parse_args(argv)

    return args


def main():
    try:
        return EnvSetup(**vars(parse())).setup()
    except subprocess.CalledProcessError as err:
        print()
        print(err.output)
        raise


if __name__ == '__main__':
    sys.exit(main())
