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

target_compile_definitions(ot-config INTERFACE
    "MBEDTLS_USER_CONFIG_FILE=\"qpg6095-mbedtls-config.h\""
)

list(APPEND OT_PUBLIC_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/crypto"
)
set(OT_PUBLIC_INCLUDES ${OT_PUBLIC_INCLUDES} PARENT_SCOPE)

if(OT_CFLAGS MATCHES "-pedantic-errors")
    string(REPLACE "-pedantic-errors" "" OT_CFLAGS "${OT_CFLAGS}")
endif()

if(OT_CFLAGS MATCHES "-Wno-c\\+\\+14-compat")
    string(REPLACE "-Wno-c++14-compat" "" OT_CFLAGS "${OT_CFLAGS}")
endif()

add_library(openthread-qpg6095-ftd
    alarm.c
    diag.c
    entropy.c
    logging.c
    misc.c
    platform.c
    radio.c
    settings.cpp
    uart.c
)

set_target_properties(
    openthread-qpg6095-ftd
    PROPERTIES
        C_STANDARD 99
        CXX_STANDARD 11
)

target_link_libraries(openthread-qpg6095-ftd
    PRIVATE
        -Wl,--start-group
        qpg6095-driver-ftd
        ${OT_MBEDTLS}
        ot-config
        -Wl,--end-group
    PUBLIC
        -T${SDK_DIR}/qpg6095/ld/qpg6095.ld
        -nostdlib
        -Wl,--gc-sections
        -Wl,-Map=$<TARGET_PROPERTY:NAME>.map,--cref
)

target_compile_definitions(openthread-qpg6095-ftd
    PUBLIC
        ${OT_PLATFORM_DEFINES}
)

target_compile_options(openthread-qpg6095-ftd
    PRIVATE
        ${OT_CFLAGS}
)

target_include_directories(openthread-qpg6095-ftd
    PRIVATE
        ${OT_PUBLIC_INCLUDES}
        ${OPENTHREAD_DIR}/examples/platforms
)


add_library(openthread-qpg6095-mtd
    alarm.c
    diag.c
    entropy.c
    logging.c
    misc.c
    platform.c
    radio.c
    settings.cpp
    uart.c
)

set_target_properties(
    openthread-qpg6095-mtd
    PROPERTIES
        C_STANDARD 99
        CXX_STANDARD 11
)

target_link_libraries(openthread-qpg6095-mtd
    PRIVATE
        -Wl,--start-group
        qpg6095-driver-mtd
        ${OT_MBEDTLS}
        ot-config
        -Wl,--end-group
    PUBLIC
        -T${SDK_DIR}/qpg6095/ld/qpg6095.ld
        -nostdlib
        -Wl,--gc-sections
        -Wl,-Map=$<TARGET_PROPERTY:NAME>.map,--cref
)

target_compile_definitions(openthread-qpg6095-mtd
    PUBLIC
        ${OT_PLATFORM_DEFINES}
)

target_compile_options(openthread-qpg6095-mtd
    PRIVATE
        ${OT_CFLAGS}
)

target_include_directories(openthread-qpg6095-mtd
    PRIVATE
        ${OT_PUBLIC_INCLUDES}
        ${OPENTHREAD_DIR}/examples/platforms
)

target_include_directories(ot-config INTERFACE ${OT_PUBLIC_INCLUDES})
target_compile_definitions(ot-config INTERFACE ${OT_PLATFORM_DEFINES})
