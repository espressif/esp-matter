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
"""Color codes for use by rest of pw_cli."""

import ctypes
import os
import sys
from typing import Optional, Union

import pw_cli.env


def _make_color(*codes):
    # Apply all the requested ANSI color codes. Note that this is unbalanced
    # with respect to the reset, which only requires a '0' to erase all codes.
    start = ''.join(f'\033[{code}m' for code in codes)
    reset = '\033[0m'

    return lambda msg: f'{start}{msg}{reset}'


# TODO(keir): Replace this with something like the 'colorful' module.
class _Color:
    # pylint: disable=too-few-public-methods
    # pylint: disable=too-many-instance-attributes
    """Helpers to surround text with ASCII color escapes"""

    def __init__(self):
        self.none = str
        self.red = _make_color(31, 1)
        self.bold_red = _make_color(30, 41)
        self.yellow = _make_color(33, 1)
        self.bold_yellow = _make_color(30, 43, 1)
        self.green = _make_color(32)
        self.bold_green = _make_color(30, 42)
        self.blue = _make_color(34, 1)
        self.cyan = _make_color(36, 1)
        self.magenta = _make_color(35, 1)
        self.bold_magenta = _make_color(30, 45)
        self.bold_white = _make_color(37, 1)
        self.black_on_white = _make_color(30, 47)  # black fg white bg
        self.black_on_green = _make_color(30, 42)  # black fg green bg
        self.black_on_red = _make_color(30, 41)  # black fg red bg


class _NoColor:
    """Fake version of the _Color class that doesn't colorize."""

    def __getattr__(self, _):
        return str


def colors(enabled: Optional[bool] = None) -> Union[_Color, _NoColor]:
    """Returns an object for colorizing strings.

    By default, the object only colorizes if both stderr and stdout are TTYs.
    """
    if enabled is None:
        env = pw_cli.env.pigweed_environment()
        enabled = env.PW_USE_COLOR or (
            sys.stdout.isatty() and sys.stderr.isatty()
        )

    if enabled and os.name == 'nt':
        # Enable ANSI color codes in Windows cmd.exe.
        kernel32 = ctypes.windll.kernel32  # type: ignore
        kernel32.SetConsoleMode(kernel32.GetStdHandle(-11), 7)

    return _Color() if enabled else _NoColor()
