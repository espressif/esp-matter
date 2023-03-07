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

cmake_minimum_required(VERSION 3.10.2)
project(ot-nxp VERSION 0.2.0)

include("${PROJECT_SOURCE_DIR}/openthread/etc/cmake/functions.cmake")

set(OT_PACKAGE_VERSION "" CACHE STRING "OpenThread Package Version")
if(OT_PACKAGE_VERSION STREQUAL "")
    ot_git_version(OT_PACKAGE_VERSION)
    message(STATUS "Setting default package version: ${OT_PACKAGE_VERSION}")
endif()
message(STATUS "Package Version: ${OT_PACKAGE_VERSION}")

set(CHIP "" CACHE STRING "Chip famille name")

if (OT_BUILD_RT)
    if (OT_BUILD_RT1060)
        set(PLAT_LIB "openthread-rt1060")
        set(CONFIG_FILE "\"openthread-core-rt1060-config.h\"")
        set(CORE_CONFIG_FILE "\"openthread-core-rt1060-config.h\"")
        set(CORE_CONFIG_CHECK "\"openthread-core-rt1060-config-check.h\"")
        set(PLATFORM rt1060)
        set(CHIP imx_rt)
    endif()
elseif (OT_BUILD_K32W0)
    if (OT_BUILD_K32W061)
        set(PLAT_LIB "openthread-k32w061")
        set(CONFIG_FILE "\"openthread-core-k32w061-config.h\"")
        set(CORE_CONFIG_FILE "\"openthread-core-k32w061-config.h\"")
        set(CORE_CONFIG_CHECK "\"openthread-core-k32w061-config-check.h\"")
        set(PLATFORM k32w061)
        set(CHIP k32w0)
    elseif (OT_BUILD_JN5189)
        set(PLAT_LIB "openthread-jn5189")
        set(CONFIG_FILE "\"openthread-core-jn5189-config.h\"")
        set(CORE_CONFIG_FILE "\"openthread-core-jn5189-config.h\"")
        set(CORE_CONFIG_CHECK "\"openthread-core-jn5189-config-check.h\"")
        set(PLATFORM jn5189)
        set(CHIP k32w0)
    endif()
endif()

set(OT_PLATFORM_LIB ${PLAT_LIB})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(RUNTIME_OUTPUT_CMAKE_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

#Set default log level if not set
if (OT_STACK_ENABLE_LOG)
    if (NOT OT_LOG_LEVEL)
        set(OT_LOG_LEVEL DEBG)
    endif()
    if (NOT OT_LOG_LEVEL)
        set(OT_LOG_OUTPUT APP)
    endif()
endif()

add_subdirectory(openthread)

# If OT logging is enabled add a dependency to SDK log components
if (OT_STACK_ENABLE_LOG)
    target_link_libraries(ot-config
        INTERFACE
        nxp-${PLATFORM}-driver)
    target_compile_definitions(ot-config
        INTERFACE
        -DOT_STACK_ENABLE_LOG)
endif ()

target_compile_definitions(ot-config INTERFACE
    OPENTHREAD_CONFIG_FILE=${CONFIG_FILE}
    OPENTHREAD_PROJECT_CORE_CONFIG_FILE=${CORE_CONFIG_FILE}
    OPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE=${CORE_CONFIG_CHECK}
)

if(CHIP)
    target_include_directories(ot-config INTERFACE
        ${PROJECT_SOURCE_DIR}/src/${CHIP}/${PLATFORM}
    )
else()
    target_include_directories(ot-config INTERFACE
        ${PROJECT_SOURCE_DIR}/src/${PLATFORM}
        )
endif()

add_subdirectory(third_party)
add_subdirectory(src)
