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
"""Tests the ELF reader Python module."""

import io
import os
import re
import unittest

from pw_tokenizer import elf_reader

# Output from the following command:
#
#   readelf -WS elf_reader_test_binary.elf
#
TEST_READELF_OUTPUT = """
There are 33 section headers, starting at offset 0x1758:

Section Headers:
  [Nr] Name              Type            Address          Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            0000000000000000 000000 000000 00      0   0  0
  [ 1] .interp           PROGBITS        0000000000000238 000238 00001c 00   A  0   0  1
  [ 2] .note.ABI-tag     NOTE            0000000000000254 000254 000020 00   A  0   0  4
  [ 3] .note.gnu.build-id NOTE            0000000000000274 000274 000024 00   A  0   0  4
  [ 4] .dynsym           DYNSYM          0000000000000298 000298 0000a8 18   A  5   1  8
  [ 5] .dynstr           STRTAB          0000000000000340 000340 00009b 00   A  0   0  1
  [ 6] .gnu.hash         GNU_HASH        00000000000003e0 0003e0 00001c 00   A  4   0  8
  [ 7] .gnu.version      VERSYM          00000000000003fc 0003fc 00000e 02   A  4   0  2
  [ 8] .gnu.version_r    VERNEED         000000000000040c 00040c 000020 00   A  5   1  4
  [ 9] .rela.dyn         RELA            0000000000000430 000430 0000d8 18   A  4   0  8
  [10] .rela.plt         RELA            0000000000000508 000508 000018 18  AI  4  12  8
  [11] .init             PROGBITS        0000000000000520 000520 000017 00  AX  0   0  4
  [12] .plt              PROGBITS        0000000000000540 000540 000020 10  AX  0   0 16
  [13] .text             PROGBITS        0000000000000560 000560 000151 00  AX  0   0 16
  [14] .fini             PROGBITS        00000000000006b4 0006b4 000009 00  AX  0   0  4
  [15] .rodata           PROGBITS        00000000000006c0 0006c0 000004 04  AM  0   0  4
  [16] .test_section_1   PROGBITS        00000000000006d0 0006d0 000010 00   A  0   0 16
  [17] .test_section_2   PROGBITS        00000000000006e0 0006e0 000004 00   A  0   0  4
  [18] .eh_frame         X86_64_UNWIND   00000000000006e8 0006e8 0000d4 00   A  0   0  8
  [19] .eh_frame_hdr     X86_64_UNWIND   00000000000007bc 0007bc 00002c 00   A  0   0  4
  [20] .fini_array       FINI_ARRAY      0000000000001d80 000d80 000008 08  WA  0   0  8
  [21] .init_array       INIT_ARRAY      0000000000001d88 000d88 000008 08  WA  0   0  8
  [22] .dynamic          DYNAMIC         0000000000001d90 000d90 000220 10  WA  5   0  8
  [23] .got              PROGBITS        0000000000001fb0 000fb0 000030 00  WA  0   0  8
  [24] .got.plt          PROGBITS        0000000000001fe0 000fe0 000020 00  WA  0   0  8
  [25] .data             PROGBITS        0000000000002000 001000 000010 00  WA  0   0  8
  [26] .tm_clone_table   PROGBITS        0000000000002010 001010 000000 00  WA  0   0  8
  [27] .bss              NOBITS          0000000000002010 001010 000001 00  WA  0   0  1
  [28] .comment          PROGBITS        0000000000000000 001010 00001d 01  MS  0   0  1
  [29] .note.gnu.gold-version NOTE            0000000000000000 001030 00001c 00      0   0  4
  [30] .symtab           SYMTAB          0000000000000000 001050 000390 18     31  21  8
  [31] .strtab           STRTAB          0000000000000000 0013e0 000227 00      0   0  1
  [32] .shstrtab         STRTAB          0000000000000000 001607 00014a 00      0   0  1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
  L (link order), O (extra OS processing required), G (group), T (TLS),
  C (compressed), x (unknown), o (OS specific), E (exclude),
  l (large), p (processor specific)
"""

TEST_ELF_PATH = os.path.join(
    os.path.dirname(__file__), 'elf_reader_test_binary.elf'
)


class ElfReaderTest(unittest.TestCase):
    """Tests the elf_reader.Elf class."""

    def setUp(self) -> None:
        super().setUp()
        self._elf_file = open(TEST_ELF_PATH, 'rb')
        self._elf = elf_reader.Elf(self._elf_file)

    def tearDown(self) -> None:
        super().tearDown()
        self._elf_file.close()

    def _section(self, name) -> elf_reader.Elf.Section:
        return next(iter(self._elf.sections_with_name(name)))

    def test_readelf_comparison_using_the_readelf_binary(self) -> None:
        """Compares elf_reader to readelf's output."""

        parse_readelf_output = re.compile(
            r'\s+'
            r'\[\s*(?P<number>\d+)\]\s+'
            r'(?P<name>\.\S*)?\s+'
            r'(?P<type>\S+)\s+'
            r'(?P<addr>[0-9a-fA-F]+)\s+'
            r'(?P<offset>[0-9a-fA-F]+)\s+'
            r'(?P<size>[0-9a-fA-F]+)\s+'
        )

        readelf_sections = []
        for number, name, _, addr, offset, size in parse_readelf_output.findall(
            TEST_READELF_OUTPUT
        ):
            readelf_sections.append(
                (
                    int(number),
                    name or '',
                    int(addr, 16),
                    int(offset, 16),
                    int(size, 16),
                )
            )

        self.assertEqual(len(readelf_sections), 33)
        self.assertEqual(len(readelf_sections), len(self._elf.sections))

        for (index, section), readelf_section in zip(
            enumerate(self._elf.sections), readelf_sections
        ):
            readelf_index, name, address, offset, size = readelf_section

            self.assertEqual(index, readelf_index)
            self.assertEqual(section.name, name)
            self.assertEqual(section.address, address)
            self.assertEqual(section.offset, offset)
            self.assertEqual(section.size, size)

    def test_dump_single_section(self) -> None:
        self.assertEqual(
            self._elf.dump_section_contents(r'\.test_section_1'),
            b'You cannot pass\0',
        )
        self.assertEqual(
            self._elf.dump_section_contents(r'\.test_section_2'),
            b'\xef\xbe\xed\xfe',
        )

    def test_dump_multiple_sections(self) -> None:
        if (
            self._section('.test_section_1').address
            < self._section('.test_section_2').address
        ):
            contents = b'You cannot pass\0\xef\xbe\xed\xfe'
        else:
            contents = b'\xef\xbe\xed\xfeYou cannot pass\0'

        self.assertIn(
            self._elf.dump_section_contents(r'.test_section_\d'), contents
        )

    def test_read_values(self) -> None:
        address = self._section('.test_section_1').address
        self.assertEqual(self._elf.read_value(address), b'You cannot pass')

        int32_address = self._section('.test_section_2').address
        self.assertEqual(
            self._elf.read_value(int32_address, 4), b'\xef\xbe\xed\xfe'
        )

    def test_read_string(self) -> None:
        bytes_io = io.BytesIO(
            b'This is a null-terminated string\0No terminator!'
        )
        self.assertEqual(
            elf_reader.read_c_string(bytes_io),
            b'This is a null-terminated string',
        )
        self.assertEqual(elf_reader.read_c_string(bytes_io), b'No terminator!')
        self.assertEqual(elf_reader.read_c_string(bytes_io), b'')

    def test_compatible_file_for_elf(self) -> None:
        self.assertTrue(elf_reader.compatible_file(self._elf_file))
        self.assertTrue(elf_reader.compatible_file(io.BytesIO(b'\x7fELF')))

    def test_compatible_file_for_elf_start_at_offset(self) -> None:
        self._elf_file.seek(13)  # Seek ahead to get out of sync
        self.assertTrue(elf_reader.compatible_file(self._elf_file))
        self.assertEqual(13, self._elf_file.tell())

    def test_compatible_file_for_invalid_elf(self) -> None:
        self.assertFalse(elf_reader.compatible_file(io.BytesIO(b'\x7fELVESF')))


def _archive_file(data: bytes) -> bytes:
    return (
        'FILE ID 90123456'
        'MODIFIED 012'
        'OWNER '
        'GROUP '
        'MODE 678'
        f'{len(data):10}'  # File size -- the only part that's needed.
        '`\n'.encode() + data
    )


class ArchiveTest(unittest.TestCase):
    """Tests reading from archive files."""

    def setUp(self) -> None:
        super().setUp()

        with open(TEST_ELF_PATH, 'rb') as fd:
            self._elf_data = fd.read()

        self._archive_entries = b'blah', b'hello', self._elf_data

        self._archive_data = elf_reader.ARCHIVE_MAGIC + b''.join(
            _archive_file(f) for f in self._archive_entries
        )
        self._archive = io.BytesIO(self._archive_data)

    def test_compatible_file_for_archive(self) -> None:
        self.assertTrue(elf_reader.compatible_file(io.BytesIO(b'!<arch>\n')))
        self.assertTrue(elf_reader.compatible_file(self._archive))

    def test_compatible_file_for_invalid_archive(self) -> None:
        self.assertFalse(elf_reader.compatible_file(io.BytesIO(b'!<arch>')))

    def test_iterate_over_files(self) -> None:
        for expected, size in zip(
            self._archive_entries, elf_reader.files_in_archive(self._archive)
        ):
            self.assertEqual(expected, self._archive.read(size))

    def test_iterate_over_empty_archive(self) -> None:
        with self.assertRaises(StopIteration):
            next(iter(elf_reader.files_in_archive(io.BytesIO(b'!<arch>\n'))))

    def test_iterate_over_invalid_archive(self) -> None:
        with self.assertRaises(elf_reader.FileDecodeError):
            for _ in elf_reader.files_in_archive(
                io.BytesIO(b'!<arch>blah blahblah')
            ):
                pass

    def test_extra_newline_after_entry_is_ignored(self) -> None:
        archive = io.BytesIO(
            elf_reader.ARCHIVE_MAGIC
            + _archive_file(self._elf_data)
            + b'\n'
            + _archive_file(self._elf_data)
        )

        for size in elf_reader.files_in_archive(archive):
            self.assertEqual(self._elf_data, archive.read(size))

    def test_two_extra_newlines_parsing_fails(self) -> None:
        archive = io.BytesIO(
            elf_reader.ARCHIVE_MAGIC
            + _archive_file(self._elf_data)
            + b'\n\n'
            + _archive_file(self._elf_data)
        )

        with self.assertRaises(elf_reader.FileDecodeError):
            for size in elf_reader.files_in_archive(archive):
                self.assertEqual(self._elf_data, archive.read(size))

    def test_iterate_over_archive_with_invalid_size(self) -> None:
        data = elf_reader.ARCHIVE_MAGIC + _archive_file(b'$' * 3210)
        file = io.BytesIO(data)

        # Iterate over the file normally.
        for size in elf_reader.files_in_archive(file):
            self.assertEqual(b'$' * 3210, file.read(size))

        # Replace the size with a hex number, which is not valid.
        with self.assertRaises(elf_reader.FileDecodeError):
            for _ in elf_reader.files_in_archive(
                io.BytesIO(data.replace(b'3210', b'0x99'))
            ):
                pass

    def test_elf_reader_dump_single_section(self) -> None:
        elf = elf_reader.Elf(self._archive)
        self.assertEqual(
            elf.dump_section_contents(r'\.test_section_1'), b'You cannot pass\0'
        )
        self.assertEqual(
            elf.dump_section_contents(r'\.test_section_2'), b'\xef\xbe\xed\xfe'
        )

    def test_elf_reader_read_values(self) -> None:
        elf = elf_reader.Elf(self._archive)
        address = next(iter(elf.sections_with_name('.test_section_1'))).address
        self.assertEqual(elf.read_value(address), b'You cannot pass')

        int32_address = next(
            iter(elf.sections_with_name('.test_section_2'))
        ).address
        self.assertEqual(elf.read_value(int32_address, 4), b'\xef\xbe\xed\xfe')

    def test_elf_reader_duplicate_sections_are_concatenated(self) -> None:
        archive_data = elf_reader.ARCHIVE_MAGIC + b''.join(
            _archive_file(f) for f in [self._elf_data, self._elf_data]
        )
        elf = elf_reader.Elf(io.BytesIO(archive_data))

        self.assertEqual(
            elf.dump_section_contents(r'\.test_section_1'),
            b'You cannot pass\0You cannot pass\0',
        )
        self.assertEqual(
            elf.dump_section_contents(r'\.test_section_2'),
            b'\xef\xbe\xed\xfe' * 2,
        )


if __name__ == '__main__':
    unittest.main()
