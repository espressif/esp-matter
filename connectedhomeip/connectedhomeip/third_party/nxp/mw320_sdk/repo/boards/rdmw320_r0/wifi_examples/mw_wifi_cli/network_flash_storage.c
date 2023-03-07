/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "network_flash_storage.h"
#include "fsl_debug_console.h"
#if CONFIG_USE_PSM
#include "psm-v2.h"
#else
#include "mflash_file.h"
#endif

#define FILE_HEADER       "wifi_networks:"
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

static uint8_t s_buf[2048];
static char s_fname[64];

static uint32_t save_file(char *filename, uint8_t *data, uint32_t dataLen)
{
    int32_t result;

    if ((filename == NULL) || (strlen(filename) > FILE_NAME_MAX_LEN) || (data == NULL) || (dataLen == 0))
    {
        return 1;
    }

    strcpy(s_fname, filename);

#if CONFIG_USE_PSM
    psm_object_handle_t ohandle;

    do
    {
        result = psm_object_open(g_psm_handle, s_fname, PSM_MODE_WRITE, dataLen, NULL, &ohandle);
        if (result != WM_SUCCESS)
        {
            break;
        }
        result = psm_object_write(ohandle, data, dataLen);
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
    result = (pdFALSE == mflash_save_file(s_fname, data, dataLen)) ? WM_FAIL : WM_SUCCESS;
#endif
    if (result != WM_SUCCESS)
    {
        PRINTF("[!] mflash_save_file failed, [%s], [%d]\r\n", s_fname, result);
        return 1;
    }
    else
    {
        PRINTF("[i] mflash_save_file success, [%s]\r\n", s_fname);
        return 0;
    }
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

uint32_t save_wifi_network(char *filename, uint8_t *network, uint32_t len)
{
    if ((filename == NULL) || (network == NULL) || (strlen(filename) > FILE_NAME_MAX_LEN) ||
        ((sizeof(FILE_HEADER) + 4 + len) > sizeof(s_buf)))
    {
        return 1;
    }

    memcpy(s_buf, FILE_HEADER, sizeof(FILE_HEADER));
    memcpy(s_buf + sizeof(FILE_HEADER), &len, 4);
    memcpy(s_buf + sizeof(FILE_HEADER) + 4, network, len);

    if (save_file(filename, s_buf, sizeof(FILE_HEADER) + 4 + len) != 0)
    {
        return 1;
    }

    return 0;
}

uint32_t get_saved_wifi_network(char *filename, uint8_t *network, uint32_t *len)
{
    int32_t result;
    uint32_t payloadLen;
    uint8_t *buf;
    uint32_t dataLen;

    if (filename == NULL || network == NULL || len == NULL || (strlen(filename) > FILE_NAME_MAX_LEN))
    {
        return 1;
    }
#if CONFIG_USE_PSM
    psm_object_handle_t ohandle;
    result = psm_object_open(g_psm_handle, filename, PSM_MODE_READ, sizeof(s_buf), NULL, &ohandle);
    if (result <= 0)
    {
        PRINTF("[%s], open_file fail [%s], (%d) \r\n", __FUNCTION__, filename, result);
        return 1;
    }
    result = psm_object_read(ohandle, s_buf, sizeof(s_buf));
    if (result <= 0)
    {
        PRINTF("[%s], read data error \r\n", __FUNCTION__);
        return 1;
    }
    dataLen = result;
    result  = psm_object_close(&ohandle);
    if (result != WM_SUCCESS)
    {
        PRINTF("[%s], close file error \r\n", __FUNCTION__);
        return 1;
    }
    buf = s_buf;
#else
    result = mflash_read_file(filename, &buf, &dataLen);
    if (result == pdFALSE)
    {
        return 1;
    }
#endif

    if ((dataLen > sizeof(FILE_HEADER) + 4) && (memcmp(buf, FILE_HEADER, sizeof(FILE_HEADER)) == 0))
    {
        buf += sizeof(FILE_HEADER);
        memcpy(&payloadLen, buf, 4);
        if (payloadLen + sizeof(FILE_HEADER) + 4 > dataLen)
        {
            return 1;
        }
        buf += 4;
        *len = MIN(*len, payloadLen);
        memcpy(network, buf, *len);
        return 0;
    }

    return 1;
}

uint32_t reset_saved_wifi_network(char *filename)
{
    if (filename == NULL || (strlen(filename) > FILE_NAME_MAX_LEN))
    {
        return 1;
    }
#if CONFIG_USE_PSM
    strcpy(s_fname, filename);
    return psm_object_delete(g_psm_handle, s_fname);
#else
    s_buf[0] = '\0';
    return save_file(filename, s_buf, 1);
#endif
}

// Added:
// Erase all saved parameters
// Method_1: Erase all saved object
// Method_2: Format the whole partition

#define MAX_ARG_CNT     64
static uint16_t g_arg_cnt=0;
static char g_argname[MAX_ARG_CNT][32];

int psmobj_cb_handler(const uint8_t *name)
{
    //PRINTF("==> %s(), [%s] \r\n", __FUNCTION__, name);
    if (g_arg_cnt < MAX_ARG_CNT) {
        strcpy(g_argname[g_arg_cnt++], (char*)name);
    } else {
        PRINTF("[error], too many saved args \r\n");
    }
    return 0;
}

uint32_t erase_all_params(void)
{
#if 1
    // ===> Using Method_1
#if CONFIG_USE_PSM
    uint16_t i;

    psm_objects_list(g_psm_handle, psmobj_cb_handler);
    for (i=0 ; i<g_arg_cnt ; i++) {
        reset_saved_wifi_network(g_argname[i]);
    }
    g_arg_cnt=0;
#endif // CONFIG_USE_PSM
    return 0;
#else
    // ===> Use Method_2
    // Fromat the whole partition => It will take effect only if after rebooting the device
    int32_t res = 0;

#if CONFIG_USE_PSM
    res = psm_format(g_psm_handle);
#endif //CONFIG_USE_PSM
    if (res == WM_SUCCESS) {
        PRINTF("psm_format ok\r\n");
        return 0;
    } else {
        PRINTF("psm_format error \r\n");
        return 1;
    }
#endif //
}
