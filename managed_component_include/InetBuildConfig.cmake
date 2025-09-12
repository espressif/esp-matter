# Generating inet/InetBuildConfig.h
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/inet/InetBuildConfig.h
"#pragma once\n
#include <sdkconfig.h>\n

#ifdef CONFIG_BUILD_CHIP_TESTS
#define INET_CONFIG_TEST 1
#else
#define INET_CONFIG_TEST 0
#endif

#ifdef CONFIG_DISABLE_IPV4
#define INET_CONFIG_ENABLE_IPV4 0
#else
#define INET_CONFIG_ENABLE_IPV4 1
#endif

#define INET_CONFIG_ENABLE_TCP_ENDPOINT 0
#define INET_CONFIG_ENABLE_UDP_ENDPOINT 1
#define HAVE_LWIP_RAW_BIND_NETIF 1

#ifndef CONFIG_CHIP_ENABLE_EXTERNAL_PLATFORM
#define INET_PLATFORM_CONFIG_INCLUDE <platform/ESP32/InetPlatformConfig.h>
#endif

#define INET_TCP_END_POINT_IMPL_CONFIG_FILE <inet/TCPEndPointImplLwIP.h>
#define INET_UDP_END_POINT_IMPL_CONFIG_FILE <inet/UDPEndPointImplLwIP.h>
")
