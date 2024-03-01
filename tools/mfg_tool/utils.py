#!/usr/bin/env python3

# Copyright 2022 Espressif Systems (Shanghai) PTE LTD
#
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

"""
Contains utilitiy functions for validating argument, certs/keys conversion, etc.
"""

import sys
import enum
import logging
import subprocess
from bitarray import bitarray
from bitarray.util import ba2int
import cryptography.hazmat.backends
import cryptography.x509
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.backends import default_backend

ROTATING_DEVICE_ID_UNIQUE_ID_LEN_BITS = 128
SERIAL_NUMBER_LEN = 16


# Lengths for manual pairing codes and qrcode
SHORT_MANUALCODE_LEN = 11
LONG_MANUALCODE_LEN = 21
QRCODE_LEN = 22


INVALID_PASSCODES = [00000000, 11111111, 22222222, 33333333, 44444444, 55555555,
                     66666666, 77777777, 88888888, 99999999, 12345678, 87654321]


class CalendarTypes(enum.Enum):
    Buddhist = 0
    Chinese = 1
    Coptic = 2
    Ethiopian = 3
    Gregorian = 4
    Hebrew = 5
    Indian = 6
    Islamic = 7
    Japanese = 8
    Korean = 9
    Persian = 10
    Taiwanese = 11


def vid_pid_str(vid, pid):
    return '_'.join([hex(vid)[2:], hex(pid)[2:]])


def disc_pin_str(discriminator, passcode):
    return '_'.join([hex(discriminator)[2:], hex(passcode)[2:]])


# Checks if the input string is a valid hex string
def ishex(s):
    try:
        n = int(s, 16)
        return True
    except ValueError:
        return False


# Validate the input string length against the min and max length
def check_str_range(s, min_len, max_len, name):
    if s and ((len(s) < min_len) or (len(s) > max_len)):
        logging.error('%s must be between %d and %d characters', name, min_len, max_len)
        sys.exit(1)


# Validate the input integer range
def check_int_range(value, min_value, max_value, name):
    if value and ((value < min_value) or (value > max_value)):
        logging.error('%s is out of range, should be in range [%d, %d]', name, min_value, max_value)
        sys.exit(1)


# Validates discriminator and passcode
def validate_commissionable_data(args):
    check_int_range(args.discriminator, 0x0000, 0x0FFF, 'Discriminator')
    if args.passcode is not None:
        if ((args.passcode < 0x0000001 and args.passcode > 0x5F5E0FE) or (args.passcode in INVALID_PASSCODES)):
            logging.error('Invalid passcode' + str(args.passcode))
            sys.exit(1)


# Validate the device instance information
def validate_device_instance_info(args):
    check_int_range(args.product_id, 0x0000, 0xFFFF, 'Product id')
    check_int_range(args.vendor_id, 0x0000, 0xFFFF, 'Vendor id')
    check_int_range(args.hw_ver, 0x0000, 0xFFFF, 'Hardware version')
    check_str_range(args.serial_num, 1, SERIAL_NUMBER_LEN, 'Serial number')
    check_str_range(args.vendor_name, 1, 32, 'Vendor name')
    check_str_range(args.product_name, 1, 32, 'Product name')
    check_str_range(args.hw_ver_str, 1, 64, 'Hardware version string')
    check_str_range(args.mfg_date, 8, 16, 'Manufacturing date')
    check_str_range(args.rd_id_uid, 32, 32, 'Rotating device Unique id')


# Validate the device information: calendar types and fixed labels
def validate_device_info(args):
    # Validate the input calendar types
    if args.calendar_types is not None:
        if not (set(args.calendar_types) <= set(CalendarTypes.__members__)):
            invalid_types = set(args.calendar_types).union(set(CalendarTypes.__members__)) - set(CalendarTypes.__members__)
            logging.error('Unknown calendar type/s: %s', invalid_types)
            logging.error('Supported calendar types: %s', ', '.join(CalendarTypes.__members__))
            sys.exit(1)

    if args.fixed_labels is not None:
        for fl in args.fixed_labels:
            _l = fl.split('/')
            if len(_l) != 3:
                logging.error('Invalid fixed label: %s', fl)
                sys.exit(1)

            if not (ishex(_l[0]) and (len(_l[1]) > 0 and len(_l[1]) < 16) and (len(_l[2]) > 0 and len(_l[2]) < 16)):
                logging.error('Invalid fixed label: %s', fl)
                sys.exit(1)


# Validates the attestation related arguments
def validate_attestation_info(args):
    # DAC key and DAC cert both should be present or none
    if (args.dac_key is not None) != (args.dac_cert is not None):
        logging.error("dac_key and dac_cert should be both present or none")
        sys.exit(1)
    else:
        # Make sure PAI certificate is present if DAC is present
        if (args.dac_key is not None) and (args.pai is False):
            logging.error('Please provide PAI certificate along with DAC certificate and DAC key')
            sys.exit(1)

    # Validate the input certificate type, if DAC is not present
    if args.dac_key is None and args.dac_cert is None:
        if args.paa:
            logging.info('Input Root certificate type PAA')
        elif args.pai:
            logging.info('Input Root certificate type PAI')
        else:
            logging.info('Do not include the device attestation certificates and keys in partition binaries')

        # Check if Key and certificate are present
        if (args.paa or args.pai) and (args.key is None or args.cert is None):
            logging.error('CA key and certificate are required to generate DAC key and certificate')
            sys.exit(1)


# Validates few basic cluster related arguments: product-label and product-url
def validate_basic_cluster_info(args):
    check_str_range(args.product_label, 1, 64, 'Product Label')
    check_str_range(args.product_url, 1, 256, 'Product URL')


# Validates the input arguments, this calls the above functions
def validate_args(args):
    # csv and mcsv both should present or none
    if (args.csv is not None) != (args.mcsv is not None):
        logging.error("csv and mcsv should be both present or none")
        sys.exit(1)
    else:
        # Read the number of lines in mcsv file
        if args.mcsv is not None:
            with open(args.mcsv, 'r') as f:
                lines = sum(1 for line in f)

            # Subtract 1 for the header line
            args.count = lines - 1

    validate_commissionable_data(args)
    validate_device_instance_info(args)
    validate_device_info(args)
    validate_attestation_info(args)
    validate_basic_cluster_info(args)

    # If discriminator/passcode/DAC/serial_number/rotating_device_id is present
    # then we are restricting the number of partitions to 1
    if (args.discriminator is not None
            or args.passcode is not None
            or args.dac_key is not None
            or args.serial_num is not None
            or args.rd_id_uid is not None):
        if args.count > 1:
            logging.error('Number of partitions should be 1 when discriminator or passcode or DAC or serial number or rotating device id is present')
            sys.exit(1)

    logging.info('Number of manufacturing NVS images to generate: {}'.format(args.count))


# Supported Calendar types is stored as a bit array in one uint32_t.
def calendar_types_to_uint32(calendar_types):
    # In validate_device_info() we have already verified that the calendar types are valid
    result = bitarray(32, endian='little')
    result.setall(0)
    for calendar_type in calendar_types:
        result[CalendarTypes[calendar_type].value] = 1
    return ba2int(result)


# get_fixed_label_dict() converts the list of strings to per endpoint dictionaries.
# example input  : ['0/orientation/up', '1/orientation/down', '2/orientation/down']
# example outout : {'0': [{'orientation': 'up'}], '1': [{'orientation': 'down'}], '2': [{'orientation': 'down'}]}
def get_fixed_label_dict(fixed_labels):
    fl_dict = {}
    for fl in fixed_labels:
        _l = fl.split('/')

        if len(_l) != 3:
            logging.error('Invalid fixed label: %s', fl)
            sys.exit(1)

        if not (ishex(_l[0]) and (len(_l[1]) > 0 and len(_l[1]) < 16) and (len(_l[2]) > 0 and len(_l[2]) < 16)):
            logging.error('Invalid fixed label: %s', fl)
            sys.exit(1)

        if _l[0] not in fl_dict.keys():
            fl_dict[_l[0]] = list()

        fl_dict[_l[0]].append({_l[1]: _l[2]})

    return fl_dict


# get_supported_modes_dict() converts the list of strings to per endpoint dictionaries.
# example with semantic tags
# input  : ['0/label1/1/"1\0x8000, 2\0x8000" 1/label2/1/"1\0x8000, 2\0x8000"']
# outout : {'1': [{'Label': 'label1', 'Mode': 0, 'Semantic_Tag': [{'value': 1, 'mfgCode': 32768}, {'value': 2, 'mfgCode': 32768}]}, {'Label': 'label2', 'Mode': 1, 'Semantic_Tag': [{'value': 1, 'mfgCode': 32768}, {'value': 2, 'mfgCode': 32768}]}]}

# example without semantic tags
# input  : ['0/label1/1 1/label2/1']
# outout : {'1': [{'Label': 'label1', 'Mode': 0, 'Semantic_Tag': []}, {'Label': 'label2', 'Mode': 1, 'Semantic_Tag': []}]}

def get_supported_modes_dict(supported_modes):
    output_dict = {}

    for mode_str in supported_modes:
        mode_label_strs = mode_str.split('/')
        mode = mode_label_strs[0]
        label = mode_label_strs[1]
        ep = mode_label_strs[2]

        semantic_tags = ''
        if (len(mode_label_strs) == 4):
            semantic_tag_strs = mode_label_strs[3].split(', ')
            semantic_tags = [{"value": int(v.split('\\')[0]), "mfgCode": int(v.split('\\')[1], 16)} for v in semantic_tag_strs]

        mode_dict = {"Label": label, "Mode": int(mode), "Semantic_Tag": semantic_tags}

        if ep in output_dict:
            output_dict[ep].append(mode_dict)
        else:
            output_dict[ep] = [mode_dict]

    return output_dict


# Convert the certificate in PEM format to DER format
def convert_x509_cert_from_pem_to_der(pem_file, out_der_file):
    with open(pem_file, 'rb') as f:
        pem_data = f.read()

    pem_cert = cryptography.x509.load_pem_x509_certificate(pem_data, default_backend())
    der_cert = pem_cert.public_bytes(serialization.Encoding.DER)

    with open(out_der_file, 'wb') as f:
        f.write(der_cert)

def convert_private_key_from_pem_to_der(pem_file, out_der_file):
    with open(pem_file, 'rb') as f:
        pem_data = f.read()

    pem_key = serialization.load_pem_private_key(pem_data, None, default_backend())

    der_key = pem_key.private_bytes(
        encoding=serialization.Encoding.DER,
        format=serialization.PrivateFormat.TraditionalOpenSSL,
        encryption_algorithm=serialization.NoEncryption(),
    )

    with open(out_der_file, 'wb') as f:
        f.write(der_key)

# Generate the Public and Private key pair binaries
def generate_keypair_bin(pem_file, out_privkey_bin, out_pubkey_bin):
    with open(pem_file, 'rb') as f:
        pem_data = f.read()

    key_pem = cryptography.hazmat.primitives.serialization.load_pem_private_key(pem_data, None)
    private_number_val = key_pem.private_numbers().private_value
    public_number_x = key_pem.public_key().public_numbers().x
    public_number_y = key_pem.public_key().public_numbers().y
    public_key_first_byte = 0x04

    with open(out_privkey_bin, 'wb') as f:
        f.write(private_number_val.to_bytes(32, byteorder='big'))

    with open(out_pubkey_bin, 'wb') as f:
        f.write(public_key_first_byte.to_bytes(1, byteorder='big'))
        f.write(public_number_x.to_bytes(32, byteorder='big'))
        f.write(public_number_y.to_bytes(32, byteorder='big'))


def execute_cmd(cmd):
    logging.debug('Executing Command: {}'.format(cmd))
    status = subprocess.run(cmd, capture_output=True)

    try:
        status.check_returncode()
    except subprocess.CalledProcessError as e:
        if status.stderr:
            logging.error('[stderr]: {}'.format(status.stderr.decode('utf-8').strip()))
        logging.error('Command failed with error: {}'.format(e))
        sys.exit(1)
