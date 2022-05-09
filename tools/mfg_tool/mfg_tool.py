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
Script to generate Matter factory NVS partition image, Onboarding codes, and QR codes.
"""

import os
import sys
import csv
import uuid
import shutil
import random
import logging
import binascii
import argparse
import pyqrcode
import subprocess
import cryptography.hazmat.backends
import cryptography.x509

from chip_nvs_keys import CHIP_KEY_MAP

if not os.getenv('IDF_PATH'):
    logging.error("IDF_PATH environment variable is not set")
    sys.exit(1)

# TODO: Handle count > 100 case. spake2p gen-verifier doesn't support count > 100

TOOLS = {
    'spake2p'   : None,
    'chip-cert' : None,
    'qrcodetool': None,
    'mfg_gen'   : None,
}

PAI = {
    'cert_pem': None,
    'cert_der': None,
    'key_pem' : None,
    'key_der' : None,
}

OUT_DIR = {
    'top' : None,
    'chip': None,
}

OUT_FILE = {
    'config_csv' : None,
    'chip_mcsv'  : None,
    'mcsv' : None,
    'pin_csv': None,
    'pin_disc_csv': None,
}

UNIQUE_ID_LEN = 16  # 16 bytes (128-bits)

INVALID_PASSCODES = [ 00000000, 11111111, 22222222, 33333333, 44444444, 55555555,
                      66666666, 77777777, 88888888, 99999999, 12345678, 87654321 ]

UUIDs = list()

def vid_pid_str(vid, pid):
    return '_'.join([hex(vid)[2:], hex(pid)[2:]])

def disc_pin_str(discriminator, passcode):
    return '_'.join([hex(discriminator)[2:], hex(passcode)[2:]])

def check_tools_exists():
    TOOLS['spake2p'] = shutil.which('spake2p')
    if TOOLS['spake2p'] is None:
        logging.error('spake2p not found, please add spake2p path to PATH environment variable')
        sys.exit(1)

    TOOLS['chip-cert'] = shutil.which('chip-cert')
    if TOOLS['chip-cert'] is None:
        logging.error('chip-cert not found, please add chip-cert path to PATH environment variable')
        sys.exit(1)

    TOOLS['qrcodetool'] = shutil.which('qrcodetool')
    if TOOLS['qrcodetool'] is None:
        logging.error('qrcodetool not found, please add qrcodetool path to PATH environment variable')
        sys.exit(1)

    TOOLS['mfg_gen'] = os.sep.join([os.getenv('IDF_PATH'), 'tools', 'mass_mfg', 'mfg_gen.py'])
    if not os.path.exists(TOOLS['mfg_gen']):
        logging.error('mfg_gen.py not found, please make sure IDF_PATH environment variable is set correctly')
        sys.exit(1)

    logging.debug('Using following tools:')
    logging.debug('spake2p:    {}'.format(TOOLS['spake2p']))
    logging.debug('chip-cert:  {}'.format(TOOLS['chip-cert']))
    logging.debug('qrcodetool: {}'.format(TOOLS['qrcodetool']))
    logging.debug('mfg_gen:    {}'.format(TOOLS['mfg_gen']))

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

def generate_passcodes(args):
    iter_count_max = 10000
    salt_len_max = 32

    cmd = [
        TOOLS['spake2p'], 'gen-verifier',
        '--count', str(args.count),
        '--iteration-count', str(iter_count_max),
        '--salt-len', str(salt_len_max),
        '--out', OUT_FILE['pin_csv'],
    ]

    # If passcode is provided, use it
    if (args.passcode):
        cmd.extend(['--pin-code', str(args.passcode)])

    execute_cmd(cmd)

def generate_discriminators(args):
    discriminators = list()

    # If discriminator is provided, use it
    if args.discriminator:
        discriminators.append(args.discriminator)
    else:
        for i in range(args.count):
            discriminators.append(random.randint(0x0000, 0x0FFF))

    return discriminators

# Append discriminator to each line of the passcode file
def append_discriminator(discriminator):
    with open(OUT_FILE['pin_csv'], 'r') as fd:
        lines = fd.readlines()

    lines[0] = ','.join([lines[0].strip(), 'Discriminator'])
    for i in range(1, len(lines)):
        lines[i] = ','.join([lines[i].strip(), str(discriminator[i - 1])])

    with open(OUT_FILE['pin_disc_csv'], 'w') as fd:
        fd.write('\n'.join(lines) + '\n')

    os.remove(OUT_FILE['pin_csv'])

# Generates the csv file containing chip specific keys and keys provided by user in csv file
def generate_config_csv(args):
    logging.info("Generating Config CSV...")

    csv_data = ''
    for k, v in CHIP_KEY_MAP.items():
        csv_data += k + ',' + 'namespace,' + '\n'
        for k1, v1 in v.items():
            csv_data += k1 + ',' + v1['type'] + ',' + v1['encoding'] + '\n'

    # Read data from the user provided csv file
    if args.csv:
        with open(args.csv, 'r') as f:
            csv_data += f.read()

    with open(OUT_FILE['config_csv'], 'w') as f:
        f.write(csv_data)

def write_chip_mcsv_header(args):
    logging.info('Writing chip manifest CSV header...')

    keys = list()
    for _, v in CHIP_KEY_MAP.items():
        for k1, _ in v.items():
            keys.append(k1)

    mcsv_header = ','.join(keys) + '\n'

    with open(OUT_FILE['chip_mcsv'], 'w') as f:
        f.write(mcsv_header)

def append_chip_mcsv_row(row_data, args):
    logging.info('Appending chip master CSV row...')

    with open(OUT_FILE['chip_mcsv'], 'a') as f:
        f.write(row_data + '\n')

# Convert the certificate in PEM format to DER format
def convert_x509_cert_from_pem_to_der(pem_file, out_der_file):
    with open(pem_file, 'rb') as f:
        pem_data = f.read()

    pem_cert = cryptography.x509.load_pem_x509_certificate(pem_data, cryptography.hazmat.backends.default_backend())
    der_cert = pem_cert.public_bytes(cryptography.hazmat.primitives.serialization.Encoding.DER)

    with open(out_der_file, 'wb') as f:
        f.write(der_cert)

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

def generate_pai(args, ca_key, ca_cert, out_key, out_cert):
    cmd = [
        TOOLS['chip-cert'], 'gen-att-cert',
        '--type', 'i',
        '--subject-cn', '"{} PAI {}"'.format(args.subject_cn_prefix, '00'),
        '--out-key', out_key,
        '--out', out_cert,
    ]

    if args.lifetime:
        cmd.extend(['--lifetime', str(args.lifetime)])
    if args.valid_from:
        cmd.extend(['--valid-from', str(args.valid_from)])

    cmd.extend([
        '--subject-vid', hex(args.vendor_id)[2:],
        '--subject-pid', hex(args.product_id)[2:],
        '--ca-key', ca_key,
        '--ca-cert', ca_cert,
    ])

    execute_cmd(cmd)
    logging.info('Generated PAI certificate: {}'.format(out_cert))
    logging.info('Generated PAI private key: {}'.format(out_key))

def generate_dac(iteration, args, discriminator, passcode, ca_key, ca_cert):
    out_key_pem  = os.sep.join([OUT_DIR['top'], UUIDs[iteration], 'internal', 'DAC_key.pem'])
    out_cert_pem = out_key_pem.replace('key.pem', 'cert.pem')
    out_cert_der = out_key_pem.replace('key.pem', 'cert.der')
    out_private_key_bin = out_key_pem.replace('key.pem', 'private_key.bin')
    out_public_key_bin  = out_key_pem.replace('key.pem', 'public_key.bin')

    cmd = [
        TOOLS['chip-cert'], 'gen-att-cert',
        '--type', 'd',
        '--subject-cn', '"{} DAC {}"'.format(args.subject_cn_prefix, iteration),
        '--out-key', out_key_pem,
        '--out', out_cert_pem,
    ]

    if args.lifetime:
        cmd.extend(['--lifetime', str(args.lifetime)])
    if args.valid_from:
        cmd.extend(['--valid-from', str(args.valid_from)])

    cmd.extend(['--subject-vid', hex(args.vendor_id)[2:],
        '--subject-pid', hex(args.product_id)[2:],
        '--ca-key', ca_key,
        '--ca-cert', ca_cert,
    ])

    execute_cmd(cmd)
    logging.info('Generated DAC certificate: {}'.format(out_cert_pem))
    logging.info('Generated DAC private key: {}'.format(out_key_pem))

    convert_x509_cert_from_pem_to_der(out_cert_pem, out_cert_der)
    logging.info('Generated DAC certificate in DER format: {}'.format(out_cert_der))

    generate_keypair_bin(out_key_pem, out_private_key_bin, out_public_key_bin)
    logging.info('Generated DAC private key in binary format: {}'.format(out_private_key_bin))
    logging.info('Generated DAC public key in binary format: {}'.format(out_public_key_bin))

    return out_cert_der, out_private_key_bin, out_public_key_bin

def use_dac_from_args(args):
    logging.info('Using DAC from command line arguments...')
    logging.info('DAC Certificate: {}'.format(args.dac_cert))
    logging.info('DAC Private Key: {}'.format(args.dac_key))

    # There should be only one UUID in the UUIDs list if DAC is specified
    out_cert_der = os.sep.join([OUT_DIR['top'], UUIDs[0], 'internal', 'DAC_cert.der'])
    out_private_key_bin = out_cert_der.replace('cert.der', 'private_key.bin')
    out_public_key_bin  = out_cert_der.replace('cert.der', 'public_key.bin')

    convert_x509_cert_from_pem_to_der(args.dac_cert, out_cert_der)
    logging.info('Generated DAC certificate in DER format: {}'.format(out_cert_der))

    generate_keypair_bin(args.dac_key, out_private_key_bin, out_public_key_bin)
    logging.info('Generated DAC private key in binary format: {}'.format(out_private_key_bin))
    logging.info('Generated DAC public key in binary format: {}'.format(out_public_key_bin))

    return out_cert_der, out_private_key_bin, out_public_key_bin

def setup_out_dirs(vid, pid, count):
    OUT_DIR['top']  = os.sep.join(['out', vid_pid_str(vid, pid)])
    OUT_DIR['stage'] = os.sep.join(['out', vid_pid_str(vid, pid), 'staging'])

    os.makedirs(OUT_DIR['top'], exist_ok=True)
    os.makedirs(OUT_DIR['stage'], exist_ok=True)

    OUT_FILE['config_csv']   = os.sep.join([OUT_DIR['stage'], 'config.csv'])
    OUT_FILE['chip_mcsv']    = os.sep.join([OUT_DIR['stage'], 'chip_master.csv'])
    OUT_FILE['mcsv']         = os.sep.join([OUT_DIR['stage'], 'master.csv'])
    OUT_FILE['pin_csv']      = os.sep.join([OUT_DIR['stage'], 'pin.csv'])
    OUT_FILE['pin_disc_csv'] = os.sep.join([OUT_DIR['stage'], 'pin_disc.csv'])

    # Create directories to store the generated files
    for i in range(count):
        uuid_str = str(uuid.uuid4())
        UUIDs.append(uuid_str)
        os.makedirs(os.sep.join([OUT_DIR['top'], uuid_str, 'internal']), exist_ok=True)

def generate_passcodes_and_discriminators(args):
    # Generate passcodes using spake2p tool
    generate_passcodes(args)
    # Randomly generate discriminators
    discriminators = generate_discriminators(args)
    # Append discriminators to passcodes file
    append_discriminator(discriminators)

def setup_csv_files(args):
    generate_config_csv(args)
    write_chip_mcsv_header(args)

def setup_root_certs(args):
    # If PAA is passed as input, then generate PAI certificate
    if args.paa:
        # output file names
        PAI['cert_pem'] = os.sep.join([OUT_DIR['stage'], 'pai_cert.pem'])
        PAI['cert_der'] = os.sep.join([OUT_DIR['stage'], 'pai_cert.der'])
        PAI['key_pem']  = os.sep.join([OUT_DIR['stage'], 'pai_key.pem'])

        generate_pai(args, args.key, args.cert, PAI['key_pem'], PAI['cert_pem'])
        convert_x509_cert_from_pem_to_der(PAI['cert_pem'], PAI['cert_der'])
        logging.info('Generated PAI certificate in DER format: {}'.format(PAI['cert_der']))

    # If PAI is passed as input, generate DACs
    elif args.pai:
        PAI['cert_pem'] = args.cert
        PAI['key_pem']  = args.key
        PAI['cert_der'] = os.sep.join([OUT_DIR['stage'], 'pai_cert.der'])

        convert_x509_cert_from_pem_to_der(PAI['cert_pem'], PAI['cert_der'])
        logging.info('Generated PAI certificate in DER format: {}'.format(PAI['cert_der']))

    # No need to verify else block as validate_args() already checks for this

# Generates DACs, add the required items to the master.csv file, and
# generates the onboarding data i.e. manual codes and QR codes and image
def generate_dacs_and_onb_data(args):
    with open(OUT_FILE['pin_disc_csv'], 'r') as csvf:
        pin_disc_dict = csv.DictReader(csvf)

        # If any key is added to chip_nvs_keys.py make sure to maintain that order below
        for row in pin_disc_dict:
            row_items = list()

            row_items.append(row['Discriminator'])
            # TODO: remove PIN Code, spec says it should not be included in nvs storage
            row_items.append(row['PIN Code'])
            row_items.append(row['Iteration Count'])
            row_items.append(row['Salt'])
            row_items.append(row['Verifier'])

            if args.dac_key is not None and args.dac_cert is not None:
                dacs = use_dac_from_args(args)
            else:
                dacs = generate_dac(int(row['Index']), args, int(row['Discriminator']), int(row['PIN Code']), PAI['key_pem'], PAI['cert_pem'])

            # This appends DAC cert, private key, and public key
            row_items.extend([os.path.abspath(x) for x in dacs])

            row_items.append(os.path.abspath(PAI['cert_der']))
            row_items.append(os.path.abspath(args.cert_dclrn))

            # Unique-id
            row_items.append(binascii.b2a_hex(os.urandom(UNIQUE_ID_LEN)).decode('utf-8'))

            mcsv_row_data = ','.join(row_items)
            append_chip_mcsv_row(mcsv_row_data, args)

            # Generate onboarding data
            generate_onboarding_data(args, int(row['Index']), int(row['Discriminator']), int(row['PIN Code']))

def merge_chip_mcsv_and_user_mcsv(args):
    logging.info('Merging chip master CSV and user master CSV...')

    with open(OUT_FILE['chip_mcsv'], 'r') as f:
        chip_mcsv_data = f.readlines()

    # If user mcsv is present, merge it with chip mcsv
    if args.mcsv:
        logging.info('User manifest CSV is present. Merging...')

        with open(args.mcsv, 'r') as f:
            user_mcsv_data = f.readlines()

        if (len(chip_mcsv_data) != len(user_mcsv_data)):
            logging.error('Chip and user mcsv files have different number of rows')
            sys.exit(1)

        chip_mcsv_data = [','.join((c_line.strip(), u_line.strip())) + '\n' for c_line, u_line in zip(chip_mcsv_data, user_mcsv_data)]

    with open(OUT_FILE['mcsv'], 'w') as f:
        f.write(''.join(chip_mcsv_data))

    os.remove(OUT_FILE['chip_mcsv'])

def organize_output_files(suffix):
    for i in range(len(UUIDs)):
        dest_path = os.sep.join([OUT_DIR['top'], UUIDs[i]])
        internal_path = os.sep.join([dest_path, 'internal'])

        replace = os.sep.join([OUT_DIR['top'], 'bin', '{}-{}.bin'.format(suffix, str(i + 1))])
        replace_with = os.sep.join([dest_path, '{}-partition.bin'.format(UUIDs[i])])
        os.rename(replace, replace_with)

        replace = os.sep.join([OUT_DIR['top'], 'csv', '{}-{}.csv'.format(suffix, str(i + 1))])
        replace_with = os.sep.join([internal_path, 'partition.csv'])
        os.rename(replace, replace_with)

        # Also copy the PAI certificate to the output directory
        shutil.copy2(PAI['cert_der'], os.sep.join([internal_path, 'PAI_cert.der']))

        logging.info('Generated output files at: {}'.format(os.sep.join([OUT_DIR['top'], UUIDs[i]])))

    os.rmdir(os.sep.join([OUT_DIR['top'], 'bin']))
    os.rmdir(os.sep.join([OUT_DIR['top'], 'csv']))

def generate_partitions(suffix, size):
    cmd = [
        TOOLS['mfg_gen'], 'generate',
        OUT_FILE['config_csv'], OUT_FILE['mcsv'],
        suffix, hex(size), '--outdir', OUT_DIR['top']
    ]
    execute_cmd(cmd)

def get_setup_payload_data(args, discriminator, passcode):
    data = 'version 0\n'
    data += 'vendorID ' + str(args.vendor_id) + '\n'
    data += 'productID ' + str(args.product_id) + '\n'
    data += 'commissioningFlow ' + str(args.commissioning_flow) + '\n'
    data += 'rendezVousInformation ' + str(1 << args.discovery_mode) + '\n'
    data += 'discriminator ' + str(discriminator) + '\n'
    data += 'setUpPINCode ' + str(passcode) + '\n'
    return data

def get_chip_qrcode(file):
    data = subprocess.check_output([TOOLS['qrcodetool'], 'generate-qr-code', '-f', file])
    data = data.decode('utf-8').splitlines()
    qrcodeline = data[-1].split(': ')

    if qrcodeline[1] == 'QR Code':
        return qrcodeline[-1]
    else:
        logging.error('Failed to generate QR code')
        return None

def get_chip_manualcode(file):
    data = subprocess.check_output([TOOLS['qrcodetool'], 'generate-manual-code', '-f', file])
    data = data.decode('utf-8').splitlines()
    manualcodeline = data[-1].split(': ')

    if manualcodeline[1] == 'Manual Code':
        return manualcodeline[-1]
    else:
        logging.error('Failed to generate manual code')
        return None

def generate_onboarding_data(args, index, discriminator, passcode):
    setup_payload_file = os.sep.join(['', 'tmp', 'setup_payload_{}.txt'.format(args.vendor_id, args.product_id)])

    with open(setup_payload_file, 'w') as f:
        f.write(get_setup_payload_data(args, discriminator, passcode))

    chip_qrcode     = get_chip_qrcode(setup_payload_file)
    chip_manualcode = get_chip_manualcode(setup_payload_file)

    logging.info('Generated QR code: ' + chip_qrcode)
    logging.info('Generated manual code: ' + chip_manualcode)

    # TODO: Append commissioning flow, discovery mode, passcode, discriminator as well?
    csv_data = 'qrcode,manualcode\n'
    csv_data += chip_qrcode + ',' + chip_manualcode + '\n'

    onboarding_data_file = os.sep.join([OUT_DIR['top'], UUIDs[index], '{}-onb_codes.csv'.format(UUIDs[index])])
    with open(onboarding_data_file, 'w') as f:
        f.write(csv_data)

    # Create QR code image as mentioned in the spec
    qrcode_file = os.sep.join([OUT_DIR['top'], UUIDs[index], '{}-qrcode.png'.format(UUIDs[index])])
    chip_qr = pyqrcode.create(chip_qrcode, version=2, error='M')
    chip_qr.png(qrcode_file, scale=6)

    os.remove(setup_payload_file)
    logging.info('Generated onboarding data and QR Code')

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

    # Validate the passcode
    if args.passcode is not None:
        if ((args.passcode < 0x0000001 and args.passcode > 0x5F5E0FE) or (args.passcode in INVALID_PASSCODES)):
            logging.error('Invalid passcode' + str(args.passcode))
            sys.exit(1)

    # Validate the discriminator
    if (args.discriminator is not None) and (args.discriminator not in range(0x0000, 0x0FFF)):
        logging.error('Invalid discriminator ' + str(args.discriminator))
        sys.exit(1)

    if args.unique_id is not None:
        if len(args.unique_id) < (UNIQUE_ID_LEN * 2):
            logging.error('Unique ID is too short')
            sys.exit(1)

        if (len(args.unique_id) % 2) != 0:
            logging.error('Unique ID is not even length')
            sys.exit(1)

        # Check for a valid hex string
        try:
            int(args.unique_id, 16)
        except ValueError:
            logging.error('Unique ID is not a valid hex string')
            sys.exit(1)

    # DAC key and DAC cert both should be present or none
    if (args.dac_key is not None) != (args.dac_cert is not None):
        logging.error("dac_key and dac_cert should be both present or none")
        sys.exit(1)
    else:
        # Make sure PAI certificate is present if DAC is present
        if (args.dac_key is not None) and (args.pai is False):
            logging.error('Please provide PAI certificate along with DAC certificate and DAC key')
            sys.exit(1)

    # If unique_id/discriminator/passcode/DAC is present then we are restricting
    # the number of partitions to 1
    if (args.discriminator is not None
            or args.passcode is not None
            or args.unique_id is not None
            or args.dac_key is not None):
        if args.count > 1:
            logging.error('Number of partitions should be 1 when unique_id or discriminator or passcode or DAC is present')
            sys.exit(1)

    # Validate the input certificate type, if DAC is not present
    if args.dac_key is None and args.dac_cert is None:
        if args.paa:
            logging.info('Input Root certificate type PAA')
        elif args.pai:
            logging.info('Input Root certificate type PAI')
        else:
            logging.error('Either PAA or PAI certificate is required')
            sys.exit(1)

        # Check if Key and certificate are present
        if args.key is None or args.cert is None:
            logging.error('PAA key and certificate are required')
            sys.exit(1)

    logging.info('Number of manufacturing NVS images to generate: {}'.format(args.count))

def get_args():
    def any_base_int(s): return int(s, 0)

    parser = argparse.ArgumentParser(description='Manufacuring partition generator tool')
    parser.add_argument('-n', '--count', type=any_base_int, default=1, help='The number of manufacturing partition binaries to generate. Default is 1. If --csv and --mcsv are present, the number of lines in the mcsv file is used.')
    parser.add_argument('-s', '--size', type=any_base_int, default=0x6000, help='The size of manufacturing partition binaries to generate. Default is 0x6000.')
    parser.add_argument('-cn', '--subject-cn-prefix', type=str, default='ESP32', help='The common name prefix of the subject of the generated certificate.')

    parser.add_argument('-v', '--vendor-id', type=any_base_int, required=True, help='The vendor ID.')
    parser.add_argument('-p', '--product-id', type=any_base_int, required=True, help='The product ID.')

    input_cert_group = parser.add_mutually_exclusive_group(required=True)
    input_cert_group.add_argument('--paa', action='store_true', help='Use input certificate as PAA certificate.')
    input_cert_group.add_argument('--pai', action='store_true', help='Use input certificate as PAI certificate.')

    # If DAC is present then PAI key is not required, so it is marked as not required here
    # but, if DAC is not present then PAI key is required and that case is validated in validate_args()
    parser.add_argument('-c', '--cert', type=str, required=True, help='The input certificate file in PEM format.')
    parser.add_argument('-k', '--key', type=str, required=False, help='The input key file in PEM format.')

    parser.add_argument('-cd', '--cert-dclrn', type=str, required=True, help='The certificate declaration file in DER format.')

    parser.add_argument('--dac-cert', type=str, help='The input DAC certificate file in PEM format.')
    parser.add_argument('--dac-key', type=str, help='The input DAC private key file in PEM format.');

    parser.add_argument('-lt', '--lifetime', default=4294967295, type=any_base_int, help='Lifetime of the generated certificate. Default is 4294967295 if not specified, this indicate that certificate does not have well defined expiration date.')
    parser.add_argument('-vf', '--valid-from', type=str, help='The start date for the certificate validity period in format <YYYY>-<MM>-<DD> [ <HH>:<MM>:<SS> ]. Default is current date.')

    parser.add_argument('--passcode', type=any_base_int, help='The passcode for pairing. Randomly generated if not specified.')
    parser.add_argument('--discriminator', type=any_base_int, help='The discriminator for pairing. Randomly generated if not specified.')

    parser.add_argument('-cf', '--commissioning-flow', type=any_base_int, default=0, help='Device commissioning flow, 0:Standard, 1:User-Intent, 2:Custom. Default is 0.', choices=[0, 1, 2])
    parser.add_argument('-dm', '--discovery-mode', type=any_base_int, default=1, help='Commissionable device discovery netowrking technology. 0:WiFi-SoftAP, 1:BLE, 2:On-network. Default is BLE.', choices=[0, 1, 2])

    parser.add_argument('--csv', type=str, help='CSV file containing the partition schema for extra options. [REF: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/mass_mfg.html#csv-configuration-file]')
    parser.add_argument('--mcsv', type=str, help='Master CSV file containig optional/extra values specified by the user. [REF: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/mass_mfg.html#master-value-csv-file]')

    parser.add_argument('-u', '--unique-id', type=str, help='Unique Identifier for the device. This should be at least 128-bit long. If not specified, a random unique id is generated.')

    return parser.parse_args()

def main():
    logging.basicConfig(format='[%(asctime)s] [%(levelname)7s] - %(message)s', level=logging.INFO)

    args = get_args()
    validate_args(args)

    check_tools_exists()

    setup_out_dirs(args.vendor_id, args.product_id, args.count)
    generate_passcodes_and_discriminators(args)
    setup_csv_files(args)
    setup_root_certs(args)
    generate_dacs_and_onb_data(args)
    merge_chip_mcsv_and_user_mcsv(args)
    generate_partitions('matter_partition', args.size)
    organize_output_files('matter_partition')

if __name__ == "__main__":
    main()
