/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_AES_H_
#define _FSL_AES_H_

#include "fsl_common.h"

/*!
 * @addtogroup aes
 * @{
 */

/*******************************************************************************
 * Definitions
 *******************************************************************************/

/*! @brief Defines AES driver version.  */
#define FSL_AES_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))

/*! AES block size in bytes */
#define AES_BLOCK_SIZE 16U

/*! AES Vector size in bytes */
#define AES_VECTOR_SIZE 16U

/*!
 * @brief AES work mode definition.
 */
typedef enum
{
    kAES_ModeEncryptECB = AES_CTRL1_DECRYPT(0) | AES_CTRL1_MODE(0), /*!< ECB encrypt */
    kAES_ModeDecryptECB = AES_CTRL1_DECRYPT(1) | AES_CTRL1_MODE(0), /*!< ECB decrypt */
    kAES_ModeEncryptCBC = AES_CTRL1_DECRYPT(0) | AES_CTRL1_MODE(1), /*!< CBC encrypt */
    kAES_ModeDecryptCBC = AES_CTRL1_DECRYPT(1) | AES_CTRL1_MODE(1), /*!< CBC decrypt */
    kAES_ModeCTR        = AES_CTRL1_DECRYPT(0) | AES_CTRL1_MODE(2), /*!< CTR */
    kAES_ModeEncryptCCM = AES_CTRL1_DECRYPT(0) | AES_CTRL1_MODE(5), /*!< CCM encrypt */
    kAES_ModeDecryptCCM = AES_CTRL1_DECRYPT(1) | AES_CTRL1_MODE(5), /*!< CCM decrypt */
    kAES_ModeMMO        = AES_CTRL1_DECRYPT(0) | AES_CTRL1_MODE(6), /*!< AES mode: MMO */
    kAES_ModeBYPASS     = AES_CTRL1_DECRYPT(0) | AES_CTRL1_MODE(7), /*!< AES mode: Bypass */
} aes_mode_t;

/*!
 * @brief AES FIFO.
 */
typedef enum
{
    kAES_InputFIFO  = AES_CTRL1_IF_CLR_MASK,                         /*!< Input FIFO. */
    kAES_OutputFIFO = AES_CTRL1_OF_CLR_MASK,                         /*!< Output FIFO. */
    kAES_AllFIFO    = AES_CTRL1_OF_CLR_MASK | AES_CTRL1_IF_CLR_MASK, /*!< Input and output FIFO. */
} aes_fifo_t;

/*!
 * @brief AES key size type definition
 */
typedef enum
{
    kAES_KeyBytes16 = 0, /*!< AES key size select: 16 bytes */
    kAES_KeyBytes32 = 1, /*!< AES key size select: 32 bytes */
    kAES_KeyBytes24 = 2, /*!< AES key size select: 24 bytes */
} aes_keysize_t;

/*!
 * @brief AES MIC size type definition
 */
typedef enum
{
    kAES_MicBytes0  = 0, /*!< AES MIC length: 0 bytes */
    kAES_MicBytes4  = 1, /*!< AES MIC length: 4 bytes */
    kAES_MicBytes8  = 2, /*!< AES MIC length: 8 bytes */
    kAES_MicBytes16 = 3, /*!< AES MIC length: 16 bytes */
} aes_miclen_t;

/*!
 * @brief AES interrupt type definition
 * @anchor aes_interrupts
 */
typedef enum
{
    kAES_InterruptOutputFIFOEmpty, /*!< AES output FIFO empty int flag */
    kAES_InterruptInputFIFOFull,   /*!< AES input FIFO full int flag */
    kAES_InterruptDone,            /*!< AES done int flag */
} aes_interrupt_t;

/*!
 * @brief AES status type definition
 * @anchor aes_status_flags
 */
enum
{
    kAES_OutputFIFOEmptyFlag = AES_STATUS_OF_EMPTY_MASK, /*!< AES output FIFO empty flag */
    kAES_InputFIFOFullFlag   = AES_STATUS_IF_FULL_MASK,  /*!< AES input FIFO full flag */
    kAES_OutputFIFOReadyFlag = AES_STATUS_OF_RDY_MASK,   /*!< AES output FIFO can be read flag */
    kAES_DoneFlag            = AES_STATUS_DONE_MASK,     /*!< AES done flag */
    kAES_Error0Flag          = AES_STATUS_STATUS(1U),    /*!< Input stream size is less than 16 bytes in CBC&CTR mode */
    kAES_Error1Flag          = AES_STATUS_STATUS(2U),    /*!< Data is more than 2^13-1 bytes in MMO mode
                     Data is not multiple of 16 bytes in ECB mode*/
    kAES_Error2Flag = AES_STATUS_STATUS(4U),             /*!< MIC mismatch during decryption in CCM* mode */
};

/*! @brief AES lock function. */
typedef status_t (*aes_lock_func_t)(void);

/*! @brief AES lock function. */
typedef void (*aes_unlock_func_t)(void);

/*******************************************************************************
 * API
 *******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @name Initialization Interfaces
 * @{
 */

/*!
 * @brief Initializes AES module.
 *
 * Initialize the module, reset the module and set the registers to default status.
 *
 * @param base AES peripheral base address.
 */
void AES_Init(AES_Type *base);

/*!
 * @brief Deinitializes AES module.
 *
 * Deinitialize the module and reset the module.
 *
 * @param base AES peripheral base address.
 */
void AES_Deinit(AES_Type *base);

/*!
 * @brief Reset AES module.
 *
 * Reset the module. This function only resets the AES module internal status,
 * but does not reset the register configurations.
 *
 * @param base AES peripheral base address.
 */
void AES_Reset(AES_Type *base);

/* @} */

/*!
 * @name Module configuration
 * @{
 */

/*!
 * @brief Set AES module work mode.
 *
 * @param base AES peripheral base address.
 * @param mode AES work mode.
 */
static inline void AES_SetMode(AES_Type *base, aes_mode_t mode)
{
    base->CTRL1 = (base->CTRL1 & ~(AES_CTRL1_MODE_MASK | AES_CTRL1_DECRYPT_MASK)) | (uint32_t)mode;
}

/*!
 * @brief Set the AES input vector.
 *
 * @param base AES peripheral base address.
 * @param inputVector Input vector.
 */
void AES_SetInputVector(AES_Type *base, const uint8_t inputVector[AES_VECTOR_SIZE]);

/*!
 * @brief Get the AES output vector.
 *
 * @param base AES peripheral base address.
 * @param outputVector Array to save the output vector.
 */
void AES_GetOutputVector(AES_Type *base, uint8_t outputVector[AES_VECTOR_SIZE]);

/*!
 * @brief Set the input data length.
 *
 * @param base AES peripheral base address.
 * @param msgLen Length of the input data in byte.
 */
static inline void AES_SetMsgLen(AES_Type *base, uint32_t msgLen)
{
    base->MSTR_LEN = msgLen;
}

/*!
 * @brief Set the additional authentication data length in CCM mode.
 *
 * @param base AES peripheral base address.
 * @param len Length in byte.
 */
static inline void AES_SetAadLen(AES_Type *base, uint32_t len)
{
    base->ASTR_LEN = len;
}

/*!
 * @brief Set the CTR mode counter modular.
 *
 * @param base AES peripheral base address.
 * @param modular The modular set to AES register. Valid range is 0~128.
 * When pass in 0~15, the CTR counter modular is 2^128, when pass in other
 * values, the CTR counter modular is 2^input value.
 */
static inline void AES_SetCTRCountModular(AES_Type *base, uint8_t modular)
{
    base->CTRL1 = (base->CTRL1 & ~AES_CTRL1_CTR_MOD_MASK) | AES_CTRL1_CTR_MOD(modular);
}

/*!
 * @brief Enable or disable the output stream to output FIFO.
 *
 * @param base AES peripheral base address.
 * @param enable Use \b true to forward output stream to output FIFO, use \b false
 * to block output stream from output FIFO.
 */
static inline void AES_EnableOutMsg(AES_Type *base, bool enable)
{
    if (enable)
    {
        base->CTRL1 |= AES_CTRL1_OUT_MSG_MASK;
    }
    else
    {
        base->CTRL1 &= ~AES_CTRL1_OUT_MSG_MASK;
    }
}

/*!
 * @brief Enable or disable DMA.
 *
 * @param base AES peripheral base address.
 * @param enable Use \b true to enable DMA, use \b false to disable DMA.
 */
static inline void AES_EnableDma(AES_Type *base, bool enable)
{
    if (enable)
    {
        base->CTRL1 |= AES_CTRL1_DMA_EN_MASK;
    }
    else
    {
        base->CTRL1 &= ~AES_CTRL1_DMA_EN_MASK;
    }
}

/*!
 * @brief Append MIC/HASH output or not.
 *
 * When enabled in CCM mode decryption, the MIC is append at the end of output stream.
 * When enabled in MMO mode, the HASH is append at the end of output stream.
 *
 * @param base AES peripheral base address.
 * @param append Use \b true to append, otherwise use \b false.
 */
static inline void AES_AppendMIC(AES_Type *base, bool append)
{
    if (append)
    {
        base->CTRL1 |= AES_CTRL1_OUT_MIC_MASK;
    }
    else
    {
        base->CTRL1 &= ~AES_CTRL1_OUT_MIC_MASK;
    }
}

/*!
 * @brief Set the MIC length in CCM mode.
 *
 * @param base AES peripheral base address.
 * @param len MIC length.
 */
static inline void AES_SetMICLength(AES_Type *base, aes_miclen_t len)
{
    base->CTRL1 = (base->CTRL1 & ~AES_CTRL1_MIC_LEN_MASK) | AES_CTRL1_MIC_LEN(len);
}

/*!
 * @brief Read AES CCM MIC output value.
 *
 * @param base AES peripheral base address.
 * @param micData The memory to save the MIC data.
 * @param len Length of \p micData in word, the valid range is 1~4.
 * @retval kStatus_InvalidArgument The required length is wrong.
 * @retval kStatus_Success Get MIC successfully.
 */
status_t AES_ReadMIC(AES_Type *base, uint32_t *micData, uint8_t len);

/*!
 * @brief Clear AES input or output FIFO.
 *
 * @param base AES peripheral base address.
 * @param fifo The FIFO to clear.
 */
void AES_ClearFIFO(AES_Type *base, aes_fifo_t fifo);

/*!
 * @brief Start the AES calculation.
 *
 * Start the calculation based on configurations.
 *
 * @param base AES peripheral base address.
 */
static inline void AES_Start(AES_Type *base)
{
    base->CTRL1 |= AES_CTRL1_START_MASK;
}

/*!
 * @brief Stop the AES calculation.
 *
 * Stop the calculation.
 *
 * @param base AES peripheral base address.
 */
static inline void AES_Stop(AES_Type *base)
{
    base->CTRL1 &= ~AES_CTRL1_START_MASK;
}

/*!
 * @brief Write data to AES input FIFO.
 *
 * @param base AES peripheral base address.
 * @param data Data written to AES FIFO.
 */
static inline void AES_WriteData(AES_Type *base, uint32_t data)
{
    base->STR_IN = data;
}

/*!
 * @brief Read data from AES output FIFO.
 *
 * @param base AES peripheral base address.
 */
static inline uint32_t AES_ReadData(AES_Type *base)
{
    return base->STR_OUT;
}

/* @} */

/*!
 * @name Interrupt and status
 * @{
 */

/*!
 * @brief Get the AES status.
 *
 * @param base AES peripheral base address.
 * @return AES status, logical OR'ed value of @ref aes_status_flags.
 */
static inline uint32_t AES_GetStatus(AES_Type *base)
{
    return base->STATUS;
}

/*!
 * @brief Enable AES interrupts.
 *
 * @param base AES peripheral base address.
 * @param interrupts The interrupts to enable, logical OR'ed value of @ref aes_interrupts.
 */
static inline void AES_EnableInterrupts(AES_Type *base, uint32_t interrupts)
{
    base->IMR &= ~interrupts;
}

/*!
 * @brief Disable AES interrupts.
 *
 * @param base AES peripheral base address.
 * @param interrupts The interrupts to disable, logical OR'ed value of @ref aes_interrupts.
 */
static inline void AES_DisableInterrupts(AES_Type *base, uint32_t interrupts)
{
    base->IMR |= interrupts;
}

/*!
 * @brief Get AES interrupt status.
 *
 * @param base AES peripheral base address.
 * @return The interrupt pending status, logical OR'ed value of @ref aes_interrupts.
 */
static inline uint32_t AES_GetInterruptStatus(AES_Type *base)
{
    return base->ISR;
}

/*!
 * @brief Clear AES interrupt status.
 *
 * @param base AES peripheral base address.
 * @param interrupts The interrupt pending status to clear, logical OR'ed value of @ref aes_interrupts.
 */
static inline void AES_ClearInterruptStatus(AES_Type *base, uint32_t interrupts)
{
    base->ICR = interrupts;
}

/*!
 * @brief Get AES raw interrupt status.
 *
 * This function is similar with @ref AES_GetInterruptStatus, the difference is
 * the raw interrupt status is not affected by interrupt enable status.
 *
 * @param base AES peripheral base address.
 * @return The raw interrupt pending status, logical OR'ed value of @ref aes_interrupts.
 */
static inline uint32_t AES_GetRawInterruptStatus(AES_Type *base)
{
    return base->IRSR;
}

/* @} */

/*!
 * @name High level interface
 * @{
 */

/*!
 * @brief Sets AES key.
 *
 * Sets AES key.
 *
 * @param base AES peripheral base address
 * @param key Input key to use for encryption or decryption
 * @param keySize Size of the input key, in bytes. Must be 16, 24, or 32.
 * @return Status from Set Key operation
 */
status_t AES_SetKey(AES_Type *base, const uint8_t *key, size_t keySize);

/*!
 * @brief Encrypts AES using the ECB block mode.
 *
 * Encrypts AES using the ECB block mode.
 *
 * @param base AES peripheral base address
 * @param plaintext Input plain text to encrypt
 * @param[out] ciphertext Output cipher text
 * @param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * @return Status from encrypt operation
 */
status_t AES_EncryptEcb(AES_Type *base, const uint8_t *plaintext, uint8_t *ciphertext, size_t size);

/*!
 * @brief Decrypts AES using the ECB block mode.
 *
 * Decrypts AES using the ECB block mode.
 *
 * @param base AES peripheral base address
 * @param ciphertext Input ciphertext to decrypt
 * @param[out] plaintext Output plain text
 * @param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * @return Status from decrypt operation
 */
status_t AES_DecryptEcb(AES_Type *base, const uint8_t *ciphertext, uint8_t *plaintext, size_t size);

/*!
 * @brief Encrypts AES using CBC block mode.
 *
 * @param base AES peripheral base address
 * @param plaintext Input plain text to encrypt
 * @param[out] ciphertext Output cipher text
 * @param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * @param iv Input initial vector to combine with the first input block.
 * @param[out] ov Output vector used for chained operation, could be NULL.
 * @return Status from encrypt operation
 */
status_t AES_EncryptCbc(AES_Type *base,
                        const uint8_t *plaintext,
                        uint8_t *ciphertext,
                        size_t size,
                        const uint8_t iv[AES_VECTOR_SIZE],
                        uint8_t ov[AES_VECTOR_SIZE]);

/*!
 * @brief Decrypts AES using CBC block mode.
 *
 * @param base AES peripheral base address
 * @param ciphertext Input cipher text to decrypt
 * @param[out] plaintext Output plain text
 * @param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * @param iv Input initial vector to combine with the first input block.
 * @param[out] ov Output vector used for chained operation, could be NULL.
 * @return Status from decrypt operation
 */
status_t AES_DecryptCbc(AES_Type *base,
                        const uint8_t *ciphertext,
                        uint8_t *plaintext,
                        size_t size,
                        const uint8_t iv[AES_VECTOR_SIZE],
                        uint8_t ov[AES_VECTOR_SIZE]);

/*!
 * @brief Encrypts or decrypts AES using CTR block mode.
 *
 * Encrypts or decrypts AES using CTR block mode.
 * AES CTR mode uses only forward AES cipher and same algorithm for encryption and decryption.
 * The only difference between encryption and decryption is that, for encryption, the input argument
 * is plain text and the output argument is cipher text. For decryption, the input argument is cipher text
 * and the output argument is plain text.
 *
 * For chained calls, the internal status are saved in the parameters: @p nonce, @p streamBlock,
 * and @p offset. In this case, initialize @p nonce to the desired value, and initialize @p offset to 0
 * before the first call, in the following calls, the @p nonce, @p streamBlock, and @p offset should be
 * preserved.
 *
 * @note The @p streamBlock contains sensitive data, discard it after used.
 *
 * @param base AES peripheral base address
 * @param input Input data for CTR block mode
 * @param[out] output Output data for CTR block mode
 * @param size Size of input and output data in bytes
 * @param[in,out] nonce and counter(updates on return)
 * @param[out] streamBlock Saved block for chained CTR calls. NULL can be passed if chained calls are
 * not used.
 * @param[out] offset Offset in @p streamBlock. NULL can be passed if chained calls
 * are not used.
 * @return Status from crypt operation
 */
status_t AES_CryptCtr(AES_Type *base,
                      const uint8_t *input,
                      uint8_t *output,
                      size_t size,
                      uint8_t nonce[AES_BLOCK_SIZE],
                      uint8_t streamBlock[AES_BLOCK_SIZE],
                      size_t *offset);

/*!
 * @brief Encrypts AES and tags using CCM block mode.
 *
 * Encrypts AES and optionally tags using CCM block mode.
 *
 * @param base AES peripheral base address
 * @param plaintext Input plain text to encrypt
 * @param[out] ciphertext Output cipher text.
 * @param size Size of input and output data in bytes
 * @param iv Input initial vector
 * @param ivSize Size of the @p iv
 * @param aad Input additional authentication data
 * @param aadSize Input size in bytes of @p aad.
 * @param[out] tag Output hash tag. Set to NULL to skip tag processing.
 * @param tagSize Input size of the tag to generate, in bytes. Must be 0,4,8, or 16.
 * @return Status from encrypt operation
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
                           size_t tagSize);

/*!
 * @brief Decrypts AES and authenticates using CCM block mode.
 *
 * Decrypts AES and optionally authenticates using CCM block mode.
 *
 * @param base AES peripheral base address
 * @param ciphertext Input cipher text to decrypt
 * @param[out] plaintext Output plain text.
 * @param size Size of input and output data in bytes
 * @param iv Input initial vector
 * @param ivSize Size of the @p iv
 * @param aad Input additional authentication data
 * @param aadSize Input size in bytes of @p aad
 * @param tag Input hash tag to compare. Set to NULL to skip tag processing.
 * @param tagSize Input size of the tag to generate, in bytes. Must be 0,4,8, or 16.
 * @retval kStatus_Success Calculation successed.
 * @retval kStatus_InvalidArgument Invalid argument.
 * @retval kStatus_Fail Tag does not match.
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
                           size_t tagSize);

/*!
 * @brief Calculate the input stream MMO HASH value.
 *
 * @param base AES peripheral base address
 * @param input Input stream.
 * @param size Length of the stream in bytes.
 * @param[out] hash Hash value of the input stream.
 * @return Status from HASH operation
 */
status_t AES_CalculateMMOHash(AES_Type *base, const uint8_t *input, size_t size, uint8_t hash[16]);

/* @} */

/*!
 * @name Function with lock
 *
 * AES module might be used by multiple sofware components, to make sure the
 * on-going proess is not broken by the call from another components, lock is
 * used to protect the operation.
 *
 * To use the lock, set the lock function with @ref AES_SetLockFunc, then when
 * calling the APIs with suffix WithLock, the lock function will be called by
 * AES driver.
 *
 * @{
 */

/*!
 * @brief Set the lock functions.
 *
 * @param lockFunc Lock function.
 * @param unlockFunc Unlock function.
 */
void AES_SetLockFunc(aes_lock_func_t lockFunc, aes_unlock_func_t unlockFunc);

/*!
 * @brief Encrypts AES using the ECB block mode.
 *
 * Encrypts AES using the ECB block mode.
 *
 * @param base AES peripheral base address
 * @param key Input key to use for encryption or decryption
 * @param keySize Size of the input key, in bytes.
 * @param plaintext Input plain text to encrypt
 * @param[out] ciphertext Output cipher text
 * @param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * @return Status from encrypt operation
 */
status_t AES_EncryptEcbWithLock(
    AES_Type *base, const uint8_t *key, size_t keySize, const uint8_t *plaintext, uint8_t *ciphertext, size_t size);

/*!
 * @brief Decrypts AES using the ECB block mode.
 *
 * Decrypts AES using the ECB block mode.
 *
 * @param base AES peripheral base address
 * @param key Input key to use for encryption or decryption
 * @param keySize Size of the input key, in bytes.
 * @param ciphertext Input ciphertext to decrypt
 * @param[out] plaintext Output plain text
 * @param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * @return Status from decrypt operation
 */
status_t AES_DecryptEcbWithLock(
    AES_Type *base, const uint8_t *key, size_t keySize, const uint8_t *ciphertext, uint8_t *plaintext, size_t size);

/*!
 * @brief Encrypts AES using CBC block mode.
 *
 * @param base AES peripheral base address
 * @param key Input key to use for encryption or decryption
 * @param keySize Size of the input key, in bytes.
 * @param plaintext Input plain text to encrypt
 * @param[out] ciphertext Output cipher text
 * @param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * @param iv Input initial vector to combine with the first input block.
 * @param[out] ov Output vector used for chained operation, could be NULL.
 * @return Status from encrypt operation
 */
status_t AES_EncryptCbcWithLock(AES_Type *base,
                                const uint8_t *key,
                                size_t keySize,
                                const uint8_t *plaintext,
                                uint8_t *ciphertext,
                                size_t size,
                                const uint8_t iv[AES_VECTOR_SIZE],
                                uint8_t ov[AES_VECTOR_SIZE]);

/*!
 * @brief Decrypts AES using CBC block mode.
 *
 * @param base AES peripheral base address
 * @param key Input key to use for encryption or decryption
 * @param keySize Size of the input key, in bytes.
 * @param ciphertext Input cipher text to decrypt
 * @param[out] plaintext Output plain text
 * @param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * @param iv Input initial vector to combine with the first input block.
 * @param[out] ov Output vector used for chained operation, could be NULL.
 * @return Status from decrypt operation
 */
status_t AES_DecryptCbcWithLock(AES_Type *base,
                                const uint8_t *key,
                                size_t keySize,
                                const uint8_t *ciphertext,
                                uint8_t *plaintext,
                                size_t size,
                                const uint8_t iv[AES_VECTOR_SIZE],
                                uint8_t ov[AES_VECTOR_SIZE]);

/*!
 * @brief Encrypts or decrypts AES using CTR block mode.
 *
 * Encrypts or decrypts AES using CTR block mode.
 * AES CTR mode uses only forward AES cipher and same algorithm for encryption and decryption.
 * The only difference between encryption and decryption is that, for encryption, the input argument
 * is plain text and the output argument is cipher text. For decryption, the input argument is cipher text
 * and the output argument is plain text.
 *
 * For chained calls, the internal status are saved in the parameters: @p nonce, @p streamBlock,
 * and @p offset. In this case, initialize @p nonce to the desired value, and initialize @p offset to 0
 * before the first call, in the following calls, the @p nonce, @p streamBlock, and @p offset should be
 * preserved.
 *
 * @note The @p streamBlock contains sensitive data, discard it after used.
 *
 * @param base AES peripheral base address
 * @param key Input key to use for encryption or decryption
 * @param keySize Size of the input key, in bytes.
 * @param input Input data for CTR block mode
 * @param[out] output Output data for CTR block mode
 * @param size Size of input and output data in bytes
 * @param[in,out] nonce and counter(updates on return)
 * @param[out] streamBlock Saved block for chained CTR calls. NULL can be passed if chained calls are
 * not used.
 * @param[out] offset Offset in @p streamBlock. NULL can be passed if chained calls
 * are not used.
 * @return Status from crypt operation
 */
status_t AES_CryptCtrWithLock(AES_Type *base,
                              const uint8_t *key,
                              size_t keySize,
                              const uint8_t *input,
                              uint8_t *output,
                              size_t size,
                              uint8_t nonce[AES_BLOCK_SIZE],
                              uint8_t streamBlock[AES_BLOCK_SIZE],
                              size_t *offset);

/*!
 * @brief Encrypts AES and tags using CCM block mode.
 *
 * Encrypts AES and optionally tags using CCM block mode.
 *
 * @param base AES peripheral base address
 * @param key Input key to use for encryption or decryption
 * @param keySize Size of the input key, in bytes.
 * @param plaintext Input plain text to encrypt
 * @param[out] ciphertext Output cipher text.
 * @param size Size of input and output data in bytes
 * @param iv Input initial vector
 * @param ivSize Size of the @p iv
 * @param aad Input additional authentication data
 * @param aadSize Input size in bytes of @p aad.
 * @param[out] tag Output hash tag. Set to NULL to skip tag processing.
 * @param tagSize Input size of the tag to generate, in bytes. Must be 0,4,8, or 16.
 * @return Status from encrypt operation
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
                                   size_t tagSize);

/*!
 * @brief Decrypts AES and authenticates using CCM block mode.
 *
 * Decrypts AES and optionally authenticates using CCM block mode.
 *
 * @param base AES peripheral base address
 * @param key Input key to use for encryption or decryption
 * @param keySize Size of the input key, in bytes.
 * @param ciphertext Input cipher text to decrypt
 * @param[out] plaintext Output plain text.
 * @param size Size of input and output data in bytes
 * @param iv Input initial vector
 * @param ivSize Size of the @p iv
 * @param aad Input additional authentication data
 * @param aadSize Input size in bytes of @p aad
 * @param tag Input hash tag to compare. Set to NULL to skip tag processing.
 * @param tagSize Input size of the tag to generate, in bytes. Must be 0,4,8, or 16.
 * @retval kStatus_Success Calculation successed.
 * @retval kStatus_InvalidArgument Invalid argument.
 * @retval kStatus_Fail Tag does not match.
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
                                   size_t tagSize);

/*!
 * @brief Calculate the input stream MMO HASH value.
 *
 * @param base AES peripheral base address
 * @param input Input stream.
 * @param size Length of the stream in bytes.
 * @param[out] hash Hash value of the input stream.
 * @return Status from HASH operation
 */
status_t AES_CalculateMMOHashWithLock(AES_Type *base, const uint8_t *input, size_t size, uint8_t hash[16]);

/* @} */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/*! @}*/ /* end of group aes */

#endif /* _FSL_AES_H_ */
