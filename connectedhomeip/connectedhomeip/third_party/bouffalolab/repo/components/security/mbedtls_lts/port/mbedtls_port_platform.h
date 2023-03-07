#ifndef MBEDTLS_PORT_PLATFORM_H_WLIVX3GZ
#define MBEDTLS_PORT_PLATFORM_H_WLIVX3GZ
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>

#define MBEDTLS_PLATFORM_STD_FREE vPortFree
#define MBEDTLS_PLATFORM_STD_CALLOC mbedtls_port_calloc

#define MBEDTLS_PLATFORM_FPRINTF_MACRO fprintf
#define MBEDTLS_PLATFORM_PRINTF_MACRO printf
#define MBEDTLS_PLATFORM_SNPRINTF_MACRO snprintf
#define MBEDTLS_PLATFORM_EXIT_MACRO exit

void *mbedtls_port_calloc( size_t nmemb, size_t size );

#endif /* end of include guard: MBEDTLS_PORT_PLATFORM_H_WLIVX3GZ */
