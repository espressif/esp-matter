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
"""Facilities for accessing the current Pigweed branding"""

import operator
from typing import Optional
from pathlib import Path

import pw_cli.env
import pw_cli.color

_memoized_banner: Optional[str] = None

# This is the default banner for Pigweed.
_PIGWEED_BANNER = '''
 ▒█████▄   █▓  ▄███▒  ▒█    ▒█ ░▓████▒ ░▓████▒ ▒▓████▄
  ▒█░  █░ ░█▒ ██▒ ▀█▒ ▒█░ █ ▒█  ▒█   ▀  ▒█   ▀  ▒█  ▀█▌
  ▒█▄▄▄█░ ░█▒ █▓░ ▄▄░ ▒█░ █ ▒█  ▒███    ▒███    ░█   █▌
  ▒█▀     ░█░ ▓█   █▓ ░█░ █ ▒█  ▒█   ▄  ▒█   ▄  ░█  ▄█▌
  ▒█      ░█░ ░▓███▀   ▒█▓▀▓█░ ░▓████▒ ░▓████▒ ▒▓████▀
'''


def banner() -> str:
    global _memoized_banner  # pylint: disable=global-statement
    if _memoized_banner is not None:
        return _memoized_banner

    parsed_env = pw_cli.env.pigweed_environment()

    # Take the banner from the file PW_BRANDING_BANNER; or use the default.
    banner_filename = parsed_env.PW_BRANDING_BANNER
    _memoized_banner = (
        Path(banner_filename).read_text()
        if banner_filename
        else _PIGWEED_BANNER
    )

    # Color the banner if requested.
    banner_color = parsed_env.PW_BRANDING_BANNER_COLOR
    if banner_color != '':
        set_color = operator.attrgetter(banner_color)(pw_cli.color.colors())
        _memoized_banner = '\n'.join(
            set_color(line) for line in _memoized_banner.splitlines()
        )

    return _memoized_banner
