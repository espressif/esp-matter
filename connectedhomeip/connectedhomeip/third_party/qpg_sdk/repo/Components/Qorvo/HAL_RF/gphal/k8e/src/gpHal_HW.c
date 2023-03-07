/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * gphal_HW.c
 *
 *  The file gpHal.h contains functions to access HW registers.
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

//GP hardware dependent register definitions
#include "gpHal.h"          //Containing all uC dependent implementations
#include "gpLog.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

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
static gpHal_Address_t gpHalHw_UncompressWishBoneAddress(gpHal_Address_t Register)
{
    gpHal_Address_t uncompressedReg;

    if ((Register >= GP_MM_RAM_ADDR_TO_COMPRESSED(GP_MM_RAM_START))
     && (Register < GP_MM_RAM_ADDR_TO_COMPRESSED(GP_MM_RAM_END)))
    {
        uncompressedReg = GP_MM_RAM_ADDR_FROM_COMPRESSED(Register);
    }
    else if ((Register >= GP_MM_FLASH_ADDR_TO_COMPRESSED(GP_MM_FLASH_LINEAR_START))
     && (Register < GP_MM_FLASH_ADDR_TO_COMPRESSED(GP_MM_FLASH_NVR_CFG_RECORD_END)))
    {
        uncompressedReg = GP_MM_FLASH_ADDR_FROM_COMPRESSED(Register);
    }
    else if ((Register >= GP_MM_WISHB_ADDR_TO_COMPRESSED(GP_MM_WISHB_START))
     && (Register < GP_MM_WISHB_ADDR_TO_COMPRESSED(GP_MM_WISHB_END)))
    {
        uncompressedReg = GP_MM_WISHB_ADDR_FROM_COMPRESSED(Register);
    }
    else
    {
        uncompressedReg = Register;
    }

    return uncompressedReg;
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

//-------------------------------------------------------------------------------------------------------
//  GENERAL FUNCTIONS
//-------------------------------------------------------------------------------------------------------

//Function starts from a raw compressed wishbone address or a raw address
//No defines can be used here as they are already uncompressed

void gpHal_WriteReg(gpHal_Address_t Register, UInt8 Data)
{
    GP_HAL_WRITE_REG(gpHalHw_UncompressWishBoneAddress(Register), Data);
}

UInt8 gpHal_ReadReg(gpHal_Address_t Register)
{
    return GP_HAL_READ_REG(gpHalHw_UncompressWishBoneAddress(Register));
}

void gpHal_ReadModifyWriteReg(gpHal_Address_t Register, UInt8 Mask, UInt8 Data)
{
    GP_HAL_READMODIFYWRITE_REG(gpHalHw_UncompressWishBoneAddress(Register),Mask,Data);
}

void gpHal_ReadRegs(gpHal_Address_t Address, void* pBuffer, UInt8 Length)
{
    GP_HAL_READ_BYTE_STREAM(gpHalHw_UncompressWishBoneAddress(Address), pBuffer, Length );
}

void gpHal_WriteRegs(gpHal_Address_t Address, void* pBuffer, UInt8 Length)
{
    GP_HAL_WRITE_BYTE_STREAM(gpHalHw_UncompressWishBoneAddress(Address), pBuffer, Length );
}

Bool gpHal_wb_memcmp(const UInt8 *ptr0, UIntPtr addr, UInt8 length)
{
    UInt8 idx = 0;
    while (idx<length)
    {
        if (GP_WB_READ_U8(addr+idx) != ptr0[idx])
        {
            return 1;
        }
        ++idx;
    }
    return 0;
}
