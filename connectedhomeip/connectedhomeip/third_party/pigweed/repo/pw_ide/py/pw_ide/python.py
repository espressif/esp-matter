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
"""Configure Python IDE support for Pigweed projects."""

from collections import defaultdict
import os
from pathlib import Path
import platform
from typing import Dict, NamedTuple

_PYTHON_VENV_PATH = (
    Path(os.path.expandvars('$_PW_ACTUAL_ENVIRONMENT_ROOT')) / 'pigweed-venv'
)


class _PythonPathsForPlatform(NamedTuple):
    bin_dir_name: str = 'bin'
    interpreter_name: str = 'python3'


# When given a platform (e.g. the output of platform.system()), this dict gives
# the platform-specific virtualenv path names.
_PYTHON_PATHS_FOR_PLATFORM: Dict[str, _PythonPathsForPlatform] = defaultdict(
    _PythonPathsForPlatform
)
_PYTHON_PATHS_FOR_PLATFORM['Windows'] = _PythonPathsForPlatform(
    bin_dir_name='Scripts', interpreter_name='pythonw.exe'
)


class PythonPaths:
    """Holds the platform-specific Python environment paths.

    The directory layout of Python virtual environments varies among
    platforms. This class holds the data needed to find the right paths
    for a specific platform.
    """

    def __init__(self, system=platform.system()):
        (bin_dir_name, interpreter_name) = _PYTHON_PATHS_FOR_PLATFORM[system]
        self.bin_dir = _PYTHON_VENV_PATH / bin_dir_name
        self.interpreter = self.bin_dir / interpreter_name
