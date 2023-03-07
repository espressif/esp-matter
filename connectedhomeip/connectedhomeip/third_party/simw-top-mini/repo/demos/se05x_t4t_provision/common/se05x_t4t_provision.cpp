/*
 *
 * Copyright 2022 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

/* ************************************************************************** */
/* Includes                                                                   */
/* ************************************************************************** */

#include <errno.h>
#include <ex_sss.h>
#include <ex_sss_boot.h>
#include <nxEnsure.h>
#include <nxLog_App.h>
#include <stdio.h>
#include <string.h>
#include <se05x_APDU.h>

#if SSS_HAVE_APPLET_SE051_H

ex_sss_boot_ctx_t gex_sss_chip_ctx;

#define NDEF_FILE_ID \
    {              \
        0xE1, 0x01 \
    }

#define PROVISION_DATA "MT:-24J0YXE00KA0648G00"

void se05x_t4t_provision(void)
{
  sss_status_t status = kStatus_SSS_Success;
  const char *portName = nullptr;
  smStatus_t smStatus       = SM_NOT_OK;
  sss_se05x_session_t *pCtx = NULL;
  uint8_t ndeffileId[2]     = NDEF_FILE_ID;
  size_t ndeffileIdLen      = sizeof(ndeffileId);

  uint8_t provisionData[]     = PROVISION_DATA;
  size_t provisionDatalen      = sizeof(provisionData);

  memset(&gex_sss_chip_ctx, 0, sizeof(gex_sss_chip_ctx));

  status = ex_sss_boot_connectstring(0, NULL, &portName);
  if (kStatus_SSS_Success != status) {
    printf("se05x error: %s\n", "ex_sss_boot_connectstring failed");
    return;
  }

  status = ex_sss_boot_open(&gex_sss_chip_ctx, portName);
  if (kStatus_SSS_Success != status) {
    printf("se05x error: %s\n", "ex_sss_boot_open failed");
    return;
  }

  status = ex_sss_key_store_and_object_init(&gex_sss_chip_ctx);
  if (kStatus_SSS_Success != status) {
    printf("se05x error: %s\n", "ex_sss_key_store_and_object_init failed");
    return;
  }

  pCtx = (sss_se05x_session_t *)&gex_sss_chip_ctx.session;

  smStatus = Se05x_T4T_API_SelectT4TApplet(&(pCtx->s_ctx));
  if (SM_OK != smStatus) {
    printf("se05x error: %s\n", "Se05x_T4T_API_SelectT4TApplet failed");
    return;
  }

  smStatus = Se05x_T4T_API_ConfigureAccessCtrl(
      &(pCtx->s_ctx), kSE05x_T4T_Interface_Contactless, kSE05x_T4T_Operation_Write, kSE05x_T4T_AccessCtrl_Granted);
  if (SM_OK != smStatus) {
    printf("se05x error: %s\n", "Se05x_T4T_API_ConfigureAccessCtrl failed");
    return;
  }

  smStatus = Se05x_T4T_API_SelectFile(&(pCtx->s_ctx), ndeffileId, ndeffileIdLen);
  if (SM_OK != smStatus) {
    printf("se05x error: %s\n", "NDEF Se05x_T4T_API_SelectFile failed");
    return;
  }

  smStatus = Se05x_T4T_API_UpdateBinary(&(pCtx->s_ctx), provisionData, provisionDatalen);
  if (SM_OK != smStatus) {
    printf("se05x error: %s\n", "Se05x_T4T_API_UpdateBinary failed");
    return;
  }

  smStatus = Se05x_T4T_API_ConfigureAccessCtrl(
      &(pCtx->s_ctx), kSE05x_T4T_Interface_Contactless, kSE05x_T4T_Operation_Write, kSE05x_T4T_AccessCtrl_Denied);
  if (SM_OK != smStatus) {
    printf("se05x error: %s\n", "Se05x_T4T_API_ConfigureAccessCtrl failed");
    return;
  }

  printf("T4T Provision successful \n");
  return;
}

#else

void se05x_t4t_provision(void)
{
  printf("T4T provision can be dome only with SE051H sample \n");
  return;
}
#endif
