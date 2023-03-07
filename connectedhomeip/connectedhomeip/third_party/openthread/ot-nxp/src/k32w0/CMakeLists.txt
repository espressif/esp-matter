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

set(K32W0_COMM_SOURCES
    ${PROJECT_SOURCE_DIR}/src/common/ram_storage.c
    platform/flash_pdm.c
    platform/pdm_ram_storage_glue.c
    platform/alarm.c
    platform/diag.c
    platform/entropy.c
    platform/flash.c
    platform/logging.c
    platform/misc.c
    platform/system.c
    platform/uart.c
    $ENV{NXP_K32W0_SDK_ROOT}/middleware/mbedtls/port/ksdk/aes_alt.c
    $ENV{NXP_K32W0_SDK_ROOT}/middleware/mbedtls/port/ksdk/ksdk_mbedtls.c
)

list(APPEND OT_PUBLIC_INCLUDES
        "${PROJECT_SOURCE_DIR}/include/"
        "${PROJECT_SOURCE_DIR}/src/common"
        "${PROJECT_SOURCE_DIR}/openthread/third_party/mbedtls/"
        "${PROJECT_SOURCE_DIR}/openthread/third_party/mbedtls/repo/"
        "${PROJECT_SOURCE_DIR}/openthread/third_party/mbedtls/repo/include/"
)

if (OT_1_2_SUPPORT)
    list(APPEND K32W0_COMM_SOURCES
        platform/radio.c
    )

    if (NOT OT_MAC_CSL_RECEIVER_ENABLE)
        # disable CSL receiver for now
        list(APPEND OT_PLATFORM_DEFINES
            OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE=0
        )
    endif()
else()
    list(APPEND K32W0_COMM_SOURCES
        platform/radio_v1.c
    )

    # disable all OT 1.2 specifics
    list(APPEND OT_PLATFORM_DEFINES
            OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE=0
            OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE=0
            OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE=0
            OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_SECURITY_ENABLE=0
            OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_TIMING_ENABLE=0
            OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE=0
    )
endif()

if (OT_NCP_SPI)
    list(APPEND K32W0_COMM_SOURCES
        platform/spi_slave.c
    )
    list(APPEND OT_PLATFORM_DEFINES
        "OT_PLAT_SPI_SUPPORT"
    )
endif()

set(K32W0_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/openthread/examples/platforms
)

if(OT_BUILD_K32W061)
    include(k32w061/k32w061.cmake)
elseif(OT_BUILD_JN5189)
    include(jn5189/jn5189.cmake)
endif()
