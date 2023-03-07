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
"""Defines ANSI color codes."""

import ctypes
import os


def _make_color(*codes):
    # Apply all the requested ANSI color codes. Note that this is unbalanced
    # with respect to the reset, which only requires a '0' to erase all codes.
    start = ''.join('\033[{}m'.format(code) for code in codes)
    reset = '\033[0m'

    return staticmethod(lambda msg: u'{}{}{}'.format(start, msg, reset))


class Color:  # pylint: disable=too-few-public-methods
    """Helpers to surround text with ASCII color escapes"""

    bold = _make_color(1)
    red = _make_color(31)
    bold_red = _make_color(31, 1)
    green = _make_color(32)
    magenta = _make_color(35, 1)


def enable_colors():
    if os.name == 'nt':
        kernel32 = ctypes.windll.kernel32
        kernel32.SetConsoleMode(kernel32.GetStdHandle(-11), 7)
