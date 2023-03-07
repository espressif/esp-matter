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

# Inspired by
# https://fuchsia.googlesource.com/infra/recipes/+/336933647862a1a9718b4ca18f0a67e89c2419f8/recipe_modules/ninja/resources/ninja_wrapper.py
"""Extracts a concise error from a ninja log."""

from pathlib import Path
import re

_RULE_RE = re.compile(r'^\s*\[\d+/\d+\] (\S+)')
_FAILED_RE = re.compile(r'^\s*FAILED: (.*)$')
_FAILED_END_RE = re.compile(r'^\s*ninja: build stopped:.*')


def parse_ninja_stdout(ninja_stdout: Path) -> str:
    failure_begins = False
    failure_lines = []
    last_line = ''

    with ninja_stdout.open() as ins:
        for line in ins:
            # Trailing whitespace isn't significant, as it doesn't affect the
            # way the line shows up in the logs. However, leading whitespace may
            # be significant, especially for compiler error messages.
            line = line.rstrip()
            if failure_begins:
                if not _RULE_RE.match(line) and not _FAILED_END_RE.match(line):
                    failure_lines.append(line)
                else:
                    # Output of failed step ends, save its info.
                    failure_begins = False
            else:
                failed_nodes_match = _FAILED_RE.match(line)
                failure_begins = False
                if failed_nodes_match:
                    failure_begins = True
                    failure_lines.extend([last_line, line])
            last_line = line

    return '\n'.join(failure_lines)
