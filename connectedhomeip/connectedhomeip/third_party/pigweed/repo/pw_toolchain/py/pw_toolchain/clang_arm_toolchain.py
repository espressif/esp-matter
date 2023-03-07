#!/usr/bin/env python3
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
"""Generates flags needed for an ARM build using clang.

Using clang on Cortex-M cores isn't intuitive as the end-to-end experience isn't
quite completely in LLVM. LLVM doesn't yet provide compatible C runtime
libraries or C/C++ standard libraries. To work around this, this script pulls
the missing bits from an arm-none-eabi-gcc compiler on the system path. This
lets clang do the heavy lifting while only relying on some headers provided by
newlib/arm-none-eabi-gcc in addition to a small assortment of needed libraries.

To use this script, specify what flags you want from the script, and run with
the required architecture flags like you would with gcc:

  python -m pw_toolchain.clang_arm_toolchain --cflags -- -mthumb -mcpu=cortex-m3

The script will then print out the additional flags you need to pass to clang to
get a working build.
"""

import argparse
import sys
import os
import subprocess

from pathlib import Path
from typing import List, Dict, Tuple

_ARM_COMPILER_PREFIX = 'arm-none-eabi'
_ARM_COMPILER_NAME = _ARM_COMPILER_PREFIX + '-gcc'


def _parse_args() -> argparse.Namespace:
    """Parses arguments for this script, splitting out the command to run."""

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        '--gn-scope',
        action='store_true',
        help=(
            "Formats the output like a GN scope so it can be ingested by "
            "exec_script()"
        ),
    )
    parser.add_argument(
        '--cflags',
        action='store_true',
        help=('Include necessary C flags in the output'),
    )
    parser.add_argument(
        '--ldflags',
        action='store_true',
        help=('Include necessary linker flags in the output'),
    )
    parser.add_argument(
        'clang_flags',
        nargs=argparse.REMAINDER,
        help='Flags to pass to clang, which can affect library/include paths',
    )
    parsed_args = parser.parse_args()

    assert parsed_args.clang_flags[0] == '--', 'arguments not correctly split'
    parsed_args.clang_flags = parsed_args.clang_flags[1:]
    return parsed_args


def _compiler_info_command(print_command: str, cflags: List[str]) -> str:
    command = [_ARM_COMPILER_NAME]
    command.extend(cflags)
    command.append(print_command)
    result = subprocess.run(
        command,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )
    result.check_returncode()
    return result.stdout.decode().rstrip()


def get_gcc_lib_dir(cflags: List[str]) -> Path:
    return Path(
        _compiler_info_command('-print-libgcc-file-name', cflags)
    ).parent


def get_compiler_info(cflags: List[str]) -> Dict[str, str]:
    compiler_info: Dict[str, str] = {}
    compiler_info['gcc_libs_dir'] = os.path.relpath(
        str(get_gcc_lib_dir(cflags)), "."
    )
    compiler_info['sysroot'] = os.path.relpath(
        _compiler_info_command('-print-sysroot', cflags), "."
    )
    compiler_info['version'] = _compiler_info_command('-dumpversion', cflags)
    compiler_info['multi_dir'] = _compiler_info_command(
        '-print-multi-directory', cflags
    )
    return compiler_info


def get_cflags(compiler_info: Dict[str, str]):
    """TODO(amontanez): Add docstring."""
    # TODO(amontanez): Make newlib-nano optional.
    cflags = [
        # TODO(amontanez): For some reason, -stdlib++-isystem and
        # -isystem-after work, but emit unused argument errors. This is the only
        # way to let the build succeed.
        '-Qunused-arguments',
        # Disable all default libraries.
        "-nodefaultlibs",
        '--target=arm-none-eabi',
    ]

    # Add sysroot info.
    cflags.extend(
        (
            '--sysroot=' + compiler_info['sysroot'],
            '-isystem'
            + str(Path(compiler_info['sysroot']) / 'include' / 'newlib-nano'),
            # This must be included after Clang's builtin headers.
            '-isystem-after' + str(Path(compiler_info['sysroot']) / 'include'),
            '-stdlib++-isystem'
            + str(
                Path(compiler_info['sysroot'])
                / 'include'
                / 'c++'
                / compiler_info['version']
            ),
            '-isystem'
            + str(
                Path(compiler_info['sysroot'])
                / 'include'
                / 'c++'
                / compiler_info['version']
                / _ARM_COMPILER_PREFIX
                / compiler_info['multi_dir']
            ),
        )
    )

    return cflags


def get_crt_objs(compiler_info: Dict[str, str]) -> Tuple[str, ...]:
    return (
        str(Path(compiler_info['gcc_libs_dir']) / 'crtfastmath.o'),
        str(Path(compiler_info['gcc_libs_dir']) / 'crti.o'),
        str(Path(compiler_info['gcc_libs_dir']) / 'crtn.o'),
        str(
            Path(compiler_info['sysroot'])
            / 'lib'
            / compiler_info['multi_dir']
            / 'crt0.o'
        ),
    )


def get_ldflags(compiler_info: Dict[str, str]) -> List[str]:
    ldflags: List[str] = [
        '-lnosys',
        # Add library search paths.
        '-L' + compiler_info['gcc_libs_dir'],
        '-L'
        + str(
            Path(compiler_info['sysroot']) / 'lib' / compiler_info['multi_dir']
        ),
        # Add libraries to link.
        '-lc_nano',
        '-lm',
        '-lgcc',
        '-lstdc++_nano',
    ]

    # Add C runtime object files.
    objs = get_crt_objs(compiler_info)
    ldflags.extend(objs)

    return ldflags


def main(
    cflags: bool,
    ldflags: bool,
    gn_scope: bool,
    clang_flags: List[str],
) -> int:
    """Script entry point."""
    compiler_info = get_compiler_info(clang_flags)
    if ldflags:
        ldflag_list = get_ldflags(compiler_info)

    if cflags:
        cflag_list = get_cflags(compiler_info)

    if not gn_scope:
        flags = []
        if cflags:
            flags.extend(cflag_list)
        if ldflags:
            flags.extend(ldflag_list)
        print(' '.join(flags))
        return 0

    if cflags:
        print('cflags = [')
        for flag in cflag_list:
            print(f'  "{flag}",')
        print(']')

    if ldflags:
        print('ldflags = [')
        for flag in ldflag_list:
            print(f'  "{flag}",')
        print(']')
    return 0


if __name__ == '__main__':
    sys.exit(main(**vars(_parse_args())))
