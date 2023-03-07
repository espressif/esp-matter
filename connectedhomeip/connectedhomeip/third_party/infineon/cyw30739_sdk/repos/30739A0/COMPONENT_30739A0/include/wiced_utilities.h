/**
 * @cond
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 * @endcond
 *
 * @file
 * This file constains general utilities of AIROC BTSDK.
 */
#pragma once

/**
 * @addtogroup helper Generic Helper Macros
 * @ingroup wicedsys
 * @{
 */

/** Macro to get the minimum from 2 arguments */
#ifndef MIN
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif

/** Macro to get the maximum from 2 arguments */
#ifndef MAX
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

#ifndef _countof
/** Determine the number of elements in an array. */
#define _countof(x) (sizeof(x) / sizeof(x[0]))
#endif

/** Macro to reverse bytes in a 16-bit value */
#define WICED_SWAP16(val) \
    ((uint16_t)((((uint16_t)(val) & (uint16_t)0x00ffU) << 8) | \
        (((uint16_t)(val) & (uint16_t)0xff00U) >> 8)))

/** Macro to reverse bytes in a 32-bit value */
#define WICED_SWAP32(val) \
    ((uint32_t)((((uint32_t)(val) & (uint32_t)0x000000ffU) << 24) | \
        (((uint32_t)(val) & (uint32_t)0x0000ff00U) <<  8) | \
        (((uint32_t)(val) & (uint32_t)0x00ff0000U) >>  8) | \
        (((uint32_t)(val) & (uint32_t)0xff000000U) >> 24)))

/** Macro to reverse bytes in a 64-bit value */
#define WICED_SWAP64(val) \
    ((uint64_t)( \
        (((uint64_t)(val) & (uint64_t)0x00000000000000ffllU) << 56) | \
        (((uint64_t)(val) & (uint64_t)0x000000000000ff00llU) << 40) | \
        (((uint64_t)(val) & (uint64_t)0x0000000000ff0000llU) << 24) | \
        (((uint64_t)(val) & (uint64_t)0x00000000ff000000llU) <<  8) | \
        (((uint64_t)(val) & (uint64_t)0x000000ff00000000llU) >>  8) | \
        (((uint64_t)(val) & (uint64_t)0x0000ff0000000000llU) >> 24) | \
        (((uint64_t)(val) & (uint64_t)0x00ff000000000000llU) >> 40) | \
        (((uint64_t)(val) & (uint64_t)0xff00000000000000llU) >> 56)))

/** Load 16-bit value from unaligned big-endian byte array */
#define WICED_NTOH16(val) \
    ((uint16_t)( \
        ((uint16_t)((val)[0]) << 8) | \
        ((uint16_t)((val)[1]))))

/** Load 32-bit value from unaligned big-endian byte array */
#define WICED_NTOH32(val) \
    ((uint32_t)( \
        ((uint32_t)((val)[0]) << 24) | \
        ((uint32_t)((val)[1]) << 16) | \
        ((uint32_t)((val)[2]) <<  8) | \
        ((uint32_t)((val)[3]))))

/** Load 64-bit value from unaligned big-endian byte array */
#define WICED_NTOH64(val) \
    ((uint64_t)( \
        ((uint64_t)((val)[0]) << 56) | \
        ((uint64_t)((val)[1]) << 48) | \
        ((uint64_t)((val)[2]) << 40) | \
        ((uint64_t)((val)[3]) << 32) | \
        ((uint64_t)((val)[4]) << 24) | \
        ((uint64_t)((val)[5]) << 16) | \
        ((uint64_t)((val)[6]) <<  8) | \
        ((uint64_t)((val)[7]))))

/** @} */
