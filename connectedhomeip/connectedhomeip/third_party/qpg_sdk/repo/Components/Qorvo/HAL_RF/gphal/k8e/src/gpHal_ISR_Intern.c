/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 *
 *  The file gpHal.h contains the general functions of the HAL (init, reset, interrupts).
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
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#include "gpHal.h"
#include "gpHal_DEFS.h"

//GP hardware dependent register definitions
#include "gpHal_HW.h"          //Containing all uC dependent implementations
#include "gpHal_reg.h"
#include "gpLog.h"
#include "hal_WB.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Inline Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/


void gpHal_Interrupt(void)
{
#if defined(GP_COMP_GPHAL_MAC) || defined(GP_COMP_GPHAL_BLE)
    if (GP_WB_READ_INT_CTRL_UNMASKED_PHY_INTERRUPT())
    {
        gpHal_PHYInterrupt();
    }
#endif //defined(GP_COMP_GPHAL_MAC) || defined(GP_COMP_GPHAL_BLE)

#if defined(GP_COMP_GPHAL_MAC) || defined(GP_COMP_GPHAL_BLE)
    while (GP_WB_READ_RCI_UNMASKED_INTERRUPTS() & GPHAL_ISR_RCI_LP_ISR_UNMASKED)
    {
        gpHal_RCIInterrupt();
    }
#endif //defined(GP_COMP_GPHAL_MAC) || defined(GP_COMP_GPHAL_BLE)

#ifdef GP_COMP_GPHAL_BLE
    while(GP_WB_READ_IPC_UNMASKED_GPM2X_INTERRUPTS() & GPHAL_ISR_IPC_GPM_LP_ISR_UNMASKED)
    {
        gpHal_IpcGpm2XInterrupt();
    }
#endif //GP_COMP_GPHAL_BLE

#if defined(GP_COMP_GPHAL_ES_EXT_EVENT) || defined(GP_COMP_GPHAL_ES_ABS_EVENT) || defined(GP_COMP_GPHAL_ES_REL_EVENT)
    while (GP_WB_READ_INT_CTRL_UNMASKED_ES_INTERRUPT())
    {
        gpHal_ESInterrupt();
    }
#endif //defined(GP_COMP_GPHAL_ES_EXT_EVENT) || defined(GP_COMP_GPHAL_ES_ABS_EVENT) || defined(GP_COMP_GPHAL_ES_REL_EVENT)
}
