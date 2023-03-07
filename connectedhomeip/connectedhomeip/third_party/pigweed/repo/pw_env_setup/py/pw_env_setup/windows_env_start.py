# -*- coding: utf-8 -*-

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
"""Prints the env_setup banner for cmd.exe.

This is done from Python as activating colors and printing ASCII art are not
easy to do in cmd.exe. Activated colors also don't persist in the parent
process.
"""

from __future__ import print_function

import argparse
import os
import sys

from .colors import Color, enable_colors  # type: ignore

_PIGWEED_BANNER = u'''
 ▒█████▄   █▓  ▄███▒  ▒█    ▒█ ░▓████▒ ░▓████▒ ▒▓████▄
  ▒█░  █░ ░█▒ ██▒ ▀█▒ ▒█░ █ ▒█  ▒█   ▀  ▒█   ▀  ▒█  ▀█▌
  ▒█▄▄▄█░ ░█▒ █▓░ ▄▄░ ▒█░ █ ▒█  ▒███    ▒███    ░█   █▌
  ▒█▀     ░█░ ▓█   █▓ ░█░ █ ▒█  ▒█   ▄  ▒█   ▄  ░█  ▄█▌
  ▒█      ░█░ ░▓███▀   ▒█▓▀▓█░ ░▓████▒ ░▓████▒ ▒▓████▀
'''


def print_banner(bootstrap, no_shell_file):
    """Print the Pigweed or project-specific banner"""
    enable_colors()

    print(Color.green('\n  WELCOME TO...'))
    print(Color.magenta(_PIGWEED_BANNER))

    if bootstrap:
        print(
            Color.green(
                '\n  BOOTSTRAP! Bootstrap may take a few minutes; '
                'please be patient'
            )
        )
        print(
            Color.green(
                '  On Windows, this stage is extremely slow (~10 minutes).\n'
            )
        )
    else:
        print(
            Color.green(
                '\n  ACTIVATOR! This sets your console environment variables.\n'
            )
        )

        if no_shell_file:
            print(Color.bold_red('Error!\n'))
            print(
                Color.red(
                    '  Your Pigweed environment does not seem to be'
                    ' configured.'
                )
            )
            print(Color.red('  Run bootstrap.bat to perform initial setup.'))

    return 0


def parse():
    """Parse command-line arguments."""
    parser = argparse.ArgumentParser(
        prog="python -m pw_env_setup.windows_env_start"
    )
    parser.add_argument('--bootstrap', action='store_true')
    parser.add_argument('--no-shell-file', action='store_true')
    return parser.parse_args()


def main():
    """Script entry point."""
    if os.name != 'nt':
        return 1
    return print_banner(**vars(parse()))


if __name__ == '__main__':
    sys.exit(main())
