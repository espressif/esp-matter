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
"""Install and check status of BoringSSL + Chromium verifier."""

import os
import pathlib
import subprocess
from typing import Sequence
import pw_package.git_repo
import pw_package.package_manager


def boringssl_repo_path(path: pathlib.Path) -> pathlib.Path:
    return path / 'src'


class BoringSSL(pw_package.package_manager.Package):
    """Install and check status of BoringSSL and chromium verifier."""

    def __init__(self, *args, **kwargs):
        super().__init__(*args, name='boringssl', **kwargs)
        self._boringssl = pw_package.git_repo.GitRepo(
            name='boringssl',
            url=''.join(
                [
                    'https://pigweed.googlesource.com',
                    '/third_party/boringssl/boringssl',
                ]
            ),
            commit='9f55d972854d0b34dae39c7cd3679d6ada3dfd5b',
        )

    def status(self, path: pathlib.Path) -> bool:
        if not self._boringssl.status(boringssl_repo_path(path)):
            return False

        # Check that necessary build files are generated.
        build_files = ['BUILD.generated.gni', 'err_data.c']
        return all(os.path.exists(path / file) for file in build_files)

    def install(self, path: pathlib.Path) -> None:
        # Checkout the library
        repo_path = boringssl_repo_path(path)
        self._boringssl.install(repo_path)

        # BoringSSL provides a src/util/generate_build_files.py script for
        # generating build files. Call the script after checkout so that
        # our .gn build script can pick them up.
        script = repo_path / 'util' / 'generate_build_files.py'
        if not os.path.exists(script):
            raise FileNotFoundError('Fail to find generate_build_files.py')
        subprocess.run(['python', script, 'gn'], cwd=path)

        # TODO(zyecheng): Add install logic for chromium certificate verifier.

    def info(self, path: pathlib.Path) -> Sequence[str]:
        return (
            f'{self.name} installed in: {path}',
            'Enable by running "gn args out" and adding this line:',
            f'  dir_pw_third_party_boringssl = "{path}"',
        )


pw_package.package_manager.register(BoringSSL)
