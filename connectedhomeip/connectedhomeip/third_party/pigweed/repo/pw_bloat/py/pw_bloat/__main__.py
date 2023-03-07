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
"""Size reporting utilities."""

import argparse
import logging
from pathlib import Path
import sys
from typing import Iterable

from pw_bloat import bloat
from pw_bloat.label import DataSourceMap
from pw_bloat.label_output import BloatTableOutput
import pw_cli.log

_LOG = logging.getLogger(__name__)


def _parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument(
        'binary',
        help='Path to the ELF file to analyze',
        metavar='BINARY',
        type=Path,
    )
    parser.add_argument(
        '-d',
        '--data-sources',
        help='Comma-separated list of additional Bloaty data sources to report',
        type=lambda s: s.split(','),
        default=(),
    )
    parser.add_argument(
        '--diff',
        metavar='BINARY',
        help='Run a size diff against a base binary file',
        type=Path,
    )
    parser.add_argument(
        '-v',
        '--verbose',
        help=('Print all log messages ' '(only errors are printed by default)'),
        action='store_true',
    )

    return parser.parse_args()


def _run_size_report(
    elf: Path, data_sources: Iterable[str] = ()
) -> DataSourceMap:
    """Runs a size analysis on an ELF file, returning a pw_bloat size map.

    Returns:
        A size map of the labels in the binary under the provided data sources.

    Raises:
        NoMemoryRegions: The binary does not define bloat memory region symbols.
    """

    bloaty_tsv = bloat.memory_regions_size_report(
        elf, additional_data_sources=data_sources, extra_args=('--tsv',)
    )

    return DataSourceMap.from_bloaty_tsv(bloaty_tsv)


def _no_memory_regions_error(elf: Path) -> None:
    _LOG.error('Executable %s does not define any bloat memory regions', elf)
    _LOG.error(
        'Refer to https://pigweed.dev/pw_bloat/#memoryregions-data-source'
    )
    _LOG.error('for information on how to configure them.')


def _single_binary_report(elf: Path, data_sources: Iterable[str] = ()) -> int:
    try:
        data_source_map = _run_size_report(elf, data_sources)
    except bloat.NoMemoryRegions:
        _no_memory_regions_error(elf)
        return 1

    print(BloatTableOutput(data_source_map).create_table())
    return 0


def _diff_report(
    target: Path, base: Path, data_sources: Iterable[str] = ()
) -> int:
    try:
        base_map = _run_size_report(base, data_sources)
        target_map = _run_size_report(target, data_sources)
    except bloat.NoMemoryRegions as err:
        _no_memory_regions_error(err.elf)
        return 1

    diff = target_map.diff(base_map)

    print(BloatTableOutput(diff).create_table())
    return 0


def main() -> int:
    """Run binary size reports."""

    args = _parse_args()

    if not args.verbose:
        pw_cli.log.set_all_loggers_minimum_level(logging.ERROR)

    if args.diff is not None:
        return _diff_report(
            args.binary, args.diff, data_sources=args.data_sources
        )

    return _single_binary_report(args.binary, data_sources=args.data_sources)


if __name__ == '__main__':
    sys.exit(main())
