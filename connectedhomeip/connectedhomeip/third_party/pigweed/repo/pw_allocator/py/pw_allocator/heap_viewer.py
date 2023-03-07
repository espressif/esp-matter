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
"""Heap visualizer of ASCII characters."""

import argparse
import sys
import math
import logging
from typing import Optional
from dataclasses import dataclass, field
import coloredlogs  # type: ignore


@dataclass
class HeapBlock:
    """Building blocks for memory chunk allocated at heap."""

    size: int
    mem_offset: int
    next: Optional['HeapBlock'] = None


@dataclass
class HeapUsage:
    """Contains a linked list of allocated HeapBlocks."""

    # Use a default_factory to avoid mutable default value. See
    # https://docs.python.org/3/library/dataclasses.html#mutable-default-values
    begin: HeapBlock = field(default_factory=lambda: HeapBlock(0, 0))

    def add_block(self, block):
        cur_block = self.begin.next
        prev_block = self.begin
        while cur_block is not None:
            if cur_block.mem_offset == block.mem_offset:
                return
            if cur_block.mem_offset < block.mem_offset:
                prev_block = cur_block
                cur_block = cur_block.next
            else:
                block.next = cur_block
                prev_block.next = block
                return
        prev_block.next = block

    def remove_block(self, address):
        cur_block = self.begin.next
        prev_block = self.begin
        while cur_block is not None:
            if cur_block.mem_offset == address:
                prev_block.next = cur_block.next
                return
            if cur_block.mem_offset < address:
                prev_block = cur_block
                cur_block = cur_block.next
            else:
                return


def add_parser_arguments(parser):
    """Parse args."""
    parser.add_argument(
        '--dump-file',
        help=(
            'dump file that contains a list of malloc and '
            'free instructions. The format should be as '
            'follows: "m <size> <address>" on a line for '
            'each malloc called and "f <address>" on a line '
            'for each free called.'
        ),
        required=True,
    )

    parser.add_argument(
        '--heap-low-address',
        help=('lower address of the heap.'),
        type=lambda x: int(x, 0),
        required=True,
    )

    parser.add_argument(
        '--heap-high-address',
        help=('higher address of the heap.'),
        type=lambda x: int(x, 0),
        required=True,
    )

    parser.add_argument(
        '--poison-enabled',
        help=('if heap poison is enabled or not.'),
        default=False,
        action='store_true',
    )

    parser.add_argument(
        '--pointer-size',
        help=('size of pointer on the machine.'),
        default=4,
        type=lambda x: int(x, 0),
    )


_LEFT_HEADER_CHAR = '['
_RIGHT_HEADER_CHAR = ']'
_USED_CHAR = '*'
_FREE_CHAR = ' '
_CHARACTERS_PER_LINE = 64
_BYTES_PER_CHARACTER = 4
_LOG = logging.getLogger(__name__)


def _exit_due_to_file_not_found():
    _LOG.critical(
        'Dump file location is not provided or dump file is not '
        'found. Please specify a valid file in the argument.'
    )
    sys.exit(1)


def _exit_due_to_bad_heap_info():
    _LOG.critical(
        'Heap low/high address is missing or invalid. Please put valid '
        'addresses in the argument.'
    )
    sys.exit(1)


def visualize(
    dump_file=None,
    heap_low_address=None,
    heap_high_address=None,
    poison_enabled=False,
    pointer_size=4,
):
    """Visualization of heap usage."""
    # TODO(b/235282507): Add standarized mechanisms to produce dump file and
    # read heap information from dump file.
    aligned_bytes = pointer_size
    header_size = pointer_size * 2

    try:
        if heap_high_address < heap_low_address:
            _exit_due_to_bad_heap_info()
        heap_size = heap_high_address - heap_low_address
    except TypeError:
        _exit_due_to_bad_heap_info()

    if poison_enabled:
        poison_offset = pointer_size
    else:
        poison_offset = 0

    try:
        allocation_dump = open(dump_file, 'r')
    except (FileNotFoundError, TypeError):
        _exit_due_to_file_not_found()

    heap_visualizer = HeapUsage()
    # Parse the dump file.
    for line in allocation_dump:
        info = line[:-1].split(' ')
        if info[0] == 'm':
            # Add a HeapBlock when malloc is called
            block = HeapBlock(
                int(math.ceil(float(info[1]) / aligned_bytes)) * aligned_bytes,
                int(info[2], 0) - heap_low_address,
            )
            heap_visualizer.add_block(block)
        elif info[0] == 'f':
            # Remove the HeapBlock when free is called
            heap_visualizer.remove_block(int(info[1], 0) - heap_low_address)

    # next_block indicates the nearest HeapBlock that hasn't finished
    # printing.
    next_block = heap_visualizer.begin
    if next_block.next is None:
        next_mem_offset = heap_size + header_size + poison_offset + 1
        next_size = 0
    else:
        next_mem_offset = next_block.next.mem_offset
        next_size = next_block.next.size

    # Flags to indicate status of the 4 bytes going to be printed.
    is_left_header = False
    is_right_header = False
    is_used = False

    # Print overall heap information
    _LOG.info(
        '%-40s%-40s',
        f'The heap starts at {hex(heap_low_address)}.',
        f'The heap ends at {hex(heap_high_address)}.',
    )
    _LOG.info(
        '%-40s%-40s',
        f'Heap size is {heap_size // 1024}k bytes.',
        f'Heap is aligned by {aligned_bytes} bytes.',
    )
    if poison_offset != 0:
        _LOG.info(
            'Poison is enabled %d bytes before and after the usable '
            'space of each block.',
            poison_offset,
        )
    else:
        _LOG.info('%-40s', 'Poison is disabled.')
    _LOG.info(
        '%-40s',
        'Below is the visualization of the heap. '
        'Each character represents 4 bytes.',
    )
    _LOG.info('%-40s', f"    '{_FREE_CHAR}' indicates free space.")
    _LOG.info('%-40s', f"    '{_USED_CHAR}' indicates used space.")
    _LOG.info(
        '%-40s',
        f"    '{_LEFT_HEADER_CHAR}' indicates header or "
        'poisoned space before the block.',
    )
    _LOG.info(
        '%-40s',
        f"    '{_RIGHT_HEADER_CHAR}' poisoned space after " 'the block.',
    )
    print()

    # Go over the heap space where there will be 64 characters each line.
    for line_base_address in range(
        0, heap_size, _CHARACTERS_PER_LINE * _BYTES_PER_CHARACTER
    ):
        # Print the heap address of the current line.
        sys.stdout.write(
            f"{' ': <13}"
            f'{hex(heap_low_address + line_base_address)}'
            f"{f' (+{line_base_address}):': <12}"
        )
        for line_offset in range(
            0, _CHARACTERS_PER_LINE * _BYTES_PER_CHARACTER, _BYTES_PER_CHARACTER
        ):
            # Determine if the current 4 bytes is used, unused, or is a
            # header.
            # The case that we have went over the previous block and will
            # turn to the next block.
            current_address = line_base_address + line_offset
            if current_address == next_mem_offset + next_size + poison_offset:
                next_block = next_block.next
                # If this is the last block, set nextMemOffset to be over
                # the last byte of heap so that the rest of the heap will
                # be printed out as unused.
                # Otherwise set the next HeapBlock allocated.
                if next_block.next is None:
                    next_mem_offset = (
                        heap_size + header_size + poison_offset + 1
                    )
                    next_size = 0
                else:
                    next_mem_offset = next_block.next.mem_offset
                    next_size = next_block.next.size

            # Determine the status of the current 4 bytes.
            if (
                next_mem_offset - header_size - poison_offset
                <= current_address
                < next_mem_offset
            ):
                is_left_header = True
                is_right_header = False
                is_used = False
            elif (
                next_mem_offset <= current_address < next_mem_offset + next_size
            ):
                is_left_header = False
                is_right_header = False
                is_used = True
            elif (
                next_mem_offset + next_size
                <= current_address
                < next_mem_offset + next_size + poison_offset
            ):
                is_left_header = False
                is_right_header = True
                is_used = False
            else:
                is_left_header = False
                is_right_header = False
                is_used = False

            if is_left_header:
                sys.stdout.write(_LEFT_HEADER_CHAR)
            elif is_right_header:
                sys.stdout.write(_RIGHT_HEADER_CHAR)
            elif is_used:
                sys.stdout.write(_USED_CHAR)
            else:
                sys.stdout.write(_FREE_CHAR)
        sys.stdout.write('\n')

    allocation_dump.close()


def main():
    """A python script to visualize heap usage given a dump file."""
    parser = argparse.ArgumentParser(description=main.__doc__)
    add_parser_arguments(parser)
    # Try to use pw_cli logs, else default to something reasonable.
    try:
        import pw_cli.log  # pylint: disable=import-outside-toplevel

        pw_cli.log.install()
    except ImportError:
        coloredlogs.install(
            level='INFO',
            level_styles={'debug': {'color': 244}, 'error': {'color': 'red'}},
            fmt='%(asctime)s %(levelname)s | %(message)s',
        )
    visualize(**vars(parser.parse_args()))


if __name__ == "__main__":
    main()
