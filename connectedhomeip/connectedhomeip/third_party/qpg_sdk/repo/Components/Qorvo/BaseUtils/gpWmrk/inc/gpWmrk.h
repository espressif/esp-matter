
/*
 * Copyright (c) 2012, 2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * gpWmrk.h
 *
 * This file defines the watermarker component api
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


#ifndef _GPWMRK_H_
#define _GPWMRK_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include <global.h>

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
/// @brief The maximum amount tallies to be monitored (can be overruled at compilation time).
#ifndef GP_WMRK_MAX_ENTRIES
#define GP_WMRK_MAX_ENTRIES          6
#endif
/// @brief The interval to generate a log which shows all subscribed tallies (can be overruled at compilation time).
#ifndef GP_WMRK_INTERVAL
#define GP_WMRK_INTERVAL                    60000000
#endif
/// @brief The maximum # chars to define a unique id.
#define GP_WMRK_MAX_UID_LENGTH             3

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/** @typedef gpWmrk_Parameters_t
 *  @brief The gpWmrk_Parameters_t defines the parameters required for the watermarker
*/
typedef struct gpWmrk_Parameters {
    UIntPtr  softLimit;
    UIntPtr  deadLimit;
    UIntPtr  value;
    char    uid[GP_WMRK_MAX_UID_LENGTH];    // unique id
    Bool    up;                             // up or down counter
} gpWmrk_Parameters_t;

/** @typedef gmrk_Id_t
 *  @brief The gpWmrk_Id_t defines the id used by the Watermarker, FAILURE equals 0xFF
 * When a counter needs to be manipulated, the very same Id needs to be passed as argument
 *

*/
typedef UInt8 gpWmrk_Id_t;

/// @brief Value returned in error situations.
#define GP_WMRK_FAILURE 0xFF

/**
 * @file gpWmrk.h
 *
 *
 * @defgroup GEN_WMRK General Watermarker functions
 * @brief The general Watermarker functionality is implemented in these functions
 *
 * @note When a counter needs to go towards its limit gpWmrk_CntrIncr should be used despite it's direction
 */


#define gpWmrk_Init()
#define gpWmrk_Subscribe(params)         0
#define gpWmrk_CntrToLimit(id)
#define gpWmrk_CntrFromLimit(id)
#define gpWmrk_CntrSet(id, val)


#endif // _GPWMRK_H_


