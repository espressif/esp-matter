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

#ifndef _GPPD_PBM_H_
#define _GPPD_PBM_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
typedef struct gpPd_RxAttributes_s {
    gpPd_Rssi_t rssi;
    gpPd_Lqi_t  lqi;
} gpPd_RxAttributes_t;

typedef struct gpPd_TxAttributes_s {
    UInt8 lastChannel;
} gpPd_TxAttributes_t;

typedef struct {
    gpPd_BufferType_t type;
    union {
        gpPd_RxAttributes_t rx;
        gpPd_TxAttributes_t txcfm;
    }attr;
} gpPd_Descriptor_t;

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#endif // _GPPD_PBM_H_


