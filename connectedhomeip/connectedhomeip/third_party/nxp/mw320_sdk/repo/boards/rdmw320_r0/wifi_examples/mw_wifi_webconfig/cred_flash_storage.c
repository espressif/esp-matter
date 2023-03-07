/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cred_flash_storage.h"
#include "fsl_debug_console.h"
#include "wpl.h"
#if CONFIG_USE_PSM
#include "psm-v2.h"
#else
#include "mflash_file.h"
#endif

#define FILE_HEADER       "wifi_credentials:"
#define FILE_NAME_MAX_LEN (sizeof(s_fname) - 1U)

#if CONFIG_USE_PSM
psm_hnd_t g_psm_handle;
#if CONFIG_SECURE_PSM
/* key and nounces for test purpose. In secure boot, key and nounces in keystore will be used. */
static uint8_t s_key[] = {1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U};
static uint8_t s_iv[]  = {0U, 1U, 4U, 9U, 16U, 25U, 36U, 49U, 64U, 81U, 100U, 121U, 144U, 169U, 196U, 225U};
#endif
#else
mflash_file_t g_file_table[2];
#endif

static char s_credentials_buf[sizeof(FILE_HEADER) + WPL_WIFI_SSID_LENGTH + WPL_WIFI_PASSWORD_LENGTH + 3];
static char s_fname[64];

static uint32_t save_file(char *filename, uint8_t *data, uint32_t data_len)
{
    int32_t result;

    if ((filename == NULL) || (strlen(filename) > FILE_NAME_MAX_LEN) || (data == NULL) || (data_len == 0))
    {
        return 1;
    }

    strcpy(s_fname, filename);

#if CONFIG_USE_PSM
    psm_object_handle_t ohandle;

    do
    {
        result = psm_object_open(g_psm_handle, s_fname, PSM_MODE_WRITE, data_len, NULL, &ohandle);
        if (result != WM_SUCCESS)
        {
            break;
        }
        result = psm_object_write(ohandle, data, data_len);
        if (result != WM_SUCCESS)
        {
            break;
        }
        result = psm_object_close(&ohandle);
        if (result != WM_SUCCESS)
        {
            break;
        }
    } while (false);
#else
    /* Write the data to file. */
    result = (pdFALSE == mflash_save_file(s_fname, data, data_len)) ? WM_FAIL : WM_SUCCESS;
#endif
    if (result != WM_SUCCESS)
    {
        PRINTF("[!] mflash_save_file failed\r\n");
        return 1;
    }
    else
    {
        PRINTF("[i] mflash_save_file success\r\n");
    }

    return 0;
}

uint32_t init_flash_storage(char *filename, flash_desc_t *pFlashDesc)
{
    if (filename == NULL || (strlen(filename) > FILE_NAME_MAX_LEN))
    {
        return 1;
    }

#if CONFIG_USE_PSM
#if CONFIG_SECURE_PSM
    psm_cfg_t cfg  = {.read_only = false,
                     .secure    = true,
                     /* Do not override keystore, so keystore will be used if available. */
                     .keystore_override = false,
                     .key               = s_key,
                     .iv                = s_iv,
                     .key_sz            = sizeof(s_key),
                     .iv_sz             = sizeof(s_iv)};
    int32_t result = psm_module_init(pFlashDesc, &g_psm_handle, &cfg);
#else  /* CONFIG_SECURE_PSM */
    int32_t result = psm_module_init(pFlashDesc, &g_psm_handle, NULL);
#endif /* CONFIG_SECURE_PSM */
    if (result != WM_SUCCESS)
    {
        PRINTF("[!] ERROR in psm_module_init!");
        return 1;
    }
#else  /* CONFIG_USE_PSM */
    /* Flash structure */

    strcpy(g_file_table[0].path, filename);
    g_file_table[0].flash_addr = pFlashDesc->fl_start;
    g_file_table[0].max_size   = pFlashDesc->fl_size;

    if (mflash_init(g_file_table, 1) != pdTRUE)
    {
        PRINTF("[!] ERROR in mflash_init!");
        return 1;
    }
#endif /* CONFIG_USE_PSM */

    return 0;
}

uint32_t save_wifi_credentials(char *filename, char *ssid, char *passphrase)
{
    if (filename == NULL || (strlen(filename) > FILE_NAME_MAX_LEN))
    {
        return 1;
    }

    if (strlen(ssid) > WPL_WIFI_SSID_LENGTH)
    {
        PRINTF("[!] SSID is too long. It can only be %d characters but is %d characters.\n", WPL_WIFI_SSID_LENGTH,
               strlen(ssid));
        return 1;
    }

    if (strlen(passphrase) > WPL_WIFI_PASSWORD_LENGTH)
    {
        PRINTF("[!] Password is too long. It can only be %d characters but is %d characters.\n",
               WPL_WIFI_PASSWORD_LENGTH, strlen(passphrase));
        return 1;
    }

    uint32_t data_len;

    strcpy(s_credentials_buf, FILE_HEADER);
    strcat(s_credentials_buf, ssid);
    strcat(s_credentials_buf, "\n");
    strcat(s_credentials_buf, passphrase);
    strcat(s_credentials_buf, "\n");

    data_len = strlen(s_credentials_buf) + 1; // Need to also store \0

    if (save_file(filename, (uint8_t *)s_credentials_buf, data_len))
    {
        return 1;
    }

    return 0;
}

uint32_t get_saved_wifi_credentials(char *filename, char *ssid, char *passphrase)
{
    char *credentials_buf;
    uint32_t data_len = 0;
    int32_t result;
    ssid[0]       = '\0';
    passphrase[0] = '\0';

    if (filename == NULL || (strlen(filename) > FILE_NAME_MAX_LEN))
    {
        return 1;
    }

#if CONFIG_USE_PSM
    psm_object_handle_t ohandle;
    result = psm_object_open(g_psm_handle, filename, PSM_MODE_READ, sizeof(s_credentials_buf), NULL, &ohandle);
    if (result <= 0)
    {
        return 1;
    }
    result = psm_object_read(ohandle, s_credentials_buf, sizeof(s_credentials_buf));
    if (result <= 0)
    {
        return 1;
    }
    data_len = result;
    result   = psm_object_close(&ohandle);
    if (result != WM_SUCCESS)
    {
        return 1;
    }
    credentials_buf = s_credentials_buf;
#else
    result = mflash_read_file(filename, (uint8_t **)&credentials_buf, &data_len);
    if (result == pdFALSE)
    {
        return 1;
    }
#endif

    if ((data_len > sizeof(FILE_HEADER)) && (strncmp(credentials_buf, FILE_HEADER, strlen(FILE_HEADER)) == 0))
    {
        credentials_buf += strlen(FILE_HEADER);
        uint32_t pos = 0;

        while (*credentials_buf != '\n' && pos <= WPL_WIFI_SSID_LENGTH)
        {
            ssid[pos] = *credentials_buf;
            credentials_buf++;
            pos++;
        };
        ssid[pos] = '\0';
        credentials_buf++;
        pos = 0;

        while (*credentials_buf != '\n' && pos <= WPL_WIFI_PASSWORD_LENGTH)
        {
            passphrase[pos] = *credentials_buf;
            pos++;
            credentials_buf++;
        };
        passphrase[pos] = '\0';
    }
    else
    {
        return 1;
    }

    return 0;
}

uint32_t reset_saved_wifi_credentials(char *filename)
{
    if (filename == NULL || (strlen(filename) > FILE_NAME_MAX_LEN))
    {
        return 1;
    }
#if CONFIG_USE_PSM
    strcpy(s_fname, filename);
    return psm_object_delete(g_psm_handle, s_fname);
#else
    s_credentials_buf[0] = '\0';
    return save_file(filename, (uint8_t *)s_credentials_buf, 1);
#endif
}
