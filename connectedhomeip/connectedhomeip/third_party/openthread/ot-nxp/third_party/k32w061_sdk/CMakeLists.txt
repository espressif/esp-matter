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

option(OT_RCP_FLOW_CONTROL "enable UART flow control for RCP" OFF)

SET_SOURCE_FILES_PROPERTIES($ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/gcc/startup_K32W061.c PROPERTIES LANGUAGE CXX)

if (SDK_RELEASE)

    add_library(nxp-k32w061-driver
        $ENV{NXP_K32W0_SDK_ROOT}/boards/k32w061dk6/wireless_examples/openthread/reed/bm/board.c
        $ENV{NXP_K32W0_SDK_ROOT}/boards/k32w061dk6/wireless_examples/openthread/reed/bm/hardware_init.c
        $ENV{NXP_K32W0_SDK_ROOT}/boards/k32w061dk6/wireless_examples/openthread/reed/bm/board_utility.c
        ${PROJECT_SOURCE_DIR}/third_party/k32w061_sdk/mr5_fixes/clock_config.c
        ${PROJECT_SOURCE_DIR}/third_party/k32w061_sdk/mr5_fixes/pin_mux.c
        ${PROJECT_SOURCE_DIR}/third_party/k32w061_sdk/mr5_fixes/serial_port_swo.c
        $ENV{NXP_K32W0_SDK_ROOT}/components/serial_manager/serial_manager.c
        $ENV{NXP_K32W0_SDK_ROOT}/components/serial_manager/serial_port_uart.c
        $ENV{NXP_K32W0_SDK_ROOT}/components/lists/generic_list.c
        $ENV{NXP_K32W0_SDK_ROOT}/components/uart/usart_adapter.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/gcc/startup_K32W061.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/system_K32W061.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_aes.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_clock.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_ctimer.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_common.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_gint.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_flash.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_flexcomm.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_gpio.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_power.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_reset.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_rng.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_sha.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_usart.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_wtimer.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_spi.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_dma.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_spi_dma.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_pint.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_inputmux.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_adc.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_spifi.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/utilities/fsl_assert.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/utilities/debug_console/fsl_debug_console.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/utilities/str/fsl_str.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/Common/MicroInt_arm_sdk2.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/Flash/Internal/Flash_Adapter.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/FunctionLib/FunctionLib.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/Lists/GenericList.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/MemManager/Source/MemManager.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/Panic/Source/Panic.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/PDM/pdm_port.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/Reset/Reset.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/SerialManager/Source/SerialManager.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/SerialManager/Source/UART_Serial_Adapter.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/TimersManager/Source/TMR_Adapter.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/TimersManager/Source/TimersManager.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/GPIO/GPIO_Adapter.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/RNG/Source/RNG.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/SecLib/SecLib.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/OSAbstraction/Source/fsl_os_abstraction_bm.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/XCVR/DK6/fsl_xcvr.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/LowPower/Source/k32w061dk6/PWR.c
    )
    else()

    add_library(nxp-k32w061-driver
        $ENV{NXP_K32W0_SDK_ROOT}/boards/k32w061dk6/utility/board_utility.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/boards/dk6/board.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/openthread/boards/dk6_k32w061/hardware_init.c
        ${PROJECT_SOURCE_DIR}/third_party/k32w061_sdk/mr5_fixes/clock_config.c
        ${PROJECT_SOURCE_DIR}/third_party/k32w061_sdk/mr5_fixes/pin_mux.c
        ${PROJECT_SOURCE_DIR}/third_party/k32w061_sdk/mr5_fixes/serial_port_swo.c
        $ENV{NXP_K32W0_SDK_ROOT}/components/serial_manager/serial_manager.c
        $ENV{NXP_K32W0_SDK_ROOT}/components/serial_manager/serial_port_uart.c
        $ENV{NXP_K32W0_SDK_ROOT}/components/uart/usart_adapter.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_clock.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_power.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_reset.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers/fsl_wtimer.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/system_K32W061.c
        $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/gcc/startup_K32W061.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/Common/MicroInt_arm_sdk2.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/Flash/Internal/Flash_Adapter.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/FunctionLib/FunctionLib.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/Lists/GenericList.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/MemManager/Source/MemManager.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/Panic/Source/Panic.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/PDM/pdm_port.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/Reset/Reset.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/SerialManager/Source/SerialManager.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/SerialManager/Source/UART_Serial_Adapter.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/TimersManager/Source/TMR_Adapter.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/TimersManager/Source/TimersManager.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/GPIO/GPIO_Adapter.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/RNG/Source/RNG.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/SecLib/SecLib.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/OSAbstraction/Source/fsl_os_abstraction_bm.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/XCVR/DK6/fsl_xcvr.c
        $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/LowPower/Source/jn518x/PWR.c
        $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/aes/fsl_aes.c
        $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/common/fsl_common.c
        $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/ctimer/fsl_ctimer.c
        $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/flexcomm/fsl_flexcomm.c
        $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/flexcomm/fsl_usart.c
        $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/lpc_gpio/fsl_gpio.c
        $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/gint/fsl_gint.c
        $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/jn_flash/fsl_flash.c
        $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/jn_rng/fsl_rng.c
        $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/sha/fsl_sha.c
        $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/flexcomm/fsl_spi.c
        $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/pint/fsl_pint.c
        $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/inputmux/fsl_inputmux.c
        $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/lpc_dma/fsl_dma.c
        $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/lpc_adc/fsl_adc.c
        $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/spifi/fsl_spifi.c
        $ENV{NXP_K32W0_SDK_ROOT}/platform/utilities/debug_console/fsl_debug_console.c
        $ENV{NXP_K32W0_SDK_ROOT}/platform/utilities/debug_console/str/fsl_str.c
        $ENV{NXP_K32W0_SDK_ROOT}/platform/utilities/assert/fsl_assert.c
    )
endif()

target_compile_definitions(nxp-k32w061-driver
    PUBLIC
        ${OT_PLATFORM_DEFINES}
        -DCPU_K32W061HN
        -DCPU_JN518X
        -DCPU_JN518X_REV=2
        -DJENNIC_CHIP_FAMILY_JN518x
        -DJENNIC_CHIP_FAMILY_NAME=_JN518x
        -DJENNIC_CHIP_FAMILY=JN518x
        -DgPWR_LDOMEM_0_9V_PD=0
        -DNO_SYSCORECLK_UPD=0
        -DENABLE_RAM_VECTOR_TABLE=1
        -DgKBD_KeysCount_c=2
)

if (OT_RCP_FLOW_CONTROL)
    target_compile_definitions(nxp-k32w061-driver
        PUBLIC
            -DgUartHwFlowControl_d
    )
endif()

target_compile_options(nxp-k32w061-driver
    PRIVATE
        ${OT_CFLAGS}
        -Wno-unknown-pragmas
        -Wno-sign-compare
        -Wno-unused-function
        -Wno-unused-parameter
        -Wno-empty-body
        -Wno-missing-field-initializers
        -Wno-clobbered
        -fno-strict-aliasing
    PUBLIC
        -imacros "${PROJECT_SOURCE_DIR}/src/k32w0/k32w061/k32w061-sdk-config.h"
)

if (SDK_RELEASE)
    target_include_directories(nxp-k32w061-driver
        PUBLIC
            ${PROJECT_SOURCE_DIR}/third_party/k32w061_sdk/mr5_fixes
            $ENV{NXP_K32W0_SDK_ROOT}/boards/k32w061dk6/wireless_examples/openthread/reed/bm
            $ENV{NXP_K32W0_SDK_ROOT}/components/serial_manager
            $ENV{NXP_K32W0_SDK_ROOT}/components/lists
            $ENV{NXP_K32W0_SDK_ROOT}/CMSIS/Include
            $ENV{NXP_K32W0_SDK_ROOT}/components/uart
            $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061
            $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers
            $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/utilities/str
            $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/utilities/debug_console
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/mbedtls/port/ksdk
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/ieee-802.15.4/uMac/Include
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/Common/
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/FunctionLib/
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/Flash/Internal
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/Lists
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/MemManager/Interface
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/OSAbstraction/Interface
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/Panic/Interface/
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/PDM/Include
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/SerialManager/Interface
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/SerialManager/Source
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/TimersManager/Interface
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/TimersManager/Source
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/XCVR/DK6/Build/Include
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/XCVR/DK6
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/RNG/Interface
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/GPIO
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/Keyboard/Interface
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/MWSCoexistence/Interface
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/Messaging/Interface
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/ModuleInfo
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/SecLib
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/LowPower/Interface/k32w061dk6
    )
    else()
    target_include_directories(nxp-k32w061-driver
        PUBLIC
            ${PROJECT_SOURCE_DIR}/third_party/k32w061_sdk/mr5_fixes
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/openthread/boards/dk6_k32w061/reed/bm
            $ENV{NXP_K32W0_SDK_ROOT}/boards/k32w061dk6/utility
            $ENV{NXP_K32W0_SDK_ROOT}/components/serial_manager
            $ENV{NXP_K32W0_SDK_ROOT}/CMSIS/Include
            $ENV{NXP_K32W0_SDK_ROOT}/components/uart
            $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061
            $ENV{NXP_K32W0_SDK_ROOT}/devices/K32W061/drivers
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/mbedtls/port/ksdk
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/ieee-802.15.4/uMac/Include
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/Common/
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/FunctionLib/
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/Flash/Internal
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/Lists
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/MemManager/Interface
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/OSAbstraction/Interface
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/Panic/Interface/
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/PDM/Include
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/SerialManager/Interface
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/SerialManager/Source
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/TimersManager/Interface
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/TimersManager/Source
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/XCVR/DK6/Build/Include
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/XCVR/DK6
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/RNG/Interface
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/GPIO
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/Keyboard/Interface
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/boards/dk6
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/ModuleInfo
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/SecLib
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/Messaging/Interface
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/LowPower/Interface/jn518x
            $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/MWSCoexistence/Interface
            $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/common
            $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/aes
            $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/sha
            $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/jn_iocon
            $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/lpc_gpio
            $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/flexcomm
            $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/jn_flash
            $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/jn_rng
            $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/fmeas
            $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/inputmux
            $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/jn_rtc
            $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/ctimer
            $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/pint
            $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/spifi
            $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/lpc_dma
            $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/lpc_adc
            $ENV{NXP_K32W0_SDK_ROOT}/platform/drivers/wwdt
            $ENV{NXP_K32W0_SDK_ROOT}/platform/utilities/debug_console
            $ENV{NXP_K32W0_SDK_ROOT}/platform/utilities/debug_console/str
    )
endif()

set(SDK_K32W0_LIBS
    $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/PDM/Library/libPDM.a
    $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/XCVR/lib/libRadio.a
    $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/framework/SecLib/lib_crypto_m4.a
    $ENV{NXP_K32W0_SDK_ROOT}/middleware/wireless/ieee-802.15.4/lib/libMiniMac.a
)

#Disable ble support
target_compile_definitions(nxp-k32w061-driver
    PRIVATE
        gSupportBle=0
        SUPPORT_FOR_BLE=0
)

if (OT_APP_CLI OR OT_APP_RCP OR OT_APP_NCP)
    #Allows to not use the OSA main function
    target_compile_definitions(nxp-k32w061-driver
        PRIVATE
            osCustomStartup=1
    )
endif()

target_link_libraries(nxp-k32w061-driver PUBLIC ${SDK_K32W0_LIBS})
