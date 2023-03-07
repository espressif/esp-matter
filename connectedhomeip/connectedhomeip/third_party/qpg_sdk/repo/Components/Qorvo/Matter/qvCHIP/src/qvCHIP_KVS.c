/*
 * Copyright (c) 2021-2022, Qorvo Inc
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

/** @file "qvCHIP_KVS.c"
 *
 *  CHIP KVS functionality
 *
 *  Implementation of qvCHIP KVS
 *
 *  The implementation will use 2 type of 'elements' stored in NVM.
 *  - The key coming from the KVS API will be stored as a key element.
 *  - The actual payload, possibly split up in chunks with max NVM size, are separate elements.
 *    The list of payload indices is added to the key element as a reference.
 *
 *  The token mask of the NVM is used in such a way to avoid use of many different keys inside the NVM.
 *  All KVS used elements will have following tokens:
 *      [ Component ID | type=key | index ]
 *
 *  A Key-Value stored will end up with:
 *  - key      = [ Component ID | type=key  | index ] [ key Hash | payload index 0 | payload index 1 | ...]
 *  - payload0 = [ Component ID | type=data | index ] [ payload byte 0 | 1 | 2 | ...]
 *  - payload1 = [ Component ID | type=data | index ] [ payload byte MAX_ | MAX_ + 1 | ...]
 *
 *  This implementation can store data sizes MAX_KVS_VALUE_LEN*MAX_KVS_PAYLOAD_EXTENSIONS.
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
/* <CodeGenerator Placeholder> Includes */

//#define DEBUG
//#define GP_LOCAL_LOG
#define GP_COMPONENT_ID GP_COMPONENT_ID_QVCHIP

#include "qvCHIP.h"

#include "hal.h"
#include "gpLog.h"
#include "gpNvm.h"
#include "gpNvm_NvmProtect.h"


#ifdef QVCHIP_DIVERSITY_KVS_HASH_KEYS
#include "mbedtls/sha256.h"
#endif // QVCHIP_DIVERSITY_KVS_HASH_KEYS
/* </CodeGenerator Placeholder> Includes */

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/* <CodeGenerator Placeholder> Macro */

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/* Pool ID for KVS is the same as pool id for NVN implementation - they share the same pool */
#define KVS_POOL_ID (0)

/* Maximum length for values stored in KVS - reserve last byte for keys larger than maximum value size */
#define MAX_KVS_KEY_LEN (GP_NVM_MAX_TOKENLENGTH - 2)

/* Maximum length for the token mask when looking for a specific token */
#define MAX_KVS_TOKENMASK_LEN (GP_NVM_MAX_TOKENLENGTH - 1)

/* Maximum ID extensions to support - 254*10 */
#define MAX_KVS_PAYLOAD_EXTENSIONS 10

/* Maximum length for values stored in KVS */
#ifdef GP_DIVERSITY_ROMUSAGE_FOR_MATTER
#define MAX_KVS_VALUE_LEN (251)
#else //GP_DIVERSITY_ROMUSAGE_FOR_MATTER
#define MAX_KVS_VALUE_LEN (255)
#endif //GP_DIVERSITY_ROMUSAGE_FOR_MATTER

/* Variable settings definitions */
#ifndef GP_NVM_NBR_OF_UNIQUE_TOKENS
#define GP_NVM_NBR_OF_UNIQUE_TOKENS GP_NVM_NBR_OF_UNIQUE_TAGS
#endif //GP_NVM_NBR_OF_UNIQUE_TOKENS
#if GP_NVM_NBR_OF_POOLS > 1
#error CHIP glue layer only built for use with 1 pool currently.
#endif //GP_NVM_NBR_OF_POOLS

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/** @brief Key information element type - will be populated with qvCHIP_KvsKeyPayload_t */
#define qvCHIP_KvsTypeKey 0x1
/** @brief Data element type */
#define qvCHIP_KvsTypeData 0x2
/** @typedef qvCHIP_KvsType_t
 *  @brief The qvCHIP_KvsType_t defines the types of NVM records used for KVS.
*/
typedef UInt8 qvCHIP_KvsType_t;

/** @brief Token payload for all KVS elements added in NVM.
    Struct used as representation of the byte stream to be written as NVM record
*/
typedef PACKED_PRE struct qvCHIP_KvsToken_s {
    /** @brief qvCHIP component ID - fixed */
    uint8_t componentId;
    /** @brief Type of KVS data */
    qvCHIP_KvsType_t type;
    uint8_t index;
} PACKED_POST qvCHIP_KvsToken_t;

/** @brief NVM record payload used for a KVS key element
    Strucct used as representation of the byte stream to be written as NVM record.
*/
typedef PACKED_PRE struct qvCHIP_KvsKeyPayload {
    /** @brief Key hash for the char key given at API level */
    uint8_t keyHash[MAX_KVS_KEY_LEN];
    /** @brief List of indices to payload elements.
    *   A payload can be split-up in multiple elements to fit a single NVM element.
    *   Each index is put in this list in sequence in which the pieces were broken down in.
    */
    uint8_t payloadIndices[MAX_KVS_PAYLOAD_EXTENSIONS];
} PACKED_POST qvCHIP_KvsKeyPayload_t;

/* </CodeGenerator Placeholder> Macro */

/*****************************************************************************
 *                    Static Data
 *****************************************************************************/

/** @brief Mutex for all public API calls */
HAL_CRITICAL_SECTION_DEF(qvCHIP_KvsMutex)

/** @brief Start of NVM area - linkerscript defined. */
extern const UIntPtr gpNvm_Start;

/** @brief Persistent LUT handle for KVS storage, used for all NVM operations */
gpNvm_LookupTable_Handle_t qvCHIP_KvsLookupHandle = gpNvm_LookupTable_Handle_Invalid;


/*****************************************************************************
 *                    Static Component Function Definitions
 *****************************************************************************/
#ifdef QVCHIP_DIVERSITY_KVS_HASH_KEYS

/** @brief Convert a char string to a hash to limit amount of bytes required
 *
 *  @param[in]  length Length of key.
 *  @param[in]  key    Array holding the key.
 *  @param[out] hash   Array to return calculated hash value from key.
*/
static qvStatus_t qvCHIP_KvsHashKey(uint8_t length, uint8_t* key, uint8_t* hash)
{
    int ret;
    mbedtls_sha256_context qvCHIP_Kvs_HashContext;
    mbedtls_sha256_init(&qvCHIP_Kvs_HashContext);
    ret = mbedtls_sha256_starts_ret(&qvCHIP_Kvs_HashContext, 0);
    if(ret)
    {
        goto exit;
    }
    ret = mbedtls_sha256_update_ret(&qvCHIP_Kvs_HashContext, key, length);
    if(ret)
    {
        goto exit;
    }
    ret = mbedtls_sha256_finish_ret(&qvCHIP_Kvs_HashContext, hash);
    if(ret)
    {
        goto exit;
    }
exit:
    mbedtls_sha256_free(&qvCHIP_Kvs_HashContext);
    return (ret) ? QV_STATUS_INVALID_DATA : QV_STATUS_NO_ERROR;
}
#endif // QVCHIP_DIVERSITY_KVS_HASH_KEYS

/** @brief Create a byte based hash from a key as string
 *
 *  @param[in]  key         String key
 *  @param[out] hashBuffer  Byte buffer to return hash created from the kes string
*/
static qvStatus_t qvCHIP_KvsCreateHash(const char* key, uint8_t* hashBuffer)
{
    qvStatus_t qvStatus = QV_STATUS_NO_ERROR;
    uint8_t keyLen = strlen(key);
#ifdef QVCHIP_DIVERSITY_KVS_HASH_KEYS
    uint8_t hash[32];
    qvStatus = qvCHIP_KvsHashKey(keyLen, (uint8_t*)key, hash);
    if(QV_STATUS_NO_ERROR != qvStatus)
    {
        return qvStatus;
    }
    MEMCPY(hashBuffer, hash, MAX_KVS_KEY_LEN);
#else
    if(keyLen > MAX_KVS_KEY_LEN)
    {
        GP_LOG_PRINTF("WARNING: Key is too long");
        return QV_STATUS_INVALID_DATA;
    }
    /* make sure we only use the string part of the key and not the rest that
       follows after the string terminator */
    MEMSET(hashBuffer, 0x00, MAX_KVS_KEY_LEN);
    MEMCPY(hashBuffer, key, MIN(keyLen, MAX_KVS_KEY_LEN));
#endif // QVCHIP_DIVERSITY_KVS_HASH_KEYS

    GP_LOG_PRINTF("'%s' -> %02x%02x%02x%02x...", 0, key,
                  hashBuffer[0], hashBuffer[1], hashBuffer[2], hashBuffer[3]);

    return qvStatus;
}

/************************
* Element handling
*************************/

/** @brief Retrieve a free index for a given KVS element type
 *
 *  @param type Type of KVS element to look for (key/data)
*/
static UInt8 qvCHIPKvs_FindFreeIndex(qvCHIP_KvsType_t type)
{
    gpNvm_Result_t nvmResult;

    qvCHIP_KvsToken_t tokenMask = {
        .componentId = GP_COMPONENT_ID,
        .type = type,
        .index = 0xFF};

    // Indices allocated in order - first empty is useable
    tokenMask.index = 0;

    nvmResult = gpNvm_ResetIterator(qvCHIP_KvsLookupHandle);
    if(nvmResult != gpNvm_Result_DataAvailable)
    {
        goto _cleanup;
    }

    do
    {
        UInt8 dataLen;

        // Scrolling through LUT for key elements with index in tokenmask
        nvmResult = gpNvm_ReadUniqueProtected(qvCHIP_KvsLookupHandle, KVS_POOL_ID, gpNvm_UpdateFrequencyIgnore, NULL,
                                              sizeof(tokenMask), (uint8_t*)&tokenMask,
                                              MAX_KVS_VALUE_LEN,
                                              &dataLen,
                                              NULL);
        if(nvmResult == gpNvm_Result_DataAvailable)
        {
            // Index already in use, look for a new one
            tokenMask.index++;
        }
        else if(nvmResult == gpNvm_Result_NoDataAvailable)
        {
            // Index not yet written - use as free index
            break;
        }
        else
        {
            GP_LOG_PRINTF("key lookup failed:%x", 0, nvmResult);
            tokenMask.index = 0xFF;
            goto _cleanup;
        }
    } while(nvmResult == gpNvm_Result_DataAvailable);
    GP_LOG_PRINTF("0: |%x|%x found free token", 0, type, tokenMask.index);


_cleanup:
    return tokenMask.index;
}

/************************
* Key Element handling
*************************/
#ifdef GP_LOCAL_LOG
/** @brief Dump the information of a KVS key element
 *
 *  @param keyElementIndex Index of element used in NVM area
 *  @param amountOfIndices Amount of payload indices in key element.
 *  @param pKeyPayload     Pointer to struct reprentation of key NVM element byte payload.
*/
static void qvCHIP_KvsDumpKeyElement(UInt8 keyElementIndex, UInt8 amountOfIndices, qvCHIP_KvsKeyPayload_t* pKeyPayload)
{
    GP_LOG_SYSTEM_PRINTF("%u: %02x%02x%02x%02x... | %u [%u, %u, ...]", 0,
                         keyElementIndex,
                         pKeyPayload->keyHash[0], pKeyPayload->keyHash[1], pKeyPayload->keyHash[2], pKeyPayload->keyHash[3],
                         amountOfIndices,
                         pKeyPayload->payloadIndices[0], amountOfIndices > 1 ? pKeyPayload->payloadIndices[1] : 0xFF);
    gpLog_Flush();
}
#else
#define qvCHIP_KvsDumpKeyElement(keyElementIndex, amountOfIndices, keyPayload)
#endif

/** @brief Add a KVS key information element to NVM.
 *
 * @param[in] keyElementIndex Index to use for key storage
 * @param[in] keyHash Array of hash bytes to use as key in the element
 * @param[in] amountOfIndices Number of payload extension elements that will be linked to this key
 * @param[in] payloadIndices List of indices to use to store the payload parts.
 *                           This list is used as ordered to store parts sequentally.
 *
 * @return status Possible values from qvStatus_t:
 *                - QV_STATUS_NVM_ERROR - any failure in gpNvm calls
 *                - QV_STATUS_NO_ERROR
*/
static qvStatus_t qvCHIP_KvsAddKeyElement(UInt8 keyElementIndex, const UInt8* keyHash, UInt8 amountOfIndices, const UInt8* payloadIndices)
{
    gpNvm_Result_t nvmResult;

    // Fill in index for key element
    qvCHIP_KvsToken_t keyTokenMask = {
        .componentId = GP_COMPONENT_ID,
        .type = qvCHIP_KvsTypeKey,
        .index = keyElementIndex};
    qvCHIP_KvsKeyPayload_t keyPayload;

    // Fill in payload = key | payload references
    if(keyHash == NULL)
    {
        MEMSET(&keyPayload.keyHash, 0x0, MAX_KVS_KEY_LEN);
    }
    else
    {
        MEMCPY(&keyPayload.keyHash, keyHash, MAX_KVS_KEY_LEN);
    }
    MEMCPY(&keyPayload.payloadIndices, payloadIndices, amountOfIndices);

    GP_LOG_PRINTF("Adding key", 0);
    qvCHIP_KvsDumpKeyElement(keyElementIndex, amountOfIndices, &keyPayload);

    // Write to NVM
    nvmResult = gpNvm_WriteProtected(KVS_POOL_ID, gpNvm_UpdateFrequencyIgnore,
                            sizeof(keyTokenMask), (uint8_t*)&keyTokenMask,
                            sizeof(keyPayload.keyHash) + amountOfIndices, (uint8_t*)&keyPayload);
    if(nvmResult != gpNvm_Result_DataAvailable)
    {
        // Write failed - NVM full ?
        return QV_STATUS_NVM_ERROR;
    }

    return QV_STATUS_NO_ERROR;
}

/** @brief Retrieve a certain KVS key information record based on the key hash.
 *
 * @param[in] keyHash
 * @param[out] pKeyIndex Pointer in which to return the index of the KVS key element
 * @param[out] payloadIndicesAmount Amount of payload parts found
 * @param[out] pPayloadIndices Pointer to list to return payload indices
 *
 * @return status Possible values from qvStatus_t:
 *                - QV_STATUS_NVM_ERROR - any failure in gpNvm calls
 *                - QV_STATUS_NO_ERROR
*/
static qvStatus_t qvCHIPVS_FindKeyDataInfo(const UInt8* keyHash, UInt8* pKeyIndex, UInt8* pPayloadIndicesAmount, UInt8* pPayloadIndices)
{
    qvStatus_t qvStatus;
    gpNvm_Result_t nvmResult;

    // Initialize in case not found
    *pKeyIndex = 0xFF;
    qvStatus = QV_STATUS_INVALID_DATA;

    GP_LOG_PRINTF("Finding -> %02x%02x%02x%02x... l:%u", 0,
                  keyHash[0], keyHash[1], keyHash[2], keyHash[3], qvCHIP_KvsLookupHandle);

    nvmResult = gpNvm_ResetIterator(qvCHIP_KvsLookupHandle);
    if(nvmResult != gpNvm_Result_DataAvailable)
    {
        return QV_STATUS_NVM_ERROR;
    }

    do
    {
        qvCHIP_KvsToken_t lookupMask;
        UInt8 lookupLen;                // Only used for check
        qvCHIP_KvsKeyPayload_t dataKey; // Required for full info fetch
        UInt8 dataLen;                  // Used to determine length of indices

        nvmResult = gpNvm_ReadNextProtected(qvCHIP_KvsLookupHandle, KVS_POOL_ID, NULL, GP_NVM_MAX_TOKENLENGTH, &lookupLen, (uint8_t*)&lookupMask, sizeof(dataKey), &dataLen, (uint8_t*)&dataKey);
        GP_LOG_PRINTF("r:%u mask:%u %u|%u|%u data:%u %02x%02x%02x%02x...", 0, nvmResult,
                      lookupLen, lookupMask.componentId, lookupMask.type, lookupMask.index,
                      dataLen, dataKey.keyHash[0], dataKey.keyHash[1], dataKey.keyHash[2], dataKey.keyHash[3]);
        if(nvmResult == gpNvm_Result_NoDataAvailable)
        {
            // Nothing more found in LUT
            qvStatus = QV_STATUS_INVALID_DATA;
            goto _cleanup;
        }

        if(lookupMask.type != qvCHIP_KvsTypeKey)
        {
            // Only look for key type elements
            // Result could be gpNvm_Result_Truncated due to the data size given.
            continue;
        }

        if(nvmResult != gpNvm_Result_DataAvailable)
        {
            // Real failure reasons
            qvStatus = QV_STATUS_NVM_ERROR;
            goto _cleanup;
        }

        // Found NVM record exceeds expected fixed key length
        // Or token used is inconsistent with KVS used structure
        if((dataLen < MAX_KVS_KEY_LEN) || (lookupLen != sizeof(lookupMask)))
        {
            qvStatus = QV_STATUS_NVM_ERROR;
            goto _cleanup;
        }

        // Check for key we're looking for
        if(MEMCMP(&dataKey.keyHash[0], keyHash, MAX_KVS_KEY_LEN) == 0)
        {
            *pKeyIndex = lookupMask.index;

            // Return Payload index information
            *pPayloadIndicesAmount = dataLen - MAX_KVS_KEY_LEN;
            MEMCPY(pPayloadIndices, &dataKey.payloadIndices, dataLen - MAX_KVS_KEY_LEN);

            GP_LOG_PRINTF("Key found", 0);
            qvCHIP_KvsDumpKeyElement(*pKeyIndex, *pPayloadIndicesAmount, &dataKey);

            qvStatus = QV_STATUS_NO_ERROR;
            break;
        }
    } while(nvmResult != gpNvm_Result_NoDataAvailable);

_cleanup:
    return qvStatus;
}

/************************
* Data element handling
*************************/

/** @brief Adding KVS data parts to NVM
 *
 *  @param[in] valueSize Size of full data block to store (in bytes)
 *  @param[in] value Pointer to byte array to store
 *  @param[in] amountOfIndices Amount of indices used to split up blocks in separate NVM records
 *  @param[in,out] pPayloadIndices List of indices to use to store payload blocks.
 *                                 If no previous indices were given (set to 0xFF), new indices will be allocated.
 *
 *  @return status Possible values from qvStatus_t:
 *                - QV_STATUS_NVM_ERROR - any failure in gpNvm calls
 *                - QV_STATUS_NO_ERROR
*/
static qvStatus_t qvCHIP_KvsAddDataElements(size_t valueSize, const void* value, uint8_t amountOfIndices, uint8_t* pPayloadIndices)
{
    gpNvm_Result_t nvmResult;

    for(UInt8 j = 0; j < amountOfIndices; j++)
    {
        qvCHIP_KvsToken_t dataTokenMask = {
            .componentId = GP_COMPONENT_ID,
            .type = qvCHIP_KvsTypeData,
            .index = 0xFF};

        // Complete token mask for a data piece
        // If key was already know, we'll be re-using the payload indices
        dataTokenMask.index = pPayloadIndices[j];
        if(dataTokenMask.index == 0xFF)
        {
            // No index known before - new element
            // Find a free element for a piece of data payload
            dataTokenMask.index = qvCHIPKvs_FindFreeIndex(qvCHIP_KvsTypeData);
            if(dataTokenMask.index == 0xFF)
            {
                GP_LOG_PRINTF("NVM error.", 0);
                return QV_STATUS_NVM_ERROR;
            }
            else
            {
                // Save in indices reference for key
                pPayloadIndices[j] = dataTokenMask.index;
            }
        }

        // Write Data payload element
        nvmResult = gpNvm_WriteProtected(KVS_POOL_ID, gpNvm_UpdateFrequencyIgnore,
                                sizeof(dataTokenMask), (uint8_t*)&dataTokenMask,
                                min(valueSize, MAX_KVS_VALUE_LEN), &((uint8_t*)value)[j * MAX_KVS_VALUE_LEN]);
        if(nvmResult != gpNvm_Result_DataAvailable)
        {
            // Write failed - NVM full ?
            GP_LOG_PRINTF("NVM add data fail r:%x", 0, nvmResult);
            return QV_STATUS_NVM_ERROR;
        }
        valueSize -= min(valueSize, MAX_KVS_VALUE_LEN);
    }

    return QV_STATUS_NO_ERROR;
}

/** @brief Retrieve all data stored in several data elements.
 *
 * @param[in] amountOfIndices Amount of different blocks to read from
 * @param[in] pPayloadIndices Array of indices to collect the data from
 * @param[in] startOffset Offset to start reading in first found element
 * @param[in] maxBytesToRead Limit on bytes to read - Partial read out of the available data possible
 * @param[out] readBytesSize Amount of bytes read from existing data.
 *                           Could be less then maxBytesToRead when data is smaller then given read buffer
 * @param[out] pReadBuffer Buffer for the data to read back.
 *
 *  @return status Possible values from qvStatus_t:
 *                - QV_STATUS_NVM_ERROR - any failure in gpNvm calls
 *                - QV_STATUS_INVALID_ARGUMENT
 *                - QV_STATUS_BUFFER_TOO_SMALL - buffer in which can be returned is too small
 *                - QV_STATUS_NO_ERROR
*/
static qvStatus_t qvCHIP_KvsGetDataElements(UInt8 amountOfIndices, const UInt8* pPayloadIndices, UInt8 startOffset, size_t maxBytesToRead,
                                            size_t* readBytesSize, UInt8* pReadBuffer)
{
    qvStatus_t qvStatus;
    gpNvm_Result_t nvmResult;

    qvCHIP_KvsToken_t tokenMask = {
        .componentId = GP_COMPONENT_ID,
        .type = qvCHIP_KvsTypeData,
        .index = 0xFF};

    qvStatus = QV_STATUS_NO_ERROR;

    nvmResult = gpNvm_ResetIterator(qvCHIP_KvsLookupHandle);
    if(nvmResult != gpNvm_Result_DataAvailable)
    {
        return QV_STATUS_NVM_ERROR;
    }

    // Paste together all extension pieces
    *readBytesSize = 0;
    for(uint8_t i = 0; i < amountOfIndices; i++)
    {
        UInt8 dataLen;
        UInt8 tempBuffer[MAX_KVS_VALUE_LEN]; // Temp buffer for offset handling and limiting data read-out

        tokenMask.index = pPayloadIndices[i];
        nvmResult = gpNvm_ReadUniqueProtected(qvCHIP_KvsLookupHandle, KVS_POOL_ID, gpNvm_UpdateFrequencyIgnore, NULL,
                                              sizeof(tokenMask), (uint8_t*)&tokenMask, MAX_KVS_VALUE_LEN, &dataLen, tempBuffer);
        if(nvmResult != gpNvm_Result_DataAvailable)
        {
            qvStatus = QV_STATUS_INVALID_DATA;
            goto _cleanup;
        }

        if(i == 0)
        {
            // First pass - take offset into account
            if(startOffset > dataLen)
            {
                qvStatus = QV_STATUS_INVALID_ARGUMENT;
                goto _cleanup;
            }
            dataLen -= startOffset;
            startOffset = 0; // Offset no longer relevant after first pass
        }

        // Copy content retrieved to read buffer
        // Take into account:
        // - offset for the first chunk read-out
        // - limit on read-out - more can be asked than is available
        const UInt8 copyLen = min(maxBytesToRead, dataLen);
        MEMCPY(&pReadBuffer[*readBytesSize], &tempBuffer[startOffset], copyLen);

        maxBytesToRead -= copyLen;
        *readBytesSize += copyLen;
        if(maxBytesToRead == 0)
        {
            if(copyLen != dataLen)
            {
                qvStatus = QV_STATUS_BUFFER_TOO_SMALL;
                break;
            }
            else
            {
                // All requested bytes were read - can be only a portion of the data
                qvStatus = QV_STATUS_NO_ERROR;
                break;
            }
        }
    }

_cleanup:
    return qvStatus;
}

/** @brief Delete a KVS element - not protected by a mutex
*
*   @param[in] keyHash List of hash bytes to identify full (key + payload) KVS record to delete
*/
static qvStatus_t qvCHIP_KvsDeleteInternal(const uint8_t* keyHash)
{
    qvStatus_t qvStatus;
    gpNvm_Result_t nvmResult;

    /* check parameters*/
    if(keyHash == NULL)
    {
        return QV_STATUS_INVALID_ARGUMENT;
    }

    UInt8 payloadIndices[MAX_KVS_PAYLOAD_EXTENSIONS];
    UInt8 payloadIndicesAmount;
    UInt8 keyIndex;

    // Retrieve list of payload elements that come with the key
    qvStatus = qvCHIPVS_FindKeyDataInfo(keyHash, &keyIndex, &payloadIndicesAmount, payloadIndices);
    if(QV_STATUS_NO_ERROR != qvStatus)
    {
        GP_LOG_PRINTF("KvsDel r:%x", 0, qvStatus);
        return qvStatus;
    }

    // Mark delete in progress - set hash to 0x0
    qvCHIP_KvsAddKeyElement(keyIndex, NULL, payloadIndicesAmount, payloadIndices);

    qvCHIP_KvsToken_t token = {
        .componentId = GP_COMPONENT_ID,
        .type = qvCHIP_KvsTypeData,
        .index = 0xFF,
    };
    // Remove all payload chunks associated
    for(UInt8 i = 0; i < payloadIndicesAmount; i++)
    {
        token.index = payloadIndices[i];

        nvmResult = gpNvm_RemoveProtected(KVS_POOL_ID, gpNvm_UpdateFrequencyIgnore, sizeof(token), (uint8_t*)&token);
        if(nvmResult != gpNvm_Result_DataAvailable)
        {
            qvStatus = QV_STATUS_INVALID_DATA;
            goto _cleanup;
        }
    }

    // Remove Key Element
    token.type = qvCHIP_KvsTypeKey;
    token.index = keyIndex;

    nvmResult = gpNvm_RemoveProtected(KVS_POOL_ID, gpNvm_UpdateFrequencyIgnore, sizeof(token), (uint8_t*)&token);
    if(nvmResult != gpNvm_Result_DataAvailable)
    {
        qvStatus = QV_STATUS_INVALID_DATA;
        goto _cleanup;
    }

_cleanup:
    return qvStatus;
}

/*****************************************************************************
 *                    Public Component Function Definitions
 *****************************************************************************/

qvStatus_t qvCHIP_KvsInit(void)
{
#ifdef GP_NVM_DIVERSITY_VARIABLE_SETTINGS
    UInt8 availableMaxTokenLength;
    UIntPtr currentNvmStart;
    gpNvm_KeyIndex_t currentNumberOfUniqueTokens;

    gpNvm_GetVariableSettings(&currentNvmStart, &currentNumberOfUniqueTokens, &availableMaxTokenLength);
    GP_LOG_PRINTF("Settings old/new: Nvm Start:%lx/%lx #Uniq Token:%u/%u Token Length:%u/%u", 0,
                  (unsigned long)currentNvmStart, (unsigned long)&gpNvm_Start,
                  currentNumberOfUniqueTokens, GP_NVM_NBR_OF_UNIQUE_TOKENS,
                  availableMaxTokenLength, GP_NVM_MAX_TOKENLENGTH);

    if(availableMaxTokenLength < GP_NVM_MAX_TOKENLENGTH)
    {
        GP_LOG_SYSTEM_PRINTF("Max token length %u < %u", 0, availableMaxTokenLength, GP_NVM_MAX_TOKENLENGTH);
        return QV_STATUS_BUFFER_TOO_SMALL;
    }

    gpNvm_SetVariableSettings((UIntPtr)&gpNvm_Start, GP_NVM_NBR_OF_UNIQUE_TOKENS);
#endif //GP_NVM_DIVERSITY_VARIABLE_SETTINGS
#ifdef GP_NVM_DIVERSITY_VARIABLE_SIZE
    UInt16 currentNrOfSectors;
    UInt8 currentNrOfPools;
    UInt8 currentSectorsPerPool[4]; // Max from NVM implementation

    UInt8 sectorsPerPool[] = {GP_NVM_POOL_1_NBR_OF_PHY_SECTORS};

    gpNvm_GetVariableSize(&currentNrOfSectors, &currentNrOfPools, currentSectorsPerPool);
    GP_LOG_PRINTF("Sizes old/new: #sectors:%u/%u #pools:%u/%u", 0,
                  currentNrOfSectors, sectorsPerPool[0],
                  currentNrOfPools, GP_NVM_NBR_OF_POOLS);

    gpNvm_SetVariableSize(sectorsPerPool[0], GP_NVM_NBR_OF_POOLS, sectorsPerPool);
#endif //GP_NVM_DIVERSITY_VARIABLE_SIZE

    hal_MutexCreate(&qvCHIP_KvsMutex);
    if(!hal_MutexIsValid(qvCHIP_KvsMutex))
    {
        return QV_STATUS_NVM_ERROR;
    }

    // LUT handle will be initialized on first use
    qvCHIP_KvsLookupHandle = gpNvm_LookupTable_Handle_Invalid;


    return QV_STATUS_NO_ERROR;
}

/** @brief Allocate Lookup handle for KVS use of NVM space
 *
 *  @return status Possible values from qvStatus_t:
 *                - QV_STATUS_NVM_ERROR - any failure in gpNvm call to build LUT
 *                - QV_STATUS_NO_ERROR
 */
static qvStatus_t qvCHIPKvs_BuildLookup(void)
{
    // Allocate once
    if(qvCHIP_KvsLookupHandle == gpNvm_LookupTable_Handle_Invalid)
    {
        // Allocate LUT for KVS storage
        UInt8 lookupMask[1] = {GP_COMPONENT_ID};
        UInt8 nrOfMatches;
        gpNvm_Result_t nvmResult;

        nvmResult = gpNvm_BuildLookupProtected(&qvCHIP_KvsLookupHandle, KVS_POOL_ID, gpNvm_UpdateFrequencyIgnore,
                                               sizeof(lookupMask), lookupMask,
                                               GP_NVM_NBR_OF_UNIQUE_TOKENS, &nrOfMatches);
        GP_LOG_PRINTF("KVS: LUT:%u allocated - %u elements in use", 0, qvCHIP_KvsLookupHandle, nrOfMatches);

        if(nvmResult != gpNvm_Result_DataAvailable)
        {
            return QV_STATUS_NVM_ERROR;
        }
    }
    return QV_STATUS_NO_ERROR;
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
qvStatus_t qvCHIP_KvsPut(const char* key, const void* value, size_t valueSize)
{
    qvStatus_t qvStatus;
    uint8_t keyHash[MAX_KVS_KEY_LEN];

    if((key == NULL) || (value == NULL))
    {
        return QV_STATUS_INVALID_ARGUMENT;
    }

    if(((valueSize / MAX_KVS_VALUE_LEN) + 1) > MAX_KVS_PAYLOAD_EXTENSIONS)
    {
        // Element too large to be split up in extension parts
        return QV_STATUS_INVALID_ARGUMENT;
    }

    qvStatus = qvCHIP_KvsCreateHash(key, keyHash);
    if(QV_STATUS_NO_ERROR != qvStatus)
    {
        return qvStatus;
    }

    hal_MutexAcquire(qvCHIP_KvsMutex);

    qvStatus = qvCHIPKvs_BuildLookup();
    if(QV_STATUS_NO_ERROR != qvStatus)
    {
        goto _cleanup;
    }

    UInt8 keyIndex;
    UInt8 payloadIndices[MAX_KVS_PAYLOAD_EXTENSIONS];
    UInt8 payloadIndicesAmount;
    UInt8 expectedPayloadIndicesAmount = 0;
    if (valueSize > 0)
    {
        expectedPayloadIndicesAmount = (((valueSize - 1) / MAX_KVS_VALUE_LEN) + 1);
    }

    // Retrieve list of payload elements that come with the key
    qvStatus = qvCHIPVS_FindKeyDataInfo(keyHash, &keyIndex, &payloadIndicesAmount, payloadIndices);

    if(QV_STATUS_INVALID_DATA == qvStatus && keyIndex == 0xFF)
    {
        //No key found yet in NVM - initialize:
        keyIndex = qvCHIPKvs_FindFreeIndex(qvCHIP_KvsTypeKey);
        if(keyIndex == 0xFF)
        {
            GP_LOG_PRINTF("NVM error", 0);
            qvStatus = QV_STATUS_NVM_ERROR;
            goto _cleanup;
        }
        payloadIndicesAmount = expectedPayloadIndicesAmount;
        MEMSET(payloadIndices, 0xFF, expectedPayloadIndicesAmount);
    }
    else if(QV_STATUS_NO_ERROR != qvStatus)
    {
        GP_LOG_PRINTF("KvsPut FindKey '%s' r:%x", 0, key, qvStatus);
        goto _cleanup;
    }
    else
    {
        if(payloadIndicesAmount != expectedPayloadIndicesAmount)
        {
            // Adjust indices list
            if(expectedPayloadIndicesAmount < payloadIndicesAmount)
            {
                // When we attempt to write a smaller value than before, reset indices for the difference in length
                MEMSET(&payloadIndices[expectedPayloadIndicesAmount], 0xFF, payloadIndicesAmount - expectedPayloadIndicesAmount);
            }
            payloadIndicesAmount = expectedPayloadIndicesAmount;
        }
    }

    // Store data parts first - in case operation is interrupted these can be flagged as invalid
    qvStatus = qvCHIP_KvsAddDataElements(valueSize, value, payloadIndicesAmount, payloadIndices);
    if(QV_STATUS_NO_ERROR != qvStatus)
    {
        goto _cleanup;
    }
    // Finish by writing key reference
    qvStatus = qvCHIP_KvsAddKeyElement(keyIndex, keyHash, payloadIndicesAmount, payloadIndices);
    if(QV_STATUS_NO_ERROR != qvStatus)
    {
        goto _cleanup;
    }

_cleanup:
    hal_MutexRelease(qvCHIP_KvsMutex);

    return qvStatus;
}

qvStatus_t qvCHIP_KvsGet(const char* key, void* value, size_t valueSize, size_t* readBytesSize,
                         size_t offsetBytes)
{
    uint8_t keyHash[MAX_KVS_KEY_LEN];
    qvStatus_t qvStatus = QV_STATUS_NO_ERROR;

    /* check parameters*/
    if((key == NULL) || (value == NULL) || (readBytesSize == NULL) ||
       (valueSize > MAX_KVS_PAYLOAD_EXTENSIONS * MAX_KVS_VALUE_LEN))
    {
        return QV_STATUS_INVALID_ARGUMENT;
    }

    // Caller expects this to be initialized.
    *readBytesSize = 0;

    qvStatus = qvCHIP_KvsCreateHash(key, keyHash);
    if(QV_STATUS_NO_ERROR != qvStatus)
    {
        return qvStatus;
    }

    hal_MutexAcquire(qvCHIP_KvsMutex);

    qvStatus = qvCHIPKvs_BuildLookup();
    if(QV_STATUS_NO_ERROR != qvStatus)
    {
        goto _cleanup;
    }

    UInt8 payloadIndices[MAX_KVS_PAYLOAD_EXTENSIONS];
    UInt8 payloadIndicesAmount;
    UInt8 keyIndex;
    NOT_USED(keyIndex);

    // Retrieve list of payload elements that come with the key
    qvStatus = qvCHIPVS_FindKeyDataInfo(keyHash, &keyIndex, &payloadIndicesAmount, payloadIndices);
    if(QV_STATUS_NO_ERROR != qvStatus)
    {
        goto _cleanup;
    }

    // Offset outside of available data ?
    if(offsetBytes / MAX_KVS_VALUE_LEN > payloadIndicesAmount)
    {
        qvStatus = QV_STATUS_INVALID_ARGUMENT;
        goto _cleanup;
    }

    // Fill up the read value with all payload elements listed
    // Take offset into account - start from later element if offset > 1 block
    qvStatus = qvCHIP_KvsGetDataElements(payloadIndicesAmount, &payloadIndices[offsetBytes / MAX_KVS_VALUE_LEN],
                                         offsetBytes % MAX_KVS_VALUE_LEN, valueSize, readBytesSize, (uint8_t*)value);
    if(QV_STATUS_NO_ERROR != qvStatus)
    {
        goto _cleanup;
    }

_cleanup:
    hal_MutexRelease(qvCHIP_KvsMutex);

    return qvStatus;
}

/** @brief Delete a KVS element - protected by a mutex */
qvStatus_t qvCHIP_KvsDelete(const char* key)
{
    qvStatus_t qvStatus;
    UInt8 keyHash[MAX_KVS_KEY_LEN];


    if((key == NULL))
    {
        return QV_STATUS_INVALID_ARGUMENT;
    }

    qvStatus = qvCHIP_KvsCreateHash(key, keyHash);
    if(QV_STATUS_NO_ERROR != qvStatus)
    {
        return qvStatus;
    }

    hal_MutexAcquire(qvCHIP_KvsMutex);

    qvStatus = qvCHIPKvs_BuildLookup();
    if(QV_STATUS_NO_ERROR != qvStatus)
    {
        goto _cleanup;
    }
    qvStatus = qvCHIP_KvsDeleteInternal(keyHash);

_cleanup:
    hal_MutexRelease(qvCHIP_KvsMutex);


    return qvStatus;
}
qvStatus_t qvCHIP_KvsErasePartition(void)
{
    gpNvm_Result_t nvmResult;

    qvStatus_t qvStatus = QV_STATUS_NO_ERROR;


    hal_MutexAcquire(qvCHIP_KvsMutex);

    qvStatus = qvCHIPKvs_BuildLookup();
    if(QV_STATUS_NO_ERROR != qvStatus)
    {
        goto _cleanup;
    }

    /* delete all extended tags matching the component ID */
    nvmResult = gpNvm_ResetIterator(qvCHIP_KvsLookupHandle);
    if(nvmResult != gpNvm_Result_DataAvailable)
    {
        qvStatus = QV_STATUS_NVM_ERROR;
        goto _cleanup;
    }

    // List NVM items.
    do
    {
        uint8_t dataLen;
        qvCHIP_KvsToken_t tokenMask;
        uint8_t tokenLength;

        nvmResult = gpNvm_ReadNextProtected(qvCHIP_KvsLookupHandle, KVS_POOL_ID, NULL, sizeof(tokenMask), &tokenLength, (uint8_t*)&tokenMask, MAX_KVS_VALUE_LEN, &dataLen, NULL);
        GP_LOG_PRINTF("r:%u mask:%u %u|%u|%u data:%u", 0, nvmResult,
                      tokenLength, tokenMask.componentId, tokenMask.type, tokenMask.index,
                      dataLen);
        if(nvmResult == gpNvm_Result_NoDataAvailable)
        {
            // Expected at end of iteration
            goto _cleanup;
        }
        else if(nvmResult != gpNvm_Result_DataAvailable)
        {
            qvStatus = QV_STATUS_NVM_ERROR;
            goto _cleanup;
        }

        nvmResult = gpNvm_RemoveProtected(KVS_POOL_ID, gpNvm_UpdateFrequencyIgnore, tokenLength, (uint8_t*)&tokenMask);
        if(nvmResult != gpNvm_Result_DataAvailable)
        {
            qvStatus = QV_STATUS_NVM_ERROR;
            goto _cleanup;
        }
    } while(nvmResult == gpNvm_Result_DataAvailable);

_cleanup:
    // Release Mutex
    hal_MutexRelease(qvCHIP_KvsMutex);


    return qvStatus;
}
