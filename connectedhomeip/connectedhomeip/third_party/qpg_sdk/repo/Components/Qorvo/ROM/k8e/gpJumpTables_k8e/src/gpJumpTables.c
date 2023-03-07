/*
 * Copyright (c) 2021, Qorvo Inc
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


#define GP_COMPONENT_ID GP_COMPONENT_ID_JUMPTABLES_K8E

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpJumpTables.h"
#include "gpHal_reg.h"
#include "gpLog.h"

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

void gpJumpTables_Init(void)
{
    if(!gpHal_FlashRemap_IsFlashSwapped())
    {
        /* map virtual window linearly onto the actual flash */
        GP_WB_WRITE_CORTEXM4_FLASH_VIRT_WINDOW_0_OFFSET(0);
        GP_WB_WRITE_CORTEXM4_FLASH_VIRT_WINDOW_1_OFFSET(1);
        GP_WB_WRITE_CORTEXM4_FLASH_VIRT_WINDOW_2_OFFSET(2);
        GP_WB_WRITE_CORTEXM4_FLASH_VIRT_WINDOW_3_OFFSET(3);
        GP_WB_WRITE_CORTEXM4_FLASH_VIRT_WINDOW_4_OFFSET(4);
        GP_WB_WRITE_CORTEXM4_FLASH_VIRT_WINDOW_5_OFFSET(5);
        GP_WB_WRITE_CORTEXM4_FLASH_VIRT_WINDOW_6_OFFSET(6);
        GP_WB_WRITE_CORTEXM4_FLASH_VIRT_WINDOW_7_OFFSET(7);
    }
    else
    {
#if !defined(GP_HAL_EXPECTED_CHIP_EMULATED)
        UInt8 base_offset = 128;
#else
        UInt8 base_offset = 64;
#endif
        /* map virtual window linearly onto the second half of the actual flash */
        GP_WB_WRITE_CORTEXM4_FLASH_VIRT_WINDOW_0_OFFSET(base_offset+0);
        GP_WB_WRITE_CORTEXM4_FLASH_VIRT_WINDOW_1_OFFSET(base_offset+1);
        GP_WB_WRITE_CORTEXM4_FLASH_VIRT_WINDOW_2_OFFSET(base_offset+2);
        GP_WB_WRITE_CORTEXM4_FLASH_VIRT_WINDOW_3_OFFSET(base_offset+3);
        GP_WB_WRITE_CORTEXM4_FLASH_VIRT_WINDOW_4_OFFSET(base_offset+4);
        GP_WB_WRITE_CORTEXM4_FLASH_VIRT_WINDOW_5_OFFSET(base_offset+5);
        GP_WB_WRITE_CORTEXM4_FLASH_VIRT_WINDOW_6_OFFSET(base_offset+6);
        GP_WB_WRITE_CORTEXM4_FLASH_VIRT_WINDOW_7_OFFSET(base_offset+7);

    }

}


