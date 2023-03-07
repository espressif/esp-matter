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
"""Injects pre main init to ST startup scripts."""

from typing import Optional

import pathlib
import re


def add_pre_main_init(startup: str) -> str:
    """Add pw_stm32cube_Init call to startup file

    The stm32cube startup files directly call main(), while pigweed expects to
    do some setup before main is called. This could include sys_io or system
    clock initialization.

    This adds a call to `pw_stm32cube_Init()` immediately before the call to
    `main()`

    Args:
        startup: The startup script read into a string

    Returns:
        A new startup script with the `pw_stm32cube_Init()` call added.

    Raises:
        ValueError if the `main()` call is not found in `startup`
    """
    match = re.search(r'\s*bl\s+main', startup)
    if match is None:
        raise ValueError("`bl main` not found in startup script")

    return (
        startup[: match.start()]
        + '\nbl pw_stm32cube_Init'
        + startup[match.start() :]
    )


def inject_init(startup_in: pathlib.Path, startup_out: Optional[pathlib.Path]):
    """Injects pw_stm32cube_Init before main in given ST startup script.

    Args:
        startup_in: path to startup_*.s file
        startup_out: path to write generated startup file or None.
                    If None, output startup script printed to stdout
    """
    with open(startup_in, 'rb') as startup_in_file:
        startup_in_str = startup_in_file.read().decode('utf-8', errors='ignore')

    startup_out_str = add_pre_main_init(startup_in_str)

    if startup_out:
        with open(startup_out, 'w') as startup_out_file:
            startup_out_file.write(startup_out_str)
    else:
        print(startup_out_str)
