/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
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

//#define GP_LOCAL_LOG

#define GP_COMPONENT_ID GP_COMPONENT_ID_BLESECURITYCOPROCESSOR

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpBle.h"
#include "gpBleComps.h"
#include "gpBleSecurityCoprocessor.h"
#include "gpBle_defs.h"
#include "gpRandom.h"
#include "gpEncryption.h"
#include "gpLog.h"

#if defined(GP_DIVERSITY_BLE_MASTER) || defined(GP_DIVERSITY_BLE_SLAVE)
#include "gpBleLlcp.h"
#endif //GP_DIVERSITY_BLE_MASTER || GP_DIVERSITY_BLE_SLAVE



#ifdef GP_HAL_DIVERSITY_SEC_CRYPTOSOC
#include "gpRandom_HASH_DRBG.h"
#endif //GP_HAL_DIVERSITY_SEC_CRYPTOSOC

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define BLE_SEC_NONCE_LENGTH    13
#define BLE_PACKET_COUNTER_DIRECTION_BIT_MASK       0x80


/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef struct {
    UInt32 lsbs;
    UInt8 msbAndDirection;
} Ble_PacketCounter_t;

typedef struct {
    Ble_PacketCounter_t incoming;
    Ble_PacketCounter_t outgoing;
    UInt8 initVector[BLE_SEC_INIT_VECTOR_LENGTH];
    UInt8 sessionKey[GP_HCI_ENCRYPTION_KEY_LENGTH];
} Ble_SecLinkContext_t;

typedef gpEncryption_Result_t (*Ble_EncryptionFunc_t)(gpEncryption_CCMOptions_t * pCCMOptions);

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

// The security link context per connection
static Ble_SecLinkContext_t Ble_SecLinkContext[BLE_LLCP_MAX_NR_OF_CONNECTIONS];

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

static void Ble_ReverseArray(UInt8* pDst, UInt8* pSrc, UInt8 length);
static gpHci_Result_t Ble_AESEncrypt(UInt8* pInplaceBuffer, UInt8* pKey);
static gpHci_Result_t Ble_PopulateCCMOptions(gpEncryption_CCMOptions_t* pOptions, UInt8* pKey, UInt8* pNonce, Ble_IntConnId_t connId, gpPd_Loh_t* pPdLoh, Bool encrypt);
static void Ble_SecConstructNonce(Ble_IntConnId_t connId, UInt8* pNonce, Bool encrypt);
static gpHci_Result_t Ble_SecurityCoprocessorCcmCommon(Ble_IntConnId_t connId, gpPd_Loh_t* pPdLoh, Bool encrypt);
static void Ble_IncrementPacketCounter(Ble_PacketCounter_t* pCounter);

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

void Ble_ReverseArray(UInt8* pDst, UInt8* pSrc, UInt8 length)
{
    IntLoop i;

    for(i = length - 1; i >=0; i--)
    {
        pDst[length - i - 1] = pSrc[i];
    }
}

// This is a wrapper around gpEncryption, to produce the correct output
gpHci_Result_t Ble_AESEncrypt(UInt8* pInplaceBuffer, UInt8* pKey)
{
    gpHal_Result_t result;
    gpEncryption_AESOptions_t AESoptions;
    UInt8 tmpKey[GP_HCI_ENCRYPTION_KEY_LENGTH];
    UInt8 tmpData[GP_HCI_ENCRYPTION_DATA_LENGTH];

    // We need to reverse both the key and the data in order for our encryption to work correctly
    Ble_ReverseArray(tmpKey, pKey, GP_HCI_ENCRYPTION_KEY_LENGTH);
    Ble_ReverseArray(tmpData, pInplaceBuffer, GP_HCI_ENCRYPTION_DATA_LENGTH);

    // Setup the encryption
    AESoptions.keylen = gpEncryption_AESKeyLen128;
    AESoptions.options = gpEncryption_KeyIdKeyPtr;
    result = gpEncryption_AESEncrypt(tmpData, tmpKey, AESoptions);

    if(result != gpHal_ResultSuccess)
    {
        return gpHci_ResultControllerBusy;
    }

    // Reverse output
    Ble_ReverseArray(pInplaceBuffer, tmpData, GP_HCI_ENCRYPTION_DATA_LENGTH);
    return gpHci_ResultSuccess;
}

// Debug code

gpHci_Result_t Ble_PopulateCCMOptions(gpEncryption_CCMOptions_t* pOptions, UInt8* pKey, UInt8* pNonce, Ble_IntConnId_t connId, gpPd_Loh_t* pPdLoh, Bool encrypt)
{
    UInt16 pdLength = pPdLoh->length;

    if(!encrypt)
    {
        // In case of decrypt, do not take mic length into account for payload data
        if (pdLength > (BLE_SEC_MIC_LENGTH + BLE_PACKET_HEADER_SIZE))
        {
            pdLength -= BLE_SEC_MIC_LENGTH;
        }
        else
        {
            return gpHci_ResultAuthenticationFailure;
        }
    }
    // We need to reverse the key in order for the encryption to be correct
    Ble_ReverseArray(pKey, Ble_SecLinkContext[connId].sessionKey, GP_HCI_ENCRYPTION_KEY_LENGTH);

    Ble_SecConstructNonce(connId, pNonce, encrypt);

    pOptions->pdHandle = pPdLoh->handle;
    pOptions->dataOffset = pPdLoh->offset + BLE_PACKET_HEADER_SIZE;     // Data starts after the 2 bytes header
    pOptions->dataLength = pdLength - BLE_PACKET_HEADER_SIZE;
#ifdef GP_DIVERSITY_DIRECTIONFINDING_SUPPORTED
    UInt8 header = gpPd_ReadByte(pPdLoh->handle, pPdLoh->offset);
    if(header & BLE_DATA_PDU_HEADER_CP_BM)
    {
        // When AoA is supported, check if the CP bit is set.
        // If this is the case, encryption should start one byte later (CTEInfo is not encrypted)
        pOptions->dataOffset++;
        pOptions->dataLength--;
    }
#endif /* GP_DIVERSITY_DIRECTIONFINDING_SUPPORTED */
    pOptions->auxOffset = pPdLoh->offset;                               // offset points to first header byte
    pOptions->auxLength = 1;                                            // a data is only the first byte of the header with NESN,SN and MD bits zero
    pOptions->micLength = BLE_SEC_MIC_LENGTH;
    pOptions->pKey = pKey;
    pOptions->pNonce = pNonce;

    return gpHci_ResultSuccess;
}

void Ble_SecConstructNonce(Ble_IntConnId_t connId, UInt8* pNonce, Bool encrypt)
{
    Ble_PacketCounter_t* pCounter;

    if(encrypt)
    {
        pCounter = &Ble_SecLinkContext[connId].outgoing;
    }
    else
    {
        pCounter = &Ble_SecLinkContext[connId].incoming;
    }

    // 32 packet counter lsbs
    MEMCPY(pNonce, &pCounter->lsbs, sizeof(pCounter->lsbs));

    // 7 packet counter msbs + 1 direction bit
    MEMCPY(&pNonce[4], &pCounter->msbAndDirection, sizeof(pCounter->msbAndDirection));

    // IV
    MEMCPY(&pNonce[5], Ble_SecLinkContext[connId].initVector, sizeof(Ble_SecLinkContext[connId].initVector));
}

gpHci_Result_t Ble_SecurityCoprocessorCcmCommon(Ble_IntConnId_t connId, gpPd_Loh_t* pPdLoh, Bool encrypt)
{
    gpEncryption_CCMOptions_t ccmOptions;
    gpHal_Result_t result;
    Ble_EncryptionFunc_t encryptionFunction;
    Ble_PacketCounter_t* pPacketCounter;
    // Provide storage for key and nonce (will be filled in by populate function)
    UInt8 nonce[BLE_SEC_NONCE_LENGTH];
    UInt8 tmpKey[GP_HCI_ENCRYPTION_KEY_LENGTH];

    GP_ASSERT_DEV_INT(BLE_IS_INT_CONN_HANDLE_VALID(connId));

    if(encrypt)
    {
        pPacketCounter = &Ble_SecLinkContext[connId].outgoing;
        encryptionFunction = gpEncryption_CCMEncrypt;
    }
    else
    {
        pPacketCounter = &Ble_SecLinkContext[connId].incoming;
        encryptionFunction = gpEncryption_CCMDecrypt;
    }

    Ble_IncrementPacketCounter(pPacketCounter);

    if(Ble_PopulateCCMOptions(&ccmOptions, tmpKey, nonce, connId, pPdLoh, encrypt) != gpHci_ResultSuccess)
    {
        return gpHci_ResultAuthenticationFailure;
    }
//#ifdef GP_DIVERSITY_DEVELOPMENT
    //Ble_DumpCCMOptions(&ccmOptions);
//#endif
    result = encryptionFunction(&ccmOptions);

    if(result != gpHal_ResultSuccess)
    {
        return gpHci_ResultAuthenticationFailure;
    }

    return gpHci_ResultSuccess;
}

// Packet counter is 39 bit, does not fit proper into standard type ==> use increment function
void Ble_IncrementPacketCounter(Ble_PacketCounter_t* pCounter)
{
    if(pCounter->lsbs == 0xFFFFFFFF)
    {
        pCounter->lsbs = 0;

        if((pCounter->msbAndDirection & 0x7F) == 0x7F)
        {
            pCounter->msbAndDirection &= BLE_PACKET_COUNTER_DIRECTION_BIT_MASK;
        }
        else
        {
            pCounter->msbAndDirection++;
        }
    }
    else
    {
        pCounter->lsbs++;
    }
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpBle_SecurityCoprocessorReset(Bool firstReset)
{
    // Reset local link context
    MEMSET(Ble_SecLinkContext, 0, sizeof(Ble_SecLinkContext_t)*BLE_LLCP_MAX_NR_OF_CONNECTIONS);
}

// This is a wrapper for the Ble_AESEncrypt function
gpHci_Result_t gpBle_AESEncrypt(UInt8* pInplaceBuffer, UInt8* pKey)
{
    return Ble_AESEncrypt(pInplaceBuffer, pKey);
}

#if defined(GP_DIVERSITY_BLE_MASTER) || defined(GP_DIVERSITY_BLE_SLAVE)
gpHci_Result_t gpBle_SecurityCoprocessorCalculateSessionKey(Ble_IntConnId_t connId, UInt8* pLtk, UInt8* pSkd, UInt8* pIv)
{
    gpHci_Result_t result;

    GP_ASSERT_DEV_INT(BLE_IS_INT_CONN_HANDLE_VALID(connId));

    MEMCPY(Ble_SecLinkContext[connId].initVector, pIv, sizeof(Ble_SecLinkContext[connId].initVector));

    // Reset packet counter
    MEMSET(&Ble_SecLinkContext[connId].incoming, 0xFF, sizeof(Ble_PacketCounter_t));
    MEMSET(&Ble_SecLinkContext[connId].outgoing, 0xFF, sizeof(Ble_PacketCounter_t));

    // Add direction bit
    if(Ble_LlcpIsMasterConnection(connId))
    {
        Ble_SecLinkContext[connId].incoming.msbAndDirection &= ~BLE_PACKET_COUNTER_DIRECTION_BIT_MASK;
        Ble_SecLinkContext[connId].outgoing.msbAndDirection |= BLE_PACKET_COUNTER_DIRECTION_BIT_MASK;
    }
    else
    {
        Ble_SecLinkContext[connId].incoming.msbAndDirection |= BLE_PACKET_COUNTER_DIRECTION_BIT_MASK;
        Ble_SecLinkContext[connId].outgoing.msbAndDirection &= ~BLE_PACKET_COUNTER_DIRECTION_BIT_MASK;
    }

    // Copy the key (will be modified in place)
    MEMCPY(Ble_SecLinkContext[connId].sessionKey, pSkd, GP_HCI_ENCRYPTION_KEY_LENGTH);

    result = Ble_AESEncrypt(Ble_SecLinkContext[connId].sessionKey, pLtk);
    return result;
}
#endif //GP_DIVERSITY_BLE_MASTER || GP_DIVERSITY_BLE_SLAVE

gpHci_Result_t gpBle_SecurityCoprocessorCcmEncrypt(Ble_IntConnId_t connId, gpPd_Loh_t* pPdLoh)
{
    return Ble_SecurityCoprocessorCcmCommon(connId, pPdLoh, true);
}

gpHci_Result_t gpBle_SecurityCoprocessorCcmDecrypt(Ble_IntConnId_t connId, gpPd_Loh_t* pPdLoh)
{
    return Ble_SecurityCoprocessorCcmCommon(connId, pPdLoh, false);
}

/*****************************************************************************
 *                    Public Service Function Definitions
 *****************************************************************************/

gpHci_Result_t gpBle_LeEncrypt(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    gpHci_Result_t result;

    GP_LOG_PRINTF("Encrypt request",0);

    // Use Command complete event to reply to an encrypt command
    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    // Copy data (encryption will be done in place)
    MEMCPY(pEventBuf->payload.commandCompleteParams.returnParams.encryptedData.encryptedData, pParams->LeEncrypt.data, GP_HCI_ENCRYPTION_DATA_LENGTH);

    result = Ble_AESEncrypt(pEventBuf->payload.commandCompleteParams.returnParams.encryptedData.encryptedData, pParams->LeEncrypt.key);

    return result;
}

gpHci_Result_t gpBle_LeRand(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    GP_LOG_PRINTF("Rand", 0);

    // Use Command complete event to reply to a rand command
    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    gpRandom_GetFromDRBG(GP_HCI_RANDOM_DATA_LENGTH, pEventBuf->payload.commandCompleteParams.returnParams.randData.randomNumber);

    return gpHci_ResultSuccess;
}

