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
"""Copies files from CIPD to a specified directory.

By default, Pigweed installs packages from a manifest file to a CIPD
subdirectory as part of environment setup. This script will copy files from this
directory into a specified output directory.

Here's an example of how to use this script:

Let's say you have a package with a static library:

CIPD path: `pigweed/third_party/libsomething`
Files:
  ./libsomething/include/something.h
  ./libsomething/libsomething.a

And this package was referenced in my_project_packages.json, which was provided
as a --cipd-package-file in your bootstrap script.

To copy the static libraryto $PW_PROJECT_ROOT/static_libraries, you'd have an
invocation something like this:

copy_from_cipd --package-name=pigweed/third_party/libsomething \
               --mainfest=$PW_PROJECT_ROOT/tools/my_project_packages.json \
               --file=libsomething/libsomething.a \
               --out=$PW_PROJECT_ROOT/static_libraries
"""

import argparse
import json
import logging
import os
import shutil
import subprocess
import sys
from pathlib import Path

import pw_env_setup.cipd_setup.update

logger = logging.getLogger(__name__)


def parse_args():
    """Parse arguments."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        '--verbose', '-v', help='Verbose output', action='store_true'
    )
    parser.add_argument(
        '--manifest',
        required=True,
        type=Path,
        help='Path to CIPD JSON manifest file',
    )
    parser.add_argument(
        '--out-dir',
        type=Path,
        default='.',
        help='Output folder to copy the specified file to',
    )
    parser.add_argument(
        '--package-name', required=True, help='The CIPD package name'
    )
    # TODO(pwbug/334) Support multiple values for --file.
    parser.add_argument(
        '--file',
        required=True,
        type=Path,
        help='Path of the file to copy from the CIPD package. '
        'This is relative to the CIPD package root of the '
        'provided manifest.',
    )
    parser.add_argument(
        '--cipd-package-root',
        type=Path,
        help="Path to the root of the package's install "
        'directory. This is usually at '
        'PW_{manifest name}_CIPD_INSTALL_DIR',
    )
    return parser.parse_args()


def check_version(manifest, cipd_path, package_name):
    base_package_name = os.path.basename(package_name)
    instance_id_path = os.path.join(
        cipd_path, '.versions', f'{base_package_name}.cipd_version'
    )
    with open(instance_id_path, 'r') as ins:
        instance_id = json.load(ins)['instance_id']

    with open(manifest, 'r') as ins:
        data = json.load(ins)['packages']

    path = None
    expected_version = None
    for entry in data:
        if package_name in entry['path']:
            path = entry['path']
            expected_version = entry['tags'][0]
    if not path:
        raise LookupError(f'failed to find {package_name} entry')

    cmd = ['cipd', 'describe', path, '-version', instance_id]
    output = subprocess.check_output(cmd).decode()
    if expected_version not in output:
        pw_env_setup.cipd_setup.update.update(
            'cipd',
            (manifest,),
            os.environ['PW_CIPD_INSTALL_DIR'],
            os.environ['CIPD_CACHE_DIR'],
        )


def main():
    args = parse_args()

    if args.verbose:
        logger.setLevel(logging.DEBUG)

    # Try to infer CIPD install root from the manifest name.
    if args.cipd_package_root is None:
        file_base_name = args.manifest.stem
        args.cipd_var = 'PW_{}_CIPD_INSTALL_DIR'.format(file_base_name.upper())
        try:
            args.cipd_package_root = os.environ[args.cipd_var]
        except KeyError:
            logger.error(
                "The %s environment variable isn't set. Did you forget to run "
                '`. ./bootstrap.sh`? Is the %s manifest installed to a '
                'different path?',
                args.cipd_var,
                file_base_name,
            )
            sys.exit(1)

    check_version(args.manifest, args.cipd_package_root, args.package_name)

    shutil.copyfile(
        os.path.join(args.cipd_package_root, args.file),
        os.path.join(args.out_dir, args.file),
    )


if __name__ == '__main__':
    logging.basicConfig()
    main()
