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
"""Install and check status of CRLSet and download chromium CRLSet."""

import os
import pathlib
import subprocess
from typing import Sequence
import pw_package.git_repo
import pw_package.package_manager


def crlset_tools_repo_path(path: pathlib.Path) -> pathlib.Path:
    return path / 'crlset-tools'


def crlset_exec_path(path: pathlib.Path) -> pathlib.Path:
    return path / 'crlset_exec'


def crlset_file_path(path: pathlib.Path) -> pathlib.Path:
    return path / 'crlset'


class CRLSet(pw_package.package_manager.Package):
    """Install and check status of CRLSet and downloaded CLRSet file."""

    def __init__(self, *args, **kwargs):
        super().__init__(*args, name='crlset', **kwargs)
        self._crlset_tools = pw_package.git_repo.GitRepo(
            name='crlset-tools',
            url='https://github.com/agl/crlset-tools.git',
            commit='1a1019bb500f93bc2b847a57cdbaede847649b99',
        )

    def status(self, path: pathlib.Path) -> bool:
        if not self._crlset_tools.status(crlset_tools_repo_path(path)):
            return False

        # The executable should have been built and exist.
        if not os.path.exists(crlset_exec_path(path)):
            return False

        # A crlset has been downloaded
        if not os.path.exists(crlset_file_path(path)):
            return False

        return True

    def install(self, path: pathlib.Path) -> None:
        self._crlset_tools.install(crlset_tools_repo_path(path))

        # Build the go tool
        subprocess.run(
            ['go', 'build', '-o', crlset_exec_path(path), 'crlset.go'],
            check=True,
            cwd=crlset_tools_repo_path(path),
        )

        crlset_tools_exec = crlset_exec_path(path)
        if not os.path.exists(crlset_tools_exec):
            raise FileNotFoundError('Fail to find crlset executable')

        # Download the latest CRLSet with the go tool
        with open(crlset_file_path(path), 'wb') as crlset_file:
            fetched = subprocess.run(
                [crlset_exec_path(path), 'fetch'],
                capture_output=True,
                check=True,
            ).stdout
            crlset_file.write(fetched)

    def info(self, path: pathlib.Path) -> Sequence[str]:
        return (
            f'{self.name} installed in: {path}',
            "Enable by running 'gn args out' and adding this line:",
            f'  pw_tls_client_CRLSET_PATH = "{crlset_file_path(path)}"',
        )


pw_package.package_manager.register(CRLSet)
