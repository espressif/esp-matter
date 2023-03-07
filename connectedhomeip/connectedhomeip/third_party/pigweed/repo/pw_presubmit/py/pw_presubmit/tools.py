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
"""General purpose tools for running presubmit checks."""

import collections.abc
from collections import Counter, defaultdict
import logging
import os
from pathlib import Path
import shlex
import subprocess
from typing import (
    Any,
    Dict,
    Iterable,
    Iterator,
    List,
    Optional,
    Sequence,
    Pattern,
    Tuple,
)

_LOG: logging.Logger = logging.getLogger(__name__)


def plural(
    items_or_count,
    singular: str,
    count_format='',
    these: bool = False,
    number: bool = True,
    are: bool = False,
) -> str:
    """Returns the singular or plural form of a word based on a count."""

    try:
        count = len(items_or_count)
    except TypeError:
        count = items_or_count

    prefix = ('this ' if count == 1 else 'these ') if these else ''
    num = f'{count:{count_format}} ' if number else ''
    suffix = (' is' if count == 1 else ' are') if are else ''

    if singular.endswith('y'):
        result = f'{singular[:-1]}{"y" if count == 1 else "ies"}'
    elif singular.endswith('s'):
        result = f'{singular}{"" if count == 1 else "es"}'
    else:
        result = f'{singular}{"" if count == 1 else "s"}'

    return f'{prefix}{num}{result}{suffix}'


def make_box(section_alignments: Sequence[str]) -> str:
    indices = [i + 1 for i in range(len(section_alignments))]
    top_sections = '{2}'.join('{1:{1}^{width%d}}' % i for i in indices)
    mid_sections = '{5}'.join(
        '{section%d:%s{width%d}}' % (i, section_alignments[i - 1], i)
        for i in indices
    )
    bot_sections = '{9}'.join('{8:{8}^{width%d}}' % i for i in indices)

    return ''.join(
        [
            '{0}',
            *top_sections,
            '{3}\n',
            '{4}',
            *mid_sections,
            '{6}\n',
            '{7}',
            *bot_sections,
            '{10}',
        ]
    )


def file_summary(
    paths: Iterable[Path],
    levels: int = 2,
    max_lines: int = 12,
    max_types: int = 3,
    pad: str = ' ',
    pad_start: str = ' ',
    pad_end: str = ' ',
) -> List[str]:
    """Summarizes a list of files by the file types in each directory."""

    # Count the file types in each directory.
    all_counts: Dict[Any, Counter] = defaultdict(Counter)

    for path in paths:
        parent = path.parents[max(len(path.parents) - levels, 0)]
        all_counts[parent][path.suffix] += 1

    # If there are too many lines, condense directories with the fewest files.
    if len(all_counts) > max_lines:
        counts = sorted(
            all_counts.items(), key=lambda item: -sum(item[1].values())
        )
        counts, others = (
            sorted(counts[: max_lines - 1]),
            counts[max_lines - 1 :],
        )
        counts.append(
            (
                f'({plural(others, "other")})',
                sum((c for _, c in others), Counter()),
            )
        )
    else:
        counts = sorted(all_counts.items())

    width = max(len(str(d)) + len(os.sep) for d, _ in counts) if counts else 0
    width += len(pad_start)

    # Prepare the output.
    output = []
    for path, files in counts:
        total = sum(files.values())
        del files['']  # Never display no-extension files individually.

        if files:
            extensions = files.most_common(max_types)
            other_extensions = total - sum(count for _, count in extensions)
            if other_extensions:
                extensions.append(('other', other_extensions))

            types = ' (' + ', '.join(f'{c} {e}' for e, c in extensions) + ')'
        else:
            types = ''

        root = f'{path}{os.sep}{pad_start}'.ljust(width, pad)
        output.append(f'{root}{pad_end}{plural(total, "file")}{types}')

    return output


def relative_paths(paths: Iterable[Path], start: Path) -> Iterable[Path]:
    """Returns relative Paths calculated with os.path.relpath."""
    for path in paths:
        yield Path(os.path.relpath(path, start))


def exclude_paths(
    exclusions: Iterable[Pattern[str]],
    paths: Iterable[Path],
    relative_to: Optional[Path] = None,
) -> Iterable[Path]:
    """Excludes paths based on a series of regular expressions."""
    if relative_to:
        relpath = lambda path: Path(os.path.relpath(path, relative_to))
    else:
        relpath = lambda path: path

    for path in paths:
        if not any(e.search(relpath(path).as_posix()) for e in exclusions):
            yield path


def _truncate(value, length: int = 60) -> str:
    value = str(value)
    return (value[: length - 5] + '[...]') if len(value) > length else value


def format_command(args: Sequence, kwargs: dict) -> Tuple[str, str]:
    attr = ', '.join(f'{k}={_truncate(v)}' for k, v in sorted(kwargs.items()))
    return attr, ' '.join(shlex.quote(str(arg)) for arg in args)


def log_run(args, **kwargs) -> subprocess.CompletedProcess:
    """Logs a command then runs it with subprocess.run.

    Takes the same arguments as subprocess.run.
    """
    _LOG.debug('[COMMAND] %s\n%s', *format_command(args, kwargs))
    return subprocess.run(args, **kwargs)


def flatten(*items) -> Iterator:
    """Yields items from a series of items and nested iterables.

    This function is used to flatten arbitrarily nested lists. str and bytes
    are kept intact.
    """

    for item in items:
        if isinstance(item, collections.abc.Iterable) and not isinstance(
            item, (str, bytes, bytearray)
        ):
            yield from flatten(*item)
        else:
            yield item
