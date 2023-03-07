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
"""Install and remove optional packages."""

import argparse
import dataclasses
import logging
import os
import pathlib
import shutil
from typing import Dict, List, Optional, Sequence, Tuple

_LOG: logging.Logger = logging.getLogger(__name__)


class Package:
    """Package to be installed.

    Subclass this to implement installation of a specific package.
    """

    def __init__(self, name):
        self._name = name

    @property
    def name(self):
        return self._name

    def install(
        self, path: pathlib.Path
    ) -> None:  # pylint: disable=no-self-use
        """Install the package at path.

        Install the package in path. Cannot assume this directory is empty—it
        may need to be deleted or updated.
        """

    def remove(self, path: pathlib.Path) -> None:  # pylint: disable=no-self-use
        """Remove the package from path.

        Removes the directory containing the package. For most packages this
        should be sufficient to remove the package, and subclasses should not
        need to override this package.
        """
        if os.path.exists(path):
            shutil.rmtree(path)

    def status(self, path: pathlib.Path) -> bool:  # pylint: disable=no-self-use
        """Returns if package is installed at path and current.

        This method will be skipped if the directory does not exist.
        """

    def info(
        self, path: pathlib.Path
    ) -> Sequence[str]:  # pylint: disable=no-self-use
        """Returns a short string explaining how to enable the package."""


_PACKAGES: Dict[str, Package] = {}


def register(package_class: type, *args, **kwargs) -> None:
    obj = package_class(*args, **kwargs)
    _PACKAGES[obj.name] = obj


@dataclasses.dataclass
class Packages:
    all: Tuple[str, ...]
    installed: Tuple[str, ...]
    available: Tuple[str, ...]


class PackageManager:
    """Install and remove optional packages."""

    def __init__(self, root: pathlib.Path):
        self._pkg_root = root
        os.makedirs(root, exist_ok=True)

    def install(self, package: str, force: bool = False) -> None:
        pkg = _PACKAGES[package]
        if force:
            self.remove(package)
        pkg.install(self._pkg_root / pkg.name)

    def remove(self, package: str) -> None:
        pkg = _PACKAGES[package]
        pkg.remove(self._pkg_root / pkg.name)

    def status(self, package: str) -> bool:
        pkg = _PACKAGES[package]
        path = self._pkg_root / pkg.name
        return os.path.isdir(path) and pkg.status(path)

    def list(self) -> Packages:
        installed = []
        available = []
        for package in sorted(_PACKAGES.keys()):
            pkg = _PACKAGES[package]
            if pkg.status(self._pkg_root / pkg.name):
                installed.append(pkg.name)
            else:
                available.append(pkg.name)

        return Packages(
            all=tuple(_PACKAGES.keys()),
            installed=tuple(installed),
            available=tuple(available),
        )

    def info(self, package: str) -> Sequence[str]:
        pkg = _PACKAGES[package]
        return pkg.info(self._pkg_root / pkg.name)


class PackageManagerCLI:
    """Command-line interface to PackageManager."""

    def __init__(self):
        self._mgr: PackageManager = None

    def install(self, package: str, force: bool = False) -> int:
        _LOG.info('Installing %s...', package)
        self._mgr.install(package, force)
        _LOG.info('Installing %s...done.', package)
        for line in self._mgr.info(package):
            _LOG.info('%s', line)
        return 0

    def remove(self, package: str) -> int:
        _LOG.info('Removing %s...', package)
        self._mgr.remove(package)
        _LOG.info('Removing %s...done.', package)
        return 0

    def status(self, package: str) -> int:
        if self._mgr.status(package):
            _LOG.info('%s is installed.', package)
            for line in self._mgr.info(package):
                _LOG.info('%s', line)
            return 0

        _LOG.info('%s is not installed.', package)
        return -1

    def list(self) -> int:
        packages = self._mgr.list()

        _LOG.info('Installed packages:')
        for package in packages.installed:
            _LOG.info('  %s', package)
            for line in self._mgr.info(package):
                _LOG.info('    %s', line)
        _LOG.info('')

        _LOG.info('Available packages:')
        for package in packages.available:
            _LOG.info('  %s', package)
        _LOG.info('')

        return 0

    def run(self, command: str, pkg_root: pathlib.Path, **kwargs) -> int:
        self._mgr = PackageManager(pkg_root.resolve())
        return getattr(self, command)(**kwargs)


def parse_args(argv: Optional[List[str]] = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser("Manage packages.")
    parser.add_argument(
        '--package-root',
        '-e',
        dest='pkg_root',
        type=pathlib.Path,
        default=pathlib.Path(os.environ['PW_PACKAGE_ROOT']),
    )
    subparsers = parser.add_subparsers(dest='command', required=True)
    install = subparsers.add_parser('install')
    install.add_argument('--force', '-f', action='store_true')
    remove = subparsers.add_parser('remove')
    status = subparsers.add_parser('status')
    for cmd in (install, remove, status):
        cmd.add_argument('package', choices=_PACKAGES.keys())
    _ = subparsers.add_parser('list')
    return parser.parse_args(argv)


def run(**kwargs):
    return PackageManagerCLI().run(**kwargs)
