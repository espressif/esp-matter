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
"""Sets up a Python 3 virtualenv for Pigweed."""

from __future__ import print_function

import contextlib
import datetime
import glob
import os
import platform
import re
import shutil
import subprocess
import sys
import stat
import tempfile

# Grabbing datetime string once so it will always be the same for all GnTarget
# objects.
_DATETIME_STRING = datetime.datetime.now().strftime('%Y%m%d-%H%M%S')


class GnTarget(object):  # pylint: disable=useless-object-inheritance
    def __init__(self, val):
        self.directory, self.target = val.split('#', 1)
        self.name = '-'.join(
            (re.sub(r'\W+', '_', self.target).strip('_'), _DATETIME_STRING)
        )


def git_stdout(*args, **kwargs):
    """Run git, passing args as git params and kwargs to subprocess."""
    return subprocess.check_output(['git'] + list(args), **kwargs).strip()


def git_repo_root(path='./'):
    """Find git repository root."""
    try:
        return git_stdout('-C', path, 'rev-parse', '--show-toplevel')
    except subprocess.CalledProcessError:
        return None


class GitRepoNotFound(Exception):
    """Git repository not found."""


def _installed_packages(venv_python):
    cmd = (venv_python, '-m', 'pip', 'list', '--disable-pip-version-check')
    output = subprocess.check_output(cmd).splitlines()
    return set(x.split()[0].lower() for x in output[2:])


def _required_packages(requirements):
    packages = set()

    for req in requirements:
        with open(req, 'r') as ins:
            for line in ins:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue
                packages.add(line.split('=')[0])

    return packages


def _check_call(args, **kwargs):
    stdout = kwargs.get('stdout', sys.stdout)

    with tempfile.TemporaryFile(mode='w+') as temp:
        try:
            kwargs['stdout'] = temp
            kwargs['stderr'] = subprocess.STDOUT
            print(args, kwargs, file=temp)
            subprocess.check_call(args, **kwargs)
        except subprocess.CalledProcessError:
            temp.seek(0)
            stdout.write(temp.read())
            raise


def _find_files_by_name(roots, name, allow_nesting=False):
    matches = []
    for root in roots:
        for dirpart, dirs, files in os.walk(root):
            if name in files:
                matches.append(os.path.join(dirpart, name))
                # If this directory is a match don't recurse inside it looking
                # for more matches.
                if not allow_nesting:
                    dirs[:] = []

            # Filter directories starting with . to avoid searching unnecessary
            # paths and finding files that should be hidden.
            dirs[:] = [d for d in dirs if not d.startswith('.')]
    return matches


def _check_venv(python, version, venv_path, pyvenv_cfg):
    if platform.system().lower() == 'windows':
        return

    # Check if the python location and version used for the existing virtualenv
    # is the same as the python we're using. If it doesn't match, we need to
    # delete the existing virtualenv and start again.
    if os.path.exists(pyvenv_cfg):
        pyvenv_values = {}
        with open(pyvenv_cfg, 'r') as ins:
            for line in ins:
                key, value = line.strip().split(' = ', 1)
                pyvenv_values[key] = value
        pydir = os.path.dirname(python)
        home = pyvenv_values.get('home')
        if pydir != home and not pydir.startswith(venv_path):
            shutil.rmtree(venv_path)
        elif pyvenv_values.get('version') not in version:
            shutil.rmtree(venv_path)


def _check_python_install_permissions(python):
    # These pickle files are not included on windows.
    # The path on windows is environment/cipd/packages/python/bin/Lib/lib2to3/
    if platform.system().lower() == 'windows':
        return

    # Make any existing lib2to3 pickle files read+write. This is needed for
    # importing yapf.
    lib2to3_path = os.path.join(
        os.path.dirname(os.path.dirname(python)), 'lib', 'python3.9', 'lib2to3'
    )
    pickle_file_paths = []
    if os.path.isdir(lib2to3_path):
        pickle_file_paths.extend(
            file_path
            for file_path in os.listdir(lib2to3_path)
            if '.pickle' in file_path
        )
    try:
        for pickle_file in pickle_file_paths:
            pickle_full_path = os.path.join(lib2to3_path, pickle_file)
            os.chmod(
                pickle_full_path, stat.S_IRUSR | stat.S_IWUSR | stat.S_IRGRP
            )
    except PermissionError:
        pass


def _flatten(*items):
    """Yields items from a series of items and nested iterables."""

    for item in items:
        if isinstance(item, (list, tuple)):
            for i in _flatten(*item):
                yield i
        else:
            yield item


def install(  # pylint: disable=too-many-arguments,too-many-locals
    project_root,
    venv_path,
    full_envsetup=True,
    requirements=None,
    constraints=None,
    gn_args=(),
    gn_targets=(),
    gn_out_dir=None,
    python=sys.executable,
    env=None,
    system_packages=False,
    use_pinned_pip_packages=True,
):
    """Creates a venv and installs all packages in this Git repo."""

    version = (
        subprocess.check_output((python, '--version'), stderr=subprocess.STDOUT)
        .strip()
        .decode()
    )
    if ' 3.' not in version:
        print('=' * 60, file=sys.stderr)
        print('Unexpected Python version:', version, file=sys.stderr)
        print('=' * 60, file=sys.stderr)
        return False

    # The bin/ directory is called Scripts/ on Windows. Don't ask.
    venv_bin = os.path.join(venv_path, 'Scripts' if os.name == 'nt' else 'bin')

    if env:
        env.set('VIRTUAL_ENV', venv_path)
        env.prepend('PATH', venv_bin)
        env.clear('PYTHONHOME')
        env.clear('__PYVENV_LAUNCHER__')
    else:
        env = contextlib.nullcontext()

    # Delete activation scripts. Typically they're created read-only and venv
    # will complain when trying to write over them fails.
    if os.path.isdir(venv_bin):
        for entry in os.listdir(venv_bin):
            if entry.lower().startswith('activate'):
                os.unlink(os.path.join(venv_bin, entry))

    pyvenv_cfg = os.path.join(venv_path, 'pyvenv.cfg')

    _check_python_install_permissions(python)
    _check_venv(python, version, venv_path, pyvenv_cfg)

    if full_envsetup or not os.path.exists(pyvenv_cfg):
        # On Mac sometimes the CIPD Python has __PYVENV_LAUNCHER__ set to
        # point to the system Python, which causes CIPD Python to create
        # virtualenvs that reference the system Python instead of the CIPD
        # Python. Clearing __PYVENV_LAUNCHER__ fixes that. See also pwbug/59.
        envcopy = os.environ.copy()
        if '__PYVENV_LAUNCHER__' in envcopy:
            del envcopy['__PYVENV_LAUNCHER__']

        # TODO(spang): Pass --upgrade-deps and remove pip & setuptools
        # upgrade below. This can only be done once the minimum python
        # version is at least 3.9.
        cmd = [python, '-m', 'venv', '--upgrade']
        cmd += ['--system-site-packages'] if system_packages else []
        cmd += [venv_path]
        _check_call(cmd, env=envcopy)

    venv_python = os.path.join(venv_bin, 'python')

    pw_root = os.environ.get('PW_ROOT')
    if not pw_root and env:
        pw_root = env.PW_ROOT
    if not pw_root:
        pw_root = git_repo_root()
    if not pw_root:
        raise GitRepoNotFound()

    # Sometimes we get an error saying "Egg-link ... does not match
    # installed location". This gets around that. The egg-link files
    # all come from 'pw'-prefixed packages we installed with --editable.
    # Source: https://stackoverflow.com/a/48972085
    for egg_link in glob.glob(
        os.path.join(venv_path, 'lib/python*/site-packages/*.egg-link')
    ):
        os.unlink(egg_link)

    def pip_install(*args):
        args = list(_flatten(args))
        with env():
            cmd = [venv_python, '-m', 'pip', 'install'] + args
            return _check_call(cmd)

    constraint_args = []
    if constraints:
        constraint_args.extend(
            '--constraint={}'.format(constraint) for constraint in constraints
        )

    pip_install(
        '--log',
        os.path.join(venv_path, 'pip-upgrade.log'),
        '--upgrade',
        'pip',
        'setuptools',
        'toml',  # Needed for pyproject.toml package installs.
        # Include wheel so pip installs can be done without build
        # isolation.
        'wheel',
        constraint_args,
    )

    # TODO(tonymd): Remove this when projects have defined requirements.
    if (not requirements) and constraints:
        requirements = constraints

    if requirements:
        requirement_args = []
        # Note: --no-build-isolation should be avoided for installing 3rd party
        # Python packages that use C/C++ extension modules.
        # https://setuptools.pypa.io/en/latest/userguide/ext_modules.html
        requirement_args.extend(
            '--requirement={}'.format(req) for req in requirements
        )
        combined_requirement_args = requirement_args + constraint_args
        pip_install(
            '--log',
            os.path.join(venv_path, 'pip-requirements.log'),
            combined_requirement_args,
        )

    def install_packages(gn_target):
        if gn_out_dir is None:
            build_dir = os.path.join(venv_path, 'gn-install-dir')
        else:
            build_dir = gn_out_dir

        env_log = 'env-{}.log'.format(gn_target.name)
        env_log_path = os.path.join(venv_path, env_log)
        with open(env_log_path, 'w') as outs:
            for key, value in sorted(os.environ.items()):
                if key.upper().endswith('PATH'):
                    print(key, '=', file=outs)
                    # pylint: disable=invalid-name
                    for v in value.split(os.pathsep):
                        print('   ', v, file=outs)
                    # pylint: enable=invalid-name
                else:
                    print(key, '=', value, file=outs)

        gn_log = 'gn-gen-{}.log'.format(gn_target.name)
        gn_log_path = os.path.join(venv_path, gn_log)
        try:
            with open(gn_log_path, 'w') as outs:
                gn_cmd = ['gn', 'gen', build_dir]

                args = list(gn_args)
                if not use_pinned_pip_packages:
                    args.append('pw_build_PIP_CONSTRAINTS=[]')

                args.append('dir_pigweed="{}"'.format(pw_root))
                gn_cmd.append('--args={}'.format(' '.join(args)))

                print(gn_cmd, file=outs)
                subprocess.check_call(
                    gn_cmd,
                    cwd=os.path.join(project_root, gn_target.directory),
                    stdout=outs,
                    stderr=outs,
                )
        except subprocess.CalledProcessError as err:
            with open(gn_log_path, 'r') as ins:
                raise subprocess.CalledProcessError(
                    err.returncode, err.cmd, ins.read()
                )

        ninja_log = 'ninja-{}.log'.format(gn_target.name)
        ninja_log_path = os.path.join(venv_path, ninja_log)
        try:
            with open(ninja_log_path, 'w') as outs:
                ninja_cmd = ['ninja', '-C', build_dir, '-v']
                ninja_cmd.append(gn_target.target)
                print(ninja_cmd, file=outs)
                subprocess.check_call(ninja_cmd, stdout=outs, stderr=outs)
        except subprocess.CalledProcessError as err:
            with open(ninja_log_path, 'r') as ins:
                raise subprocess.CalledProcessError(
                    err.returncode, err.cmd, ins.read()
                )

        with open(os.path.join(venv_path, 'pip-list.log'), 'w') as outs:
            subprocess.check_call(
                [venv_python, '-m', 'pip', 'list'],
                stdout=outs,
            )

    if gn_targets:
        with env():
            for gn_target in gn_targets:
                install_packages(gn_target)

    return True
