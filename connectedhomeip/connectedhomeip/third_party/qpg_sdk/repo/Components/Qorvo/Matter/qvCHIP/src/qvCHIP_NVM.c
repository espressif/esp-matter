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
 */

/** @file "qvCHIP_NVM.c"
 *
 *  CHIP NVM functionality
 *
 *  Implementation of qvCHIP NVM
*/

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
/* <CodeGenerator Placeholder> Includes */

#define GP_COMPONENT_ID GP_COMPONENT_ID_QVCHIP

#include "gpAssert.h"
#include "gpNvm.h"

#include "qvCHIP.h"

/* </CodeGenerator Placeholder> Includes */

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/** Helper macro to translate 16-bit in a usable KVS keys
    Normal keys are ASCII based key arrays - adding '1' to avoid \0
*/
#define NVM_KEY_TO_KVS(key)                                   \
    {                                                         \
        0x01 + (key & 0xFF), 0x01 + ((key >> 8) & 0xFF), '\0' \
    }
/** Helper to check if created key didn't overflow to \0
*/
#define NVM_KEY_TO_KVS_CHECK(charKey) GP_ASSERT_DEV_EXT(strlen(charKey) == 2);

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

qvStatus_t qvCHIP_Nvm_Backup(uint16_t key, const uint8_t* pRamLocation, uint16_t length)
{
    if(NULL == pRamLocation || 0 == length)
    {
        return QV_STATUS_INVALID_ARGUMENT;
    }

    char charKey[] = NVM_KEY_TO_KVS(key);
    NVM_KEY_TO_KVS_CHECK(charKey);

    return qvCHIP_KvsPut(charKey, pRamLocation, length);
}

qvStatus_t qvCHIP_Nvm_Restore(uint16_t key, uint8_t* pRamLocation, uint16_t* length)
{
    qvStatus_t status;
    size_t readBytes;

    if(NULL == pRamLocation || NULL == length)
    {
        return QV_STATUS_INVALID_ARGUMENT;
    }

    char charKey[] = NVM_KEY_TO_KVS(key);
    NVM_KEY_TO_KVS_CHECK(charKey);

    status = qvCHIP_KvsGet(charKey, pRamLocation, *length, &readBytes, 0);
    *length = (uint16_t)readBytes;

    return status;
}

void qvCHIP_Nvm_ClearValue(uint16_t key)
{
    char charKey[] = NVM_KEY_TO_KVS(key);
    NVM_KEY_TO_KVS_CHECK(charKey);

    qvCHIP_KvsDelete(charKey);
}

bool qvCHIP_Nvm_ValueExists(uint16_t key, uint16_t* length)
{
    if(NULL == length)
    {
        return false;
    }

    char charKey[] = NVM_KEY_TO_KVS(key);
    size_t dummy;

    NVM_KEY_TO_KVS_CHECK(charKey);

    NOT_USED(length);

    // Fetch data but provide a too small buffer - use truncate result as existence check
    return (qvCHIP_KvsGet(charKey, &dummy, 0, &dummy, 0) == QV_STATUS_BUFFER_TOO_SMALL);
}
