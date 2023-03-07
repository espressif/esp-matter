/***************************************************************************//**
 * @file
 * @brief Private header for EZSP Host functions
 *
 * This file should be included only by ezsp-host-ui.c and ash-host.c.
 *
 * See @ref ezsp_util for documentation.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

/** @addtogroup ezsp_util
 *
 * See ezsp-host-priv.h.
 *
 *@{
 */

#ifndef SILABS_EZSP_HOST_PRIV_H
#define SILABS_EZSP_HOST_PRIV_H

// Defined in ezsp-host-ui.c
void ezspPrintElapsedTime(void);
void ezspTraceEzspFrameId(const char *message, uint8_t *ezspFrame);
void ezspTraceEzspVerbose(char *format, ...);

#ifdef EZSP_ASH
  #include "app/ezsp-host/ash/ash-host-priv.h"
#endif

#endif //__EZSP_HOST_PRIV_H___

/** @} END addtogroup
 */
