#!/usr/bin/env python
# Copyright 2020 Espressif Systems (Shanghai) Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
import argparse
from pyasn1_modules import pem
from sys import exit
import helper_scripts as hs
import binascii
import os
import sys
try:
    import esptool
except ImportError:  # cheat and use IDF's copy of esptool if available
    idf_path = os.getenv("IDF_PATH")
    if not idf_path or not os.path.exists(idf_path):
        raise
    sys.path.insert(0, os.path.join(idf_path, "components", "esptool_py", "esptool"))
    import esptool


BINARY_STUB_PATH = '/sample_bins/secure_cert_mfg.bin'


def main():
    parser = argparse.ArgumentParser(description='''Provision the ESPWROOM32SE device with
        device_certificate and signer_certificate required for TLS authentication''')

    parser.add_argument(
        '--signer-cert',
        dest='signer_cert',
        default='sample_certs/sample_signer_cert.pem',
        metavar='relative/path/to/signer_cert.pem',
        help='relative path(from secure_cert_mfg.py) to signer certificate.')

    parser.add_argument(
        '--signer-cert-private-key',
        dest='signer_privkey',
        default='sample_certs/sample_signer_key.pem',
        metavar='relative/path/to/signer-priv-key',
        help='relative path(from secure_cert_mfg.py) to signer certificate private key')

    parser.add_argument(
        "--pwd", '--password',
        dest='password',
        metavar='[password]',
        help='the password associated with the private key')

    parser.add_argument(
        "--port", '-p',
        dest='port',
        metavar='[port]',
        required=True,
        help='uart com port to which ESP device is connected')

    parser.add_argument(
        "--i2c-sda-pin', '-sda_pin'",
        dest='i2c_sda_pin',
        default=16,type=int,
        help='The pin no of I2C SDA pin of esp32 to which atecc608 is connected, default = 16')

    parser.add_argument(
        "--i2c-scl-pin', '-scl_pin'",
        dest='i2c_scl_pin',
        default=17,type=int,
        help='The pin no of I2C SCL pin of esp32 to which atecc608 is connected, default = 17')

    parser.add_argument(
        "--type", "--print-atecc608-type",
        dest='print_atecc608_type',action='store_true',
        help='print type of atecc608 chip connected to your ESP device')

    parser.add_argument(
        "--valid-for-years",
        dest='nva_years',
        default=40,type=int,
        help='number of years for which device cert is valid (from current year), efault = 40')
    args = parser.parse_args()
    esp = esptool.ESP32ROM(args.port,baud=115200)
    hs.serial.load_app_stub(BINARY_STUB_PATH,esp)
    init_mfg = hs.serial.cmd_interpreter()

    retval = init_mfg.wait_for_init(esp._port)
    if retval is not True:
        print("CMD prompt timed out.")
        exit(0)

    retval = init_mfg.exec_cmd(esp._port, "init {0} {1}".format(args.i2c_sda_pin, args.i2c_scl_pin))
    hs.serial.esp_cmd_check_ok(retval, "init {0} {1}".format(args.i2c_sda_pin, args.i2c_scl_pin))

    if "TrustCustom" in retval[1]['Return']:
        print("ATECC608 chip is of type TrustCustom")
        provision_trustcustom_device(esp, args,init_mfg)
    elif "Trust&Go" in retval[1]['Return']:
        print("ATECC608 chip is of type Trust&Go")
        hs.manifest.generate_manifest_file(esp, args, init_mfg)
    elif "TrustFlex" in retval[1]['Return']:
        print("ATECC608 chip is of type TrustFlex")
        hs.manifest.generate_manifest_file(esp, args, init_mfg)
    else:
        print("Invalid type")
        exit(0)


def provision_trustcustom_device(esp, args, init_mfg):

    retval = init_mfg.exec_cmd(esp._port, "print-chip-info")
    hs.serial.esp_cmd_check_ok(retval, "print-chip-info")

    index = retval[1]['Return'].find("Serial Number:\r\n")
    index += len("Serial Number:\r\n")
    serial_number = bytearray(9)
    s = retval[1]['Return'][index:]
    s = s.strip()
    serial_number = bytearray.fromhex(s)
    serial_number_hex = (binascii.hexlify(serial_number)).decode()
    print('Serial Number:')
    print(serial_number_hex.upper())

    if args.print_atecc608_type is True:
        # print chip info and exit
        exit(0)
    print("Provisioning the Device")
    retval = init_mfg.exec_cmd(esp._port, "generate-keys 0")
    hs.serial.esp_cmd_check_ok(retval, "generate-keys")

    retval = init_mfg.exec_cmd(esp._port, "generate-csr")
    hs.serial.esp_cmd_check_ok(retval, "generate-csr")

    print("CSR obtained from device is:")
    print(retval[1]['Return'])

    try:
        # load private keys of signers to sign the CSR
        private_key = hs.cert_sign.load_privatekey(args.signer_privkey, args.password)
        signer_cert = hs.cert_sign.load_certificate(args.signer_cert)
        # Sign the CSR using the generated keys
        device_cert = hs.cert_sign.sign_csr(retval[1]['Return'].encode(), signer_cert, private_key, serial_number_hex, args.nva_years)
        print("Device cert generated: \n")
        dec_device_cert = device_cert.decode()
        print(dec_device_cert)
        print("Saving device cert to output_files/device_cert.pem")

        if esp_handle_file("./output_files/device_cert.pem", "write", dec_device_cert) is not True:
            print("Error in writing device certificate")
            exit(0)
        cert_der = esp_handle_file("./output_files/device_cert.pem", "pem_read")
    except ValueError:
        print("Unsupported Key,Cert or CSR format specified.")
        exit(0)

    # get the cert definition and template data in string format
    print("program device cert")
    cert_def_str = hs.cert2certdef.esp_create_cert_def_str(cert_der, "DEVICE_CERT")

    retval = init_mfg.exec_cmd(esp._port, "provide-cert-def 0", cert_def_str)
    hs.serial.esp_cmd_check_ok(retval, "program-device-cert-def")

    retval = init_mfg.exec_cmd(esp._port, "program-dev-cert", device_cert)
    hs.serial.esp_cmd_check_ok(retval, "program-dev-cert")
    print(retval[1]['Return'])

    signer_cert_data = esp_handle_file(args.signer_cert, "read")
    cert_der = esp_handle_file(args.signer_cert, "pem_read")
    print("Signer cert is:")
    print(signer_cert_data)

    print("program signer cert")
    cert_def_str = hs.cert2certdef.esp_create_cert_def_str(cert_der, "SIGNER_CERT")

    retval = init_mfg.exec_cmd(esp._port, "provide-cert-def 1", cert_def_str)
    hs.serial.esp_cmd_check_ok(retval, "program-signer-cert-def")

    retval = init_mfg.exec_cmd(esp._port, "program-signer-cert", signer_cert_data)
    hs.serial.esp_cmd_check_ok(retval, "program-signer-cert")


def esp_handle_file(file_name, operation, data=None):
    if operation == "read":
        with open(file_name, "r") as cert_file:
            data = cert_file.read()
        return data
    elif operation == "pem_read":
        with open(file_name, "r") as cert_file:
            data = pem.readPemFromFile(cert_file)
        return data
    elif operation == "write":
        with open(file_name, "w+") as cert_file:
            cert_file.write(data)
        return True


if __name__ == "__main__":
    main()
