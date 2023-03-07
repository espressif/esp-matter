/*
 * mbed Microcontroller Library
 * Copyright (c) 2019-2020 Cypress Semiconductor Corporation
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * \file    crypto_common.c
 * \version 1.2
 *
 * \brief   Source file for common mbedtls acceleration functions
 *
 */

#include <string.h>

#include "crypto_common.h"
#include "cy_crypto_common.h"
#include "cy_crypto_core_hw.h"
#include "cy_crypto_core_mem.h"

#define CY_CRYPTO_CHECK_PARAM(cond)         do {                \
                                                if( !(cond) )   \
                                                    return;     \
                                            } while( 0 )

#if defined(CY_USING_HAL) && !defined(CY_CRYPTO_HAL_DISABLE)

typedef cy_rslt_t cy_cmgr_rslt_t;

#define CY_CMGR_RSLT_ERR_INUSE          CYHAL_HWMGR_RSLT_ERR_INUSE
#define CY_CMGR_RSLT_SUCCESS            CY_RSLT_SUCCESS

#define cy_cmgr_crypto_reserve(...)     cyhal_crypto_reserve(__VA_ARGS__)
#define cy_cmgr_crypto_free(...)        cyhal_crypto_free(__VA_ARGS__)

#else /* defined(CY_USING_HAL) && !defined(CY_CRYPTO_HAL_DISABLE) */

typedef uint32_t cy_cmgr_rslt_t;

#define CY_CMGR_RSLT_ERR_INUSE          (0x1001L)
#define CY_CMGR_RSLT_SUCCESS            (0L)

/** Block count for CRYPTO blocks */
#define CY_CMGR_CRYPTO_INST_COUNT       CY_IP_MXCRYPTO_INSTANCES

static CRYPTO_Type* CY_GMGR_CRYPTO_BASE_ADDR[CY_CMGR_CRYPTO_INST_COUNT] =
{
    CRYPTO,
};

// Number of Crypto features
#define CY_CMGR_CRYPTO_FEATURES_NUM        ((uint32_t)CY_CMGR_CRYPTO_COMMON + 1u)

// Defines for maximum available features in Crypto block
#define CY_CMGR_CRYPTO_FEATURE_CRC_MAX_VAL         (1u)
#define CY_CMGR_CRYPTO_FEATURE_TRNG_MAX_VAL        (1u)
#define CY_CMGR_CRYPTO_FEATURE_VU_MAX_VAL          (256u)
#define CY_CMGR_CRYPTO_FEATURE_COMMON_MAX_VAL      (256u)

static uint16_t cy_cmgr_crypto_features[CY_CMGR_CRYPTO_INST_COUNT][CY_CMGR_CRYPTO_FEATURES_NUM] = {{0}};
static uint16_t cy_cmgr_crypto_features_max_val[CY_CMGR_CRYPTO_FEATURES_NUM] = {CY_CMGR_CRYPTO_FEATURE_CRC_MAX_VAL,
                                                                     CY_CMGR_CRYPTO_FEATURE_TRNG_MAX_VAL,
                                                                     CY_CMGR_CRYPTO_FEATURE_VU_MAX_VAL,
                                                                     CY_CMGR_CRYPTO_FEATURE_COMMON_MAX_VAL};

static bool cy_cmgr_crypto_is_enabled(uint32_t instance_num)
{
    uint8_t reserved = (cy_cmgr_crypto_features[instance_num][CY_CMGR_CRYPTO_CRC]  |
                        cy_cmgr_crypto_features[instance_num][CY_CMGR_CRYPTO_TRNG] |
                        cy_cmgr_crypto_features[instance_num][CY_CMGR_CRYPTO_VU]   |
                        cy_cmgr_crypto_features[instance_num][CY_CMGR_CRYPTO_COMMON]);

    return (reserved != 0);
}

static cy_cmgr_rslt_t cy_cmgr_crypto_reserve(CRYPTO_Type** base, cy_cmgr_resource_inst_t *resource, cy_cmgr_feature_t feature)
{
    cy_cmgr_rslt_t result = CY_CMGR_RSLT_ERR_INUSE;

    for (uint32_t i = 0u; i < CY_CMGR_CRYPTO_INST_COUNT; i++)
    {
        if (cy_cmgr_crypto_features[i][feature] < cy_cmgr_crypto_features_max_val[feature])
        {
            resource->type = CY_CMGR_RSC_CRYPTO;
            resource->block_num = i;
            *base = CY_GMGR_CRYPTO_BASE_ADDR[i];

            result = CY_CMGR_RSLT_SUCCESS;

            //Enable block if this as this first feature that is reserved in block
            if (!cy_cmgr_crypto_is_enabled(i))
            {
                Cy_Crypto_Core_Enable(*base);
            }

            if(result == CY_CMGR_RSLT_SUCCESS)
            {
                ++cy_cmgr_crypto_features[i][feature];
                break;
            }
        }
    }
    return result;
}

static void cy_cmgr_crypto_free(CRYPTO_Type* base, cy_cmgr_resource_inst_t *resource, cy_cmgr_feature_t feature)
{
    if (cy_cmgr_crypto_features[resource->block_num][feature] != 0)
    {
        --cy_cmgr_crypto_features[resource->block_num][feature];
    }

    //If this was the last feature then free the underlying crypto block as well.
    if (!cy_cmgr_crypto_is_enabled(resource->block_num))
    {
        if (Cy_Crypto_Core_IsEnabled(base))
        {
            Cy_Crypto_Core_Disable(base);
        }

        resource->type = CY_CMGR_RSC_INVALID;
    }
}
#endif /* defined(CY_USING_HAL) && !defined(CY_CRYPTO_HAL_DISABLE) */

/*******************************************************************************
*       Crypto object manage functions
*******************************************************************************/
bool cy_hw_crypto_reserve(cy_hw_crypto_t *obj, cy_cmgr_feature_t feature)
{
    cy_cmgr_rslt_t status;
    CY_ASSERT( obj != NULL );

    status = cy_cmgr_crypto_reserve(&(obj->base), &(obj->resource), feature);
    if (CY_CMGR_RSLT_SUCCESS == status)
    {
        obj->feature = feature;
    }

    return (CY_CMGR_RSLT_SUCCESS == status);
}

void cy_hw_crypto_release(cy_hw_crypto_t *obj)
{
    CY_ASSERT( obj != NULL );
    if (obj->resource.type == CY_CMGR_RSC_CRYPTO)
    {
        cy_cmgr_crypto_free(obj->base, &(obj->resource), obj->feature);
    }
}

void cy_hw_zeroize(void *data, uint32_t dataSize)
{
    cy_hw_crypto_t obj;
    CY_CRYPTO_CHECK_PARAM( data != NULL );
    CY_CRYPTO_CHECK_PARAM( dataSize > 0u );

    if (cy_hw_crypto_reserve(&obj, CY_CMGR_CRYPTO_COMMON))
    {
        Cy_Crypto_Core_MemSet(obj.base, data, 0u, (uint16_t)dataSize);
    }
    cy_hw_crypto_release(&obj);
}

void cy_hw_sha_init(void *ctx, uint32_t ctxSize)
{
    CY_CRYPTO_CHECK_PARAM( ctx != NULL );
    CY_CRYPTO_CHECK_PARAM( ctxSize > 0u );

    cy_hw_zeroize(ctx, ctxSize);

    (void)cy_hw_crypto_reserve((cy_hw_crypto_t *)ctx, CY_CMGR_CRYPTO_COMMON);
}

void cy_hw_sha_free(void *ctx, uint32_t ctxSize)
{
    CY_CRYPTO_CHECK_PARAM( ctx != NULL );
    CY_CRYPTO_CHECK_PARAM( ctxSize > 0u);

    cy_hw_crypto_release((cy_hw_crypto_t *)ctx);
    cy_hw_zeroize(ctx, ctxSize);
}

int cy_hw_sha_start(cy_hw_crypto_t *obj, cy_stc_crypto_sha_state_t *hashState,
                    cy_en_crypto_sha_mode_t shaMode, void *shaBuffers)
{
    cy_en_crypto_status_t status;

    if ((hashState == NULL) || (shaBuffers == NULL))
        return (-1);

    status = Cy_Crypto_Core_Sha_Init(obj->base, hashState, shaMode, shaBuffers);
    if (CY_CRYPTO_SUCCESS != status)
        return (-1);

    status = Cy_Crypto_Core_Sha_Start(obj->base, hashState);
    if (CY_CRYPTO_SUCCESS != status)
        return (-1);

    return (0);
}

int cy_hw_sha_update(cy_hw_crypto_t *obj, cy_stc_crypto_sha_state_t *hashState,
                     const uint8_t *in, uint32_t inlen)
{
    cy_en_crypto_status_t status;

    if ((hashState == NULL) || (in == NULL))
        return (-1);

    if (hashState->blockSize == 0)
        return (-1);

    status = Cy_Crypto_Core_Sha_Update(obj->base, hashState, in, inlen);
    if (CY_CRYPTO_SUCCESS != status)
        return (-1);

    return (0);
}

int cy_hw_sha_finish(cy_hw_crypto_t *obj, cy_stc_crypto_sha_state_t *hashState, uint8_t *output)
{
    cy_en_crypto_status_t status;

    if ((hashState == NULL) || (output == NULL))
        return (-1);

    if (hashState->blockSize == 0)
        return (-1);

    status = Cy_Crypto_Core_Sha_Finish(obj->base, hashState, output);
    if (CY_CRYPTO_SUCCESS != status)
        return (-1);

    return (0);
}

int cy_hw_sha_process(cy_hw_crypto_t *obj, cy_stc_crypto_sha_state_t *hashState, const uint8_t *in)
{
    cy_en_crypto_status_t status;

    status = Cy_Crypto_Core_Sha_Update(obj->base, hashState, in, hashState->blockSize);
    if (CY_CRYPTO_SUCCESS != status)
        return (-1);

    return (0);
}

void cy_hw_sha_clone( void *ctxDst, const void *ctxSrc, uint32_t ctxSize,
                     cy_stc_crypto_sha_state_t *hashStateDst, void *shaBuffersDst)
{
    CY_CRYPTO_CHECK_PARAM( ctxDst != NULL );
    CY_CRYPTO_CHECK_PARAM( ctxSrc != NULL );
    CY_CRYPTO_CHECK_PARAM( hashStateDst != NULL );
    CY_CRYPTO_CHECK_PARAM( shaBuffersDst != NULL );

    Cy_Crypto_Core_MemCpy(((cy_hw_crypto_t *)ctxSrc)->base, ctxDst, ctxSrc, (uint16_t)ctxSize);
    Cy_Crypto_Core_Sha_Init(((cy_hw_crypto_t *)ctxSrc)->base, hashStateDst, (cy_en_crypto_sha_mode_t)hashStateDst->mode, shaBuffersDst);
}
