/*
 * Copyright (c) 2014, 2016, GreenPeak Technologies
 * Copyright (c) 2017-2018, Qorvo Inc
 *
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

#ifndef _GPPD_RAM_H_
#define _GPPD_RAM_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#ifndef GP_PD_BUFFER_SIZE
#define GP_PD_BUFFER_SIZE           0x80
#endif //GP_PD_BUFFER_SIZE

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
typedef struct gpPd_RxAttributes_s {
    UInt32      rxTimestamp;
    UInt32      rxTimestampChip;
    gpPd_Rssi_t rssi;
    gpPd_Lqi_t  lqi;
    UInt8       rxChannel;
    UInt8       txAckKeyId;
    UInt16      txAckFc;
    UInt32      txAckFrameCounter;
} gpPd_RxAttributes_t;

typedef struct gpPd_TxCfmAttributes_s {
    UInt32      txTimestamp;
    UInt8       lastChannel;
    UInt8       txRetryCntr;
    UInt8       framePendingFromTxPbm;
    UInt8       txCCACntr;
    gpPd_Lqi_t  ackLqi;
    Bool        rxEnhancedAck;
} gpPd_TxCfmAttributes_t;

typedef struct {
    gpPd_BufferType_t type;
    union {
        gpPd_RxAttributes_t rx;
        gpPd_TxCfmAttributes_t txcfm;
    }attr;
    UInt8 buffer[GP_PD_BUFFER_SIZE];
} gpPd_Descriptor_t;


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/



#endif // _GPPD_RAM_H_


