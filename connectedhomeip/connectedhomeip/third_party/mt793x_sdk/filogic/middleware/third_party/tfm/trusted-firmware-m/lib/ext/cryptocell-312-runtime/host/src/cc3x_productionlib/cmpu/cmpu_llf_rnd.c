/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/************* Include Files ****************/
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "cc_pal_log.h"
#include "cc_prod_error.h"
#include "prod_hw_defs.h"
#include "cmpu_llf_rnd.h"
#include "dx_rng.h"

#include "cc_rng_plat.h"
#include "llf_rnd_trng.h"
#include "cc_general_defs.h"
#include "cc_hal_plat.h"

extern unsigned long gCcRegBase;

extern uint32_t *pRndWorkBuff;

/*******************************************************************************/
/**
 * @brief The CC_PROD_LLF_RND_GetTrngSource reads random source of needed size from TRNG.
 *
 *        The function is used in Self, Instantiation and Reseeding functions.
 *
  * @ppSourceOut[out] - The pointer to to pointer to the entropy source buffer.
 *                   The buffer contains one empty word for using by CRYS level
 *                   and then buffer for output the rng source.
 * @pSourceOutSize[out] - The pointer to the size in bytes of entropy source
 *                      in - required size, output - actual size.
 *
 * @return uint32_t - On success 0 is returned, otherwise indicates failure.
 */
uint32_t CC_PROD_LLF_RND_GetTrngSource(uint32_t           **ppSourceOut, /*out*/
        uint32_t           *pSourceOutSize,
                                       uint32_t *pRndWorkBuff)
{
        uint32_t error = 0;
        CCRndState_t rndState;
        CCRndParams_t  trngParams;

        error = RNG_PLAT_SetUserRngParameters( &trngParams);
        if (error != CC_OK) {
                return error;
        }

        error = LLF_RND_RunTrngStartupTest(&rndState, &trngParams, pRndWorkBuff);
        if (error != CC_OK) {
                return error;
        }

        error = LLF_RND_GetTrngSource(&rndState, &trngParams, CC_FALSE, NULL, ppSourceOut, pSourceOutSize, pRndWorkBuff, false);
        return error;
}


uint32_t CC_PROD_LLF_RND_VerifyGeneration(uint8_t *pBuff)
{
        CC_UNUSED_PARAM(pBuff);
        return CC_OK;

} /* End of setUserRngParameters */
