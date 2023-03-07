/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

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

#define CBC_MSG_LEN    128
#define CBC_KEY_LEN    (128 / 8)
#define CBC_VECTOR_LEN 16

#define CTR_MSG_LEN    256
#define CTR_KEY_LEN    (128 / 8)
#define CTR_VECTOR_LEN 16

#define CCM_MSG_LEN 128
#define CCM_IV_LEN  12
#define CCM_AAD_LEN 32
#define CCM_TAG_LEN 16
#define CCM_KEY_LEN (128 / 8)

#define MMO_MSG_LEN 32
#define MMO_KEY_LEN (128 / 8)

#define APP_PRINTF(...)                                    \
    do                                                     \
    {                                                      \
        xSemaphoreTakeRecursive(printLock, portMAX_DELAY); \
        PRINTF(__VA_ARGS__);                               \
        xSemaphoreGiveRecursive(printLock);                \
    } while (0)

typedef uint32_t (*app_aes_func_t)(void);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static uint32_t APP_AES_ECB(void);
static uint32_t APP_AES_CBC_OneStep(void);
static uint32_t APP_AES_CTR_OneStep(void);
static uint32_t APP_AES_CCM(void);
static uint32_t APP_AES_MMO(void);

static void aes_task_1(void *pvParameters);
static void aes_task_2(void *pvParameters);

/*******************************************************************************
 * Variables
 ******************************************************************************/
static SemaphoreHandle_t aesLock;
static SemaphoreHandle_t printLock;

/*******************************************************************************
 * Code
 ******************************************************************************/

static status_t APP_AES_Lock(void)
{
    if (pdTRUE == xSemaphoreTakeRecursive(aesLock, portMAX_DELAY))
    {
        return kStatus_Success;
    }
    else
    {
        return kStatus_Fail;
    }
}

static void APP_AES_Unlock(void)
{
    xSemaphoreGiveRecursive(aesLock);
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

    APP_PRINTF("\r\n======== AES ECB START ========\r\n");

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

    AES_EncryptEcbWithLock(APP_AES, key, sizeof(key), plainText, cipherText, sizeof(plainText));

    AES_DecryptEcbWithLock(APP_AES, key, sizeof(key), cipherText, plainTextDecrypt, sizeof(cipherText));

    if (0 != memcmp(plainText, plainTextDecrypt, sizeof(plainText)))
    {
        APP_PRINTF("\r\nERROR: ECB decrypted data not match.\r\n");

        ret = 1;
    }
    else
    {
        ret = 0;
    }

    APP_PRINTF("\r\n======== AES ECB STOP ========\r\n");

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

    APP_PRINTF("\r\n======== AES CBC ONESTEP START ========\r\n");

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

    AES_EncryptCbcWithLock(APP_AES, key, sizeof(key), plainText, cipherText, sizeof(plainText), inputVector, NULL);

    AES_DecryptCbcWithLock(APP_AES, key, sizeof(key), cipherText, plainTextDecrypt, sizeof(plainText), inputVector,
                           NULL);

    if (0 != memcmp(plainText, plainTextDecrypt, sizeof(plainText)))
    {
        APP_PRINTF("\r\nERROR: CBC decrypted data not match.\r\n");

        ret = 1;
    }
    else
    {
        ret = 0;
    }

    APP_PRINTF("\r\n======== AES CBC ONESTEP STOP ========\r\n");

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

    APP_PRINTF("\r\n======== AES CTR ONESTEP START ========\r\n");

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

    /* Set nonce and counter to defined value. */
    for (uint32_t i = 0; i < AES_BLOCK_SIZE; i++)
    {
        nonce_counter[i] = i * i;
    }
    AES_CryptCtrWithLock(APP_AES, key, sizeof(key), plainText, cipherText, sizeof(plainText), nonce_counter, NULL,
                         NULL);

    /* Set nonce and counter to defined value. */
    for (uint32_t i = 0; i < AES_BLOCK_SIZE; i++)
    {
        nonce_counter[i] = i * i;
    }
    AES_CryptCtrWithLock(APP_AES, key, sizeof(key), cipherText, plainTextDecrypt, sizeof(plainText), nonce_counter,
                         NULL, NULL);

    if (0 != memcmp(plainText, plainTextDecrypt, sizeof(plainText)))
    {
        APP_PRINTF("\r\nERROR: AES CTR decrypted data not match.\r\n");

        ret = 1;
    }
    else
    {
        ret = 0;
    }

    APP_PRINTF("\r\n======== AES CTR ONESTEP STOP ========\r\n");

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

    APP_PRINTF("\r\n======== AES CCM START ========\r\n");

    AES_EncryptTagCcmWithLock(APP_AES, key, sizeof(key), plainText, cipherText, sizeof(plainText), iv, sizeof(iv), aad,
                              sizeof(aad), tag, sizeof(tag));

    status = AES_DecryptTagCcmWithLock(APP_AES, key, sizeof(key), cipherText, plainTextDecrypt,
                                       sizeof(plainTextDecrypt), iv, sizeof(iv), aad, sizeof(aad), tag, sizeof(tag));
    if (kStatus_Success != status)
    {
        ret++;
    }

    if (0 != memcmp(plainText, plainTextDecrypt, sizeof(plainText)))
    {
        APP_PRINTF("\r\nERROR: CCM decrypted data not match.\r\n");

        ret++;
    }

    APP_PRINTF("\r\n======== AES CCM STOP ========\r\n");

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

    APP_PRINTF("\r\n======== AES MMO START ========\r\n");

    AES_CalculateMMOHashWithLock(APP_AES, plainText, sizeof(plainText), hash);

    if (0 != memcmp(hashRef, hash, sizeof(hash)))
    {
        APP_PRINTF("\r\nERROR: Hash not match.\r\n");

        ret = 1;
    }
    else
    {
        ret = 0;
    }

    APP_PRINTF("\r\n======== AES MMO STOP ========\r\n");

    return ret;
}

static void aes_task_1(void *pvParameters)
{
    static const app_aes_func_t funcs[] = {
        APP_AES_ECB,
        APP_AES_CBC_OneStep,
        APP_AES_CTR_OneStep,
    };

    while (1)
    {
        for (uint8_t i = 0; i < ARRAY_SIZE(funcs); i++)
        {
            configASSERT(0U == funcs[i]());
        }
    }
}

static void aes_task_2(void *pvParameters)
{
    static const app_aes_func_t funcs[] = {
        APP_AES_CCM,
        APP_AES_MMO,
    };

    while (1)
    {
        for (uint8_t i = 0; i < ARRAY_SIZE(funcs); i++)
        {
            configASSERT(0U == funcs[i]());
        }
    }
}

/*!
 * @brief Main function
 */
int main(void)
{
    BOARD_InitPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    PRINTF("\r\nAES lock example started\r\n");

    aesLock = xSemaphoreCreateRecursiveMutex();

    if (NULL == aesLock)
    {
        PRINTF("Failed to create AES lock\r\n");
        while (1)
            ;
    }

    printLock = xSemaphoreCreateRecursiveMutex();

    if (NULL == printLock)
    {
        PRINTF("Failed to create PRINTF lock\r\n");
        while (1)
            ;
    }

    AES_Init(APP_AES);

    AES_SetLockFunc(APP_AES_Lock, APP_AES_Unlock);

    if (xTaskCreate(aes_task_1, "task1", configMINIMAL_STACK_SIZE + 512, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }
    if (xTaskCreate(aes_task_2, "task2", configMINIMAL_STACK_SIZE + 512, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }
    /* Start scheduling. */
    vTaskStartScheduler();
    for (;;)
        ;
}
