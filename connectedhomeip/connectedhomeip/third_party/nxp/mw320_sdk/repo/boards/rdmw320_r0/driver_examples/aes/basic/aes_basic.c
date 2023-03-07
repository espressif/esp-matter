/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_aes.h"

#include "fsl_device_registers.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define APP_AES AES
#define ECB_MSG_LEN 64
#define ECB_KEY_LEN (128 / 8)

#define CBC_MSG_LEN    256
#define CBC_KEY_LEN    (128 / 8)
#define CBC_VECTOR_LEN 16

#define CTR_MSG_LEN    256
#define CTR_KEY_LEN    (128 / 8)
#define CTR_VECTOR_LEN 16

#define CCM_MSG_LEN 256
#define CCM_IV_LEN  12
#define CCM_AAD_LEN 100
#define CCM_TAG_LEN 16
#define CCM_KEY_LEN (128 / 8)

#define MMO_MSG_LEN 32
#define MMO_KEY_LEN (128 / 8)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static uint32_t APP_AES_ECB(void);
static uint32_t APP_AES_CBC_OneStep(void);
static uint32_t APP_AES_CBC_Chain(void);
static uint32_t APP_AES_CTR_OneStep(void);
static uint32_t APP_AES_CTR_Chain(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
static void dump_hex(const char *str, const uint8_t data[], uint32_t len)
{
    PRINTF("\r\n%s", str);

    for (uint32_t i = 0; i < len; i++)
    {
        if (i % 16 == 0)
            PRINTF("\r\n%04X: ", i);

        PRINTF(" %02X", data[i]);
    }

    PRINTF("\r\n");
}

/*
 * ECB function examples.
 *
 * This function returns 0 if success, return 1 if error.
 */
static uint32_t APP_AES_ECB(void)
{
    uint32_t ret;
    uint8_t key[ECB_KEY_LEN];
    uint8_t plainText[ECB_MSG_LEN];
    uint8_t cipherText[ECB_MSG_LEN];
    uint8_t plainTextDecrypt[ECB_MSG_LEN];

    PRINTF("\r\n======== AES ECB START ========\r\n");

    for (uint32_t i = 0; i < ECB_MSG_LEN; i++)
    {
        plainText[i] = i;
    }

    for (uint32_t i = 0; i < ECB_KEY_LEN; i++)
    {
        key[i] = i * i;
    }

    memset(plainTextDecrypt, 0, sizeof(plainTextDecrypt));
    memset(cipherText, 0, sizeof(cipherText));

    AES_SetKey(APP_AES, key, sizeof(key));

    AES_EncryptEcb(APP_AES, plainText, cipherText, sizeof(plainText));

    AES_DecryptEcb(APP_AES, cipherText, plainTextDecrypt, sizeof(cipherText));

    dump_hex("Plain Text:", plainText, sizeof(plainText));
    dump_hex("Key:", key, sizeof(key));
    dump_hex("Cipher Text:", cipherText, sizeof(cipherText));
    dump_hex("Decrypted Text :", plainTextDecrypt, sizeof(plainTextDecrypt));

    if (0 != memcmp(plainText, plainTextDecrypt, sizeof(plainText)))
    {
        PRINTF("\r\nERROR: Decrypted data not match.\r\n");

        ret = 1;
    }
    else
    {
        PRINTF("\r\nDecrypted data match.\r\n");

        ret = 0;
    }

    PRINTF("\r\n======== AES ECB STOP ========\r\n");

    return ret;
}

/*
 * CBC function examples.
 *
 * This function returns 0 if success, return 1 if error.
 */
static uint32_t APP_AES_CBC_OneStep(void)
{
    uint32_t ret;
    uint8_t key[CBC_KEY_LEN];
    uint8_t plainText[CBC_MSG_LEN];
    uint8_t cipherText[CBC_MSG_LEN];
    uint8_t plainTextDecrypt[CBC_MSG_LEN];
    uint8_t inputVector[CBC_VECTOR_LEN];

    PRINTF("\r\n======== AES CBC ONESTEP START ========\r\n");

    for (uint32_t i = 0; i < CBC_MSG_LEN; i++)
    {
        plainText[i] = i;
    }

    for (uint32_t i = 0; i < CBC_KEY_LEN; i++)
    {
        key[i] = 2 * i * i;
    }

    for (uint32_t i = 0; i < CBC_VECTOR_LEN; i++)
    {
        inputVector[i] = 2 * i;
    }

    memset(plainTextDecrypt, 0, sizeof(plainTextDecrypt));
    memset(cipherText, 0, sizeof(cipherText));

    AES_SetKey(APP_AES, key, sizeof(key));

    AES_EncryptCbc(APP_AES, plainText, cipherText, sizeof(plainText), inputVector, NULL);

    AES_DecryptCbc(APP_AES, cipherText, plainTextDecrypt, sizeof(plainText), inputVector, NULL);

    dump_hex("Plain Text:", plainText, sizeof(plainText));
    dump_hex("Key:", key, sizeof(key));
    dump_hex("Cipher Text:", cipherText, sizeof(cipherText));
    dump_hex("Decrypted Text :", plainTextDecrypt, sizeof(plainTextDecrypt));

    if (0 != memcmp(plainText, plainTextDecrypt, sizeof(plainText)))
    {
        PRINTF("\r\nERROR: Decrypted data not match.\r\n");

        ret = 1;
    }
    else
    {
        PRINTF("\r\nDecrypted data match.\r\n");

        ret = 0;
    }

    PRINTF("\r\n======== AES CBC ONESTEP STOP ========\r\n");

    return ret;
}

/*
 * CBC function examples.
 *
 * This function returns 0 if success, return 1 if error.
 */
static uint32_t APP_AES_CBC_Chain(void)
{
    uint32_t ret;
    uint8_t key[CBC_KEY_LEN];
    uint8_t plainText[CBC_MSG_LEN];
    uint8_t cipherText[CBC_MSG_LEN];
    uint8_t plainTextDecrypt[CBC_MSG_LEN];
    uint8_t inputVector[CBC_VECTOR_LEN];
    uint8_t outputVector[CBC_VECTOR_LEN];

    PRINTF("\r\n======== AES CBC CHAIN START ========\r\n");

    for (uint32_t i = 0; i < CBC_MSG_LEN; i++)
    {
        plainText[i] = i;
    }

    for (uint32_t i = 0; i < CBC_KEY_LEN; i++)
    {
        key[i] = 2 * i * i;
    }

    for (uint32_t i = 0; i < CBC_VECTOR_LEN; i++)
    {
        inputVector[i] = 2 * i;
    }

    memset(plainTextDecrypt, 0, sizeof(plainTextDecrypt));
    memset(cipherText, 0, sizeof(cipherText));
    memset(outputVector, 0, sizeof(outputVector));

    AES_SetKey(APP_AES, key, sizeof(key));

    memcpy(outputVector, inputVector, sizeof(outputVector));
    for (uint32_t i = 0; i < CBC_MSG_LEN; i += 16)
    {
        /* Calculate 16 byte per loop. */
        AES_EncryptCbc(APP_AES, &plainText[i], &cipherText[i], 16, outputVector, outputVector);
    }

    memcpy(outputVector, inputVector, sizeof(outputVector));
    for (uint32_t i = 0; i < CBC_MSG_LEN; i += 16)
    {
        /* Calculate 16 byte per loop. */
        AES_DecryptCbc(APP_AES, &cipherText[i], &plainTextDecrypt[i], 16, outputVector, outputVector);
    }

    dump_hex("Plain Text:", plainText, sizeof(plainText));
    dump_hex("Key:", key, sizeof(key));
    dump_hex("Cipher Text:", cipherText, sizeof(cipherText));
    dump_hex("Decrypted Text :", plainTextDecrypt, sizeof(plainTextDecrypt));

    if (0 != memcmp(plainText, plainTextDecrypt, sizeof(plainText)))
    {
        PRINTF("\r\nERROR: Decrypted data not match.\r\n");

        ret = 1;
    }
    else
    {
        PRINTF("\r\nDecrypted data match.\r\n");

        ret = 0;
    }

    PRINTF("\r\n======== AES CBC CHAIN STOP ========\r\n");

    return ret;
}

/*
 * CTR function examples.
 *
 * This function returns 0 if success, return 1 if error.
 */
static uint32_t APP_AES_CTR_OneStep(void)
{
    uint32_t ret;
    uint8_t key[CTR_KEY_LEN];
    uint8_t plainText[CTR_MSG_LEN];
    uint8_t cipherText[CTR_MSG_LEN];
    uint8_t plainTextDecrypt[CTR_MSG_LEN];

    uint8_t nonce_counter[AES_BLOCK_SIZE];

    PRINTF("\r\n======== AES CTR ONESTEP START ========\r\n");

    for (uint32_t i = 0; i < CTR_MSG_LEN; i++)
    {
        plainText[i] = i;
    }

    for (uint32_t i = 0; i < CTR_KEY_LEN; i++)
    {
        key[i] = i * i;
    }

    memset(plainTextDecrypt, 0, sizeof(plainTextDecrypt));
    memset(cipherText, 0, sizeof(cipherText));

    AES_SetKey(APP_AES, key, sizeof(key));

    /* Set nonce and counter to defined value. */
    for (uint32_t i = 0; i < AES_BLOCK_SIZE; i++)
    {
        nonce_counter[i] = i * i;
    }
    AES_CryptCtr(APP_AES, plainText, cipherText, sizeof(plainText), nonce_counter, NULL, NULL);

    /* Set nonce and counter to defined value. */
    for (uint32_t i = 0; i < AES_BLOCK_SIZE; i++)
    {
        nonce_counter[i] = i * i;
    }
    AES_CryptCtr(APP_AES, cipherText, plainTextDecrypt, sizeof(plainText), nonce_counter, NULL, NULL);

    dump_hex("Plain Text:", plainText, sizeof(plainText));
    dump_hex("Key:", key, sizeof(key));
    dump_hex("Cipher Text:", cipherText, sizeof(cipherText));
    dump_hex("Decrypted Text :", plainTextDecrypt, sizeof(plainTextDecrypt));

    if (0 != memcmp(plainText, plainTextDecrypt, sizeof(plainText)))
    {
        PRINTF("\r\nERROR: Decrypted data not match.\r\n");

        ret = 1;
    }
    else
    {
        PRINTF("\r\nDecrypted data match.\r\n");

        ret = 0;
    }

    PRINTF("\r\n======== AES CTR ONESTEP STOP ========\r\n");

    return ret;
}

/*
 * CTR function examples.
 *
 * This function returns 0 if success, return 1 if error.
 */
static uint32_t APP_AES_CTR_Chain(void)
{
    uint32_t ret;
    uint8_t key[CTR_KEY_LEN];
    uint8_t plainText[CTR_MSG_LEN];
    uint8_t cipherText[CTR_MSG_LEN];
    uint8_t plainTextDecrypt[CTR_MSG_LEN];

    size_t nc_off;
    uint8_t nonce_counter[AES_BLOCK_SIZE];
    uint8_t stream_block[AES_BLOCK_SIZE];

    PRINTF("\r\n======== AES CTR CHAIN START ========\r\n");

    for (uint32_t i = 0; i < CTR_MSG_LEN; i++)
    {
        plainText[i] = i;
    }

    for (uint32_t i = 0; i < CTR_KEY_LEN; i++)
    {
        key[i] = i * i;
    }

    memset(plainTextDecrypt, 0, sizeof(plainTextDecrypt));
    memset(cipherText, 0, sizeof(cipherText));

    AES_SetKey(APP_AES, key, sizeof(key));

    /* Set nonce and counter to defined value. */
    for (uint32_t i = 0; i < AES_BLOCK_SIZE; i++)
    {
        nonce_counter[i] = i * i;
    }
    nc_off = 0;

    for (uint32_t i = 0; i < CTR_MSG_LEN; i += 17)
    {
        if ((CTR_MSG_LEN - i) >= 17)
        {
            AES_CryptCtr(APP_AES, &plainText[i], &cipherText[i], 17, nonce_counter, stream_block, &nc_off);
        }
        else
        {
            AES_CryptCtr(APP_AES, &plainText[i], &cipherText[i], CTR_MSG_LEN - i, nonce_counter, stream_block, &nc_off);
        }
    }

    /* Set nonce and counter to defined value. */
    for (uint32_t i = 0; i < AES_BLOCK_SIZE; i++)
    {
        nonce_counter[i] = i * i;
    }

    nc_off = 0;

    for (uint32_t i = 0; i < CTR_MSG_LEN; i += 17)
    {
        if ((CTR_MSG_LEN - i) >= 17)
        {
            AES_CryptCtr(APP_AES, &cipherText[i], &plainTextDecrypt[i], 17, nonce_counter, stream_block, &nc_off);
        }
        else
        {
            AES_CryptCtr(APP_AES, &cipherText[i], &plainTextDecrypt[i], CTR_MSG_LEN - i, nonce_counter, stream_block,
                         &nc_off);
        }
    }

    dump_hex("Plain Text:", plainText, sizeof(plainText));
    dump_hex("Key:", key, sizeof(key));
    dump_hex("Cipher Text:", cipherText, sizeof(cipherText));
    dump_hex("Decrypted Text :", plainTextDecrypt, sizeof(plainTextDecrypt));

    if (0 != memcmp(plainText, plainTextDecrypt, sizeof(plainText)))
    {
        PRINTF("\r\nERROR: Decrypted data not match.\r\n");

        ret = 1;
    }
    else
    {
        PRINTF("\r\nDecrypted data match.\r\n");

        ret = 0;
    }

    PRINTF("\r\n======== AES CTR CHAIN STOP ========\r\n");

    return ret;
}

/*
 * CCM* function examples.
 *
 * This function returns 0 if success, return 1 if error.
 */
static uint32_t APP_AES_CCM(void)
{
    status_t status;
    uint32_t ret = 0;
    uint8_t key[CCM_KEY_LEN];
    uint8_t plainText[CCM_MSG_LEN];
    uint8_t cipherText[CCM_MSG_LEN];
    uint8_t plainTextDecrypt[CCM_MSG_LEN];
    uint8_t iv[CCM_IV_LEN];
    uint8_t aad[CCM_AAD_LEN];
    uint8_t tag[CCM_TAG_LEN];

    for (uint32_t i = 0; i < CCM_MSG_LEN; i++)
    {
        plainText[i] = i;
    }

    for (uint32_t i = 0; i < CCM_KEY_LEN; i++)
    {
        key[i] = i * i;
    }

    for (uint32_t i = 0; i < CCM_IV_LEN; i++)
    {
        iv[i] = 2 * i;
    }

    for (uint32_t i = 0; i < CCM_AAD_LEN; i++)
    {
        aad[i] = 3 * i;
    }

    memset(tag, 0, sizeof(tag));
    memset(cipherText, 0, sizeof(cipherText));
    memset(plainTextDecrypt, 0, sizeof(plainTextDecrypt));

    PRINTF("\r\n======== AES CCM START ========\r\n");

    /* Set key. */
    AES_SetKey(APP_AES, key, sizeof(key));

    AES_EncryptTagCcm(APP_AES, plainText, cipherText, sizeof(plainText), iv, sizeof(iv), aad, sizeof(aad), tag,
                      sizeof(tag));

    status = AES_DecryptTagCcm(APP_AES, cipherText, plainTextDecrypt, sizeof(plainTextDecrypt), iv, sizeof(iv), aad,
                               sizeof(aad), tag, sizeof(tag));
    if (kStatus_Success != status)
    {
        ret++;
    }

    dump_hex("Plain Text:", plainText, ARRAY_SIZE(plainText));
    dump_hex("Key:", key, ARRAY_SIZE(key));
    dump_hex("Cipher Text:", cipherText, ARRAY_SIZE(cipherText));
    dump_hex("Tag:", tag, sizeof(tag));
    dump_hex("Decrypted Text :", plainTextDecrypt, sizeof(plainTextDecrypt));

    if (0 != memcmp(plainText, plainTextDecrypt, sizeof(plainText)))
    {
        PRINTF("\r\nERROR: Decrypted data not match.\r\n");

        ret++;
    }
    else
    {
        PRINTF("\r\nDecrypted data match.\r\n");
    }

    PRINTF("\r\n======== AES CCM STOP ========\r\n");

    return ret;
}

/*
 * MMO function examples.
 *
 * This function returns 0 if success, return 1 if error.
 */
static uint32_t APP_AES_MMO(void)
{
    uint32_t ret = 0;

    uint8_t plainText[MMO_MSG_LEN];
    uint8_t hash[MMO_KEY_LEN];
    const uint8_t hashRef[] = {0x89, 0x04, 0x5F, 0x84, 0x7A, 0x7C, 0x54, 0x4A,
                               0xFC, 0x20, 0x25, 0xF8, 0xEF, 0x9D, 0x9C, 0x64};

    for (uint32_t i = 0; i < MMO_MSG_LEN; i++)
    {
        plainText[i] = i;
    }

    for (uint32_t i = 0; i < MMO_KEY_LEN; i++)
    {
        hash[i] = 2 * i;
    }

    PRINTF("\r\n======== AES MMO START ========\r\n");

    dump_hex("Plain Text:", plainText, ARRAY_SIZE(plainText));
    dump_hex("Initial Hash:", hash, ARRAY_SIZE(hash));

    AES_CalculateMMOHash(APP_AES, plainText, sizeof(plainText), hash);

    dump_hex("Hash:", hash, ARRAY_SIZE(hash));

    if (0 != memcmp(hashRef, hash, sizeof(hash)))
    {
        PRINTF("\r\nERROR: Hash not match.\r\n");

        ret = 1;
    }
    else
    {
        PRINTF("\r\nHash match.\r\n");

        ret = 0;
    }

    PRINTF("\r\n======== AES MMO STOP ========\r\n");

    return ret;
}

/*!
 * @brief Main function
 */
int main(void)
{
    uint32_t error = 0;

    BOARD_InitPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    PRINTF("\r\nAES example started\r\n");

    AES_Init(APP_AES);

    error += APP_AES_ECB();
    error += APP_AES_CBC_OneStep();
    error += APP_AES_CBC_Chain();
    error += APP_AES_CTR_OneStep();
    error += APP_AES_CTR_Chain();
    error += APP_AES_CCM();
    error += APP_AES_MMO();

    PRINTF("\r\nAES example %s\r\n", (error > 0) ? "failed" : "successed");

    while (true)
        ;
}
