# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if(IOTSDK_LWIP)
    add_subdirectory(lwip)
    if(TARGET tfm-ns-interface)
        message(WARNING "The EMAC interface used by LwIP can only run in secure mode. \
                Running it in non secure mode will cause a secure fault.")
    endif()
endif()

add_subdirectory(ip-network)

add_subdirectory(sockets)
