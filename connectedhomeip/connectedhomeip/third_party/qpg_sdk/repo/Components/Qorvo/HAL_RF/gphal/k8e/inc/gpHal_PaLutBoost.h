/*
 * Copyright (c) 2021, Qorvo Inc
 *
 * !!! This is a generated file. !!!
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
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
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#include "gpHal.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

//wrappers/stubs for RAP code
#define abs(a)              (((a) < 0) ? -(a) : (a))

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

/*
 * The actual RAP methods.
 */

#define RAP_TX_PA_NORMAL (0x00)
#define RAP_TX_PA_LOW (0x40)
#define RAP_TX_PA_ULTRALOW (0x80)

#define RAP_TX_PA_LUT_OFFSET (-24)

static const UInt8 rap_tx_pa_lut[] = {
     (RAP_TX_PA_NORMAL   | 1 )
    ,(RAP_TX_PA_NORMAL   | 1 )
    ,(RAP_TX_PA_NORMAL   | 1 )
    ,(RAP_TX_PA_NORMAL   | 1 )
    ,(RAP_TX_PA_NORMAL   | 1 )
    ,(RAP_TX_PA_NORMAL   | 1 )
    ,(RAP_TX_PA_NORMAL   | 2 )
    ,(RAP_TX_PA_NORMAL   | 2 )
    ,(RAP_TX_PA_NORMAL   | 2 )
    ,(RAP_TX_PA_NORMAL   | 3 )
    ,(RAP_TX_PA_NORMAL   | 3 )
    ,(RAP_TX_PA_NORMAL   | 3 )
    ,(RAP_TX_PA_NORMAL   | 4 )
    ,(RAP_TX_PA_NORMAL   | 4 )
    ,(RAP_TX_PA_NORMAL   | 5 )
    ,(RAP_TX_PA_NORMAL   | 6 )
    ,(RAP_TX_PA_NORMAL   | 6 )
    ,(RAP_TX_PA_NORMAL   | 7 )
    ,(RAP_TX_PA_NORMAL   | 8 )
    ,(RAP_TX_PA_NORMAL   | 9 )
    ,(RAP_TX_PA_NORMAL   | 10)
    ,(RAP_TX_PA_NORMAL   | 11)
    ,(RAP_TX_PA_NORMAL   | 12)

    ,(RAP_TX_PA_NORMAL   | 13)
    ,(RAP_TX_PA_NORMAL   | 14)
    ,(RAP_TX_PA_NORMAL   | 16)
    ,(RAP_TX_PA_NORMAL   | 18)
    ,(RAP_TX_PA_NORMAL   | 20)
    ,(RAP_TX_PA_NORMAL   | 23)
    ,(RAP_TX_PA_NORMAL   | 26)
    ,(RAP_TX_PA_NORMAL   | 30)
    ,(RAP_TX_PA_NORMAL   | 35)
    ,(RAP_TX_PA_NORMAL   | 42)
    ,(RAP_TX_PA_NORMAL   | 54)
    ,(RAP_TX_PA_NORMAL   | 63)
};

