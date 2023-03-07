/******************************************************************************

 @file  crypto_board.c

 @brief TI-RTOS and *Ware implementation of Crypto board service

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2013-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/
#include <xdc/std.h>
#include "crypto_board.h"

#include <inc/hw_types.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/crypto/CryptoCC26XX.h>
#include "icall.h"

/** @internal service dispatcher entity ID */
static ICall_EntityID Crypto_entity;
CryptoCC26XX_Handle CryptoCC26XXHandle;

/* service function.
 * See type description of ICall_ServiceFunc for
 * descriptions of arguments. */
static ICall_Errno Crypto_func(ICall_FuncArgsHdr *args)
{
  if(args->func == CRYPTOBOARD_AES_LOAD_KEY)
  {
    CryptoBoard_AesArgs *cryptoArgs = (CryptoBoard_AesArgs *) args;
    return CryptoCC26XX_allocateKey( CryptoCC26XXHandle,
                                     (CryptoCC26XX_KeyLocation)cryptoArgs->keyLocation,
                                     (const uint32_t *)cryptoArgs->keySrc );
  }
  else if(args->func == CRYPTOBOARD_AES_RELEASE_KEY)
  {
    CryptoBoard_AesArgs *cryptoArgs = (CryptoBoard_AesArgs *) args;
    int keyIndex = (int)cryptoArgs->keyLocation;
    return CryptoCC26XX_releaseKey( CryptoCC26XXHandle, &keyIndex );
  }
  else if(args->func == CRYPTOBOARD_AES_ECB)
  {
    CryptoBoard_AesArgs *cryptoArgs = (CryptoBoard_AesArgs *) args;
    CryptoCC26XX_AESECB_Transaction transaction;

    transaction.opType = CRYPTOCC26XX_OP_AES_ECB;
    transaction.keyIndex = cryptoArgs->keyLocation;
    transaction.msgIn = (uint32_t *)cryptoArgs->msgIn;
    transaction.msgOut = (uint32_t *)cryptoArgs->msgOut;
    return CryptoCC26XX_transact( CryptoCC26XXHandle,
                                  (CryptoCC26XX_Transaction *)&transaction );
  }
  else if(args->func == CRYPTOBOARD_AES_CCM)
  {
    CryptoBoard_AesCcmArgs *cryptoArgs = (CryptoBoard_AesCcmArgs *) args;
    CryptoCC26XX_AESCCM_Transaction transaction;

    transaction.opType = CRYPTOCC26XX_OP_AES_CCM;
    transaction.keyIndex = cryptoArgs->AesCcmTransact.keyLocation;
    transaction.authLength = cryptoArgs->AesCcmTransact.authLength;
    transaction.nonce = (char *)cryptoArgs->AesCcmTransact.nonce;
    transaction.msgIn = (char *)cryptoArgs->AesCcmTransact.plainText;
    transaction.header = (char *)cryptoArgs->AesCcmTransact.header;
    transaction.fieldLength = cryptoArgs->AesCcmTransact.fieldLength;
    transaction.msgInLength = cryptoArgs->AesCcmTransact.plainTextLength;
    transaction.headerLength = cryptoArgs->AesCcmTransact.headerLength;
    transaction.msgOut = (uint32_t *)cryptoArgs->msgOut;
    return CryptoCC26XX_transactPolling( CryptoCC26XXHandle,
                                  (CryptoCC26XX_Transaction *)&transaction );
  }
  else if(args->func == CRYPTOBOARD_AES_CCM_INV)
  {
    CryptoBoard_AesCcmArgs *cryptoArgs = (CryptoBoard_AesCcmArgs *) args;
    CryptoCC26XX_AESCCM_Transaction transaction;

    transaction.opType = CRYPTOCC26XX_OP_AES_CCMINV;
    transaction.keyIndex = cryptoArgs->AesCcmTransact.keyLocation;
    transaction.authLength = cryptoArgs->AesCcmTransact.authLength;
    transaction.nonce = (char *)cryptoArgs->AesCcmTransact.nonce;
    transaction.msgIn = (char *)cryptoArgs->AesCcmTransact.plainText;
    transaction.header = (char *)cryptoArgs->AesCcmTransact.header;
    transaction.fieldLength = cryptoArgs->AesCcmTransact.fieldLength;
    transaction.msgInLength = cryptoArgs->AesCcmTransact.plainTextLength;
    transaction.headerLength = cryptoArgs->AesCcmTransact.headerLength;
    transaction.msgOut = (uint32_t *)cryptoArgs->msgOut;
    return CryptoCC26XX_transactPolling( CryptoCC26XXHandle,
                                  (CryptoCC26XX_Transaction *)&transaction );
  }
  else
  {
    /* Unknown function ID */
    return ICALL_ERRNO_INVALID_FUNCTION;
  }
}


/* See header file to find the function description. */
void Crypto_init(void)
{
  ICall_SyncHandle sem;

  /* Enroll the service to the dispatcher */
  if (ICall_enrollService(ICALL_SERVICE_CLASS_CRYPTO,
                          Crypto_func, &Crypto_entity, &sem) !=
                          ICALL_ERRNO_SUCCESS)
  {
    ICall_abort();
  }
}
