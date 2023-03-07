#-------------------------------------------------------------------------------
# Copyright (c) 2017-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file gathers Cortex-M0p specific settings which control the build system.

#If Armv6-M architecture type is specified here, the compiler cmake files will
#set generic Armv6-M CPU type flags as building options directly, instead of the
#dedicated ones for Cortex-M0plus. It may break building on Cortex-M0plus. Thus
#skip the setting of architecture type and leave it to compiler cmake files.

set(ARM_CPU_TYPE "Cortex-M0p")
