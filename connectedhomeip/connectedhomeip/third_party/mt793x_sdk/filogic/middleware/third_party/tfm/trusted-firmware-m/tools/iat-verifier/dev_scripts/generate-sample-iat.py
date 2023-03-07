#!/usr/bin/env python3
# -----------------------------------------------------------------------------
# Copyright (c) 2019-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# -----------------------------------------------------------------------------

import base64
import struct

import cbor
from ecdsa import SigningKey
from pycose.sign1message import Sign1Message

from iatverifier import const
from iatverifier.util import sign_eat


# First byte indicates "GUID"
GUID = b'\x01' + struct.pack('QQQQ', 0x0001020304050607, 0x08090A0B0C0D0E0F,
                             0x1011121314151617, 0x18191A1B1C1D1E1F)
NONCE = struct.pack('QQQQ', 0X0001020304050607, 0X08090A0B0C0D0E0F,
                    0X1011121314151617, 0X18191A1B1C1D1E1F)
ORIGIN = struct.pack('QQQQ', 0X0001020304050607, 0X08090A0B0C0D0E0F,
                     0X1011121314151617, 0X18191A1B1C1D1E1F)
BOOT_SEED = struct.pack('QQQQ', 0X0001020304050607, 0X08090A0B0C0D0E0F,
                        0X1011121314151617, 0X18191A1B1C1D1E1F)
SIGNER_ID = struct.pack('QQQQ', 0X0001020304050607, 0X08090A0B0C0D0E0F,
                        0X1011121314151617, 0X18191A1B1C1D1E1F)
MEASUREMENT = struct.pack('QQQQ', 0X0001020304050607, 0X08090A0B0C0D0E0F,
                          0X1011121314151617, 0X18191A1B1C1D1E1F)

token_map = {
  const.INSTANCE_ID: GUID,
  const.IMPLEMENTATION_ID: ORIGIN,
  const.CHALLENGE: NONCE,
  const.CLIENT_ID: 2,
  const.SECURITY_LIFECYCLE: const.SL_SECURED,
  const.PROFILE_ID: 'http://example.com',
  const.BOOT_SEED: BOOT_SEED,
  const.SW_COMPONENTS: [
        {
            # bootloader
            const.SW_COMPONENT_TYPE: 'BL',
            const.SIGNER_ID: SIGNER_ID,
            const.SW_COMPONENT_VERSION: '3.4.2',
            const.MEASUREMENT_VALUE: MEASUREMENT,
            const.MEASUREMENT_DESCRIPTION: 'TF-M_SHA256MemPreXIP',
        },
        {
            # mod1
            const.SW_COMPONENT_TYPE: 'M1',
            const.SIGNER_ID: SIGNER_ID,
            const.SW_COMPONENT_VERSION: '3.4.2',
            const.MEASUREMENT_VALUE: MEASUREMENT,
        },
        {
            # mod2
            const.SW_COMPONENT_TYPE: 'M2',
            const.SIGNER_ID: SIGNER_ID,
            const.SW_COMPONENT_VERSION: '3.4.2',
            const.MEASUREMENT_VALUE: MEASUREMENT,
        },
        {
            # mod3
            const.SW_COMPONENT_TYPE: 'M3',
            const.SIGNER_ID: SIGNER_ID,
            const.SW_COMPONENT_VERSION: '3.4.2',
            const.MEASUREMENT_VALUE: MEASUREMENT,
        },
    ],
}


if __name__ == '__main__':
    import sys
    if len(sys.argv) != 3:
        print('Usage: {} KEYFILE OUTFILE'.format(sys.argv[0]))
        sys.exit(1)
    keyfile = sys.argv[1]
    outfile = sys.argv[2]

    sk = SigningKey.from_pem(open(keyfile, 'rb').read())
    token = cbor.dumps(token_map)
    signed_token = sign_eat(token, sk)

    with open(outfile, 'wb') as wfh:
        wfh.write(signed_token)
