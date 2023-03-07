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
"""Generates a useful bloaty config file containing new data sources."""

import argparse
import logging
import re
import sys
from typing import BinaryIO, Dict, List, NamedTuple, Optional, TextIO

import pw_cli.argument_types
from elftools.elf import elffile  # type: ignore

_LOG = logging.getLogger('bloaty_config')

# 'pw_bloat_config_memory_region_NAME_{start,end}{_N,}' where _N defaults to 0.
_MEMORY_REGION_SYMBOL_RE = re.compile(
    r'pw_bloat_config_memory_region_'
    + r'(?P<name>\w+)_(?P<limit>(start|end))(_(?P<index>\d+))?'
)


def _parse_args() -> argparse.Namespace:
    """Return a CLI argument parser for this module."""
    parser = argparse.ArgumentParser(
        description='Generates useful bloaty configurations entries',
        epilog='Hint: try this:\n'
        '   python -m pw_bloat.bloaty_config my_app.elf -o my_app.bloat',
    )
    parser.add_argument('elf_file', type=argparse.FileType('rb'))
    parser.add_argument(
        '--output',
        '-o',
        type=argparse.FileType('w'),
        help='The generated bloaty configuration',
        default=sys.stdout,
    )
    parser.add_argument(
        '--utilization',
        action='store_true',
        dest='utilization',
        default=True,
        help=(
            'Generate the utilization custom_data_source based on sections '
            'with "unused_space" in anywhere in their name'
        ),
    )
    parser.add_argument(
        '--no-utilization',
        action='store_false',
        dest='utilization',
    )

    parser.add_argument(
        '--memoryregions',
        action='store_true',
        default=True,
        help=(
            'Generate the memoryregions custom_data_source based on '
            'symbols defined in the linker script matching the following '
            'pattern: '
            '"pw::bloat::config::memory_region::NAME[0].{start,end}"'
        ),
    )
    parser.add_argument(
        '--no-memoryregions',
        action='store_false',
        dest='memoryregions',
    )

    parser.add_argument(
        '-l',
        '--loglevel',
        type=pw_cli.argument_types.log_level,
        default=logging.INFO,
        help='Set the log level' '(debug, info, warning, error, critical)',
    )
    return parser.parse_args()


def _parse_memory_regions(parsed_elf_file: elffile.ELFFile) -> Optional[Dict]:
    """
    Search for the special pw::bloat::config symbols in the ELF binary.

    This produces a dictionary which looks like:
      {
        MEMORY_REGION_NAME_0:{
          0:(VM_START_ADDRESS, VM_END_ADDRESS)
          ...
          N:(VM_START_ADDRESS, VM_END_ADDRESS)
        }
        ...
        MEMORY_REGION_NAME_M:{
          0:(VM_START_ADDRESS, VM_END_ADDRESS)
          ...
          K:(VM_START_ADDRESS, VM_END_ADDRESS)
        }
      }
    """
    symtab_section = parsed_elf_file.get_section_by_name('.symtab')
    assert symtab_section

    # Produces an initial dictionary which looks like:
    #  {
    #    MEMORY_REGION_NAME_0:{
    #      0:{ 'start':vm_start_address, 'end':vm_end_address }
    #      ...
    #      N:{ 'start':vm_start_address, 'end':vm_end_address }
    #    }
    #    ...
    #    MEMORY_REGION_NAME_M:{
    #      0:{ 'start':vm_start_address, 'end':vm_end_address }
    #      ...
    #      K:{ 'start':vm_start_address, 'end':vm_end_address }
    #    }
    #  }
    memory_regions: Dict = {}
    for symbol in symtab_section.iter_symbols():
        match = _MEMORY_REGION_SYMBOL_RE.match(symbol.name)
        if not match:
            continue

        name = match.group('name')
        limit = match.group('limit')
        if match.group('index'):
            index = int(match.group('index'))
        else:
            index = 0
        if name not in memory_regions:
            memory_regions[name] = {}
        memory_region = memory_regions[name]
        if index not in memory_region:
            memory_region[index] = {}
        memory_region_segment = memory_region[index]
        memory_region_segment[limit] = symbol.entry.st_value

    # If the user did not provide a single pw::bloat::config symbol in the ELF
    # binary then bail out and do nothing.
    if not memory_regions:
        _LOG.info('No valid pw::bloat::config::memory_region::* symbols found')
        return None

    # Ensure all memory regions' ranges have an end and start.
    missing_range_limits = False
    for region_name, ranges in memory_regions.items():
        for index, limits in ranges.items():
            if 'start' not in limits:
                missing_range_limits = True
                _LOG.error(
                    '%s[%d] is missing the start address', region_name, index
                )
            if 'end' not in limits:
                missing_range_limits = True
                _LOG.error(
                    '%s[%d] is missing the end address', region_name, index
                )
    if missing_range_limits:
        _LOG.error('Invalid memory regions detected: missing ranges')
        return None

    # Translate the initial memory_regions dictionary to the tupled return
    # format, i.e. (start, end) values in the nested dictionary.
    tupled_memory_regions: Dict = {}
    for region_name, ranges in memory_regions.items():
        if region_name not in tupled_memory_regions:
            tupled_memory_regions[region_name] = {}
        for index, limits in ranges.items():
            tupled_memory_regions[region_name][index] = (
                limits['start'],
                limits['end'],
            )

    # Ensure the memory regions do not overlap.
    if _memory_regions_overlap(tupled_memory_regions):
        _LOG.error('Invalid memory regions detected: overlaps detected')
        return None

    return tupled_memory_regions


def _parse_segments(parsed_elf_file: elffile.ELFFile) -> Dict:
    """
    Report all of the segment information from the ELF binary.

    Iterates over all of the segments in the ELF file's program header and
    reports where they reside in virtual memory through a dictionary which
    looks like:
      {
        0:(start_vmaddr,end_vmaddr),
        ...
        N:(start_vmaddr,end_vmaddr),
      }
    """
    segments = {}
    for i in range(parsed_elf_file.num_segments()):
        segment = parsed_elf_file.get_segment(i)
        start_vmaddr = segment['p_vaddr']
        memory_size = segment['p_memsz']
        if memory_size == 0:
            continue  # Not a loaded segment which resides in virtual memory.
        end_vmaddr = start_vmaddr + memory_size
        segments[i] = (start_vmaddr, end_vmaddr)
    return segments


def _memory_regions_overlap(memory_regions: Dict) -> bool:
    """Returns where any memory regions overlap each other."""
    overlaps_detected = False
    for current_name, current_ranges in memory_regions.items():
        for current_index, (
            current_start,
            current_end,
        ) in current_ranges.items():
            for other_name, other_ranges in memory_regions.items():
                for other_index, (
                    other_start,
                    other_end,
                ) in other_ranges.items():
                    if (
                        current_name == other_name
                        and current_index == other_index
                    ):
                        continue  # Skip yourself.
                    # Check if the other region end is within this region.
                    other_end_overlaps = (
                        current_start < other_end <= current_end
                    )
                    other_start_overlaps = (
                        current_start <= other_start < current_end
                    )
                    if other_end_overlaps or other_start_overlaps:
                        overlaps_detected = True
                        _LOG.error(
                            f'error: {current_name}[{current_index}] '
                            + f'[{hex(current_start)},'
                            + f'{hex(current_end)}] overlaps with '
                            + f'{other_name}[{other_index}] '
                            f'[{hex(other_start)},'
                            + f'{hex(other_end)}] overlaps with '
                        )
    return overlaps_detected


def _get_segments_to_memory_region_map(elf_file: BinaryIO) -> Optional[Dict]:
    """
    Processes an ELF file to look up what memory regions segments are in.

    Returns the result from map_segments_to_memory_regions if valid memory
    regions were parsed out of the ELF file.
    """
    parsed_elf_file = elffile.ELFFile(elf_file)

    memory_regions = _parse_memory_regions(parsed_elf_file)
    if not memory_regions:
        return None

    segments = _parse_segments(parsed_elf_file)

    return map_segments_to_memory_regions(
        segments=segments, memory_regions=memory_regions
    )


def map_segments_to_memory_regions(
    segments: Dict, memory_regions: Dict
) -> Dict:
    """
    Maps segments to the virtual memory regions they reside in.

    This takes in the results from _parse_memory_regions and _parse_segments and
    produces a dictionary which looks like:
    {
      SEGMENT_INDEX_0:'MEMORY_REGION_NAME_0',
      SEGMENT_INDEX_1:'MEMORY_REGION_NAME_0',
      ...
      SEGMENT_INDEX_N:'MEMORY_REGION_NAME_M',
    }
    """

    # Now for each segment, determine what memory region it belongs to
    # and generate a bloaty config output for it.
    segment_to_memory_region = {}
    for segment, (segment_start, segment_end) in segments.items():
        # Note this is the final filter bloaty rewrite pattern format.
        for memory_region_name, memory_region_info in memory_regions.items():
            for _, (
                subregion_start,
                subregion_end,
            ) in memory_region_info.items():
                if (
                    segment_start >= subregion_start
                    and segment_end <= subregion_end
                ):
                    # We found the subregion the segment resides in.
                    segment_to_memory_region[segment] = memory_region_name
        if segment not in segment_to_memory_region:
            _LOG.error(
                f'Error: Failed to find memory region for LOAD #{segment} '
                + f'[{hex(segment_start)},{hex(segment_end)}]'
            )
    return segment_to_memory_region


def generate_memoryregions_data_source(segment_to_memory_region: Dict) -> str:
    output: List[str] = []
    output.append('custom_data_source: {')
    output.append('  name: "memoryregions"')
    output.append('  base_data_source: "segments"')
    for segment_index, memory_region in segment_to_memory_region.items():
        output.append('  rewrite: {')
        segment_filter = r'^LOAD ' + f'#{segment_index}' + r' \\[.*\\]$'
        output.append(f'    pattern:"{segment_filter}"')
        output.append(f'    replacement:"{memory_region}"')
        output.append('  }')
    output.append('  rewrite: {')
    output.append('    pattern:".*"')
    output.append('    replacement:"Not resident in memory"')
    output.append('  }')
    output.append('}')
    return '\n'.join(output) + '\n'


def generate_utilization_data_source() -> str:
    output: List[str] = []
    output.append('custom_data_source: {')
    output.append('  name:"utilization"')
    output.append('  base_data_source:"sections"')
    output.append('  rewrite: {')
    output.append('    pattern:"unused_space"')
    output.append('    replacement:"Free space"')
    output.append('  }')
    output.append('  rewrite: {')
    output.append('    pattern:".*"')
    output.append('    replacement:"Used space"')
    output.append('  }')
    output.append('}')
    return '\n'.join(output) + '\n'


class BloatyConfigResult(NamedTuple):
    has_memoryregions: bool
    has_utilization: bool


def generate_bloaty_config(
    elf_file: BinaryIO,
    enable_memoryregions: bool,
    enable_utilization: bool,
    out_file: TextIO,
) -> BloatyConfigResult:
    """Generates a Bloaty config file from symbols within an ELF.

    Returns:
        Tuple indicating whether a memoryregions data source, a utilization data
        source, or both were written.
    """

    result = [False, False]

    if enable_memoryregions:
        # Enable the "memoryregions" data_source if the user provided the
        # required pw_bloat specific symbols in their linker script.
        segment_to_memory_region = _get_segments_to_memory_region_map(elf_file)
        if not segment_to_memory_region:
            _LOG.info('memoryregions data_source is not provided')
        else:
            _LOG.info('memoryregions data_source is provided')
            out_file.write(
                generate_memoryregions_data_source(segment_to_memory_region)
            )
            result[0] = True

    if enable_utilization:
        _LOG.info('utilization data_source is provided')
        out_file.write(generate_utilization_data_source())
        result[1] = True

    return BloatyConfigResult(*result)


def main() -> int:
    """Generates a useful bloaty config file containing new data sources."""
    args = _parse_args()

    logging.basicConfig(format='%(message)s', level=args.loglevel)

    generate_bloaty_config(
        elf_file=args.elf_file,
        enable_memoryregions=args.memoryregions,
        enable_utilization=args.utilization,
        out_file=args.output,
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
