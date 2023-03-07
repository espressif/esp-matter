/*
 * Copyright (c) 2019-2021, Qorvo Inc
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
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
 * $Header$
 * $Change$
 * $DateTime$
 *
 */
/**
 * @file
 * @brief defines math helper functions + clk enable/disable
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */

#include "cryptolib_internal.h"
#include "cryptolib_def.h"
#include <stddef.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
#include "cryptodma.h"
#include "sxregs.h"
#include "sx_generic.h"

#include "hal.h"
#include "gpHal.h"

const block_t NULL_blk = {NULL, 0, BLOCK_S_INCR_ADDR | DMA_AXI_DESCR_DISCARD};

static bool rng_startup_failed = false;

HAL_CRITICAL_SECTION_DEF(sx_usage_lock);

void sx_math_u64_to_u8array(uint64_t in, uint8_t *out, uint32_t little_endian)
{
   UInt32 i = 0;
   if(little_endian){
      for(i = 0; i < 8; i++) out[i] = (in >>  8*i)&0xFF;
   }
   else {
      for(i = 0; i < 8; i++) out[7-i] = (in >>  8*i)&0xFF;
   }
}

block_t sx_getNULL_blk(void)
{
   return NULL_blk;
}

bool sx_rng_startup_failed(void)
{
   return rng_startup_failed;
}

void sx_rng_set_startup_failed(bool fail)
{
   rng_startup_failed = fail;
}

#if !defined(GP_DIVERSITY_GPHAL_K8E)
#error "Silex clock on/off needs to be defined for this chip version"
#endif

void sx_enable_clock(void)
{
#ifdef HAL_MUTEX_SUPPORTED
    // Create mutex first time enable was called
    if(!HAL_VALID_MUTEX(sx_usage_lock))
    {
        HAL_CREATE_MUTEX(&sx_usage_lock);
    }
    HAL_ACQUIRE_MUTEX(sx_usage_lock);
#endif //HAL_MUTEX_SUPPORTED
    // make sure clock is disabled before enabling
    GP_ASSERT_SYSTEM(GP_WB_READ_SEC_PROC_CTRL_SEC_PROC_ENABLE() == 0);

    GP_WB_WRITE_SEC_PROC_CTRL_SEC_PROC_ENABLE(1);
}

void sx_disable_clock(void)
{
    // Make sure clock was enabled before disabling
    GP_ASSERT_SYSTEM(GP_WB_READ_SEC_PROC_CTRL_SEC_PROC_ENABLE() == 1);

    GP_WB_WRITE_SEC_PROC_CTRL_SEC_PROC_ENABLE(0);

#ifdef HAL_MUTEX_SUPPORTED
    GP_ASSERT_SYSTEM(HAL_VALID_MUTEX(sx_usage_lock));
    HAL_RELEASE_MUTEX(sx_usage_lock);
#endif //HAL_MUTEX_SUPPORTED
}
