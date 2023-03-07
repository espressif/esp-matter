/*
 * Copyright (c) 2013-2016, GreenPeak Technologies
 * Copyright (c) 2017-2018, Qorvo Inc
 *
 * gpEncryption.c
 *
 * Contains encryption API
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

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpEncryption.h"
#include "gpHal.h"
#include "gpHal_SEC.h"
#include "gpEncryption_aes_mmo.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_ENCRYPTION

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
 *                    Static functions
 *****************************************************************************/




/*****************************************************************************
 *                    Public functions
 *****************************************************************************/

//-------------------------------------------------------------------------------------------------------
//  SYNCHRONOUS SECURITY FUNCTIONS
//-------------------------------------------------------------------------------------------------------
gpEncryption_Result_t gpEncryption_AESEncrypt(UInt8* pInplaceBuffer, UInt8* pAesKey, gpEncryption_AESOptions_t AESoptions)
{
    gpEncryption_Result_t result;
    result = gpHal_AESEncrypt(pInplaceBuffer, pAesKey, AESoptions);
    return result;
}

gpEncryption_Result_t gpEncryption_CCMEncrypt(gpEncryption_CCMOptions_t * pCCMOptions)
{
    gpEncryption_Result_t result;
    result = gpHal_CCMEncrypt(pCCMOptions);
    return result;
}

gpEncryption_Result_t gpEncryption_CCMDecrypt(gpEncryption_CCMOptions_t* pCCMOptions)
{
    gpEncryption_Result_t result;
    result = gpHal_CCMDecrypt(pCCMOptions);

    return result;
}

void gpEncryption_Init(void)
{
    gpEncryptionAesMmo_Init();
}

