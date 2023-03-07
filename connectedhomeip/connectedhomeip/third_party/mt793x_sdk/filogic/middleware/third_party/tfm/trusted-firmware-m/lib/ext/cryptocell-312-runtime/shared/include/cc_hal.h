/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef __CC_HAL_H__
#define __CC_HAL_H__

/*!
@file
@brief This file contains HAL definitions and APIs.
*/

#include <stdint.h>
#include "cc_hal_plat.h"
#include "cc_pal_types_plat.h"

/*! HAL return code definitions. */
typedef enum {
    CC_HAL_OK = 0,
    CC_HAL_ENODEV,        /* Device not opened or does not exist */
    CC_HAL_EINTERNAL,     /* Internal driver error (check system log) */
    CC_HAL_MAPFAILED,
    CC_HAL_ENOTSUP,       /* Unsupported function/option */
    CC_HAL_ENOPERM,       /* Not enough permissions for request */
    CC_HAL_EINVAL,        /* Invalid parameters */
    CC_HAL_ENORSC,        /* No resources available (e.g., memory) */
    CC_HAL_RESERVE32B = 0x7FFFFFFFL
} CCHalRetCode_t;

/*!
 * @brief This function is used to map ARM TrustZone CryptoCell TEE registers to Host virtual address space.
    It is called by ::CC_LibInit, and returns a non-zero value in case of failure.
        The existing implementation supports Linux environment. In case virtual addressing is not used, the function can be minimized to contain only the
    following line, and return OK:
        gCcRegBase = (uint32_t)DX_BASE_CC;
  @return CC_HAL_OK on success.
  @return A non-zero value in case of failure.
*/
int CC_HalInit(void);


/*!
 * @brief This function is used to wait for the IRR interrupt signal.
 *
 * @return CCError_t - return CC_OK upon success
 */
CCError_t CC_HalWaitInterrupt(uint32_t data /*!< [in] The interrupt bits to wait upon. */ );

/*!
 * @brief This function is used to wait for the IRR interrupt signal.
 * The existing implementation performs a "busy wait" on the IRR.
 *
 * @return CCError_t - return CC_OK upon success
 */
CCError_t CC_HalWaitInterruptRND(uint32_t data);

/*!
 * @brief This function clears the DSCRPTR_COMPLETION bit in the ICR signal.
 */
void CC_HalClearInterrupt(uint32_t data);

/*!
 * @brief This function is called by CC_LibInit and is used for initializing the ARM TrustZone CryptoCell TEE cache settings registers.
          The existing implementation sets the registers to their default values in HCCC cache coherency mode
      (ARCACHE = 0x2, AWCACHE = 0x7, AWCACHE_LAST = 0x7).
          These values should be changed by the customer in case the customer's platform requires different HCCC values, or in case SCCC is needed
      (the values for SCCC are ARCACHE = 0x3, AWCACHE = 0x3, AWCACHE_LAST = 0x3).

 * @return void
 */
void CC_HalInitHWCacheParams(void);

/*!
 * @brief This function is used to unmap ARM TrustZone CryptoCell TEE registers' virtual address.
      It is called by CC_LibFini, and returns a non-zero value in case of failure.
      In case virtual addressing is not used, the function can be minimized to be an empty function returning OK.
   @return CC_HAL_OK on success.
   @return A non-zero value in case of failure.
 */
int CC_HalTerminate(void);

/*!
 * @brief This function is used to clear the interrupt vector.

 * @return void.
 */
void CC_HalClearInterruptBit(uint32_t data /*!< [in] The interrupt bits to clear. */);

/*!
 * @brief This function is used to mask IRR interrupts.

 * @return void.
 */
void CC_HalMaskInterrupt(uint32_t data /*!< [in] The interrupt bits to mask. */);


#endif

