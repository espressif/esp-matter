/***************************************************************************//**
 * @file
 * @brief Private header for EZSP ASH Host functions
 *
 * This file should be included only by ezsp-host-ui.c, ash-host-ui.c
 * and ash-host.c.
 *
 * See @ref ash_util for documentation.
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

/** @addtogroup ash_util
 *
 * See ash-host-priv.h.
 *
 *@{
 */

#ifndef SILABS_ASH_HOST_PRIV_H
#define SILABS_ASH_HOST_PRIV_H

// Defined in ash-host-ui.c
void ashTraceFrame(bool sent);
void ashTraceEventRecdFrame(const char *string);
void ashTraceEventTime(const char *string);
void ashTraceDisconnected(uint8_t error);
void ashTraceArray(uint8_t *name, uint8_t len, uint8_t *data);
void ashCountFrame(bool sent);

// Defined in ash-host.c
uint8_t readTxControl(void);
uint8_t readRxControl(void);
uint8_t readAckRx(void);
uint8_t readAckTx(void);
uint8_t readFrmTx(void);
uint8_t readFrmReTx(void);
uint8_t readFrmRx(void);
uint8_t readAshTimeouts(void);

#endif //__ASH_HOST_PRIV_H___

/** @} END addtogroup
 */
