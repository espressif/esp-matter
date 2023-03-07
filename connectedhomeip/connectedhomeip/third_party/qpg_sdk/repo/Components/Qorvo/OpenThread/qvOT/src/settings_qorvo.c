/*
 * Copyright (c) 2017-2021, Qorvo Inc
 *
 * settings_qorvo.c
 *   This file contains the implementation of the qorvo settings api for openthread.
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
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#define GP_COMPONENT_ID GP_COMPONENT_ID_QVOT

// #define GP_LOCAL_LOG
#define LOG_PREFIX "[Q] Nvm-----------: "


/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

// Openthread headers
#include "openthread/platform/settings.h"

// Qorvo Headers
#include "gpNvm.h"
#include "gpNvm_NvmProtect.h"
#include "gpAssert.h"
#include "gpLog.h"

/*****************************************************************************
 *                    Macros and Types Definitions
 *****************************************************************************/

#define QVOT_MAX_SUPPORTED_CHILDREN 70  // Hard maximum, due to implementation
#define QVOT_MIN_SUPPORTED_CHILDREN 10  // Minimum required by Thread specification

// Thread Routers MUST support 10 children, but SHOULD support 64 children
#ifndef QORVOOPENTHREAD_MAX_CHILDREN
#define QORVOOPENTHREAD_MAX_CHILDREN 0
#else
// "FTD (REEDs) have to support at least QVOT_MIN_SUPPORTED_CHILDREN children"
GP_COMPILE_TIME_VERIFY(QORVOOPENTHREAD_MAX_CHILDREN >= QVOT_MIN_SUPPORTED_CHILDREN);
// "FTD (REEDs) can support maximum QVOT_MAX_SUPPORTED_CHILDREN children"
GP_COMPILE_TIME_VERIFY(QORVOOPENTHREAD_MAX_CHILDREN <= QVOT_MAX_SUPPORTED_CHILDREN);
#endif

#define NVM_TAG_OPENTHREAD_NROFCHILDRENSTORED   0x00
#define NVM_TAG_OPENTHREAD_ACTIVEDATASET        0x01
#define NVM_TAG_OPENTHREAD_PENDINGDATASET       0x02
#define NVM_TAG_OPENTHREAD_NETWORKINFO          0x03
#define NVM_TAG_OPENTHREAD_PARENTINFO           0x04
// Move NVM_TAG_OPENTHREAD_CHILDINFO_BASE  (was 0x05) to last entry
#define NVM_TAG_OPENTHREAD_EOL_AUTOSTART        0x06
#define NVM_TAG_OPENTHREAD_SLAACIIDSECRETKEY    0x07
#define NVM_TAG_OPENTHREAD_DADINFO              0x08
#define NVM_TAG_OPENTHREAD_EOL_OMRPREFIX        0x09
#define NVM_TAG_OPENTHREAD_EOL_ONLINKPREFIX     0x0a
#define NVM_TAG_OPENTHREAD_SRPKEY               0x0b
#define NVM_TAG_OPENTHREAD_SRPCLIENTINFO        0x0c
#define NVM_TAG_OPENTHREAD_SRPSERVERINFO        0x0d
#define NVM_TAG_OPENTHREAD_EOL_NAT64PREFIX      0x0e
#define NVM_TAG_OPENTHREAD_BRULAPREFIX          0x0f
// Must be last
#define NVM_TAG_OPENTHREAD_CHILDINFO_BASE       0x10


#define NVM_TAG_OPENTHREAD_SIZEOF_NROFCHILDRENSTORED (1)
#define NVM_TAG_OPENTHREAD_SIZEOF_ACTIVEDATASET      (120) /* bytes. The spec requests 255, but it never uses more than 120. */
#define NVM_TAG_OPENTHREAD_SIZEOF_PENDINGDATASET     (120) /* bytes. */
#define NVM_TAG_OPENTHREAD_SIZEOF_NETWORKINFO        (38)  /* bytes. */
#define NVM_TAG_OPENTHREAD_SIZEOF_PARENTINFO         (10)  /* bytes. */
#define NVM_TAG_OPENTHREAD_SIZEOF_SLAACIIDSECRETKEY  (32)  /* bytes. */
#define NVM_TAG_OPENTHREAD_SIZEOF_DADINFO            (1)   /* bytes. uint8 counter */
#define NVM_TAG_OPENTHREAD_SIZEOF_SRPKEY             (126) /* bytes. */
#define NVM_TAG_OPENTHREAD_SIZEOF_SRPCLIENTINFO      (18)  /* bytes. OT_IP6_ADDRESS_SIZE + port(uint16) */
#define NVM_TAG_OPENTHREAD_SIZEOF_SRPSERVERINFO      (18)  /* bytes. OT_IP6_ADDRESS_SIZE + port(uint16) */
#define NVM_TAG_OPENTHREAD_SIZEOF_BRULAPREFIX        (17)  /* bytes. OT_IP6_ADDRESS_SIZE + length(uint8) */
#define NVM_TAG_OPENTHREAD_SIZEOF_CHILDINFO          (17)  /* bytes. Note that there can be multiple entries of this key ! */

#define NVM_TAG_OPENTHREAD_HEADERSIZE (offsetof(NvmTagsBuffer, NvmData))

typedef struct
{
    uint8_t dataValid;
    uint8_t dataSize; /* Note: dataSize type needs to be adapted if any tag size would be larger than 255 ! */
    union {
        uint8_t activeDataSet[NVM_TAG_OPENTHREAD_SIZEOF_ACTIVEDATASET];
        uint8_t pendingDataSet[NVM_TAG_OPENTHREAD_SIZEOF_PENDINGDATASET];
        uint8_t NetworkInfo[NVM_TAG_OPENTHREAD_SIZEOF_NETWORKINFO];
        uint8_t parentInfo[NVM_TAG_OPENTHREAD_SIZEOF_PARENTINFO];
        uint8_t slaacIidSecretKey[NVM_TAG_OPENTHREAD_SIZEOF_SLAACIIDSECRETKEY];
        uint8_t dadInfo[NVM_TAG_OPENTHREAD_SIZEOF_DADINFO];
        uint8_t srpKey[NVM_TAG_OPENTHREAD_SIZEOF_SRPKEY];
        uint8_t srpClientInfo[NVM_TAG_OPENTHREAD_SIZEOF_SRPCLIENTINFO];
        uint8_t srpServerInfo[NVM_TAG_OPENTHREAD_SIZEOF_SRPSERVERINFO];
        uint8_t brUlaPrefix[NVM_TAG_OPENTHREAD_SIZEOF_BRULAPREFIX];
        uint8_t childInfo[NVM_TAG_OPENTHREAD_SIZEOF_CHILDINFO];
    } NvmData;
} NvmTagsBuffer;

typedef struct NvmTag_
{
    uint8_t otTagId;
    uint8_t nvmTagId;
    uint8_t maxTagSize;
} NvmTag_t;

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

static bool qorvoSettings_DefaultInitializer(const ROM void* pTag, uint8_t* pBuffer);

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

static const NvmTag_t NvmLookupTable[] =
{
    /* OpenThread key tag ID*/              /* Internal NVM tag ID*/                /* Max size for internal tag */
    {OT_SETTINGS_KEY_ACTIVE_DATASET,        NVM_TAG_OPENTHREAD_ACTIVEDATASET,       NVM_TAG_OPENTHREAD_SIZEOF_ACTIVEDATASET},
    {OT_SETTINGS_KEY_PENDING_DATASET,       NVM_TAG_OPENTHREAD_PENDINGDATASET,      NVM_TAG_OPENTHREAD_SIZEOF_PENDINGDATASET},
    {OT_SETTINGS_KEY_NETWORK_INFO,          NVM_TAG_OPENTHREAD_NETWORKINFO,         NVM_TAG_OPENTHREAD_SIZEOF_NETWORKINFO},
    {OT_SETTINGS_KEY_PARENT_INFO,           NVM_TAG_OPENTHREAD_PARENTINFO,          NVM_TAG_OPENTHREAD_SIZEOF_PARENTINFO},
    {OT_SETTINGS_KEY_SLAAC_IID_SECRET_KEY,  NVM_TAG_OPENTHREAD_SLAACIIDSECRETKEY,   NVM_TAG_OPENTHREAD_SIZEOF_SLAACIIDSECRETKEY},
    {OT_SETTINGS_KEY_DAD_INFO,              NVM_TAG_OPENTHREAD_DADINFO,             NVM_TAG_OPENTHREAD_SIZEOF_DADINFO},
    {OT_SETTINGS_KEY_SRP_ECDSA_KEY,         NVM_TAG_OPENTHREAD_SRPKEY,              NVM_TAG_OPENTHREAD_SIZEOF_SRPKEY},
    {OT_SETTINGS_KEY_SRP_CLIENT_INFO,       NVM_TAG_OPENTHREAD_SRPCLIENTINFO,       NVM_TAG_OPENTHREAD_SIZEOF_SRPCLIENTINFO},
    {OT_SETTINGS_KEY_SRP_SERVER_INFO,       NVM_TAG_OPENTHREAD_SRPSERVERINFO,       NVM_TAG_OPENTHREAD_SIZEOF_SRPSERVERINFO},
    {OT_SETTINGS_KEY_BR_ULA_PREFIX,         NVM_TAG_OPENTHREAD_BRULAPREFIX,         NVM_TAG_OPENTHREAD_SIZEOF_BRULAPREFIX},
    {OT_SETTINGS_KEY_CHILD_INFO,            NVM_TAG_OPENTHREAD_CHILDINFO_BASE,      NVM_TAG_OPENTHREAD_SIZEOF_CHILDINFO},
};
#define QORVOOPENTHREAD_NVM_MAX_SUPPORTED_KEYS number_of_elements(NvmLookupTable)

static uint8_t qorvoSettings_NrOfChildrenStored;

#define QORVOOPENTHREAD_NVM_BASE_TAG_ID ((uint16_t)(GP_COMPONENT_ID << 8))
#define QORVOOPENTHREAD_NVM_MINIMAL_TAG_COUNT (QORVOOPENTHREAD_NVM_MAX_SUPPORTED_KEYS)
#define QORVOOPENTHREAD_NVM_MINIMAL_TAGS_DATA \
     {QORVOOPENTHREAD_NVM_BASE_TAG_ID + NVM_TAG_OPENTHREAD_ACTIVEDATASET,      NULL,  NVM_TAG_OPENTHREAD_HEADERSIZE + NVM_TAG_OPENTHREAD_SIZEOF_ACTIVEDATASET,      gpNvm_UpdateFrequencyLow, NULL, NULL} \
    ,{QORVOOPENTHREAD_NVM_BASE_TAG_ID + NVM_TAG_OPENTHREAD_PENDINGDATASET,     NULL,  NVM_TAG_OPENTHREAD_HEADERSIZE + NVM_TAG_OPENTHREAD_SIZEOF_PENDINGDATASET,     gpNvm_UpdateFrequencyLow, NULL, NULL} \
    ,{QORVOOPENTHREAD_NVM_BASE_TAG_ID + NVM_TAG_OPENTHREAD_NETWORKINFO,        NULL,  NVM_TAG_OPENTHREAD_HEADERSIZE + NVM_TAG_OPENTHREAD_SIZEOF_NETWORKINFO,        gpNvm_UpdateFrequencyLow, NULL, NULL} \
    ,{QORVOOPENTHREAD_NVM_BASE_TAG_ID + NVM_TAG_OPENTHREAD_PARENTINFO,         NULL,  NVM_TAG_OPENTHREAD_HEADERSIZE + NVM_TAG_OPENTHREAD_SIZEOF_PARENTINFO,         gpNvm_UpdateFrequencyLow, NULL, NULL} \
    ,{QORVOOPENTHREAD_NVM_BASE_TAG_ID + NVM_TAG_OPENTHREAD_SLAACIIDSECRETKEY,  NULL,  NVM_TAG_OPENTHREAD_HEADERSIZE + NVM_TAG_OPENTHREAD_SIZEOF_SLAACIIDSECRETKEY,  gpNvm_UpdateFrequencyLow, NULL, NULL} \
    ,{QORVOOPENTHREAD_NVM_BASE_TAG_ID + NVM_TAG_OPENTHREAD_DADINFO,            NULL,  NVM_TAG_OPENTHREAD_HEADERSIZE + NVM_TAG_OPENTHREAD_SIZEOF_DADINFO,            gpNvm_UpdateFrequencyLow, NULL, NULL} \
    ,{QORVOOPENTHREAD_NVM_BASE_TAG_ID + NVM_TAG_OPENTHREAD_SRPKEY,             NULL,  NVM_TAG_OPENTHREAD_HEADERSIZE + NVM_TAG_OPENTHREAD_SIZEOF_SRPKEY,             gpNvm_UpdateFrequencyLow, NULL, NULL} \
    ,{QORVOOPENTHREAD_NVM_BASE_TAG_ID + NVM_TAG_OPENTHREAD_SRPCLIENTINFO,      NULL,  NVM_TAG_OPENTHREAD_HEADERSIZE + NVM_TAG_OPENTHREAD_SIZEOF_SRPCLIENTINFO,      gpNvm_UpdateFrequencyLow, NULL, NULL} \
    ,{QORVOOPENTHREAD_NVM_BASE_TAG_ID + NVM_TAG_OPENTHREAD_SRPSERVERINFO,      NULL,  NVM_TAG_OPENTHREAD_HEADERSIZE + NVM_TAG_OPENTHREAD_SIZEOF_SRPSERVERINFO,      gpNvm_UpdateFrequencyLow, NULL, NULL} \
    ,{QORVOOPENTHREAD_NVM_BASE_TAG_ID + NVM_TAG_OPENTHREAD_BRULAPREFIX,        NULL,  NVM_TAG_OPENTHREAD_HEADERSIZE + NVM_TAG_OPENTHREAD_SIZEOF_BRULAPREFIX,        gpNvm_UpdateFrequencyLow, NULL, NULL}
#define QORVOOPENTHREAD_NVM_CHILD_ENTRY(X) \
    ,{QORVOOPENTHREAD_NVM_BASE_TAG_ID + NVM_TAG_OPENTHREAD_CHILDINFO_BASE + (X), NULL,  NVM_TAG_OPENTHREAD_HEADERSIZE + NVM_TAG_OPENTHREAD_SIZEOF_CHILDINFO,          gpNvm_UpdateFrequencyLow, NULL, NULL}

const gpNvm_IdentifiableTag_t ROM qorvoSettings_NvmElements[QORVOOPENTHREAD_NVM_MINIMAL_TAG_COUNT + QORVOOPENTHREAD_MAX_CHILDREN] FLASH_PROGMEM = {
/* note that the sizes in this table are 1 byte more then the actual data to be able to easily generate the "Not Found" error in the platform Api */
    QORVOOPENTHREAD_NVM_MINIMAL_TAGS_DATA
/* Child info storage */
    ,{QORVOOPENTHREAD_NVM_BASE_TAG_ID + NVM_TAG_OPENTHREAD_NROFCHILDRENSTORED, (uint8_t*)&(qorvoSettings_NrOfChildrenStored), NVM_TAG_OPENTHREAD_SIZEOF_NROFCHILDRENSTORED, gpNvm_UpdateFrequencyLow, (gpNvm_cbDefaultValueInitializer_t)qorvoSettings_DefaultInitializer, NULL}

#if(QORVOOPENTHREAD_MAX_CHILDREN > 0)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(0)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(1)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(2)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(3)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(4)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(5)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(6)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(7)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(8)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(9)
#endif


#if(QORVOOPENTHREAD_MAX_CHILDREN > 10)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(10)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 11)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(11)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 12)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(12)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 13)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(13)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 14)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(14)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 15)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(15)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 16)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(16)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 17)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(17)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 18)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(18)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 19)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(19)
#endif


#if(QORVOOPENTHREAD_MAX_CHILDREN > 20)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(20)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 21)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(21)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 22)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(22)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 23)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(23)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 24)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(24)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 25)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(25)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 26)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(26)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 27)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(27)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 28)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(28)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 29)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(29)
#endif


#if(QORVOOPENTHREAD_MAX_CHILDREN > 30)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(30)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 31)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(31)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 32)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(32)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 33)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(33)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 34)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(34)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 35)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(35)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 36)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(36)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 37)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(37)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 38)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(38)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 39)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(39)
#endif


#if(QORVOOPENTHREAD_MAX_CHILDREN > 40)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(40)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 41)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(41)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 42)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(42)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 43)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(43)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 44)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(44)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 45)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(45)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 46)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(46)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 47)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(47)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 48)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(48)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 49)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(49)
#endif


#if(QORVOOPENTHREAD_MAX_CHILDREN > 50)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(50)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 51)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(51)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 52)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(52)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 53)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(53)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 54)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(54)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 55)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(55)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 56)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(56)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 57)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(57)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 58)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(58)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 59)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(59)
#endif


#if(QORVOOPENTHREAD_MAX_CHILDREN > 60)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(60)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 61)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(61)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 62)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(62)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 63)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(63)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 64)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(64)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 65)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(65)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 66)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(66)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 67)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(67)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 68)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(68)
#endif
#if(QORVOOPENTHREAD_MAX_CHILDREN > 69)
    QORVOOPENTHREAD_NVM_CHILD_ENTRY(69)
#endif
};

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
static NvmTag_t* qorvoSettings_GetTagStructByKey(uint16_t aKey)
{
    uint8_t i;

    for(i = 0; i < QORVOOPENTHREAD_NVM_MAX_SUPPORTED_KEYS; i++)
    {
        if(NvmLookupTable[i].otTagId == aKey)
        {
            return (NvmTag_t*)(&NvmLookupTable[i]);
        }
    }

    return NULL;
}

static bool qorvoSettings_DefaultInitializer(const ROM void* pTag, uint8_t* pBuffer)
{
    gpNvm_IdentifiableTag_t tag;
    MEMCPY_P((uint8_t*)&tag, pTag, sizeof(gpNvm_IdentifiableTag_t));
    if(NULL == pBuffer)
    {
        pBuffer = tag.pRamLocation;
        if(NULL == pBuffer)
        {
            return false;
        }
    }

    if(QORVOOPENTHREAD_NVM_BASE_TAG_ID + NVM_TAG_OPENTHREAD_NROFCHILDRENSTORED == tag.uniqueTagId)
    {
        MEMSET(pBuffer, 0x00, sizeof(uint8_t));
    }
    else
    {
        GP_LOG_SYSTEM_PRINTF(LOG_PREFIX "CRIT: Did not find tag id 0x%4x", 0, tag.uniqueTagId);
        GP_ASSERT_DEV_INT(false);
        return false; //Signal NVM init failure
    }

    return true;
}

/* Special functionality to delete a ChildInfo tag */
/* In case a single entry is deleted from the childInfo list, we shift all the subsequent childInfo entries */
/* This has the advantage that the ChildInfo entries are always in a list without gaps, making the code much easier */
/* the disadvantage is that we need to perform multiple NVM accesses to shift all entries, but we assume this  */
/* scenario does not occur often */
/* We also assume that the OpenThread code does not remember the indexes in the NVM */
/* and that if it iterates of the ChildInfo entries, it will only delete one entry during a full loop over all entries */
static otError qorvoSettings_DeleteChild(int childOffset)
{
    GP_LOG_PRINTF(LOG_PREFIX "delete child %i/%u", 0, childOffset, qorvoSettings_NrOfChildrenStored);

    if(childOffset == -1)
    {
        // Delete all childs
        for(uint8_t i = 0; i < qorvoSettings_NrOfChildrenStored; i++)
        {
            gpNvm_ClearProtected(GP_COMPONENT_ID, NVM_TAG_OPENTHREAD_CHILDINFO_BASE + i);
        }

        qorvoSettings_NrOfChildrenStored = 0;
    }
    else if((childOffset < 0) || (childOffset >= qorvoSettings_NrOfChildrenStored))
    {
        // Out of bounds of stored entries
        GP_LOG_PRINTF(LOG_PREFIX "WARN: Child not fount", 0);
        return OT_ERROR_NOT_FOUND;
    }
    else
    {
        NvmTagsBuffer buffer;

        // shift all entries after the current one one index up.
        // Last entry 'frees up'

        for(uint8_t i = childOffset; i < (qorvoSettings_NrOfChildrenStored - 1); i++)
        {
            GP_LOG_PRINTF(LOG_PREFIX "shifting tag:%u >> %u", 0, i + 1, i);
            gpNvm_RestoreProtected(GP_COMPONENT_ID, NVM_TAG_OPENTHREAD_CHILDINFO_BASE + i + 1, (uint8_t*)&buffer);
            gpNvm_BackupProtected(GP_COMPONENT_ID, NVM_TAG_OPENTHREAD_CHILDINFO_BASE + i, (uint8_t*)&buffer);
        }

        // Remove the last entry
        gpNvm_ClearProtected(GP_COMPONENT_ID, NVM_TAG_OPENTHREAD_CHILDINFO_BASE + (qorvoSettings_NrOfChildrenStored - 1));

        qorvoSettings_NrOfChildrenStored--;
    }

    // Update number of stored in NVM
    gpNvm_BackupProtected(GP_COMPONENT_ID, NVM_TAG_OPENTHREAD_NROFCHILDRENSTORED, (uint8_t*)(&qorvoSettings_NrOfChildrenStored));

    return OT_ERROR_NONE;
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
void qorvoSettingsInit()
{
    // Register the NVM storage
    gpNvm_RegisterElementsProtected(qorvoSettings_NvmElements, number_of_elements(qorvoSettings_NvmElements));

    gpNvm_RestoreProtected(GP_COMPONENT_ID, NVM_TAG_OPENTHREAD_NROFCHILDRENSTORED, NULL);
    /* safety mechanism to ensure number of children is always initialized */
    if(qorvoSettings_NrOfChildrenStored > QORVOOPENTHREAD_MAX_CHILDREN)
    {
        qorvoSettings_NrOfChildrenStored = 0;
        gpNvm_BackupProtected(GP_COMPONENT_ID, NVM_TAG_OPENTHREAD_NROFCHILDRENSTORED, (uint8_t*)(&qorvoSettings_NrOfChildrenStored));
    }
}

otError qorvoSettingsGet(uint16_t aKey, int aChildIndex, uint8_t* aValue, uint16_t* aValueLength)
{
    NvmTagsBuffer buffer;
    NvmTag_t* pKeyTag;
    uint8_t tagId;

    if((aValue == NULL) || (aValueLength == NULL))
    {
        return OT_ERROR_INVALID_ARGS;
    }

    pKeyTag = qorvoSettings_GetTagStructByKey(aKey);
    if(pKeyTag == NULL)
    {
        return OT_ERROR_NOT_FOUND;
    }
    tagId = pKeyTag->nvmTagId;

    if(aKey == OT_SETTINGS_KEY_CHILD_INFO)
    {
        if((aChildIndex < 0) || (aChildIndex >= qorvoSettings_NrOfChildrenStored))
        {
            return OT_ERROR_INVALID_ARGS;
        }
        tagId += aChildIndex;
    }

    gpNvm_RestoreProtected(GP_COMPONENT_ID, tagId, (uint8_t*)(&buffer));
    GP_LOG_PRINTF(LOG_PREFIX "get key:%d ind:%d tag:%d valid=%d", 0, aKey, aChildIndex, tagId, buffer.dataValid);

    if(buffer.dataValid == 1) // 0xFF will be set after NVM clearing
    {
        GP_LOG_PRINTF(LOG_PREFIX "exp len: %d max len; %d stored: %d", 0, *aValueLength, pKeyTag->maxTagSize, buffer.dataSize);
        *aValueLength = buffer.dataSize;
        // Should never be stored with a higher length
        if(*aValueLength > pKeyTag->maxTagSize)
        {
            return OT_ERROR_INVALID_ARGS;
        }
        MEMCPY(aValue, &buffer.NvmData, *aValueLength);
#ifdef GP_LOCAL_LOG
        gpLog_PrintBuffer(*aValueLength, aValue);
#endif // GP_LOCAL_LOG

        return OT_ERROR_NONE;
    }

    return OT_ERROR_NOT_FOUND;
}

otError qorvoSettingsAdd(uint16_t aKey, bool isFlatTag, const uint8_t* aValue, uint16_t aValueLength)
{
    NvmTagsBuffer buffer;
    NvmTag_t* pKeyTag;
    uint8_t tagId;

    pKeyTag = qorvoSettings_GetTagStructByKey(aKey);
    if(pKeyTag == NULL)
    {
        return OT_ERROR_NOT_FOUND;
    }

    // for ChildInfo entries, the nvm entries are added on top of the existing entries
    // for all other entries, the nvm entries are overwriting the existing entry
    if(((aKey == OT_SETTINGS_KEY_CHILD_INFO) && (isFlatTag == true)) ||
       ((aKey != OT_SETTINGS_KEY_CHILD_INFO) && (isFlatTag == false)) ||
       (aValue == NULL) || (aValueLength > pKeyTag->maxTagSize))
    {
        return OT_ERROR_INVALID_ARGS;
    }

    tagId = pKeyTag->nvmTagId;

    // if not a MTD and a child is stored: check that we have enough space to store more data
    if((aKey == OT_SETTINGS_KEY_CHILD_INFO) && (qorvoSettings_NrOfChildrenStored >= QORVOOPENTHREAD_MAX_CHILDREN))
    {
        return OT_ERROR_NO_BUFS;
    }

    if(isFlatTag == false) // this means aKey must be OT_SETTINGS_KEY_CHILD_INFO - address the end of the children list
    {
        tagId += qorvoSettings_NrOfChildrenStored;
    }

    //Fill buffer
    MEMSET(&buffer, 0x00, sizeof(NvmTagsBuffer));
    MEMCPY(&buffer.NvmData, aValue, aValueLength);
    buffer.dataValid = 1;
    buffer.dataSize = (uint8_t)(aValueLength & 0xFF);

    GP_LOG_PRINTF(LOG_PREFIX "add key:%d ind:%d tag:%d: stored %u/%u", 0, aKey, isFlatTag, tagId, buffer.dataSize, pKeyTag->maxTagSize);
    gpNvm_BackupProtected(GP_COMPONENT_ID, tagId, (uint8_t*)(&buffer));
#ifdef GP_LOCAL_LOG
    gpLog_PrintBuffer(aValueLength, (uint8_t*)aValue);
#endif // GP_LOCAL_LOG

    // Update children stored variable
    if(aKey == OT_SETTINGS_KEY_CHILD_INFO)
    {
        qorvoSettings_NrOfChildrenStored++;
        gpNvm_BackupProtected(GP_COMPONENT_ID, NVM_TAG_OPENTHREAD_NROFCHILDRENSTORED, (uint8_t*)(&qorvoSettings_NrOfChildrenStored));
    }

    return OT_ERROR_NONE;
}

otError qorvoSettingsDelete(uint16_t aKey, int aChildIndex)
{
    NvmTag_t* pKeyTag;

    pKeyTag = qorvoSettings_GetTagStructByKey(aKey);
    if(pKeyTag == NULL)
    {
        GP_LOG_PRINTF(LOG_PREFIX "WARN: key:%d NOT FOUND", 0, aKey);
        return OT_ERROR_NOT_FOUND;
    }

    if(aKey == OT_SETTINGS_KEY_CHILD_INFO)
    {
        return qorvoSettings_DeleteChild(aChildIndex);
    }
    else
    {
        GP_LOG_PRINTF(LOG_PREFIX "del key:%d tag:%d", 0, aKey, pKeyTag->nvmTagId);
        gpNvm_ClearProtected(GP_COMPONENT_ID, pKeyTag->nvmTagId);
    }

    return OT_ERROR_NONE;
}

void qorvoSettingsWipe(void)
{
    GP_LOG_PRINTF(LOG_PREFIX "Wipe all", 0);
    gpNvm_ClearProtected(GP_COMPONENT_ID, gpNvm_AllTags);
}
