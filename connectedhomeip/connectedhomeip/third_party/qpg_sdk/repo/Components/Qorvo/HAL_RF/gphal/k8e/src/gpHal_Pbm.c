/*
 * Copyright (c) 2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
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

#include "global.h"
#include "gpHal.h"
#include "gpHal_Pbm.h"
#include "gpHal_DEFS.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

// The maximum amount of pbms supported by the hardware
#define GP_HAL_MAX_NR_OF_PBMS   GP_MM_RAM_PBM_OPTS_NR_OF

// The number of pbms the user has specified (or the defaults)
#define GP_HAL_NR_OF_PBMS_SPECIFIED_BY_USER     (GP_HAL_PBM_TYPE1_AMOUNT + GP_HAL_PBM_TYPE2_AMOUNT)

// The number of pbms that are used in total (user specified + reserved)
#define GP_HAL_NR_OF_PBMS_CONFIGURED            (GP_HAL_NR_OF_PBMS_SPECIFIED_BY_USER + GP_HAL_MAX_NR_OF_RESERVED_PBMS)

// Check that the configured amount of PBMs does not exceed HW amount when PD is using PBM
#if defined(GP_DIVERSITY_PD_USE_PBM_VARIANT)
GP_COMPILE_TIME_VERIFY(GP_HAL_NR_OF_PBMS_CONFIGURED <= GP_HAL_MAX_NR_OF_PBMS);
#endif // GP_DIVERSITY_PD_USE_PBM_VARIANT

#define GP_HAL_PBM_USER_MIN_SIZE        16

#define GP_HAL_TOTAL_RESERVED_PBM_MEMORY    (GP_HAL_MAX_NR_OF_RESERVED_PBMS*GP_HAL_RESERVED_PBM_SIZE)

#define GP_HAL_TOTAL_PBM_MEMORY     (GP_HAL_TOTAL_RESERVED_PBM_MEMORY+(GP_HAL_PBM_TYPE1_AMOUNT*GP_HAL_PBM_TYPE1_SIZE) + (GP_HAL_PBM_TYPE2_AMOUNT*GP_HAL_PBM_TYPE2_SIZE))

#define GP_HAL_FREQOFFSET_HZPERUNIT 244

#define GP_HAL_SENSITIVITY_LEVEL (-101)

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

#define GP_HAL_CALCULATE_START_PBM_ADDRESS(absPBMAddress)           ((absPBMAddress) - ((absPBMAddress) % GP_MM_RAM_PBM_ZIGBEE_DATA_SIZE)) /*per 0x80*/
#define GP_HAL_REMAP_REL_ADDRESS_TO_DATA_SEGMENT(relAddr)           ((relAddr) & 0x7F)

#define GP_HAL_WRITE_PBM_START_TO_MM_ENTRY(index, startAddress)     GP_WB_WRITE_U16(GP_WB_MM_PBM_0_DATA_BASE_ADDRESS_ADDRESS + 2*index, startAddress)

// Compile time verification of pbm options size (restricted by HW to 0x20 per pbm)
GP_COMPILE_TIME_VERIFY(GP_WB_PBM_BLE_FORMAT_R_SIZE <= GP_MM_RAM_PBM_OPTS_SIZE);
GP_COMPILE_TIME_VERIFY(GP_WB_PBM_BLE_FORMAT_T_SIZE <= GP_MM_RAM_PBM_OPTS_SIZE);
GP_COMPILE_TIME_VERIFY(GP_WB_PBM_FORMAT_R_SIZE <= GP_MM_RAM_PBM_OPTS_SIZE);
GP_COMPILE_TIME_VERIFY(GP_WB_PBM_FORMAT_T_SIZE <= GP_MM_RAM_PBM_OPTS_SIZE);

GP_COMPILE_TIME_VERIFY(GP_HAL_PBM_MAX_SIZE % 4 == 0);

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef struct {
    UInt8 pbmHandle;
    Bool txPbm;
    UInt16 pbmLength;
    UInt8* pData;
} gpHal_PbmDescriptor_t;

typedef struct {
    UInt16 size;
    UInt8 amount;
} gpHal_PbmResource_t;

// Make sure the memory fields are aligned on 32 bits!
typedef struct {
    UInt8 memory[GP_HAL_TOTAL_PBM_MEMORY];
    UInt32 memoryAllocated;
} gpHal_PbmAdmin_t;

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

static Bool gpHal_PbmInitialized = false;

const static gpHal_PbmResource_t gpHal_PbmResources[] =
{
    // The first entry is for reserved pbms
    {GP_HAL_RESERVED_PBM_SIZE, GP_HAL_MAX_NR_OF_RESERVED_PBMS},
    // All these entries are configured by the user
    {GP_HAL_PBM_TYPE1_SIZE, GP_HAL_PBM_TYPE1_AMOUNT},
    {GP_HAL_PBM_TYPE2_SIZE, GP_HAL_PBM_TYPE2_AMOUNT}
};

// Context for PBMs
static gpHal_PbmDescriptor_t gpHal_PbmDescriptors[GP_HAL_MAX_NR_OF_RESERVED_PBMS+GP_HAL_PBM_TYPE1_AMOUNT+GP_HAL_PBM_TYPE2_AMOUNT];

// PBM data buffers (need to be stored in lower ram)
static gpHal_PbmAdmin_t gpHal_PbmAdmin LINKER_SECTION(".lower_ram_retain");

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

// pbm configuration

static UInt16 gpHal_PbmGetMaxResourceSize(void);
static void gpHal_PbmCheckConfiguration(void);
static UInt8 gpHal_GetPbmWithMemory(UInt16 size);
static void gpHal_PbmConfigureRxPbms(void);
static void gpHal_PbmConfigureTxPbms(void);

// pbm descriptor handling
static gpHal_PbmDescriptor_t* gpHal_PbmDescriptorAllocate(UInt8 pbmHandle, Bool txPbm, UInt16 pbmLength);
static gpHal_PbmDescriptor_t* gpHal_PbmDescriptorGet(UInt8 pbmHandle);
static void gpHal_PbmDescriptorMallocMemory(gpHal_PbmDescriptor_t* pDesc);
static void gpHal_PbmDescriptorFreeMemory(gpHal_PbmDescriptor_t* pDesc);
static void gpHal_PbmDescriptorFree(gpHal_PbmDescriptor_t* pDesc);

// Various
static void gpHal_PbmSetDefaultOptions(UInt8 pbmHandle);
static void gpHal_PbmGetMemoryIndexRange(UInt16 size, UInt8* pStart, UInt8* pEnd, UInt16* pAccumulatedSize);
static UInt16 gpHal_GetRssiAdcVrefCalValue(void);
static Int8 gpHal_CalculateIntegratingModeRSSI(UInt8 protoRSSI);
static UInt8 gpHal_GetIntegratingModeLQI(UInt8 PBMentry);
static Int8 gpHal_GetIntegratingModeRSSI(UInt8 PBMentry);
static Int8 gpHal_GetContinuousModeRSSI(UInt8 PBMentry);
static UInt8 gpHal_GetContinuousModeLQI(UInt8 PBMentry);


/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/


void gpHal_PbmCheckConfiguration(void)
{
    UIntLoop i;
    UInt16 totalMemoryRequested = 0;

    for(i = 0; i < number_of_elements(gpHal_PbmResources); i++)
    {
        // Check if all sizes are aligned on 4 bytes
        if(gpHal_PbmResources[i].size % 4 != 0)
        {
            GP_LOG_SYSTEM_PRINTF("PBM size %i is no multiple of 4",0,gpHal_PbmResources[i].size);
            GP_ASSERT_SYSTEM(false);
        }

        if(i != 0)
        {
            // Check if sizes are specified in ascending order
            if(gpHal_PbmResources[i-1].size >= gpHal_PbmResources[i].size)
            {
                GP_LOG_SYSTEM_PRINTF("PBM sizes should be specified in ascending order",0);
                GP_ASSERT_SYSTEM(false);
            }

            if(gpHal_PbmResources[i].size < GP_HAL_PBM_USER_MIN_SIZE)
            {
                GP_LOG_SYSTEM_PRINTF("PBM min sizes should be %i",0,GP_HAL_PBM_USER_MIN_SIZE);
                GP_ASSERT_SYSTEM(false);
            }
        }

        totalMemoryRequested += (gpHal_PbmResources[i].amount*gpHal_PbmResources[i].size);
    }

    if(totalMemoryRequested != sizeof(gpHal_PbmAdmin.memory))
    {
        GP_LOG_SYSTEM_PRINTF("memory mismatch, bytes req: %i, alloc: %i",0,totalMemoryRequested, (int)sizeof(gpHal_PbmAdmin.memory));
        GP_ASSERT_DEV_INT(false);
    }
}

UInt8 gpHal_GetPbmWithMemory(UInt16 size)
{
    UInt8 handle;

    handle = GP_WB_READ_PBM_ADM_PBM_ENTRY_CLAIM();

    // Allocated and valid check?
    if(GP_HAL_CHECK_PBM_VALID(handle))
    {
        gpHal_PbmDescriptor_t* pDesc;

        pDesc = gpHal_PbmDescriptorAllocate(handle, true, size);

        if(pDesc == NULL)
        {
            // Failed to allocate PBM descriptor, free handle again
            GP_WB_WRITE_PBM_ADM_PBM_ENTRY_RETURN(handle);
            handle = GP_PBM_INVALID_HANDLE;
        }
    }

    return handle;
}

UInt16 gpHal_PbmGetMaxResourceSize(void)
{
    UIntLoop i;
    UInt16 maxSize;

    maxSize = 0;
    for (i = 0; i < number_of_elements(gpHal_PbmResources); i++)
    {
        if(gpHal_PbmResources[i].amount)
        {
            maxSize = max(maxSize,gpHal_PbmResources[i].size);
        }
    }

    return maxSize;
}

void gpHal_PbmConfigureRxPbms(void)
{
    UIntLoop i;
    UInt32 rxPbmMask = 0;

    // Pre-configure RX PBMs
    for(i = 0; i < GP_HAL_NR_OF_RX_PBMS; i++)
    {
        // Allocate long-life descriptor for RX PBMs
        gpHal_PbmDescriptor_t* pDesc;

        //Rx wants largest PBM's
        pDesc = gpHal_PbmDescriptorAllocate(i, false, gpHal_PbmGetMaxResourceSize());
        GP_ASSERT_DEV_EXT(pDesc != NULL);

        BIT_SET(rxPbmMask, i);
    }

    // Update the PBM list that the hardware can claim
    GP_WB_WRITE_PBM_ADM_PBM_ENTRY_RXMAC_MASK(rxPbmMask);
}

void gpHal_PbmConfigureTxPbms(void)
{
    // All pbms that are not reserved for RX, can be claimed for TX
    // This includes special PBMs like eg for calibration or ACK
    UInt32 txPbmMask = 0;
    UIntLoop i;

    // Add pbms to tx mask
    for(i = GP_HAL_NR_OF_RX_PBMS; i < GP_HAL_NR_OF_PBMS_CONFIGURED; i++)
    {
        BIT_SET(txPbmMask,i);

        // Set window to zero if tx pbm is not claimed (to avoid confusion)
        GP_HAL_WRITE_PBM_START_TO_MM_ENTRY(i, 0);
    }

    // Set mm pointers of unused pbms to 0
    for(i = GP_HAL_NR_OF_PBMS_CONFIGURED; i < GP_HAL_MAX_NR_OF_PBMS; i++)
    {
        GP_HAL_WRITE_PBM_START_TO_MM_ENTRY(i, 0);
    }

    GP_WB_WRITE_PBM_ADM_PBM_ENTRY_UC_MASK(txPbmMask);
}

gpHal_PbmDescriptor_t* gpHal_PbmDescriptorAllocate(UInt8 pbmHandle, Bool txPbm, UInt16 pbmLength)
{
    UIntLoop i;
    gpHal_PbmDescriptor_t* pDesc = NULL;

    for(i = 0; i < number_of_elements(gpHal_PbmDescriptors); i++)
    {
        if(gpHal_PbmDescriptors[i].pbmHandle == GP_PBM_INVALID_HANDLE)
        {
            gpHal_PbmDescriptors[i].pbmHandle = pbmHandle;
            gpHal_PbmDescriptors[i].pbmLength = pbmLength;
            gpHal_PbmDescriptors[i].txPbm = txPbm;
            pDesc = &gpHal_PbmDescriptors[i];
            break;
        }
    }

    if(pDesc != NULL)
    {
        UInt16 pbmWindowStart;

        gpHal_PbmDescriptorMallocMemory(pDesc);

        if(pDesc->pData == NULL)
        {
            GP_LOG_SYSTEM_PRINTF("Unable to claim pbm memory for handle: %i (len: %i)",0,pbmHandle,pbmLength);
            // Unable to configure the memory, free descriptor again
            gpHal_PbmDescriptors[i].pbmHandle = GP_PBM_INVALID_HANDLE;
            return NULL;
        }

        if(txPbm)
        {
            // To avoid any garbage data from previous action, make sure to default the complete pbm options memory
            // Can only do this for TX, as RX pbms are not claimed yet when we configure them
            gpHal_PbmSetDefaultOptions(pbmHandle);
        }

        // MM pointer is offset from start (in 4 bytes resolution)
        pbmWindowStart = (UIntPtr)pDesc->pData - (UIntPtr)GP_MM_RAM_LINEAR_START;
        pbmWindowStart >>= 2;

        // Update the window
        GP_HAL_WRITE_PBM_START_TO_MM_ENTRY(pDesc->pbmHandle, pbmWindowStart);
   }

    return pDesc;
}

gpHal_PbmDescriptor_t* gpHal_PbmDescriptorGet(UInt8 pbmHandle)
{
    UIntLoop i;

    for(i = 0; i < number_of_elements(gpHal_PbmDescriptors); i++)
    {
        if(gpHal_PbmDescriptors[i].pbmHandle == pbmHandle)
        {
            return &gpHal_PbmDescriptors[i];
        }
    }

    return NULL;
}

void gpHal_PbmDescriptorMallocMemory(gpHal_PbmDescriptor_t* pDesc)
{
    UIntLoop i;
    UInt8 start;
    UInt8 end;
    UInt16 accumulatedSize;

    GP_ASSERT_DEV_EXT(pDesc->pData == NULL);

    gpHal_PbmGetMemoryIndexRange(pDesc->pbmLength, &start, &end, &accumulatedSize);

    for(i = start; i <= end; i++)
    {
        if(BIT_TST(gpHal_PbmAdmin.memoryAllocated, i) == 0)
        {
            BIT_SET(gpHal_PbmAdmin.memoryAllocated, i);
            //GP_LOG_SYSTEM_PRINTF("accum = %x en tot = %x",0,accumulatedSize,(i-start)*(pDesc->pbmLength));
            pDesc->pData = &gpHal_PbmAdmin.memory[accumulatedSize + (i-start)*(pDesc->pbmLength)];
            //GP_LOG_SYSTEM_PRINTF("start mem = %lx, dataP = %lx",0,gpHal_PbmAdmin.memory, pDesc->pData);
            break;
        }
    }
}

void gpHal_PbmDescriptorFreeMemory(gpHal_PbmDescriptor_t* pDesc)
{
    UIntLoop i;
    UInt8 start;
    UInt8 end;
    UInt16 accumulatedSize;

    GP_ASSERT_DEV_EXT(pDesc->pData != NULL);

    gpHal_PbmGetMemoryIndexRange(pDesc->pbmLength, &start, &end, &accumulatedSize);

    for(i = start; i <= end; i++)
    {
        if(pDesc->pData == &gpHal_PbmAdmin.memory[accumulatedSize + (i-start)*pDesc->pbmLength])
        {
            BIT_CLR(gpHal_PbmAdmin.memoryAllocated, i);
            pDesc->pData = NULL;
            break;
        }
    }

    // Data should be NULL now
    GP_ASSERT_DEV_EXT(pDesc->pData == NULL);
}

void gpHal_PbmDescriptorFree(gpHal_PbmDescriptor_t* pDesc)
{
    gpHal_PbmDescriptorFreeMemory(pDesc);

    // Reset pointer in MM (to indicate pbm is not in use)
    // This causes the pbm data to become different (because the pointer to the actual PBM data is reset).
    // Any read/write done after this statement will result in undefined behavior.
    // Since the pbm is freed, this is actual not a bad thing.
    // However, if you dump the pbm through the regmap interface, the result will also be different.

    GP_HAL_WRITE_PBM_START_TO_MM_ENTRY(pDesc->pbmHandle, 0);
    pDesc->pbmHandle = GP_PBM_INVALID_HANDLE;
}

void gpHal_PbmSetDefaultOptions(UInt8 pbmHandle)
{
    gpHal_Address_t pbmOptAddress;
    UInt8 tempBuffer[GP_MM_RAM_PBM_OPTS_SIZE];

    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(pbmHandle) && GP_HAL_IS_PBM_ALLOCATED(pbmHandle));

    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(pbmHandle);

    MEMSET((UInt8*)tempBuffer, 0, GP_MM_RAM_PBM_OPTS_SIZE);
    GP_HAL_WRITE_BYTE_STREAM(pbmOptAddress, tempBuffer, GP_MM_RAM_PBM_OPTS_SIZE);
}

void gpHal_PbmGetMemoryIndexRange(UInt16 size, UInt8* pStart, UInt8* pEnd, UInt16* pAccumulatedSize)
{
    UIntLoop i;
    UInt8 start = 0;
    UInt8 end = 0;
    UInt16 accumulatedSize = 0;

    for(i = 0; i < number_of_elements(gpHal_PbmResources); i++)
    {
        if(i != 0)
        {
            start += gpHal_PbmResources[i-1].amount;
            accumulatedSize += (gpHal_PbmResources[i-1].amount* gpHal_PbmResources[i-1].size);
        }

        end += gpHal_PbmResources[i].amount;

        if(gpHal_PbmResources[i].size == size)
        {
            break;
        }

    }

    *pStart = start;
    *pEnd = end - 1;
    *pAccumulatedSize = accumulatedSize;
}

UInt16 gpHal_GetRssiAdcVrefCalValue(void)
{
    UInt16 value = GP_WB_READ_NVR_RSSI_ADC_VREF();
    if (value == 0)
    {
       return 1826;
    }
    return value;
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpHal_PbmInit(void)
{
    if(!gpHal_PbmInitialized)
    {
        UIntLoop i;
        UInt8 currentRxPbm;

        // Check if all parameters are correctly configured
        gpHal_PbmCheckConfiguration();


        // Default initialization
        for(i = 0; i < number_of_elements(gpHal_PbmDescriptors); i++)
        {
            gpHal_PbmDescriptors[i].pbmHandle = GP_PBM_INVALID_HANDLE;
            gpHal_PbmDescriptors[i].txPbm = false;
            gpHal_PbmDescriptors[i].pbmLength = 0;
            gpHal_PbmDescriptors[i].pData = NULL;
        }

        gpHal_PbmAdmin.memoryAllocated = 0;

        // Drop current RX pbm (will be configured later on)
        GP_WB_WRITE_PBM_ADM_PBM_ENTRY_RXMAC_MASK(0);
        currentRxPbm = GP_WB_READ_RIB_RX_PBM_PTR();
        GP_WB_RIB_RX_PBM_PTR_CLR();
        GP_WB_WRITE_PBM_ADM_PBM_ENTRY_RETURN(currentRxPbm);

        // Configure RX pbms - pbms that can be claimed by HW
        gpHal_PbmConfigureRxPbms();
        // Configure TX pbms - pbms that can be claimed by UC
        gpHal_PbmConfigureTxPbms();

        gpHal_PbmInitialized = true;
    }
}

UInt8 gpHal_GetHandle(UInt16 size)
{
    UIntLoop i;

    for(i = 0; i < number_of_elements(gpHal_PbmResources); i++)
    {
        // Check the size is supported
        if(size == gpHal_PbmResources[i].size)
        {
            break;
        }
    }

    if(i == number_of_elements(gpHal_PbmResources))
    {
        // Unsupported size specified
        return GP_PBM_INVALID_HANDLE;
    }

    return gpHal_GetPbmWithMemory(size);
}

void gpHal_FreeHandle(UInt8 handle)
{
    gpHal_PbmDescriptor_t* pDesc;

    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(handle) && GP_HAL_IS_PBM_ALLOCATED(handle));

    if(handle == GP_WB_READ_RIB_RX_PBM_PTR() && GP_WB_READ_RIB_RX_PBM_PTR_VALID())
    {
        // This is an indication something is wrong. NRT wants to free the pbm that is currently claimed by the RIB.
        // Doing this will cause unexpected behavior.
        // The assert should be put back when the stack tests are fixed
        GP_LOG_SYSTEM_PRINTF("Error: Both HW and SW are using pbm# %x ",0, handle);
        GP_ASSERT_SYSTEM(false);
    }

    pDesc = gpHal_PbmDescriptorGet(handle);

    GP_ASSERT_DEV_EXT(pDesc != NULL);

    if(pDesc->txPbm)
    {
        // The descriptor should only be freed for TX PBMs, an RX PBM should have its descriptor available at all times
        gpHal_PbmDescriptorFree(pDesc);
    }

    GP_WB_WRITE_PBM_ADM_PBM_ENTRY_RETURN(handle);
}

//Lqi for K7 pbm is invalid, we calculate the lqi from the rssi value
UInt8 gpHal_GetContinuousModeLQI(UInt8 PBMentry)
{
    gpHal_Address_t pbmOptAddress;
    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(PBMentry) && GP_HAL_IS_PBM_ALLOCATED(PBMentry));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);
    {
        Int8 rssi;
        rssi = gpHal_CalculateRSSI(GP_WB_READ_PBM_FORMAT_R_RSSI(pbmOptAddress));
        return gpHal_CalculateLQIfromRSSI(rssi);
    }
}

UInt8 gpHal_GetIntegratingModeLQI(UInt8 PBMentry)
{
    gpHal_Address_t pbmOptAddress;
    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(PBMentry) && GP_HAL_IS_PBM_ALLOCATED(PBMentry));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);
    {
        Int8 rssi;
        rssi = gpHal_CalculateIntegratingModeRSSI(GP_WB_READ_PBM_FORMAT_R_RSSI(pbmOptAddress));
        return gpHal_CalculateLQIfromRSSI(rssi);
    }
}

Int8 gpHal_GetIntegratingModeRSSI(UInt8 PBMentry)
{
    gpHal_Address_t pbmOptAddress;
    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(PBMentry) && GP_HAL_IS_PBM_ALLOCATED(PBMentry));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);
    return (Int8)gpHal_CalculateIntegratingModeRSSI(GP_WB_READ_PBM_FORMAT_R_RSSI(pbmOptAddress));
}

Int8 gpHal_GetRSSI(UInt8 PBMentry)
{
    gpPd_Handle_t pdHandle;
    pdHandle = gpPd_GetPdFromPBM(PBMentry);
    if (!GP_HAL_CHECK_PBM_VALID(pdHandle) || (gpPd_GetPdType(pdHandle) == gpPd_BufferTypeZigBee))
    {
        return gpHal_GetContinuousModeRSSI(PBMentry);
    }
    else
    {
        return gpHal_GetIntegratingModeRSSI(PBMentry);
    }
}

UInt8 gpHal_GetLQI(UInt8 PBMentry)
{
    gpPd_Handle_t pdHandle;
    pdHandle = gpPd_GetPdFromPBM(PBMentry);
    if (!GP_HAL_CHECK_PBM_VALID(pdHandle) || (gpPd_GetPdType(pdHandle) == gpPd_BufferTypeZigBee))
    {
        return gpHal_GetContinuousModeLQI(PBMentry);
    }
    else
    {
        return gpHal_GetIntegratingModeLQI(PBMentry);
    }
}

Int8 gpHal_GetSensitivityLevel(void)
{
    /* can be further optimized to differentiate on the rx mode */
    return GP_HAL_SENSITIVITY_LEVEL;
}

Int8 gpHal_GetContinuousModeRSSI(UInt8 PBMentry)
{
    gpHal_Address_t pbmOptAddress;
    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(PBMentry) && GP_HAL_IS_PBM_ALLOCATED(PBMentry));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);
    return (Int8)gpHal_CalculateRSSI(GP_WB_READ_PBM_FORMAT_R_RSSI(pbmOptAddress));
}

void gpHal_GetRxTimestamp(UInt8 PBMentry, UInt32* pTimeStamp)
{
    gpHal_Address_t pbmOptAddress;

    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(PBMentry) && GP_HAL_IS_PBM_ALLOCATED(PBMentry));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);
    *pTimeStamp = GP_WB_READ_PBM_FORMAT_R_TIMESTAMP(pbmOptAddress);
}

UInt16* gpHal_GetPhaseSamples(UInt8 PBMentry)
{
    gpHal_Address_t pbmOptAddress;
    UInt32 compressedAddr;

    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(PBMentry) && GP_HAL_IS_PBM_ALLOCATED(PBMentry));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);

    compressedAddr = GP_WB_READ_PBM_BLE_FORMAT_R_RAW_PHASE_SAMPLE_PTR(pbmOptAddress);
    if (0 == compressedAddr)
    {
        return (UInt16*)(NULL);
    }

    intptr_t ptr = GP_MM_RAM_ADDR_FROM_COMPRESSED(compressedAddr);
    return (UInt16*)(ptr);
}

UInt8 gpHal_GetRxedChannel(UInt8 PBMentry)
{
    gpHal_Address_t pbmOptAddress;
    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(PBMentry) && GP_HAL_IS_PBM_ALLOCATED(PBMentry));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);
    return GP_WB_READ_PBM_FORMAT_R_CHANNEL(pbmOptAddress) + 11;
}

Int32 gpHal_GetRxedFreqOffset(UInt8 PBMentry)
{
    gpHal_Address_t pbmOptAddress;
    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(PBMentry) && GP_HAL_IS_PBM_ALLOCATED(PBMentry));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);
    return (GP_HAL_FREQOFFSET_HZPERUNIT * GP_WB_READ_PBM_FORMAT_R_FOFFSET(pbmOptAddress));
}

UInt16 gpHal_GetRxedAntenna(UInt8 PBMentry)
{
    gpHal_Address_t pbmOptAddress;
    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(PBMentry) && GP_HAL_IS_PBM_ALLOCATED(PBMentry));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);
    return GP_WB_READ_PBM_FORMAT_R_ANTENNA(pbmOptAddress);
}

void gpHal_GetTxTimestamp(UInt8 PBMentry, UInt32* pTimeStamp)
{
    gpHal_Address_t pbmOptAddress;

    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(PBMentry) && GP_HAL_IS_PBM_ALLOCATED(PBMentry));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);
    *pTimeStamp = GP_WB_READ_PBM_FORMAT_T_TIMESTAMP(pbmOptAddress);
}

UInt8 gpHal_GetTxAckLQI(UInt8 PBMentry)
{
    gpHal_Address_t pbmOptAddress;
    UInt8 lqi = 0;

    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(PBMentry) && GP_HAL_IS_PBM_ALLOCATED(PBMentry));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);

    if (GP_WB_READ_PBM_FORMAT_T_RETURN_CODE(pbmOptAddress) == GP_WB_ENUM_PBM_RETURN_CODE_TX_SUCCESS &&
        GP_WB_READ_PBM_FORMAT_T_GP_ACKED_MODE(pbmOptAddress))
    {
        Int8 rssi = gpHal_CalculateRSSI(GP_WB_READ_PBM_FORMAT_T_ACK_RSSI(pbmOptAddress));
        lqi = gpHal_CalculateLQIfromRSSI(rssi);
    }

    return lqi;
}

UInt8 gpHal_GetTxCCACntr(UInt8 PBMentry)
{
    gpHal_Address_t pbmOptAddress;

    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(PBMentry) && GP_HAL_IS_PBM_ALLOCATED(PBMentry));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);
    return GP_WB_READ_PBM_FORMAT_T_GP_CSMA_CA_ENABLE(pbmOptAddress) ? GP_WB_READ_PBM_FORMAT_T_TX_CCA_CNT(pbmOptAddress) : 0;
}

UInt8 gpHal_GetTxRetryCntr(UInt8 PBMentry)
{
    gpHal_Address_t pbmOptAddress;

    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(PBMentry) && GP_HAL_IS_PBM_ALLOCATED(PBMentry));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);
    return GP_WB_READ_PBM_FORMAT_T_TX_RETRY_EXTENDED(pbmOptAddress);
}

UInt8 gpHal_GetFramePendingFromTxPbm(UInt8 PBMentry)
{
    gpHal_Address_t pbmOptAddress;

    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(PBMentry) && GP_HAL_IS_PBM_ALLOCATED(PBMentry));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);
    return GP_WB_READ_PBM_FORMAT_T_TX_FRM_PENDING(pbmOptAddress);
}

Bool gpHal_GetRxEnhancedAckFromTxPbm(UInt8 PBMentry)
{
    gpHal_Address_t pbmOptAddress;

    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(PBMentry) && GP_HAL_IS_PBM_ALLOCATED(PBMentry));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);
    return ((GP_WB_READ_PBM_FORMAT_T_ACK_SEC(pbmOptAddress)) && (GP_WB_READ_PBM_FORMAT_T_ACK_FRAME_VERS(pbmOptAddress) == 0x02));
}

UInt16 gpHal_GetFrameControlFromTxAckAfterRx(UInt8 PBMentry)
{
    gpHal_Address_t pbmOptAddress;
    UInt16 framecontrol;
    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(PBMentry) && GP_HAL_IS_PBM_ALLOCATED(PBMentry));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);
    framecontrol = GP_WB_READ_PBM_FORMAT_R_ACK_FRAME_CTRL(pbmOptAddress);
    GP_ASSERT_DEV_INT(framecontrol != 0xFC); // 0xFC would indicate that the RT did not have the time yet to write the value.
    if(framecontrol == 0xFC)
    {
        GP_LOG_PRINTF("FP for Tx Ack unknown",0);
        return GPHAL_FRAMEPENDING_UNKNOWN;
    }
    GP_LOG_PRINTF("FC for Tx Ack: 0x%x fp: %d",0, framecontrol, GP_WB_GET_PBM_FORMAT_R_ACK_FP_FROM_ACK_FRAME_CTRL(framecontrol));
    return framecontrol;
}

UInt32 gpHal_GetFrameCounterFromTxAckAfterRx(UInt8 PBMentry)
{
    gpHal_Address_t pbmOptAddress;

    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(PBMentry) && GP_HAL_IS_PBM_ALLOCATED(PBMentry));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);

    GP_LOG_PRINTF("Ack PBM: %d fcount:  0x%08x",0, PBMentry, (UInt16)GP_WB_READ_PBM_FORMAT_R_ACK_FRAMECOUNTER(pbmOptAddress));

    return GP_WB_READ_PBM_FORMAT_R_ACK_FRAMECOUNTER(pbmOptAddress);
}

UInt8 gpHal_GetKeyIdFromTxAckAfterRx(UInt8 PBMentry)
{
    gpHal_Address_t pbmOptAddress;

    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(PBMentry) && GP_HAL_IS_PBM_ALLOCATED(PBMentry));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);

    GP_LOG_PRINTF("Ack PBM: %d keyid:  0x%02x",0, PBMentry, (UInt16)GP_WB_READ_PBM_FORMAT_R_ACK_KEYID(pbmOptAddress));

    return GP_WB_READ_PBM_FORMAT_R_ACK_KEYID(pbmOptAddress);
}

Int8 gpHal_CalculateRSSI(UInt8 protoRSSI)
{
    //RSSI (proto) -> RSSI (dBm)
    Int32 temp;

    //P(dBm) = ((RSSI_ADC_VREF_CAL_VALUE * (CODE + RSSI_OFFSET - 135)) >> 12) - 73
    temp   = protoRSSI ;
    temp  -= 135;
    temp  *= gpHal_GetRssiAdcVrefCalValue();
    temp  += 1<<11; //round instead of floor
    temp >>= 12;
    temp  -= 73;
    if (temp > INT8_MAX)
    {
        temp = INT8_MAX;
    }
    if (temp < INT8_MIN)
    {
        temp = INT8_MIN;
    }


    return (Int8)temp;
}

Int8 gpHal_CalculateIntegratingModeRSSI(UInt8 protoRSSI)
{
    //RSSI (proto) -> RSSI (dBm)
    Int32 temp;

    //P(dBm) = ((CODE_NO_COMP * 3645)/4096) - 144 see AD-3153 option2.
    temp = protoRSSI ;
    temp *= 3645;
    temp >>= 12;
    temp -= 144;

    if (temp > INT8_MAX)
    {
        temp = INT8_MAX;
    }
    if (temp < INT8_MIN)
    {
        temp = INT8_MIN;
    }

    return (Int8)temp;
}

UInt8 gpHal_CalculateLQIfromRSSI(Int8 RSSI)
{
    UInt16 temp;

    // Floor
    if(RSSI < -92) // Minimum dB value for LQI 0x00
    {
        temp = 0;
    }
    // Ceil
    else if(RSSI > -62) // Maximum dB value for LQI 0xFF
    {
        temp = 255;
    }
    else
    {
        // Let result start on zero and multiply with 8.5 to map the 40 dB range to 0 -> 0xFF (255/30)
        temp = ((Int16)RSSI + 92) * 85;
        temp /=10;
    }

    return (UInt8)temp;
}

UInt8 gpHal_CalculateProtoRSSI(Int8 rssi)
{
    //RSSI (dBm) -> RSSI (proto)
    Int32 protoRSSI = rssi;

    protoRSSI  += 73;
    protoRSSI <<= 12;
    protoRSSI  /= gpHal_GetRssiAdcVrefCalValue();
    protoRSSI  += 135;

    if (protoRSSI > UINT8_MAX)
    {
        protoRSSI = UINT8_MAX;
    }

    if (protoRSSI < 0)
    {
        protoRSSI = 0;
    }

    return (UInt8)protoRSSI;
}

void gpHal_WriteDataInPBMCyclic(gpHal_Address_t pbmAddr,UInt8 pbmOffset, UInt8* pData, UInt8 length)
{
    UInt8 startWritePBM = GP_HAL_REMAP_REL_ADDRESS_TO_DATA_SEGMENT((pbmAddr) +(pbmOffset));
    UInt8 splitlength = 0;
    if (length == 0)
    {
        return;
    }
    if((startWritePBM + (length)) > GP_MM_RAM_PBM_ZIGBEE_DATA_SIZE)
    {
        // split in 2
        splitlength = startWritePBM+(length)-GP_MM_RAM_PBM_ZIGBEE_DATA_SIZE;
        // write second part
        //GP_LOG_SYSTEM_PRINTF("WD_: addr:%lx l:%u-%u o:%u stwr:%u",0,pbmAddr, length, splitlength, pbmOffset, startWritePBM);
        GP_HAL_WRITE_DATA_IN_PBM( GP_HAL_CALCULATE_START_PBM_ADDRESS((pbmAddr)) , (UInt8*)((pData)+(length)-splitlength) , splitlength , 0);
    }
    //GP_LOG_SYSTEM_PRINTF("WD: addr:%lx l:%u o:%u stwr:%u pD:%lx",0,pbmAddr, length, pbmOffset, startWritePBM, pData);
    // write first part or full part
    GP_HAL_WRITE_DATA_IN_PBM( GP_HAL_CALCULATE_START_PBM_ADDRESS((pbmAddr)) , (pData) , ((length)-splitlength) , startWritePBM);
}

void gpHal_ReadDataInPBMCyclic(gpHal_Address_t pbmAddr,UInt8 pbmOffset, UInt8* pData, UInt8 length)
{
    UInt8 startWritePBM = GP_HAL_REMAP_REL_ADDRESS_TO_DATA_SEGMENT((pbmAddr) +(pbmOffset));
    UInt8 splitlength = 0;
    if((startWritePBM + (length)) > 128)
    {
        // split in 2
        splitlength = startWritePBM+(length)-128;
        // write second part
        //GP_LOG_SYSTEM_PRINTF("RD_: addr:%lx l:%u-%u o:%u stwr:%u",0,pbmAddr, length, splitlength, pbmOffset, startWritePBM);
        GP_HAL_READ_DATA_IN_PBM( GP_HAL_CALCULATE_START_PBM_ADDRESS((pbmAddr)) , (UInt8*)((pData)+(length)-splitlength) , splitlength , 0);
    }
    // write first part or full part
    GP_HAL_READ_DATA_IN_PBM( GP_HAL_CALCULATE_START_PBM_ADDRESS((pbmAddr)) , (pData) , ((length)-splitlength) , startWritePBM);
    //GP_LOG_SYSTEM_PRINTF("RD: addr:%lx l:%u o:%u stwr:%u  %x %x %x %x",0,pbmAddr, length, pbmOffset, startWritePBM,
                                                                         //pData[0], pData[1], pData[2], pData[3]);
}

void gpHal_WriteByteInPBMCyclic(gpHal_Address_t pbmAddr,UInt8 pbmOffset, UInt8 byte)
{
    GP_HAL_WRITE_BYTE_IN_PBM(GP_HAL_CALCULATE_START_PBM_ADDRESS((pbmAddr)),
                             (byte),
                             GP_HAL_REMAP_REL_ADDRESS_TO_DATA_SEGMENT(pbmAddr + pbmOffset));
}

UInt8 gpHal_ReadByteInPBMCyclic(gpHal_Address_t pbmAddr,UInt8 pbmOffset )
{
    return GP_HAL_READ_BYTE_IN_PBM(GP_HAL_CALCULATE_START_PBM_ADDRESS(pbmAddr),
                                   GP_HAL_REMAP_REL_ADDRESS_TO_DATA_SEGMENT(pbmAddr + pbmOffset));
}

void gpHal_WriteDataInPBM(gpHal_Address_t pbmAddr,UInt16 pbmOffset, UInt8* pData, UInt8 length)
{
    //GP_LOG_SYSTEM_PRINTF("wr: addr: %lx, offset: %x len: %x",0,pbmAddr,pbmOffset, length);
    //gpLog_PrintBuffer(length,pData);
    GP_HAL_WRITE_DATA_IN_PBM(pbmAddr, pData, length, pbmOffset);
}

void gpHal_ReadDataInPBM(gpHal_Address_t pbmAddr,UInt16 pbmOffset, UInt8* pData, UInt8 length)
{
    GP_HAL_READ_DATA_IN_PBM(pbmAddr, pData, length, pbmOffset);
}

void gpHal_WriteByteInPBM(gpHal_Address_t pbmAddr,UInt16 pbmOffset, UInt8 byte)
{
    //GP_LOG_SYSTEM_PRINTF("wr: addr: %lx, offset: %x len: %x en byte: %x",0,pbmAddr,pbmOffset, 1, byte);
    GP_HAL_WRITE_BYTE_IN_PBM(pbmAddr, byte, pbmOffset);
}

UInt8 gpHal_ReadByteInPBM(gpHal_Address_t pbmAddr,UInt16 pbmOffset)
{
    return GP_HAL_READ_BYTE_IN_PBM(pbmAddr, pbmOffset);
}

void gpHal_MakeBareCopyPBM( UInt8 PBMentryOrig , UInt8 PBMentryDst )
{
    gpHal_Address_t tempCopyAddress;
    UInt8 tempBuffer[32];

    //copy PBM options - GP_MM_RAM_PBM_OPTS_SIZE
    COMPILE_TIME_ASSERT(32 == GP_MM_RAM_PBM_OPTS_SIZE); //Assumption here
    GP_HAL_READ_BYTE_STREAM(GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentryOrig), tempBuffer, GP_MM_RAM_PBM_OPTS_SIZE);
/*
    GP_LOG_SYSTEM_PRINTF("CP_: addr:%lx -> %lx : %x %x %x %x",0,GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentryOrig),
                                                               GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentryDst),
                                                               tempBuffer[0], tempBuffer[1],
                                                               tempBuffer[2], tempBuffer[3]);
*/
    GP_HAL_WRITE_BYTE_STREAM(GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentryDst), tempBuffer, GP_MM_RAM_PBM_OPTS_SIZE);

    //copy PBM data - GP_MM_RAM_PBM_DATA_SIZE
    COMPILE_TIME_ASSERT((GP_MM_RAM_PBM_ZIGBEE_DATA_SIZE % 32) == 0); //Assumption here
    for(tempCopyAddress=0; tempCopyAddress < GP_MM_RAM_PBM_ZIGBEE_DATA_SIZE; tempCopyAddress+=32)
    {
        GP_HAL_READ_BYTE_STREAM( GP_HAL_PBM_ENTRY2ADDR_DATA_BASE(PBMentryOrig) + tempCopyAddress, tempBuffer, 32);
/*
        GP_LOG_SYSTEM_PRINTF("CP: addr:%lx -> %lx : %x %x %x %x",0, GP_HAL_PBM_ENTRY2ADDR_DATA_BASE(PBMentryOrig) + tempCopyAddress,
                                                                    GP_HAL_PBM_ENTRY2ADDR_DATA_BASE(PBMentryDst)  + tempCopyAddress,
                                                                    tempBuffer[0], tempBuffer[1],
                                                                    tempBuffer[2], tempBuffer[3]);
*/
        GP_HAL_WRITE_BYTE_STREAM(GP_HAL_PBM_ENTRY2ADDR_DATA_BASE(PBMentryDst) + tempCopyAddress, tempBuffer, 32);
    }
}

#ifdef GP_COMP_GPHAL_BLE
gpHal_Result_t gpHal_PbmGetEventCounter(UInt8 pbmHandle, UInt16* pEventCount)
{
    gpHal_Address_t pbmOptAddress;

    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(pbmHandle) && GP_HAL_IS_PBM_ALLOCATED(pbmHandle));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(pbmHandle);

    *pEventCount = GP_WB_READ_PBM_BLE_FORMAT_R_EVENT_COUNTER(pbmOptAddress);

    return gpHal_ResultSuccess;
}

void gpHal_PbmSetCteLengthUs(UInt8 pbmHandle, UInt8 cteLengthUs)
{
    gpHal_Address_t pbmOptAddress;

    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(pbmHandle) && GP_HAL_IS_PBM_ALLOCATED(pbmHandle));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(pbmHandle);

    GP_WB_WRITE_PBM_BLE_FORMAT_T_SUPPLEMENTAL_LEN(pbmOptAddress, cteLengthUs);
    GP_WB_WRITE_PBM_BLE_FORMAT_T_SUPPLEMENTAL_INVERT(pbmOptAddress, 0x01);
}

UInt8 gpHal_PbmGetBleRxPhy(UInt8 pbmHandle)
{
    gpHal_Address_t pbmOptAddress;

    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(pbmHandle) && GP_HAL_IS_PBM_ALLOCATED(pbmHandle));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(pbmHandle);

    return GP_WB_READ_PBM_BLE_FORMAT_R_PHY_MODE(pbmOptAddress);
}
#endif //GP_COMP_GPHAL_BLE
