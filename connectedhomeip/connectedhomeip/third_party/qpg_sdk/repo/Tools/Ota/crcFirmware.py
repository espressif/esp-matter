"""
This tool will calculate a crc of the relevant part of a hex file from a given and
patch it into the User License.
The size will be determined by the intelhex module by reading out the max address in the HEX file.
Next to hashing the application data, this module will also hash:
* The VPP in the user-license
* <entries to be added>
* <entries to be added>
* <entries to be added>
"""

import argparse
import os
import struct
import sys
import logging

from intelhex import IntelHex
from binascii import crc32

# CONSTANTS
USER_LICENSE_CRC_VALUE_OFFSET = 0x10
USER_LICENSE_CRC_START_ADDRESS_MSB_OFFSET = 0x14
USER_LICENSE_CRC_START_ADDRESS_LSB_OFFSET = 0x16
USER_LICENSE_CRC_TYPE_OFFSET = 0x17
USER_LICENSE_CRC_SIZE_OFFSET = 0x18
USER_LICENSE_VPP_OFFSET = 0x1C

USER_LICENSE_LOAD_COMPLETED_OFFSET = 0x78
USER_LICENSE_FRESHNESS_COUNTER_OFFSET = 0x7F

USER_LICENSE_BULK_ERASE_LOCK_MAGIC_WORD_OFFSET = 0x28
USER_LICENSE_BULK_ERASE_LOCK_MAGIC_WORD = 0xA2693A5C

LOADED_USER_LICENSE_SIZE = 0x80
EXTENDED_USER_LICENSE_SIZE = 0x80
USER_LICENSE_FULL_SIZE = LOADED_USER_LICENSE_SIZE + EXTENDED_USER_LICENSE_SIZE

EXTENDED_USER_LICENSE_SIGNATURE_OFFSET = (LOADED_USER_LICENSE_SIZE + 0x28)
EXTENDED_USER_LICENSE_SIGNATURE_SIZE = 64

EXTENDED_USER_LICENSE_SECTION_1_ADDRESS_OFFSET = 0x98
EXTENDED_USER_LICENSE_SECTION_1_SIZE_OFFSET = (EXTENDED_USER_LICENSE_SECTION_1_ADDRESS_OFFSET + 4)

USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD = 0x693A5C81

CRC_START_ADDRESS_MSB_OOB = 0xFFFF
CRC_START_ADDRESS_LSB_OOB = 0xFF
CRC_TYPE_32BIT_CRC = 1
CRC_SIZE_OOB = 0X00000000


def add_crc(intel_hex_file, start_addr_app, crc_pad_upto):
    """
    add_crc adds a CRC over a specified image to the specified Intel HEX file object.
    """
    if crc_pad_upto:
        crc_end_addr = start_addr_app + int(crc_pad_upto, 16)
    else:
        crc_end_addr = intel_hex_file.maxaddr()

    crc_range_start = start_addr_app + USER_LICENSE_VPP_OFFSET

    # Type is 32-bit CRC
    intel_hex_file[start_addr_app + USER_LICENSE_CRC_TYPE_OFFSET] = CRC_TYPE_32BIT_CRC

    # Start Address is determined out of band, thus 0xFFFF
    ihex_set_uint16(intel_hex_file,
                    start_addr_app + USER_LICENSE_CRC_START_ADDRESS_MSB_OFFSET,
                    CRC_START_ADDRESS_MSB_OOB)
    intel_hex_file[start_addr_app + USER_LICENSE_CRC_START_ADDRESS_LSB_OFFSET] = CRC_START_ADDRESS_LSB_OOB

    # Size is determined out of band, thus 0x00000000
    ihex_set_uint32(intel_hex_file,
                    start_addr_app + USER_LICENSE_CRC_SIZE_OFFSET,
                    CRC_SIZE_OOB)

    # Gather data to calculate checksum over
    data = bytearray()
    for j in range(crc_range_start, crc_end_addr):

        # Mask out Load Complete MW
        if (j == (start_addr_app + 0x78)
           or j == (start_addr_app + 0x79)
           or j == (start_addr_app + 0x7A)
           or j == (start_addr_app + 0x7B)
           or j == (start_addr_app + 0x7F)):
            # Mask out freshness counter entry
            data.append(0x00)
        else:
            data.append(intel_hex_file[j])

    crcvalue = (~crc32(memoryview(data)) ^ 0xFFFFFFFF) & 0xFFFFFFFF

    logging.info("CRC range: %x - %x" % (crc_range_start, crc_end_addr))
    logging.info("Adding crc value %x" % crcvalue)

    # Write checksum to license area
    ihex_set_uint32(intel_hex_file, start_addr_app + USER_LICENSE_CRC_VALUE_OFFSET, crcvalue)


# Adds a CRC over both sections and the user license
def add_crc_sections(intel_hex_file, license_address, image):
    # Calculate CRC over image
    crcvalue = (~crc32(memoryview(image)) ^ 0xFFFFFFFF) & 0xFFFFFFFF
    logging.info("Adding crc value 0x%x" % crcvalue)

    # Type is 32-bit CRC
    intel_hex_file[license_address + USER_LICENSE_CRC_TYPE_OFFSET] = CRC_TYPE_32BIT_CRC

    # Start Address is determined out of band, thus 0xFFFF
    ihex_set_uint16(intel_hex_file,
                    license_address + USER_LICENSE_CRC_START_ADDRESS_MSB_OFFSET,
                    CRC_START_ADDRESS_MSB_OOB)
    intel_hex_file[license_address + USER_LICENSE_CRC_START_ADDRESS_LSB_OFFSET] = CRC_START_ADDRESS_LSB_OOB

    # Size is determined out of band, thus 0x00000000
    ihex_set_uint32(intel_hex_file,
                    license_address + USER_LICENSE_CRC_SIZE_OFFSET,
                    CRC_SIZE_OOB)

    # Write checksum to license area
    ihex_set_uint32(intel_hex_file, license_address + USER_LICENSE_CRC_VALUE_OFFSET, crcvalue)


def create_intel_hex_file_object(hex_filename):
    """
    create_intel_hex_file_object creates an Intel HEX object to read and modify
    without altering the HEX file.
    """
    intel_hex_file = IntelHex(hex_filename)
    intel_hex_file.padding = 0x00  # default is 0xFF
    intel_hex_file.writePath = hex_filename

    logging.info("Hex contains addresses 0x%08x to 0x%08x" % (intel_hex_file.minaddr(),
                                                              intel_hex_file.maxaddr()))

    return intel_hex_file


def get_license_data_to_hash(intel_hex_file, start_addr_license):
    image = bytearray()

    start_offset = start_addr_license + USER_LICENSE_VPP_OFFSET

    start_signature_offset = (start_addr_license
                              + EXTENDED_USER_LICENSE_SIGNATURE_OFFSET)

    end_signature_offset = (start_addr_license
                            + EXTENDED_USER_LICENSE_SIGNATURE_OFFSET
                            + EXTENDED_USER_LICENSE_SIGNATURE_SIZE)

    start_load_completed_mw_offset = (start_addr_license
                                      + USER_LICENSE_LOAD_COMPLETED_OFFSET)

    end_load_completed_mw_offset = (start_addr_license
                                    + USER_LICENSE_LOAD_COMPLETED_OFFSET
                                    + 4)

    start_freshness_counter_offset = (start_addr_license
                                      + USER_LICENSE_FRESHNESS_COUNTER_OFFSET - 3)

    stop_freshness_counter_offset = (start_addr_license
                                     + USER_LICENSE_FRESHNESS_COUNTER_OFFSET
                                     + 1)

    end_offset = start_addr_license + USER_LICENSE_FULL_SIZE
    logging.info("adding license [0x%lx,0x%lx]" % (start_offset, end_offset))

    for i in range(start_offset, end_offset):
        # Zero out the signature
        if (start_signature_offset <= i < end_signature_offset) or \
                (start_load_completed_mw_offset <= i < end_load_completed_mw_offset) or \
                (start_freshness_counter_offset <= i < stop_freshness_counter_offset):
            image.append(0x00)
        else:
            image.append(intel_hex_file[i])

    return image


def get_section_data_to_hash(intel_hex_file, start_addr, size, add_padding):
    logging.info("adding section [0x%lx,0x%lx] (size 0x%lx bytes) padding: %r",
                 start_addr, start_addr + size, size, add_padding)
    image = bytearray()
    for i in range(start_addr, start_addr + size):
        if add_padding:
            # Add zero byte padding redundantly
            if intel_hex_file[i] == 0x00:
                intel_hex_file[i] = 0x00
        image.append(intel_hex_file[i])

    return image


def add_section(intel_hex_file, start_area, license_offset, section_number, argument, padding):
    if argument is None:
        offset = 0xFFFFFFFF
        size = 0xFFFFFFFF
    else:
        offset = int(argument.split(':')[0], 16)
        size = int(argument.split(':')[1], 16)

    image = bytearray()

    if offset != 0xFFFFFFFF:
        if size == 0xFFFFFFFF:
            logging.info("Taking maximum address from HEX file to calculate and align size of section with offset 0x%lx"
                         % offset)
            # # Use maximum hex file address
            # size = align_section_size((intel_hex_file.maxaddr() - (start_area + offset)), ALIGNMENT_SIZE)
            size = intel_hex_file.maxaddr() - (start_area + offset) + 1

        logging.info("Setting section %d - start = %#lx - size = %#x = %d", section_number + 1, start_area + offset,
                     size, size)

        image.extend(get_section_data_to_hash(intel_hex_file, start_area + offset, size, padding))

    ihex_set_uint32(intel_hex_file,
                    start_area + license_offset + EXTENDED_USER_LICENSE_SECTION_1_ADDRESS_OFFSET + (section_number * 8),
                    offset)
    ihex_set_uint32(intel_hex_file,
                    start_area + license_offset + EXTENDED_USER_LICENSE_SECTION_1_SIZE_OFFSET + (section_number * 8),
                    size)

    return image


def ihex_set_uint16(intel_hex_file, offset, value):
    """
    ihex_set_uint16 sets the 16 bit value at a specified offset inside the Intel HEX file object.
    """
    _, _, third, fourth = struct.unpack('>BBBB', struct.pack('>I', value))

    intel_hex_file[offset + 0] = fourth
    intel_hex_file[offset + 1] = third


def ihex_set_uint32(intel_hex_file, offset, value):
    """
    ihex_set_uint32 sets the 32 bit value at a specified offset inside the Intel HEX file object.
    """
    first, second, third, fourth = struct.unpack('>BBBB', struct.pack('>I', value))

    intel_hex_file[offset + 0] = fourth
    intel_hex_file[offset + 1] = third
    intel_hex_file[offset + 2] = second
    intel_hex_file[offset + 3] = first


def parse_command_line_arguments():
    """
    parse_command_line_arguments parses the command line arguments of the signfw
    application.
    """
    parser = argparse.ArgumentParser()

    parser.add_argument("--hex",
                        help="path to hex file to be crced")
    parser.add_argument("--output",
                        help="output file to be written")
    parser.add_argument("--start_addr_area",
                        help="start address of app area")

    parser.add_argument("--add_padding",
                        help="add zero padding to complete hex file, to prevent any gaps",
                        action='store_true')

    parser.add_argument("--add_crc",
                        help="add crc calculation",
                        action='store_true')
    parser.add_argument("--crc_start_addr",
                        help="optional: define start address for CRC calculation")
    parser.add_argument("--crc_pad_upto",
                        help="optional: add padding to this address for CRC calculation")

    parser.add_argument("--set_bootloader_loaded",
                        help="set USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD",
                        action='store_true')
    parser.add_argument("--set_user_license_bulk_erase_lock_magic_word",
                        help="set user license bulk erase lock magic word",
                        action='store_true')

    parser.add_argument("--license_offset",
                        help="offset relative to start_addr_area where the user license begins")

    parser.add_argument("--section1",
                        help="<offset>:<size> - Section 1 to hash")

    parser.add_argument("--section2",
                        help="<offset>:<size> - Section 2 to hash")

    args = parser.parse_args()
    if not args.hex:
        logging.error("Supply HEX file path")
        sys.exit()

    if args.license_offset is None:
        logging.warning("Using non-licensed approach")

        if not args.crc_start_addr:
            args.crc_start_addr = "0x04000000"
            logging.warning("Picking default crc start address")

    if not args.start_addr_area:
        args.start_addr_area = "0x04000000"
        logging.warning("Picking default start address")

    if args.add_padding:
        logging.info("Adding zero padding to complete HEX file")

    return args


def main():
    """
    main is the entry point of the signfw application.
    """
    logging.basicConfig(level=logging.INFO)

    args = parse_command_line_arguments()

    ###############################################

    logging.info("Reading Hex file: %s" % os.path.basename(args.hex))

    intel_hex_file = create_intel_hex_file_object(args.hex)

    start_addr_area = int(args.start_addr_area, 16)

    image = bytearray()

    if args.license_offset is not None:
        license_offset = int(args.license_offset, 16)
        license_address = start_addr_area + license_offset

        logging.info("Start Address = 0x%08x" % start_addr_area)
        logging.info("License located @ 0x%lx" % license_address)

        assert intel_hex_file.maxaddr() > (start_addr_area + license_offset)

        image.extend(add_section(intel_hex_file, start_addr_area, license_offset, 0, args.section1, args.add_padding))
        image.extend(add_section(intel_hex_file, start_addr_area, license_offset, 1, args.section2, args.add_padding))
        image.extend(get_license_data_to_hash(intel_hex_file, license_address))

    if args.set_bootloader_loaded:
        if 'license_address' not in locals():
            license_address = start_addr_area
        logging.info("populating USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD at %#x",
                     license_address + USER_LICENSE_LOAD_COMPLETED_OFFSET)
        ihex_set_uint32(intel_hex_file,
                        license_address + USER_LICENSE_LOAD_COMPLETED_OFFSET,
                        USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD)
    if args.set_user_license_bulk_erase_lock_magic_word:
        logging.info("populating USER_LICENSE_BULK_ERASE_LOCK_MAGIC_WORD")
        ihex_set_uint32(intel_hex_file,
                        start_addr_area + USER_LICENSE_BULK_ERASE_LOCK_MAGIC_WORD_OFFSET,
                        USER_LICENSE_BULK_ERASE_LOCK_MAGIC_WORD)

    if args.add_crc:
        if args.crc_start_addr:
            crc_start_addr = int(args.crc_start_addr, 16)
            add_crc(intel_hex_file, crc_start_addr, args.crc_pad_upto)
        else:
            add_crc_sections(intel_hex_file, license_address, image)

    if args.output:
        intel_hex_file.writePath = args.output

    # Overwrite the hex file with the modified (CRC'd) intel hex object
    logging.info("Writing Hex file: %s", intel_hex_file.writePath)
    intel_hex_file.tofile(intel_hex_file.writePath, format='hex')


if __name__ == "__main__":
    main()
