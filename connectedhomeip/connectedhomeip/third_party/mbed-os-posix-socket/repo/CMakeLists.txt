# Copyright (c) 2020 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(mbed-os-posix-socket INTERFACE)

target_include_directories(mbed-os-posix-socket
    INTERFACE
        .
        posix/include
        net/include
)

target_sources(mbed-os-posix-socket
    INTERFACE
        net/src/BSDSocket.cpp
        net/src/net_common.cpp
        net/src/net_dns.cpp
        net/src/net_if.cpp
        net/src/net_socket.cpp
        posix/src/eventfd.cpp
        posix/src/if.cpp
        posix/src/ifaddrs.cpp
        posix/src/inet.cpp
        posix/src/ioctl.cpp
        posix/src/netdb.cpp
        posix/src/select.cpp
        posix/src/socket.cpp
)

target_link_libraries(mbed-os-posix-socket 
    INTERFACE
        mbed-netsocket
)
