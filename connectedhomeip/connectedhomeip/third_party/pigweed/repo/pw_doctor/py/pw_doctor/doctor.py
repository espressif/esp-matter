#!/usr/bin/env python3
# Copyright 2019 The Pigweed Authors
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
"""Checks if the environment is set up correctly for Pigweed."""

import argparse
from concurrent import futures
import logging
import json
import os
import pathlib
import shutil
import subprocess
import sys
import tempfile
from typing import Callable, Iterable, List, Optional, Set

import pw_cli.pw_command_plugins
import pw_env_setup.cipd_setup.update as cipd_update


def call_stdout(*args, **kwargs):
    kwargs.update(stdout=subprocess.PIPE)
    proc = subprocess.run(*args, **kwargs)
    return proc.stdout.decode('utf-8')


class _Fatal(Exception):
    pass


class Doctor:
    def __init__(
        self, *, log: Optional[logging.Logger] = None, strict: bool = False
    ):
        self.strict = strict
        self.log = log or logging.getLogger(__name__)
        self.failures: Set[str] = set()

    def run(self, checks: Iterable[Callable]):
        with futures.ThreadPoolExecutor() as executor:
            futures.wait(
                [executor.submit(self._run_check, c, executor) for c in checks]
            )

    def _run_check(self, check, executor):
        ctx = DoctorContext(self, check.__name__, executor)
        try:
            self.log.debug('Running check %s', ctx.check)
            check(ctx)
            ctx.wait()
        except _Fatal:
            pass
        except:  # pylint: disable=bare-except
            self.failures.add(ctx.check)
            self.log.exception(
                '%s failed with an unexpected exception', check.__name__
            )

        self.log.debug('Completed check %s', ctx.check)


class DoctorContext:
    """The context object provided to each context function."""

    def __init__(self, doctor: Doctor, check: str, executor: futures.Executor):
        self._doctor = doctor
        self.check = check
        self._executor = executor
        self._futures: List[futures.Future] = []

    def submit(self, function, *args, **kwargs):
        """Starts running the provided function in parallel."""
        self._futures.append(
            self._executor.submit(self._run_job, function, *args, **kwargs)
        )

    def wait(self):
        """Waits for all parallel tasks started with submit() to complete."""
        futures.wait(self._futures)
        self._futures.clear()

    def _run_job(self, function, *args, **kwargs):
        try:
            function(*args, **kwargs)
        except _Fatal:
            pass
        except:  # pylint: disable=bare-except
            self._doctor.failures.add(self.check)
            self._doctor.log.exception(
                '%s failed with an unexpected exception', self.check
            )

    def fatal(self, fmt, *args, **kwargs):
        """Same as error() but terminates the check early."""
        self.error(fmt, *args, **kwargs)
        raise _Fatal()

    def error(self, fmt, *args, **kwargs):
        self._doctor.log.error(fmt, *args, **kwargs)
        self._doctor.failures.add(self.check)

    def warning(self, fmt, *args, **kwargs):
        if self._doctor.strict:
            self.error(fmt, *args, **kwargs)
        else:
            self._doctor.log.warning(fmt, *args, **kwargs)

    def info(self, fmt, *args, **kwargs):
        self._doctor.log.info(fmt, *args, **kwargs)

    def debug(self, fmt, *args, **kwargs):
        self._doctor.log.debug(fmt, *args, **kwargs)


def register_into(dest):
    def decorate(func):
        dest.append(func)
        return func

    return decorate


CHECKS: List[Callable] = []


@register_into(CHECKS)
def pw_plugins(ctx: DoctorContext):
    if pw_cli.pw_command_plugins.errors():
        ctx.error('Not all pw plugins loaded successfully')


def unames_are_equivalent(uname_actual: str, uname_expected: str) -> bool:
    """Determine if uname values are equivalent for this tool's purposes."""

    # Support `mac-arm64` through Rosetta until `mac-arm64` binaries are ready
    # Expected and actual unames will not literally match on M1 Macs because
    # they pretend to be Intel Macs for the purpose of environment setup. But
    # that's intentional and doesn't require any user action.
    if "Darwin" in uname_expected and "arm64" in uname_expected:
        uname_expected = uname_expected.replace("arm64", "x86_64")

    return uname_actual == uname_expected


@register_into(CHECKS)
def env_os(ctx: DoctorContext):
    """Check that the environment matches this machine."""
    if '_PW_ACTUAL_ENVIRONMENT_ROOT' not in os.environ:
        return
    env_root = pathlib.Path(os.environ['_PW_ACTUAL_ENVIRONMENT_ROOT'])
    config = env_root / 'config.json'
    if not config.is_file():
        return

    with open(config, 'r') as ins:
        data = json.load(ins)
    if data['os'] != os.name:
        ctx.error(
            'Current OS (%s) does not match bootstrapped OS (%s)',
            os.name,
            data['os'],
        )

    # Skipping sysname and nodename in os.uname(). nodename could change
    # based on the current network. sysname won't change, but is
    # redundant because it's contained in release or version, and
    # skipping it here simplifies logic.
    uname = ' '.join(getattr(os, 'uname', lambda: ())()[2:])
    if not unames_are_equivalent(uname, data['uname']):
        ctx.warning(
            'Current uname (%s) does not match Bootstrap uname (%s), '
            'you may need to rerun bootstrap on this system',
            uname,
            data['uname'],
        )


@register_into(CHECKS)
def pw_root(ctx: DoctorContext):
    """Check that environment variable PW_ROOT is set and makes sense."""
    try:
        root = pathlib.Path(os.environ['PW_ROOT']).resolve()
    except KeyError:
        ctx.fatal('PW_ROOT not set')

    # If pigweed is intentionally vendored and not in a git repo or submodule,
    # set PW_DISABLE_ROOT_GIT_REPO_CHECK=1 during bootstrap to suppress the
    # following check.
    if os.environ.get('PW_DISABLE_ROOT_GIT_REPO_CHECK', '0') == '1':
        return

    git_root = pathlib.Path(
        call_stdout(['git', 'rev-parse', '--show-toplevel'], cwd=root).strip()
    )
    git_root = git_root.resolve()
    if root != git_root:
        ctx.error(
            'PW_ROOT (%s) != `git rev-parse --show-toplevel` (%s)',
            root,
            git_root,
        )


@register_into(CHECKS)
def git_hook(ctx: DoctorContext):
    """Check that presubmit git hook is installed."""
    if not os.environ.get('PW_ENABLE_PRESUBMIT_HOOK_WARNING'):
        return

    try:
        root = pathlib.Path(os.environ['PW_ROOT'])
    except KeyError:
        return  # This case is handled elsewhere.

    hook = root / '.git' / 'hooks' / 'pre-push'
    if not os.path.isfile(hook):
        ctx.info(
            'Presubmit hook not installed, please run '
            "'pw presubmit --install' before pushing changes."
        )


@register_into(CHECKS)
def python_version(ctx: DoctorContext):
    """Check the Python version is correct."""
    actual = sys.version_info
    expected = (3, 8)
    if actual[0:2] < expected or actual[0] != expected[0]:
        # If we get the wrong version but it still came from CIPD print a
        # warning but give it a pass.
        if 'chromium' in sys.version:
            ctx.warning(
                'Python %d.%d.x expected, got Python %d.%d.%d',
                *expected,
                *actual[0:3],
            )
        else:
            ctx.error(
                'Python %d.%d.x required, got Python %d.%d.%d',
                *expected,
                *actual[0:3],
            )


@register_into(CHECKS)
def virtualenv(ctx: DoctorContext):
    """Check that we're in the correct virtualenv."""
    try:
        venv_path = pathlib.Path(os.environ['VIRTUAL_ENV']).resolve()
    except KeyError:
        ctx.error('VIRTUAL_ENV not set')
        return

    # When running in LUCI we might not have gone through the normal environment
    # setup process, so we need to skip the rest of this step.
    if 'LUCI_CONTEXT' in os.environ:
        return

    var = 'PW_ROOT'
    if '_PW_ACTUAL_ENVIRONMENT_ROOT' in os.environ:
        var = '_PW_ACTUAL_ENVIRONMENT_ROOT'
    root = pathlib.Path(os.environ[var]).resolve()

    if root not in venv_path.parents:
        ctx.error('VIRTUAL_ENV (%s) not inside %s (%s)', venv_path, var, root)
        ctx.error('\n'.join(os.environ.keys()))


@register_into(CHECKS)
def cipd(ctx: DoctorContext):
    """Check cipd is set up correctly and in use."""
    if os.environ.get('PW_DOCTOR_SKIP_CIPD_CHECKS'):
        return

    cipd_path = 'pigweed'

    cipd_exe = shutil.which('cipd')
    if not cipd_exe:
        ctx.fatal('cipd not in PATH (%s)', os.environ['PATH'])

    temp = tempfile.NamedTemporaryFile(prefix='cipd', delete=False)
    subprocess.run(
        ['cipd', 'acl-check', '-json-output', temp.name, cipd_path],
        stdout=subprocess.PIPE,
    )
    if not json.load(temp)['result']:
        ctx.fatal(
            "can't access %s CIPD directory, have you run "
            "'cipd auth-login'?",
            cipd_path,
        )

    commands_expected_from_cipd = [
        'arm-none-eabi-gcc',
        'gn',
        'ninja',
        'protoc',
    ]

    # TODO(mohrr) get these tools in CIPD for Windows.
    if os.name == 'posix':
        commands_expected_from_cipd += [
            'bloaty',
            'clang++',
        ]

    for command in commands_expected_from_cipd:
        path = shutil.which(command)
        if path is None:
            ctx.error(
                'could not find %s in PATH (%s)', command, os.environ['PATH']
            )
        elif 'cipd' not in path:
            ctx.warning(
                'not using %s from cipd, got %s (path is %s)',
                command,
                path,
                os.environ['PATH'],
            )


@register_into(CHECKS)
def cipd_versions(ctx: DoctorContext):
    """Check cipd tool versions are current."""

    if os.environ.get('PW_DOCTOR_SKIP_CIPD_CHECKS'):
        return

    if 'PW_CIPD_INSTALL_DIR' not in os.environ:
        ctx.error('PW_CIPD_INSTALL_DIR not set')
    cipd_dir = pathlib.Path(os.environ['PW_CIPD_INSTALL_DIR'])

    with open(cipd_dir / '_all_package_files.json', 'r') as ins:
        json_paths = [pathlib.Path(x) for x in json.load(ins)]

    platform = cipd_update.platform()

    def check_cipd(package, install_path):
        if platform not in package['platforms']:
            ctx.debug(
                "skipping %s because it doesn't apply to %s",
                package['path'],
                platform,
            )
            return

        tags_without_refs = [x for x in package['tags'] if ':' in x]
        if not tags_without_refs:
            ctx.debug(
                'skipping %s because it tracks a ref, not a tag (%s)',
                package['path'],
                ', '.join(package['tags']),
            )
            return

        ctx.debug('checking version of %s', package['path'])

        name = [part for part in package['path'].split('/') if '{' not in part][
            -1
        ]

        # If the exact path is specified in the JSON file use it, and require it
        # exist.
        if 'version_file' in package:
            path = install_path / package['version_file']
            notify_method = ctx.error
        # Otherwise, follow a heuristic to find the file but don't require the
        # file to exist.
        else:
            path = install_path / '.versions' / f'{name}.cipd_version'
            notify_method = ctx.debug

        # Check if a .exe cipd_version exists on Windows.
        path_windows = install_path / '.versions' / f'{name}.exe.cipd_version'
        if os.name == 'nt' and path_windows.is_file():
            path = path_windows

        if not path.is_file():
            notify_method(f'no version file for {name} at {path}')
            return

        with path.open() as ins:
            installed = json.load(ins)
        ctx.debug(f'found version file for {name} at {path}')

        describe = (
            'cipd',
            'describe',
            installed['package_name'],
            '-version',
            installed['instance_id'],
        )
        ctx.debug('%s', ' '.join(describe))
        output_raw = subprocess.check_output(describe).decode()
        ctx.debug('output: %r', output_raw)
        output = output_raw.split()

        for tag in package['tags']:
            if tag not in output:
                ctx.error(
                    'CIPD package %s in %s is out of date, please rerun '
                    'bootstrap',
                    installed['package_name'],
                    install_path,
                )

            else:
                ctx.debug(
                    'CIPD package %s in %s is current',
                    installed['package_name'],
                    install_path,
                )

    deduped_packages = cipd_update.deduplicate_packages(
        cipd_update.all_packages(json_paths)
    )
    for json_path in json_paths:
        ctx.debug(f'Checking packages in {json_path}')
        if not json_path.exists():
            ctx.error(
                'CIPD package file %s may have been deleted, please '
                'rerun bootstrap',
                json_path,
            )
            continue

        install_path = pathlib.Path(
            cipd_update.package_installation_path(cipd_dir, json_path)
        )
        for package in json.loads(json_path.read_text()).get('packages', ()):
            if package not in deduped_packages:
                ctx.debug(
                    f'Skipping overridden package {package["path"]} '
                    f'with tag(s) {package["tags"]}'
                )
                continue
            ctx.submit(check_cipd, package, install_path)


@register_into(CHECKS)
def symlinks(ctx: DoctorContext):
    """Check that the platform supports symlinks."""

    try:
        root = pathlib.Path(os.environ['PW_ROOT']).resolve()
    except KeyError:
        return  # This case is handled elsewhere.

    with tempfile.TemporaryDirectory() as tmpdir:
        dest = pathlib.Path(tmpdir).resolve() / 'symlink'
        try:
            os.symlink(root, dest)
            failure = False
        except OSError:
            # TODO(pwbug/500) Find out what errno is set when symlinks aren't
            # supported by the OS.
            failure = True

        if not os.path.islink(dest) or failure:
            ctx.warning(
                'Symlinks are not supported or current user does not have '
                'permission to use them. This may cause build issues. If on '
                'Windows, turn on Development Mode to enable symlink support.'
            )


def run_doctor(strict=False, checks=None):
    """Run all the Check subclasses defined in this file."""

    if checks is None:
        checks = tuple(CHECKS)

    doctor = Doctor(strict=strict)
    doctor.log.debug('Doctor running %d checks...', len(checks))

    doctor.run(checks)

    if doctor.failures:
        doctor.log.info('Failed checks: %s', ', '.join(doctor.failures))
        doctor.log.info(
            "Your environment setup has completed, but something isn't right "
            'and some things may not work correctly. You may continue with '
            'development, but please seek support at '
            'https://issues.pigweed.dev/new or by reaching out to your team.'
        )
    else:
        doctor.log.info('Environment passes all checks!')
    return len(doctor.failures)


def main() -> int:
    """Check that the environment is set up correctly for Pigweed."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        '--strict',
        action='store_true',
        help='Run additional checks.',
    )

    return run_doctor(**vars(parser.parse_args()))


if __name__ == '__main__':
    # By default, display log messages like a simple print statement.
    logging.basicConfig(format='%(message)s', level=logging.INFO)
    sys.exit(main())
