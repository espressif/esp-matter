/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * gpHal_MAC.c
 *   This file contains the implementation of the MAC functions
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



/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
//#define GP_LOCAL_LOG

#include "gpPd.h"

#include "gpHal.h"
#include "gpHal_DEFS.h"
#include "gpHal_Ble.h"

//GP hardware dependent register definitions
#include "gpHal_HW.h"
#include "gpHal_reg.h"
#include "gpHal_Ble_DEFS.h"
#include "gpHal_BleWl_defs.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

// In this implementation, all entries in whitelist have all PDU types as valid (is also used to indicate valid entries)
#define GPHAL_BLE_WHITELIST_VALID_FT_MASK       0xFF

#ifdef GP_COMP_CHIPEMU
extern UInt32 gpChipEmu_GetGpMicroStructBleWhiteListStart(UInt32 gp_mm_ram_linear_start);
#define GP_HAL_BLE_WHITELIST_START           gpChipEmu_GetGpMicroStructBleWhiteListStart(GP_MM_RAM_LINEAR_START)
#else
#define GP_HAL_BLE_WHITELIST_START          ((UIntPtr)&gpHal_BleWl_WhitelistMemory[0])
#endif // GP_COMP_CHIPEMU

#define GP_HAL_BLE_WHITELIST_ENTRY_SIZE                      8

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

#define GPHAL_BLE_WHITELIST_ID_TO_OFFSET(id)  (GP_HAL_BLE_WHITELIST_START + GP_HAL_BLE_WHITELIST_ENTRY_SIZE*id)

#define GPHAL_BLE_WHITELIST_ENTRY_VALID_OFFSET(offset)   (GP_WB_READ_BLE_WHITELIST_ENTRY_VALID_FRAME_TYPE_MASK(offset) != 0)
#define GPHAL_BLE_WHITELIST_ENTRY_VALID(id)              (GPHAL_BLE_WHITELIST_ENTRY_VALID_OFFSET(GPHAL_BLE_WHITELIST_ID_TO_OFFSET(id)))

#define GP_HAL_BLE_IS_REGULAR_ENTRY(statemask)           (statemask & GP_WB_BLE_WHITELIST_ENTRY_S_ENTRY_TYPE_MASK)
#define GP_HAL_BLE_IS_SPECIAL_ENTRY(statemask)           !(statemask & GP_WB_BLE_WHITELIST_ENTRY_S_ENTRY_TYPE_MASK)

// Macro for converting an address type to a mask entry
#define GPHAL_BLE_WHITELIST_ADDRESS_TYPE_TO_MASK(type)    (type + 1)
#define GPHAL_BLE_WHITELIST_MASK_TO_ADDRESS_TYPE(type)    (type - 1)

#define GPBLE_WHITELIST_SET_PROPERTY(prop, mask)  (prop |= mask)
#define GPBLE_WHITELIST_CLR_PROPERTY(prop, mask)  (prop &= ~mask)

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

//Placeholder to allocate space to store whitelist in lower ram part
#ifndef GP_COMP_CHIPEMU
/*
 * compile time verification of info structures
 * NOTE: Whitelist Entry size is not written to RT, so changes to GP_HAL_BLE_WHITELIST_ENTRY_SIZE
 *       must be applied in RT too (rx_filter.c).
 */
GP_COMPILE_TIME_VERIFY((GP_HAL_BLE_WHITELIST_ENTRY_SIZE % GP_WB_MAX_MEMBER_SIZE) == 0);
GP_COMPILE_TIME_VERIFY(GP_HAL_BLE_WHITELIST_ENTRY_SIZE >= GP_WB_BLE_WHITELIST_ENTRY_SIZE);

COMPILER_ALIGNED(GP_WB_MAX_MEMBER_SIZE) static UInt8 gpHal_BleWl_WhitelistMemory[GP_HAL_BLE_WHITELIST_ENTRY_SIZE*GP_HAL_BLE_MAX_NR_OF_WHITELIST_ENTRIES] LINKER_SECTION(".lower_ram_retain_gpmicro_accessible");
#endif // GP_COMP_CHIPEMU

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/



/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

Bool gpHal_CheckWhitelistRange(UInt8 rangeStart, UInt8 rangeStop)
{
    if(rangeStart > rangeStop)
    {
        return false;
    }

    if(rangeStop >= GP_HAL_BLE_MAX_NR_OF_WHITELIST_ENTRIES)
    {
        return false;
    }

    return true;
}

UInt8 gpHal_BleFindWhiteListEntry(UInt8 addressType, BtDeviceAddress_t* pAddress, UInt8 start, UInt8 stop)
{
    UInt8 i;

    for(i = start; i <= stop; i++)
    {
        gpHal_Address_t offset = GPHAL_BLE_WHITELIST_ID_TO_OFFSET(i);
        UInt64 wlAddress = GP_WB_READ_BLE_WHITELIST_ENTRY_DEVICE_ADDRESS(offset);

        if(!GPHAL_BLE_WHITELIST_ENTRY_VALID_OFFSET(offset))
        {
            continue;
        }

        if(MEMCMP(pAddress->addr, &wlAddress, sizeof(BtDeviceAddress_t)) != 0)
        {
            continue;
        }

        if(GPHAL_BLE_WHITELIST_ADDRESS_TYPE_TO_MASK(addressType) != GP_WB_READ_BLE_WHITELIST_ENTRY_ADDRESS_TYPE(offset))
        {
            continue;
        }

        return i;
    }
    return GP_HAL_BLE_WL_ID_INVALID;
}

#ifdef GP_DIVERSITY_GPHAL_WHITELIST_UPDATE_CALLBACK
void gpHal_NotifyIndexUpdate(UInt8 index, Bool isValid)
{
    BtDeviceAddress_t address;
    UInt8 addressType;

    gpHal_Address_t offset = GPHAL_BLE_WHITELIST_ID_TO_OFFSET(index);
    UInt64 wlAddress = GP_WB_READ_BLE_WHITELIST_ENTRY_DEVICE_ADDRESS(offset);
    MEMCPY(address.addr, &wlAddress, GP_WB_BLE_WHITELIST_ENTRY_DEVICE_ADDRESS_LEN);
    addressType = GPHAL_BLE_WHITELIST_MASK_TO_ADDRESS_TYPE(GP_WB_READ_BLE_WHITELIST_ENTRY_ADDRESS_TYPE(offset));

    //GP_LOG_PRINTF("update_wl_index: %x %x%x %x", 0, index, (UInt32)(wlAddress>>32), (UInt32)wlAddress, addressType);

    gpHal_BleWl_cbUpdateIndex(addressType, &address, isValid ? index : GP_HAL_BLE_WL_ID_INVALID);
}
#endif // GP_DIVERSITY_GPHAL_WHITELIST_UPDATE_CALLBACK

void gpHal_BleAddToWhiteList(UInt8 index, gpHal_WhiteListEntry_t* pEntry)
{
    gpHal_Address_t offset = GPHAL_BLE_WHITELIST_ID_TO_OFFSET(index);
    UInt8 wlLength;

    wlLength = GP_WB_READ_BLEFILT_WHITELIST_LENGTH();

    // First write valid states (overwrites address type)
    GP_WB_WRITE_BLE_WHITELIST_ENTRY_VALID_STATE_MASK_ADDRESS_TYPE(offset, pEntry->stateMask);
    // Write address type
    GP_WB_WRITE_BLE_WHITELIST_ENTRY_ADDRESS_TYPE(offset, GPHAL_BLE_WHITELIST_ADDRESS_TYPE_TO_MASK(pEntry->addressType));

    GP_WB_WRITE_BLE_WHITELIST_ENTRY_VALID_FRAME_TYPE_MASK(offset, GPHAL_BLE_WHITELIST_VALID_FT_MASK);

    GP_HAL_WRITE_BYTE_STREAM(offset + GP_WB_BLE_WHITELIST_ENTRY_DEVICE_ADDRESS_ADDRESS, pEntry->address.addr, sizeof(BtDeviceAddress_t));

    if(index >= wlLength)
    {
        wlLength = (index + 1);
        GP_WB_WRITE_BLEFILT_WHITELIST_LENGTH(wlLength);
    }

#ifdef GP_DIVERSITY_GPHAL_WHITELIST_UPDATE_CALLBACK
    gpHal_NotifyIndexUpdate(index, true);
#endif
}

void gpHal_RemoveFromWhiteList(UInt8 index, gpHal_WhiteListEntry_t* pEntry)
{
    NOT_USED(pEntry);
    gpHal_BleClearWhiteList(index,index);
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpHal_BleWlInit(void)
{
    UInt32 lAddr = GP_HAL_BLE_WHITELIST_START - GP_MM_RAM_LINEAR_START;
#ifndef GP_COMP_CHIPEMU
    NOT_USED(gpHal_BleWl_WhitelistMemory);
#endif // GP_COMP_CHIPEMU

    GP_WB_WRITE_BLEFILT_WHITELIST_BASE_ADDRESS((UInt16)lAddr);
    GP_WB_WRITE_BLEFILT_WHITELIST_LENGTH(0);
}

gpHal_Result_t gpHal_BleAddDeviceToWhiteList(gpHal_WhiteListEntry_t* pEntry, UInt8 rangeStart, UInt8 rangeStop)
{
    UIntLoop i;
    UInt8 currentEntryIndex;

    if(!gpHal_CheckWhitelistRange(rangeStart, rangeStop))
    {
        return gpHal_ResultInvalidParameter;
    }

    currentEntryIndex = gpHal_BleFindWhiteListEntry(pEntry->addressType, &pEntry->address, rangeStart, rangeStop);

    if(currentEntryIndex != GP_HAL_BLE_WL_ID_INVALID)
    {
        // Already present, overwrite current item
        gpHal_BleAddToWhiteList(currentEntryIndex, pEntry);
        return gpHal_ResultSuccess;
    }

    // Item not present yet, add on first available location
    for(i = rangeStart; i <= rangeStop; i++)
    {
        if(!GPHAL_BLE_WHITELIST_ENTRY_VALID(i))
        {
            gpHal_BleAddToWhiteList(i, pEntry);
            return gpHal_ResultSuccess;
        }
    }

    return gpHal_ResultBusy;
}
void gpHal_BleClearWhiteList(UInt8 rangeStart, UInt8 rangeStop)
{
    UIntLoop i;
    UInt8 wlLength;

    // Consider return iso assert
    GP_ASSERT_DEV_EXT(gpHal_CheckWhitelistRange(rangeStart, rangeStop));

    GP_LOG_PRINTF("gpHal_BleClearWhiteList: %x,%x",0,rangeStart,rangeStop);
#ifdef GP_DIVERSITY_GPHAL_WHITELIST_UPDATE_CALLBACK
    for(i = rangeStart; i <= rangeStop; i++)
    {
        if (GPHAL_BLE_WHITELIST_ENTRY_VALID(i))
        {
            BtDeviceAddress_t address;
            UInt8 addressType;
            gpHal_Address_t offset = GPHAL_BLE_WHITELIST_ID_TO_OFFSET(i);
            UInt64 wlAddress = GP_WB_READ_BLE_WHITELIST_ENTRY_DEVICE_ADDRESS(offset);
            MEMCPY(address.addr, &wlAddress, GP_WB_BLE_WHITELIST_ENTRY_DEVICE_ADDRESS_LEN);
            addressType = GPHAL_BLE_WHITELIST_MASK_TO_ADDRESS_TYPE(GP_WB_READ_BLE_WHITELIST_ENTRY_ADDRESS_TYPE(offset));

            UInt8 new_idx = GP_HAL_BLE_WL_ID_INVALID;
            if (0<rangeStart)
            {
                new_idx = gpHal_BleFindWhiteListEntry(addressType, &address, 0, rangeStart-1);
                GP_LOG_PRINTF("[0,%x]->%x",0,rangeStart-1,new_idx);
            }
            if (GP_HAL_BLE_WL_ID_INVALID == new_idx && gpHal_CheckWhitelistRange(rangeStop+1, GP_HAL_BLE_MAX_NR_OF_WHITELIST_ENTRIES-1) )
            {
                new_idx = gpHal_BleFindWhiteListEntry(addressType, &address, rangeStop+1, GP_HAL_BLE_MAX_NR_OF_WHITELIST_ENTRIES-1);
                GP_LOG_PRINTF("[%x,%x]->%x",0,rangeStop+1,GP_HAL_BLE_MAX_NR_OF_WHITELIST_ENTRIES-1,new_idx);
            }

            gpHal_NotifyIndexUpdate(GP_HAL_BLE_WL_ID_INVALID == new_idx ? i : new_idx, GP_HAL_BLE_WL_ID_INVALID != new_idx);
        }
    }
#endif


#ifdef GP_COMP_CHIPEMU
{
    UInt32 iLoop;
    UInt32 byteLen = ((rangeStop-rangeStart+1)*GP_HAL_BLE_WHITELIST_ENTRY_SIZE);
    for (iLoop=0; iLoop < byteLen / sizeof(UInt32); ++iLoop)
    {
        GP_WB_WRITE_U32(GPHAL_BLE_WHITELIST_ID_TO_OFFSET(rangeStart)+(iLoop*sizeof(UInt32)),0);
    }
}
#else
    MEMSET((void*)(GPHAL_BLE_WHITELIST_ID_TO_OFFSET(rangeStart)),0,(rangeStop-rangeStart+1)*GP_HAL_BLE_WHITELIST_ENTRY_SIZE);
#endif

    wlLength = GP_WB_READ_BLEFILT_WHITELIST_LENGTH();
    /* Only update the length if the tail of the list was removed */
    if (wlLength <= (rangeStop+1))
    {
        // Calculate index of new last valid item of list
        wlLength = 0;
        for(i =  0; i < rangeStart; i++)
        {
            if(GPHAL_BLE_WHITELIST_ENTRY_VALID(i))
            {
                wlLength = i+1;
            }
        }
    }

    GP_WB_WRITE_BLEFILT_WHITELIST_LENGTH(wlLength);
}

Bool gpHal_BleIsWhiteListEntryValid(UInt8 id)
{
    GP_ASSERT_DEV_EXT(id < GP_HAL_BLE_MAX_NR_OF_WHITELIST_ENTRIES);

    return GPHAL_BLE_WHITELIST_ENTRY_VALID(id);
}

gpHal_Result_t gpHal_UpdateWhiteListEntryState(UInt8 id, UInt8 state, Bool set)
{
    GP_ASSERT_DEV_EXT(id < GP_HAL_BLE_MAX_NR_OF_WHITELIST_ENTRIES);

    if(id != GP_HAL_BLE_WL_ID_INVALID)
    {
        gpHal_Address_t offset = GPHAL_BLE_WHITELIST_ID_TO_OFFSET(id);
        UInt8 applicableStates = GP_WB_READ_BLE_WHITELIST_ENTRY_VALID_STATE_MASK_ADDRESS_TYPE(offset);

        if(set)
        {
            GPBLE_WHITELIST_SET_PROPERTY(applicableStates, state);
        }
        else
        {
            GPBLE_WHITELIST_CLR_PROPERTY(applicableStates, state);
        }

        GP_WB_WRITE_BLE_WHITELIST_ENTRY_VALID_STATE_MASK_ADDRESS_TYPE(offset, applicableStates);
        return gpHal_ResultSuccess;
    }

    return gpHal_ResultBusy;
}

#ifdef GP_DIVERSITY_GPHAL_WHITELIST_UPDATE_CALLBACK
UInt8 gpHal_BleWl_FindEntry(UInt8 addressType, BtDeviceAddress_t *pAddress)
{
    UInt8 wl_len = GP_WB_READ_BLEFILT_WHITELIST_LENGTH();
    if (0 == wl_len)
    {
        return GP_HAL_BLE_WL_ID_INVALID;
    }
    return gpHal_BleFindWhiteListEntry(addressType, pAddress, 0, wl_len-1);
}
#endif // GP_DIVERSITY_GPHAL_WHITELIST_UPDATE_CALLBACK

UInt8 gpHal_BleGetWhitelistEntryState(UInt8 id)
{
    UInt8 state = 0x00;

    if(id != GP_HAL_BLE_WL_ID_INVALID)
    {
        gpHal_Address_t offset = GPHAL_BLE_WHITELIST_ID_TO_OFFSET(id);
        state = GP_WB_READ_BLE_WHITELIST_ENTRY_VALID_STATE_MASK_ADDRESS_TYPE(offset) & ~GP_WB_BLE_WHITELIST_ENTRY_ADDRESS_TYPE_MASK;
    }

    //GP_LOG_PRINTF("entry state: %d", 0, state);

    return state;
}

gpHal_Result_t gpHal_BleGetWhitelistEntry(UInt8 id, gpHal_WhiteListEntry_t* pEntry)
{
    UInt8 ret = gpHal_ResultInvalidParameter;

    if(id != GP_HAL_BLE_WL_ID_INVALID)
    {
        gpHal_Address_t offset = GPHAL_BLE_WHITELIST_ID_TO_OFFSET(id);
        UInt64 wlAddress = GP_WB_READ_BLE_WHITELIST_ENTRY_DEVICE_ADDRESS(offset);

        MEMCPY(&pEntry->address, &wlAddress, sizeof(BtDeviceAddress_t));
        pEntry->addressType = GPHAL_BLE_WHITELIST_MASK_TO_ADDRESS_TYPE(GP_WB_READ_BLE_WHITELIST_ENTRY_ADDRESS_TYPE(offset));
        pEntry->stateMask   = GP_WB_READ_BLE_WHITELIST_ENTRY_VALID_STATE_MASK_ADDRESS_TYPE(offset) & ~GP_WB_BLE_WHITELIST_ENTRY_ADDRESS_TYPE_MASK;

        ret = gpHal_ResultSuccess;
    }

    return ret;
}

gpHal_Result_t gpHal_BleUpdateWhiteListEntryState(UInt8 addressType, BtDeviceAddress_t* pAddress, UInt8 stateMask_mask, UInt8 stateMask_value, Bool createIfNotExist)
{
    GP_LOG_PRINTF("--- WL update ---", 0);

    gpHal_WhiteListEntry_t wlEntry;
    gpHal_Result_t result = gpHal_ResultSuccess;

    wlEntry.addressType = addressType;
    wlEntry.stateMask   = stateMask_value;
    MEMCPY(&wlEntry.address, pAddress, sizeof(BtDeviceAddress_t));

    UInt8 regular_cnt = 0;
    UInt8 special_cnt = 0;
    for(UInt8 i=0; i<GP_HAL_BLE_MAX_NR_OF_WHITELIST_ENTRIES; i++)
    {
        UInt8 entryStateMask = gpHal_BleGetWhitelistEntryState(i);
        if(entryStateMask != 0x00)
        {
            GP_HAL_BLE_IS_REGULAR_ENTRY(entryStateMask) ? regular_cnt++ : special_cnt++;
        }
    }

    GP_LOG_PRINTF("regular entries cnt: %d", 0, regular_cnt);
    GP_LOG_PRINTF("special entries cnt: %d", 0, special_cnt);

    UInt8 idx = gpHal_BleFindWhiteListEntry(wlEntry.addressType, &wlEntry.address, 0x00, GP_HAL_BLE_MAX_NR_OF_WHITELIST_ENTRIES-1);
    if(idx != GP_HAL_BLE_WL_ID_INVALID) // entry exists
    {
        UInt8 stateMask_old = gpHal_BleGetWhitelistEntryState(idx);
        UInt8 stateMask_new = (stateMask_old & ~stateMask_mask) | (stateMask_value & stateMask_mask);

        GP_LOG_PRINTF("old state mask: 0x%02x", 0, stateMask_old);
        GP_LOG_PRINTF("state mask: 0x%02x", 0, stateMask_mask);
        GP_LOG_PRINTF("new state mask: 0x%02x", 0, stateMask_new);

        if(!stateMask_new)
        {
            gpHal_RemoveFromWhiteList(idx, &wlEntry);
            result = gpHal_ResultSuccess;
        }
        else if(GP_HAL_BLE_IS_REGULAR_ENTRY(stateMask_old) && GP_HAL_BLE_IS_SPECIAL_ENTRY(stateMask_mask))
        {
            if(special_cnt < GP_HAL_BLE_MAX_NR_OF_WL_CONTROLLER_SPECIFIC_ENTRIES)
            {
                GP_LOG_PRINTF("updating entry...", 0);

                gpHal_Address_t offset = GPHAL_BLE_WHITELIST_ID_TO_OFFSET(idx);
                UInt8 addrType = (GP_WB_READ_BLE_WHITELIST_ENTRY_VALID_STATE_MASK_ADDRESS_TYPE(offset) & GP_WB_BLE_WHITELIST_ENTRY_ADDRESS_TYPE_MASK);
                GP_WB_WRITE_BLE_WHITELIST_ENTRY_VALID_STATE_MASK_ADDRESS_TYPE(offset, addrType | stateMask_new);

                result = gpHal_ResultSuccess;
            }
            else
            {
                result = gpHal_ResultBusy;
            }
        }
        else if(GP_HAL_BLE_IS_SPECIAL_ENTRY(stateMask_old) && GP_HAL_BLE_IS_REGULAR_ENTRY(stateMask_mask))
        {
            if(regular_cnt < (GP_HAL_BLE_MAX_NR_OF_WHITELIST_ENTRIES - GP_HAL_BLE_MAX_NR_OF_WL_CONTROLLER_SPECIFIC_ENTRIES))
            {
                GP_LOG_PRINTF("updating entry...", 0);

                gpHal_Address_t offset = GPHAL_BLE_WHITELIST_ID_TO_OFFSET(idx);
                UInt8 addrType = (GP_WB_READ_BLE_WHITELIST_ENTRY_VALID_STATE_MASK_ADDRESS_TYPE(offset) & GP_WB_BLE_WHITELIST_ENTRY_ADDRESS_TYPE_MASK);
                GP_WB_WRITE_BLE_WHITELIST_ENTRY_VALID_STATE_MASK_ADDRESS_TYPE(offset, addrType | stateMask_new);

                result = gpHal_ResultSuccess;
            }
            else
            {
                result = gpHal_ResultBusy;
            }
        }
        else
        {
            GP_LOG_PRINTF("updating entry...", 0);

            gpHal_Address_t offset = GPHAL_BLE_WHITELIST_ID_TO_OFFSET(idx);
            UInt8 addrType = (GP_WB_READ_BLE_WHITELIST_ENTRY_VALID_STATE_MASK_ADDRESS_TYPE(offset) & GP_WB_BLE_WHITELIST_ENTRY_ADDRESS_TYPE_MASK);
            GP_WB_WRITE_BLE_WHITELIST_ENTRY_VALID_STATE_MASK_ADDRESS_TYPE(offset, addrType | stateMask_new);;
        }
    }
    else if(createIfNotExist) // create new entry
    {
        if(stateMask_value != 0x00)
        {
            if(GP_HAL_BLE_IS_REGULAR_ENTRY(stateMask_value))
            {
                GP_LOG_PRINTF("creating regular entry with state mask: 0x%02x", 0, wlEntry.stateMask);

                if(regular_cnt < (GP_HAL_BLE_MAX_NR_OF_WHITELIST_ENTRIES - GP_HAL_BLE_MAX_NR_OF_WL_CONTROLLER_SPECIFIC_ENTRIES))
                {
                    //add regular entry
                    result = gpHal_BleAddDeviceToWhiteList(&wlEntry, 0x00, GP_HAL_BLE_MAX_NR_OF_WHITELIST_ENTRIES-1);
                }
                else
                {
                    result = gpHal_ResultBusy;
                }
            }
            else
            {
                GP_LOG_PRINTF("creating special entry with state mask: 0x%02x", 0, wlEntry.stateMask);

                if(special_cnt < GP_HAL_BLE_MAX_NR_OF_WL_CONTROLLER_SPECIFIC_ENTRIES)
                {
                    //add special entry
                    result = gpHal_BleAddDeviceToWhiteList(&wlEntry, 0x00, GP_HAL_BLE_MAX_NR_OF_WHITELIST_ENTRIES-1);
                }
                else
                {
                    result = gpHal_ResultBusy;
                }
            }
        }
        else
        {
            result = gpHal_ResultSuccess;
        }
    }

    return result;
}

void gpHal_BleUpdateWhiteListEntryStateBulk(UInt8 matchMask, UInt8 stateMask_mask, UInt8 stateMask_value)
{
    GP_LOG_PRINTF("--- WL bulk update ---", 0);
    GP_LOG_PRINTF("matchMask: 0x%02x stateMask_mask: 0x%02x stateMask_value: 0x%02x", 0, matchMask, stateMask_mask, stateMask_value);

    for(UInt8 i=0; i<GP_HAL_BLE_MAX_NR_OF_WHITELIST_ENTRIES; i++)
    {
        UInt8 stateMask = gpHal_BleGetWhitelistEntryState(i);
        if((stateMask != 0x00) && (matchMask & stateMask))
        {
            UInt8 stateMask_new = (stateMask & ~stateMask_mask) | (stateMask_value & stateMask_mask);

            GP_LOG_PRINTF("updating entry [%d] ...", 0, i);
            GP_LOG_PRINTF("old state mask: 0x%02x", 0, stateMask);
            GP_LOG_PRINTF("new state mask: 0x%02x", 0, stateMask_new);

            if(stateMask_new)
            {
                gpHal_Address_t offset = GPHAL_BLE_WHITELIST_ID_TO_OFFSET(i);
                UInt8 addrType = (GP_WB_READ_BLE_WHITELIST_ENTRY_VALID_STATE_MASK_ADDRESS_TYPE(offset) & GP_WB_BLE_WHITELIST_ENTRY_ADDRESS_TYPE_MASK);
                GP_WB_WRITE_BLE_WHITELIST_ENTRY_VALID_STATE_MASK_ADDRESS_TYPE(offset, addrType | stateMask_new);
            }
            else
            {
                gpHal_RemoveFromWhiteList(i, NULL);
            }
        }
    }
}
