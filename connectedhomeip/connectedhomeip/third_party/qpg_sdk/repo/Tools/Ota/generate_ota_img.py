#!/usr/bin/env python3

import argparse
import sys
import os
import logging
import shutil
import subprocess
from dataclasses import dataclass
from typing import Tuple

DESCRIPTION = """\
Turn a Matter application build hex-file into a bootable image and generate an ota image
"""


@dataclass
class GenerateOtaImageArguments:
    """helper to enforce type checking on argparse output"""
    chip_config_header: str
    chip_root: str
    in_file: str
    out_file: str
    version: int
    version_str: str
    vendor_id: str
    product_id: str
    sign: bool
    pem_file_path: str
    pem_password: str
    flash_app_start_offset: int
    compression: str
    prune_only: bool


DEFAULT_FLASH_APP_START_OFFSET = 0x6000
UPGRADE_SECUREBOOT_PUBLICKEY_OFFSET = 0x1800
LICENSE_SIZE = 0x100

SCRIPT_PATH = os.path.dirname(__file__)
CRCFIRMWARE_PATH = f"{SCRIPT_PATH}/crcFirmware.py"
HEX2BIN_PATH = f"{SCRIPT_PATH}/hex2bin.py"
COMPRESSFIRMWARE_PATH = f"{SCRIPT_PATH}/compressFirmware.py"
SIGNFIRMWARE_PATH = f"{SCRIPT_PATH}/signFirmware.py"

if not os.path.isfile(os.path.join(SCRIPT_PATH, "crypto_utils.py")):
    CRCFIRMWARE_PATH = os.getenv("QORVO_CRCFIRMWARE_PATH", CRCFIRMWARE_PATH)
    HEX2BIN_PATH = os.getenv("QORVO_HEX2BIN_PATH", HEX2BIN_PATH)
    COMPRESSFIRMWARE_PATH = os.getenv("QORVO_COMPRESSFIRMWARE_PATH", COMPRESSFIRMWARE_PATH)
    SIGNFIRMWARE_PATH = os.getenv("QORVO_SIGNFIRMWARE_PATH", SIGNFIRMWARE_PATH)


def parse_command_line_arguments() -> GenerateOtaImageArguments:
    """Parse command-line arguments"""
    def any_base_int(string):
        return int(string, 0)
    parser = argparse.ArgumentParser(description=DESCRIPTION)

    parser.add_argument("--chip_config_header",
                        help="path to Matter config header file")

    parser.add_argument("--chip_root",
                        help="Path to root Matter directory")

    parser.add_argument("--in_file",
                        help="Path to input file to format to Matter OTA fileformat")

    parser.add_argument("--out_file",
                        help="Path to output file (.ota file)")
    parser.add_argument('-vn', '--version', type=any_base_int, help='Software version (numeric)', default=1)
    parser.add_argument('-vs', '--version-str', help='Software version (string)', default="1.0")
    parser.add_argument('-vid', '--vendor-id', help='Vendor ID (string)', default=None)
    parser.add_argument('-pid', '--product-id', help='Product ID (string)', default=None)
    parser.add_argument('--sign', help='sign firmware', action='store_true')
    parser.add_argument('--pem_file_path', help='PEM file path (string)', default=None)
    parser.add_argument('--pem_password', help='PEM file password (string)', default=None)
    parser.add_argument('--flash_app_start_offset',
                        type=any_base_int,
                        help='Offset of the application in program flash',
                        default=DEFAULT_FLASH_APP_START_OFFSET)
    parser.add_argument("--compression",
                        choices=['none', 'lzma'],
                        default="lzma",
                        help="compression type (default to none)")
    parser.add_argument("--prune_only",
                        help="prune unneeded sections; don't add an upgrade user license (external storage scenario)",
                        action='store_true')

    args = parser.parse_args()

    return GenerateOtaImageArguments(**vars(args))


def validate_arguments(args: GenerateOtaImageArguments):
    if not args.chip_root:
        logging.error("Supply Matter root directory")
        sys.exit(-1)
    else:
        assert os.path.isdir(args.chip_root), f"The path specified as chip root is not a directory: {args.chip_root}"

    if not args.in_file:
        logging.error("Supply an input file")
        sys.exit(-1)
    else:
        assert os.path.isfile(args.in_file), f"The path specified as input file is not a file: {args.in_file}"

    if not args.out_file:
        logging.error("Supply an output file")
        sys.exit(-1)


def run_script(command: str):
    """ run a python script using the current interpreter """
    assert command != ""
    logging.info("%s", command)
    subprocess.check_output(f"{sys.executable} {command}", shell=True)


def extract_vid_pid_vn_and_vs(chip_config_header: str) -> Tuple[str, str, str, str]:
    """ Determine vendorid/product id/version number/version string
        from a CHIP project's headers
    """
    vid = None
    pid = None
    version_number = None
    version_str = None

    with open(chip_config_header, 'r', encoding='utf-8') as config_file:
        lines = config_file.readlines()

    for line in lines:
        if "#define " in line:
            if 'CHIP_DEVICE_CONFIG_DEVICE_VENDOR_ID' in line:
                vid = line.split()[2]
            if 'CHIP_DEVICE_CONFIG_DEVICE_PRODUCT_ID' in line:
                pid = line.split()[2]
            if 'CHIP_DEVICE_CONFIG_DEVICE_SOFTWARE_VERSION' in line:
                version_number = line.split()[2]
            if 'CHIP_DEVICE_CONFIG_DEVICE_SOFTWARE_VERSION_STRING' in line:
                version_str = line.split()[2]

    return (vid, pid, version_number, version_str)


def determine_example_project_config_header(args: GenerateOtaImageArguments):
    """ Determine the CHIPProjectConfig.h path of a matter-sourcetree based example application."""
    if 'lighting' in args.in_file:
        project_name = 'lighting-app'
    elif 'lock' in args.in_file:
        project_name = 'lock-app'
    elif 'persistent' in args.in_file:
        project_name = 'persistent-storage'
    elif 'shell' in args.in_file:
        project_name = 'shell'
    else:
        raise Exception(f"Unable to deduce which example project {args.in_file} belongs to!")

    return f"{args.chip_root}/examples/{project_name}/qpg/include/CHIPProjectConfig.h"


def determine_vid_and_pid_values(args: GenerateOtaImageArguments) -> Tuple[str, str, str, str]:
    """ Decide which Vendor ID, Product ID, Version number and string the user wants to use
    """
    used_chip_config_header = args.chip_config_header or determine_example_project_config_header(args)
    (vid, pid, version, version_str) = extract_vid_pid_vn_and_vs(used_chip_config_header)

    if args.vendor_id:
        vid = args.vendor_id
    if vid is None:
        raise Exception("No VID given or found in ProjectConfig")
    if args.product_id:
        pid = args.product_id
    if pid is None:
        raise Exception("No PID given or found in ProjectConfig")
    if args.version:
        version = args.version
    if version is None:
        raise Exception("No SW version given or found in ProjectConfig")
    if args.version_str:
        args.version_str = args.version_str.strip('"\'')
        version_str = args.version_str
    if version_str is None:
        raise Exception("No SW version string given or found in ProjectConfig")

    print(f"Found: VID:{vid} PID:{pid} Version:{version} Version str:{version_str}")

    return (vid, pid, version, version_str)


def post_process_image(args: GenerateOtaImageArguments):
    """Run Qorvo image post-processing steps

    WARNING: THIS FUNCTION MODIFIES THE INPUT FILE!

    Add necessary metadata for the bootloader to process
    * crc/sign, set application loaded by bootloader flag
    """

    input_base_path = os.path.splitext(args.in_file)[0]
    copy_of_unmodified_input = f"{input_base_path}.input.hex"

    # we modify in place, keep a copy of the input for reference
    shutil.copyfile(args.in_file, copy_of_unmodified_input)

    common_arguments = (" --set_bootloader_loaded"
                        f" --hex {args.in_file}"
                        f" --license_offset {args.flash_app_start_offset:#x}"
                        f" --section1 {args.flash_app_start_offset+LICENSE_SIZE:#x}:0xffffffff"
                        " --section2 0x800:0x1000"
                        f" --start_addr_area 0x4000000"
                        )

    if args.sign:
        run_script(f"{SIGNFIRMWARE_PATH}"
                   f" --pem {args.pem_file_path} "
                   f" --pem_password {args.pem_password}"
                   f" --write_secureboot_public_key {UPGRADE_SECUREBOOT_PUBLICKEY_OFFSET:#x}"
                   f"{common_arguments}")
    else:
        run_script(f"{CRCFIRMWARE_PATH} --add_crc"
                   f" {common_arguments}")


def compress_ota_payload(args: GenerateOtaImageArguments):
    """Apply compression and add metadata for the Qorvo bootloader"""
    input_base_path = os.path.splitext(args.in_file)[0]
    intermediate_hash_added_binary = f"{input_base_path}-with-hash.bin"
    intermediate_compressed_binary_path = f"{input_base_path}.compressed.bin"
    run_script(f"{HEX2BIN_PATH} {args.in_file} {intermediate_hash_added_binary}")
    run_script(f"{COMPRESSFIRMWARE_PATH} "
               f"{'' if args.sign else '--add_crc'}"
               f" --compression={args.compression}"
               f" {'--prune_only' if args.prune_only else ''}"
               f" --input {intermediate_hash_added_binary}"
               f" --license_offset {args.flash_app_start_offset-0x10:#x} --ota_offset 0xa0000"
               f" --output {intermediate_compressed_binary_path}"
               " --page_size 0x200 --sector_size 0x400"
               + (f" --pem {args.pem_file_path} "
                  f" --pem_password {args.pem_password}" if args.sign and not args.prune_only else "")
               )
    return intermediate_compressed_binary_path


def main():
    """ Main """

    logging.basicConfig(level=logging.INFO)

    args = parse_command_line_arguments()

    validate_arguments(args)

    # Parse values from commandline arguments or application ProjectConfig header
    (vid, pid, version, version_str) = determine_vid_and_pid_values(args)

    # Bootable image preparation
    post_process_image(args)

    # Qorvo specific OTA preparation
    intermediate_compressed_binary_path = compress_ota_payload(args)

    # Matter header wrapping
    tool_args = f"create -v {vid} -p {pid} -vn {version} -vs {version_str} -da sha256 "
    run_script(f"{args.chip_root}/src/app/ota_image_tool.py {tool_args} "
               f"{intermediate_compressed_binary_path} {args.out_file}")


if __name__ == "__main__":
    main()
