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

from chip_nvs import *
from utils import *
from datetime import datetime

if not os.getenv('IDF_PATH'):
    logging.error("IDF_PATH environment variable is not set")
    sys.exit(1)

TOOLS = {
    'spake2p': None,
    'chip-cert': None,
    'chip-tool': None,
    'mfg_gen': None,
}

PAI = {
    'cert_pem': None,
    'cert_der': None,
    'key_pem': None,
    'key_der': None,
}

OUT_DIR = {
    'top': None,
    'chip': None,
}

OUT_FILE = {
    'config_csv': None,
    'mcsv': None,
    'pin_csv': None,
    'pin_disc_csv': None,
}

UUIDs = list()


def check_tools_exists(args):
    TOOLS['spake2p'] = shutil.which('spake2p')
    if TOOLS['spake2p'] is None:
        logging.error('spake2p not found, please add spake2p path to PATH environment variable')
        sys.exit(1)
    # if the certs and keys are not in the generated partitions or the specific dac cert and key are used,
    # the chip-cert is not needed.
    if args.paa or (args.pai and (args.dac_cert is None and args.dac_key is None)):
        TOOLS['chip-cert'] = shutil.which('chip-cert')
        if TOOLS['chip-cert'] is None:
            logging.error('chip-cert not found, please add chip-cert path to PATH environment variable')
            sys.exit(1)

    TOOLS['chip-tool'] = shutil.which('chip-tool')
    if TOOLS['chip-tool'] is None:
        logging.error('chip-tool not found, please add chip-tool path to PATH environment variable')
        sys.exit(1)

    TOOLS['mfg_gen'] = os.sep.join([os.getenv('IDF_PATH'), 'tools', 'mass_mfg', 'mfg_gen.py'])
    if not os.path.exists(TOOLS['mfg_gen']):
        logging.error('mfg_gen.py not found, please make sure IDF_PATH environment variable is set correctly')
        sys.exit(1)

    logging.debug('Using following tools:')
    logging.debug('spake2p:    {}'.format(TOOLS['spake2p']))
    logging.debug('chip-cert:  {}'.format(TOOLS['chip-cert']))
    logging.debug('chip-tool:  {}'.format(TOOLS['chip-tool']))
    logging.debug('mfg_gen:    {}'.format(TOOLS['mfg_gen']))


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
    csv_data = chip_nvs_get_config_csv()

    with open(OUT_FILE['config_csv'], 'w') as f:
        f.write(csv_data)


def write_chip_mcsv_header(args):
    logging.info('Writing chip manifest CSV header...')
    mcsv_header = chip_get_keys_as_csv() + '\n'
    with open(OUT_FILE['mcsv'], 'w') as f:
        f.write(mcsv_header)


def append_chip_mcsv_row(row_data):
    logging.info('Appending chip master CSV row...')
    with open(OUT_FILE['mcsv'], 'a') as f:
        f.write(row_data + '\n')


def generate_pai(args, ca_key, ca_cert, out_key, out_cert):
    cmd = [
        TOOLS['chip-cert'], 'gen-att-cert',
        '--type', 'i',
        '--subject-cn', '"{} PAI {}"'.format(args.cn_prefix, '00'),
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
    out_key_pem = os.sep.join([OUT_DIR['top'], UUIDs[iteration], 'internal', 'DAC_key.pem'])
    out_cert_pem = out_key_pem.replace('key.pem', 'cert.pem')
    out_cert_der = out_key_pem.replace('key.pem', 'cert.der')
    out_private_key_bin = out_key_pem.replace('key.pem', 'private_key.bin')
    out_public_key_bin = out_key_pem.replace('key.pem', 'public_key.bin')

    cmd = [
        TOOLS['chip-cert'], 'gen-att-cert',
        '--type', 'd',
        '--subject-cn', '"{} DAC {}"'.format(args.cn_prefix, iteration),
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
    out_public_key_bin = out_cert_der.replace('cert.der', 'public_key.bin')

    convert_x509_cert_from_pem_to_der(args.dac_cert, out_cert_der)
    logging.info('Generated DAC certificate in DER format: {}'.format(out_cert_der))

    generate_keypair_bin(args.dac_key, out_private_key_bin, out_public_key_bin)
    logging.info('Generated DAC private key in binary format: {}'.format(out_private_key_bin))
    logging.info('Generated DAC public key in binary format: {}'.format(out_public_key_bin))

    return out_cert_der, out_private_key_bin, out_public_key_bin


def setup_out_dirs(vid, pid, count):
    OUT_DIR['top'] = os.sep.join(['out', vid_pid_str(vid, pid)])
    OUT_DIR['stage'] = os.sep.join(['out', vid_pid_str(vid, pid), 'staging'])

    os.makedirs(OUT_DIR['top'], exist_ok=True)
    os.makedirs(OUT_DIR['stage'], exist_ok=True)

    OUT_FILE['config_csv'] = os.sep.join([OUT_DIR['stage'], 'config.csv'])
    OUT_FILE['mcsv'] = os.sep.join([OUT_DIR['stage'], 'master.csv'])
    OUT_FILE['pin_csv'] = os.sep.join([OUT_DIR['stage'], 'pin.csv'])
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


def write_csv_files(args):
    generate_config_csv(args)
    write_chip_mcsv_header(args)


def setup_root_certs(args):
    # If PAA is passed as input, then generate PAI certificate
    if args.paa:
        # output file names
        PAI['cert_pem'] = os.sep.join([OUT_DIR['stage'], 'pai_cert.pem'])
        PAI['cert_der'] = os.sep.join([OUT_DIR['stage'], 'pai_cert.der'])
        PAI['key_pem'] = os.sep.join([OUT_DIR['stage'], 'pai_key.pem'])

        generate_pai(args, args.key, args.cert, PAI['key_pem'], PAI['cert_pem'])
        convert_x509_cert_from_pem_to_der(PAI['cert_pem'], PAI['cert_der'])
        logging.info('Generated PAI certificate in DER format: {}'.format(PAI['cert_der']))

    # If PAI is passed as input, generate DACs
    elif args.pai:
        PAI['cert_pem'] = args.cert
        PAI['key_pem'] = args.key
        PAI['cert_der'] = os.sep.join([OUT_DIR['stage'], 'pai_cert.der'])

        convert_x509_cert_from_pem_to_der(PAI['cert_pem'], PAI['cert_der'])
        logging.info('Generated PAI certificate in DER format: {}'.format(PAI['cert_der']))


def overwrite_values_in_mcsv(args, index):
    with open(args.mcsv, 'r') as mcsvf:
        mcsv_dict = list(csv.DictReader(mcsvf))[index]
        with open(args.csv, 'r') as csvf:
            csv_reader = csv.reader(csvf, delimiter=',')
            current_namespace = 'chip-factory'
            for csv_data in csv_reader:
                if 'namespace' in csv_data:
                    current_namespace = csv_data[0]
                else:
                    chip_nvs_map_update(current_namespace, csv_data[0], csv_data[1], csv_data[2], mcsv_dict[csv_data[0]])


# This function generates the DACs, picks the commissionable data from the already present csv file,
# and generates the onboarding payloads, and writes everything to the master csv
def write_per_device_unique_data(args):
    with open(OUT_FILE['pin_disc_csv'], 'r') as csvf:
        pin_disc_dict = csv.DictReader(csvf)

        for row in pin_disc_dict:
            chip_factory_update('discriminator', row['Discriminator'])
            chip_factory_update('iteration-count', row['Iteration Count'])
            chip_factory_update('salt', row['Salt'])
            chip_factory_update('verifier', row['Verifier'])

            if args.paa or args.pai:
                if args.dac_key is not None and args.dac_cert is not None:
                    dacs = use_dac_from_args(args)
                else:
                    dacs = generate_dac(int(row['Index']), args, int(row['Discriminator']),
                                        int(row['PIN Code']), PAI['key_pem'], PAI['cert_pem'])

                chip_factory_update('dac-cert', os.path.abspath(dacs[0]))
                chip_factory_update('dac-key', os.path.abspath(dacs[1]))
                chip_factory_update('dac-pub-key', os.path.abspath(dacs[2]))
                chip_factory_update('pai-cert', os.path.abspath(PAI['cert_der']))

            # If serial number is not passed, then generate one
            if (args.serial_num is None):
                chip_factory_update('serial-num', binascii.b2a_hex(os.urandom(SERIAL_NUMBER_LEN)).decode('utf-8'))

            if (args.enable_rotating_device_id is True) and (args.rd_id_uid is None):
                chip_factory_update('rd-id-uid', binascii.b2a_hex(os.urandom(int(ROTATING_DEVICE_ID_UNIQUE_ID_LEN_BITS / 8))).decode('utf-8'))

            if (args.csv is not None and args.mcsv is not None):
                overwrite_values_in_mcsv(args, int(row['Index']))

            mcsv_row_data = chip_get_values_as_csv()
            append_chip_mcsv_row(mcsv_row_data)

            # Generate onboarding data
            generate_onboarding_data(args, int(row['Index']), int(chip_factory_get_val('discriminator')), int(row['PIN Code']))


def organize_output_files(suffix, args):
    for i in range(len(UUIDs)):
        dest_path = os.sep.join([OUT_DIR['top'], UUIDs[i]])
        internal_path = os.sep.join([dest_path, 'internal'])

        replace = os.sep.join([OUT_DIR['top'], 'bin', '{}-{}.bin'.format(suffix, str(i + 1))])
        replace_with = os.sep.join([dest_path, '{}-partition.bin'.format(UUIDs[i])])
        os.rename(replace, replace_with)

        if args.encrypt:
            replace = os.sep.join([OUT_DIR['top'], 'keys', 'keys-{}-{}.bin'.format(suffix, str(i + 1))])
            replace_with = os.sep.join([dest_path, '{}-keys-partition.bin'.format(UUIDs[i])])
            os.rename(replace, replace_with)

        replace = os.sep.join([OUT_DIR['top'], 'csv', '{}-{}.csv'.format(suffix, str(i + 1))])
        replace_with = os.sep.join([internal_path, 'partition.csv'])
        os.rename(replace, replace_with)

        # Also copy the PAI certificate to the output directory
        if args.paa or args.pai:
            shutil.copy2(PAI['cert_der'], os.sep.join([internal_path, 'PAI_cert.der']))

        logging.info('Generated output files at: {}'.format(os.sep.join([OUT_DIR['top'], UUIDs[i]])))

    os.rmdir(os.sep.join([OUT_DIR['top'], 'bin']))
    os.rmdir(os.sep.join([OUT_DIR['top'], 'csv']))
    if args.encrypt:
        os.rmdir(os.sep.join([OUT_DIR['top'], 'keys']))

def generate_summary(args):
    master_csv = os.sep.join([OUT_DIR['stage'], 'master.csv'])
    summary_csv = os.sep.join([OUT_DIR['top'], 'summary-{}.csv'.format(datetime.now().strftime("%Y-%m-%d-%H:%M:%S"))])

    summary_csv_data = ''
    with open(master_csv, 'r') as mcsvf:
        summary_lines = mcsvf.read().splitlines()
        summary_csv_data += summary_lines[0] + ',pincode,qrcode,manualcode\n'
        with open(OUT_FILE['pin_disc_csv'], 'r') as pdcsvf:
            pin_disc_dict = csv.DictReader(pdcsvf)
            for row in pin_disc_dict:
                pincode = row['PIN Code']
                discriminator = row['Discriminator']
                qrcode = get_chip_qrcode(TOOLS['chip-tool'], args.vendor_id, args.product_id,
                                         args.commissioning_flow, discriminator, pincode, args.discovery_mode)
                manualcode = get_chip_manualcode(TOOLS['chip-tool'], args.vendor_id, args.product_id,
                                                 args.commissioning_flow, discriminator, pincode)
                summary_csv_data += summary_lines[1 + int(row['Index'])] + ',' + pincode + ',' + qrcode + ',' + manualcode + '\n'

    with open(summary_csv, 'w') as scsvf:
        scsvf.write(summary_csv_data)

def generate_partitions(suffix, size, encrypt):
    cmd = [
        'python3', TOOLS['mfg_gen'], 'generate',
        OUT_FILE['config_csv'], OUT_FILE['mcsv'],
        suffix, hex(size), '--outdir', OUT_DIR['top']
    ]

    if encrypt:
        cmd.append('--keygen')

    execute_cmd(cmd)


def generate_onboarding_data(args, index, discriminator, passcode):
    chip_manualcode = get_chip_manualcode(TOOLS['chip-tool'], args.vendor_id, args.product_id,
                                          args.commissioning_flow, discriminator, passcode)
    chip_qrcode = get_chip_qrcode(TOOLS['chip-tool'], args.vendor_id, args.product_id,
                                  args.commissioning_flow, discriminator, passcode, args.discovery_mode)

    logging.info('Generated QR code: ' + chip_qrcode)
    logging.info('Generated manual code: ' + chip_manualcode)

    csv_data = 'qrcode,manualcode,discriminator,passcode\n'
    csv_data += chip_qrcode + ',' + chip_manualcode + ',' + str(discriminator) + ',' + str(passcode) + '\n'

    onboarding_data_file = os.sep.join([OUT_DIR['top'], UUIDs[index], '{}-onb_codes.csv'.format(UUIDs[index])])
    with open(onboarding_data_file, 'w') as f:
        f.write(csv_data)

    # Create QR code image as mentioned in the spec
    qrcode_file = os.sep.join([OUT_DIR['top'], UUIDs[index], '{}-qrcode.png'.format(UUIDs[index])])
    chip_qr = pyqrcode.create(chip_qrcode, version=2, error='M')
    chip_qr.png(qrcode_file, scale=6)

    logging.info('Generated onboarding data and QR Code')


def get_args():
    def any_base_int(s): return int(s, 0)

    parser = argparse.ArgumentParser(description='Manufacuring partition generator tool',
                                     formatter_class=lambda prog: argparse.HelpFormatter(prog, max_help_position=50))

    g_gen = parser.add_argument_group('General options')
    g_gen.add_argument('-n', '--count', type=any_base_int, default=1,
                       help='The number of manufacturing partition binaries to generate. Default is 1. \
                              If --csv and --mcsv are present, the number of lines in the mcsv file is used.')
    g_gen.add_argument('-s', '--size', type=any_base_int, default=0x6000,
                       help='The size of manufacturing partition binaries to generate. Default is 0x6000.')
    g_gen.add_argument('-e', '--encrypt', action='store_true', required=False,
                      help='Encrypt the factory parititon NVS binary')

    g_commissioning = parser.add_argument_group('Commisioning options')
    g_commissioning.add_argument('--passcode', type=any_base_int,
                                 help='The passcode for pairing. Randomly generated if not specified.')
    g_commissioning.add_argument('--discriminator', type=any_base_int,
                                 help='The discriminator for pairing. Randomly generated if not specified.')
    g_commissioning.add_argument('-cf', '--commissioning-flow', type=any_base_int, default=0,
                                 help='Device commissioning flow, 0:Standard, 1:User-Intent, 2:Custom. \
                                          Default is 0.', choices=[0, 1, 2])
    g_commissioning.add_argument('-dm', '--discovery-mode', type=any_base_int, default=1,
                                 help='Commissionable device discovery netowrking technology. \
                                          0:WiFi-SoftAP, 1:BLE, 2:On-network. Default is BLE.', choices=[0, 1, 2])

    g_dac = parser.add_argument_group('Device attestation credential options')
    g_dac.add_argument('-cn', '--cn-prefix', default='ESP32',
                       help='The common name prefix of the subject of the generated certificate.')
    g_dac.add_argument('-lt', '--lifetime', default=4294967295, type=any_base_int,
                       help='Lifetime of the generated certificate. Default is 4294967295 if not specified, \
                              this indicate that certificate does not have well defined expiration date.')
    g_dac.add_argument('-vf', '--valid-from',
                       help='The start date for the certificate validity period in format <YYYY>-<MM>-<DD> [ <HH>:<MM>:<SS> ]. \
                              Default is current date.')
    # If DAC is present then PAI key is not required, so it is marked as not required here
    # but, if DAC is not present then PAI key is required and that case is validated in validate_args()
    g_dac.add_argument('-c', '--cert', help='The input certificate file in PEM format.')
    g_dac.add_argument('-k', '--key', help='The input key file in PEM format.')
    g_dac.add_argument('-cd', '--cert-dclrn', help='The certificate declaration file in DER format.')
    g_dac.add_argument('--dac-cert', help='The input DAC certificate file in PEM format.')
    g_dac.add_argument('--dac-key', help='The input DAC private key file in PEM format.')
    input_cert_group = g_dac.add_mutually_exclusive_group(required=False)
    input_cert_group.add_argument('--paa', action='store_true', help='Use input certificate as PAA certificate.')
    input_cert_group.add_argument('--pai', action='store_true', help='Use input certificate as PAI certificate.')

    g_dev_inst_info = parser.add_argument_group('Device instance information options')
    g_dev_inst_info.add_argument('-v', '--vendor-id', type=any_base_int, help='Vendor id')
    g_dev_inst_info.add_argument('--vendor-name', help='Vendor name')
    g_dev_inst_info.add_argument('-p', '--product-id', type=any_base_int, help='Product id')
    g_dev_inst_info.add_argument('--product-name', help='Product name')
    g_dev_inst_info.add_argument('--hw-ver', type=any_base_int, help='Hardware version')
    g_dev_inst_info.add_argument('--hw-ver-str', help='Hardware version string')
    g_dev_inst_info.add_argument('--mfg-date', help='Manufacturing date in format YYYY-MM-DD')
    g_dev_inst_info.add_argument('--serial-num', help='Serial number')
    g_dev_inst_info.add_argument('--enable-rotating-device-id', action='store_true', help='Enable Rotating device id in the generated binaries')
    g_dev_inst_info.add_argument('--rd-id-uid',
                        help='128-bit unique identifier for generating rotating device identifier, provide 32-byte hex string, e.g. "1234567890abcdef1234567890abcdef"')

    g_dev_inst = parser.add_argument_group('Device instance options')
    g_dev_inst.add_argument('--calendar-types', nargs='+',
                            help='List of supported calendar types. Supported Calendar Types: Buddhist, Chinese, Coptic, \
                                Ethiopian, Gregorian, Hebrew, Indian, Islamic, Japanese, Korean, Persian, Taiwanese')
    g_dev_inst.add_argument('--locales', nargs='+',
                            help='List of supported locales, Language Tag as defined by BCP47, eg. en-US en-GB')
    g_dev_inst.add_argument('--fixed-labels', nargs='+',
                            help='List of fixed labels, eg: "0/orientation/up" "1/orientation/down" "2/orientation/down"')

    g_basic = parser.add_argument_group('Few more Basic clusters options')
    g_basic.add_argument('--product-label', help='Product label')
    g_basic.add_argument('--product-url', help='Product URL')

    g_extra_info = parser.add_argument_group('Extra information options using csv files')
    g_extra_info.add_argument('--csv', help='CSV file containing the partition schema for extra options. \
            [REF: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/mass_mfg.html#csv-configuration-file]')
    g_extra_info.add_argument('--mcsv', help='Master CSV file containig optional/extra values specified by the user. \
            [REF: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/mass_mfg.html#master-value-csv-file]')

    return parser.parse_args()


def add_optional_KVs(args):
    # Device instance information
    if args.vendor_id is not None:
        chip_factory_append('vendor-id', 'data', 'u32', args.vendor_id)
    if args.vendor_name is not None:
        chip_factory_append('vendor-name', 'data', 'string', args.vendor_name)
    if args.product_id is not None:
        chip_factory_append('product-id', 'data', 'u32', args.product_id)
    if args.product_name is not None:
        chip_factory_append('product-name', 'data', 'string', args.product_name)
    if args.hw_ver is not None:
        chip_factory_append('hardware-ver', 'data', 'u32', args.hw_ver)
    if args.hw_ver_str is not None:
        chip_factory_append('hw-ver-str', 'data', 'string', args.hw_ver_str)
    if args.mfg_date is not None:
        chip_factory_append('mfg-date', 'data', 'string', args.mfg_date)
    if args.enable_rotating_device_id:
        chip_factory_append('rd-id-uid', 'data', 'hex2bin', args.rd_id_uid)

    # Add the serial-num
    chip_factory_append('serial-num', 'data', 'string', args.serial_num)

    # Add certificates and keys
    if args.paa or args.pai:
        chip_factory_append('dac-cert', 'file', 'binary', None)
        chip_factory_append('dac-key', 'file', 'binary', None)
        chip_factory_append('dac-pub-key', 'file', 'binary', None)
        chip_factory_append('pai-cert', 'file', 'binary', None)

    # Add certificate declaration
    if args.cert_dclrn:
        chip_factory_append('cert-dclrn','file','binary', os.path.relpath(args.cert_dclrn))

    # Add the Keys in csv files
    if args.csv is not None:
        chip_nvs_map_append_config_csv(args.csv)

    # Device information
    if args.calendar_types is not None:
        chip_factory_append('cal-types', 'data', 'u32', calendar_types_to_uint32(args.calendar_types))

    # Supported locale is stored as multiple entries, key format: "locale/<index>, example key: "locale/0"
    if (args.locales is not None):
        chip_factory_append('locale-sz', 'data', 'u32', len(args.locales))
        for i in range(len(args.locales)):
            chip_factory_append('locale/{:x}'.format(i), 'data', 'string', args.locales[i])

    # Each endpoint can contains the fixed lables
    #  - fl-sz/<index>     : number of fixed labels for the endpoint
    #  - fl-k/<ep>/<index> : fixed label key for the endpoint and index
    #  - fl-v/<ep>/<index> : fixed label value for the endpoint and index
    if (args.fixed_labels is not None):
        dict = get_fixed_label_dict(args.fixed_labels)
        for key in dict.keys():
            chip_factory_append('fl-sz/{:x}'.format(int(key)), 'data', 'u32', len(dict[key]))

            for i in range(len(dict[key])):
                entry = dict[key][i]
                chip_factory_append('fl-k/{:x}/{:x}'.format(int(key), i), 'data', 'string', list(entry.keys())[0])
                chip_factory_append('fl-v/{:x}/{:x}'.format(int(key), i), 'data', 'string', list(entry.values())[0])

    # Keys from basic clusters
    if args.product_label is not None:
        chip_factory_append('product-label', 'data', 'string', args.product_label)
    if args.product_url is not None:
        chip_factory_append('product-url', 'data', 'string', args.product_url)


def main():
    logging.basicConfig(format='[%(asctime)s] [%(levelname)7s] - %(message)s', level=logging.INFO)

    args = get_args()
    validate_args(args)
    check_tools_exists(args)
    setup_out_dirs(args.vendor_id, args.product_id, args.count)
    add_optional_KVs(args)
    generate_passcodes_and_discriminators(args)
    write_csv_files(args)
    if args.paa or args.pai:
        setup_root_certs(args)
    write_per_device_unique_data(args)
    generate_partitions('matter_partition', args.size, args.encrypt)
    organize_output_files('matter_partition', args)
    generate_summary(args)


if __name__ == "__main__":
    main()
