# -----------------------------------------------------------------------------
# Copyright (c) 2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# -----------------------------------------------------------------------------

from setuptools import setup

setup(
    name='iatverifier',
    version='0.1',
    packages=[
        'iatverifier',
    ],
    scripts=[
        'scripts/check_iat',
        'scripts/compile_token',
        'scripts/decompile_token',
    ],
    python_requires='>=3.6',
    install_requires=[
        'cbor',
        'cryptography',
        'ecdsa',
        'pycose>=0.1.2',
        'pyyaml',
    ],
)
