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

set(UART_BAUD_RATE "115200" CACHE STRING "Uart baud rate")

target_compile_definitions(ot-config INTERFACE
    "MBEDTLS_USER_CONFIG_FILE=\"k32w061-mbedtls-config.h\""
    "MBEDTLS_CONFIG_FILE=\"mbedtls-config.h\"" 
)

set(OT_PUBLIC_INCLUDES ${OT_PUBLIC_INCLUDES} PARENT_SCOPE)

add_library(openthread-k32w061
    ${K32W0_COMM_SOURCES}
    $<TARGET_OBJECTS:openthread-platform-utils>
)

set_target_properties(openthread-k32w061
    PROPERTIES
        C_STANDARD 99
        CXX_STANDARD 11
)

target_link_libraries(openthread-k32w061
    PUBLIC
        -Wl,--start-group nxp-k32w061-driver mbedtls -Wl,--end-group
        -Wl,--gc-sections
        -Wl,-Map=$<TARGET_PROPERTY:NAME>.map,-print-memory-usage
    PRIVATE
        ot-config
)

if (OT_APP_CLI OR OT_APP_RCP OR OT_APP_NCP)
    target_link_libraries(openthread-k32w061
        PUBLIC
            -T${PROJECT_SOURCE_DIR}/src/k32w0/k32w061/k32w061.ld
    )
    target_compile_definitions(openthread-k32w061
        PRIVATE
            #For openthread stack application board initialization is done in the ot platform layer
            OT_PLAT_BOARD_INIT
    )
endif ()

#Openthread libs need to have openthread platform dependencies
target_link_libraries(ot-config
    INTERFACE
        openthread-k32w061
)

target_compile_definitions(openthread-k32w061
    PUBLIC
        ${OT_PLATFORM_DEFINES}
        OT_PLAT_UART_BAUD_RATE=${UART_BAUD_RATE}
)

target_include_directories(openthread-k32w061
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/k32w061
    PRIVATE
        ${K32W0_INCLUDES}
        ${OT_PUBLIC_INCLUDES}
)

target_include_directories(ot-config INTERFACE ${OT_PUBLIC_INCLUDES})
