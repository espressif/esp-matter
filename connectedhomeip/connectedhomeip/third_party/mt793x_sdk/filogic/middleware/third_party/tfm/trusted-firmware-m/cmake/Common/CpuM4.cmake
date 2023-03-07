#-------------------------------------------------------------------------------
# Copyright (c) 2017-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file gathers Cortex-M4 specific settings which control the build system.

#If Armv7-M architecture type is specified here, the compiler cmake files will
#set generic Armv7-M CPU type flags as building options directly, instead of the
#dedicated ones for Cortex-M4. It may break building on Cortex-M4. Thus skip the
#setting of architecture type and leave it to compiler cmake files.

set(ARM_CPU_TYPE "Cortex-M4")
