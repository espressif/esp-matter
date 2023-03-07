/** @file psm-v2-secure.c
 *
 *  @brief Persistent storage manager version 2
 *
 *  Copyright 2008-2020 NXP
 *
 *  NXP CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code ("Materials") are owned by NXP, its
 *  suppliers and/or its licensors. Title to the Materials remains with NXP,
 *  its suppliers and/or its licensors. The Materials contain
 *  trade secrets and proprietary and confidential information of NXP, its
 *  suppliers and/or its licensors. The Materials are protected by worldwide copyright
 *  and trade secret laws and treaty provisions. No part of the Materials may be
 *  used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without NXP's prior
 *  express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by NXP in writing.
 *
 */

#ifdef CONFIG_SECURE_PSM

#include <psm-v2.h>
#include "psm-v2-secure.h"
#include <keystore.h>
#include "psm-internal.h"
#include "wm_os.h"
#include "fsl_aes.h"

/*
 * To encrypt (and decrypt) for each PSM object the "name + value"
 * together is considered as a single unit. 'resetkey' function is called at
 * the start of enc/dec operation for every object.
 *
 * Note that IV/nonce will be repeated for each PSM object. As of now each
 * object encryption starts will same IV and key.
 */

typedef struct
{
    bool is_key_copied;
    const uint8_t *key;
    const uint8_t *iv;
    uint16_t key_sz;
    uint16_t iv_sz;
    /* CTR context */
    uint8_t nonce[AES_BLOCK_SIZE];
    uint8_t streamBlock[AES_BLOCK_SIZE];
    size_t offset;
} sec_data_t;

static int psm_keystore_get_data(sec_data_t *sec_data)
{
    /* get encryption key & key_size from keystore */
    psm_d("Using secure keystore");
    int rv = keystore_get_ref(KEY_PSM_ENCRYPT_KEY, &sec_data->key_sz, &sec_data->key);
    if (rv == ERR_TLV_NOT_FOUND)
    {
        psm_e(
            "psm: Could not read key key_size from keystore. "
            "Please flash correct boot2");
        return -WM_E_NOENT;
    }

    /* get nonce from keystore */
    rv = keystore_get_ref(KEY_PSM_NONCE, &sec_data->iv_sz, &sec_data->iv);
    if (rv == ERR_TLV_NOT_FOUND)
    {
        psm_e("psm: Could not read nonce");
        return -WM_E_NOENT;
    }

    return WM_SUCCESS;
}

int psm_security_init(psm_sec_hnd_t *sec_handle)
{
    sec_data_t *sec_data = os_mem_calloc(sizeof(sec_data_t));
    if (!sec_data)
        return -WM_E_NOMEM;

    int rv = psm_keystore_get_data(sec_data);
    if (rv != WM_SUCCESS)
    {
        os_mem_free(sec_data);
        return rv;
    }

    sec_data->is_key_copied = false;

    *sec_handle = (psm_sec_hnd_t)sec_data;
    return WM_SUCCESS;
}

int psm_security_init_with_key(psm_sec_hnd_t *sec_handle, psm_cfg_t *psm_cfg)
{
    /* Check if psm_cfg provides a key */
    if ((psm_cfg->key == NULL) || (psm_cfg->iv == NULL) || (psm_cfg->key_sz == 0) || (psm_cfg->iv_sz == 0))
    {
        return -WM_E_INVAL;
    }

    sec_data_t *sec_data = os_mem_calloc(sizeof(sec_data_t));
    if (!sec_data)
    {
        return -WM_E_NOMEM;
    }

    uint8_t *key = os_mem_calloc(psm_cfg->key_sz);
    if (!key)
    {
        os_mem_free(sec_data);
        return -WM_E_NOMEM;
    }
    memcpy(key, psm_cfg->key, psm_cfg->key_sz);
    sec_data->key    = key;
    sec_data->key_sz = psm_cfg->key_sz;

    uint8_t *iv = os_mem_calloc(psm_cfg->iv_sz);
    if (!iv)
    {
        os_mem_free(key);
        os_mem_free(sec_data);
        return -WM_E_NOMEM;
    }
    memcpy(iv, psm_cfg->iv, psm_cfg->iv_sz);
    sec_data->iv    = iv;
    sec_data->iv_sz = psm_cfg->iv_sz;

    sec_data->is_key_copied = true;

    *sec_handle = (psm_sec_hnd_t)sec_data;
    return WM_SUCCESS;
}

#define KEY_IN_KS_UNINIT  -1
#define KEY_IN_KS_ABSENT  0
#define KEY_IN_KS_PRESENT 1

bool psm_is_key_present_in_keystore()
{
    static int8_t key_present = KEY_IN_KS_UNINIT;

    /* Prevent frequent searches */
    if (key_present == KEY_IN_KS_UNINIT)
        key_present = ((keystore_get_ref(KEY_PSM_ENCRYPT_KEY, NULL, NULL) != ERR_TLV_NOT_FOUND) &&
                       (keystore_get_ref(KEY_PSM_NONCE, NULL, NULL) != ERR_TLV_NOT_FOUND));

    return key_present;
}

int psm_key_reset(psm_sec_hnd_t sec_handle, psm_resetkey_mode_t mode)
{
    sec_data_t *sec_data = (sec_data_t *)sec_handle;

    if (!sec_data)
        return -WM_FAIL;

    AES_SetCTRCountModular(AES, 0);
    assert(sec_data->iv_sz >= AES_BLOCK_SIZE);
    memcpy(sec_data->nonce, sec_data->iv, AES_BLOCK_SIZE);
    sec_data->offset = 0U;

    return WM_SUCCESS;
}

int psm_encrypt(psm_sec_hnd_t sec_handle, const void *plain, void *cipher, uint32_t len)
{
    sec_data_t *sec_data = (sec_data_t *)sec_handle;
    int status;

    if (!sec_data)
        return -WM_FAIL;

    status = AES_CryptCtrWithLock(AES, sec_data->key, sec_data->key_sz, plain, cipher, len, sec_data->nonce,
                                  sec_data->streamBlock, &sec_data->offset);

    return status;
}

int psm_decrypt(psm_sec_hnd_t sec_handle, const void *cipher, void *plain, uint32_t len)
{
    sec_data_t *sec_data = (sec_data_t *)sec_handle;
    int status;

    if (!sec_data)
        return -WM_FAIL;

    status = AES_CryptCtrWithLock(AES, sec_data->key, sec_data->key_sz, cipher, plain, len, sec_data->nonce,
                                  sec_data->streamBlock, &sec_data->offset);

    return status;
}

void psm_security_deinit(psm_sec_hnd_t *sec_handle)
{
    sec_data_t *sec_data = (sec_data_t *)*sec_handle;

    if (!sec_data)
        return;

    uint8_t *key = (uint8_t *)sec_data->key;
    uint8_t *iv  = (uint8_t *)sec_data->iv;

    if (sec_data->is_key_copied)
    {
        os_mem_free(iv);
        os_mem_free(key);
    }
    os_mem_free(sec_data);
}
#endif /* CONFIG_SECURE_PSM */
