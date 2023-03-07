"""Create a signature of the hash of relevant parts of a hex file and patch it into the Extended User License.

The coverage of the hash will be determined by the intelhex module by reading out the max address in the HEX file.
Next to hashing the application data, this module will also hash:
* The VPP in the user-license
* <entries to be added>
* <entries to be added>
* <entries to be added>
"""
import argparse
import hashlib
import os
import struct
import codecs
import sys
import logging
from typing import Callable, Tuple
from dataclasses import dataclass, field

from ecdsa import NIST256p
from ecdsa.curves import Curve
from intelhex import IntelHex

if os.path.isfile(os.path.join(os.path.dirname(__file__), "crypto_utils.py")):
    # In the Matter DK, all python modules are exported to this script's directory
    import crypto_utils as crypto_utils
else:
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
USER_LICENSE_VPP_OFFSET = 0x1C

USER_LICENSE_LOAD_COMPLETED_OFFSET = 0x78
USER_LICENSE_FRESHNESS_COUNTER_OFFSET = 0x7F
USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD = 0x693A5C81

EXTENDED_USER_LICENSE_OFFSET = 0x80
EXTENDED_USER_LICENSE_SIGNATURE_OFFSET = (EXTENDED_USER_LICENSE_OFFSET + 0x28)
EXTENDED_USER_LICENSE_SIGNATURE_SIZE = 64

EXTENDED_USER_LICENSE_SECTION_1_ADDRESS_OFFSET = 0x98
EXTENDED_USER_LICENSE_SECTION_1_SIZE_OFFSET = (EXTENDED_USER_LICENSE_SECTION_1_ADDRESS_OFFSET + 4)

EXTENDED_USER_LICENSE_SIGNATURE_ALGORITHM_ECDSA = 0x01

EXTENDED_USER_LICENSE_CURVE_NONE = 0x00
EXTENDED_USER_LICENSE_CURVE_P256 = 0x01
EXTENDED_USER_LICENSE_CURVE_ED25519 = 0x02

EXTENDED_USER_LICENSE_HASH_ALGO_NONE = 0x00
EXTENDED_USER_LICENSE_HASH_ALGO_SHA256 = 0x01
EXTENDED_USER_LICENSE_SIG_SIZE_P256 = 64

EXTENDED_USER_LICENSE_SIGNATURE_ALGORITHM_OFFSET = 0x90
EXTENDED_USER_LICENSE_CURVE_SELECTION_OFFSET = 0x91
EXTENDED_USER_LICENSE_HASH_ALGORITHM_OFFSET = 0x92
EXTENDED_USER_LICENSE_SIGNATURE_SIZE_OFFSET = 0x93


@dataclass
class SignFirmwareArguments:
    """helper to enforce type checking on argparse output"""
    hex: str
    output: str
    pem: str
    pem_password: bytes
    start_addr_area: int
    add_padding: bool
    license_offset: int
    write_secureboot_public_key: bool
    set_bootloader_loaded: bool
    section1: Tuple[int, int]
    section2: Tuple[int, int]
    x25519: bool
    x25519_private_key_binfile: str
    # application namespace
    intel_hex_write_path: str = field(init=False)


@dataclass
class SigningInformation:
    """Information from a PEM file needed to sign data"""
    curve: Curve
    hash_function: Callable
    private_key: bytes
    public_key: bytes


def set_signature_metadata(intel_hex_file: IntelHex, license_address: int, sign_info: SigningInformation):
    """set signature parameters in the loaded user license"""

    def set_with_override_warning(offset, value, expected_default=0):
        value_found = intel_hex_file[license_address + offset]
        if value_found not in (value, expected_default):
            logging.warning("offset %#x already has non-default value %#x", offset, value_found)
        intel_hex_file[license_address + offset] = value

    set_with_override_warning(EXTENDED_USER_LICENSE_SIGNATURE_ALGORITHM_OFFSET,
                              EXTENDED_USER_LICENSE_SIGNATURE_ALGORITHM_ECDSA)

    if sign_info.curve == NIST256p:
        set_with_override_warning(EXTENDED_USER_LICENSE_CURVE_SELECTION_OFFSET, EXTENDED_USER_LICENSE_CURVE_P256)
    elif sign_info.curve is None:
        set_with_override_warning(EXTENDED_USER_LICENSE_CURVE_SELECTION_OFFSET, EXTENDED_USER_LICENSE_CURVE_NONE)
    else:
        raise Exception(f"unable to handle curve {sign_info.curve}")

    if sign_info.hash_function == hashlib.sha256:
        set_with_override_warning(EXTENDED_USER_LICENSE_HASH_ALGORITHM_OFFSET, EXTENDED_USER_LICENSE_HASH_ALGO_SHA256)
        set_with_override_warning(EXTENDED_USER_LICENSE_SIGNATURE_SIZE_OFFSET, EXTENDED_USER_LICENSE_SIG_SIZE_P256)
    elif sign_info.hash_function is None:
        set_with_override_warning(EXTENDED_USER_LICENSE_HASH_ALGORITHM_OFFSET, EXTENDED_USER_LICENSE_HASH_ALGO_NONE)
    else:
        raise Exception(f"unable to handle hash_function {sign_info.hash_function}")


def add_pem_signature(intel_hex_file: IntelHex, image: bytes, license_address: int,
                      sign_info: SigningInformation) -> bytes:
    """Calculate signature and add it to the hex file object, return it"""
    logging.info("Hashing and signing image")

    signature: bytes = crypto_utils.signMessage(image, sign_info.private_key, curve=sign_info.curve,
                                                hashfunc=sign_info.hash_function)

    # Write generated signature to hex file
    crypto_utils.addSignatureToIntelHexObject(intel_hex_file,
                                              signature,
                                              license_address + EXTENDED_USER_LICENSE_SIGNATURE_OFFSET)

    logging.info("=====================================")
    logging.info("Signature to be put in user license:")
    logging.info(crypto_utils.getCCodeBuffer(signature, "signature"))
    return signature


def add_secureboot_public_key(intel_hex_file: IntelHex, sign_info: SigningInformation,
                              secureboot_public_key_offset: int) -> None:
    """Add a public key at an offset to an intel hex file object"""
    logging.info("populating secureboot public key for the bootloader to use at %#x: %d",
                 secureboot_public_key_offset, len(sign_info.public_key))
    for index, value in enumerate(sign_info.public_key):
        intel_hex_file[secureboot_public_key_offset + index] = value


def load_pem_file(pem_file_path: str, pem_password: bytes) -> SigningInformation:
    """Load a PEM file and validate it is usable"""
    # Retrieve information from PEM file
    logging.info("Reading PEM file: %s", os.path.basename(pem_file_path))

    (pem_curve, private_key, public_key) = crypto_utils.getPrivatePublicKeysFromPEMFile(pem_file_path,
                                                                                        pem_password)

    # Assure correct curves and hashes are being used together

    logging.info("PEM file has curve: %s", pem_curve)

    if pem_curve == "secp256r1":
        curve = NIST256p
        hash_function = hashlib.sha256
    else:
        raise NotImplementedError(f"PEM file uses {pem_curve} instead of the expected 'secp256r1'")

    logging.info("Using signing curve: %s", curve.name)

    return SigningInformation(curve, hash_function, private_key, public_key)


def add_x25519_signature(intel_hex_file: IntelHex, image: bytes, license_address: int, args: SignFirmwareArguments):
    """Calculate signature and add it to the hex file object"""
    from aes_mmo import aes_mmo_hash
    from x25519 import x25519_sign_and_fill_in_signature

    signature_address = license_address + EXTENDED_USER_LICENSE_SIGNATURE_OFFSET
    aes_mmo_hash_buf = bytearray(image)

    print(f"AES-MMO hashing {len(aes_mmo_hash_buf)} bytes total")

    aes_mmo_result = aes_mmo_hash(aes_mmo_hash_buf)

    print("aes_mmo_hash:")
    print(' '.join(f'0x{byte_value:02x}' for byte_value in aes_mmo_result))

    with open(args.x25519_private_key_binfile, "rb") as file_contents:
        dump = file_contents.read(-1)

    x25519_sign_and_fill_in_signature(intel_hex_file, aes_mmo_result, bytearray(dump), signature_address)


def create_intel_hex_file_object(hex_filename) -> IntelHex:
    """Create an Intel HEX object to read and modify without altering the HEX file."""
    logging.info("Reading Hex file: %s", os.path.basename(hex_filename))

    intel_hex_file = IntelHex(hex_filename)
    intel_hex_file.padding = 0x00  # default is 0xFF

    logging.info("Hex contains addresses 0x%08x to 0x%08x", intel_hex_file.minaddr(), intel_hex_file.maxaddr())

    return intel_hex_file


def get_license_data_to_hash(intel_hex_file, start_addr_license) -> bytes:
    """Compose a bytes buffer of license data that needs to be hashed"""
    image = b''

    start_offset = start_addr_license + USER_LICENSE_VPP_OFFSET

    # Skip signature
    start_signature_offset = (start_addr_license + EXTENDED_USER_LICENSE_SIGNATURE_OFFSET)

    end_signature_offset = (start_addr_license +
                            EXTENDED_USER_LICENSE_SIGNATURE_OFFSET + EXTENDED_USER_LICENSE_SIGNATURE_SIZE)

    start_load_completed_mw_offset = (start_addr_license + USER_LICENSE_LOAD_COMPLETED_OFFSET)

    end_load_completed_mw_offset = (start_addr_license + USER_LICENSE_LOAD_COMPLETED_OFFSET + 4)

    # skipping the three reserved bytes before the freshness counter as well
    start_freshness_counter_offset = (start_addr_license + USER_LICENSE_FRESHNESS_COUNTER_OFFSET - 3)
    # skip freshness counter byte
    stop_freshness_counter_offset = (start_addr_license + USER_LICENSE_FRESHNESS_COUNTER_OFFSET + 1)

    end_offset = end_signature_offset
    logging.info("adding license [0x%lx,0x%lx]", start_offset, end_offset)

    for i in range(start_offset, end_offset):
        if (start_signature_offset <= i < end_signature_offset) or \
                (start_load_completed_mw_offset <= i < end_load_completed_mw_offset) or \
                (start_freshness_counter_offset <= i < stop_freshness_counter_offset):
            pass
        else:
            image += struct.pack("B", intel_hex_file[i])

    return image


def get_section_data_to_hash(intel_hex_file, start_addr, size) -> bytes:
    """Compose a bytes buffer from an address range"""
    logging.info("adding section [0x%lx,0x%lx] (size 0x%lx bytes)",
                 start_addr, start_addr + size, size)

    for i in range(start_addr, start_addr + size):
        # define/fill gaps to use .tobinstr() successfully
        if intel_hex_file[i] == 0x00:
            intel_hex_file[i] = 0x00

    return intel_hex_file.tobinstr(start_addr, size=size)


def add_section(intel_hex_file: IntelHex, start_area: int, license_offset: int,
                section_number: int, argument: Tuple, must_pad_to_16_byte_multiple: bool) -> bytes:
    """Add a section definition to the intel_hex_file's extended userlicense and return its content for hashing.

    must_pad_to_16_byte_multiple: Set if the ROM_aes_mmo_update function which is
    used to hash the section requires a multiple of 16-bytes as data to be
    hashed, if the size of the section is not a multiple of 16 bytes we add
    zero padding to get to this multiple.
    """
    (offset, size) = argument

    image = b''

    if offset != 0xFFFFFFFF:
        if size == 0xFFFFFFFF:
            logging.info("Taking maximum address from HEX file to calculate and align size of section with offset"
                         " %#lx", offset)
            # # Use maximum hex file address
            # size = align_section_size((intel_hex_file.maxaddr() - (start_area + offset)), ALIGNMENT_SIZE)
            size = intel_hex_file.maxaddr() - (start_area + offset) + 1

        logging.info("Adding section %d: start = %#lx size = %#x = %d", section_number, start_area + offset, size,
                     size)

        image += get_section_data_to_hash(intel_hex_file, start_area + offset, size)

        assert len(image) == size

        if must_pad_to_16_byte_multiple:
            remainder = size % 16
            if remainder != 0:
                for _i in range(0, 16 - remainder):
                    image += struct.pack("B", 0x00)

                logging.info("Section size not a multiple of 16 as required by ROM_aes_mmo_update, \
                             adding %d zero bytes", 16 - remainder)

            assert len(image) % 16 == 0

    ihex_set_uint32(intel_hex_file,
                    start_area + license_offset + EXTENDED_USER_LICENSE_SECTION_1_ADDRESS_OFFSET + (section_number * 8),
                    offset)
    ihex_set_uint32(intel_hex_file,
                    start_area + license_offset + EXTENDED_USER_LICENSE_SECTION_1_SIZE_OFFSET + (section_number * 8),
                    size)

    return image


def ihex_set_uint16(intel_hex_file, offset, value):
    """Set the 16 bit value at a specified offset inside the Intel HEX file object."""
    _, _, third, fourth = struct.unpack('>BBBB', struct.pack('>I', value))

    intel_hex_file[offset + 0] = fourth
    intel_hex_file[offset + 1] = third


def ihex_set_uint32(intel_hex_file, offset, value):
    """Set the 32 bit value at a specified offset inside the Intel HEX file object."""
    first, second, third, fourth = struct.unpack('>BBBB', struct.pack('>I', value))

    intel_hex_file[offset + 0] = fourth
    intel_hex_file[offset + 1] = third
    intel_hex_file[offset + 2] = second
    intel_hex_file[offset + 3] = first


def parse_command_line_arguments() -> SignFirmwareArguments:
    """Parse the command line arguments"""
    def base_16_int(string):
        return int(string, 16)

    def two_hexint_tuple(string):
        return (int(string.split(':')[0], 16), int(string.split(':')[1], 16))

    def str_as_bytes(string):
        return codecs.encode(string, 'UTF-8')

    parser = argparse.ArgumentParser()

    parser.add_argument("--hex",
                        help="path to hex file to be signed (overwritten if --output is not specified)")
    parser.add_argument("--output",
                        help="output file to be written")
    parser.add_argument("--pem",
                        help="path to PEM file to be signed")
    parser.add_argument("--pem_password",
                        type=str_as_bytes,
                        help="optional PEM file password")
    parser.add_argument("--start_addr_area",
                        type=base_16_int,
                        help="base-16 start address of app area")

    parser.add_argument("--add_padding",
                        help="an obsolete argument",
                        action='store_true')

    parser.add_argument("--license_offset",
                        type=base_16_int,
                        help="base-16 offset relative to start_addr_area where the user license begins")

    parser.add_argument("--write_secureboot_public_key",
                        type=base_16_int,
                        help="base-16 offset to write the secureboot public key")

    parser.add_argument("--set_bootloader_loaded",
                        help="set USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD",
                        action='store_true')

    parser.add_argument("--section1",
                        type=two_hexint_tuple,
                        default=(0xFFFFFFFF, 0xFFFFFFFF),
                        help="<base-16 offset>:<base-16 size> - Section 1 to hash")

    parser.add_argument("--section2",
                        type=two_hexint_tuple,
                        default=(0xFFFFFFFF, 0xFFFFFFFF),
                        help="<base-16 offset>:<base-16 size> - Section 2 to hash")

    parser.add_argument("--x25519",
                        help="use AES-MMO + x25519 signing",
                        action='store_true')

    parser.add_argument("--x25519_private_key_binfile",
                        help="private key used for signing")

    args = parser.parse_args()

    return SignFirmwareArguments(**vars(args))


def validate_command_line_arguments(args: SignFirmwareArguments) -> None:
    """validate the command line arguments"""
    if not args.hex:
        logging.error("supply HEX file path")
        sys.exit(-1)

    if args.pem and not args.pem_password:
        logging.error("supply PEM file path")
        sys.exit(-1)

    if args.license_offset is None:
        logging.error("license start offset not defined")
        sys.exit(-1)

    if not args.start_addr_area:
        logging.info("taking default start address")
        args.start_addr_area = 0x04000000

    if args.add_padding:
        logging.info("You specified an obsolete argument: --padding")

    if args.output:
        args.intel_hex_write_path = args.output
    else:
        # Overwrite the input hex file with the modified (signed) intel hex object
        args.intel_hex_write_path = args.hex


def sign_firmware(args: SignFirmwareArguments):
    """sign firmware application with typed arguments"""

    intel_hex_file: IntelHex = create_intel_hex_file_object(args.hex)

    image: bytes = b''

    if args.license_offset is not None:
        license_offset = args.license_offset
        license_address = args.start_addr_area + args.license_offset
        signature_address = license_address + EXTENDED_USER_LICENSE_SIGNATURE_OFFSET

        logging.info("Start Address = 0x%08x", args.start_addr_area)
        logging.info("License located @ 0x%lx", license_address)
        logging.info("Signature located @ 0x%lx", signature_address)

        assert intel_hex_file.maxaddr() > (args.start_addr_area + license_offset)

        image += add_section(intel_hex_file, args.start_addr_area, license_offset, 0,
                             args.section1, args.x25519)

        image += add_section(intel_hex_file, args.start_addr_area, license_offset, 1,
                             args.section2, args.x25519)

    if args.set_bootloader_loaded:
        logging.info("populating USER_LICENSE_LOAD_COMPLETED_OFFSET at %#x",
                     license_address + USER_LICENSE_LOAD_COMPLETED_OFFSET)
        ihex_set_uint32(intel_hex_file,
                        license_address + USER_LICENSE_LOAD_COMPLETED_OFFSET,
                        USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD)

    if args.pem:
        sign_info: SigningInformation = load_pem_file(args.pem, args.pem_password)

        set_signature_metadata(intel_hex_file, license_address, sign_info)

        logging.info("=====================================")
        logging.info("Public key to be put in bootloader:")
        logging.info(crypto_utils.getCCodeBuffer(sign_info.public_key, "public_key"))
        logging.info("=====================================")

    if args.pem or args.x25519:
        logging.info("Hashing license")
        image += get_license_data_to_hash(intel_hex_file, license_address)

    if args.pem:
        signature = add_pem_signature(intel_hex_file, image, license_address, sign_info)
        if args.write_secureboot_public_key:
            add_secureboot_public_key(intel_hex_file, sign_info,
                                      args.start_addr_area + args.write_secureboot_public_key)
    elif args.x25519:
        add_x25519_signature(intel_hex_file, image, license_address, args)
    else:
        raise Exception("nothing to do: specify a pem or x25519 file!")

    logging.info("Writing Hex file: %s", args.intel_hex_write_path)
    intel_hex_file.tofile(args.intel_hex_write_path, format='hex')

    if args.pem:
        # Verify that image is signed correctly
        if crypto_utils.verifyMessage(image,
                                      sign_info.public_key,
                                      signature,
                                      curve=sign_info.curve,
                                      hashfunc=sign_info.hash_function):
            logging.info("SUCCESS: Message OK")
        else:
            logging.error("Message NOK")


def main():
    """the entry point of the application."""
    logging.basicConfig(level=logging.INFO)

    args = parse_command_line_arguments()
    validate_command_line_arguments(args)
    sign_firmware(args)


if __name__ == "__main__":
    main()
