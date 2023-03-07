********** Building Process **********
There are two ways to build the TestAL libraries:

1. USING build_config.sh

	1. Export KERNEL_DIR in case of mbedOS or FreeRTOS.
	2. Run the build_config.sh script, with or without a configuration number.
	3. The static libraries will be located in the project directory.

2. BUILDING MANUALLY
	1. Configure the toolchain.
	2. Run "make distclean".
	3. Run "make setconfig_testal_<OS>_<TARGET>"
	4. Run "make"
	5. The static libraries will be located in the project directory.

********** Cleaning Process **********
For cleaning object files, config file and static libraries:
	1. Run “make distclean".

For cleaning object files:
	1. Run “make clean".

For cleaning the current config file:
	1. Run "make clrconfig".

--------------

Copyright (c) 2001-2019, Arm Limited. All rights reserved.