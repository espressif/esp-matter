/*
 * Copyright (c) 2008-2010, 2012-2016, GreenPeak Technologies
 * Copyright (c) 2017-2018, Qorvo Inc
 *
 * gpHal_global.h
 *
 *  Contains general definitions used in the different blocks.
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
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _GPHAL_GLOBAL_H_
#define _GPHAL_GLOBAL_H_

/** @file gp_global.h
 *  The file gp_global.h contains general definitions used in the different blocks. The result enumeration for the gphal functions and different callbacks can be found here.
 *
 *  @brief Contains general definitions used in the different blocks.
*/

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#ifdef GP_COMP_GPHAL_MAC
#include "gpPd.h"
#endif //GP_COMP_GPHAL_MAC

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/** @name gpHal_Result_t*/
//@{
/** @brief The function returned successful. */
#define gpHal_ResultSuccess             0x0
/** @brief An invalid parameter was given as a parameter to this function. */
#define gpHal_ResultInvalidParameter    0x5
/** @brief The GP chip is in receive mode. */
#define gpHal_ResultRxOn                0x6
/** @brief The GP chip is busy. */
#define gpHal_ResultBusy                0x7
/** @brief The GP chip radio is off. */
#define gpHal_ResultTrxOff              0x8
/** @brief The GP chip radio is transmitting. */
#define gpHal_ResultTxOn                0x9
/** @brief The GP chip has timed out waiting for the COEX Grant signal. */
#define gpHal_ResultGrantTimeout        0xa
/** @brief The GP chip unsupported operation*/
#define gpHal_ResultUnsupported         0xb
/** @brief The request was invalid (event not present, ...)*/
#define gpHal_ResultInvalidRequest      0xc2
/** @brief The handle given as parameter cannot be found. */
#define gpHal_ResultInvalidHandle       0xe7
/** @brief Channel access failure. */
#define gpHal_ResultCCAFailure          0xe1
/** @brief Ack was required but no ack received. */
#define gpHal_ResultNoAck               0xe9
/** @typedef gpHal_Result_t
 *  @brief The gpHal_Result_t type defines the result of various HAL functions.
*/
typedef UInt8 gpHal_Result_t;
//@}

/** @typedef gpHal_AbsoluteEventCallback_t
 *  @brief   The gpHal_AbsoluteEventCallback_t callback type definition defines the callback prototype of an Absolute Event interrupt. */
typedef void (*gpHal_AbsoluteEventCallback_t)(void);

/** @typedef gpHal_ExternalEventCallback_t
 *  @brief   The gpHal_ExternalEventCallback_t callback typedef defines the callback prototype of the External Event interrupt. */
typedef void (*gpHal_ExternalEventCallback_t)(void);

typedef void (*gpHal_LowBatteryCallback_t)(void);

/** @name gpHal_RxInfo_t */
//@{
typedef struct {
/** @brief The channel the packet was received on */
    UInt8 rxChannel;
/** @typedef gpHal_RxInfo_t
 *  @brief The gpHal_MultiChannelOptions_t struct contains some info about the rx of a packet.
*/
} gpHal_RxInfo_t;
//@}

#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpHal_CodeJumpTableFlash_Defs_gpglobal.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */

#ifdef GP_COMP_GPHAL_MAC

/**
 * @brief This callback indicates the reception of a data.
 *
 * This callback is called when a data packet has been received.
 *
 * This callback has to be implemented by the software layer that is using the GPHAL (only required
 * when using the fixed callbacks).
 *
 * @param pdLoh     Packet descriptor structure that contains length, offset and unique handle of the received packet.
 * @param rxInfo    Additional information about the rx of the packet.
*/
GP_API void gpHal_cbDataIndication(gpPd_Loh_t pdLoh, gpHal_RxInfo_t *rxInfo);

/**
 * @brief This callback indicates the completion of a data transmission.
 *
 * This callback is called when a data transmission has been completed.  The transmission has been
 * initiated by calling the gpHal_DataRequest() function.
 *
 * This callback has to be implemented by the software layer that is using the GPHAL (only required
 * when using the fixed callbacks).
 *
 * @param pdLoh             Packet descriptor structure that contains length, offset
                            and unique handle of the transmitted packet.
 * @param status            The status of the data transfer.
 * @param lastChannelUsed   The channel where the packet has been sent.
 *
*/
GP_API void gpHal_cbDataConfirm(gpHal_Result_t status, gpPd_Loh_t pdLoh, UInt8 lastChannelUsed);

/**
 * @brief This callback indicates the reception of a commabd data request request.
 *
 * It's goal is to set the FP bit correctly in the Ack
 *
 * This callback has to be implemented by the software layer that is using the GPHAL (only required
 * when using the fixed callbacks).
 *
*/
GP_API void gpHal_cbCmdDataRequest(void);

/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */
/**
 * @brief This callback indicates the completion of an Energy detection (ED).
 *
 * This callback is called when an energy detection (ED) has been completed.
 *
 * This callback has to be implemented by the software layer that is using the GPHAL (only required
 * when using the fixed callbacks).
 *
 * @param channelMask The measured channels.
 * @param protoED    The raw energy level measured.  This value needs to be converted by gpHal_CalculateED()
 *                   in order to get the real energy level.
*/
GP_API void gpHal_cbEDConfirm(UInt16 channelMask, UInt8 *protoED);
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */

/**
 * @brief This callback indicates that a timed trigger for a data transmission has failed.
 *
 * This callback is called when a timed transmission for a data transmission has failed due
 * to a busy state of the transmitter.  This can occur for instance, when the transmitter is
 * transmitting a acknowledge frame at the moment the transmission triggers gets fired.
 *
 * When this occurs, the pending data packet in the transmission queue needs to be handled
 * by the software (by clearing the pending packet or by rescheduling a new trigger).
 *
 * This callback has to be implemented by the software layer that is using the GPHAL (only required
 * when using the fixed callbacks).
 *
*/
GP_API void gpHal_cbBusyTX(void);

/**
 * @brief This callback indicates the transmission queue was empty, when a timed transmission was triggered.
 *
 * This callback is called when the transmission queue is empty and a timed transmission should be started.
 *
 * This callback has to be implemented by the software layer that is using the GPHAL (only required
 * when using the fixed callbacks).
 *
*/
GP_API void gpHal_cbEmptyQueue(void);
#endif //GP_COMP_GPHAL_MAC

/**
 * @brief This callback is called when an absolute event has occured.
 *
 * This callback is called when an absolute event has occured.
 *
 * This callback has to be implemented by the software layer that is using the GPHAL (only required
 * when using the fixed callbacks).
 *
*/
GP_API void gpHal_cbExternalEvent(void);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

#ifdef __cplusplus
}
#endif
#endif  /* _GPHAL_GLOBAL_H_ */

