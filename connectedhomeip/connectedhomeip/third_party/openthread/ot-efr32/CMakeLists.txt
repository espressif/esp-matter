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


# ==============================================================================
# CMake project definitions
# ==============================================================================
cmake_minimum_required(VERSION 3.16.0)
project(ot-efr32 VERSION 1.0.0)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Toolchain file
message(STATUS "Toolchain file: ${CMAKE_TOOLCHAIN_FILE}")

# ==============================================================================
# OpenThread
# ==============================================================================
include(${PROJECT_SOURCE_DIR}/openthread/etc/cmake/functions.cmake)
include(${PROJECT_SOURCE_DIR}/third_party/silabs/cmake/utility.cmake)

# Package Version
set(OT_PACKAGE_VERSION "" CACHE STRING "OpenThread Package Version")
if(OT_PACKAGE_VERSION STREQUAL "")
    ot_git_version(OT_PACKAGE_VERSION)
    message(STATUS "Setting default package version: ${OT_PACKAGE_VERSION}")
endif()
message(STATUS "Package Version: ${OT_PACKAGE_VERSION}")

set(OT_PLATFORM_LIB "openthread-efr32-soc")
set(OT_PLATFORM_LIB_RCP openthread-efr32-rcp)

set(OT_PLATFORM "external" CACHE STRING "disable example platforms" FORCE)

# ==============================================================================
# mbedtls
# ==============================================================================
set(OT_EXTERNAL_MBEDTLS "openthread-efr32-soc-mbedtls" CACHE STRING "use silabs mbedtls" FORCE)
set(OT_BUILTIN_MBEDTLS_MANAGEMENT OFF CACHE BOOL "disable builtin mbedtls management" FORCE)
set(OT_MBEDTLS ${OT_EXTERNAL_MBEDTLS})

# NOTE: The "openthread" subdirectory needs to be added after all OT_* variables are defined
add_subdirectory(openthread)

# Add any efr32 example projects
add_subdirectory(examples)

target_compile_options(ot-config INTERFACE
    -Wno-shadow
    -Wno-pedantic
)

# ==============================================================================
# Generated platform projects
# ==============================================================================
set(slc_generated_projects
    ${PROJECT_SOURCE_DIR}/build/${BOARD_LOWERCASE}/slc/soc/platform
    ${PROJECT_SOURCE_DIR}/build/${BOARD_LOWERCASE}/slc/soc/mbedtls
    ${PROJECT_SOURCE_DIR}/build/${BOARD_LOWERCASE}/slc/rcp/platform
    ${PROJECT_SOURCE_DIR}/build/${BOARD_LOWERCASE}/slc/rcp/mbedtls
)

foreach(slc_project IN LISTS slc_generated_projects)
    message("Checking ${slc_project}")
    if(EXISTS ${slc_project})
        message("......... EXISTS")
        add_subdirectory(${slc_project})
    else()
        message("......... NOT EXISTS")
    endif()
endforeach()

# efr32mg12 Sub-GHz support
set(EFR32MG12_915MHZ_PHY_SOURCES
    ${PROJECT_SOURCE_DIR}/src/efr32mg12/phy/PHY_IEEE802154_915MHZ_OQPSK_EFR32XG12.c
    ${PROJECT_SOURCE_DIR}/src/efr32mg12/phy/PHY_IEEE802154_915MHZ_2GFSK_EFR32XG12.c
)
string(COMPARE EQUAL "${PLATFORM_LOWERCASE}" "efr32mg12" _cmp)
if (_cmp)
    target_sources(${OT_PLATFORM_LIB} PUBLIC ${EFR32MG12_915MHZ_PHY_SOURCES})
endif()
