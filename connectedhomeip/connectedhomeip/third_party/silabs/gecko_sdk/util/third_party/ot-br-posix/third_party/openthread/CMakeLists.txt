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

set(OT_BORDER_AGENT ON CACHE BOOL "enable border agent" FORCE)
set(OT_BORDER_ROUTER ON CACHE BOOL "enable border router feature" FORCE)
set(OT_BORDER_ROUTING ${OTBR_BORDER_ROUTING} CACHE BOOL "enable border routing feature" FORCE)
set(OT_BUILD_EXECUTABLES OFF CACHE BOOL "disable building executables" FORCE)
set(OT_BUILTIN_MBEDTLS_MANAGEMENT OFF CACHE BOOL "diable mbedTLS management" FORCE)
set(OT_COMMISSIONER ON CACHE BOOL "enable commissioner")
set(OT_DAEMON ON CACHE BOOL "enable daemon mode" FORCE)
set(OT_FIREWALL ON CACHE BOOL "enable firewall feature")
set(OT_JOINER ON CACHE BOOL "enable joiner" FORCE)
set(OT_LEGACY ON CACHE STRING "enable legacy network support" FORCE)
if (NOT OT_THREAD_VERSION EQUAL "1.1")
    set(OT_LINK_METRICS_SUBJECT ON CACHE BOOL "enable link metrics subject" FORCE)
endif()
set(OT_SLAAC ON CACHE BOOL "enable SLAAC" FORCE)
set(OT_TREL ${OTBR_TREL} CACHE BOOL "enable TREL" FORCE)

if (NOT OT_LOG_LEVEL)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(OT_LOG_LEVEL "DEBG" CACHE STRING "set OpenThread log level to DEBG" FORCE)
    else()
        set(OT_LOG_LEVEL "INFO" CACHE STRING "set OpenThread log level to INFO" FORCE)
    endif()
endif()

set(OT_LOG_LEVEL_DYNAMIC ON CACHE BOOL "enable dynamic log level control" FORCE)
set(OT_MAC_FILTER ON CACHE STRING "enable MAC filter" FORCE)
set(OT_MBEDTLS_CONFIG_FILE "\"${PROJECT_SOURCE_DIR}/third_party/openthread/mbedtls-config.h\"" CACHE STRING "use mbedtls-config.h of this project" FORCE)
set(OT_PLATFORM "posix" CACHE STRING "use posix platform" FORCE)
set(OT_PLATFORM_NETIF ON CACHE BOOL "enable platform netif" FORCE)
set(OT_PLATFORM_UDP ON CACHE BOOL "enable platform UDP" FORCE)
set(OT_UDP_FORWARD OFF CACHE BOOL "diable udp forward" FORCE)
set(OT_BACKBONE_ROUTER_DUA_NDPROXYING ${OTBR_DUA_ROUTING} CACHE BOOL "Configure DUA ND Proxy feature in OpenThread" FORCE)

if (OTBR_SRP_ADVERTISING_PROXY)
    set(OT_SRP_SERVER ON CACHE BOOL "enable SRP server" FORCE)
    set(OT_ECDSA ON CACHE BOOL "enable ECDSA" FORCE)
    set(OT_SERVICE ON CACHE BOOL "enable service" FORCE)
    set(OT_EXTERNAL_HEAP ON CACHE BOOL "enable external heap" FORCE)
endif()

if (OTBR_BACKBONE_ROUTER)
    set(OT_BACKBONE_ROUTER ON CACHE BOOL "Enable Backbone Router feature in OpenThread" FORCE)
    set(OT_SERVICE ON CACHE BOOL "Backbone Router requires Thread network service" FORCE)
endif()

if (NOT OT_THREAD_VERSION STREQUAL "1.1")
    if (OT_REFERENCE_DEVICE)
        set(OT_DUA ON CACHE BOOL "Enable Thread 1.2 DUA for reference devices")
    endif()
    set(OT_MLR ON CACHE BOOL "Enable Thread 1.2 MLR by default")
endif()

set(OT_COAP ON CACHE BOOL "Enable CoAP in OpenThread")
set(OT_COAPS ON CACHE BOOL "Enable secure CoAP in OpenThread")

# Diagnostics / Logging
set(OT_DIAGNOSTIC ON CACHE BOOL "Enable diagnostics module" FORCE)
set(OT_FULL_LOGS ON CACHE BOOL "Enable debug level logging" FORCE)
set(OT_LOG_LEVEL "DEBG" CACHE STRING "Set max OpenThread log level to DEBG (dynamic logging turned on)" FORCE)
set(OT_COPROCESSOR_RPC ON CACHE BOOL "Enable co-processor RPC support" FORCE)

# RCP recovery
set(OT_RCP_RESTORATION_MAX_COUNT "100" CACHE STRING "set max RCP restoration count" FORCE)

if (OTBR_DNSSD_DISCOVERY_PROXY)
    set(OT_DNSSD_SERVER ON CACHE BOOL "enable DNS-SD server support" FORCE)
endif()

if (NOT OT_POSIX_SETTINGS_PATH)
    set(OT_POSIX_SETTINGS_PATH "\"/var/lib/thread\"" CACHE STRING "set the directory to store Thread data" FORCE)
endif()

# Multi-PAN RCP support
if (OT_MULTIPAN_RCP)
    set(OT_RCP_RESTORATION_MAX_COUNT "100" CACHE STRING "set max RCP restoration count" FORCE)
    set(OT_POSIX_MAX_POWER_TABLE OFF CACHE BOOL "enable max power table" FORCE)
    set(OT_CHANNEL_MANAGER OFF CACHE BOOL "enable channel manager support" FORCE)
    set(OT_CHANNEL_MONITOR OFF CACHE BOOL "enable channel monitor support" FORCE)
endif()

add_subdirectory(repo EXCLUDE_FROM_ALL)

if (OT_COPROCESSOR_RPC)
    # NOTE: The following must be true otherwise prints on the POSIX Host will be truncated.
    #    OPENTHREAD_CONFIG_CLI_MAX_LINE_LENGTH >= OPENTHREAD_CONFIG_COPROCESSOR_RPC_OUTPUT_BUFFER_SIZE
    target_compile_definitions(ot-config INTERFACE "OPENTHREAD_CONFIG_CLI_MAX_LINE_LENGTH=1200")
    target_compile_definitions(ot-config INTERFACE "OPENTHREAD_CONFIG_COPROCESSOR_RPC_OUTPUT_BUFFER_SIZE=1200")
endif()

target_compile_definitions(ot-config INTERFACE
    "-DOPENTHREAD_CONFIG_MAX_STATECHANGE_HANDLERS=3"
    "-DOPENTHREAD_CONFIG_MLE_STEERING_DATA_SET_OOB_ENABLE=1"
    "-DOPENTHREAD_CONFIG_TCP_ENABLE=0"
)

if (NOT OT_THREAD_VERSION STREQUAL "1.1")
    target_compile_definitions(ot-config INTERFACE
        "-DOPENTHREAD_CONFIG_TMF_PROXY_DUA_ENABLE=1"
        "-DOPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE=1"
    )
endif()

# SubGhz support
if (OTBR_PROPRIETARY_RADIO_SUPPORT)
    target_compile_definitions(ot-config INTERFACE
        "-DOPENTHREAD_CONFIG_LOG_PLATFORM=1"
        "-DOPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_SUPPORT=1"
        "-DOPENTHREAD_CONFIG_RADIO_2P4GHZ_OQPSK_SUPPORT=0"
        "-DOPENTHREAD_CONFIG_RADIO_915MHZ_OQPSK_SUPPORT=0"
        "-DOPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_PAGE=23"
        "-DOPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MIN=0"
        "-DOPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MAX=24"
        "-DOPENTHREAD_CONFIG_DEFAULT_CHANNEL=OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MIN"
        "-DOPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MASK=0x1ffffff"
    )
endif()
