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
"""Check the formatting of TODOs."""

import logging
from pathlib import Path
import re
from typing import Iterable, Pattern, Sequence, Union

from pw_presubmit import PresubmitContext, filter_paths

_LOG: logging.Logger = logging.getLogger(__name__)

EXCLUDE: Sequence[str] = (
    # Metadata
    r'^docker/tag$',
    r'\byarn.lock$',
    # Data files
    r'\.bin$',
    r'\.csv$',
    r'\.elf$',
    r'\.gif$',
    r'\.jpg$',
    r'\.json$',
    r'\.png$',
    r'\.svg$',
    r'\.xml$',
)

# todo-check: disable
BUGS_ONLY = re.compile(r'\bTODO\(b/\d+(?:, ?b/\d+)*\).*\w')
BUGS_OR_USERNAMES = re.compile(
    r'\bTODO\((?:b/\d+|[a-z]+)(?:, ?(?:b/\d+|[a-z]+))*\).*\w'
)
_TODO = re.compile(r'\bTODO\b')
# todo-check: enable

# If seen, ignore this line and the next.
_IGNORE = 'todo-check: ignore'

# Ignore a whole section. Please do not change the order of these lines.
_DISABLE = 'todo-check: disable'
_ENABLE = 'todo-check: enable'


def _process_file(ctx: PresubmitContext, todo_pattern: re.Pattern, path: Path):
    with path.open() as ins:
        _LOG.debug('Evaluating path %s', path)
        enabled = True
        prev = ''

        try:
            for i, line in enumerate(ins, 1):
                if _DISABLE in line:
                    enabled = False
                elif _ENABLE in line:
                    enabled = True

                if not enabled or _IGNORE in line or _IGNORE in prev:
                    prev = line
                    continue

                if _TODO.search(line):
                    if not todo_pattern.search(line):
                        # todo-check: ignore
                        ctx.fail(f'Bad TODO on line {i}:', path)
                        ctx.fail(f'    {line.strip()}')

                prev = line

        except UnicodeDecodeError:
            # File is not text, like a gif.
            _LOG.debug('File %s is not a text file', path)


def create(
    todo_pattern: re.Pattern = BUGS_ONLY,
    exclude: Iterable[Union[Pattern[str], str]] = EXCLUDE,
):
    """Create a todo_check presubmit step that uses the given pattern."""

    @filter_paths(exclude=exclude)
    def todo_check(ctx: PresubmitContext):
        """Check that TODO lines are valid."""  # todo-check: ignore
        for path in ctx.paths:
            _process_file(ctx, todo_pattern, path)

    return todo_check
