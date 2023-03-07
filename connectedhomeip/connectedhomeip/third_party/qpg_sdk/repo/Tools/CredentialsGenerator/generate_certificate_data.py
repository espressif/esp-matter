#!/usr/bin/env python3
import os
import sys
import os
import sys
import shutil
import logging
import argparse
import struct
import subprocess
import cryptography.x509
from typing import Tuple, List, Union
from enum import Enum, unique
from cryptography.hazmat.backends import default_backend
from dataclasses import dataclass
import subprocess
import os.path


@dataclass
class CertificateDataGeneratorArguments:
    """helper to enforce type checking on argparse output"""
    paa_out_key: str
    paa_out_cert: str
    pai_out_key: str
    pai_out_cert: str
    dac_out_key: str
    dac_out_cert: str
    cd: str
    vid: str
    pid: str
    did: str
    nmbr_dacs: int
    chip_cert_tool_path: str
    sign_cd_cert: str
    sign_cd_priv_key: str
    mode:str


def validate_args(args: CertificateDataGeneratorArguments):
    # Validate the passcode
    if args.paa_out_key is None:
        args.paa_out_key = "qorvo_paa_key"

    if args.paa_out_cert is None:
        args.paa_out_cert = "qorvo_paa_cert"

    if args.pai_out_key is None:
        args.pai_out_key = "qorvo_pai_key"

    if args.pai_out_cert is None:
        args.pai_out_cert = "qorvo_pai_cert"

    if args.dac_out_key is None:
        args.dac_out_key = "qorvo_dac_key"

    if args.dac_out_cert is None:
        args.dac_out_cert = "qorvo_dac_cert"

    if args.cd is None:
        args.cd = "qorvo_cd"

    if args.vid is None:
        logging.error("Vendor ID argument (--vid) is a mandatory argument!")
        sys.exit(1)

    if args.pid is None:
        logging.error("Product ID argument (--pid) is a mandatory argument!")
        sys.exit(1)

    if args.did is None:
        logging.error("Device ID argument (--did) is a mandatory argument!")
        sys.exit(1)

    if args.nmbr_dacs is None:
        args.nmbr_dacs = 1

    if args.chip_cert_tool_path is None:
        logging.error("Path to chip_cert tool (--chip-cert-tool-path) is a mandatory argument!")
        sys.exit(1)

    if args.sign_cd_cert is None:
        logging.error(
            "Path to file that contains the certificate for signing the Certification Declaration (--sign-cd-cert) is a mandatory argument!")
        sys.exit(1)

    if args.sign_cd_priv_key is None:
        logging.error(
            "Path to file that contains the private key for signing the Certification Declaration (--sign-cd-priv-key) is a mandatory argument!")
        sys.exit(1)
    
    if args.mode.lower() == "pai":
        # Checking whether PAA cert and PAA key are ready
        if not os.path.exists(str(args.paa_out_cert) + ".pem") or \
           not os.path.exists(str(args.paa_out_key) + ".pem") :
            logging.error(str(args.paa_out_cert) + ".pem and " + str(args.paa_out_key) + ".pem are required")
            sys.exit(1)

    if args.mode.lower() == "dac":
        # Checking whether PAI cert and PAI key are ready
        if not os.path.exists(str(args.pai_out_cert) + ".pem") or \
           not os.path.exists(str(args.pai_out_key) + ".pem") :
            logging.error(str(args.pai_out_cert) + ".pem and " + str(args.pai_out_key) + ".pem are required")
            sys.exit(1)


def run_cmds(cmds):
    responses = []
    output = ""
    f = subprocess.PIPE

    for cmd in cmds:
        # print(cmd["cmd"])
        result = subprocess.run(cmd["cmd"],
                                stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE)
        # print(result.stdout.decode('utf-8'))
        # print(result.stderr.decode('utf-8'))


def convert_cert_to_der_format(args: CertificateDataGeneratorArguments, input):
    convert_cmd = {
        "resp": "",
        "cmd": [str(args.chip_cert_tool_path) + 'chip-cert.elf', 'convert-cert',
                '-d',
                str(input) + ".pem",
                str(input) + ".der"],
        "timeout": 5
    }

    run_cmds([convert_cmd])


def convert_key_to_der_format(args: CertificateDataGeneratorArguments, input):
    convert_cmd = {
        "resp": "",
        "cmd": [str(args.chip_cert_tool_path) + 'chip-cert.elf', 'convert-key',
                '-d',
                str(input) + ".pem",
                str(input) + ".der"],
        "timeout": 5
    }

    run_cmds([convert_cmd])


def generate_dac(args: CertificateDataGeneratorArguments, outkeyfile, outcertfile):
    dac_generation_cmd = {
        "resp": "",
        "cmd": [str(args.chip_cert_tool_path) + 'chip-cert.elf', 'gen-att-cert',
                '--type', 'd',
                '--subject-cn', "Matter Development DAC 01",
                '--subject-vid', str(args.vid),
                '--subject-pid', str(args.pid),
                '--lifetime', '7305',
                '--ca-key', str(args.pai_out_key) + ".pem",
                '--ca-cert', str(args.pai_out_cert) + ".pem",
                '--out-key', outkeyfile + ".pem",
                '--out', outcertfile + ".pem"],
        "timeout": 5
    }
    run_cmds([dac_generation_cmd])


def generate_pai(args: CertificateDataGeneratorArguments):
    pai_generation_cmd = {
        "resp": "",
        "cmd": [str(args.chip_cert_tool_path) + 'chip-cert.elf', 'gen-att-cert',
                '--type', 'i',
                '--subject-cn', "Matter Development PAI 01",
                '--subject-vid', str(args.vid),
                '--lifetime', '7305',
                '--ca-key', str(args.paa_out_key) + ".pem",
                '--ca-cert', str(args.paa_out_cert) + ".pem",
                '--out-key', str(args.pai_out_key) + ".pem",
                '--out', str(args.pai_out_cert) + ".pem"],
        "timeout": 5
    }
    run_cmds([pai_generation_cmd])


def generate_paa(args: CertificateDataGeneratorArguments):
    paa_generation_cmd = {
        "resp": "",
        "cmd": [str(args.chip_cert_tool_path) + 'chip-cert.elf', 'gen-att-cert',
                '--type', 'a',
                '--subject-cn', "Matter Development PAA 01",
                '--lifetime', '7305',
                '--out-key', str(args.paa_out_key) + ".pem",
                '--out', str(args.paa_out_cert) + ".pem"],
        "timeout": 5
    }
    run_cmds([paa_generation_cmd])


def generate_cd(args: CertificateDataGeneratorArguments):
    cd_generation_cmd = {
        "resp": "",
        "cmd": [str(args.chip_cert_tool_path) + 'chip-cert.elf', 'gen-cd',
                '-C', str(args.sign_cd_cert),
                '-K', str(args.sign_cd_priv_key),
                '--out', str(args.cd) + ".bin",
                '-f', '1',
                '-V', str(args.vid),
                '-p', str(args.pid),
                '-d', str(args.did),
                '-c', 'ZIG0000000000000000',
                '-l', '0',
                '-i', '0',
                '-n', '001',
                '-t', '0'],
        "timeout": 5
    }
    run_cmds([cd_generation_cmd])


def generate_certificate_data(args: CertificateDataGeneratorArguments):

    if args.mode.lower() in ("paa", "all"):
        # PAA cert + public/private keypair
        print("Generation of PAA certificate and private/public key pair")
        generate_paa(args)
        convert_cert_to_der_format(args, args.paa_out_cert)
        convert_key_to_der_format(args, args.paa_out_key)
        print("------> PAA cert DONE")

    if args.mode.lower() in ("pai", "all"):
        # PAI cert + public/private keypair
        print("Generation of PAI certificate and private/public key pair")
        generate_pai(args)
        convert_cert_to_der_format(args, args.pai_out_cert)
        convert_key_to_der_format(args, args.pai_out_key)
        print("------> PAI cert DONE")

    if args.mode.lower() in ("dac", "all"):
        # DAC cert + public/private keypair
        for x in range(args.nmbr_dacs):
            print("Generation of DAC certificate and private/public key pair -" + str(x + 1))
            dac_cert_file = str(args.dac_out_cert) + "_" + str(x + 1)
            dac_key_file = str(args.dac_out_key) + "_" + str(x + 1)
            generate_dac(args, dac_key_file, dac_cert_file)
            convert_cert_to_der_format(args, dac_cert_file)
            convert_key_to_der_format(args, dac_key_file)
            print("------> DAC cert DONE")

        # CD
        print("Generation of Certification Declaration and signing of Certification Declaration")
        generate_cd(args)
        print("------> CD DONE")


def parse_command_line_arguments() -> CertificateDataGeneratorArguments:
    """parse command line arguments"""
    def any_base_int(s):
        return int(s, 0)

    parser = argparse.ArgumentParser(description='Chip Certificate data generator tool')

    parser.add_argument('--chip-cert-tool-path', type=str,
                        help='path to the binary of chip-cert tool (example: <path>/<to>/QMatter/Tools/CredentialsGenerator)')
    parser.add_argument('--paa-out-key', type=str,
                        help='filename for storing the PAA key (without extension) [default: qorvo_paa_key]')
    parser.add_argument('--paa-out-cert', type=str,
                        help='filename for storing the PAA certificate (without extension) [default: qorvo_paa_cert]')
    parser.add_argument('--pai-out-key', type=str,
                        help='filename for storing the PAI key (without extension) [default: qorvo_pai_key]')
    parser.add_argument('--pai-out-cert', type=str,
                        help='filename for storing the PAI certificate (without extension) [default: qorvo_pai_cert]')
    parser.add_argument('--dac-out-key', type=str,
                        help='filename for storing the DAC key (without extension) [default: qorvo_dac_key]')
    parser.add_argument('--dac-out-cert', type=str,
                        help='filename for storing the DAC certificate (without extension) [default: qorvo_dac_cert]')
    parser.add_argument('--cd', type=str,
                        help='filename for storing the certification declaration (without extension) [default: qorvo_cd]')
    parser.add_argument('--sign-cd-priv-key', type=str,
                        help='path to the file containing private key to be used to sign the Certification Declaration (example: <path>/<to>/QMatter/Tools/CredentialsGenerator/CertificationDeclaration/Chip-Test-CD-Signing-Key.pem')
    parser.add_argument('--sign-cd-cert', type=str,
                        help='path to the file containing the certificate to be used to sign the Certification Declaration (example: <path>/<to>/QMatter/Tools/CredentialsGenerator/CertificationDeclaration/Chip-Test-CD-Signing-Cert.pem')
    parser.add_argument('--vid', type=str,
                        help='vendor ID in hexadecimal form without 0x (example: --vid=FFF1)')
    parser.add_argument('--pid', type=str,
                        help='product ID in hexadecimal form without 0x (example: --pid=0123)')
    parser.add_argument('--did', type=str,
                        help='device ID in hexadecimal form without 0x (example: --pid=0016)')
    parser.add_argument('--nmbr-dacs', type=any_base_int,
                        help='number of DACs to be generated [default: 1]')
    parser.add_argument('--mode', choices=["pai", "paa", "dac", "all", "PAI", "PAA", "DAC", "ALL"], default="all", type=str,
                        help='generation mode [default: all], \'--mode=paa\' for generating PAA key and cert only, \'--mode=pai\' for generating PAI key and cert only, \'--mode=dac\' for generating DAC key and cert only')

    args = parser.parse_args()
    return CertificateDataGeneratorArguments(**vars(args))


def main():
    """entry point of the program"""
    args = parse_command_line_arguments()
    validate_args(args)
    generate_certificate_data(args)
    


if __name__ == "__main__":
    logging.basicConfig(format='[%(asctime)s] [%(levelname)7s] - %(message)s', level=logging.INFO)
    main()
