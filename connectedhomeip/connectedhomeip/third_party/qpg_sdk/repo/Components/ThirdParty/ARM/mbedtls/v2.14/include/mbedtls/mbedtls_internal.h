/*
 * Copyright (c) 2012-2014, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * This file should be included by all mbedtls_alt implementation (*.c) files.
 * This file should NOT be included by application code or by any public mbedtls_alt header (*.h) files.
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright law
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header:
 * $Change:
 * $DateTime:
 *
 */


#ifndef MBEDTLS_INTERNAL_H
#define MBEDTLS_INTERNAL_H

#include "gpAssert.h"
#include "gpLog.h"

#define GP_COMPONENT_ID                         GP_COMPONENT_ID_TLS

#define MBEDTLS_PLATFORM_PRINTF_MACRO(fmt, ...) do { \
                                                  GP_LOG_SYSTEM_PRINTF(fmt, 0, ##__VA_ARGS__); \
                                                  gpLog_Flush(); \
                                                  } while(0)
#define MBEDTLS_PLATFORM_ASSERT(cond, msg)      GP_ASSERT_DEV_INT(cond)
/* Mapping cryptolib asserts used in mbedtls_alt to MBEDTLS assert */
#define CRYPTOLIB_ASSERT                        MBEDTLS_PLATFORM_ASSERT
#define CRYPTOLIB_ASSERT_NM(cond)               MBEDTLS_PLATFORM_ASSERT(cond, "")


#endif /* MBEDTLS_ALT_INTERNAL_H */
