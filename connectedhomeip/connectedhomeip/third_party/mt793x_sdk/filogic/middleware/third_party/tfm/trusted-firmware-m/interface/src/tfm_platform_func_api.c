/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include "tfm_platform_api.h"
#include "tfm_ns_interface.h"
#include "tfm_veneers.h"
#ifdef CRYPTO_HW_ACCELERATOR
#include "hal_aes.h"
#include "log/tfm_log.h"
#include "psa/crypto.h"
#include "psa/crypto_types.h"
#include "psa/crypto_values.h"
#include "psa/crypto_sizes.h"
#include "psa/crypto_struct.h"
#endif /* CRYPTO_HW_ACCELERATOR */
#include "hal_spm.h"
#include "memory_attribute.h"

enum tfm_platform_err_t tfm_platform_system_reset(void)
{
    return (enum tfm_platform_err_t) tfm_ns_interface_dispatch(
                                (veneer_fn)tfm_platform_sp_system_reset_veneer,
                                0,
                                0,
                                0,
                                0);
}

enum tfm_platform_err_t
tfm_platform_ioctl(tfm_platform_ioctl_req_t request,
                   psa_invec *input, psa_outvec *output)
{
    tfm_platform_ioctl_req_t req = request;
    struct psa_invec in_vec[2];
    size_t inlen, outlen;

    in_vec[0].base = &req;
    in_vec[0].len = sizeof(req);
    if (input != NULL) {
        in_vec[1].base = input->base;
        in_vec[1].len = input->len;
        inlen = 2;
    } else {
        inlen = 1;
    }

    if (output != NULL) {
        outlen = 1;
    } else {
        outlen = 0;
    }

    return (enum tfm_platform_err_t) tfm_ns_interface_dispatch(
                                (veneer_fn)tfm_platform_sp_ioctl_veneer,
                                (uint32_t)in_vec, (uint32_t)inlen,
                                (uint32_t)output, (uint32_t)outlen);
}

void tfm_sleep_management_internal_sys_init(void *slp)
{
    struct psa_invec in_vec[1];
    void *tfm_slp = slp;

    in_vec[0].base = &tfm_slp;
    in_vec[0].len = sizeof(tfm_slp);

    tfm_ns_interface_dispatch((veneer_fn)tfm_platform_slp_internal_sys_init_veneer, (uint32_t)in_vec, 1, 0, 0);
}

void tfm_sleep_management_platform_sys_init(void)
{
    tfm_ns_interface_dispatch((veneer_fn)tfm_platform_slp_platform_sys_init_veneer, 0, 0, 0, 0);
}

void tfm_suspend_callback_SYS_CONN_BUS(void *data)
{
    tfm_ns_interface_dispatch((veneer_fn)tfm_platform_slp_conn_bus_backup_veneer, 0, 0, 0, 0);
}

ATTR_TEXT_IN_TCM uint32_t tfm_sleep_management_enter_deep_sleep_sys(void)
{
    return tfm_ns_interface_dispatch((veneer_fn)tfm_platform_slp_system_sleep_wakeup_veneer, 0, 0, 0, 0);
}

void tfm_slp_conn_bus_suspend(void *data)
{
    tfm_ns_interface_dispatch((veneer_fn)tfm_platform_slp_conn_bus_suspend_veneer, 0, 0, 0, 0);
}

void tfm_slp_conn_bus_resume(void *data)
{
    tfm_ns_interface_dispatch((veneer_fn)tfm_platform_slp_conn_bus_resume_veneer, 0, 0, 0, 0);
}

void tfm_sleep_management_enter_wfi_sys(void)
{
    tfm_ns_interface_dispatch((veneer_fn)tfm_platform_slp_enter_wfi_sys_veneer, 0, 0, 0, 0);
}

ATTR_TEXT_IN_TCM void tfm_spm_suspend_module_lock(spm_suspend_lock_module_t module, bool lock)
{
    struct psa_invec in_vec[2];
    spm_suspend_lock_module_t tfm_mod = module;
    bool tfm_lock = lock;

    in_vec[0].base = &tfm_mod;
    in_vec[0].len = sizeof(tfm_mod);
    in_vec[1].base = &tfm_lock;
    in_vec[1].len = sizeof(tfm_lock);

    tfm_ns_interface_dispatch((veneer_fn)tfm_platform_spm_spm_suspend_module_lock_veneer, (uint32_t)in_vec, 2, 0, 0);
}

void tfm_spm_ctrl(spm_ctrl_t ctrl)
{
    struct psa_invec in_vec[1];
    spm_ctrl_t tfm_ctrl = ctrl;

    in_vec[0].base = &tfm_ctrl;
    in_vec[0].len = sizeof(tfm_ctrl);

    tfm_ns_interface_dispatch((veneer_fn)tfm_platform_spm_spm_ctrl_veneer, (uint32_t)in_vec, 1, 0, 0);
}

ATTR_TEXT_IN_TCM void tfm_spm_mtcmos(spm_module_t module, spm_onoff_t onoff, unsigned int chain_no)
{
    struct psa_invec in_vec[3];
    spm_module_t tfm_mod = module;
    spm_onoff_t tfm_onoff = onoff;
    unsigned int tfm_chain_no = chain_no;

    in_vec[0].base = &tfm_mod;
    in_vec[0].len = sizeof(tfm_mod);
    in_vec[1].base = &tfm_onoff;
    in_vec[1].len = sizeof(tfm_onoff);
    in_vec[2].base = &tfm_chain_no;
    in_vec[2].len = sizeof(tfm_chain_no);

    tfm_ns_interface_dispatch((veneer_fn)tfm_platform_spm_spm_mtcmos_veneer, (uint32_t)in_vec, 3, 0, 0);
}

void tfm_hal_spm_init(void)
{
    tfm_ns_interface_dispatch((veneer_fn)tfm_platform_spm_hal_spm_init_veneer, 0, 0, 0, 0);
}

int32_t tfm_aes_ecb_encrypt(void *out_EncTxt, void *in_PlnTxt, void *in_Key)
{
#ifdef MTK_HW_ACCELERATOR_WO_MBEDTLS
    struct psa_invec in_vec[2], out_vec[1];
    size_t inlen, outlen;

    in_vec[0].base = &in_PlnTxt;
    in_vec[0].len = sizeof(in_PlnTxt);
    in_vec[1].base = &in_Key;
    in_vec[1].len = sizeof(in_Key);
    inlen = 2;

    out_vec[0].base = &out_EncTxt;
    out_vec[0].len = sizeof(out_EncTxt);
    outlen = 1;

    return (int32_t) tfm_ns_interface_dispatch(
                                (veneer_fn)tfm_tfm_hal_aes_ecb_encrypt_veneer,
                                (uint32_t)in_vec, (uint32_t)inlen,
                                (uint32_t)out_vec, (uint32_t)outlen);
#endif
#ifdef CRYPTO_HW_ACCELERATOR
    psa_status_t status = PSA_SUCCESS;
    psa_key_attributes_t key_attributes = psa_key_attributes_init();
    psa_key_usage_t usage = (PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    int alg = PSA_ALG_ECB_NO_PADDING;
    psa_key_type_t key_type = PSA_KEY_TYPE_AES;
    psa_key_handle_t key_handle;
    psa_cipher_operation_t handle = psa_cipher_operation_init();
    hal_aes_buffer_t *plain_text = (hal_aes_buffer_t*)in_PlnTxt;
    hal_aes_buffer_t *encrypted_data = (hal_aes_buffer_t*)out_EncTxt;
    hal_aes_buffer_t *key = (hal_aes_buffer_t*)in_Key;
    unsigned int proc_len = 0;

    psa_set_key_usage_flags(&key_attributes, usage);
    psa_set_key_algorithm(&key_attributes, alg);
    psa_set_key_type(&key_attributes, key_type);

    /* Import a key */
    status = psa_import_key(&key_attributes, key->buffer, key->length, &key_handle);
    if (status != PSA_SUCCESS) {
        LOG_MSG("Error importing a key");
        goto destroy_key;
    }

    /* Setup the encryption object */
    status = psa_cipher_encrypt_setup(&handle, key_handle, alg);
    if (status != PSA_SUCCESS) {
        if (status == PSA_ERROR_NOT_SUPPORTED) {
            LOG_MSG("Algorithm NOT SUPPORTED by the implementation");
        } else {
            LOG_MSG("Cipher encryption setup fail");
        }
        goto destroy_key;
    }

    /* Encrypt plain_text information */
    status = psa_cipher_update(&handle, plain_text->buffer, plain_text->length,
            encrypted_data->buffer, encrypted_data->length,
            (size_t *)(&proc_len));
    if (status != PSA_SUCCESS) {
        LOG_MSG("Error encrypting plain_text of information");
        status = psa_cipher_abort(&handle);
        if (status != PSA_SUCCESS) {
            LOG_MSG("Error aborting the operation");
        }
        goto destroy_key;
    }

    /* Finalise the cipher operation */
    status = psa_cipher_finish(&handle, &(encrypted_data->buffer[encrypted_data->length]),
            encrypted_data->length,
            (size_t *)(&proc_len));
    if (status != PSA_SUCCESS) {
        LOG_MSG("Error finalising the cipher operation");
        status = psa_cipher_abort(&handle);
        if (status != PSA_SUCCESS) {
            LOG_MSG("aborting the operation");
        }
        goto destroy_key;
    }

destroy_key:
    /* Destroy the key */
    status = psa_destroy_key(key_handle);
    if (status != PSA_SUCCESS) {
        LOG_MSG("Error destroying a key");
    }
#endif /* CRYPTO_HW_ACCELERATOR */
}

int tfm_generate_random(void *ctr_drbg, unsigned char *output, size_t output_len)
{
#ifdef MTK_HW_ACCELERATOR_WO_MBEDTLS
    struct psa_invec out_vec[1];
    size_t outlen;

    out_vec[0].base = &output;
    out_vec[0].len = output_len;
    outlen = 1;

    return (int) tfm_ns_interface_dispatch(
                                (veneer_fn)tfm_tfm_hal_trng_random_generate_veneer,
                                0, 0,
                                (uint32_t)out_vec, (uint32_t)outlen);
#else
    (void)ctr_drbg;

    return (int) psa_generate_random(output, output_len);
#endif
}
