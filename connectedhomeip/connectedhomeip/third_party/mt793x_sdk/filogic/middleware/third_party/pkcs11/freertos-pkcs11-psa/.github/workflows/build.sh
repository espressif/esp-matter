#!/bin/sh
#
# Build checked out freertos-pkcs11-psa submodule against amazon-freertos tree.
#
# Copyright (c) 2020 Linaro Limited
#
# Based on instructions:
# https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_cypress_psoc64.html
#
# This script assumes:
# 1. The gnuarmemb toolchain is already in the PATH.
# 2. A particular revision of the freertos-pkcs11-psa repo was cloned under
# "freertos-pkcs11-psa" subdirectory of the current directory.
#
# This script is supposed to be usable both as part of Github Workflow and
# for local testing/debugging, please keep it such.

set -ex

# Set to 1 to clone just minimal set of git submodules required for the
# build. Mostly useful for local testing (as submodule set changes over
# upstream revisions).
OPTIMIZE_SUBMODULES=0

SOURCE_REPO=https://github.com/aws/amazon-freertos
REPO_SUBDIR=amazon-freertos

git clone -q $SOURCE_REPO $REPO_SUBDIR

cd $REPO_SUBDIR

# Allow to override amazon-freertos's revision/branch to build against
# (e.g., to build against a particular amazon-freertos PR, if which case
# the file content should be "pull/NNN/head").
if [ -f ../freertos-pkcs11-psa/amazon-freertos.rev ]; then
    REV=$(cat ../freertos-pkcs11-psa/amazon-freertos.rev)
    git fetch origin $REV:$REV || true
    git checkout $REV
fi

if [ $OPTIMIZE_SUBMODULES -eq 0 ]; then
    git submodule update --recursive --init
else
    git submodule update --recursive --init \
    freertos_kernel \
    libraries/coreHTTP \
    libraries/coreJSON \
    libraries/coreMQTT \
    libraries/device_defender_for_aws \
    libraries/device_shadow_for_aws_iot_embedded_sdk \
    libraries/jobs_for_aws \
    libraries/3rdparty/lwip \
    libraries/3rdparty/mbedtls \
    libraries/3rdparty/pkcs11 \
    libraries/abstractions/pkcs11/corePKCS11
fi

# Remove freertos-pkcs11-psa submodule dir, as cloned from amazon-freertos.
rm -rf libraries/abstractions/pkcs11/psa

# Instead, graft pre-cloned CI submodule clone.
#mv ../freertos-pkcs11-psa libraries/abstractions/pkcs11/psa
ln -s $PWD/../freertos-pkcs11-psa libraries/abstractions/pkcs11/psa

ls -l libraries/abstractions/pkcs11
ls -l libraries/abstractions/pkcs11/psa/

# To build images to completion, we need firmware signing keys. The
# sample keys used to be in the repo, but were removed. As a quick
# workaround, just revert the removal commit. TODO: Perhaps, (pre)generate
# our own test keys.
# Revert doesn't go thru completely due to some submodule foo, but the
# needed files are put into the working tree, so we just ignore error.
git revert 287ed79eb6137443133d2a7200bc5591c02a8973 || true

if [ -d .venv ]; then
    . .venv/bin/activate
else
    python3 -m venv .venv
    . .venv/bin/activate
    python3 -m pip install cysecuretools
fi

which cysecuretools
cysecuretools version

cd projects/cypress/CY8CKIT_064S0S2_4343W/mtb/aws_demos
rm -rf build

cmake -DVENDOR=cypress -DBOARD=CY8CKIT_064S0S2_4343W -DCOMPILER=arm-gcc -DBUILD_CLONE_SUBMODULES=OFF \
    -S ../../../../.. -B build

cmake --build build

echo "=== Build results ==="
ls -l build/*.hex
echo "==="

# Dump submodule status after the build, to double-check that the build
# process didn't re-cloned upstream revision of freertos-pkcs11-psa
# (-DBUILD_CLONE_SUBMODULES=OFF should ensure that).
git submodule status
