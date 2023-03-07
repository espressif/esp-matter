#-------------------------------------------------------------------------------
# Copyright (c) 2001-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#!/bin/bash

# This script defines only the necessary environment variables for TestAL makefile.
# Note that it does not run our internal script set_toolchain.sh as it is
# supposed to be used by external users (e.g. costumers) or another projects
# makefiles, which usually configures their own environment.

choice=$1

SUPPORTED_CONFIGURATIONS="SUPPORTED CONFIGURATIONS
|----------------------------------------------------------------------|
|ConfNum|    OS    |    CPU     |      Toolchain      | Device | Board |
|----------------------------------------------------------------------|
|   1   |  linux   | cortex-a9  |      arm-xilinx     | target | Zynq  |
|   2   |          |            |     Deprecated      |        |       |
|   3   |  linux   |    x86     |        native       |  host  |       |
|   4   | freertos | cortex-m3  |    arm-compiler-5   | target | MPS2+ |
|   5   |          |            |     Deprecated      |        |       |
|   6   |  no_os   | cortex-m3  |    arm-compiler-5   | target | MPS2+ |
|   7   | freertos | cortex-m33 |     arm-none-eabi   | target | MPS2+ |
|   8   |  linux   | cortex-a9  |      arm-br-7.3     | target | Zynq  |
|   9   | freertos | cortex-m3  |     arm-none-eabi   | target | MPS2+ |
|   10  |  linux   |  a72 a53   |    aarch64-br-7.3   | target | Juno  |
|   11  | freertos | cortex-m33 |    arm-compiler-6   | target | MPS2+ |
|   12  | freertos | cortex-m3  |    arm-compiler-6   | target | MPS2+ |
|----------------------------------------------------------------------|

There is an option to run this script with ConfNum as an argument.

Please enter your choice: "

if [ -z $1 ]; then echo "$SUPPORTED_CONFIGURATIONS"; read choice; fi

make clean
make clrconfig

case $choice in
1)
#set_toolchain arm-xilinx-14.7
make setconfig_testal_linux_ca9
;;
2)
echo "Deprecated configuration!"
exit 0;
;;
3)
#set_toolchain native
make setconfig_testal_linux_x86
;;
4)
#set_toolchain arm-compiler-5
make setconfig_testal_freertos_cm3
if [ -z $KERNEL_DIR ]; then echo "Please enter freertos KERNEL_DIR path:"; read KERNEL_DIR;
export KERNEL_DIR; fi
;;
5)
echo "Deprecated configuration!"
exit 0;
;;
6)
#set_toolchain arm-compiler-5
make setconfig_testal_no_os_cm3
;;
7)
#set_toolchain arm-none-eabi
make setconfig_testal_freertos_cm33
if [ -z $KERNEL_DIR ]; then echo "Please enter freertos KERNEL_DIR path:"; read KERNEL_DIR;
export KERNEL_DIR; fi
;;
8)
#set_toolchain arm-br-7.3
make setconfig_testal_linux_ca9
;;
9)
#set_toolchain arm-none-eabi
make setconfig_testal_freertos_cm3
if [ -z $KERNEL_DIR ]; then echo "Please enter freertos KERNEL_DIR path:"; read KERNEL_DIR;
export KERNEL_DIR; fi
;;
10)
#set_toolchain aarch64-br-7.3
make setconfig_testal_linux_ca72.ca53
;;
11)
#set_toolchain arm-compiler-6
make setconfig_testal_freertos_cm33
;;
12)
#set_toolchain arm-compiler-6
make setconfig_testal_freertos_cm3
;;
*)
echo "Configuration number $1 is undefined"
exit 1;
;;
esac

echo

make
if [ $? -gt 0 ]; then
	status=1
	echo -e "\033[1;41m========\033[0m"
	echo -e "\033[1;41m= FAIL =\033[0m"
	echo -e "\033[1;41m========\033[0m"
	exit 1
fi
make clean
make clrconfig

echo
