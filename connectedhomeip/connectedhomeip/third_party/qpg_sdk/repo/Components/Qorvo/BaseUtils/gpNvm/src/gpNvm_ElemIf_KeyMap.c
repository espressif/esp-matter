/*
 * Copyright (c) 2017-2021, Qorvo Inc
 *
 * This file gives an implementation of the Non Volatile Memory component using element interface
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
#ifdef GP_NVM_USE_ASSERT_SAFETY_NET
#include "gpNvm_AssertSafetyNet.h"
#endif // #ifdef GP_NVM_USE_ASSERT_SAFETY_NET
#include "global.h"
#include "gpSched.h"
#include "gpLog.h"
#include "gpAssert.h"
#include "gpUtils.h" //CRC calculations
#include "gpPoolMem.h"

#include "gpNvm.h"
#include "gpNvm_NvmProtect.h"
#include "gpNvm_defs.h"
#include "gpNvm_ElemIf.h"
/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_NVM

#ifndef GP_NVM_NBR_OF_UNIQUE_TAGS
#error error: GP_NVM_NBR_OF_UNIQUE_TAGS should defined in the make environment
#endif

#ifndef GP_NVM_TYPE
#error GP_NVM_TYPE should be specified in the build environment
#endif //GP_NVM_TYPE

#ifdef GP_NVM_NBR_OF_UNIQUE_TAGS_OVERRULE
#undef GP_NVM_NBR_OF_UNIQUE_TAGS
#define GP_NVM_NBR_OF_UNIQUE_TAGS GP_NVM_NBR_OF_UNIQUE_TAGS_OVERRULE
#endif

#ifndef GP_NVM_VERSION
#define GP_NVM_VERSION 1
#endif

#define NVM_KEYMAP_TOKENLENGTH (2)


#define GP_NVM_ELEMENT_ACCESS_TYPE_BACKUP   0
#define GP_NVM_ELEMENT_ACCESS_TYPE_RESTORE  1
#define GP_NVM_ELEMENT_ACCESS_TYPE_CLEAR    2

#define GP_NVM_UID_TO_COMPID(u)             ((UInt8)((u)>>8))
#define GP_NVM_UID_TO_TAGID(u)              ((UInt8)((u) & 0xFF))
#define GP_NVM_COMPID_TAGID_TO_UID(c,t)     ((UInt16)((((UInt16)(c))<<8) | ((UInt16)((t) & 0xFF))))

#define GP_NVM_REGISTER_ELEMENT_PROTECTED_ENTRY() {                 \
    Int8 registerElementEntryCounter;                               \
    HAL_DISABLE_GLOBAL_INT();                                       \
    Nvm_registerElementEntryCounter++;                              \
    registerElementEntryCounter = Nvm_registerElementEntryCounter;  \
    HAL_ENABLE_GLOBAL_INT();                                        \
    if (registerElementEntryCounter > 2) {                          \
        GP_LOG_PRINTF("Unexpected reentrancy detected.",0);         \
        while(true);}}

#define GP_NVM_REGISTER_ELEMENT_PROTECTED_EXIT() {                  \
    Int8 registerElementEntryCounter;                               \
    HAL_DISABLE_GLOBAL_INT();                                       \
    Nvm_registerElementEntryCounter--;                              \
    registerElementEntryCounter = Nvm_registerElementEntryCounter;  \
    HAL_ENABLE_GLOBAL_INT();                                        \
    if (registerElementEntryCounter < 0) {                          \
        GP_LOG_PRINTF("Unexpected exit.",0);                        \
        while(true);}}

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

void Nvm_CheckConsistency(void);

static Bool Nvm_cbNvmTypeConsistencyChecker(const ROM void *pTag);
static Bool Nvm_cbNvmTypeDefaultValueInitializer(const ROM void *pTag, UInt8* pBuffer);

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

static const ROM gpNvm_IdentifiableTag_t * Nvm_Tags[GP_NVM_NBR_OF_UNIQUE_TAGS];
static Int8 Nvm_registerElementEntryCounter = 0;

//Storing type for consistency check
UInt8 gpNvm_Type;
const ROM gpNvm_IdentifiableTag_t gpNvm_NvmElement_NvmType FLASH_PROGMEM = {
    GP_NVM_COMPID_TAGID_TO_UID(GP_COMPONENT_ID, GP_NVM_TYPE_UNIQUE_TAG_ID),
    &gpNvm_Type,
    sizeof(UInt8),
    NVM_TYPE_UPDATE_FREQUENCY,
    &Nvm_cbNvmTypeDefaultValueInitializer,
    &Nvm_cbNvmTypeConsistencyChecker};

/*****************************************************************************
 *                    Internal Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

Bool Nvm_cbNvmTypeConsistencyChecker(const ROM void * pTag)
{
    gpNvm_IdentifiableTag_t tag;
    MEMCPY_P((UInt8*)&tag, pTag, sizeof(gpNvm_IdentifiableTag_t));

    if (0 == tag.pRamLocation)
    {
        return false;
    }
    return (GP_NVM_TYPE == *(tag.pRamLocation));
}

Bool Nvm_cbNvmTypeDefaultValueInitializer(const ROM void * pTag, UInt8* pBuffer)
{
    gpNvm_IdentifiableTag_t tag;
    UInt8 gp_nvm_type = GP_NVM_TYPE;

    MEMCPY_P((UInt8*)&tag, pTag, sizeof(gpNvm_IdentifiableTag_t));
    if(NULL == pBuffer)
    {
        pBuffer = tag.pRamLocation;
        if(NULL == pBuffer)
        {
            return false;
        }
    }
    MEMCPY_P(pBuffer, (UInt8*)&gp_nvm_type, sizeof(UInt8));
    return true;
}

void Nvm_CheckConsistency(void)
{
    gpNvm_KeyIndex_t i;
    Bool consistencyOk = true;

    //Check if tag storage is full
    for(i=0; i < GP_NVM_NBR_OF_UNIQUE_TAGS; i++)
    {
        if(Nvm_Tags[i] == NULL)
        {
            break;
        }
    }

// avoid compiler warning in case GP_NVM_NBR_OF_UNIQUE_TAGS is the maximum value of
// gpNvm_KeyIndex_t. (i.e. 255 when type(gpNvm_KeyIndex_t) == UInt8 and 2^32-1 when
// type(gpNvm_KeyIndex_t) == UInt32)
#if GP_NVM_NBR_OF_UNIQUE_TAGS != UINT8_MAX && GP_NVM_NBR_OF_UNIQUE_TAGS != UINT32_MAX
    //Check validity
    GP_ASSERT_SYSTEM(i <= GP_NVM_NBR_OF_UNIQUE_TAGS);
#endif

    //Check consistency per tag if callback is available
    for(i=0; i < GP_NVM_NBR_OF_UNIQUE_TAGS; i++)
    {
        if(Nvm_Tags[i] != NULL)
        {
            gpNvm_IdentifiableTag_t nvmTag;

            MEMCPY_P(&nvmTag, Nvm_Tags[i], sizeof(gpNvm_IdentifiableTag_t));
            if(nvmTag.ConsistencyCheckerCB != NULL)
            {
                if(!nvmTag.ConsistencyCheckerCB(Nvm_Tags[i]))
                {
                    GP_LOG_SYSTEM_PRINTF("Consistency fail - tag:%x",0, nvmTag.uniqueTagId);
                    consistencyOk = false;
                    break;
                }
            }
        }
    }

    if (!consistencyOk)
    {
        //Put NVM to default state
        gpNvm_ClearNvm();
        gpNvm_cbFailedCheckConsistency();
    }

#ifdef GP_NVM_USE_ASSERT_SAFETY_NET
    Nvm_DisableSafetyNet();
#endif //#ifdef GP_NVM_USE_ASSERT_SAFETY_NET
#if defined(GP_DIVERSITY_GPHAL_K8E)
    Nvm_FreeTagCache();
#endif
}

void Nvm_Access(UInt8 componentId, UInt8 tagId, UInt8* pRamLocation, UInt8 accessType)
{
    gpNvm_KeyIndex_t i;
    UInt16 uniqueTagId = GP_NVM_COMPID_TAGID_TO_UID(componentId, tagId);
    UInt16 mask;
    Bool tagFound = false;
    gpNvm_LookupTable_Handle_t lutHandle;
    gpNvm_Result_t result;
    Bool freeLutAfterUse;
    UInt8 tokenLength;
    UInt8 token[NVM_KEYMAP_TOKENLENGTH];
    gpNvm_KeyIndex_t maxNbrOfMatches = GP_NVM_NBR_OF_UNIQUE_TAGS;
    if(   (GP_NVM_ELEMENT_ACCESS_TYPE_CLEAR != accessType)
       && (tagId == gpNvm_AllTags || componentId == gpNvm_AllComponents))
    {
        //When gpNvm_AllTags used, default ram location will be used to backup data
        GP_ASSERT_DEV_EXT(pRamLocation == NULL);
    }

    token[0] = componentId;
    token[1] = tagId;

    if(tagId == gpNvm_AllTags && componentId == gpNvm_AllComponents)
    {
        tokenLength = 0;
        mask = GP_NVM_COMPID_TAGID_TO_UID(0x00,0x00);
    }
    else if(componentId == gpNvm_AllComponents)
    {
        tokenLength = 0;
        mask = GP_NVM_COMPID_TAGID_TO_UID(0x00,0xff);
    }
    else if(tagId == gpNvm_AllTags)
    {
        tokenLength = 1;
        mask = GP_NVM_COMPID_TAGID_TO_UID(0xff,0x00);
    }
    else
    {
        tokenLength = NVM_KEYMAP_TOKENLENGTH;
        mask = GP_NVM_COMPID_TAGID_TO_UID(0xff,0xff);
        maxNbrOfMatches = 1;
    }
    if(GP_NVM_ELEMENT_ACCESS_TYPE_RESTORE == accessType)
    {
        result = \
        gpNvm_AcquireLutHandleProtected(
          &lutHandle,
          gpNvm_PoolId_Tag,
          gpNvm_UpdateFrequencyIgnore,
          tokenLength,
          token,
          &freeLutAfterUse,
          maxNbrOfMatches
        );

        GP_ASSERT_DEV_EXT (result != gpNvm_Result_Truncated);
    }
    for(i = 0; i < GP_NVM_NBR_OF_UNIQUE_TAGS; i++)
    {
        gpNvm_IdentifiableTag_t nvmTag;

        HAL_WDT_RESET();

        if(NULL == Nvm_Tags[i])
        {
            break;
        }
        MEMCPY_P(&nvmTag, Nvm_Tags[i], sizeof(gpNvm_IdentifiableTag_t));
        if((nvmTag.uniqueTagId & mask) == (uniqueTagId & mask))
        {
            UInt8 tempBuffer[NVM_MAX_TAG_SIZE];
            UInt8* valueBuffer;
            if (pRamLocation)
            {
                valueBuffer = pRamLocation;
            }
            else if (nvmTag.pRamLocation)
            {
                valueBuffer = nvmTag.pRamLocation;
            }
            else
            {
                if(GP_NVM_ELEMENT_ACCESS_TYPE_CLEAR != accessType)
                {
                    /* skip save/restore without src/dst */
                    continue;
                }
                valueBuffer = tempBuffer;
            }

            token[0] = GP_NVM_UID_TO_COMPID(nvmTag.uniqueTagId);
            token[1] = GP_NVM_UID_TO_TAGID(nvmTag.uniqueTagId);

            tagFound = true;

            if(GP_NVM_ELEMENT_ACCESS_TYPE_CLEAR == accessType
            )
            {
                if(NULL != nvmTag.DefaultValueInitializerCB)
                {
                    if(!nvmTag.DefaultValueInitializerCB(Nvm_Tags[i], valueBuffer))
                    {
                        MEMSET(valueBuffer,-1,nvmTag.size);
                    }
                }
                else
                {
                    MEMSET(valueBuffer,-1,nvmTag.size);
                }
            }

            if(   (GP_NVM_ELEMENT_ACCESS_TYPE_BACKUP == accessType)
               || (GP_NVM_ELEMENT_ACCESS_TYPE_CLEAR == accessType))
            {
                gpNvm_Write(gpNvm_PoolId_Tag,
                            nvmTag.updateFrequency,
                            NVM_KEYMAP_TOKENLENGTH,
                            token,
                            nvmTag.size,
                            valueBuffer);
            }
            else if(GP_NVM_ELEMENT_ACCESS_TYPE_RESTORE == accessType)
            {
                UInt8 DataLength = 0;
                gpNvm_Result_t result;

                result = \
                gpNvm_ReadUnique(
                    lutHandle,
                    gpNvm_PoolId_Tag,
                    nvmTag.updateFrequency,
                    NULL,
                    NVM_KEYMAP_TOKENLENGTH,
                    token,
                    nvmTag.size,
                    &DataLength,
                    valueBuffer
                );
                if (!(result == gpNvm_Result_DataAvailable || result == gpNvm_Result_Truncated))
                {
                    MEMSET(valueBuffer, 0xff, nvmTag.size);
                }
            }
            else
            {
                GP_ASSERT_DEV_INT(false);
            }
            if(maxNbrOfMatches == 1)
            {
                break;
            }
        }
    }

    if ((GP_NVM_ELEMENT_ACCESS_TYPE_RESTORE == accessType) &&
        freeLutAfterUse)
    {
        gpNvm_FreeLookup(lutHandle);
    }
    GP_ASSERT_DEV_EXT(tagFound);
}
/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
void gpNvm_Init(void)
{
#ifdef GP_NVM_USE_ASSERT_SAFETY_NET
    Nvm_EnableSafetyNet();
#endif

    gpNvm_InitProtection();

    Nvm_Init();

#if defined(GP_DIVERSITY_GPHAL_K8E)
    Nvm_BuildTagCache();
#endif


    gpNvm_RegisterElement(&gpNvm_NvmElement_NvmType);
    //Filling in static var for consistency checker
    gpNvm_Restore(GP_COMPONENT_ID, GP_NVM_TYPE_UNIQUE_TAG_ID, NULL);

    gpSched_ScheduleEvent(0, Nvm_CheckConsistency);
}

void gpNvm_DeInit(void)
{
    MEMSET(&Nvm_Tags, 0, sizeof(Nvm_Tags));

#if defined(GP_DIVERSITY_GPHAL_K8E)
    Nvm_DeInit();
#endif
}

gpNvm_Result_t gpNvm_GetNvmType(UInt8* pNvmType)
{
    GP_ASSERT_DEV_EXT(pNvmType);
    gpNvm_Restore(GP_COMPONENT_ID, GP_NVM_TYPE_UNIQUE_TAG_ID, pNvmType);
    return gpNvm_Result_DataAvailable;
}

Bool gpNvm_CheckAccessible(void)
{
    return Nvm_CheckAccessible();
}

void gpNvm_ClearNvm(void)
{
    UInt8 i;
    const ROM gpNvm_IdentifiableTag_t * tags[GP_NVM_NBR_OF_UNIQUE_TAGS];
    UInt8 numRegisteredTags = 0;

    /* Save the registration information localy and then void it */
    for(i = 0; i < GP_NVM_NBR_OF_UNIQUE_TAGS; i++)
    {
        tags[i] = Nvm_Tags[i];
        if(NULL != tags[i])
        {
            numRegisteredTags++;
        }
        else
        {
            break;
        }
        Nvm_Tags[i] = NULL;
    }

    //Erase the flash structure
    Nvm_TagIf_Erase();

    //Register again the tags (from local copy). this wil reconstruct the LUTs in NVM.
    //This procedure also purges all of the tags that existed in versions running on this platform
    //other than the original and current
    for(i = 0; i < GP_NVM_NBR_OF_UNIQUE_TAGS; i++)
    {
        if (Nvm_Tags[i] == &gpNvm_NvmElement_NvmType)
        {
            /* skip nvm type for now */
            continue;
        }
        if(NULL == tags[i])
        {
            break;
        }

        gpNvm_RegisterElement(tags[i]);
    }

    /* clear will process Nvm_Tags[] in order, we need to write the nvmtype at
     * the end, since it acts as a clear-complete marker */
    gpNvm_RegisterElement(&gpNvm_NvmElement_NvmType);

    gpNvm_Clear(gpNvm_AllComponents, gpNvm_AllTags);

    gpNvm_Flush();
}

void gpNvm_Flush(void)
{
    Nvm_Flush();
}

void gpNvm_RegisterElement(const ROM gpNvm_IdentifiableTag_t* pTag)
{
    gpNvm_KeyIndex_t i;
    gpNvm_IdentifiableTag_t nvmTag;

    GP_NVM_REGISTER_ELEMENT_PROTECTED_ENTRY();

    GP_ASSERT_DEV_EXT(NULL != pTag);

    MEMCPY_P(&nvmTag, pTag, sizeof(gpNvm_IdentifiableTag_t));

    /* Verify macro tricks did not result in an element table having double uniqueTagId's */
    for(i = 0; i < GP_NVM_NBR_OF_UNIQUE_TAGS; i++)
    {
        gpNvm_IdentifiableTag_t existingNvmTag;
        if(NULL == Nvm_Tags[i])
        {
            break;
        }
        if (Nvm_Tags[i] == pTag) {
            // allow re-registration
            break;
        }
        MEMCPY_P(&existingNvmTag, Nvm_Tags[i], sizeof(gpNvm_IdentifiableTag_t));

        GP_ASSERT_SYSTEM(nvmTag.uniqueTagId != existingNvmTag.uniqueTagId);
    }

    /* Add the identifiableTag to our list */
    for(i = 0; i < GP_NVM_NBR_OF_UNIQUE_TAGS; i++)
    {
        if(NULL == Nvm_Tags[i]
        || (Nvm_Tags[i] == pTag) // allow re-registration
        ) {
            Nvm_Tags[i] = pTag;
            break;
        }
    }

    GP_ASSERT_DEV_EXT(i < GP_NVM_NBR_OF_UNIQUE_TAGS); // no more free tags available
#if GP_NVM_NBR_OF_UNIQUE_TAGS > 255
    GP_ASSERT_DEV_EXT(i < 0x100); // TagIf limitation
#endif
    GP_NVM_REGISTER_ELEMENT_PROTECTED_EXIT();
}

void gpNvm_RegisterElements(const ROM gpNvm_IdentifiableTag_t* pTag, UInt8 nbrOfTags)
{
    while(nbrOfTags > 0)
    {
        gpNvm_RegisterElement(pTag++);
        nbrOfTags--;
    }
}

void gpNvm_Backup(UInt8 componentId, UInt8 tagId, UInt8* pRamLocation)
{
    Nvm_Access(componentId, tagId, pRamLocation, GP_NVM_ELEMENT_ACCESS_TYPE_BACKUP);
}

void gpNvm_Restore(UInt8 componentId, UInt8 tagId, UInt8* pRamLocation)
{
    // GP_DIVERSITY_NVM_STUB configuration uses Nvm_Access
    // w/GP_NVM_ELEMENT_ACCESS_TYPE_RESTORE to call default initializers
    Nvm_Access(componentId, tagId, pRamLocation, GP_NVM_ELEMENT_ACCESS_TYPE_RESTORE);
}

void gpNvm_Clear(UInt8 componentId, UInt8 tagId)
{
    Nvm_Access(componentId, tagId, NULL, GP_NVM_ELEMENT_ACCESS_TYPE_CLEAR);
}


