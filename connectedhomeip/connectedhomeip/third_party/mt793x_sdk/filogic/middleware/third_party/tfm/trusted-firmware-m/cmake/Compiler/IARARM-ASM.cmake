#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

include(Compiler/IARARM)
set(CMAKE_ASM_SOURCE_FILE_EXTENSIONS s;S;asm)
__compiler_iararm(ASM)

