#
#  Copyright (c) 2021, The OpenThread Authors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#

set(CMAKE_SYSTEM_NAME              Generic)
set(CMAKE_SYSTEM_PROCESSOR         ARM)
set(CMAKE_TRY_COMPILE_TARGET_TYPE  STATIC_LIBRARY)

file(GLOB TOOLS_DIR $ENV{HOME}/ModusToolbox/tools_*/gcc/bin)
if(TOOLS_DIR)
    set(TOOLS_DIR ${TOOLS_DIR}/)
endif()

set(CMAKE_C_COMPILER               ${TOOLS_DIR}arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER             ${TOOLS_DIR}arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER             ${TOOLS_DIR}arm-none-eabi-as)
set(CMAKE_RANLIB                   ${TOOLS_DIR}arm-none-eabi-ranlib)

execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE COMPILER_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)

set(COMMON_ARCH_FLAGS              "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")
set(COMMON_C_FLAGS                 "${COMMON_ARCH_FLAGS} -specs=nano.specs -fdata-sections -ffunction-sections -g3")

set(CMAKE_C_FLAGS_INIT             "${COMMON_C_FLAGS} -std=c11")
set(CMAKE_CXX_FLAGS_INIT           "${COMMON_C_FLAGS} -std=c++17")
set(CMAKE_EXE_LINKER_FLAGS_INIT    "${COMMON_ARCH_FLAGS}")

set(CMAKE_C_FLAGS_DEBUG            "-Os")
set(CMAKE_CXX_FLAGS_DEBUG          "-Os")
