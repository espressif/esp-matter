/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_aes.h"
#include "string.h"

/*******************************************************************************
 * Definitions
 *******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.mw_aes"
#endif

#define AES_CTRL1_RESET_VALUE    (AES_CTRL1_OUT_MSG_MASK | AES_CTRL1_OUT_MIC_MASK)
#define AES_CTRL2_RESET_VALUE    (0U)
#define AES_ASTR_LEN_RESET_VALUE (0U)
#define AES_MSTR_LEN_RESET_VALUE (0U)
#define AES_IV0_RESET_VALUE      (0U)
#define AES_IV1_RESET_VALUE      (0U)
#define AES_IV2_RESET_VALUE      (0U)
#define AES_IV3_RESET_VALUE      (0U)
#define AES_KEY0_RESET_VALUE     (0U)
#define AES_KEY1_RESET_VALUE     (0U)
#define AES_KEY2_RESET_VALUE     (0U)
#define AES_KEY3_RESET_VALUE     (0U)
#define AES_KEY4_RESET_VALUE     (0U)
#define AES_KEY5_RESET_VALUE     (0U)
#define AES_KEY6_RESET_VALUE     (0U)
#define AES_KEY7_RESET_VALUE     (0U)
#define AES_IMR_RESET_VALUE      (AES_IMR_MASK_0_MASK | AES_IMR_MASK_1_MASK | AES_IMR_MASK_2_MASK)

/*!
 * @brief Structure to handle CCM output data array.
 *
 * In CCM calculation, the input data is written as 4-byte to AES registers,
 * and the data should be got from different arrays. The problem is the arrays
 * might be 4-byte unaligned. For easy operation, the arrays are collected into
 * a structure, to operate use a common function.
 */
typedef struct
{
    const uint8_t *data;
    size_t len;
} aes_input_data_array_t;

/*!
 * @brief Structure to handle CCM output data array.
 *
 * In CCM calculation, the output data is read as 4-byte from AES registers,
 * and the data should be saved to different arrays. The problem is the arrays
 * might be 4-byte unaligned. For easy operation, the arrays are collected into
 * a structure, to operate use a common function.
 */
typedef struct
{
    uint8_t *data;
    size_t len;
} aes_output_data_array_t;

/*!
 * @brief Union for bytes and word convertion.
 */
typedef union
{
    uint32_t word;
    uint8_t bytes[4];
} aes_word_byte_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief AES common operation.
 *
 * Only used for kAES_ModeEncryptECB, kAES_ModeDecryptECB, kAES_ModeEncryptCBC,
 * kAES_ModeDecryptCBC, kAES_ModeCTR
 *
 * @param base AES peripheral base address.
 * @param mode AES mode used in this operation.
 * @param input Input data.
 * @param output Output data.
 * @param len Length of input and output data in bytes.
 * @param inputVector Input vector.
 * @param outputVector Output vector.
 */
static void AES_Operation(AES_Type *base,
                          aes_mode_t mode,
                          const uint8_t *input,
                          uint8_t *output,
                          size_t len,
                          const uint8_t *inputVector,
                          uint8_t *outputVector);

/*!
 * @brief Read word from a group of arrays.
 *
 * This function reads word from a group of arrays. The arrays might not be
 * 4-byte aligned.
 *
 * @param arrays Pointer to the array group.
 * @param totalArrays Number of the arrays in the group.
 * @param curArrayIndex Reading from which array currently. This could be set to 0
 * before the first read, and should not be modified in later read.
 * @return The word read from arrays. If there is no more data from the array, returns 0.
 */
static uint32_t AES_ReadWordFromArray(aes_input_data_array_t arrays[], uint8_t totalArrays, uint8_t *curArrayIndex);

/*!
 * @brief Write word to a group of arrays.
 *
 * This function writes word to a group of arrays. The arrays might not be
 * 4-byte aligned.
 *
 * @param arrays Pointer to the array group.
 * @param totalArrays Number of the arrays in the group.
 * @param curArrayIndex Writing to which array currently. This could be set to 0
 * before the first read, and should not be modified in later write.
 * @param word The word to write. If no space in the arrays anymore, the word will be dropped.
 */
static void AES_WriteWordToArray(aes_output_data_array_t arrays[],
                                 uint8_t totalArrays,
                                 uint8_t *curArrayIndex,
                                 uint32_t word);

/*!
 * @brief Prepare the CCM operation.
 *
 * @param base AES peripheral base address.
 * @param mode AES mode used in this operation.
 * @param dataSize Length of plaintext or ciphertext in bytes.
 * @param iv Input vector.
 * @param ivSize Length of input vector in bytes.
 * @param aadSize Length of input additional authentication data in bytes.
 * @param tagSize Length of tag data in bytes.
 * @retval kStatus_Success Prepare successfully.
 * @retval kStatus_InvalidArgument Invalid parameters.
 */
static status_t AES_PrepareTagCcm(
    AES_Type *base, aes_mode_t mode, size_t dataSize, const uint8_t *iv, size_t ivSize, size_t aadSize, size_t tagSize);

/*******************************************************************************
 * Variables
 ******************************************************************************/
static aes_lock_func_t s_aesLockFunc     = NULL;
static aes_unlock_func_t s_aesUnlockFunc = NULL;

/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t AES_ReadWordFromArray(aes_input_data_array_t arrays[], uint8_t totalArrays, uint8_t *curArrayIndex)
{
    uint8_t byteCopyIndex;
    aes_input_data_array_t *pArray;
    aes_word_byte_t ret;

    pArray = &arrays[*curArrayIndex];

    if (pArray->len >= sizeof(uint32_t))
    {
        ret.word = *(uint32_t *)(pArray->data);
        pArray->len -= sizeof(uint32_t);
        pArray->data += sizeof(uint32_t);
    }
    else
    {
        ret.word      = 0;
        byteCopyIndex = 0;

        while (*curArrayIndex < totalArrays)
        {
            while ((byteCopyIndex < sizeof(uint32_t)) && (pArray->len > 0U))
            {
                ret.bytes[byteCopyIndex] = *(pArray->data);
                pArray->len--;
                pArray->data++;
                byteCopyIndex++;
            }

            if (byteCopyIndex == sizeof(uint32_t))
            {
                break;
            }
            else
            {
                (*curArrayIndex)++;
                pArray++;
                continue;
            }
        }
    }

    return ret.word;
}

static void AES_WriteWordToArray(aes_output_data_array_t arrays[],
                                 uint8_t totalArrays,
                                 uint8_t *curArrayIndex,
                                 uint32_t word)
{
    uint8_t byteCopyIndex;
    aes_output_data_array_t *pArray;
    aes_word_byte_t data;

    pArray = &arrays[*curArrayIndex];

    if (pArray->len >= sizeof(uint32_t))
    {
        /* If pArray->data is NULL, drop the data. */
        if (NULL != pArray->data)
        {
            *(uint32_t *)(pArray->data) = word;
            pArray->data += sizeof(uint32_t);
        }

        pArray->len -= sizeof(uint32_t);
    }
    else
    {
        data.word     = word;
        byteCopyIndex = 0;

        while (*curArrayIndex < totalArrays)
        {
            while ((byteCopyIndex < sizeof(uint32_t)) && (pArray->len > 0U))
            {
                if (NULL != pArray->data)
                {
                    *pArray->data = data.bytes[byteCopyIndex];
                    pArray->data++;
                }

                pArray->len--;
                byteCopyIndex++;
            }

            if (byteCopyIndex == sizeof(uint32_t))
            {
                break;
            }
            else
            {
                (*curArrayIndex)++;
                pArray++;
                continue;
            }
        }
    }
}

static void AES_Operation(AES_Type *base,
                          aes_mode_t mode,
                          const uint8_t *input,
                          uint8_t *output,
                          size_t len,
                          const uint8_t *inputVector,
                          uint8_t *outputVector)
{
    size_t inputSize  = len;
    size_t outputSize = len;

    AES_Reset(base);

    AES_SetMode(base, mode);

    AES_SetMsgLen(base, (uint32_t)len);

    if (inputVector != NULL)
    {
        AES_SetInputVector(base, inputVector);
    }

    AES_EnableOutMsg(base, true);

    AES_Start(base);

    while (outputSize > 0)
    {
        if (inputSize > 0)
        {
            /* Input FIFO not full, feed data. */
            if (0U == (AES_GetStatus(base) & kAES_InputFIFOFullFlag))
            {
                AES_WriteData(base, *(const uint32_t *)input);
                input += sizeof(uint32_t);
                inputSize -= sizeof(uint32_t);
            }
        }

        /* Output FIFO not empty, read data. */
        if (0U == (AES_GetStatus(base) & kAES_OutputFIFOEmptyFlag))
        {
            *(uint32_t *)output = AES_ReadData(base);
            output += sizeof(uint32_t);
            outputSize -= sizeof(uint32_t);
        }
    }

    if (outputVector != NULL)
    {
        AES_GetOutputVector(base, outputVector);
    }

    AES_Stop(base);
}

static status_t AES_PrepareTagCcm(
    AES_Type *base, aes_mode_t mode, size_t dataSize, const uint8_t *iv, size_t ivSize, size_t aadSize, size_t tagSize)
{
    uint8_t localIV[AES_VECTOR_SIZE] = {0};
    aes_miclen_t micLen;

    /* Verify parameter. */
    if ((ivSize < 7) || (ivSize > 13))
    {
        return kStatus_InvalidArgument;
    }

    if ((tagSize != 16U) && (tagSize != 8U) && (tagSize != 4U) && (tagSize != 0U))
    {
        return kStatus_InvalidArgument;
    }

    AES_Reset(base);

    /* Input vector configuration. */
    memcpy(localIV, iv, ivSize);

    localIV[13] = 15U - ivSize;

    AES_SetInputVector(base, localIV);

    /* AAD configuration. */
    AES_SetAadLen(base, aadSize);

    /* TAG configuration. */
    micLen = (tagSize == 16U) ?
                 kAES_MicBytes16 :
                 ((tagSize == 8U) ? kAES_MicBytes8 : ((tagSize == 4U) ? kAES_MicBytes4 : kAES_MicBytes0));

    AES_SetMode(base, mode);

    AES_SetMICLength(base, micLen);

    if (kAES_ModeDecryptCCM == mode)
    {
        AES_AppendMIC(base, false);
    }
    else
    {
        AES_AppendMIC(base, (kAES_MicBytes0 != micLen));
    }

    AES_SetMsgLen(base, dataSize);

    AES_EnableOutMsg(base, true);

    return kStatus_Success;
}

/*!
 * brief Initializes AES module.
 *
 * Initialize the module, reset the module and set the registers to default status.
 *
 * param base AES peripheral base address.
 */
void AES_Init(AES_Type *base)
{
    AES_Reset(base);

    base->CTRL1    = AES_CTRL1_RESET_VALUE;
    base->CTRL2    = AES_CTRL2_RESET_VALUE;
    base->ASTR_LEN = AES_ASTR_LEN_RESET_VALUE;
    base->MSTR_LEN = AES_MSTR_LEN_RESET_VALUE;
    base->IV0      = AES_IV0_RESET_VALUE;
    base->IV1      = AES_IV1_RESET_VALUE;
    base->IV2      = AES_IV2_RESET_VALUE;
    base->IV3      = AES_IV3_RESET_VALUE;
    base->KEY0     = AES_KEY0_RESET_VALUE;
    base->KEY1     = AES_KEY1_RESET_VALUE;
    base->KEY2     = AES_KEY2_RESET_VALUE;
    base->KEY3     = AES_KEY3_RESET_VALUE;
    base->KEY4     = AES_KEY4_RESET_VALUE;
    base->KEY5     = AES_KEY5_RESET_VALUE;
    base->KEY6     = AES_KEY6_RESET_VALUE;
    base->KEY7     = AES_KEY7_RESET_VALUE;
    base->IMR      = AES_IMR_RESET_VALUE;

    /* Clear interrupt status. */
    base->ICR = base->IRSR;
}

/*!
 * brief Deinitializes AES module.
 *
 * Deinitialize the module and reset the module.
 *
 * param base AES peripheral base address.
 */
void AES_Deinit(AES_Type *base)
{
    AES_Reset(base);
}

/*!
 * brief Reset AES module.
 *
 * Reset the module.
 *
 * param base AES peripheral base address.
 */
void AES_Reset(AES_Type *base)
{
    uint32_t cnt = 0x20;

    base->CTRL2 |= AES_CTRL2_AES_RESET_MASK;

    while (0U != cnt--)
    {
        __NOP();
    }

    base->CTRL2 &= ~AES_CTRL2_AES_RESET_MASK;
}

/*!
 * brief Set the AES input vector.

 * param base AES peripheral base address.
 * param inputVector Input vector.
 */
void AES_SetInputVector(AES_Type *base, const uint8_t inputVector[AES_VECTOR_SIZE])
{
    base->IV0 = ((uint32_t *)inputVector)[0];
    base->IV1 = ((uint32_t *)inputVector)[1];
    base->IV2 = ((uint32_t *)inputVector)[2];
    base->IV3 = ((uint32_t *)inputVector)[3];
}

/*!
 * brief Get the AES output vector.
 *
 * param base AES peripheral base address.
 * param outputVector Array to save the output vector.
 */
void AES_GetOutputVector(AES_Type *base, uint8_t outputVector[AES_VECTOR_SIZE])
{
    ((uint32_t *)outputVector)[0] = base->OV0;
    ((uint32_t *)outputVector)[1] = base->OV1;
    ((uint32_t *)outputVector)[2] = base->OV2;
    ((uint32_t *)outputVector)[3] = base->OV3;
}

/*!
 * brief Read AES CCM MIC output value.
 *
 * param base AES peripheral base address.
 * param micData The memory to save the MIC data.
 * param len Length of micData in word, the valid range is 1~4.
 * retval kStatus_InvalidArgument The required length is wrong.
 * retval kStatus_Success Get MIC successfully.
 */
status_t AES_ReadMIC(AES_Type *base, uint32_t *micData, uint8_t len)
{
    uint32_t cnt;

    /* check for len */
    if ((len == 0U) || (len > 4U))
    {
        return kStatus_InvalidArgument;
    }

    /* captured the mic data */
    for (cnt = 0u; cnt < len; cnt++)
    {
        micData[cnt] = (&base->OV0)[cnt];
    }

    return kStatus_Success;
}

/*!
 * brief Clear AES input or output FIFO.
 *
 * param base AES peripheral base address.
 * param fifo The FIFO to clear.
 */
void AES_ClearFIFO(AES_Type *base, aes_fifo_t fifo)
{
    uint32_t tmpCnt = 0x20;

    base->CTRL1 |= (uint32_t)fifo;

    while (0U != tmpCnt--)
    {
        __NOP();
    }

    base->CTRL1 &= ~(uint32_t)fifo;
}

/*!
 * brief Sets AES key.
 *
 * Sets AES key.
 *
 * param base AES peripheral base address
 * param key Input key to use for encryption or decryption
 * param keySize Size of the input key, in bytes. Must be 16, 24, or 32.
 * return Status from Set Key operation
 */
status_t AES_SetKey(AES_Type *base, const uint8_t *key, size_t keySize)
{
    status_t status = kStatus_Success;
    aes_keysize_t aesKeySize;
    uint32_t i;

    switch (keySize)
    {
        case 16:
            aesKeySize = kAES_KeyBytes16;
            break;

        case 24:
            aesKeySize = kAES_KeyBytes24;
            break;

        case 32:
            aesKeySize = kAES_KeyBytes32;
            break;

        default:
            status = kStatus_InvalidArgument;
    }

    if (kStatus_Success == status)
    {
        /* Set aesKeySize.  */
        base->CTRL1 = (base->CTRL1 & ~AES_CTRL1_KEY_SIZE_MASK) | AES_CTRL1_KEY_SIZE(aesKeySize);

        for (i = 0; i < keySize / 4; i++)
        {
            (&base->KEY0)[7 - i] = ((const uint32_t *)key)[i];
        }
        for (; i < 8; i++)
        {
            (&base->KEY0)[7 - i] = 0U;
        }
    }

    return status;
}

/*!
 * brief Encrypts AES using the ECB block mode.
 *
 * Encrypts AES using the ECB block mode.
 *
 * param base AES peripheral base address
 * param plaintext Input plain text to encrypt
 * param[out] ciphertext Output cipher text
 * param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * return Status from encrypt operation
 */
status_t AES_EncryptEcb(AES_Type *base, const uint8_t *plaintext, uint8_t *ciphertext, size_t size)
{
    status_t status;

    /* ECB mode, size must be 16-byte multiple */
    if ((size % 16U) != 0U)
    {
        status = kStatus_InvalidArgument;
    }
    else
    {
        AES_Operation(base, kAES_ModeEncryptECB, plaintext, ciphertext, size, NULL, NULL);

        status = kStatus_Success;
    }

    return status;
}

/*!
 * brief Decrypts AES using the ECB block mode.
 *
 * Decrypts AES using the ECB block mode.
 *
 * param base AES peripheral base address
 * param ciphertext Input ciphertext to decrypt
 * param[out] plaintext Output plain text
 * param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * return Status from decrypt operation
 */
status_t AES_DecryptEcb(AES_Type *base, const uint8_t *ciphertext, uint8_t *plaintext, size_t size)
{
    status_t status;

    /* ECB mode, size must be 16-byte multiple */
    if ((size % 16U) != 0U)
    {
        status = kStatus_InvalidArgument;
    }
    else
    {
        AES_Operation(base, kAES_ModeDecryptECB, ciphertext, plaintext, size, NULL, NULL);

        status = kStatus_Success;
    }

    return status;
}

/*!
 * brief Encrypts AES using CBC block mode.
 *
 * param base AES peripheral base address
 * param plaintext Input plain text to encrypt
 * param[out] ciphertext Output cipher text
 * param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * param iv Input initial vector to combine with the first input block.
 * param[out] ov Output vector used for chained operation, could be NULL.
 * return Status from encrypt operation
 */
status_t AES_EncryptCbc(AES_Type *base,
                        const uint8_t *plaintext,
                        uint8_t *ciphertext,
                        size_t size,
                        const uint8_t iv[AES_VECTOR_SIZE],
                        uint8_t ov[AES_VECTOR_SIZE])
{
    status_t status;

    /* ECB mode, size must be 16-byte multiple */
    if ((size % 16U) != 0U)
    {
        status = kStatus_InvalidArgument;
    }
    else
    {
        AES_Operation(base, kAES_ModeEncryptCBC, plaintext, ciphertext, size, iv, ov);

        status = kStatus_Success;
    }

    return status;
}

/*!
 * brief Decrypts AES using CBC block mode.
 *
 * param base AES peripheral base address
 * param ciphertext Input cipher text to decrypt
 * param[out] plaintext Output plain text
 * param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * param iv Input initial vector to combine with the first input block.
 * param[out] ov Output vector used for chained operation, could be NULL.
 * return Status from decrypt operation
 */
status_t AES_DecryptCbc(AES_Type *base,
                        const uint8_t *ciphertext,
                        uint8_t *plaintext,
                        size_t size,
                        const uint8_t iv[AES_VECTOR_SIZE],
                        uint8_t ov[AES_VECTOR_SIZE])
{
    status_t status;

    /* ECB mode, size must be 16-byte multiple */
    if ((size % 16U) != 0U)
    {
        status = kStatus_InvalidArgument;
    }
    else
    {
        AES_Operation(base, kAES_ModeDecryptCBC, ciphertext, plaintext, size, iv, ov);

        status = kStatus_Success;
    }

    return status;
}

/*!
 * brief Encrypts or decrypts AES using CTR block mode.
 *
 * Encrypts or decrypts AES using CTR block mode.
 * AES CTR mode uses only forward AES cipher and same algorithm for encryption and decryption.
 * The only difference between encryption and decryption is that, for encryption, the input argument
 * is plain text and the output argument is cipher text. For decryption, the input argument is cipher text
 * and the output argument is plain text.
 *
 * For chained calls, the internal status are saved in the parameters: nonce, streamBlock,
 * and offset. In this case, initialize nonce to the desired value, and initialize offset to 0
 * before the first call, in the following calls, the nonce, streamBlock, and offset should be
 * preserved.
 *
 * note The streamBlock contains sensitive data, discard it after used.
 *
 * param base AES peripheral base address
 * param input Input data for CTR block mode
 * param[out] output Output data for CTR block mode
 * param size Size of input and output data in bytes
 * param[in,out] nonce and counter(updates on return)
 * param[out] streamBlock Saved block for chained CTR calls. NULL can be passed if chained calls are
 * not used.
 * param[out] offset Offset in streamBlock. NULL can be passed if chained calls
 * are not used.
 * return Status from crypt operation
 */
status_t AES_CryptCtr(AES_Type *base,
                      const uint8_t *input,
                      uint8_t *output,
                      size_t size,
                      uint8_t nonce[AES_BLOCK_SIZE],
                      uint8_t streamBlock[AES_BLOCK_SIZE],
                      size_t *offset)
{
    size_t n2, n3;
    size_t localOffset;
    uint8_t lastBlockIn[AES_BLOCK_SIZE];
    uint8_t lastBlockOut[AES_BLOCK_SIZE];

    /*
     * The AES output is divided into three parts, the length of each parts are:
     * n1, n2, n3.
     *
     * n1: If previous calculation is not AES block size aligned, there are valid data in
     * streamBlock. Input data should first XOR'ed with the data in streamBlock.
     * n2: After n1, the AES block size aligned data in left data is n2. They are fed into
     * AES directly.
     * n3: After n2, the left AES block size unaligned part is n3. They are pad to AES
     * block size and fed into AES.
     */

    if ((NULL == streamBlock) || (NULL == offset))
    {
        localOffset = 0U;
    }
    else
    {
        localOffset = *offset;
    }

    /* Handle the n1 part. */
    while (0U != size)
    {
        if (localOffset == 0U)
        {
            break;
        }

        *output = *input ^ streamBlock[localOffset];
        output++;
        input++;
        size--;
        localOffset++;

        localOffset &= (AES_BLOCK_SIZE - 1U);
    }

    if (offset != NULL)
    {
        *offset = localOffset;
    }

    n2 = size & ~(AES_BLOCK_SIZE - 1U);
    n3 = size - n2;

    /* Handle the n2 part. */
    if (n2 > 0)
    {
        AES_Operation(base, kAES_ModeCTR, input, output, n2, nonce, nonce);

        input += n2;
        output += n2;
    }

    /* Handle the n3 part. */
    if (n3 > 0)
    {
        memset(lastBlockIn, 0, sizeof(lastBlockIn));
        memcpy(lastBlockIn, input, n3);

        AES_Operation(base, kAES_ModeCTR, lastBlockIn, lastBlockOut, AES_BLOCK_SIZE, nonce, nonce);

        memcpy(output, lastBlockOut, n3);

        if (NULL != streamBlock)
        {
            memcpy(&streamBlock[n3], &lastBlockOut[n3], AES_BLOCK_SIZE - n3);
        }

        if (NULL != offset)
        {
            *offset = n3;
        }
    }

    return kStatus_Success;
}

/*!
 * brief Encrypts AES and tags using CCM block mode.
 *
 * Encrypts AES and optionally tags using CCM block mode.
 *
 * param base AES peripheral base address
 * param plaintext Input plain text to encrypt
 * param[out] ciphertext Output cipher text.
 * param size Size of input and output data in bytes
 * param iv Input initial vector
 * param ivSize Size of the iv
 * param aad Input additional authentication data
 * param aadSize Input size in bytes of aad.
 * param[out] tag Output hash tag. Set to NULL to skip tag processing.
 * param tagSize Input size of the tag to generate, in bytes. Must be 0,4,8, or 16.
 * return Status from encrypt operation
 */
status_t AES_EncryptTagCcm(AES_Type *base,
                           const uint8_t *plaintext,
                           uint8_t *ciphertext,
                           size_t size,
                           const uint8_t *iv,
                           size_t ivSize,
                           const uint8_t *aad,
                           size_t aadSize,
                           uint8_t *tag,
                           size_t tagSize)
{
    status_t status;
    int32_t inputSize, outputSize;
    uint8_t curOutputArrayIndex;
    uint8_t curInputArrayIndex;
    uint32_t inputWord;
    uint32_t outputWord;

    /* Input: AAD + Plain. */
    aes_input_data_array_t inputDataArrays[2];
    /* Output: AAD + Cipher + MIC. */
    aes_output_data_array_t outputDataArrays[3];

    if (NULL == tag)
    {
        tagSize = 0U;
    }

    status = AES_PrepareTagCcm(base, kAES_ModeEncryptCCM, size, iv, ivSize, aadSize, tagSize);

    if (kStatus_Success != status)
    {
        return status;
    }

    /* Input: AAD + plain. */
    inputDataArrays[0].data = aad;
    inputDataArrays[0].len  = aadSize;
    inputDataArrays[1].data = plaintext;
    inputDataArrays[1].len  = size;
    inputSize               = aadSize + size;

    /* Output: AAD + Chiper + MIC. */
    outputDataArrays[0].data = NULL;
    outputDataArrays[0].len  = aadSize;
    outputDataArrays[1].data = ciphertext;
    outputDataArrays[1].len  = size;
    outputDataArrays[2].data = tag;
    outputDataArrays[2].len  = tagSize;
    outputSize               = aadSize + size + tagSize;

    AES_Start(base);

    curOutputArrayIndex = 0;
    curInputArrayIndex  = 0;

    inputWord = AES_ReadWordFromArray(inputDataArrays, ARRAY_SIZE(inputDataArrays), &curInputArrayIndex);

    while (outputSize > 0)
    {
        if (inputSize > 0)
        {
            /* Input FIFO not full, feed data. */
            if (0U == (AES_GetStatus(base) & kAES_InputFIFOFullFlag))
            {
                AES_WriteData(base, inputWord);
                inputWord = AES_ReadWordFromArray(inputDataArrays, ARRAY_SIZE(inputDataArrays), &curInputArrayIndex);
                inputSize -= sizeof(uint32_t);
            }
        }

        /* Output FIFO not empty, read data. */
        if (0U == (AES_GetStatus(base) & kAES_OutputFIFOEmptyFlag))
        {
            outputWord = AES_ReadData(base);
            AES_WriteWordToArray(outputDataArrays, ARRAY_SIZE(outputDataArrays), &curOutputArrayIndex, outputWord);
            outputSize -= sizeof(uint32_t);
        }
    }

    AES_Stop(base);

    return kStatus_Success;
}

/*!
 * brief Decrypts AES and authenticates using CCM block mode.
 *
 * Decrypts AES and optionally authenticates using CCM block mode.
 *
 * param base AES peripheral base address
 * param ciphertext Input cipher text to decrypt
 * param[out] plaintext Output plain text.
 * param size Size of input and output data in bytes
 * param iv Input initial vector
 * param ivSize Size of the iv
 * param aad Input additional authentication data
 * param aadSize Input size in bytes of aad
 * param tag Input hash tag to compare. Set to NULL to skip tag processing.
 * param tagSize Input size of the tag to generate, in bytes. Must be 0,4,8, or 16.
 * return Status from decrypt operation
 */
status_t AES_DecryptTagCcm(AES_Type *base,
                           const uint8_t *ciphertext,
                           uint8_t *plaintext,
                           size_t size,
                           const uint8_t *iv,
                           size_t ivSize,
                           const uint8_t *aad,
                           size_t aadSize,
                           const uint8_t *tag,
                           size_t tagSize)
{
    status_t status;
    int32_t inputSize, outputSize;
    uint8_t curOutputArrayIndex;
    uint8_t curInputArrayIndex;
    uint32_t inputWord;
    uint32_t outputWord;

    /* Input: AAD + Cipher  + MIC. */
    aes_input_data_array_t inputDataArrays[3];
    /* Output: AAD + Plain. */
    aes_output_data_array_t outputDataArrays[2];

    if (NULL == tag)
    {
        tagSize = 0U;
    }

    status = AES_PrepareTagCcm(base, kAES_ModeDecryptCCM, size, iv, ivSize, aadSize, tagSize);

    if (kStatus_Success != status)
    {
        return status;
    }

    /* Input: AAD + Chiper + MIC. */
    inputDataArrays[0].data = aad;
    inputDataArrays[0].len  = aadSize;
    inputDataArrays[1].data = ciphertext;
    inputDataArrays[1].len  = size;
    inputDataArrays[2].data = tag;
    inputDataArrays[2].len  = tagSize;
    inputSize               = aadSize + size + tagSize;

    /* Output: AAD + Plain. */
    outputDataArrays[0].data = NULL;
    outputDataArrays[0].len  = aadSize;
    outputDataArrays[1].data = plaintext;
    outputDataArrays[1].len  = (plaintext == NULL) ? 0 : size;
    outputSize               = aadSize + size;

    AES_Start(base);

    curOutputArrayIndex = 0;
    curInputArrayIndex  = 0;

    inputWord = AES_ReadWordFromArray(inputDataArrays, ARRAY_SIZE(inputDataArrays), &curInputArrayIndex);

    while (outputSize > 0)
    {
        if (0U != (AES_GetStatus(base) & kAES_Error2Flag))
        {
            status = kStatus_Fail;
            break;
        }

        if (inputSize > 0)
        {
            /* Input FIFO not full, feed data. */
            if (0U == (AES_GetStatus(base) & kAES_InputFIFOFullFlag))
            {
                AES_WriteData(base, inputWord);
                inputWord = AES_ReadWordFromArray(inputDataArrays, ARRAY_SIZE(inputDataArrays), &curInputArrayIndex);
                inputSize -= sizeof(uint32_t);
            }
        }

        /* Output FIFO not empty, read data. */
        if (0U == (AES_GetStatus(base) & kAES_OutputFIFOEmptyFlag))
        {
            outputWord = AES_ReadData(base);
            AES_WriteWordToArray(outputDataArrays, ARRAY_SIZE(outputDataArrays), &curOutputArrayIndex, outputWord);
            outputSize -= sizeof(uint32_t);
        }
    }

    AES_Stop(base);

    return status;
}

/*!
 * brief Calculate the input stream MMO HASH value.
 *
 * param base AES peripheral base address
 * param input Input stream.
 * param size Length of the stream in bytes.
 * param[out] hash Hash value of the input stream.
 * return Status from HASH operation
 */
status_t AES_CalculateMMOHash(AES_Type *base, const uint8_t *input, size_t size, uint8_t hash[16])
{
    assert(NULL != input);
    aes_word_byte_t dataToWrite;

    AES_Reset(base);

    AES_SetMode(base, kAES_ModeMMO);

    AES_SetKey(base, hash, 16);

    AES_SetMsgLen(base, size);

    AES_EnableOutMsg(base, false);

    AES_AppendMIC(base, true);

    AES_SetInputVector(base, hash);

    AES_Start(base);

    /* Handle the 4-byte aligned part. */
    while (size >= sizeof(uint32_t))
    {
        dataToWrite.word = *(const uint32_t *)input;
        input += sizeof(uint32_t);
        size -= sizeof(uint32_t);

        /* Wait for input FIFO not full. */
        while (0U != (AES_GetStatus(base) & kAES_InputFIFOFullFlag))
        {
        }

        AES_WriteData(base, dataToWrite.word);
    }

    /* Handle the 4-byte unaligned part. */
    if (size > 0U)
    {
        dataToWrite.word = 0U;

        for (uint8_t i = 0; i < size; i++)
        {
            dataToWrite.bytes[i] = *input;
            input++;
        }

        /* Wait for input FIFO not full. */
        while (0U != (AES_GetStatus(base) & kAES_InputFIFOFullFlag))
        {
        }

        AES_WriteData(base, dataToWrite.word);
    }

    /* Wait for proess complete. */
    while (0U == (AES_GetStatus(base) & kAES_DoneFlag))
    {
    }

    AES_Stop(base);

    AES_GetOutputVector(base, hash);

    return kStatus_Success;
}

/*!
 * brief Set the lock functions.
 *
 * param lockFunc Lock function.
 * param unlockFunc Unlock function.
 */
void AES_SetLockFunc(aes_lock_func_t lockFunc, aes_unlock_func_t unlockFunc)
{
    s_aesLockFunc   = lockFunc;
    s_aesUnlockFunc = unlockFunc;
}

/*
 * brief Encrypts AES using the ECB block mode.
 *
 * Encrypts AES using the ECB block mode.
 *
 * param base AES peripheral base address
 * param key Input key to use for encryption or decryption
 * param keySize Size of the input key, in bytes.
 * param plaintext Input plain text to encrypt
 * param[out] ciphertext Output cipher text
 * param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * return Status from encrypt operation
 */
status_t AES_EncryptEcbWithLock(
    AES_Type *base, const uint8_t *key, size_t keySize, const uint8_t *plaintext, uint8_t *ciphertext, size_t size)
{
    status_t status = kStatus_Success;

    if (s_aesLockFunc)
    {
        status = s_aesLockFunc();
    }

    if (kStatus_Success == status)
    {
        status = AES_SetKey(base, key, keySize);

        if (kStatus_Success == status)
        {
            status = AES_EncryptEcb(base, plaintext, ciphertext, size);
        }

        if (s_aesUnlockFunc)
        {
            s_aesUnlockFunc();
        }
    }

    return status;
}

/*!
 * brief Decrypts AES using the ECB block mode.
 *
 * Decrypts AES using the ECB block mode.
 *
 * param base AES peripheral base address
 * param key Input key to use for encryption or decryption
 * param keySize Size of the input key, in bytes.
 * param ciphertext Input ciphertext to decrypt
 * param[out] plaintext Output plain text
 * param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * return Status from decrypt operation
 */
status_t AES_DecryptEcbWithLock(
    AES_Type *base, const uint8_t *key, size_t keySize, const uint8_t *ciphertext, uint8_t *plaintext, size_t size)
{
    status_t status = kStatus_Success;

    if (s_aesLockFunc)
    {
        status = s_aesLockFunc();
    }

    if (kStatus_Success == status)
    {
        status = AES_SetKey(base, key, keySize);

        if (kStatus_Success == status)
        {
            status = AES_DecryptEcb(base, ciphertext, plaintext, size);
        }

        if (s_aesUnlockFunc)
        {
            s_aesUnlockFunc();
        }
    }

    return status;
}

/*
 * brief Encrypts AES using CBC block mode.
 *
 * param base AES peripheral base address
 * param key Input key to use for encryption or decryption
 * param keySize Size of the input key, in bytes.
 * param plaintext Input plain text to encrypt
 * param[out] ciphertext Output cipher text
 * param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * param iv Input initial vector to combine with the first input block.
 * param[out] ov Output vector used for chained operation, could be NULL.
 * return Status from encrypt operation
 */
status_t AES_EncryptCbcWithLock(AES_Type *base,
                                const uint8_t *key,
                                size_t keySize,
                                const uint8_t *plaintext,
                                uint8_t *ciphertext,
                                size_t size,
                                const uint8_t iv[AES_VECTOR_SIZE],
                                uint8_t ov[AES_VECTOR_SIZE])
{
    status_t status = kStatus_Success;

    if (s_aesLockFunc)
    {
        status = s_aesLockFunc();
    }

    if (kStatus_Success == status)
    {
        status = AES_SetKey(base, key, keySize);

        if (kStatus_Success == status)
        {
            status = AES_EncryptCbc(base, plaintext, ciphertext, size, iv, ov);
        }

        if (s_aesUnlockFunc)
        {
            s_aesUnlockFunc();
        }
    }

    return status;
}

/*
 * brief Decrypts AES using CBC block mode.
 *
 * param base AES peripheral base address
 * param key Input key to use for encryption or decryption
 * param keySize Size of the input key, in bytes.
 * param ciphertext Input cipher text to decrypt
 * param[out] plaintext Output plain text
 * param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * param iv Input initial vector to combine with the first input block.
 * param ov[out] Output vector used for chained operation, could be NULL.
 * return Status from decrypt operation
 */
status_t AES_DecryptCbcWithLock(AES_Type *base,
                                const uint8_t *key,
                                size_t keySize,
                                const uint8_t *ciphertext,
                                uint8_t *plaintext,
                                size_t size,
                                const uint8_t iv[AES_VECTOR_SIZE],
                                uint8_t ov[AES_VECTOR_SIZE])
{
    status_t status = kStatus_Success;

    if (s_aesLockFunc)
    {
        status = s_aesLockFunc();
    }

    if (kStatus_Success == status)
    {
        status = AES_SetKey(base, key, keySize);

        if (kStatus_Success == status)
        {
            status = AES_DecryptCbc(base, ciphertext, plaintext, size, iv, ov);
        }

        if (s_aesUnlockFunc)
        {
            s_aesUnlockFunc();
        }
    }

    return status;
}

/*!
 * brief Encrypts or decrypts AES using CTR block mode.
 *
 * Encrypts or decrypts AES using CTR block mode.
 * AES CTR mode uses only forward AES cipher and same algorithm for encryption and decryption.
 * The only difference between encryption and decryption is that, for encryption, the input argument
 * is plain text and the output argument is cipher text. For decryption, the input argument is cipher text
 * and the output argument is plain text.
 *
 * For chained calls, the internal status are saved in the parameters:  nonce,  streamBlock,
 * and  offset. In this case, initialize  nonce to the desired value, and initialize  offset to 0
 * before the first call, in the following calls, the  nonce,  streamBlock, and  offset should be
 * preserved.
 *
 * @note The  streamBlock contains sensitive data, discard it after used.
 *
 * param base AES peripheral base address
 * param key Input key to use for encryption or decryption
 * param keySize Size of the input key, in bytes.
 * param input Input data for CTR block mode
 * param[out] output Output data for CTR block mode
 * param size Size of input and output data in bytes
 * param[in,out] nonce and counter(updates on return)
 * param[out] streamBlock Saved block for chained CTR calls. NULL can be passed if chained calls are
 * not used.
 * param[out] offset Offset in  streamBlock. NULL can be passed if chained calls
 * are not used.
 * return Status from crypt operation
 */
status_t AES_CryptCtrWithLock(AES_Type *base,
                              const uint8_t *key,
                              size_t keySize,
                              const uint8_t *input,
                              uint8_t *output,
                              size_t size,
                              uint8_t nonce[AES_BLOCK_SIZE],
                              uint8_t streamBlock[AES_BLOCK_SIZE],
                              size_t *offset)
{
    status_t status = kStatus_Success;

    if (s_aesLockFunc)
    {
        status = s_aesLockFunc();
    }

    if (kStatus_Success == status)
    {
        status = AES_SetKey(base, key, keySize);

        if (kStatus_Success == status)
        {
            status = AES_CryptCtr(base, input, output, size, nonce, streamBlock, offset);
        }

        if (s_aesUnlockFunc)
        {
            s_aesUnlockFunc();
        }
    }

    return status;
}

/*!
 * brief Encrypts AES and tags using CCM block mode.
 *
 * Encrypts AES and optionally tags using CCM block mode.
 *
 * param base AES peripheral base address
 * param key Input key to use for encryption or decryption
 * param keySize Size of the input key, in bytes.
 * param plaintext Input plain text to encrypt
 * param[out] ciphertext Output cipher text.
 * param size Size of input and output data in bytes
 * param iv Input initial vector
 * param ivSize Size of the iv
 * param aad Input additional authentication data
 * param aadSize Input size in bytes of aad.
 * param[out] tag Output hash tag. Set to NULL to skip tag processing.
 * param tagSize Input size of the tag to generate, in bytes. Must be 0,4,8, or 16.
 * return Status from encrypt operation
 */
status_t AES_EncryptTagCcmWithLock(AES_Type *base,
                                   const uint8_t *key,
                                   size_t keySize,
                                   const uint8_t *plaintext,
                                   uint8_t *ciphertext,
                                   size_t size,
                                   const uint8_t *iv,
                                   size_t ivSize,
                                   const uint8_t *aad,
                                   size_t aadSize,
                                   uint8_t *tag,
                                   size_t tagSize)
{
    status_t status = kStatus_Success;

    if (s_aesLockFunc)
    {
        status = s_aesLockFunc();
    }

    if (kStatus_Success == status)
    {
        status = AES_SetKey(base, key, keySize);

        if (kStatus_Success == status)
        {
            status = AES_EncryptTagCcm(base, plaintext, ciphertext, size, iv, ivSize, aad, aadSize, tag, tagSize);
        }

        if (s_aesUnlockFunc)
        {
            s_aesUnlockFunc();
        }
    }

    return status;
}

/*!
 * brief Decrypts AES and authenticates using CCM block mode.
 *
 * Decrypts AES and optionally authenticates using CCM block mode.
 *
 * param base AES peripheral base address
 * param key Input key to use for encryption or decryption
 * param keySize Size of the input key, in bytes.
 * param ciphertext Input cipher text to decrypt
 * param[out] plaintext Output plain text.
 * param size Size of input and output data in bytes
 * param iv Input initial vector
 * param ivSize Size of the iv
 * param aad Input additional authentication data
 * param aadSize Input size in bytes of aad
 * param tag Input hash tag to compare. Set to NULL to skip tag processing.
 * param tagSize Input size of the tag to generate, in bytes. Must be 0,4,8, or 16.
 * return Status from decrypt operation
 */
status_t AES_DecryptTagCcmWithLock(AES_Type *base,
                                   const uint8_t *key,
                                   size_t keySize,
                                   const uint8_t *ciphertext,
                                   uint8_t *plaintext,
                                   size_t size,
                                   const uint8_t *iv,
                                   size_t ivSize,
                                   const uint8_t *aad,
                                   size_t aadSize,
                                   const uint8_t *tag,
                                   size_t tagSize)
{
    status_t status = kStatus_Success;

    if (s_aesLockFunc)
    {
        status = s_aesLockFunc();
    }

    if (kStatus_Success == status)
    {
        status = AES_SetKey(base, key, keySize);

        if (kStatus_Success == status)
        {
            status = AES_DecryptTagCcm(base, ciphertext, plaintext, size, iv, ivSize, aad, aadSize, tag, tagSize);
        }

        if (s_aesUnlockFunc)
        {
            s_aesUnlockFunc();
        }
    }

    return status;
}

/*!
 * brief Calculate the input stream MMO HASH value.
 *
 * param base AES peripheral base address
 * param input Input stream.
 * param size Length of the stream in bytes.
 * param[out] hash Hash value of the input stream.
 * return Status from HASH operation
 */
status_t AES_CalculateMMOHashWithLock(AES_Type *base, const uint8_t *input, size_t size, uint8_t hash[16])
{
    status_t status = kStatus_Success;

    if (s_aesLockFunc)
    {
        status = s_aesLockFunc();
    }

    if (kStatus_Success == status)
    {
        status = AES_CalculateMMOHash(base, input, size, hash);

        if (s_aesUnlockFunc)
        {
            s_aesUnlockFunc();
        }
    }

    return status;
}
