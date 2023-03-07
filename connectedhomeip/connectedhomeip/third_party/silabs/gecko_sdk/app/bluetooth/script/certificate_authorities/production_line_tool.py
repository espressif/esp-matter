#!/usr/bin/env python3

# Copyright 2022 Silicon Laboratories Inc. www.silabs.com
#
# SPDX-License-Identifier: Zlib
#
# The licensor of this software is Silicon Laboratories Inc.
#
# This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.

'''Certificate Authority

Handles signing Certificate Signing Requests and retrieving the Static 
Authentication Data from connected embedded devices.

Prerequisites:
A connected device with the prepared Certificate Signing Request.
Simplicity Commander added to path.
Cryptography module installed.
Certificate Authority created using the create_authority_certificate.py script.
'''
# Metadata
__author__ = 'Silicon Laboratories, Inc'
__copyright__ = 'Copyright 2022, Silicon Laboratories, Inc.'

import os, sys, pathlib
import datetime
import re
import binascii, struct
import argparse
import subprocess
from cryptography import x509
from cryptography.hazmat import primitives
from cryptography.hazmat.primitives import serialization

RAM_DATA_LEN_MAX        = 1024
STATIC_AUTH_DATA_LEN    = 32

NVM3_CONTROL_BLOCK_KEY = { 'ble': 0x40400, 'btmesh': 0x60400 }
NVM3_CONTROL_BLOCK_SIZE = 17
CHAIN_LINK_DATA_LEN     = 192

# Control Block bitmap
CERTIFICATE_ON_DEVICE_BIT       = 0
DEVICE_EC_KEY_BIT               = 1
STATIC_AUTH_DATA_BIT            = 2

def main(level, validity, serial, ip, protocol):
    # Check the presence of Simplicity Commander.
    try:
        subprocess.run(['commander', '-v'], stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT) # Supress output
    except FileNotFoundError:
        raise FileNotFoundError('Cannot find Simplicity Commander.')

    # Check arguments.
    if level < 0:
        raise Exception('Authority level should be greater than or equal to zero!')
    if validity < 1:
        raise Exception('Validity should be greater than or equal to 1 [days]!')
    if serial is not None and ip is not None:
        raise Exception('At most one of [J-Link serial number] or [IP address] shall be defined.')
    if protocol not in NVM3_CONTROL_BLOCK_KEY:
        raise Exception('Invalid protocol: ' + protocol + ' (choose from: ' + str(NVM3_CONTROL_BLOCK_KEY.keys()) + ')')
    
    # Check connected device.
    cmd = ('commander adapter probe ' + device_argument()).split()
    adapter_probe = str(subprocess_call(cmd).stdout)

    # Check chip family.
    substr_start = adapter_probe.lower().find('efr')
    substr_end = adapter_probe.find(' ', substr_start)
    family = adapter_probe[substr_start:substr_end]
    print(family, 'detected.\n\n')

    if 'xg22' in family.lower():
        ram_start = 0x20000000 # The CSR allocation is different for xG22 devices.
    else:
        ram_start = 0x20010000
    
    # Paths
    if level == 0:
        path_auth_dir = os.path.join(os.path.abspath(os.path.join((__file__), '..')), 'central_authority')
    else:
        path_auth_dir = os.path.join(os.path.abspath(os.path.join((__file__), '..')), 'intermediate_' + str(level) + '_authority')
    
    if not os.path.exists(path_auth_dir):
        raise FileNotFoundError(path_auth_dir + ' authority is incomplete.')
    
    # Retrieve RAM data.
    ram_data = read_ram(ram_start, RAM_DATA_LEN_MAX)

    # Retrieve Control Block from NVM3.
    path_nvm3_dump = dump_nvm3(path_auth_dir)
    control_block  = ControlBlock(path_nvm3_dump)

    if control_block.static_auth_present == True:
        # Get Static Authentication Data from RAM data.
        static_auth = get_static_auth(ram_data) # static_auth is ready for further processing.

    if control_block.device_ec_key_present == False:
        print('EC key pair is missing from the device. It is needed to generate a CSR. Exiting.')
        os.remove(path_nvm3_dump)
        sys.exit(0)

    # Get Certificate Signing Request from RAM data.
    path_csr = get_csr(ram_data, path_auth_dir)

    # Sign it and create the Device Certificate.
    path_cert_pem = sign_csr(path_auth_dir, path_csr, validity)
    os.remove(path_csr) # Remove CSR file since it is not needed anymore.

    # Exit if the device is not configured to hold the certificate.
    if control_block.certificate_on_device == False:
        print('The device is not configured to hold the certificate.')
        print(path_cert_pem, ' is ready for further processing.')
        os.remove(path_nvm3_dump)
        sys.exit(0)

    # Change the format of a certificate from PEM to DER.
    path_cert_der = convert_crt(path_cert_pem)

    # Add the signed certificate to the NVM3 image according to the Control Block.
    patch_nvm3(path_nvm3_dump,
               path_cert_der,
               control_block.nvm3_key[CERTIFICATE_ON_DEVICE_BIT],
               control_block.max_link_data_len)

    # Flash back the modified NVM3 content onto the device.
    flash_nvm3(path_nvm3_dump)

    # Clean up.
    os.remove(path_cert_der)
    os.remove(path_nvm3_dump)

def read_ram(start_address, memory_size):
    '''Get data from the RAM in the given range.

    Keyword arguments:
    start_address -- Address to start reading from.
    memory_size -- Memory size to read.
    Return values:
    The retrieved data from the RAM.
    '''
    cmd = ('commander readmem '
           + device_argument()
           + ' --range ' + hex(start_address) + ':+' + hex(memory_size)).split()
    output = subprocess_call(cmd).stdout

    # Grab raw bytes from the output.
    output = re.findall(r'^([0-9A-Fa-f]{8}):(\s--)*((\s[0-9A-Fa-f]{2}){1,16})(\s--)*',
                        output.decode('utf-8'),
                        re.MULTILINE)
    s = ''

    for item in output:
      s = s + item[2].replace(' ', '')

    return binascii.unhexlify(s)

def get_static_auth(ram_data):
    '''Static Authentication Data.

    Keyword arguments:
    ram_data -- Data retrieved from the RAM using read_ram
    Return values:
    static_auth -- Static Authentication Data.
    '''
    # Select static authentication data.
    static_auth = ram_data[1:STATIC_AUTH_DATA_LEN]

    print('Static Authentication Data has been successfully retrieved from the device.\n\n')
    return static_auth

def get_csr(ram_data, path_dir):
    '''Get Certificate Signing Request.

    Keyword arguments:
    ram_data -- Data retrieved from the RAM using read_ram
    path_dir -- Path to working directory where the CSR file shall be created.
    Return values:
    path_csr -- Path to the Certificate Signing Request file.
    '''
    # Select the certificate signing request.
    is_complete = ram_data[0] == 1 # Completion bit
    tuple = struct.unpack('<H', ram_data[STATIC_AUTH_DATA_LEN + 1 : STATIC_AUTH_DATA_LEN + 3])
    csr_len = tuple[0]

    if is_complete == False or csr_len == 0:
        raise Exception('Certificate signing request generation is incomplete!')

    csr_bytes = ram_data[STATIC_AUTH_DATA_LEN + 3 : STATIC_AUTH_DATA_LEN + 3 + csr_len]
    csr = x509.load_der_x509_csr(csr_bytes)

    # Write CSR to file.
    common_name = csr.subject.get_attributes_for_oid(x509.oid.NameOID.COMMON_NAME)[0].value
    path_csr = os.path.join(path_dir, common_name + '.csr')
    
    with open(path_csr, 'wb') as f:
        f.write(csr.public_bytes(serialization.Encoding.PEM))

    print('Certificate Signing Request has been successfully retrieved from the device.\n\n')
    return path_csr

def sign_csr(path_auth_dir, path_device_csr, validity):
    '''Create a device certificate by signing a Certificate Signing Request.
    
    Keyword arguments:
    path_auth_dir -- Path to the certificate authority that shall sign the request.
    path_device_csr -- The location of the certificate signing request file in 
                       PEM format.
    validity -- The valid period of the certificate in days starting
                from the moment of signing. (Default: 365)
    Return values:
    path_device_cert -- The location of the certificate file in PEM format.
    '''

    # Set paths and check prerequisites.
    path_auth_key = os.path.join(path_auth_dir, 'private_key.pem')
    path_auth_cert = os.path.join(path_auth_dir, 'certificate.pem')

    if not os.path.exists(path_auth_key) or not os.path.exists(path_auth_cert):
        raise FileNotFoundError(path_auth_dir + ' authority is incomplete.')

    path_device_csr = pathlib.Path(path_device_csr)

    if not os.path.exists(path_device_csr):
        raise FileNotFoundError('Certificate signing request cannot be found at ' + path_device_csr)

    path_device_cert = pathlib.Path(os.path.splitext(path_device_csr)[0] + '.crt')

    # Load certificate signing request
    with open(path_device_csr, 'rb') as f:
        csr = x509.load_pem_x509_csr(f.read())

    # Load private key and certificate of the authority.
    with open(path_auth_key, 'rb') as f:
        authority_key = primitives.serialization.load_pem_private_key(f.read(), password=None)
    
    with open(path_auth_cert, 'rb') as f:
        authority_cert = x509.load_pem_x509_certificate(f.read())

    # Check the validity of the higher authority.
    # Note: if there is a revocation list, it should be also checked if the certificate is revoked or not.
    if datetime.datetime.utcnow() < authority_cert.not_valid_before or authority_cert.not_valid_after < datetime.datetime.utcnow():
        raise Exception('The validity period of ' + path_auth_cert + ' has expired.')
    
    # Check if the CSR signature is valid
    if not csr.is_signature_valid:
        raise Exception('Invalid CSR signature!')
    
    # Create a new certificate from the CSR.
    cert = (
        x509.CertificateBuilder()
            .subject_name(csr.subject)
            .issuer_name(authority_cert.issuer)
            .public_key(csr.public_key())
            .serial_number(x509.random_serial_number())
            .not_valid_before(datetime.datetime.utcnow())
            .not_valid_after(datetime.datetime.utcnow() + datetime.timedelta(validity))
    )

    # Add requested extensions
    for ext in csr.extensions:
        cert = cert.add_extension(ext.value, ext.critical)
    
    # Sign certificate with the private key of the authority.
    cert = cert.sign(authority_key, csr.signature_hash_algorithm)

    # Verify signature.
    authority_cert.public_key().verify(cert.signature,
                                       cert.tbs_certificate_bytes,
                                       primitives.asymmetric.ec.ECDSA(csr.signature_hash_algorithm))
    
    # Write the signed certificate to file in PEM format.
    with open(path_device_cert, 'wb') as f:
        f.write(cert.public_bytes(primitives.serialization.Encoding.PEM))
    
    print('Signing completed.\n' + str(path_device_cert) + ' created.\n\n')
    return path_device_cert

def convert_crt(path_pem):
    '''Make a copy of a PEM certificate file in DER format.

    Keyword arguments:
    path_pem -- Path to the certificate file in PEM format.
    Return values:
    path_der -- Path to the formatted certificate file.
    '''
    # Check input
    if not os.path.exists(path_pem):
        raise FileNotFoundError('Cannot find certificate file.')

    # Load certificate
    with open(path_pem, 'rb') as f:
        crt = x509.load_pem_x509_certificate(f.read())
    
    # Write the certificate to file in DER format.
    path_der = pathlib.Path(os.path.splitext(path_pem)[0] + '.der')

    with open(path_der, 'wb') as f:
        f.write(crt.public_bytes(primitives.serialization.Encoding.DER))
    
    print('Certificate converted from PEM to DER successfully. ' + str(path_der) + ' created.')
    return path_der

def dump_nvm3(path_dir):
    '''Retrieve NVM3 content in s37 format.

    Keyword arguments:
    path_dir -- Path to working directory where the NVM3 image file shall be created.
    Return values:
    path_nvm3_dump -- Path to the NVM3 image file.
    '''
    path_nvm3_dump = os.path.join(path_dir, 'nvm3_dump.s37')

    # Remove temp file if exist
    if os.path.exists(path_nvm3_dump):
        print(path_nvm3_dump + ' already exists. Removing.\n')
        os.remove(path_nvm3_dump)
    
    # Search for the NVM3 area in the device's flash and dump the content to file.
    subprocess_call(('commander nvm3 read ' + device_argument() + ' -o ' + path_nvm3_dump).split())
    print('NVM3 dumped to ' + str(path_nvm3_dump))
    return path_nvm3_dump

def patch_nvm3(path_nvm3_dump, path_data, nvm3_start_key, nvm3_object_size):
    '''Create an NVM3 patch file containing the given data and apply on the NVM3 image.

    Keyword arguments:
    path_nvm3_dump -- Path to the NVM3 image.
    path_data -- A binary file. The data of this file will be added to the NVM3 image.
    nvm3_start_key -- NVM3 objects will be created or overwritten starting from this key.
    nvm3_object_size -- The size of each NVM3 object data.
    '''

    path_nvm3_patch = str(pathlib.Path(os.path.splitext(path_nvm3_dump)[0] + '.patch'))
    nvm3_key = nvm3_start_key

    with open(path_data, 'rb') as f:
        crt = f.read()

    # Create patch file
    with open(path_nvm3_patch, 'w') as f:
        while nvm3_key != 0:
            # Select next chunk.
            if len(crt) >= nvm3_object_size:
                length = nvm3_object_size
            else:
                length = len(crt)

            chunk = crt[:length]
            crt = crt[length:]

            if len(crt) > 0:
                next_nvm3_key = nvm3_key + 1
                header = 0x0001 | 2 | next_nvm3_key << 2
            else: # The last chunk is being processed.
                next_nvm3_key = 0
                header = 0x0001
            
            # Construct next line and write to file.
            key = NVM3_CONTROL_BLOCK_KEY[args.protocol] | nvm3_key # Add key prefix.
            data = struct.pack(f'<HHH', header, 0x0001, length) + chunk # Pack data chunk.
            f.write(hex(key) + ' : OBJ : ' + binascii.hexlify(data).decode('utf-8') + '\n') # <key>:<type>:<data>

            nvm3_key = next_nvm3_key

    # Apply patch
    subprocess_call(['commander', 'nvm3', 'set', path_nvm3_dump, '--nvm3file', path_nvm3_patch, '--outfile', path_nvm3_dump])
    print(str(path_nvm3_dump) + ' patched successfully.')
    os.remove(path_nvm3_patch)

def flash_nvm3(path_nvm3_dump):
    '''Flash NVM3 content.

    Keyword arguments:
    path_nvm3_dump -- Path to the NVM3 image.
    '''
    subprocess_call(('commander flash ' + path_nvm3_dump + ' ' + device_argument()).split())
    print('The device is updated with the new Flash image.')

class ControlBlock():
    def __init__(self, path_nvm3_dump):
        '''Get Control Block from an NVM3 image file.

        Keyword arguments:
        path_nvm3_dump -- Path to the NVM3 image.
        Return values:
        ControlBlock -- The retrieved and parsed Control Block.
        '''
        nvm3_object = self.get_nvm3_object(path_nvm3_dump)
        self.parse_data(nvm3_object)
        self.validate()
    
    def get_nvm3_object(self, path_nvm3_dump):
        # Read NVM3 object from the NVM3 image that belongs to the control block key.
        cmd = ('commander nvm3 parse ' + path_nvm3_dump + ' --key ' + str(NVM3_CONTROL_BLOCK_KEY[args.protocol])).split()
        nvm3_object = subprocess_call(cmd).stdout.decode('utf-8')

        # Check Control Block key.
        key = re.findall(r'^Key\s+:\s+(0x\d+).*$', nvm3_object, re.MULTILINE)

        if len(re.findall(r'^Found NVM3 range:', nvm3_object, re.MULTILINE)) == 0 or len(key) == 0:
            raise Exception('NVM3 range cannot be found.')

        self.key = key[0]
        print('Control Block found at ' + self.key)
        return nvm3_object
    
    def parse_data(self, nvm3_object):
        # Get raw bytes of the Control Block.
        data = ''

        for item in re.findall(r'^([0-9A-Fa-f]{8}):((\s[0-9A-Fa-f]{2}){1,16})', nvm3_object, re.MULTILINE):
            data = data + item[1].replace(' ', '')

        data = binascii.unhexlify(data)

        if data == None or len(data) != NVM3_CONTROL_BLOCK_SIZE:
            raise Exception('Invalid Control Block!')

        # Unpack the data of the NVM3 object.
        self.nvm3_key = [0, 0, 0]
        (self.header,
         self.next_control_block,
         bitmap,
         self.nvm3_key[0], self.nvm3_key[1], self.nvm3_key[2],
         self.max_link_data_len) = struct.unpack('<HHQbbbH', data)
        self.version = self.header & 0xf000 # Upper 4 bits of the header contain version number. Other bits are reserved for future use.
        self.next_control_block |= NVM3_CONTROL_BLOCK_KEY[args.protocol] & 0xF0000 # Only stored on two bytes. Adding prefix.

        # Inspect bitmap.
        print('Bitmap: ', bin(bitmap))

        if bitmap & (1 << CERTIFICATE_ON_DEVICE_BIT):
            self.certificate_on_device = True
            print('\tCertificate on device is required. NVM3 key: '
                  + hex(self.nvm3_key[CERTIFICATE_ON_DEVICE_BIT]))
        else:
            self.certificate_on_device = False
            print('\tCertificate on device is not required.')
        
        if bitmap & (1 << DEVICE_EC_KEY_BIT):
            self.device_ec_key_present = True
            print('\tEC key pair is present.')
        else:
            self.device_ec_key_present = False
            print('\tEC key pair is missing.')

        if bitmap & (1 << STATIC_AUTH_DATA_BIT):
            self.static_auth_present = True
            print('\tStatic Authentication data is present.')
        else:
            self.static_auth_present = False
            print('\tStatic Authentication data is not present.')
        
        print('Maximum link data length: ', self.max_link_data_len)

    def validate(self):
        # Expecting zero as the version number.
        if self.version != 0:
            raise Exception('Invalid Control Block! Version: ', self.version, '. Zero is expected.')
        else:
            print('Header: ', hex(self.header), '. Version: ', self.version)
        
        # Expecting only one control block. Therefore it should point onto itself.
        if self.next_control_block != NVM3_CONTROL_BLOCK_KEY[args.protocol]:
            raise Exception('Invalid Control Block! Next Control Block NVM3 key: ',
                            hex(self.next_control_block), '. ',
                            hex(NVM3_CONTROL_BLOCK_KEY[args.protocol]), ' is expected.')
        else:
            print('Next Control Block: ', hex(self.next_control_block), '\n\n')
        
        # Check the security of NVM3 ITS data.
        if self.device_ec_key_present and self.nvm3_key[DEVICE_EC_KEY_BIT] != 0:
            raise Exception('EC key pair is exposed. Any data stored in ITS should not be accessible!')

        if self.static_auth_present and self.nvm3_key[STATIC_AUTH_DATA_BIT] != 0:
            raise Exception('Static Authentication data is exposed. Any data stored in ITS should not be accessible!')

def subprocess_call(command):
    '''Handle subprocess calls.

    Keyword arguments:
    command -- The CLI command to be called.
    Return values:
    process -- The CompletedProcess instance of the subprocess.
    '''
    try:
        process = subprocess.run(command,
                                 check=True,
                                 stdout=subprocess.PIPE,
                                 stderr=subprocess.STDOUT) # Merge stderr into stdout.
    except subprocess.CalledProcessError as e:
        raise Exception('[E: ', e.returncode, '] ', e.stdout)
    except:
        raise Exception('[E: ', e.returncode, '] ', e.stdout)
    return process

def device_argument():
    '''Returns the device argument of a Simplicity Commander command.
    '''
    if args.serial is not None and args.ip is not None:
        raise Exception('At most one of [J-Link serial number] or [IP address] shall be defined.')
    elif args.serial is not None:
        return '--serialno ' + args.serial
    elif args.ip is not None:
        return '--ip ' + args.ip
    else:
        return ''

class CustomFormatter(argparse.ArgumentDefaultsHelpFormatter, argparse.RawDescriptionHelpFormatter):
    pass

def load_args():
    '''Parse command line arguments.'''
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=CustomFormatter)

    parser.epilog = (
        'examples:\n'
        '  %(prog)s                     Try to autodetect device.\n'
        '  %(prog)s --serial 440192051  Connect to device with the given J-Link serial.\n'
        '  %(prog)s --ip 192.168.0.143  Connect to device with the given IP address.')

    parser.add_argument('-l', '--level',
                        default='0',
                        type=int,
                        help='The level authority that shall sign the request.')
    parser.add_argument('-v', '--validity',
                        default='365',
                        type=int,
                        help='The valid period of the certificate in days starting '\
                             'from the moment of signing.')
    parser.add_argument('-s', '--serial',
                        type=str,
                        help='J-Link serial number. Should not be given together with the IP address.')
    parser.add_argument('-i', '--ip',
                        type=str,
                        help='IP Address. Should not be given together with the J-Link serial number.')
    parser.add_argument('-p', '--protocol',
                        default='btmesh',
                        type=str.lower,
                        choices=['ble', 'btmesh'],
                        help='Determines which NVM3 region to use.')

    args = parser.parse_args()
    return args

if __name__ == '__main__':
    args = load_args()
    main(args.level, args.validity, args.serial, args.ip, args.protocol)
