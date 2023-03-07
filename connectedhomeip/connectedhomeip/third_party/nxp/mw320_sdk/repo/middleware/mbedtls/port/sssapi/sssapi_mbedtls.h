/*
 * Copyright 2019-2020 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SSSAPI_MBEDTLS_H
#define SSSAPI_MBEDTLS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "fsl_sss_mgmt.h"
#include "fsl_sss_sscp.h"
#include "fsl_sscp_mu.h"

extern sss_sscp_key_store_t g_keyStore;
extern sss_sscp_session_t g_sssSession;
extern sscp_context_t g_sscpContext;
extern uint32_t g_isCryptoHWInitialized;

int fsl_mbedtls_printf(const char *fmt_s, ...);
void CRYPTO_InitHardware(void);

#ifdef __cplusplus
}
#endif

#endif /* SSSAPI_MBEDTLS_H */
