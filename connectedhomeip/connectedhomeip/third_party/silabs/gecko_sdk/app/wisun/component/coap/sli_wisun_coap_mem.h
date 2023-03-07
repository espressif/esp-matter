/***************************************************************************//**
 * @file
 * @brief Wi-SUN CoAP memory handler module
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef __SL_WISUN_COAP_MEM_H__
#define __SL_WISUN_COAP_MEM_H__

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <inttypes.h>
#include <stddef.h>
#include "sl_wisun_coap_config.h"
#include "sl_component_catalog.h"

/**************************************************************************//**
 * @defgroup SL_WISUN_COAP_MEMORY Memory Handler
 * @ingroup SL_WISUN_COAP
 * @{
 *****************************************************************************/

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// Memory pool setup
#if SL_WISUN_COAP_MEM_USE_STATIC_MEMORY_POOL

/// Count of static memory pool options
#define WISUN_COAP_MEMORY_OPTION_COUNT     (4U)

/// Very low memory request ID
#define WISUN_COAP_MEMORY_VERY_LOW_ID      (1000U)
/// Very low memory request size
#define WISUN_COAP_MEMORY_VERY_LOW_SIZE    (32U)
/// Very low memory request max count
#define WISUN_COAP_MEMORY_VERY_LOW_COUNT   (30U)

/// Low memory request ID
#define WISUN_COAP_MEMORY_LOW_ID           (1001U)
/// Low memory requests size
#define WISUN_COAP_MEMORY_LOW_SIZE         (64U)
/// Low memory requests max count
#define WISUN_COAP_MEMORY_LOW_COUNT        (15U)

/// Medium memory request ID
#define WISUN_COAP_MEMORY_MEDIUM_ID        (1002U)
/// Medium memory request size
#define WISUN_COAP_MEMORY_MEDIUM_SIZE      (128U)
/// Medium memory request max count
#define WISUN_COAP_MEMORY_MEDIUM_COUNT     (5U)

/// High memory requests ID
#define WISUN_COAP_MEMORY_HIGH_ID          (1003U)
/// High memory requests size
#define WISUN_COAP_MEMORY_HIGH_SIZE        (256U)
/// High memory requests max count
#define WISUN_COAP_MEMORY_HIGH_COUNT       (2U)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Initialize static memory pools
 * @details
 *****************************************************************************/
void _wisun_coap_mem_init(void);

#else

// Choose headers for os provided malloc & free
#if defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)
# include "FreeRTOS.h"
#else
# include "sl_malloc.h"
#endif

#endif

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Wi-SUN CoAP malloc for internal usage
 * @details Not thread safe. Only used in sl_wisun_coap.c
 * @param size size to alloc
 * @return void* ptr to the allocated memory, on error NULL
 *****************************************************************************/
void *_wisun_coap_mem_malloc(size_t size);

/**************************************************************************//**
 * @brief Wi-SUN CoAP free memory for internal usage
 * @details Not thread safe. Only used in sl_wisun_coap.c
 * @param addr address
 *****************************************************************************/
void _wisun_coap_mem_free(void *addr);

/** @}*/

#ifdef __cplusplus
}
#endif

#endif  // end __SL_WISUN_COAP_MEM_H__
