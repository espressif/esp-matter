/*
 * Copyright (c) 2013-2016, GreenPeak Technologies
 * Copyright (c) 2017-2020, Qorvo Inc
 *
 * gpEncryption_aes_mmo.c
 *
 * Contains AES MMO based hash value calcuation API
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
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_ENCRYPTION

#include "global.h"
#include "gpAssert.h"
#include "gpEncryption.h"
#include "gpHal_SEC.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/* Key size equal to block size - 128 bits */
#define AES_BLOCK_SIZE_BYTES 16

#ifdef GP_ENCRYPTION_DIVERSITY_USE_AES_MMO_HW
#define AESMMO_HW_MAX_NOF_INPUT_BLOCKS (0xFF / AES_BLOCK_SIZE_BYTES) //0xFF is the max msg len that can be passed to aesmmo hw
#endif

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef struct
{
    UInt32 AesMmoMsgLengthBytes;
    // intermediate location where hash value is stored
    UInt8 AesMmohash[AES_BLOCK_SIZE_BYTES];
} AesMmoContext;

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

static AesMmoContext gpEncryption_AesMmoCtx LINKER_SECTION(".lower_ram_retain");
// padded blocks
static UInt8 paddedBlock1[AES_BLOCK_SIZE_BYTES] LINKER_SECTION(".lower_ram_retain");
static UInt8 paddedBlock2[AES_BLOCK_SIZE_BYTES] LINKER_SECTION(".lower_ram_retain");
static Bool gpEncryption_AesMmoInUse;

/*****************************************************************************
 *                    Static functions
 *****************************************************************************/

/* Insert n-bit representation of msglen at location buf */
static void insertmsglen(UInt32 msgLenBits, UInt8* buf, UInt8 nbit)
{

    GP_ASSERT_SYSTEM(nbit == 16 || nbit == 32);
    if(nbit == 32)
    {
        *buf++ = (msgLenBits >> 24);
        *buf++ = (msgLenBits >> 16) & 0xFF;
    }

    *buf++ = (msgLenBits >> 8) & 0xFF;
    *buf++ = msgLenBits & 0xFF;
}

static gpEncryption_Result_t calcAesMmoHash(UInt32 nofblocks, UInt8 *msg, UInt8 *hash)
{
#ifndef GP_ENCRYPTION_DIVERSITY_USE_AES_MMO_HW
    UInt8 i;
    UInt32 blkidx;
    UInt8 encrypted_data[AES_BLOCK_SIZE_BYTES];
    UInt8 *pmsg;
    gpEncryption_Result_t result;
    gpEncryption_AESOptions_t aesOptions;
    aesOptions.keylen = gpEncryption_AESKeyLen128;
    aesOptions.options = gpEncryption_KeyIdKeyPtr;
    // take aes encryption of the msg blocks
    for(blkidx = 0; blkidx < nofblocks; blkidx++)
    {
        pmsg = &msg[blkidx * AES_BLOCK_SIZE_BYTES];
        /* Note: this can be further optimized if required by adding additional gpEncryption API
        that takes separate input and output buffer (instead of inplace buffer) */
        memcpy(encrypted_data, pmsg, AES_BLOCK_SIZE_BYTES);
        result = gpEncryption_AESEncrypt(encrypted_data, hash, aesOptions);
        GP_ASSERT_DEV_EXT(result == gpEncryption_ResultSuccess);

        for(i = 0; i < AES_BLOCK_SIZE_BYTES; i++)
        {
            hash[i] = encrypted_data[i] ^ pmsg[i];
        }
    }
#else

    UInt32 nofiter = nofblocks / AESMMO_HW_MAX_NOF_INPUT_BLOCKS;
    UInt32 remblks = nofblocks % AESMMO_HW_MAX_NOF_INPUT_BLOCKS;
    UInt32 iter;
    UInt32 pmsgCompressed;
    UInt32 pmsg, pmsgEnd;

    pmsg = (UInt32)msg;
    pmsgEnd = pmsg + nofblocks * AES_BLOCK_SIZE_BYTES;
    /* Find msg address in compressed address map, depending on if data lies in Flash or System RAM */
    if (pmsg >= GP_MM_FLASH_START && pmsgEnd < GP_MM_FLASH_ALT_START)
    {
        pmsgCompressed = GP_MM_FLASH_ADDR_TO_COMPRESSED(pmsg);
    }
    else if (pmsg >= GP_MM_FLASH_ALT_START && pmsgEnd < GP_MM_FLASH_ALT_END)
    {
        pmsgCompressed = GP_MM_FLASH_ALT_ADDR_TO_COMPRESSED(pmsg);
    }
    else if (pmsg >= GP_MM_RAM_START && pmsgEnd < GP_MM_RAM_END)
    {
        pmsgCompressed = GP_MM_RAM_ADDR_TO_COMPRESSED(pmsg);
    }
    else
    {
        /* SSP cannot handle data located in ROM or UCRAM */
        return gpEncryption_ResultInvalidParameter;
    }

    UInt32 maxAesMmoBytes = AESMMO_HW_MAX_NOF_INPUT_BLOCKS * AES_BLOCK_SIZE_BYTES;
    for (iter = 0; iter < nofiter; iter++)
    {
        gpHalSec_SspAesMMO(pmsgCompressed,
            GP_MM_RAM_ADDR_TO_COMPRESSED((UInt32)hash),
            gpEncryption_AESKeyLen128,
            maxAesMmoBytes);
        pmsgCompressed += maxAesMmoBytes;
    }

     gpHalSec_SspAesMMO(pmsgCompressed,
            GP_MM_RAM_ADDR_TO_COMPRESSED((UInt32)hash),
            gpEncryption_AESKeyLen128,
            remblks * AES_BLOCK_SIZE_BYTES);

#endif
     return gpEncryption_ResultSuccess;
}


static void fillPaddedBlocks(UInt8* msg, UInt32 msglenbytes, UInt32 totalMsgLengthBytes, UInt8 *isPaddedBlock2Present)
{
    UInt8 indexPaddedBlock1;
    UInt8 indexPaddedBlock2;

    const UInt8 firstPaddedByte = 0x80;
    const UInt8 insertLocation16bitMsgLength = (AES_BLOCK_SIZE_BYTES - 2);
    const UInt8 insertLocation32bitMsgLength = (AES_BLOCK_SIZE_BYTES - 6);

    /* index of block (block is AES_BLOCK_SIZE_BYTES in size) which is copied to paddedBlock1 */
    UInt32 lastblock;

    /* nofblocks of message on which AES will be done */
    UInt8 bytesInLastBlock; //if last block has 16 bytes, all of that should be copied to paddedBlock1
    UInt8 remainingSpacePaddedBlock1;
    UInt32 msgLenBits; // 8 bits in octet

    GP_ASSERT_DEV_EXT(msg != NULL);
    GP_ASSERT_DEV_EXT(isPaddedBlock2Present != NULL);
    GP_ASSERT_DEV_EXT(msglenbytes != 0 && totalMsgLengthBytes != 0);

    lastblock = (msglenbytes - 1) / AES_BLOCK_SIZE_BYTES;
    bytesInLastBlock = (msglenbytes - 1) % AES_BLOCK_SIZE_BYTES + 1;
    remainingSpacePaddedBlock1 = (AES_BLOCK_SIZE_BYTES - bytesInLastBlock);
    msgLenBits = totalMsgLengthBytes * 8;

    memset(paddedBlock1, 0, AES_BLOCK_SIZE_BYTES);
    memset(paddedBlock2, 0, AES_BLOCK_SIZE_BYTES);
    indexPaddedBlock1 = 0;
    indexPaddedBlock2 = 0;

    /* copy the last message block to paddedBlock1 */
    memcpy(paddedBlock1, &msg[lastblock * AES_BLOCK_SIZE_BYTES], bytesInLastBlock);
    indexPaddedBlock1 += bytesInLastBlock;

    /*    If the message M has length less than 2^block_size bits, pad this message according to the following */
    if(msgLenBits < (1 << AES_BLOCK_SIZE_BYTES))
    {
        /* if there is no space left in the first padded block to insert 1 followed by zeros */
        if(!remainingSpacePaddedBlock1)
        {
            //add 0x80 to second padded block
            paddedBlock2[indexPaddedBlock2] = firstPaddedByte;
            /* Form the padded message Mâ€™ by right-concatenating to the resulting string the n-bit string
             that is equal to the binary representation of the integer l */
            insertmsglen(msgLenBits, &paddedBlock2[insertLocation16bitMsgLength], 16);
            indexPaddedBlock2 = 15;
        }
        /* if there is space left in first padded block to insert 1 followed by zeros, but no space to insert
        n-bit (2 bytes) msglen */
        else if(remainingSpacePaddedBlock1 < 3)
        {
            paddedBlock1[indexPaddedBlock1] = firstPaddedByte;
            insertmsglen(msgLenBits, &paddedBlock2[insertLocation16bitMsgLength], 16);
            indexPaddedBlock2 = 15;
        }
        /* if there is space left in first padded block to insert 1 followed by zeros and n-bit msg len */
        else
        {
            paddedBlock1[indexPaddedBlock1] = firstPaddedByte;
            insertmsglen(msgLenBits, &paddedBlock1[insertLocation16bitMsgLength], 16);
        }
    }
    else
    {
        if(remainingSpacePaddedBlock1 < 1)
        {
            //add 0x80 to second last block
            paddedBlock2[indexPaddedBlock2] = firstPaddedByte;

            /* right-concatenating to the resulting string the 2n-bit string that is equal to the binary representation
             of the integer l and right-concatenating to the resulting string the n-bit all-zero bit string */
            insertmsglen(msgLenBits, &paddedBlock2[insertLocation32bitMsgLength], 32);
            paddedBlock2[14] = 0;
            paddedBlock2[15] = 0;
            indexPaddedBlock2 = 15;
        }
        else if(remainingSpacePaddedBlock1 < 7)
        {
            paddedBlock1[indexPaddedBlock1] = firstPaddedByte;
            insertmsglen(msgLenBits, &paddedBlock2[insertLocation32bitMsgLength], 32);
            paddedBlock2[14] = 0;
            paddedBlock2[15] = 0;
            indexPaddedBlock2 = 15;
        }
        else
        {
            paddedBlock1[indexPaddedBlock1] = firstPaddedByte;
            insertmsglen(msgLenBits, &paddedBlock1[insertLocation32bitMsgLength], 32);
            paddedBlock1[14] = 0;
            paddedBlock1[15] = 0;
        }
    }
    *isPaddedBlock2Present = (indexPaddedBlock2 > 0)? 1: 0;
}

/*****************************************************************************
 *                    Public functions
 *****************************************************************************/

void gpEncryptionAesMmo_Init(void)
{
    gpEncryption_AesMmoInUse = 0;
}

void gpEncryptionAesMmo_Start(void)
{
    GP_ASSERT_DEV_EXT(gpEncryption_AesMmoInUse == 0);
    gpEncryption_AesMmoInUse = 1;

    // clear aes mmo context
    memset(&gpEncryption_AesMmoCtx, 0, sizeof(AesMmoContext));
}

gpEncryption_Result_t gpEncryptionAesMmo_Update(UInt8* msg, UInt32 nofBlocks)
{
    gpEncryption_Result_t res;

    GP_ASSERT_DEV_EXT(gpEncryption_AesMmoInUse == 1);

    {
        UInt32 msglenbytes = nofBlocks * AES_BLOCK_SIZE_BYTES;
        AesMmoContext* pCtx;
        pCtx = &gpEncryption_AesMmoCtx;
        pCtx->AesMmoMsgLengthBytes += msglenbytes;

        res = calcAesMmoHash(nofBlocks, msg, pCtx->AesMmohash);
    }
    return res;
}

gpEncryption_Result_t gpEncryptionAesMmo_Finalize(UInt8* hash_out, UInt8* msg, UInt32 msglenbytes)
{
    gpEncryption_Result_t res;

    GP_ASSERT_DEV_EXT(gpEncryption_AesMmoInUse == 1);

    {
    UInt8 isPaddedBlock2Present = 0;
    AesMmoContext *pCtx;
    UInt32 nofblocks;

    pCtx = &gpEncryption_AesMmoCtx;
    pCtx->AesMmoMsgLengthBytes += msglenbytes;
    nofblocks = (msglenbytes - 1) / AES_BLOCK_SIZE_BYTES;

    fillPaddedBlocks(msg, msglenbytes, pCtx->AesMmoMsgLengthBytes, &isPaddedBlock2Present);
    res = calcAesMmoHash(nofblocks, msg, pCtx->AesMmohash);
    if (res != gpEncryption_ResultSuccess)
    {
        return res;
    }

    res = calcAesMmoHash(1, paddedBlock1, pCtx->AesMmohash);
    if (res != gpEncryption_ResultSuccess)
    {
        return res;
    }

    if(isPaddedBlock2Present)
    {
        res = calcAesMmoHash(1, paddedBlock2, pCtx->AesMmohash);
        if (res != gpEncryption_ResultSuccess)
        {
            return res;
        }
    }
    memcpy(hash_out, pCtx->AesMmohash, AES_BLOCK_SIZE_BYTES);
    }

    gpEncryption_AesMmoInUse = 0;
    return res;
}
