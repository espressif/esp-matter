#
#  Copyright (c) 2020, The OpenThread Authors.
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

cmake_minimum_required(VERSION 3.10.2)
project(ot-qorvo VERSION 0.1.0)

set(QORVO_PLATFORM_VALUES
    "gp712"
    "qpg6095"
    "qpg6100"
    "qpg6105"
    "qpg7015m"
)
set_property(CACHE QORVO_PLATFORM PROPERTY STRINGS ${QORVO_PLATFORM_VALUES})
if(NOT QORVO_PLATFORM IN_LIST QORVO_PLATFORM_VALUES)
    message(FATAL_ERROR "Please select a supported platform: ${QORVO_PLATFORM_VALUES}")
endif()

set(OT_PLATFORM_LIB_FTD "openthread-${QORVO_PLATFORM}-ftd")
set(OT_PLATFORM_LIB_MTD "openthread-${QORVO_PLATFORM}-mtd")
set(OT_PLATFORM_LIB_RCP "openthread-${QORVO_PLATFORM}-ftd")
set(OT_PLATFORM_LIB "openthread-${QORVO_PLATFORM}-ftd")


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

if (DEFINED ENV{QORVO_OT_SDK})
    SET(SDK_DIR "$ENV{QORVO_OT_SDK}")
else()
    SET(SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/Qorvo/repo")
endif()

include("${PROJECT_SOURCE_DIR}/etc/options.cmake")

set(OPENTHREAD_DIR ${PROJECT_SOURCE_DIR}/openthread)
add_subdirectory(${OPENTHREAD_DIR} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

target_compile_definitions(ot-config INTERFACE
    OPENTHREAD_CONFIG_FILE="openthread-core-${QORVO_PLATFORM}-config.h"
    OPENTHREAD_PROJECT_CORE_CONFIG_FILE="openthread-core-${QORVO_PLATFORM}-config.h"
    OPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE="openthread-core-${QORVO_PLATFORM}-config-check.h"
)

target_include_directories(ot-config INTERFACE
    ${PROJECT_SOURCE_DIR}/src/${QORVO_PLATFORM}
)

add_subdirectory(src/${QORVO_PLATFORM})
add_subdirectory(third_party/Qorvo)
