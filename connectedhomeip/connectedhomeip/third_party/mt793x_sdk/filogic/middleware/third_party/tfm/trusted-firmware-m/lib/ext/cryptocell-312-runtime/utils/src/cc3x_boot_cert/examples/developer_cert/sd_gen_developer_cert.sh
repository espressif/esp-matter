#-------------------------------------------------------------------------------
# Copyright (c) 2001-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#!/bin/bash

cd ../../../../bin/

cp ../tester/cc3x_secure_debug_test/dat/dev_keypair1.pem .
cp ../tester/cc3x_secure_debug_test/dat/dev_pwd1.txt .
cp ../tester/cc3x_secure_debug_test/dat/oem_keypair1.pem .
cp ../tester/cc3x_secure_debug_test/dat/oem_pwd1.txt .
cp ../tester/cc3x_secure_debug_test/dat/root_key1.txt .
cp ../src/cc3x_boot_cert/examples/developer_cert/sb_developer_dbg_cert.cfg .


echo
echo " Gen: developer SOC ID... "
echo "================================================"
./get_pub_key.sh oem_keypair1.pem oem_pwd1.txt oem_pubkey1.pem
./gen_hbk_app -h hbk1.bin -p oem_pubkey1.pem
./gen_socid_app -h hbk1.bin -r root_key1.txt -s soc_id1.bin

echo
echo " Run: generate developer certificate "
echo "================================================"
./cert_dbg_developer_util.py sb_developer_dbg_cert.cfg


