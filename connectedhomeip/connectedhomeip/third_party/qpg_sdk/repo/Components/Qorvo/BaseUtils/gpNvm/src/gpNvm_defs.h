/*
 * Copyright (c) 2011-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * This file contains the internal defines, typedefs,... of the NVM component
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

#ifndef _GPNVM_DEFS_H_
#define _GPNVM_DEFS_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpNvm.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

extern const UIntPtr gpNvm_NvmBaseAddr;

/*****************************************************************************
 *                    Public Function Declarations
 *****************************************************************************/

void Nvm_Init(void);
void Nvm_DeInit(void);
UInt16 Nvm_GetMaxSize(void);
void Nvm_Flush(void);
void Nvm_DumpInfo(void);
/*
* Address based interface
*/
void Nvm_WriteByte(UIntPtr address, UInt8 value, gpNvm_UpdateFrequency_t updateFrequency);
UInt8 Nvm_ReadByte(UIntPtr address, gpNvm_UpdateFrequency_t updateFrequency);
Bool Nvm_WriteBlock(UIntPtr address, UInt16 length, UInt8* txBuffer, gpNvm_UpdateFrequency_t updateFrequency);
void Nvm_ReadBlock(UIntPtr address, UInt16 length, UInt8* rxBuffer, gpNvm_UpdateFrequency_t updateFrequency);
void Nvm_Erase(void);
Bool Nvm_CheckAccessible(void);


void Nvm_CompressDataByFrequency(gpNvm_UpdateFrequency_t freqToSearch);

/*
* Tag interface
*/
Bool Nvm_WriteTag(UInt8 tagId, UInt16 length, UInt8* tagData, gpNvm_UpdateFrequency_t updateFrequency);
Bool Nvm_ReadTag(UInt8 tagId, UInt16 length, UInt8* tagData, gpNvm_UpdateFrequency_t updateFrequency);
UInt8 Nvm_GetSectionBaseTag(UInt8 nbrOfTags);
void Nvm_TagIf_Erase(void);

/* Only supported by gpNvm_RW_Kx_SubpagedFlash_v1 and gpNvm_RW_Kx_SubpagedFlash_v2 */
void Nvm_FreeTagCache(void);
void Nvm_BuildTagCache(void);
void Nvm_Pool_PhysicalErase(gpNvm_PoolId_t poolId);

#ifdef GP_NVM_DIVERSITY_ELEMENT_IF


#define NVM_LUT_UPDATE_FREQUENCY            gpNvm_UpdateFrequencyLow
#define NVM_CRC_UPDATE_FREQUENCY            gpNvm_UpdateFrequencyInitOnly
#define NVM_TYPE_UPDATE_FREQUENCY           gpNvm_UpdateFrequencyInitOnly
#define NVM_SIZEOFEL_UPDATE_FREQUENCY       gpNvm_UpdateFrequencyLow


/**@brief Nvm_cbTagIdx2Addr calculates the address of the element corresponding to the given tag index.
 *
 *  The calculated address is based at the beginning of the NVM storage (i.e. address 0 corresponds to the lowest addressed byte of the NVM).
 *  If the address is not known, the last known address is calculated. Index of the calculated element is returned.
 *  @param tagIndex             index of the requested element in the NVM storage.
 *  @param pAddr                reference to the location where address is to be written.
 *  @return                     index of the element whose address was calculated. Note: it can be lower than the requested index.
 */
Bool Nvm_cbTagIdx2Addr(UInt8 tagIndex, UIntPtr * pAddr, UInt8* pFoundIndex);

/**@brief Nvm_CreateTag erases required number of bytes starting from the beginning of the specified element
 *
 *  The specified location is overwritten with value that is determined internaly in the platform driver and no assumption may be made about it.
 *  If the specified location doesn't exist (element with that index is not yet created in the NVM) then new element may be created provided that
 *  element with index smaller by 1 already exists. Size of newly created element will be set to size requested for erasure.
 *  @param index    Index of the element to be erased
 *  @param size     Number of consecutive bytes to be erased at the beginning of the NVM element.
 */
Bool Nvm_CreateTag(UInt8 index, UInt16 size, gpNvm_UpdateFrequency_t updateFrequency);

/**@brief Nvm_IsTagCreated checks if the specified tag exists and if it can accomodate specified number of bytes
 *
 * @param index     Index of the element being checked for existance
 * @param size      Minimal required size of the element
 */
Bool Nvm_IsTagCreated(UInt8 index, UInt16 size);



#endif /* GP_NVM_DIVERSITY_ELEMENT_IF */


#endif //_GPNVM_DEFS_H_
