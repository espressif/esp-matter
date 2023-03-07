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

set_property(SOURCE $ENV{NXP_RT1060_SDK_ROOT}/devices/MIMXRT1062/gcc/startup_MIMXRT1062.S PROPERTY LANGUAGE C)

if (SDK_RELEASE)
    message("SDK_RELEASE driver path chosen")
    set(BOARD_FILES_PATH $ENV{NXP_RT1060_SDK_ROOT}/boards/evkmimxrt1060/project_template)
    set(DRIVER_FILES_PATH $ENV{NXP_RT1060_SDK_ROOT}/devices/MIMXRT1062/drivers)
    set(DRIVER_LPUART_FILES_PATH ${DRIVER_FILES_PATH})
    set(DRIVER_PIT_FILES_PATH ${DRIVER_FILES_PATH})
    set(DRIVER_TRNG_FILES_PATH ${DRIVER_FILES_PATH})
    set(DRIVER_FLEXSPI_FILES_PATH ${DRIVER_FILES_PATH})
    set(DRIVER_IGPIO_FILES_PATH ${DRIVER_FILES_PATH})
    set(DRIVER_DCP_FILES_PATH ${DRIVER_FILES_PATH})
    set(DRIVER_FSL_CACHE_FILES_PATH ${DRIVER_FILES_PATH})
    set(DRIVER_COMMON_FILES_PATH ${DRIVER_FILES_PATH})
    set(DRIVER_DEVICE_FILES_PATH ${DRIVER_FILES_PATH})
    set(DRIVER_EDMA_FILES_PATH ${DRIVER_FILES_PATH})
    set(DRIVER_DMAMUX_FILES_PATH ${DRIVER_FILES_PATH})
    set(DEBUG_CONSOLE_FILES_PATH $ENV{NXP_RT1060_SDK_ROOT}/devices/MIMXRT1062/utilities/debug_console)
    set(STR_FILES_PATH $ENV{NXP_RT1060_SDK_ROOT}/devices/MIMXRT1062/utilities/str)
    set(FSL_FLEXSPI_NOR_BOOT_FILES_PATH $ENV{NXP_RT1060_SDK_ROOT}/devices/MIMXRT1062/xip)
else ()
    message("SDK_DEV driver path chosen")
    set(BOARD_FILES_PATH $ENV{NXP_RT1060_SDK_ROOT}/boards/evkmimxrt1060)
    set(DRIVER_LPUART_FILES_PATH $ENV{NXP_RT1060_SDK_ROOT}/platform/drivers/lpuart)
    set(DRIVER_PIT_FILES_PATH $ENV{NXP_RT1060_SDK_ROOT}/platform/drivers/pit)
    set(DRIVER_TRNG_FILES_PATH $ENV{NXP_RT1060_SDK_ROOT}/platform/drivers/trng)
    set(DRIVER_FLEXSPI_FILES_PATH $ENV{NXP_RT1060_SDK_ROOT}/platform/drivers/flexspi)
    set(DRIVER_IGPIO_FILES_PATH $ENV{NXP_RT1060_SDK_ROOT}/platform/drivers/igpio)
    set(DRIVER_DCP_FILES_PATH $ENV{NXP_RT1060_SDK_ROOT}/platform/drivers/dcp)
    set(DRIVER_FSL_CACHE_FILES_PATH $ENV{NXP_RT1060_SDK_ROOT}/platform/drivers/cache/armv7-m7)
    set(DRIVER_COMMON_FILES_PATH $ENV{NXP_RT1060_SDK_ROOT}/platform/drivers/common)
    set(DRIVER_DEVICE_FILES_PATH $ENV{NXP_RT1060_SDK_ROOT}/devices/MIMXRT1062/drivers)
    set(DRIVER_EDMA_FILES_PATH $ENV{NXP_RT1060_SDK_ROOT}/platform/drivers/edma)
    set(DRIVER_DMAMUX_FILES_PATH $ENV{NXP_RT1060_SDK_ROOT}/platform/drivers/dmamux)
    set(DEBUG_CONSOLE_FILES_PATH $ENV{NXP_RT1060_SDK_ROOT}/platform/utilities/debug_console)
    set(STR_FILES_PATH ${DEBUG_CONSOLE_FILES_PATH}/str)
    set(FSL_FLEXSPI_NOR_BOOT_FILES_PATH $ENV{NXP_RT1060_SDK_ROOT}/devices/MIMXRT1052/xip)
endif ()

add_library(nxp-rt1060-driver
#freertos files
    $ENV{NXP_RT1060_SDK_ROOT}/rtos/freertos/freertos_kernel/event_groups.c
    $ENV{NXP_RT1060_SDK_ROOT}/rtos/freertos/freertos_kernel/croutine.c
    $ENV{NXP_RT1060_SDK_ROOT}/rtos/freertos/freertos_kernel/list.c
    $ENV{NXP_RT1060_SDK_ROOT}/rtos/freertos/freertos_kernel/portable/GCC/ARM_CM4F/port.c
    $ENV{NXP_RT1060_SDK_ROOT}/rtos/freertos/freertos_kernel/queue.c
    $ENV{NXP_RT1060_SDK_ROOT}/rtos/freertos/freertos_kernel/stream_buffer.c
    $ENV{NXP_RT1060_SDK_ROOT}/rtos/freertos/freertos_kernel/tasks.c
    $ENV{NXP_RT1060_SDK_ROOT}/rtos/freertos/freertos_kernel/timers.c
    $ENV{NXP_RT1060_SDK_ROOT}/rtos/freertos/freertos_kernel/portable/MemMang/heap_4.c
#startup files
    $ENV{NXP_RT1060_SDK_ROOT}/devices/MIMXRT1062/gcc/startup_MIMXRT1062.S
    #$ENV{NXP_RT1060_SDK_ROOT}/devices/MIMXRT1062/mcuxpresso/startup_mimxrt1062.c
    $ENV{NXP_RT1060_SDK_ROOT}/devices/MIMXRT1062/system_MIMXRT1062.c
#board files
    ${BOARD_FILES_PATH}/board.c 
    ${BOARD_FILES_PATH}/dcd.c
    ${BOARD_FILES_PATH}/clock_config.c
#other SDK files
    ${DRIVER_LPUART_FILES_PATH}/fsl_lpuart.c
    ${DRIVER_LPUART_FILES_PATH}/fsl_lpuart_edma.c
    ${DRIVER_PIT_FILES_PATH}/fsl_pit.c
    ${DRIVER_TRNG_FILES_PATH}/fsl_trng.c
    ${DRIVER_FLEXSPI_FILES_PATH}/fsl_flexspi.c
    ${DRIVER_COMMON_FILES_PATH}/fsl_common_arm.c
    ${DRIVER_DEVICE_FILES_PATH}/fsl_clock.c
    ${DRIVER_DCP_FILES_PATH}/fsl_dcp.c
    ${DRIVER_FSL_CACHE_FILES_PATH}/fsl_cache.c
    ${DRIVER_EDMA_FILES_PATH}/fsl_edma.c
    ${DRIVER_DMAMUX_FILES_PATH}/fsl_dmamux.c
    ${DEBUG_CONSOLE_FILES_PATH}/fsl_debug_console.c
    ${STR_FILES_PATH}/fsl_str.c
# XIP
    ${FSL_FLEXSPI_NOR_BOOT_FILES_PATH}/fsl_flexspi_nor_boot.c
    $ENV{NXP_RT1060_SDK_ROOT}/boards/evkmimxrt1060/xip/evkmimxrt1060_flexspi_nor_config.c
#SDK components
    $ENV{NXP_RT1060_SDK_ROOT}/components/serial_manager/fsl_component_serial_manager.c
    $ENV{NXP_RT1060_SDK_ROOT}/components/serial_manager/fsl_component_serial_port_uart.c
    $ENV{NXP_RT1060_SDK_ROOT}/components/uart/fsl_adapter_lpuart.c
    $ENV{NXP_RT1060_SDK_ROOT}/components/lists/fsl_component_generic_list.c
    $ENV{NXP_RT1060_SDK_ROOT}/components/osa/fsl_os_abstraction_free_rtos.c
    $ENV{NXP_RT1060_SDK_ROOT}/components/log/fsl_component_log.c
    $ENV{NXP_RT1060_SDK_ROOT}/components/log/fsl_component_log_backend_debugconsole.c
    $ENV{NXP_RT1060_SDK_ROOT}/components/timer_manager/fsl_component_timer_manager.c
    $ENV{NXP_RT1060_SDK_ROOT}/components/timer/fsl_adapter_pit.c
    $ENV{NXP_RT1060_SDK_ROOT}/components/rng/fsl_adapter_trng.c
    $ENV{NXP_RT1060_SDK_ROOT}/components/internal_flash/fsl_adapter_flexspi_nor_flash.c
#Framework files
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/wireless/framework/FunctionLib/FunctionLib.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/wireless/framework/NVM/Source/NV_Flash.c
)

#freertos 
target_include_directories(nxp-rt1060-driver 
    PUBLIC
#freertos includes
        $ENV{NXP_RT1060_SDK_ROOT}/rtos/freertos/freertos_kernel/include
        $ENV{NXP_RT1060_SDK_ROOT}/rtos/freertos/freertos_kernel/portable/GCC/ARM_CM4F
#boards include path location
        ${BOARD_FILES_PATH}
#CMSIS includes
        $ENV{NXP_RT1060_SDK_ROOT}/CMSIS/Core/Include 
#other SDK files
        $ENV{NXP_RT1060_SDK_ROOT}/devices/MIMXRT1062/drivers
        $ENV{NXP_RT1060_SDK_ROOT}/devices/MIMXRT1062
        ${DRIVER_COMMON_FILES_PATH}
        ${DRIVER_LPUART_FILES_PATH}
        ${DRIVER_IGPIO_FILES_PATH}
        ${DRIVER_DCP_FILES_PATH}
        ${DRIVER_FSL_CACHE_FILES_PATH}
        ${DRIVER_PIT_FILES_PATH}
        ${DRIVER_TRNG_FILES_PATH}
        ${DRIVER_FLEXSPI_FILES_PATH}
        ${DRIVER_EDMA_FILES_PATH}
        ${DRIVER_DMAMUX_FILES_PATH}
        ${DEBUG_CONSOLE_FILES_PATH}
        ${STR_FILES_PATH}
# XIP
        $ENV{NXP_RT1060_SDK_ROOT}/devices/MIMXRT1052/xip
        $ENV{NXP_RT1060_SDK_ROOT}/boards/evkmimxrt1060/xip
#SDK components
        $ENV{NXP_RT1060_SDK_ROOT}/components/uart
        $ENV{NXP_RT1060_SDK_ROOT}/components/serial_manager
        $ENV{NXP_RT1060_SDK_ROOT}/components/lists
        $ENV{NXP_RT1060_SDK_ROOT}/components/osa
        $ENV{NXP_RT1060_SDK_ROOT}/components/common_task
        $ENV{NXP_RT1060_SDK_ROOT}/components/log
        $ENV{NXP_RT1060_SDK_ROOT}/components/timer_manager
        $ENV{NXP_RT1060_SDK_ROOT}/components/timer
        $ENV{NXP_RT1060_SDK_ROOT}/components/rng
        $ENV{NXP_RT1060_SDK_ROOT}/components/internal_flash
#Framework files
        $ENV{NXP_RT1060_SDK_ROOT}/middleware/wireless/framework/FunctionLib
        $ENV{NXP_RT1060_SDK_ROOT}/middleware/wireless/framework/Common
        $ENV{NXP_RT1060_SDK_ROOT}/middleware/wireless/framework/NVM/Interface
)

target_compile_definitions(nxp-rt1060-driver
    PUBLIC
    -D__STARTUP_CLEAR_BSS
    -D__STARTUP_INITIALIZE_NONCACHEDATA
    #-DSERIAL_MANAGER_NON_BLOCKING_MODE=1
    #-DSERIAL_MANAGER_TASK_HANDLE_RX_AVAILABLE_NOTIFY=1
    -DSDK_OS_FREE_RTOS
    -DOSA_USED
    -DFSL_OSA_MAIN_FUNC_ENABLE=0
    -DFSL_RTOS_FREE_RTOS
    -DXIP_EXTERNAL_FLASH=1
    -DXIP_BOOT_HEADER_ENABLE=1
    -DCPU_MIMXRT1062DVL6A
    -DFSL_SDK_ENABLE_DRIVER_CACHE_CONTROL=1
    -DSDK_COMPONENT_INTEGRATION=1
    -DFSL_DRIVER_TRANSFER_DOUBLE_WEAK_IRQ=0
    -DDEBUG_CONSOLE_TRANSFER_NON_BLOCKING
    -DHAL_UART_ADAPTER_FIFO=1
    -DSDK_DEBUGCONSOLE_UART=1
    -DSERIAL_PORT_TYPE_UART=1
    -DCONFIG_ARM=1
    -DSERIAL_MANAGER_TASK_STACK_SIZE=4048
    #DMA support enablement
    #-DHAL_UART_DMA_ENABLE=1
    #-DSERIAL_PORT_TYPE_UART_DMA=1
    #NVM flags
    -DgNvStorageIncluded_d=1
    -DgNvTableKeptInRam_d=1
    -DNVM_NO_COMPONNET=1
    #Async logging feature
    -DLOG_ENABLE_ASYNC_MODE=1
    -DLOG_MAX_ARGUMENT_COUNT=17
    -DLOG_MAX_BUFF_LOG_COUNT=250
    -DLOG_ENABLE_OVERWRITE=1
)

target_link_libraries(nxp-rt1060-driver
    PRIVATE
        openthread-rt1060
)

target_compile_options(nxp-rt1060-driver
    PRIVATE
        ${OT_CFLAGS}
        -Wno-unknown-pragmas
        -Wno-sign-compare
        -Wno-unused-function
        -Wno-unused-parameter
        -Wno-empty-body
)

add_library(nxp-rt1060-mbedtls
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/aes.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/aesni.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/arc4.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/aria.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/asn1parse.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/asn1write.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/base64.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/bignum.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/blowfish.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/camellia.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/ccm.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/certs.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/cipher.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/cipher_wrap.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/chacha20.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/chachapoly.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/cmac.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/ctr_drbg.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/debug.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/des.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/dhm.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/ecdh.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/ecdsa.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/ecjpake.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/ecp.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/ecp_curves.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/entropy.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/entropy_poll.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/error.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/gcm.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/havege.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/hkdf.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/hmac_drbg.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/md.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/md2.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/md4.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/md5.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/memory_buffer_alloc.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/net_sockets.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/nist_kw.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/oid.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/padlock.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/pem.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/pk.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/pk_wrap.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/pkcs5.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/pkcs11.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/pkcs12.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/pkparse.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/pkwrite.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/platform.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/platform_util.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/poly1305.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/psa_crypto.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/psa_crypto_client.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/psa_crypto_driver_wrappers.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/psa_crypto_ecp.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/psa_crypto_rsa.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/psa_crypto_se.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/psa_crypto_slot_management.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/psa_crypto_storage.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/psa_its_file.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/ripemd160.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/rsa.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/rsa_internal.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/sha1.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/sha256.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/sha512.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/ssl_cache.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/ssl_ciphersuites.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/ssl_cli.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/ssl_cookie.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/ssl_msg.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/ssl_srv.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/ssl_ticket.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/ssl_tls.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/ssl_tls13_keys.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/threading.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/timing.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/version.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/version_features.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/x509.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/x509_create.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/x509_crl.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/x509_crt.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/x509_csr.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/x509write_crt.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/x509write_csr.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library/xtea.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/port/ksdk/ksdk_mbedtls.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/port/ksdk/des_alt.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/port/ksdk/aes_alt.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/port/ksdk/ecp_alt.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/port/ksdk/ecp_curves_alt.c
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/port/ksdk/ecp_alt_ksdk.c
)

target_include_directories(nxp-rt1060-mbedtls
    PRIVATE
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/library
    PUBLIC
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/include
    $ENV{NXP_RT1060_SDK_ROOT}/middleware/mbedtls/port/ksdk
)

target_compile_definitions(nxp-rt1060-mbedtls 
    PUBLIC
    "MBEDTLS_USER_CONFIG_FILE=\"rt1060-mbedtls-config.h\""
    "MBEDTLS_CONFIG_FILE=\"${PROJECT_SOURCE_DIR}/openthread/third_party/mbedtls/mbedtls-config.h\"" 
)

target_compile_options(nxp-rt1060-mbedtls
    PRIVATE
        -Wno-unused-function
)

target_link_libraries(nxp-rt1060-mbedtls
    PUBLIC
    nxp-rt1060-driver
    ot-config
)

target_link_libraries(ot-config
    INTERFACE
    nxp-rt1060-mbedtls
)
