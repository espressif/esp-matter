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
"""Installs or updates prebuilt tools.

Must be tested with Python 2 and Python 3.

The stdout of this script is meant to be executed by the invoking shell.
"""

from __future__ import print_function

import collections
import hashlib
import json
import os
import platform as platform_module
import re
import subprocess
import sys


def check_auth(cipd, package_files, cipd_service_account, spin):
    """Check have access to CIPD pigweed directory."""
    cmd = [cipd]
    extra_args = []
    if cipd_service_account:
        extra_args.extend(['-service-account-json', cipd_service_account])

    paths = []
    for package_file in package_files:
        with open(package_file, 'r') as ins:
            # This is an expensive RPC, so only check the first few entries
            # in each file.
            for i, entry in enumerate(json.load(ins).get('packages', ())):
                if i >= 3:
                    break
                parts = entry['path'].split('/')
                while '${' in parts[-1]:
                    parts.pop(-1)
                paths.append('/'.join(parts))

    username = None
    try:
        output = subprocess.check_output(
            cmd + ['auth-info'] + extra_args, stderr=subprocess.STDOUT
        ).decode()
        logged_in = True

        match = re.search(r'Logged in as (\S*)\.', output)
        if match:
            username = match.group(1)

    except subprocess.CalledProcessError:
        logged_in = False

    def _check_all_paths():
        inaccessible_paths = []

        for path in paths:
            # Not catching CalledProcessError because 'cipd ls' seems to never
            # return an error code unless it can't reach the CIPD server.
            output = subprocess.check_output(
                cmd + ['ls', path] + extra_args, stderr=subprocess.STDOUT
            ).decode()
            if 'No matching packages' not in output:
                continue

            # 'cipd ls' only lists sub-packages but ignores any packages at the
            # given path. 'cipd instances' will give versions of that package.
            # 'cipd instances' does use an error code if there's no such package
            # or that package is inaccessible.
            try:
                subprocess.check_output(
                    cmd + ['instances', path] + extra_args,
                    stderr=subprocess.STDOUT,
                )
            except subprocess.CalledProcessError:
                inaccessible_paths.append(path)

        return inaccessible_paths

    inaccessible_paths = _check_all_paths()

    if inaccessible_paths and not logged_in:
        with spin.pause():
            stderr = lambda *args: print(*args, file=sys.stderr)
            stderr()
            stderr(
                'Not logged in to CIPD and no anonymous access to the '
                'following CIPD paths:'
            )
            for path in inaccessible_paths:
                stderr('  {}'.format(path))
            stderr()
            stderr('Attempting CIPD login')
            try:
                # Note that with -service-account-json, auth-login is a no-op.
                subprocess.check_call(cmd + ['auth-login'] + extra_args)
            except subprocess.CalledProcessError:
                stderr('CIPD login failed')
                return False

        inaccessible_paths = _check_all_paths()

    if inaccessible_paths:
        stderr = lambda *args: print(*args, file=sys.stderr)
        stderr('=' * 60)
        username_part = ''
        if username:
            username_part = '({}) '.format(username)
        stderr(
            'Your account {}does not have access to the following '
            'paths'.format(username_part)
        )
        stderr('(or they do not exist)')
        for path in inaccessible_paths:
            stderr('  {}'.format(path))
        stderr('=' * 60)
        return False

    return True


def platform(rosetta=None):
    """Return the CIPD platform string of the current system."""
    # If running inside a bootstrapped environment we can use the env var.
    # Otherwise, require rosetta be set.
    if rosetta is None:
        rosetta = os.environ['_PW_ROSETTA']

    osname = {
        'darwin': 'mac',
        'linux': 'linux',
        'windows': 'windows',
    }[platform_module.system().lower()]

    if platform_module.machine().startswith(('aarch64', 'armv8')):
        arch = 'arm64'
    elif platform_module.machine() == 'x86_64':
        arch = 'amd64'
    elif platform_module.machine() == 'i686':
        arch = 'i386'
    else:
        arch = platform_module.machine()

    platform_arch = '{}-{}'.format(osname, arch).lower()

    # Support `mac-arm64` through Rosetta until `mac-arm64` binaries are ready
    if platform_arch == 'mac-arm64' and rosetta:
        return 'mac-amd64'

    return platform_arch


def all_package_files(env_vars, package_files):
    """Recursively retrieve all package files."""

    result = []
    to_process = []
    for pkg_file in package_files:
        args = []
        if env_vars:
            args.append(env_vars.get('PW_PROJECT_ROOT'))
        args.append(pkg_file)

        # The signature here is os.path.join(a, *p). Pylint doesn't like when
        # we call os.path.join(*args), but is happy if we instead call
        # os.path.join(args[0], *args[1:]). Disabling the option on this line
        # seems to be a less confusing choice.
        path = os.path.join(*args)  # pylint: disable=no-value-for-parameter

        to_process.append(path)

    while to_process:
        package_file = to_process.pop(0)
        result.append(package_file)

        with open(package_file, 'r') as ins:
            entries = json.load(ins).get('included_files', ())

        for entry in entries:
            entry = os.path.join(os.path.dirname(package_file), entry)

            if entry not in result and entry not in to_process:
                to_process.append(entry)

    return result


def all_packages(package_files):
    packages = []
    for package_file in package_files:
        name = package_file_name(package_file)
        with open(package_file, 'r') as ins:
            file_packages = json.load(ins).get('packages', ())
            for package in file_packages:
                if 'subdir' in package:
                    package['subdir'] = os.path.join(name, package['subdir'])
                else:
                    package['subdir'] = name
            packages.extend(file_packages)
    return packages


def deduplicate_packages(packages):
    deduped = collections.OrderedDict()
    for package in reversed(packages):
        if package['path'] in deduped:
            del deduped[package['path']]
        deduped[package['path']] = package
    return reversed(list(deduped.values()))


def write_ensure_file(
    package_files, ensure_file, platform
):  # pylint: disable=redefined-outer-name
    packages = all_packages(package_files)
    deduped_packages = deduplicate_packages(packages)

    with open(ensure_file, 'w') as outs:
        outs.write(
            '$VerifiedPlatform linux-amd64\n'
            '$VerifiedPlatform mac-amd64\n'
            '$ParanoidMode CheckPresence\n'
        )

        for pkg in deduped_packages:
            # If this is a new-style package manifest platform handling must
            # be done here instead of by the cipd executable.
            if 'platforms' in pkg and platform not in pkg['platforms']:
                continue

            outs.write('@Subdir {}\n'.format(pkg.get('subdir', '')))
            outs.write('{} {}\n'.format(pkg['path'], ' '.join(pkg['tags'])))


def package_file_name(package_file):
    return os.path.basename(os.path.splitext(package_file)[0])


def package_installation_path(root_install_dir, package_file):
    """Returns the package installation path.

    Args:
      root_install_dir: The CIPD installation directory.
      package_file: The path to the .json package definition file.
    """
    return os.path.join(
        root_install_dir, 'packages', package_file_name(package_file)
    )


def update(  # pylint: disable=too-many-locals
    cipd,
    package_files,
    root_install_dir,
    cache_dir,
    rosetta=False,
    env_vars=None,
    spin=None,
    trust_hash=False,
):
    """Grab the tools listed in ensure_files."""

    package_files = all_package_files(env_vars, package_files)

    # TODO(mohrr) use os.makedirs(..., exist_ok=True).
    if not os.path.isdir(root_install_dir):
        os.makedirs(root_install_dir)

    # This file is read by 'pw doctor' which needs to know which package files
    # were used in the environment.
    package_files_file = os.path.join(
        root_install_dir, '_all_package_files.json'
    )
    with open(package_files_file, 'w') as outs:
        json.dump(package_files, outs, indent=2)

    if env_vars:
        env_vars.prepend('PATH', root_install_dir)
        env_vars.set('PW_CIPD_INSTALL_DIR', root_install_dir)
        env_vars.set('CIPD_CACHE_DIR', cache_dir)

    pw_root = None

    if env_vars:
        pw_root = env_vars.get('PW_ROOT', None)
    if not pw_root:
        pw_root = os.environ['PW_ROOT']

    plat = platform(rosetta)

    ensure_file = os.path.join(root_install_dir, 'packages.ensure')
    write_ensure_file(package_files, ensure_file, plat)

    install_dir = os.path.join(root_install_dir, 'packages')

    cmd = [
        cipd,
        'ensure',
        '-ensure-file',
        ensure_file,
        '-root',
        install_dir,
        '-log-level',
        'debug',
        '-json-output',
        os.path.join(root_install_dir, 'packages.json'),
        '-cache-dir',
        cache_dir,
        '-max-threads',
        '0',  # 0 means use CPU count.
    ]

    cipd_service_account = None
    if env_vars:
        cipd_service_account = env_vars.get('PW_CIPD_SERVICE_ACCOUNT_JSON')
    if not cipd_service_account:
        cipd_service_account = os.environ.get('PW_CIPD_SERVICE_ACCOUNT_JSON')
    if cipd_service_account:
        cmd.extend(['-service-account-json', cipd_service_account])

    hasher = hashlib.sha256()
    encoded = '\0'.join(cmd)
    if hasattr(encoded, 'encode'):
        encoded = encoded.encode()
    hasher.update(encoded)
    with open(ensure_file, 'rb') as ins:
        hasher.update(ins.read())
    digest = hasher.hexdigest()

    with open(os.path.join(root_install_dir, 'hash.log'), 'w') as hashlog:
        print('calculated digest:', digest, file=hashlog)

        hash_file = os.path.join(root_install_dir, 'packages.sha256')
        print('hash file path:', hash_file, file=hashlog)
        print('exists:', os.path.isfile(hash_file), file=hashlog)
        print('trust_hash:', trust_hash, file=hashlog)
        if trust_hash and os.path.isfile(hash_file):
            with open(hash_file, 'r') as ins:
                digest_file = ins.read().strip()
                print('contents:', digest_file, file=hashlog)
                print('equal:', digest == digest_file, file=hashlog)
                if digest == digest_file:
                    return True

    if not check_auth(cipd, package_files, cipd_service_account, spin):
        return False

    log = os.path.join(root_install_dir, 'packages.log')
    try:
        with open(log, 'w') as outs:
            print(*cmd, file=outs)
            subprocess.check_call(cmd, stdout=outs, stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError:
        with open(log, 'r') as ins:
            sys.stderr.write(ins.read())
            raise

    with open(hash_file, 'w') as outs:
        print(digest, file=outs)

    # Set environment variables so tools can later find things under, for
    # example, 'share'.
    if env_vars:
        for package_file in reversed(package_files):
            name = package_file_name(package_file)
            file_install_dir = os.path.join(install_dir, name)
            # Some executables get installed at top-level and some get
            # installed under 'bin'. A small number of old packages prefix the
            # entire tree with the platform (e.g., chromium/third_party/tcl).
            for bin_dir in (
                file_install_dir,
                os.path.join(file_install_dir, 'bin'),
                os.path.join(file_install_dir, plat, 'bin'),
            ):
                if os.path.isdir(bin_dir):
                    env_vars.prepend('PATH', bin_dir)
            env_vars.set(
                'PW_{}_CIPD_INSTALL_DIR'.format(name.upper()), file_install_dir
            )

            # Windows has its own special toolchain.
            if os.name == 'nt':
                env_vars.prepend(
                    'PATH', os.path.join(file_install_dir, 'mingw64', 'bin')
                )

    return True
