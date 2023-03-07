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
project(openthread-br VERSION 0.3.0)

add_compile_options(-Wall -Wextra -Werror -Wfatal-errors -Wuninitialized -Wno-missing-braces)

add_library(otbr-config INTERFACE)

set(OTBR_INFRA_IF_NAME "wlan0" CACHE STRING "The infrastructure interface name")
set(OTBR_VENDOR_NAME "OpenThread" CACHE STRING "The vendor name")
set(OTBR_PRODUCT_NAME "BorderRouter" CACHE STRING "The product name")
set(OTBR_NAME "${OTBR_VENDOR_NAME}_${OTBR_PRODUCT_NAME}" CACHE STRING "The package name")
set(OTBR_MESHCOP_SERVICE_INSTANCE_NAME "${OTBR_VENDOR_NAME} ${OTBR_PRODUCT_NAME}" CACHE STRING "The OTBR MeshCoP service instance name")
set(OTBR_MDNS "avahi" CACHE STRING "mDNS publisher provider")
set(OTBR_SYSLOG_FACILITY_ID LOG_USER CACHE STRING "Syslog logging facility")

set_property(CACHE OTBR_MDNS PROPERTY STRINGS "avahi" "mDNSResponder")

include("${PROJECT_SOURCE_DIR}/etc/cmake/options.cmake")

if(NOT CMAKE_C_STANDARD)
    set(CMAKE_C_STANDARD 99)
    set(CMAKE_C_STANDARD_REQUIRED ON)
endif()

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

if (OTBR_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(STATUS "Coverage: ON")
    target_compile_options(otbr-config INTERFACE -g -O0 --coverage)
    target_link_libraries(otbr-config INTERFACE --coverage)
endif()

message(STATUS "OTBR package name: ${OTBR_NAME}")

if(NOT OTBR_VERSION)
    execute_process(
        COMMAND git describe --dirty --always
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE OTBR_GIT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(OTBR_GIT_VERSION)
        set(OTBR_VERSION "${PROJECT_VERSION}-${OTBR_GIT_VERSION}")
    else()
        set(OTBR_VERSION "${PROJECT_VERSION}")
    endif()
endif()

message(STATUS "Version: ${OTBR_VERSION}")

target_include_directories(otbr-config INTERFACE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)
target_compile_definitions(otbr-config INTERFACE
    "OTBR_VENDOR_NAME=\"${OTBR_VENDOR_NAME}\""
    "OTBR_PRODUCT_NAME=\"${OTBR_PRODUCT_NAME}\""
    "OTBR_PACKAGE_NAME=\"${OTBR_NAME}\""
    "OTBR_PACKAGE_VERSION=\"${OTBR_VERSION}\""
    "OTBR_MESHCOP_SERVICE_INSTANCE_NAME=\"${OTBR_MESHCOP_SERVICE_INSTANCE_NAME}\""
    "OTBR_SYSLOG_FACILITY_ID=${OTBR_SYSLOG_FACILITY_ID}"
)

if(BUILD_SHARED_LIBS)
    target_link_libraries(otbr-config INTERFACE -Wl,--unresolved-symbols=ignore-in-shared-libs)
endif()

include(GNUInstallDirs)

pkg_check_modules(SYSTEMD systemd)

if(SYSTEMD_FOUND)
    pkg_get_variable(OTBR_SYSTEMD_UNIT_DIR systemd systemdsystemunitdir)
endif()


add_subdirectory(third_party EXCLUDE_FROM_ALL)
add_subdirectory(src)
add_subdirectory(tools)


if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)

    if(BUILD_TESTING)
        pkg_check_modules(CPPUTEST cpputest REQUIRED)
        add_subdirectory(tests)
    endif()

    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "OpenThread Authors (https://github.com/openthread/openthread)")
    set(CPACK_PACKAGE_CONTACT "OpenThread Authors (https://github.com/openthread/openthread)")
    include(CPack)
endif()

if (OTBR_DOC)
    add_subdirectory(doc)
endif()
