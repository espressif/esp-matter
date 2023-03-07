/*
 * Copyright (c) 2009-2013, 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017-2021, Qorvo Inc
 *
 *   Non Volatilie Memory component
 *   Declarations of the public functions and enumerations of gpNvm.
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


#ifndef _GPNVM_H_
#define _GPNVM_H_


#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpNvm_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

/// @file "gpNvm.h"
/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
/* <CodeGenerator Placeholder> AdditionalIncludes */
//---------------------
//General NVM functions
//---------------------

/**
 * @file gpNvm.h
 *
 *
 * @defgroup GEN_NVM General NVM functions
 * @brief The general NVM functionality is implemented in these functions
 *
 * Checking and clearing the NVM is made possible by these functions.
 * The implementation of these functions can be unique for each type of NVM used.
 */

/* </CodeGenerator Placeholder> AdditionalIncludes */

/*****************************************************************************
 *                    Enum Definitions
 *****************************************************************************/

/** @enum gpNvm_UpdateFrequency_t */
//@{
/** @brief Updated more often, typically used for attributes that can get updated during normal user usage */
#define gpNvm_UpdateFrequencyHigh                              0
/** @brief Updated in rare condition */
#define gpNvm_UpdateFrequencyLow                               1
/** @brief Updated only during initialisation phase, typically used for attributes which are only set once */
#define gpNvm_UpdateFrequencyInitOnly                          2
/** @brief Set during configuration, read-only information */
#define gpNvm_UpdateFrequencyReadOnly                          3
/** @brief Wildcard value used with read queries */
#define gpNvm_UpdateFrequencyIgnore                            4
#define gpNvm_NrOfUpdateFrequencyTypes                         5
/** @typedef gpNvm_UpdateFrequency_t
    @brief The gpNvm_UpdateFrequency_t type defines indicating the frequency a tag will require a new backup into NVM.
*/
typedef UInt8                             gpNvm_UpdateFrequency_t;
//@}

/** @enum gpNvm_Result_t */
//@{
#define gpNvm_Result_DataAvailable                             0
#define gpNvm_Result_NoDataAvailable                           1
#define gpNvm_Result_NoLookUpTable                             2
#define gpNvm_Result_NoUniqueMaskMatch                         4
#define gpNvm_Result_Truncated                                 5
#define gpNvm_Result_Error                                     0xFF
typedef UInt8                             gpNvm_Result_t;
//@}

/** @enum gpNvm_PoolId_t */
//@{
/** @brief The pool used by the tag API */
#define gpNvm_PoolId_Tag                                       0
/** @brief Application specified pool */
#define gpNvm_PoolId_Application1                              1
/** @brief Application specified pool */
#define gpNvm_PoolId_Application2                              2
/** @brief Application specified pool */
#define gpNvm_PoolId_Application3                              3
/** @brief Application specified pool */
#define gpNvm_PoolId_Application4                              4
/** @brief Used to iterate all pools */
#define gpNvm_PoolId_AllPoolIds                                0xFE
/** @typedef gpNvm_PoolId_t
    @brief TBD:Namespace prefix or physical pool id
*/
typedef UInt8                             gpNvm_PoolId_t;
//@}

#define gpNvm_LookupTable_Handle_Invalid                       (0xff)
typedef UInt8 gpNvm_LookupTable_Handle_t;
/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/** @macro gpNvm_AllComponents */
/** @brief Wildcard used to select all nvm-sections in a backup/restore/clear call. */
#define gpNvm_AllComponents                          0xEE
/** @macro gpNvm_AllTags */
/** @brief Wildcard used to select all tags of the specified nvm-sections in a backup/restore/clear call. */
#define gpNvm_AllTags                                0xEE
/** @macro GP_NVM_SECTIONID_TAG */
#define GP_NVM_SECTIONID_TAG                         16
/* <CodeGenerator Placeholder> AdditionalMacroDefinitions */
#define GP_NVM_CONST       const

/** @macro GP_NVM_MAX_TOKENLENGTH */
#if !defined(GP_NVM_MAX_TOKENLENGTH)
#if defined(GP_NVM_DIVERSITY_ELEMIF_KEYMAP)
#define GP_NVM_MAX_TOKENLENGTH                       2
#else /* ELEMIF_ARRAYMAP */
#define GP_NVM_MAX_TOKENLENGTH                       1
#endif /* defined(GP_NVM_DIVERSITY_ELEMIF_KEYMAP) */
#endif // !defined(GP_NVM_MAX_TOKENLENGTH)

/** @macro GP_NVM_MAX_PAYLOADLENGTH */
#if !defined(GP_NVM_MAX_PAYLOADLENGTH)
#if defined(GP_DIVERSITY_GPHAL_K8E)
#define GP_NVM_MAX_PAYLOADLENGTH                     255
#else
#if defined(GP_NVM_DIVERSITY_ELEMIF_KEYMAP)
#define GP_NVM_MAX_PAYLOADLENGTH                     239
#else //GP_NVM_DIVERSITY_SIMPLE_IF
#define GP_NVM_MAX_PAYLOADLENGTH                     241
#endif /* defined(GP_NVM_DIVESITY_ELEMIF_KEYMAP) */
#endif // GPHAL_K8C
#else
// Check that the max mayloadlength does not exceed 255, since payloadlength is stored in the header in a single byte!
GP_COMPILE_TIME_VERIFY(GP_NVM_MAX_PAYLOADLENGTH < 256);
#endif // !defined(GP_NVM_MAX_PAYLOADLENGTH)

#if !defined(GP_NVM_NBR_OF_POOLS)
#define GP_NVM_NBR_OF_POOLS 1
#endif //!defined(GP_NVM_NBR_OF_POOLS)

/* </CodeGenerator Placeholder> AdditionalMacroDefinitions */

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef UInt16 gpNvm_Type_t;

#ifndef GP_NVM_NBR_OF_UNIQUE_TAGS
#define GP_NVM_NBR_OF_UNIQUE_TAGS 255
#endif
#if GP_NVM_NBR_OF_UNIQUE_TAGS > 255
typedef UInt32 gpNvm_KeyIndex_t;
#else
typedef UInt8 gpNvm_KeyIndex_t;
#endif
typedef UInt16 gpNvm_VersionCrc_t;

/** @pointer to function gpNvm_cbCheckConsistency_t
 *  @brief The gpNvm_cbCheckConsistency_t defines the callback type for an additional section specific check-consistency routine (see gpNvm_RegisterSection())
*   @return status
*/
typedef Bool (*gpNvm_cbCheckConsistency_t) (void);

/** @pointer to function gpNvm_cbUpdate_t
 *  @brief The gpNvm_cbUpdate_t defines the callback type for the up or downgrade routine between NVM versions
*   @return status
*/
typedef Bool (*gpNvm_cbUpdate_t) (void);


/* <CodeGenerator Placeholder> AdditionalTypeDefinitions */
/** @struct gpNvm_Tag_t
 *  @brief The gpNvm_Tag structure specifies the fields that describe an nvm-tag
*/
typedef struct {
/** @brief A pointer to the corresponding RAM memory of the attributed to be backuped in nvm */
    UInt8*                      pRamLocation;
/** @brief The number of bytes of the nvm-tag */
    UInt16                      size;
/** @brief An indication of the update frequency */
    gpNvm_UpdateFrequency_t     updateFrequency;
/** @brief A pointer to flash memory containing the default values for the tag, 0xFF will be used as default values if pointer is specified as NULL */
    GP_NVM_CONST ROM UInt8*                pDefaultValues;
} gpNvm_Tag_t;

/** @struct gpNvm_Section_t */
typedef struct {
    UInt8                                   componentId;
    const ROM gpNvm_Tag_t* FLASH_PROGMEM    tags;
    UInt8                                   nbrOfTags;
#ifndef GP_NVM_DIVERSITY_TAG_IF
    UInt8*                                  baseAddr;
#else
    UInt8                                   baseTag;
#endif
    gpNvm_cbCheckConsistency_t              cbCheckConsistency;
} gpNvm_Section_t;


/** @struct gpNvm_CompatibilityEntry
 *  @brief The gpNvm_CompatibilityEntry structure specifies the information for compatible NVM versions */
/** @typedef gpNvm_CompatibilityEntry_t
 *  @brief The gpNvm_CompatibilityEntry_t type describes a compatibility entry */
typedef struct gpNvm_CompatibilityEntry {
    UInt16  crc;
    gpNvm_cbUpdate_t upgradeCb;
    gpNvm_cbUpdate_t downgradeCb;
} gpNvm_CompatibilityEntry_t;

/* </CodeGenerator Placeholder> AdditionalTypeDefinitions */
/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpNvm_CodeJumpTableFlash_Defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */

//Requests
/** @brief Initializes the NVM software block.  After execution of this function data can be read or written to NVM.
*/
void gpNvm_Init(void);
void gpNvm_DeInit(void);
/** @brief Reserves part of space on local NVM for remote NVM usage. This function should be called at most once. If not called, no space is reserved for remote NVM.
*
*   @param requiredSize              Size of the NVM to be used remotely.
*   @return offset                   Offset within local NVM where remote NVM may store sections
*/

UInt16 gpNvm_ReserveRemoteNvmSpace(UInt16 requiredSize);
/** @brief Clears the complete NVM. This function clears the complete NVM. This includes all tags of all registered nvm-sections and all legacy implementations of the component specific interfaces described in the other sections.  Readonly tags will not be erased unless the NVM was unlocked using gpNvm_SetLock().
*/
void gpNvm_ClearNvm(void);
/** @brief Requests a forced flush of the write buffers to NVM. This function request the nvm component to flush all pending write data to NVM. If the NVM implementation doesn't use buffers, the function should be stubbed.
*/
void gpNvm_Flush(void);
/** @brief Dumps the NVM content. This function dumps the content of the specified tag of the specified component. It will also indicate with a '*' that this content is not in sync with the current values of the corresponding ram variables
*
*   @param componentId               The unique idenditfier nvm-section of the section to be restored, gpNvm_AllComponents can be used as wildcard.
*   @param tagId                     The sequence number of the tag to be restored, gpNvm_AllTags can be used as wildcard.
*/
void gpNvm_Dump(UInt8 componentId, UInt8 tagId);
/** @brief This function dumps the structure of the NVM tags and NVM CRC
*/
void gpNvm_DumpStructure(void);
/** @brief Backup the content of the specified NVM-tag.  The content of the RAM memory (pRamLocation parameter) specified by the combination of the component and tag will be written to the non-volitale memory.
*
*   @param componentId               The unique idenditfier nvm-section of the section to be backuped, gpNvm_AllComponents can be used as wildcard.
*   @param tagId                     The sequence number of the tag to be backuped
*   @param pRamLocation              Pointer to the data that should be backuped.  When this pointer is NULL, the internal stored pointer is used.
*/
void gpNvm_Backup(UInt8 componentId, UInt8 tagId, UInt8* pRamLocation);
/** @brief Restore the content of the specified NVM-tag using the provided RAM location. The content of the RAM memory (pRamLocation parameter) specified by the combination of the component and tag will be updated with the corresponding copy in the the non-volitale memory.
*
*   @param componentId               The unique idenditfier nvm-section of the section to be restored, gpNvm_AllComponents can be used as wildcard.
*   @param tagId                     The sequence number of the tag to be restored.
*   @param pRamLocation              Pointer to the data that should be restored.  When this pointer is NULL, the internal stored pointer is used.
*/
void gpNvm_Restore(UInt8 componentId, UInt8 tagId, UInt8* pRamLocation);
/** @brief Clears the content of the specified NVM-tag using the provided RAM location.  The non-volatile memory specified by the combination of the component and tag will be updated to the default values (pDefaultValues). If there are no default values specified the content of the non-volatile memory will be set to 0xFF.
*
*   @param componentId               The unique identifier nvm-section of the section to be cleared, gpNvm_AllComponents can be used as wildcard.
*   @param tagId                     The sequence number of the tag to be cleard, gpNvm_AllTags can be used as wildcard.
*/
void gpNvm_Clear(UInt8 componentId, UInt8 tagId);
#if !(defined(GP_NVM_DIVERSITY_TAG_IF))
void Nvm_WriteByte(UIntPtr address, UInt8 value, gpNvm_UpdateFrequency_t updateFrequency);
#endif //!(defined(GP_NVM_DIVERSITY_TAG_IF))
#if !(defined(GP_NVM_DIVERSITY_TAG_IF))
UInt8 Nvm_ReadByte(UIntPtr address, gpNvm_UpdateFrequency_t updateFrequency);
#endif //!(defined(GP_NVM_DIVERSITY_TAG_IF))
/** @brief  Locks or unlocks the possibility to write or clear read-only elements.  Default the NVM area will be unable to write or clear read-only elements (tags with update frequency gpNvm_UpdateFrequencyReadOnly).  One can 'unlock' the area by calling this function with false.  Re-lock the area after the needed manipulations by calling this function with true.
*
*   @param lock                      Lock (true) or Unlock (false) the NVM area.
*/
void gpNvm_SetLock(Bool lock);
/** @brief Checks if NVM is consistent.  The function checks if the NVM is consistent. If not, NVM is cleared.
*/
void gpNvm_CheckConsistency(void);
/** @brief Checks if NVM is accessible.  The function checks if the NVM is accessible
*   @return result                   Boolean that retunrns the result of the check.
*/
Bool gpNvm_CheckAccessible(void);
#if !(defined(GP_NVM_DIVERSITY_TAG_IF))
void Nvm_ReadBlock(UIntPtr address, UInt16 lengthBlock, UInt8* data, gpNvm_UpdateFrequency_t updateFrequency);
#endif //!(defined(GP_NVM_DIVERSITY_TAG_IF))
#if defined(GP_NVM_DIVERSITY_TAG_IF)
Bool Nvm_WriteTag(UInt8 tagId, UInt16 length, UInt8* tagData, gpNvm_UpdateFrequency_t updateFrequency);
#endif //defined(GP_NVM_DIVERSITY_TAG_IF)
#if defined(GP_NVM_DIVERSITY_TAG_IF)
Bool Nvm_ReadTag(UInt8 tagId, UInt16 length, UInt8* tagData, gpNvm_UpdateFrequency_t updateFrequency);
#endif //defined(GP_NVM_DIVERSITY_TAG_IF)
void gpNvm_Refresh(void);
#if defined(GP_NVM_DIVERSITY_TAG_IF)
/** @brief Write data to the NVM added with meta data on updateFrequency and token references. Note: no partial writes supported
*
*   @param poolId                    The pool ID to be used
*   @param updateFrequency           reference to group of similar behaving attributes
*   @param tokenLength               length in bytes of pToken
*   @param pToken                    array forming unique reference to the attribute within the updateFrequency scope
*   @param dataLength                length in bytes of pData
*   @param pData                     pointer to data to write
*   @return result
*/
gpNvm_Result_t gpNvm_Write(gpNvm_PoolId_t poolId, gpNvm_UpdateFrequency_t updateFrequency, UInt8 tokenLength, UInt8* pToken, UInt8 dataLength, UInt8* pData);
#endif //defined(GP_NVM_DIVERSITY_TAG_IF)
#if defined(GP_NVM_DIVERSITY_TAG_IF)
/** @brief Create a temporary lookup table containing references to attributes passing the filter criteria for a specific update frequency
* WARNING: be carefull when specifying maxNrMatches: account for previously (previously instanciated) deleted tokens.
*   You should always call gpNvm_FreeLookup when gpNvm_Result_DataAvailable or gpNvm_Result_Truncated is returned, and you are done
*   working with the lookup table.
*
*   @param handle                    lookuptable handle
*   @param poolId                    The pool ID to be used
*   @param updateFrequency           reference to group of similar behaving attributes
*   @param tokenMaskLength           length of pTokenMask
*   @param pTokenMask                array defining the first tokenMaskLength tokens that need to match
*   @param maxNrMatches               maximum of expected data entry matches - when this maximum is reached, an error is returned
*   @param pNrOfMatches              number of matches in the created lookup table (gpNvm_BuildLookup_Error is error)
*   @return result                   gpNvm_Result_Truncated in case there were more entries than requested
*/
gpNvm_Result_t gpNvm_BuildLookup(gpNvm_LookupTable_Handle_t* handle, gpNvm_PoolId_t poolId, gpNvm_UpdateFrequency_t updateFrequency, UInt8 tokenMaskLength, UInt8* pTokenMask, gpNvm_KeyIndex_t maxNrMatches, gpNvm_KeyIndex_t* pNrOfMatches);
#endif //defined(GP_NVM_DIVERSITY_TAG_IF)

gpNvm_Result_t gpNvm_AcquireLutHandle(
    gpNvm_LookupTable_Handle_t* pHandle,
    gpNvm_PoolId_t poolId,
    gpNvm_UpdateFrequency_t updateFrequencySpec,
    UInt8 tokenLength,
    UInt8* pToken,
    Bool* freeAfterUse,
    gpNvm_KeyIndex_t maxNbrOfMatches
);

#if defined(GP_NVM_DIVERSITY_TAG_IF)
/** @brief Create a temporary lookup table containing references to attributes passing the filter criteria for all update frequencies
*
*   @param handle                    lookuptable handle
*   @param poolId                    The pool ID to be used
*   @param tokenMaskLength           length of pTokenMask
*   @param pTokenMask                array defining the first tokenMaskLength tokens that need to match
*   @param maxNrMatches               maximum of expected data entry matches - when this maximum is reached, an error is returned
*   @param pNrOfMatches              number of matches in the created lookup table (gpNvm_BuildLookup_Error is error)
*   @return result                   gpNvm_Result_Truncated in case there were more entries than requested
*/
gpNvm_Result_t gpNvm_BuildLookupOverAllUpdateFrequencies(gpNvm_LookupTable_Handle_t* handle, gpNvm_PoolId_t poolId, UInt8 tokenMaskLength, UInt8* pTokenMask, gpNvm_KeyIndex_t maxNrMatches, gpNvm_KeyIndex_t* pNrOfMatches);
#endif //defined(GP_NVM_DIVERSITY_TAG_IF)
/** @brief Retrieve the next data that matches the filter criteria from the NVM, this method requires the availability of a temporary lookup table
*   You should always call gpNvm_FreeLookup when gpNvm_Result_DataAvailable or gpNvm_Result_Truncated is returned, and you are done
*   working with the lookup table.
*
*   @param handle                    lookuptable handle
*   @param poolId                    The pool ID to be used
*   @param pUpdateFrequency          used to return the updateFrequency of the read data typically used by caller when lookup table is created with gpNvm_BuildLookupOverAllUpdateFrequencies; will be ignored when specifying NULL
*   @param maxTokenLength            maximum length in bytes of pToken
*   @param pTokenLength              filled in number of bytes in pToken typically used by caller when lookup table is created with tokenlength 0; will be ignored when specifying 0
*   @param pToken                    pointer to buffer to return token information typically used by caller when lookup table is created with gpNvm_BuildLookupOverAllUpdateFrequencies; will be ignored when specifying NULL
*   @param maxDataLength             maximum length in bytes of pData
*   @param pDataLength               number of bytes in pData
*   @param pData                     pointer to buffer to return data
*   @return result                   gpNvm_Result_Truncated in case there were more entries than requested
*/
gpNvm_Result_t gpNvm_ReadNext(gpNvm_LookupTable_Handle_t handle, gpNvm_PoolId_t poolId, gpNvm_UpdateFrequency_t* pUpdateFrequency, UInt8 maxTokenLength, UInt8* pTokenLength, UInt8* pToken, UInt8 maxDataLength, UInt8* pDataLength, UInt8* pData);
/** @brief method to retrieve data of an attribute which has a unique match of the specified filters in the temporary lookup table
*
*   @param handle                    lookuptable handle
*   @param poolId                    The pool ID to be used
*   @param updateFrequencySpec       specify the updateFrequency of the read data, gpNvm_UpdateFrequencyIgnore can be used as wildcard
*   @param pUpdateFrequency          returns the update frequency of the attribute
*   @param tokenLength               length in bytes of pToken
*   @param pToken                    array forming the unique reference tot the attribute within the updateFrequency scope - will result in unique match
*   @param maxDataLength             maximum length in bytes of pData
*   @param pDataLength               number of bytes in pData
*   @param pData                     pointer to buffer to return data
*   @return result                   gpNvm_Result_DataAvailable gpNvm_Result_NoDataAvailable gpNvm_Result_NoLookUpTable gpNvm_Result_NoUniqueMaskMatch
*/
gpNvm_Result_t gpNvm_ReadUnique(gpNvm_LookupTable_Handle_t handle, gpNvm_PoolId_t poolId, gpNvm_UpdateFrequency_t updateFrequencySpec, gpNvm_UpdateFrequency_t* pUpdateFrequency, UInt8 tokenLength, UInt8* pToken, UInt8 maxDataLength, UInt8* pDataLength, UInt8* pData);
/** @brief Method to free a lookuptable after use
*
*   @param handle                    lookuptable handle
*/
void gpNvm_FreeLookup(gpNvm_LookupTable_Handle_t handle);
/** @brief Remove an nvm entry
*
*   @param poolId                    The pool ID to be used
*   @param updateFrequencySpec       specify the updateFrequency of the read data, gpNvm_UpdateFrequencyIgnore can be used as wildcard
*   @param tokenLength               length in bytes of pToken
*   @param pToken                    array forming the unique reference tot the attribute within the updateFrequency scope - will result in unique match
*   @return result                   gpNvm_Result_DataAvailable gpNvm_Result_Error
*/
gpNvm_Result_t gpNvm_Remove(gpNvm_PoolId_t poolId, gpNvm_UpdateFrequency_t updateFrequencySpec, UInt8 tokenLength, UInt8* pToken);
/** @brief Erase the content of a specific pool.
*
*   @param poolId                    The pool ID to be erased. Use gpNvm_PoolId_AllPoolIds to erase all pools.
*   @return result                   gpNvm_Result_DataAvailable gpNvm_Result_Error
*/
gpNvm_Result_t gpNvm_ErasePool(gpNvm_PoolId_t poolId);

gpNvm_Result_t gpNvm_GetNextTokenKey(gpNvm_LookupTable_Handle_t handle, gpNvm_PoolId_t poolId, gpNvm_UpdateFrequency_t updateFrequencySpec, UInt8 tokenLength, UInt8* pToken, UInt8* pTokenKey);
/** @brief Reset the iteration sequence of ReadNext to the first item.
*
*   @param handle                    lookuptable handle
*   @return result                   gpNvm_Result_DataAvailable gpNvm_Result_Error
*/
gpNvm_Result_t gpNvm_ResetIterator(gpNvm_LookupTable_Handle_t handle);

/** @brief Physical erase the NVM pool.
 *  Warning : This function should be used only inside Nvm_SafetyNetHandler
 *
 *  @param poolId    Pool Id to be erased
 */
gpNvm_Result_t gpNvm_PhysicalErasePool(gpNvm_PoolId_t poolId);

//Indications
/** @brief Callback triggered if consistency check failed.
*/
void gpNvm_cbFailedCheckConsistency(void);


/** @brief Controls the automatic background defragmentation. To allow multiple usage a counter mechanism in implemented. The auto defragmentation is enabled by default.
*
*   @param enable                    false = disable defragmentation (counter is incremented), true = enable defragmentation (counter is decremented). The number of calls with true should match the number with calls with false to enable the background defragmentation.
*/
void gpNvm_SetBackgroundDefragmentationMode(Bool enable);

/* <CodeGenerator Placeholder> AdditionalPublicFunctionDefinitions */
/**@ingroup GEN_NVM
 * @brief Registers a component to the NVM software block
 *
 *  Registers a component to the NVM software block. This will define a NVM section. The function should be called from the _init
 *  function of the component.
 *
 *  @param componentId          An unique idenditfier of the registered section, which will be used further on to refer to this section. The GP_COMPONENT_ID should be used for this.
 *  @param gpNvm_Tag_t          A pointer to a gpNvmTag_t array allocated in flash contining the information of all nvm-tags of the registered section.
 *  @param nbrOfTags            The number of tags in the gpNvm_Tag_t array tags.
 *  @param cbCheckConsistency   A function pointer to a callback which will be called by the nvm-component if it's performing a consistency check. This allows specific checks on the
 *                              content of the section which are only known by the component. If specified as NULL, this additional check will be ommitted.
 */
void gpNvm_RegisterSection(UInt8 componentId, const ROM gpNvm_Tag_t *tags, UInt8 nbrOfTags, gpNvm_cbCheckConsistency_t cbCheckConsistency);



#ifdef GP_NVM_DIVERSITY_ELEMENT_IF
#include "gpNvm_ElemIf.h"
#endif /* GP_NVM_DIVERSITY_ELEMENT_IF */


/* </CodeGenerator Placeholder> AdditionalPublicFunctionDefinitions */

#ifdef GP_NVM_DIVERSITY_VARIABLE_SIZE
/** @brief Override NVM size and pool size parameters.
 *  This function must be called before gpNvm_Init().
 */
void gpNvm_SetVariableSize(UInt16 nrOfSectors, UInt8 nrOfPools, const UInt8 *sectorsPerPool);

/** @brief Retrieve NVM size and pool size parameters.
 *
 *  This function can be used to pre-check the boundaries set during compilation,
 *  before attempting to alter the configuration
 */
void gpNvm_GetVariableSize(UInt16* nrOfSectors, UInt8* nrOfPools, UInt8 *sectorsPerPool);
#endif //GP_NVM_DIVERSITY_VARIABLE_SIZE

#ifdef GP_NVM_DIVERSITY_VARIABLE_SETTINGS
/** @brief Set variable information: token sizes, start of NVM area.
 *
 *  Values will initialize with the compile-time defaults
 *  This function must be called before gpNvm_Init().
 */
void gpNvm_SetVariableSettings(UIntPtr pNvmStart, gpNvm_KeyIndex_t numberOfUniqueTokens);

/** @brief Retrieve current settings used by NVM component.
 *
 *  This function can be used to pre-check the boundaries set during compilation,
 *  before attempting to alter the configuration
*/
void gpNvm_GetVariableSettings(UIntPtr* pNvmStart, gpNvm_KeyIndex_t* numberOfUniqueTokens, UInt8* maxTokenLength);
#endif //GP_NVM_DIVERSITY_VARIABLE_SETTINGS

gpNvm_Result_t gpNvm_GetNvmType(UInt8* pNvmType);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif //_GPNVM_H_

