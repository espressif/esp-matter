# -----------------------------------------------------------------------------
# Copyright (c) 2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# -----------------------------------------------------------------------------

import argparse
import base64
import json
import logging
import struct
import sys

import cbor
from ecdsa import SigningKey
from pycose.sign1message import Sign1Message

from iatverifier import const
from iatverifier.util import extract_iat_from_cose, recursive_bytes_to_strings


logging.basicConfig(level=logging.INFO, format='%(levelname)8s: %(message)s')
logger = logging.getLogger('iat-verify')

seen_errors = False


def error(message, keep_going=False):
    global seen_errors
    seen_errors = True
    if keep_going:
        logger.error(message)
    else:
        raise ValueError(message)


def decode(value, key, keep_going=False):
    if key in const.IS_UTF_8:
        try:
            return value.decode()
        except UnicodeDecodeError as e:
            msg = 'Error decodeing value for "{}": {}'
            error(msg.format(key, e), keep_going)
            return str(value)[2:-1]
    else:  # not a UTF-8 value, i.e. a bytestring
        return value


# ----------------------------------------------------------------------------
# Validation functions
#
def validate_instance_id(value, keep_going=False, strict=False):
    _validate_bytestring_length(value, 'INSTANCE_ID', 33, keep_going)
    if value[0] != 0x01:
        msg = 'Invalid INSTANCE_ID: first byte must be 0x01, found: 0x{}'
        error(msg.format(value[0]), keep_going)


def validate_challege(value, keep_going=False, strict=False):
    if not isinstance(value, bytes):
        msg = 'Invalid CHALLENGE; must be a bytes string.'
        error(msg, keep_going)

    value_len = len(value)
    if value_len not in const.HASH_SIZES:
        msg = 'Invalid CHALLENGE length; must one of {}, found {} bytes'
        error(msg.format(const.HASH_SIZES, value_len), keep_going)


def validate_implementation_id(value, keep_going=False, strict=False):
    pass


def validate_hardware_id(value, keep_going=False, strict=False):
    pass


def validate_originator(value, keep_going=False, strict=False):
    pass


def validate_sw_components(value, keep_going=False, strict=False):
    if not isinstance(value, list):
        msg = 'Invalid SW_COMPONENTS value (must be an array): {}'
        error(msg.format(value), keep_going)
        return

    for sw_component in value:
        if not isinstance(sw_component, dict):
            msg = 'Invalid SW_COMPONENTS array entry (must be a map): {}'
            error(msg.format(sw_component), keep_going)
            return

        for k, v in sw_component.items():
            if k not in const.ALLOWED_SW_COMPONENT_CLAIMS:
                if strict:
                    msg = 'Unexpected SW_COMPONENT claim: {}'
                    error(msg.format(k), keep_going)
                else:
                    continue
            try:
                validation_funcs[k](v, keep_going)
            except Exception:
                if not keep_going:
                    raise


def validate_sw_component_type(value, keep_going=False, strict=False):
    pass


def validate_no_measurements(vlaue, keep_going=False, strict=False):
    pass


def validate_client_id(value, keep_going=False, strict=False):
    if not isinstance(value, int):
        msg = 'Invalid CLIENT_ID, must be an int: {}'
        error(msg.format(value), keep_going)


def validate_security_lifecycle(value, keep_going=False, strict=False):
    if not isinstance(value, int):
        msg = 'Invalid SECURITY_LIFECYCLE, must be an int: {}'
        error(msg.format(value), keep_going)


def validate_profile_id(value, keep_going=False, strict=False):
    if not isinstance(value, str):
        msg = 'Invalid PROFILE_ID (must be a string): {}'.format(value)
        error(msg.format(value), keep_going)


def validate_boot_seed(value, keep_going=False, strict=False):
    _validate_bytestring_length(value, 'BOOT_SEED', 32, keep_going)


def validate_signer_id(value, keep_going=False, strict=False):
    _validate_bytestring_length(value, 'SIGNER_ID', 32, keep_going)


def validate_sw_component_version(value, keep_going=False, strict=False):
    pass


def validate_measurement_value(value, keep_going=False, strict=False):
    _validate_bytestring_length(value, 'MEASUREMENT', 32, keep_going)


def validate_measurement_description(value, keep_going=False, strict=False):
    pass


def validate_challenge(value, keep_going=False, strict=False):
    pass


validation_funcs = {v: globals().get('validate_{}'.format(n.lower()))
                    for v, n in const.NAMES.items()}


def validate_manadatory_claims(token, keep_going=False):
    for mand_claim in const.MANDATORY_CLAIMS:
        if mand_claim not in token:
            msg = 'Invalid IAT: missing MANDATORY claim "{}"'
            error(msg.format(const.NAMES[mand_claim]), keep_going)

    if const.SW_COMPONENTS in token:
        if (not token[const.SW_COMPONENTS] and
                const.NO_MEASUREMENTS not in token):
            error('Invalid IAT: no software measurements defined and '
                  'NO_MEASUREMENTS claim is not present.')

        for entry_number, sw_component_entry in \
                enumerate(token[const.SW_COMPONENTS]):
            for mand_claim in const.MANDATORY_SW_COMPONENT_CLAIMS:
                if mand_claim not in sw_component_entry:
                    msg = ('Invalid IAT: missing MANDATORY claim "{}" '
                           'from sw_componentule at index {}')
                    error(msg.format(const.NAMES[mand_claim],
                                     entry_number),
                          keep_going)

    elif const.NO_MEASUREMENTS not in token:
        error('Invalid IAT: no software measurements defined and '
              'NO_MEASUREMENTS claim is not present.')


def _validate_bytestring_length(value, name, expected_len, keep_going=False):
    if not isinstance(value, bytes):
        msg = 'Invalid {}: must be a bytes string: found {}'
        error(msg.format(name, type(value)), keep_going)

    value_len = len(value)
    if value_len != expected_len:
        msg = 'Invalid {} length: must be exactly {} bytes, found {} bytes'
        error(msg.format(name, expected_len, value_len), keep_going)
# ----------------------------------------------------------------------------


def decode_sw_component(raw_sw_component, keep_going=True, strict=False):
    sw_component = {}
    for k, v in raw_sw_component.items():
        if isinstance(v, bytes):
            v = decode(v, k, keep_going)
        try:
            sw_component[const.NAMES[k]] = v
        except KeyError:
            if strict:
                if not keep_going:
                    raise
            else:
                sw_component[k] = v
    return sw_component


def decode_and_validate_iat(encoded_iat, keep_going=False, strict=False):
    try:
        raw_token = cbor.loads(encoded_iat)
    except Exception as e:
        msg = 'Invalid CBOR: {}'
        raise ValueError(msg.format(e))

    validate_manadatory_claims(raw_token, keep_going)

    token = {}
    for entry in raw_token.keys():
        try:
            entry_name = const.NAMES[entry]
        except KeyError:
            if strict:
                error('Invalid IAT claim: {}'.format(entry), keep_going)
            if isinstance(value, bytes):
                value = decode(value, entry, keep_going)
            token[entry] = value
            continue

        value = raw_token[entry]
        validation_funcs[entry](value, keep_going, strict)
        if entry_name == 'SW_COMPONENTS':
            try:
                token[entry_name] = []
                for raw_sw_component in value:
                    decoded_component = decode_sw_component(raw_sw_component,
                                                            keep_going)
                    token[entry_name].append(decoded_component)
            except TypeError:
                error('Invalid SW_COMPONENT value: {}'.format(value),
                      keep_going)
        elif entry_name == 'SECURITY_LIFECYCLE':
            try:
                name_idx = (value >> const.SL_SHIFT) - 1
                token[entry_name] = const.SL_NAMES[name_idx]
            except IndexError:
                token[entry_name] = 'CUSTOM({})'.format(value)
        else:  # not SW_COMPONENT or SECURITY_LIFECYCLE
            if isinstance(value, bytes):
                value = decode(value, entry_name, keep_going)
            token[entry_name] = value

    return token


def main():
    parser = argparse.ArgumentParser(
        description='''
        Validates a signed Initial Attestation Token (IAT), checking
        that the signature is valid, the token contian the required
        fields, and those fields are in a valid format.
        ''')
    parser.add_argument('-k', '--keyfile',
                        help='''
                        Path to a file containing signing key in PEM format.
                         ''')
    parser.add_argument('tokenfile',
                        help='''
                        path to a file containing a signed IAT.
                        ''')
    parser.add_argument('-K', '--keep-going', action='store_true',
                        help='''
                        Do not stop upon encountering a validation error.
                        ''')
    parser.add_argument('-p', '--print-iat', action='store_true',
                        help='''
                        Print the decoded token in JSON format.
                        ''')
    parser.add_argument('-s', '--strict', action='store_true',
                        help='''
                        Report failure if unknown claim is encountered.
                        ''')
    args = parser.parse_args()

    logging.basicConfig(level=logging.INFO)

    try:
        raw_iat = extract_iat_from_cose(args.keyfile, args.tokenfile,
                                        args.keep_going)
        if args.keyfile and not seen_errors:
            print('Signature OK')
    except ValueError as e:
        logger.error('Could not extract IAT from COSE:\n\t{}'.format(e))
        sys.exit(1)

    try:
        token = decode_and_validate_iat(raw_iat, args.keep_going, args.strict)
        if not seen_errors:
            print('Token format OK')
    except ValueError as e:
        logger.error('Could not validate IAT:\n\t{}'.format(e))
        sys.exit(1)

    if args.print_iat:
        print('Token:')
        json.dump(recursive_bytes_to_strings(token, in_place=True),
                  sys.stdout, indent=4)
        print('')
