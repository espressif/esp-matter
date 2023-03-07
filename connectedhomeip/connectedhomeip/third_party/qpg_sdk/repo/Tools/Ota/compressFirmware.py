"""
This tool will compress a binary format firmware file with lzma after applying padding up to a page
size multiple. .
This tool will also calculate the CRC over the relevant part of the binary file and patch it
into the Loaded User License. Alternatively, if secure boot is targetted, a signature will be
created and injected into the Extended User License
"""
import argparse
import os
import struct
import codecs
import sys
import logging
import hashlib
import lzma
from typing import Optional
from dataclasses import dataclass, field

from binascii import crc32
from ecdsa import NIST256p, NIST192p


if os.path.isfile(os.path.join(os.path.dirname(__file__), "crypto_utils.py")):
    # In the Matter DK, all python modules are exported to this script's directory
    import crypto_utils as crypto_utils
else:
    # When running from the Qorvo codebase, use the dependencies from original paths
    moduleroot = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
    sys.path.append(os.path.join(moduleroot, "comps"))
    from crypto_utils import crypto_utils
    sys.path.remove(os.path.join(moduleroot, "comps"))

    # Determine if we are an .exe or not
    if not getattr(sys, 'frozen', False):
        current_dir = os.path.dirname(os.path.realpath(__file__))
        parent_dir = os.path.dirname(current_dir)
        sys.path.append(os.path.join(parent_dir, "..", "..", "..", "..", "..", "Env", "vless", "gppy_vless", "inf"))
        from getEnvVersion import getEnvVersion

        try:
            envVersion = getEnvVersion()
            envPath = os.path.join(parent_dir, "..", "..", "..", "..", "..", "Env", envVersion)
        except Exception as e:
            # Fallback to ENV_PATH
            print("WARNING: getEnvVersion() failed, falling back to ENV_PATH")
            envPath = os.path.abspath(os.environ.get('ENV_PATH', ""))

        sec_deps_path = os.path.join(envPath, "gppy", "tools", "sec")
        sys.path.append(sec_deps_path)


# CONSTANTS
USER_LICENSE_CRC_VALUE_OFFSET = 0x10
USER_LICENSE_VPP_OFFSET = 0x1C

USER_LICENSE_LOAD_COMPLETED_OFFSET = 0x78
USER_LICENSE_FRESHNESS_COUNTER_OFFSET = 0x7F

EXTENDED_USER_LICENSE_OFFSET = 0x80
EXTENDED_USER_LICENSE_SIGNATURE_OFFSET = (EXTENDED_USER_LICENSE_OFFSET + 0x28)
EXTENDED_USER_LICENSE_SIGNATURE_SIZE = 64

EXTENDED_USER_LICENSE_SECTION_1_ADDRESS_OFFSET = 0x98
EXTENDED_USER_LICENSE_SECTION_1_SIZE_OFFSET = (EXTENDED_USER_LICENSE_SECTION_1_ADDRESS_OFFSET + 4)
EXTENDED_USER_LICENSE_SECTION_2_ADDRESS_OFFSET = (EXTENDED_USER_LICENSE_SECTION_1_SIZE_OFFSET + 4)
EXTENDED_USER_LICENSE_SECTION_2_SIZE_OFFSET = (EXTENDED_USER_LICENSE_SECTION_2_ADDRESS_OFFSET + 4)

LICENSE_SIZE = 0x100

CRC_START_ADDRESS_MSB_OOB = 0xFFFF
CRC_START_ADDRESS_LSB_OOB = 0xFF
CRC_TYPE_32BIT_CRC = 1
CRC_SIZE_OOB = 0X00000000

EXTENDED_USER_LICENSE_SECTION_NOT_IN_USE = 0xFFFFFFFF


@dataclass
class CompressFirmwareArguments:
    """helper to enforce type checking on argparse output"""
    input: str
    output: str
    add_crc: bool
    license_offset: int
    page_size: int
    sector_size: int
    pem: str
    pem_password: str
    x25519: bool
    x25519_private_key_binfile: str
    compression: str
    prune_only: bool
    ota_offset: int
    # application namespace variables
    license_sector_file: str = field(init=False)
    raw_app_content_file: str = field(init=False)
    jumptableFile: str = field(init=False)
    paddedBinFile: str = field(init=False)
    compressed_payload_file: str = field(init=False)
    license_sector: bytearray = field(init=False)
    section1_addr: int = field(init=False)
    section1_size: int = field(init=False)
    section2_addr: int = field(init=False)
    section2_size: int = field(init=False)
    jumptables_data: bytes = field(init=False)
    raw_app_contents: bytes = field(init=False)
    padded_data: bytes = field(init=False)
    compressed_app_data: bytes = field(init=False)


def read_input_file(args: CompressFirmwareArguments):
    """read the input file"""
    # Read file contents
    with open(args.input, 'rb') as binary_input_file:
        binary_input = binary_input_file.read(-1)

    args.license_sector = extract_license_sector(args, binary_input)

    parse_section_sizes(args)

    if args.section2_addr != EXTENDED_USER_LICENSE_SECTION_NOT_IN_USE:
        args.jumptables_data = extract_jumptables_contents(args, binary_input)

    args.raw_app_contents = extract_app_contents(args, binary_input)


def extract_license_sector(args: CompressFirmwareArguments, binary_input) -> bytearray:
    """Return the license sector"""
    return bytearray(binary_input[args.license_offset:
                                  args.license_offset + args.sector_size])


def parse_section_sizes(args: CompressFirmwareArguments) -> None:
    """parse section values from the license sector"""
    args.section1_addr = struct.unpack_from('I', args.license_sector, EXTENDED_USER_LICENSE_SECTION_1_ADDRESS_OFFSET)[0]
    args.section1_size = struct.unpack_from('I', args.license_sector, EXTENDED_USER_LICENSE_SECTION_1_SIZE_OFFSET)[0]
    args.section2_addr = struct.unpack_from('I', args.license_sector, EXTENDED_USER_LICENSE_SECTION_2_ADDRESS_OFFSET)[0]
    args.section2_size = struct.unpack_from('I', args.license_sector, EXTENDED_USER_LICENSE_SECTION_2_SIZE_OFFSET)[0]


def extract_jumptables_contents(args: CompressFirmwareArguments, binary_input: bytes) -> bytearray:
    """Extract the jumptables content from the input data."""
    # section1 starts after the userlicense, compensate for in offset calculation
    section1_user_license_skip_length = 0x100
    start_offset = args.section1_addr - args.license_offset - section1_user_license_skip_length
    jumptables_start_offset = 0x800 - start_offset
    return bytearray(binary_input[jumptables_start_offset:
                                  jumptables_start_offset + args.section2_size])


def extract_app_contents(args: CompressFirmwareArguments, binary_input: bytes) -> bytearray:
    """Extract the application section from the input data."""
    return bytearray(binary_input[args.license_offset:len(binary_input)])


def write_intermediate_parts(args: CompressFirmwareArguments):
    """write out individual steps for debugging purposes"""
    logging.info("Dumping intemediate parts")

    if args.section2_addr != EXTENDED_USER_LICENSE_SECTION_NOT_IN_USE:
        # If jumptables are present
        with open(args.jumptableFile, 'wb') as jump_tables_file:
            jump_tables_file.write(bytes(args.jumptables_data))
        logging.info("Written jumptables to %s", args.jumptableFile)

    with open(args.license_sector_file, 'wb') as license_file:  # obsolete intermediate
        license_file.write(bytes(args.license_sector))
        logging.info("Written license sector to %s", args.license_sector_file)

    with open(args.raw_app_content_file, 'wb') as app_bin_file:
        app_bin_file.write(bytes(args.raw_app_contents))
        logging.info("Written application to %s", args.raw_app_content_file)

    with open(args.paddedBinFile, 'wb') as padded_bin_file:
        padded_bin_file.write(bytes(args.padded_data))
        logging.info("Written padded file to %s", args.paddedBinFile)

    if args.compression != 'none':
        with open(args.compressed_payload_file, 'wb') as compressed_payload_file:
            compressed_payload_file.write(bytes(args.compressed_app_data))
            logging.info("Written compressed payload to %s", args.compressed_payload_file)


def add_crc(args: CompressFirmwareArguments):
    """Calculate crc over data"""
    data = bytearray()

    # Add application part section 1
    data.extend(args.compressed_app_data)

    # Add application part section 2
    if args.section2_addr != EXTENDED_USER_LICENSE_SECTION_NOT_IN_USE:
        data.extend(args.jumptables_data)

    # Add license part
    for i in range(USER_LICENSE_VPP_OFFSET, LICENSE_SIZE):
        # Mask out Load Complete MW
        ulc_offsets = range(USER_LICENSE_LOAD_COMPLETED_OFFSET, USER_LICENSE_LOAD_COMPLETED_OFFSET + 4)
        if i in [*ulc_offsets, USER_LICENSE_FRESHNESS_COUNTER_OFFSET]:
            # Mask out data
            data.append(0x00)
        else:
            data.append(args.license_sector[i])

    crcvalue = (~crc32(memoryview(data)) ^ 0xFFFFFFFF) & 0xFFFFFFFF

    # Write checksum to license area
    struct.pack_into('IHBBI', args.license_sector, USER_LICENSE_CRC_VALUE_OFFSET, crcvalue,
                     CRC_START_ADDRESS_MSB_OOB, CRC_START_ADDRESS_LSB_OOB, CRC_TYPE_32BIT_CRC, CRC_SIZE_OOB)
    logging.info("Packed CRC %#x", crcvalue)


def calculate_signature(image: bytes, pemfile_path: str, password):
    """Calculate the signature over a specified image using the private key
    contained in the specified file.
    """
    # Retrieve information from PEM file
    logging.info("Reading PEM file: %s", os.path.basename(pemfile_path))

    (pem_curve, private_key, public_key) = crypto_utils.getPrivatePublicKeysFromPEMFile(pemfile_path,
                                                                                        password)

    # Assure correct curves and hashes are being used together

    if pem_curve == "secp192r1":
        curve = NIST192p
        hash_function = hashlib.sha1
    elif pem_curve == "secp256r1":
        curve = NIST256p
        hash_function = hashlib.sha256
    else:
        assert False

    logging.info("Using signing curve: %s", curve.name)
    logging.info("PEM file has curve: %s", pem_curve)

    # print "INFO: Using signing curve: %x" % curve
    # print "INFO: PEM file has curve: %x" % pem_curve

    logging.info("Hashing and signing image")
    signature = crypto_utils.signMessage(image, private_key, curve=curve, hashfunc=hash_function)

    logging.info("=====================================")
    logging.info("Signature to be put in user license:")
    logging.info(crypto_utils.getCCodeBuffer(signature, "signature"))
    # crypto_utils.printCCodeBuffer(private_key, "private_key")
    logging.info("=====================================")
    logging.info("Public key to be put in bootloader:")
    logging.info(crypto_utils.getCCodeBuffer(public_key, "public_key"))
    logging.info("=====================================")

    return signature, public_key, curve, hash_function


def add_signature(args: CompressFirmwareArguments):
    """Add a signature over a specified image to the license sector."""
    # Gather data to calculate signature
    image = b''

    # Add application part section 1
    for byte in args.compressed_app_data:
        image += struct.pack("B", byte)

    if args.x25519:
        """ The ROM_aes_mmo_update function, which is used to hash the section, requires a multiple of 16-bytes as
        data to be hashed, so if the size of the section is not a multiple of 16 bytes, we add zero padding to get
        to this multiple"""

        remainder = len(image) % 16
        if remainder != 0:
            for i in range(0, 16 - remainder):
                image += struct.pack("B", 0x00)

            logging.info("Section size not a multiple of 16 as required by ROM_aes_mmo_update, adding %d zero bytes",
                         (16 - remainder))

        assert len(image) % 16 == 0

    # Add application part section 2 if needed
    if args.section2_addr != EXTENDED_USER_LICENSE_SECTION_NOT_IN_USE:
        for byte in args.jumptables_data:
            image += struct.pack("B", byte)

    if args.x25519:
        """ The ROM_aes_mmo_update function, which is used to hash the section, requires a multiple of 16-bytes as
        data to be hashed, so if the size of the section is not a multiple of 16 bytes, we add zero padding to get
        to this multiple"""

        remainder = len(image) % 16
        if remainder != 0:
            for i in range(0, 16 - remainder):
                image += struct.pack("B", 0x00)

            logging.info("Section size not a multiple of 16 as required by ROM_aes_mmo_update, adding %d zero bytes",
                         (16 - remainder))

        assert len(image) % 16 == 0

    # Add license part
    start_load_completed_mw_offset = USER_LICENSE_LOAD_COMPLETED_OFFSET
    end_load_completed_mw_offset = USER_LICENSE_LOAD_COMPLETED_OFFSET + 4
    start_freshness_counter_offset = USER_LICENSE_FRESHNESS_COUNTER_OFFSET - 3
    stop_freshness_counter_offset = USER_LICENSE_FRESHNESS_COUNTER_OFFSET + 1
    for i in range(USER_LICENSE_VPP_OFFSET, EXTENDED_USER_LICENSE_SIGNATURE_OFFSET):
        if (start_load_completed_mw_offset <= i < end_load_completed_mw_offset) or \
           (start_freshness_counter_offset <= i < stop_freshness_counter_offset):
            pass
        else:
            image += struct.pack("B", args.license_sector[i])

    if args.x25519:
        from aes_mmo import aes_mmo_hash
        from x25519 import x25519_sign_and_return_response

        aes_mmo_hash_buf = bytearray(image)

        print("AES-MMO hashing %d bytes total", len(aes_mmo_hash_buf))

        aes_mmo_result = aes_mmo_hash(aes_mmo_hash_buf)

        print("aes_mmo_hash:")
        print(' '.join(f'{byte_value:#02x}' for byte_value in aes_mmo_result))

        with open(args.x25519_private_key_binfile, "rb") as file_contents:
            private_key = file_contents.read(-1)

        signature = x25519_sign_and_return_response(aes_mmo_result, bytearray(private_key))
    else:
        signature, public_key, curve, hash_function = calculate_signature(
            image,
            args.pem,
            args.pem_password
        )

        if crypto_utils.verifyMessage(image, public_key, signature, curve=curve, hashfunc=hash_function):
            logging.info("SUCCESS: Message OK")
        else:
            logging.error("Message NOK")

    args.license_sector[
        EXTENDED_USER_LICENSE_SIGNATURE_OFFSET:
        EXTENDED_USER_LICENSE_SIGNATURE_OFFSET + EXTENDED_USER_LICENSE_SIGNATURE_SIZE
    ] = bytearray(signature)


def fill_license(args: CompressFirmwareArguments):
    """Transform the user license into the compressed user license by adjusting content"""
    logging.info("Filling the compressed license")

    # Organize ota image as follows:
    # 0x1000 bytes before ota_space is reserved for jumptables
    # first sector will be used for user license
    # the actual compressed image starts afterwards and length should be set to compressed image size
    section1_addr = args.ota_offset + args.sector_size
    section1_size = len(args.compressed_app_data)
    section2_size = args.section2_size
    if args.section2_addr != EXTENDED_USER_LICENSE_SECTION_NOT_IN_USE:
        section2_addr = args.ota_offset - section2_size
    else:
        section2_addr = args.section2_addr

    # pack the calculated values into the binary data + set vpp to ota_area location offset
    struct.pack_into('IIII', args.license_sector, EXTENDED_USER_LICENSE_SECTION_1_ADDRESS_OFFSET,
                     section1_addr, section1_size, section2_addr, section2_size)
    logging.info("Packed sections in compressed license: section1 %#x:%#x, section2 %#x:%#x",
                 section1_addr, section1_size, section2_addr, section2_size)


def write_combined_output_image(args: CompressFirmwareArguments):
    """Combine all binary files to one output file"""
    logging.info("Combining all binary files to one output file")

    # Overwrite file with compressed data with data in the right order
    with open(args.output, 'wb') as combined_file:
        if args.section2_addr != EXTENDED_USER_LICENSE_SECTION_NOT_IN_USE:
            combined_file.write(bytes(args.jumptables_data))
        if args.license_offset is not None and not args.prune_only:
            combined_file.write(bytes(args.license_sector))
        combined_file.write(args.compressed_app_data)


def parse_command_line_arguments() -> CompressFirmwareArguments:
    """Parse the command line arguments of the application."""
    def base_16_int(string):
        return int(string, 16)

    def str_as_bytes(string):
        return codecs.encode(string, 'UTF-8')

    parser = argparse.ArgumentParser()

    parser.add_argument("--input",
                        help="path to bin file to be compressed")
    parser.add_argument("--output",
                        help="output bin file to be written")

    parser.add_argument("--add_crc",
                        help="add crc calculation",
                        action='store_true')

    parser.add_argument("--license_offset",
                        type=base_16_int,
                        help="offset relative to the start of the file where the user license begins")
    parser.add_argument("--ota_offset",
                        type=base_16_int,
                        help="offset of the ota area relative to the start of the flash")

    parser.add_argument("--page_size",
                        type=base_16_int,
                        help="the page size used in the target device flash")
    parser.add_argument("--sector_size",
                        type=base_16_int,
                        help="the sector size used in the target device flash")

    parser.add_argument("--pem",
                        help="path to PEM file to be signed")
    parser.add_argument("--pem_password",
                        type=str_as_bytes,
                        help="optional PEM file password")

    parser.add_argument("--x25519",
                        help="use AES-MMO + x25519 signing",
                        action='store_true')

    parser.add_argument("--x25519_private_key_binfile",
                        help="private key used for signing")

    parser.add_argument("--compression",
                        choices=['lzma', 'none'],
                        default="lzma",
                        help="compression type (default to lzma)")

    parser.add_argument("--prune_only",
                        help="prune unneeded sections; don't add an upgrade user license (external storage scenario)",
                        action='store_true')

    args = parser.parse_args()

    return CompressFirmwareArguments(**vars(args))


def validate_command_line_arguments(args: CompressFirmwareArguments) -> None:
    if not args.input:
        logging.error("Supply BIN file path")
        sys.exit(-1)

    if args.pem:
        if not args.pem_password:
            logging.error("Supply PEM file password path")
            sys.exit(-1)

    if not args.output:
        input_fn_parts = os.path.splitext(args.input)
        args.output = input_fn_parts[0] + ".compressed" + input_fn_parts[1]
        logging.warning("Setting default output file = %s", args.output)
    if args.license_offset is None:
        logging.info("Not using license approach")

    # Set file variables
    args.license_sector_file = os.path.splitext(args.input)[0] + ".license.bin"
    args.raw_app_content_file = os.path.splitext(args.input)[0] + ".application-extracted.bin"
    args.jumptableFile = os.path.splitext(args.input)[0] + ".jumptables.bin"
    args.paddedBinFile = os.path.splitext(args.input)[0] + ".application-after-padding.bin"
    args.compressed_payload_file = os.path.splitext(args.input)[0] + ".application-compressed-out.bin.lzma"


def lzma_compress(input_data: bytes) -> bytes:
    """Compress with lzma and add a header containing the uncompressed size."""

    dictionary_size = 1 << 16
    lc_value = 3  # -lc3 : set number of literal context bits : [0, 8] : default = 3
    lp_value = 0  # -lp0 : set number of literal pos bits : [0, 4] : default = 0
    pb_value = 2  # -pb2 : set number of pos bits : [0, 4] : default = 2

    ota_filters = [
        {"id": lzma.FILTER_LZMA1,
            "preset": 7 | lzma.PRESET_EXTREME,
            "dict_size": dictionary_size,
            "lc": lc_value,  # lc: Number of literal context bits.
            "lp": lp_value,  # lp: Number of literal position bits. The sum lc + lp must be at most 4.
            "pb": pb_value,  # pb: Number of position bits; must be at most 4.
         },
    ]

    properties = (pb_value * 5 + lp_value) * 9 + lc_value
    assert properties == 0x5d, f"properties is {properties:x} instead"

    compressor = lzma.LZMACompressor(format=lzma.FORMAT_RAW, filters=ota_filters)
    header = struct.pack("<BIQ",            # little-endian
                         properties,        # byte
                         dictionary_size,   # unsigned int
                         len(input_data),   # unsigned long long
                         )

    compressed_data = b''
    compressed_data += compressor.compress(input_data)
    compressed_data += compressor.flush()
    logging.info("input data length: %#x output data length: %#x", len(input_data), len(compressed_data))

    output = b''
    output += header
    output += compressed_data
    return output


def pad_to_page_size(page_size, input_data: bytes) -> bytes:
    """Pad the file to a page size multiple by adding 0's."""
    output = b''
    logging.info("length input data: %#x", len(input_data))
    output += input_data
    bytes_in_incomplete_page = len(input_data) % page_size
    if bytes_in_incomplete_page != 0:
        padding_size = page_size - bytes_in_incomplete_page
        output += b'\x00' * padding_size
    return output


def main(args: CompressFirmwareArguments):
    """the entry point of the application."""
    logging.basicConfig(level=logging.INFO)

    validate_command_line_arguments(args)

    if args.license_offset is not None:
        # Split off the jumptables and user license if needed
        read_input_file(args)
        data_to_pad = bytes(args.raw_app_contents)
    else:
        with open(args.input, 'rb') as input_file:
            data_to_pad = bytes(input_file.read(-1))

    args.padded_data = pad_to_page_size(args.page_size, data_to_pad)

    if args.compression == 'lzma':
        args.compressed_app_data = lzma_compress(args.padded_data)
    elif args.compression == 'none':
        args.compressed_app_data = args.padded_data
    else:
        raise Exception(f'Unsupported compression {args.compression}')

    if args.license_offset is None:
        with open(args.output, 'wb') as output_file:
            output_file.write(args.compressed_app_data)
    else:
        if not args.prune_only:
            fill_license(args)

        if args.add_crc:
            add_crc(args)

        if args.pem or args.x25519:
            add_signature(args)

        write_intermediate_parts(args)

        write_combined_output_image(args)


if __name__ == "__main__":
    args = parse_command_line_arguments()
    main(args)
