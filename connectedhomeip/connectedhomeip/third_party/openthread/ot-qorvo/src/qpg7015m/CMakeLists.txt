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

list(APPEND OT_PLATFORM_DEFINES
    "_BSD_SOURCE=1"
    "_DEFAULT_SOURCE=1"
)

target_compile_definitions(ot-config INTERFACE
    "MBEDTLS_USER_CONFIG_FILE=\"qpg7015m-mbedtls-config.h\""
)

list(APPEND OT_PUBLIC_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/crypto"
)

set(OT_PLATFORM_DEFINES ${OT_PLATFORM_DEFINES} PARENT_SCOPE)

set(OT_PUBLIC_INCLUDES ${OT_PUBLIC_INCLUDES} PARENT_SCOPE)

if(OT_CFLAGS MATCHES "-pedantic-errors")
    string(REPLACE "-pedantic-errors" "" OT_CFLAGS "${OT_CFLAGS}")
endif()

if(OT_CFLAGS MATCHES "-Wno-c\\+\\+14-compat")
    string(REPLACE "-Wno-c++14-compat" "" OT_CFLAGS "${OT_CFLAGS}")
endif()

set(PLATFORM_SOURCES
    alarm.c
    diag.c
    entropy.c
    flash.c
    logging.c
    misc.c
    radio.c
    system.c
)

if(OT_QORVO_SOCKET)
    list(APPEND PLATFORM_SOURCES
        uart-socket.c
    )
else()
    list(APPEND PLATFORM_SOURCES
        uart-posix.c
    )
endif()

add_library(openthread-qpg7015m-ftd
    STATIC
    ${PLATFORM_SOURCES}
)

set_target_properties(
    openthread-qpg7015m-ftd
    PROPERTIES
        C_STANDARD 99
        CXX_STANDARD 11
)

target_link_libraries(openthread-qpg7015m-ftd
    PRIVATE
        -Wl,--start-group
        qpg7015m-driver-ftd
        ${OT_MBEDTLS}
        ot-config
        -Wl,--end-group
    PUBLIC
        -lrt
        -pthread
        -Wl,--gc-sections
        -Wl,-Map=$<TARGET_PROPERTY:NAME>.map
)

target_compile_definitions(openthread-qpg7015m-ftd
    PUBLIC
        ${OT_PLATFORM_DEFINES}
)

target_compile_options(openthread-qpg7015m-ftd
    PRIVATE
        ${OT_CFLAGS}
)

target_include_directories(openthread-qpg7015m-ftd
    PRIVATE
        ${OT_PUBLIC_INCLUDES}
        ${OPENTHREAD_DIR}/examples/platforms
)

add_library(openthread-qpg7015m-mtd
    STATIC
    ${PLATFORM_SOURCES}
)

set_target_properties(
    openthread-qpg7015m-mtd
    PROPERTIES
        C_STANDARD 99
        CXX_STANDARD 11
)

target_link_libraries(openthread-qpg7015m-mtd
    PRIVATE
        -Wl,--start-group
        qpg7015m-driver-mtd
        ${OT_MBEDTLS}
        ot-config
        -Wl,--end-group
    PUBLIC
        -lrt
        -pthread
        -Wl,--gc-sections
        -Wl,-Map=$<TARGET_PROPERTY:NAME>.map
)

target_compile_definitions(openthread-qpg7015m-mtd
    PUBLIC
        ${OT_PLATFORM_DEFINES}
)

target_compile_options(openthread-qpg7015m-mtd
    PRIVATE
        ${OT_CFLAGS}
)

target_include_directories(openthread-qpg7015m-mtd
    PRIVATE
        ${OT_PUBLIC_INCLUDES}
        ${OPENTHREAD_DIR}/examples/platforms
)

target_include_directories(ot-config INTERFACE ${OT_PUBLIC_INCLUDES})
target_compile_definitions(ot-config INTERFACE ${OT_PLATFORM_DEFINES})
