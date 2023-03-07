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

'''Authority certificate generator.

Create an EC key pair and a certificate for either the Central or an 
Intermediate Authority of the given level.

Prerequisites:
Cryptography, jinja2 modules installed.
Higher level (n-1) certificate must be present. (Except for root certificate generation.)

Note:
This script overwrites the existing private key and certificate that belongs to 
the given level.
'''
# Metadata
__author__ = 'Silicon Laboratories, Inc'
__copyright__ = 'Copyright 2022, Silicon Laboratories, Inc.'

import os, pathlib
import stat
import datetime
import argparse
import cryptography
from cryptography import x509
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.primitives import serialization
from jinja2 import FileSystemLoader, Environment

def main(level,
         validity,
         subj_country,
         subj_organization,
         subj_organizational_unit,
         subj_state,
         subj_common_name,
         subj_locality,
         subj_email_address):

    # Check arguments
    if level < 0:
        raise Exception('Authority level must be greater than or equal to zero!')
    if validity < 1:
        raise Exception('Valid period must be greater than or equal to one day!')

    # For intermediate certificate creation, check the existance of the higher certificate.
    if level > 0:
        if level == 1:
            prev_name = 'central'
        else:
            prev_name = 'intermediate_' + str(level - 1)

        path_prev_dir = os.path.join(os.path.abspath(os.path.join((__file__), '..')), prev_name + '_authority')
        path_prev_key = os.path.join(path_prev_dir, 'private_key.pem')
        path_prev_cert = os.path.join(path_prev_dir, 'certificate.pem')

        if not os.path.exists(path_prev_key) or not os.path.exists(path_prev_cert):
            raise FileNotFoundError(prev_name + ' authority cannot be found at ' + path_prev_dir)
    
    # Paths
    if level == 0:
        name = 'central'
    else:
        name = 'intermediate_' + str(level)

    path_dir = os.path.join(os.path.abspath(os.path.join((__file__), '..')), name + '_authority')
    path_key = os.path.join(path_dir, 'private_key.pem')
    path_cert = os.path.join(path_dir, 'certificate.pem')
    path_template = os.path.join(os.path.abspath(os.path.join((__file__), '..')), 'sl_bt_cbap_root_cert.h.jinja')

    # Create directory
    if not os.path.exists(path_dir):
        os.makedirs(path_dir)

    # Remove output files
    if os.path.exists(path_key):
        print(path_key + ' already exists. Removing.')
        os.remove(path_key)
    
    if os.path.exists(path_cert):
        print(path_cert + ' already exists. Removing.')
        os.remove(path_cert)

    # Generate EC key pair. Use X9.62/SECG curve over a 256 bit prime field (aka prime256v1).
    key = ec.generate_private_key(ec.SECP256R1())

    # Write private key to file in PEM format.
    with open(path_key, 'wb') as f:
        f.write(key.private_bytes(
            serialization.Encoding.PEM,
            serialization.PrivateFormat.TraditionalOpenSSL,
            encryption_algorithm=serialization.NoEncryption()
        ))
    
    # Change file permissions
    os.chmod(path_key, stat.S_IREAD + stat.S_IWRITE)
    print(name + ' authority EC private key created.')

    # Pack certificate subjects
    subjects = x509.Name([
        x509.NameAttribute(x509.oid.NameOID.COUNTRY_NAME, subj_country),
        x509.NameAttribute(x509.oid.NameOID.ORGANIZATION_NAME, subj_organization),
        x509.NameAttribute(x509.oid.NameOID.ORGANIZATIONAL_UNIT_NAME, subj_organizational_unit),
        x509.NameAttribute(x509.oid.NameOID.STATE_OR_PROVINCE_NAME, subj_state),
        x509.NameAttribute(x509.oid.NameOID.COMMON_NAME, subj_common_name),
        x509.NameAttribute(x509.oid.NameOID.LOCALITY_NAME, subj_locality),
        x509.NameAttribute(x509.oid.NameOID.EMAIL_ADDRESS, subj_email_address),
    ])

    if level == 0:
        # Create root certificate. Self-signing.
        cert = create_certificate(key.public_key(), key, subjects, subjects, validity)
    else:
        # Create intermediate certificate.
        # Retrieve the private key and the certificate of the higher authority.
        with open(path_prev_key, 'rb') as f:
            root_key = serialization.load_pem_private_key(f.read(), password=None)
        
        with open(path_prev_cert, 'rb') as f:
            root_cert = x509.load_pem_x509_certificate(f.read())
        
        # Check the validity of the higher authority.
        # Note: if there is a revocation list, it should be also checked if the certificate is revoked or not.
        if datetime.datetime.utcnow() < root_cert.not_valid_before or root_cert.not_valid_after < datetime.datetime.utcnow():
            raise Exception('The validity period of ' + path_prev_cert + ' has expired.')

        # Sign with the key of the higher authority.
        cert = create_certificate(key.public_key(), root_key, subjects, root_cert.issuer, validity)
    
    # Write the certificate to file in PEM format.
    with open(path_cert, 'wb') as f:
        f.write(cert.public_bytes(serialization.Encoding.PEM))

    convert_header(path_cert, path_template) # Create header for SoC example
    print(name + ' authority certificate created.')

def create_certificate(public_key, signing_key, subjects, issuer, validity):
    ''' Create an x509 certificate object.

    Keyword arguments:
    public_key -- The public key of the certificate.
    signing_key -- The key to be used to sign the certificate with.
    subjects -- x509 certificate subjects.
    issuer -- The issuer of the certificate.
    validity -- Validity period in days starting from the moment of creation.
    Return values:
    cert -- The created certificate.
    '''
    # Create certificate
    cert = x509.CertificateBuilder().subject_name(
        subjects
    ).issuer_name(
        issuer
    ).public_key(
        public_key
    ).serial_number(
        x509.random_serial_number()
    ).not_valid_before(
        datetime.datetime.utcnow()
    ).not_valid_after(
        datetime.datetime.utcnow() + datetime.timedelta(validity)
    )

    # Add extensions
    cert = cert.add_extension(x509.SubjectKeyIdentifier.from_public_key(public_key), critical=False)
    cert = cert.add_extension(x509.BasicConstraints(ca=True, path_length=None), critical=True)

    # Sign certificate
    cert = cert.sign(signing_key, cryptography.hazmat.primitives.hashes.SHA256())
    return cert

def convert_header(path_pem, path_template):
    '''Create a header file out of a PEM certificate that can be used 
    directly by the Gecko SDK SoC sample applications.

    Keyword arguments:
    path_pem -- Path to the certificate file in PEM format.
    path_template -- Path to the jinja2 template directory.
    Return values:
    path_header -- Path to the certificate header file.
    '''
    # Check input
    if not os.path.exists(path_pem):
        raise FileNotFoundError('Cannot find certificate file.')

    # Load certificate
    with open(path_pem, 'r') as f:
        crt = f.readlines()
    
    # Remove PEM certificate delimiters
    crt.pop(0)
    crt.pop()

    # Format string
    i = 0
    while i < len(crt):
        if i < len(crt) - 1:
            crt[i] = crt[i] = '  "' + crt[i].strip() + '" \\\n'
        else:
            crt[i] = crt[i] = '  "' + crt[i].strip() + '"'
        i = i + 1

    crt = ''.join(crt)

    # Load jinja template
    env = Environment()
    env.loader = FileSystemLoader(os.path.normpath(os.path.join(path_template, '..').replace('\\', '/')))

    # Write to file
    path_header = os.path.join(os.path.abspath(os.path.join(path_pem, '..')),
                               pathlib.Path(path_template).stem)

    with open(path_header, 'w') as f:
        f.write(env.get_template(str(pathlib.Path(path_template).name)).render(sl_bt_cbap_root_cert = crt))
    
    print(str(path_header) + ' created.')
    return path_header

def load_args():
    '''Parse command line arguments'''
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument('-l', '--level',
                        default='0',
                        type=int,
                        help='Authority level. If zero passed, root certificate '\
                             'will be created. With greater values than zero, '\
                             'intermediate certificate will be created with '\
                             'the given level.')

    parser.add_argument('-v', '--validity',
                        default='365',
                        type=int,
                        help='The valid period of the certificate in days starting '\
                             'from the moment of generation.')
    
    parser.add_argument('-c', '--country',
                        default='FI',
                        type=str.upper,
                        help='The country subject of the x509 certificate.')

    parser.add_argument('-o', '--organization',
                        default='Silicon Laboratories',
                        type=str,
                        help='The organization subject of the x509 certificate.')

    parser.add_argument('-ou', '--organizationalUnit',
                        default='Wireless',
                        type=str,
                        help='The organizational unit subject of the x509 certificate.',
                        dest='organizational_unit')
    
    parser.add_argument('-st', '--state',
                        default='Uusimaa',
                        type=str,
                        help='The state subject of the x509 certificate.')

    parser.add_argument('-cn', '--commonName',
                        default='Silabs',
                        type=str,
                        help='The common name unit subject of the x509 certificate.',
                        dest='common_name')

    parser.add_argument('-lo', '--locality',
                        default='Espoo',
                        type=str,
                        help='The locality subject of the x509 certificate.')
    
    parser.add_argument('-e', '--emailAddress',
                        default='support@silabs.com',
                        type=str,
                        help='The e-mail address unit subject of the x509 certificate.',
                        dest='email_address')
    
    return parser.parse_args()

if __name__ == '__main__':
    args = load_args()

    main(args.level,
         args.validity,
         args.country,
         args.organization,
         args.organizational_unit,
         args.state,
         args.common_name,
         args.locality,
         args.email_address)
