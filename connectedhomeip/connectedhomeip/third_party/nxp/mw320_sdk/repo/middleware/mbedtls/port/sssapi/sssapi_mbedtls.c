/*
 * Copyright 2019-2020 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "sssapi_mbedtls.h"
#include "fsl_common.h"
#include "fsl_snt.h"

sss_sscp_key_store_t g_keyStore;
sss_sscp_session_t g_sssSession;
sscp_context_t g_sscpContext;
static uint32_t g_isCryptoHWInitialized = 0;

/******************************************************************************/
/******************** CRYPTO_InitHardware **************************************/
/******************************************************************************/
/*!
 * @brief Application init for various Crypto blocks.
 *
 * This function is provided to be called by MCUXpresso SDK applications.
 * It calls basic init for Crypto Hw acceleration and Hw entropy modules.
 */
void CRYPTO_InitHardware(void)
{
    if (!g_isCryptoHWInitialized)
    {
        SNT_loadFwLocal(S3MUA);
        if (sscp_mu_init(&g_sscpContext, (MU_Type*)S3MUA) != kStatus_SSCP_Success)
        {
        }
        else if (sss_sscp_open_session(&g_sssSession, kType_SSS_Sentinel300, &g_sscpContext, 0u, NULL) !=
                 kStatus_SSS_Success)
        {
        }
        else if (sss_sscp_key_store_context_init(&g_keyStore, &g_sssSession) != kStatus_SSS_Success)
        {
        }
        else if (sss_sscp_key_store_allocate(&g_keyStore, 0u) != kStatus_SSS_Success)
        {
        }
        g_isCryptoHWInitialized = 1;
    }
}