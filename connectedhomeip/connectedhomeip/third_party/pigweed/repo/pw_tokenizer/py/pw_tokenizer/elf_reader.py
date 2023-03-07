#!/usr/bin/env python3
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
"""Reads data from ELF sections.

This module provides tools for dumping the contents of an ELF section. It can
also be used to read values at a particular address. A command line interface
for both of these features is provided.

This module supports any ELF-format file, including .o and .so files. This
module also has basic support for archive (.a) files. All ELF files in an
archive are read as one unit.
"""

import argparse
import collections
from pathlib import Path
import re
import struct
import sys
from typing import (
    BinaryIO,
    Iterable,
    Mapping,
    NamedTuple,
    Optional,
    Pattern,
    Tuple,
    Union,
)

ARCHIVE_MAGIC = b'!<arch>\n'
ELF_MAGIC = b'\x7fELF'


def _check_next_bytes(fd: BinaryIO, expected: bytes, what: str) -> None:
    actual = fd.read(len(expected))
    if expected != actual:
        raise FileDecodeError(
            f'Invalid {what}: expected {expected!r}, found {actual!r} in file '
            f'{getattr(fd, "name", "(unknown")}'
        )


def files_in_archive(fd: BinaryIO) -> Iterable[int]:
    """Seeks to each file in an archive and yields its size."""

    _check_next_bytes(fd, ARCHIVE_MAGIC, 'archive magic number')

    while True:
        # In some archives, the first file ends with an additional \n. If that
        # is present, skip it.
        if fd.read(1) != b'\n':
            fd.seek(-1, 1)

        # Each file in an archive is prefixed with an ASCII header:
        #
        #   16 B - file identifier (text)
        #   12 B - file modification timestamp (decimal)
        #    6 B - owner ID (decimal)
        #    6 B - group ID (decimal)
        #    8 B - file mode (octal)
        #   10 B - file size in bytes (decimal)
        #    2 B - ending characters (`\n)
        #
        # Skip the unused portions of the file header, then read the size.
        fd.seek(16 + 12 + 6 + 6 + 8, 1)
        size_str = fd.read(10)
        if not size_str:
            return

        try:
            size = int(size_str, 10)
        except ValueError as exc:
            raise FileDecodeError(
                'Archive file sizes must be decimal integers'
            ) from exc

        _check_next_bytes(fd, b'`\n', 'archive file header ending')
        offset = fd.tell()  # Store offset in case the caller reads the file.

        yield size

        fd.seek(offset + size)


def _elf_files_in_archive(fd: BinaryIO):
    if _bytes_match(fd, ELF_MAGIC):
        yield  # The value isn't used, so just yield None.
    else:
        for _ in files_in_archive(fd):
            if _bytes_match(fd, ELF_MAGIC):
                yield


class Field(NamedTuple):
    """A field in an ELF file.

    Fields refer to a particular piece of data in an ELF file or section header.
    """

    name: str
    offset_32: int
    offset_64: int
    size_32: int
    size_64: int


class _FileHeader(NamedTuple):
    """Fields in the ELF file header."""

    section_header_offset: Field = Field('e_shoff', 0x20, 0x28, 4, 8)
    section_count: Field = Field('e_shnum', 0x30, 0x3C, 2, 2)
    section_names_index: Field = Field('e_shstrndx', 0x32, 0x3E, 2, 2)


FILE_HEADER = _FileHeader()


class _SectionHeader(NamedTuple):
    """Fields in an ELF section header."""

    section_name_offset: Field = Field('sh_name', 0x00, 0x00, 4, 4)
    section_address: Field = Field('sh_addr', 0x0C, 0x10, 4, 8)
    section_offset: Field = Field('sh_offset', 0x10, 0x18, 4, 8)
    section_size: Field = Field('sh_size', 0x14, 0x20, 4, 8)

    # section_header_end records the size of the header.
    section_header_end: Field = Field('section end', 0x28, 0x40, 0, 0)


SECTION_HEADER = _SectionHeader()


def read_c_string(fd: BinaryIO) -> bytes:
    """Reads a null-terminated string from the provided file descriptor."""
    string = bytearray()
    while True:
        byte = fd.read(1)
        if not byte or byte == b'\0':
            return bytes(string)
        string += byte


def _bytes_match(fd: BinaryIO, expected: bytes) -> bool:
    """Peeks at the next bytes to see if they match the expected."""
    try:
        offset = fd.tell()
        data = fd.read(len(expected))
        fd.seek(offset)
        return data == expected
    except IOError:
        return False


def compatible_file(file: Union[BinaryIO, str, Path]) -> bool:
    """True if the file type is supported (ELF or archive)."""
    try:
        fd = open(file, 'rb') if isinstance(file, (str, Path)) else file

        offset = fd.tell()
        fd.seek(0)
        result = _bytes_match(fd, ELF_MAGIC) or _bytes_match(fd, ARCHIVE_MAGIC)
        fd.seek(offset)
    finally:
        if isinstance(file, (str, Path)):
            fd.close()

    return result


class FileDecodeError(Exception):
    """Invalid data was read from an ELF file."""


class FieldReader:
    """Reads ELF fields defined with a Field tuple from an ELF file."""

    def __init__(self, elf: BinaryIO):
        self._elf = elf
        self.file_offset = self._elf.tell()

        _check_next_bytes(self._elf, ELF_MAGIC, 'ELF file header')
        size_field = self._elf.read(1)  # e_ident[EI_CLASS] (address size)

        int_unpacker = self._determine_integer_format()

        if size_field == b'\x01':
            self.offset = lambda field: field.offset_32
            self._size = lambda field: field.size_32
            self._decode = lambda f, d: int_unpacker[f.size_32].unpack(d)[0]
        elif size_field == b'\x02':
            self.offset = lambda field: field.offset_64
            self._size = lambda field: field.size_64
            self._decode = lambda f, d: int_unpacker[f.size_64].unpack(d)[0]
        else:
            raise FileDecodeError('Unknown size {!r}'.format(size_field))

    def _determine_integer_format(self) -> Mapping[int, struct.Struct]:
        """Returns a dict of structs used for converting bytes to integers."""
        endianness_byte = self._elf.read(1)  # e_ident[EI_DATA] (endianness)
        if endianness_byte == b'\x01':
            endianness = '<'
        elif endianness_byte == b'\x02':
            endianness = '>'
        else:
            raise FileDecodeError(
                'Unknown endianness {!r}'.format(endianness_byte)
            )

        return {
            1: struct.Struct(endianness + 'B'),
            2: struct.Struct(endianness + 'H'),
            4: struct.Struct(endianness + 'I'),
            8: struct.Struct(endianness + 'Q'),
        }

    def read(self, field: Field, base: int = 0) -> int:
        self._elf.seek(self.file_offset + base + self.offset(field))
        data = self._elf.read(self._size(field))
        return self._decode(field, data)

    def read_string(self, offset: int) -> str:
        self._elf.seek(self.file_offset + offset)
        return read_c_string(self._elf).decode()


class Elf:
    """Represents an ELF file and the sections in it."""

    class Section(NamedTuple):
        """Info about a section in an ELF file."""

        name: str
        address: int
        offset: int
        size: int

        file_offset: int  # Starting place in the file; 0 unless in an archive.

        def range(self) -> range:
            return range(self.address, self.address + self.size)

        def __lt__(self, other) -> bool:
            return self.address < other.address

    def __init__(self, elf: BinaryIO):
        self._elf = elf
        self.sections: Tuple[Elf.Section, ...] = tuple(self._list_sections())

    def _list_sections(self) -> Iterable['Elf.Section']:
        """Reads the section headers to enumerate all ELF sections."""
        for _ in _elf_files_in_archive(self._elf):
            reader = FieldReader(self._elf)
            base = reader.read(FILE_HEADER.section_header_offset)
            section_header_size = reader.offset(
                SECTION_HEADER.section_header_end
            )

            # Find the section with the section names in it.
            names_section_header_base = (
                base
                + section_header_size
                * reader.read(FILE_HEADER.section_names_index)
            )
            names_table_base = reader.read(
                SECTION_HEADER.section_offset, names_section_header_base
            )

            base = reader.read(FILE_HEADER.section_header_offset)
            for _ in range(reader.read(FILE_HEADER.section_count)):
                name_offset = reader.read(
                    SECTION_HEADER.section_name_offset, base
                )

                yield self.Section(
                    reader.read_string(names_table_base + name_offset),
                    reader.read(SECTION_HEADER.section_address, base),
                    reader.read(SECTION_HEADER.section_offset, base),
                    reader.read(SECTION_HEADER.section_size, base),
                    reader.file_offset,
                )

                base += section_header_size

    def section_by_address(self, address: int) -> Optional['Elf.Section']:
        """Returns the section that contains the provided address, if any."""
        # Iterate in reverse to give priority to sections with nonzero addresses
        for section in sorted(self.sections, reverse=True):
            if address in section.range():
                return section

        return None

    def sections_with_name(self, name: str) -> Iterable['Elf.Section']:
        for section in self.sections:
            if section.name == name:
                yield section

    def read_value(
        self, address: int, size: Optional[int] = None
    ) -> Union[None, bytes, int]:
        """Reads specified bytes or null-terminated string at address."""
        section = self.section_by_address(address)
        if not section:
            return None

        assert section.address <= address
        self._elf.seek(
            section.file_offset + section.offset + address - section.address
        )

        if size is None:
            return read_c_string(self._elf)

        return self._elf.read(size)

    def dump_sections(
        self, name: Union[str, Pattern[str]]
    ) -> Mapping[str, bytes]:
        """Returns a mapping of section names to section contents.

        If processing an archive with multiple object files, the contents of
        sections with duplicate names are concatenated in the order they appear
        in the archive.
        """
        name_regex = re.compile(name)

        sections: Mapping[str, bytearray] = collections.defaultdict(bytearray)
        for section in self.sections:
            if name_regex.match(section.name):
                self._elf.seek(section.file_offset + section.offset)
                sections[section.name].extend(self._elf.read(section.size))

        return sections

    def dump_section_contents(
        self, name: Union[str, Pattern[str]]
    ) -> Optional[bytes]:
        """Dumps a binary string containing the sections matching the regex.

        If processing an archive with multiple object files, the contents of
        sections with duplicate names are concatenated in the order they appear
        in the archive.
        """
        sections = self.dump_sections(name)
        return b''.join(sections.values()) if sections else None

    def summary(self) -> str:
        return '\n'.join(
            '[{0:2}] {1.address:08x} {1.offset:08x} {1.size:08x} '
            '{1.name}'.format(i, section)
            for i, section in enumerate(self.sections)
        )

    def __str__(self) -> str:
        return 'Elf({}\n)'.format(
            ''.join('\n  {},'.format(s) for s in self.sections)
        )


def _read_addresses(elf, size: int, output, address: Iterable[int]) -> None:
    for addr in address:
        value = elf.read_value(addr, size)

        if value is None:
            raise ValueError('Invalid address 0x{:08x}'.format(addr))

        output(value)


def _dump_sections(elf: Elf, output, sections: Iterable[Pattern[str]]) -> None:
    if not sections:
        output(elf.summary().encode())
        return

    for section_pattern in sections:
        output(elf.dump_section_contents(section_pattern))


def _parse_args() -> argparse.Namespace:
    """Parses and returns command line arguments."""
    parser = argparse.ArgumentParser(description=__doc__)

    def hex_int(arg):
        return int(arg, 16)

    parser.add_argument(
        '-e',
        '--elf',
        type=argparse.FileType('rb'),
        help='the ELF file to examine',
        required=True,
    )

    parser.add_argument(
        '-d',
        '--delimiter',
        default=ord('\n'),
        type=int,
        help=r'delimiter to write after each value; \n by default',
    )

    parser.set_defaults(handler=lambda **_: parser.print_help())

    subparsers = parser.add_subparsers(
        help='select whether to work with addresses or whole sections'
    )

    section_parser = subparsers.add_parser('section')
    section_parser.set_defaults(handler=_dump_sections)
    section_parser.add_argument(
        'sections',
        metavar='section_regex',
        nargs='*',
        type=re.compile,  # type: ignore
        help='section name regular expression',
    )

    address_parser = subparsers.add_parser('address')
    address_parser.set_defaults(handler=_read_addresses)
    address_parser.add_argument(
        '--size',
        type=int,
        help='the size to read; reads until a null terminator by default',
    )
    address_parser.add_argument(
        'address', nargs='+', type=hex_int, help='hexadecimal addresses to read'
    )

    return parser.parse_args()


def _main(args):
    """Calls the appropriate handler for the command line options."""
    handler = args.handler
    del args.handler

    delim = args.delimiter
    del args.delimiter

    def output(value):
        if value is not None:
            sys.stdout.buffer.write(value)
            sys.stdout.buffer.write(bytearray([delim]))
            sys.stdout.flush()

    args.output = output
    args.elf = Elf(args.elf)

    handler(**vars(args))


if __name__ == '__main__':
    _main(_parse_args())
