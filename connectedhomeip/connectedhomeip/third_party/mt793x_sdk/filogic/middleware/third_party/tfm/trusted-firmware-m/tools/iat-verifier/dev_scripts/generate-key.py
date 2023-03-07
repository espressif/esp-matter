#!/usr/bin/env python3
# -----------------------------------------------------------------------------
# Copyright (c) 2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# -----------------------------------------------------------------------------

import sys

from ecdsa import SigningKey, NIST256p


if __name__ == '__main__':
    outfile = sys.argv[1]

    sk = SigningKey.generate(curve=NIST256p)
    with open(outfile, 'wb') as wfh:
        wfh.write(sk.to_pem())
