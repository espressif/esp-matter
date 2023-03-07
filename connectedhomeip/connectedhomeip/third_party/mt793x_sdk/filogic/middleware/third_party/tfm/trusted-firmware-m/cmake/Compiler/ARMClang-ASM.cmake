#-------------------------------------------------------------------------------
# Copyright (c) 2017, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

include(Compiler/ARMClang)
set(CMAKE_ASM_SOURCE_FILE_EXTENSIONS s;S;asm)
__compiler_armclang(ASM)
