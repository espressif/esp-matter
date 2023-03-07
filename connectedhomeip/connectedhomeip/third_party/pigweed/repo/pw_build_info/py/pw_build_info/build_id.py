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
"""Library that parses an ELF file for a GNU build-id."""

import argparse
import logging
from pathlib import Path
import sys
from typing import BinaryIO, Optional
import elftools  # type: ignore
from elftools.elf import elffile, notes, sections  # type: ignore

_LOG = logging.getLogger('build_id_parser')
_PW_BUILD_ID_SYM_NAME = 'gnu_build_id_begin'


class GnuBuildIdError(Exception):
    """An exception raised when a GNU build ID is malformed."""


def read_build_id_from_section(elf_file: BinaryIO) -> Optional[bytes]:
    """Reads a build ID from a .note.gnu.build-id section."""
    parsed_elf_file = elffile.ELFFile(elf_file)
    build_id_section = parsed_elf_file.get_section_by_name('.note.gnu.build-id')

    if build_id_section is None:
        return None

    section_notes = list(
        n
        for n in notes.iter_notes(
            parsed_elf_file,
            build_id_section['sh_offset'],
            build_id_section['sh_size'],
        )
    )

    if len(section_notes) != 1:
        raise GnuBuildIdError('GNU build ID section contains multiple notes')

    build_id_note = section_notes[0]
    if build_id_note['n_name'] != 'GNU':
        raise GnuBuildIdError('GNU build ID note name invalid')

    if build_id_note['n_type'] != 'NT_GNU_BUILD_ID':
        raise GnuBuildIdError('GNU build ID note type invalid')

    return bytes.fromhex(build_id_note['n_desc'])


def _addr_is_in_segment(addr: int, segment) -> bool:
    """Checks if the provided address resides within the provided segment."""
    # Address references uninitialized memory. Can't read.
    if addr >= segment['p_vaddr'] + segment['p_filesz']:
        raise GnuBuildIdError('GNU build ID is runtime-initialized')

    return addr in range(segment['p_vaddr'], segment['p_memsz'])


def _read_build_id_from_offset(elf, offset: int) -> bytes:
    """Attempts to read a GNU build ID from an offset in an elf file."""
    note = elftools.common.utils.struct_parse(
        elf.structs.Elf_Nhdr, elf.stream, stream_pos=offset
    )
    elf.stream.seek(offset + elf.structs.Elf_Nhdr.sizeof())
    name = elf.stream.read(note['n_namesz'])

    if name != b'GNU\0':
        raise GnuBuildIdError('GNU build ID note name invalid')

    return elf.stream.read(note['n_descsz'])


def read_build_id_from_symbol(elf_file: BinaryIO) -> Optional[bytes]:
    """Reads a GNU build ID using gnu_build_id_begin to locate the data."""
    parsed_elf_file = elffile.ELFFile(elf_file)

    matching_syms = None
    for section in parsed_elf_file.iter_sections():
        if not isinstance(section, sections.SymbolTableSection):
            continue
        matching_syms = section.get_symbol_by_name(_PW_BUILD_ID_SYM_NAME)
        if matching_syms is not None:
            break
    if matching_syms is None:
        return None

    if len(matching_syms) != 1:
        raise GnuBuildIdError('Multiple GNU build ID start symbols defined')

    gnu_build_id_sym = matching_syms[0]
    section_number = gnu_build_id_sym['st_shndx']

    if section_number == 'SHN_UNDEF':
        raise GnuBuildIdError('GNU build ID start symbol undefined')

    matching_section = parsed_elf_file.get_section(section_number)

    build_id_start_addr = gnu_build_id_sym['st_value']
    for segment in parsed_elf_file.iter_segments():
        if segment.section_in_segment(matching_section):
            offset = (
                build_id_start_addr - segment['p_vaddr'] + segment['p_offset']
            )
            return _read_build_id_from_offset(parsed_elf_file, offset)

    return None


def read_build_id(elf_file: BinaryIO) -> Optional[bytes]:
    """Reads a GNU build ID from an ELF binary."""
    # Prefer to read the build ID from a dedicated section.
    maybe_build_id = read_build_id_from_section(elf_file)
    if maybe_build_id is not None:
        return maybe_build_id

    # If there's no dedicated section, try and use symbol information to find
    # the build info.
    return read_build_id_from_symbol(elf_file)


def find_matching_elf(uuid: bytes, search_dir: Path) -> Optional[Path]:
    """Recursively searches a directory for an ELF file with a matching UUID."""
    elf_file_paths = search_dir.glob('**/*.elf')
    for elf_file in elf_file_paths:
        try:
            candidate_id = read_build_id(open(elf_file, 'rb'))
        except GnuBuildIdError:
            continue
        if candidate_id is None:
            continue
        if candidate_id == uuid:
            return elf_file

    return None


def _main(elf_file: BinaryIO) -> int:
    logging.basicConfig(format='%(message)s', level=logging.INFO)
    build_id = read_build_id(elf_file)
    if build_id is None:
        _LOG.error('Error: No GNU build ID found.')
        return 1

    _LOG.info(build_id.hex())
    return 0


def _parse_args():
    """Parses command-line arguments."""

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        'elf_file',
        type=argparse.FileType('rb'),
        help='The .elf to parse build info from',
    )

    return parser.parse_args()


if __name__ == '__main__':
    sys.exit(_main(**vars(_parse_args())))
