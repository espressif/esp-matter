/*
 * Copyright (c) 2021, Qorvo Inc
 *
 * gpHal_RadioMgmt.h
 *  Radio claim management for different modules in NRT.
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
 * $Header:
 * $Change:
 * $DateTime:
 *
 */

#ifndef _GPHAL_RADIOMGMT_H_
#define _GPHAL_RADIOMGMT_H_


#ifdef GP_DIVERSITY_GPHAL_RADIO_MGMT_SUPPORTED
#endif

/* Maximum number of callbacks that can be registered with radioMgmt module */
#define GPHAL_RADIOMGMT_MAXUSERS 3

#define gpHalRadioMgmt_success 1
#define gpHalRadioMgmt_fail 0
typedef UInt8 gpHalRadioMgmt_status;

/* Call back to be called once radio is granted */
typedef void(*radioMgmtUserCallBack)(void);

void gpHalRadioMgmt_init(void);

/* check if radio granted or raise new claims if pending users */
void gpHalRadioMgmt_check(void);

/* API to be called to see if chip can go to  sleep */
UInt8 gpHalRadioMgmt_isSleepAllowed(void);

/* Claim radio, asynchronously (non-blocking wait for grant). Callback cb is called once radio is granted */
/* Callback function needs to release radio before it exits */
/* If cb == NULL, claim is restarted if there are any pending claims - useful to restart claim once claim is 
released to unblock a blocking claim by different interface (e.g. by ZB) */
gpHalRadioMgmt_status gpHalRadioMgmtAsynch_claimRadio(radioMgmtUserCallBack cb);

/* Release radio after claiming asynchronously. Goes together with gpHalRadioMgmtAsynch_claimRadio() */
void gpHalRadioMgmtAsynch_releaseRadio(void);

/* Claim radio, synchronously (blocking) wait for grant. */
/* NOTE: Following blocking API calls are expected to be used only for test modes or during Init stage */
void gpHalRadioMgmtSynch_claimRadio(void);

/* Release radio after claiming synchronously. Goes together with gpHalRadioMgmtSynch_claimRadio() */
void gpHalRadioMgmtSynch_releaseRadio(void);

/* Check if radio is granted to radio management  */
Bool radioMgmt_IsRadioGranted(void);

/* Check if radio is claimed by radio management */
Bool radioMgmt_IsRadioClaimed(void);

#endif /* _GPHAL_RADIOMGMT_H_ */
