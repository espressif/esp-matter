"""
This tool will generate a RMA token to be used in conjunction with a secure bootloader

Example:
    python generateRMAToken.py \
    --pem private_key_p256_enc.pem \
    --pem_password \
    --token 0xAABBCCDD \
    --mac_addr 9c:69:37:00:00:24:0d:cc \
    --rma_token_area_address 0x04000000
"""
import argparse
import getpass
import hashlib
import os
import struct
import codecs
import sys
import logging

import intelhex
from ecdsa import NIST256p, NIST192p

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
        app_dir = os.path.join(current_dir, "..", "..", "..")
        sys.path.append(os.path.join(app_dir, "..", "..", "..", "Env", "vless", "gppy_vless", "inf"))
        from getEnvVersion import getEnvVersion
        version_xml = os.path.join(app_dir, "gpVersion.xml")

        logging.debug("abs env sec path %s", os.path.abspath(os.path.join(app_dir, "..", "..", "..", "Env",
                                                       getEnvVersion(version_xml), "gppy", "tools", "sec")))

        sys.path.append(os.path.join(app_dir, "..", "..", "..", "Env", getEnvVersion(version_xml), "gppy",
                                     "tools", "sec"))

RMA_TOKEN_MAGIC_WORD = "0x1B4FA925"

logging.debug("crypto_utils module path: %s", crypto_utils.__file__)


def calculate_signature(image, pemfile_path, password):
    """
    calculate_signature calculates the signature over a specified image using the private key
    contained in the specified file. The password for the file is also required.
    """
    # Retrieve information from PEM file
    print("INFO: Reading PEM file: %s" % os.path.basename(pemfile_path))

    (pem_curve, private_key, public_key) = crypto_utils.getPrivatePublicKeysFromPEMFile(pemfile_path, password)

    # Assure correct curves and hashes are being used together

    if pem_curve == "secp192r1":
        curve = NIST192p
        hash_function = hashlib.sha1
    elif pem_curve == "secp256r1":
        curve = NIST256p
        hash_function = hashlib.sha256
    else:
        print("ERR: Unsupported curve in supplied keys: %s" % pem_curve)
        assert False

    print("INFO: Using signing curve: %s" % curve.name)
    print("INFO: PEM file has curve: %s" % pem_curve)

    print("INFO: Hashing and signing image")
    signature = crypto_utils.signMessage(image, private_key, curve=curve, hashfunc=hash_function)

    print("=====================================")
    print("Signature of RMA Token:")
    crypto_utils.printCCodeBuffer(signature, "signature")
    print("=====================================")
    print("Public key to be put in bootloader:")
    crypto_utils.printCCodeBuffer(public_key, "public_key")
    print("=====================================")

    return signature, public_key, curve, hash_function


def generate_rma_hex_token(rma_token_area, signature):
    """
    Generates rma_token.hex file that can be written to a target using i.e progUtilTool.exe

    :param rma_token_area: hex string of RMA token area address
    :param signature: list of bytes of previously calculated signature
    """
    magic_word_bytes = bytearray().fromhex(RMA_TOKEN_MAGIC_WORD[2:])
    magic_word_bytes_le = [b for b in reversed(magic_word_bytes)]
    rma_token_area_int = int(rma_token_area, 16)

    rma_hex = intelhex.IntelHex()
    for index, byte in enumerate(magic_word_bytes_le + list(signature)):
        rma_hex[rma_token_area_int + index] = byte

    rma_hex_file_name = "rma_token.hex"

    rma_hex.write_hex_file(rma_hex_file_name)
    print("RMA token saved in: {}".format(rma_hex_file_name))


def parse_command_line_arguments():
    """Parse the command line arguments of the signfw application."""
    parser = argparse.ArgumentParser()

    parser.add_argument("--pem",
                        help="path to PEM file", required=False)

    parser.add_argument("--pem_password",
                        help="will prompt for optional PEM file password", action="store_true")

    parser.add_argument("--token",
                        help="arbitrary 32-bit token value to be hashed together with the MAC_Address, "
                             "format \"0xAABBCCDD\"",
                        required=True)

    parser.add_argument("--mac_addr",
                        help="MAC Address of the device for which the RMA token is generated, "
                             "format AA:BB:CC:DD:EE:FF:00:11",
                        required=True)

    parser.add_argument("--rma_token_area_address",
                        help="Flash memory address of RMA token area. "
                             "When provided will generate 'rma_token.hex' file that can be written to target, "
                             "format \"[0x]04000000\"")
    parser.add_argument("--x25519",
                        help="use AES-MMO + x25519 signing",
                        action='store_true')
    parser.add_argument("--x25519_private_key_binfile",
                        help="path to x25519 private key bin file")
    args = parser.parse_args()

    if args.pem_password:
        args.pem_password = getpass.getpass(prompt="PEM password:")
    else:
        args.pem_password = None

    if not args.pem and not args.x25519:
        print("select ECDSA or x25519 signing")
        sys.exit(-1)

    if args.x25519 and not args.x25519_private_key_binfile:
        print("specify path to private key")
        sys.exit(-1)

    return args


def generate_rma_token(pemfile, pem_passw, rma_token, mac_address, arg_x25519=False, x25519_private_key_binfile=None):
    image = b''  # RMA Token + MAC Address

    # Add Token to image
    rma_token.reverse()
    for byte in rma_token:
        image += struct.pack("B", byte)

    # Add MAC address to image
    for byte in mac_address:
        image += struct.pack("B", byte)

    if arg_x25519:
        from gppy.tools.sec.aes_mmo import aes_mmo_hash
        from gppy.tools.sec.x25519 import x25519_sign_and_return_response

        aes_mmo_hash_buf = bytearray(image)

        print("AES-MMO hashing %d bytes total" % len(aes_mmo_hash_buf))

        aes_mmo_result = aes_mmo_hash(aes_mmo_hash_buf)

        print("aes_mmo_hash:")
        print(' '.join('0x{:02x}'.format(x) for x in aes_mmo_result))

        f = open(x25519_private_key_binfile, "rb")
        dump = f.read()

        # TODO: insert private key

        signature = bytearray(x25519_sign_and_return_response(aes_mmo_result, bytearray(dump)))

    else:

        signature, public_key, curve, hash_function = calculate_signature(
            image,
            pemfile,
            codecs.encode(pem_passw, 'UTF-8')
        )

        signature = bytearray(signature)

    return signature


def hex_string_to_bytes(hex_string):
    """Convert hex string (ie. "3af") to bytes (ie. b'\x03\xaf')"""
    return codecs.decode('0' * (len(hex_string) % 2) + hex_string, 'hex')


def main():
    args = parse_command_line_arguments()

    ###############################################
    token = hex_string_to_bytes(args.token[2:])
    token = bytearray(token)

    mac_addr = args.mac_addr.split(":")
    mac_addr.reverse()

    mac_address_list = []
    for byte in mac_addr:
        mac_address_list.append(int(byte, 16))

    signature = generate_rma_token(args.pem, args.pem_password, token, mac_address_list,
                                   args.x25519, args.x25519_private_key_binfile)

    if args.rma_token_area_address:
        generate_rma_hex_token(args.rma_token_area_address, signature)


if __name__ == "__main__":
    main()
