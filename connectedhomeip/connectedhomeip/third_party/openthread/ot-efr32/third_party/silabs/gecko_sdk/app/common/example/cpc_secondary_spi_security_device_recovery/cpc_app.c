/***************************************************************************//**
 * @file cpc_app.c
 * @brief Secondary firmware for the CPC sample application
 * @version 1.0.0
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "sl_cpc.h"
#include "sl_cpc_security.h"

/*******************************************************************************
 ************************  GLOBAL FUNCTIONS   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize CPC application
 ******************************************************************************/
void cpc_app_init(void)
{
  sl_cpc_security_unbind();
}

/***************************************************************************//**
 * Re-implementation of weak function sl_cpc_security_on_unbind_request()
 ******************************************************************************/
uint64_t sl_cpc_security_on_unbind_request(bool is_link_encrypted)
{
  (void) is_link_encrypted;

  return SL_CPC_SECURITY_OK_TO_UNBIND;
}

/***************************************************************************//**
 * baremetal process action
 ******************************************************************************/
void cpc_app_process_action(void)
{
}
