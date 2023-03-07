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
"""Install and check status of the Raspberry Pi Pico SDK."""

from contextlib import contextmanager
import os
from pathlib import Path
from typing import Sequence
import subprocess

import pw_package.git_repo
import pw_package.package_manager


@contextmanager
def change_working_dir(directory: Path):
    original_dir = Path.cwd()
    try:
        os.chdir(directory)
        yield directory
    finally:
        os.chdir(original_dir)


class PiPicoSdk(pw_package.package_manager.Package):
    """Install and check status of the Raspberry Pi Pico SDK."""

    def __init__(self, *args, **kwargs):
        super().__init__(*args, name='pico_sdk', **kwargs)
        self._pico_sdk = pw_package.git_repo.GitRepo(
            name='pico_sdk',
            url='https://github.com/raspberrypi/pico-sdk',
            commit='2e6142b15b8a75c1227dd3edbe839193b2bf9041',
        )

    def install(self, path: Path) -> None:
        self._pico_sdk.install(path)

        # Run submodule update --init to fetch tinyusb.
        with change_working_dir(path) as _pico_sdk_repo:
            subprocess.run(
                ['git', 'submodule', 'update', '--init'], capture_output=True
            )

    def info(self, path: Path) -> Sequence[str]:
        return (
            f'{self.name} installed in: {path}',
            "Enable by running 'gn args out' and adding this line:",
            f'  PICO_SRC_DIR = "{path}"',
        )


pw_package.package_manager.register(PiPicoSdk)
