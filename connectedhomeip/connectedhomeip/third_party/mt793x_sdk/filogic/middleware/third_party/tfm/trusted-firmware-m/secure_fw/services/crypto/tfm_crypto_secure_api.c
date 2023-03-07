/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_veneers.h"
#include "tfm_crypto_defs.h"
#include "psa/crypto.h"
#ifdef TFM_PSA_API
#include "psa_manifest/sid.h"
#endif

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))

#ifdef TFM_PSA_API
#include "psa/client.h"

#define PSA_CONNECT(service)                                    \
    psa_handle_t ipc_handle;                                    \
    ipc_handle = psa_connect(service##_SID, service##_VERSION); \
    if (!PSA_HANDLE_IS_VALID(ipc_handle)) {                     \
        return PSA_ERROR_GENERIC_ERROR;                         \
    }                                                           \

#define PSA_CLOSE() psa_close(ipc_handle)

#define API_DISPATCH(sfn_name, sfn_id)                         \
    psa_call(ipc_handle, PSA_IPC_CALL,                         \
        in_vec, ARRAY_SIZE(in_vec),                            \
        out_vec, ARRAY_SIZE(out_vec))

#define API_DISPATCH_NO_OUTVEC(sfn_name, sfn_id)               \
    psa_call(ipc_handle, PSA_IPC_CALL,                         \
        in_vec, ARRAY_SIZE(in_vec),                            \
        (psa_outvec *)NULL, 0)
#else
#define API_DISPATCH(sfn_name, sfn_id)                         \
    tfm_##sfn_name##_veneer(                                   \
        in_vec, ARRAY_SIZE(in_vec),                            \
        out_vec, ARRAY_SIZE(out_vec))

#define API_DISPATCH_NO_OUTVEC(sfn_name, sfn_id)               \
    tfm_##sfn_name##_veneer(                                   \
        in_vec, ARRAY_SIZE(in_vec),                            \
        NULL, 0)
#endif /* TFM_PSA_API */

__attribute__((section("SFN")))
psa_status_t psa_crypto_init(void)
{
    /* Service init is performed during TFM boot up,
     * so application level initialisation is empty
     */
    return PSA_SUCCESS;
}

__attribute__((section("SFN")))

psa_status_t psa_open_key(psa_key_id_t id,
                          psa_key_handle_t *handle)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_OPEN_KEY_SID,
    };
    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = &id, .len = sizeof(psa_key_id_t)},
    };
    psa_outvec out_vec[] = {
        {.base = handle, .len = sizeof(psa_key_handle_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_open_key,
                          TFM_CRYPTO_OPEN_KEY);

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_close_key(psa_key_handle_t handle)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_CLOSE_KEY_SID,
        .key_handle = handle,
    };
    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH_NO_OUTVEC(tfm_crypto_close_key,
                                    TFM_CRYPTO_CLOSE_KEY);;

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_import_key(const psa_key_attributes_t *attributes,
                            const uint8_t *data,
                            size_t data_length,
                            psa_key_handle_t *handle)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_IMPORT_KEY_SID,
    };
    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = attributes, .len = sizeof(psa_key_attributes_t)},
        {.base = data, .len = data_length}
    };
    psa_outvec out_vec[] = {
        {.base = handle, .len = sizeof(psa_key_handle_t)}
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_import_key,
                          TFM_CRYPTO_IMPORT_KEY);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_destroy_key(psa_key_handle_t handle)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_DESTROY_KEY_SID,
        .key_handle = handle,
    };
    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH_NO_OUTVEC(tfm_crypto_destroy_key,
                                    TFM_CRYPTO_DESTROY_KEY);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_get_key_attributes(psa_key_handle_t handle,
                                    psa_key_attributes_t *attributes)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_GET_KEY_ATTRIBUTES_SID,
        .key_handle = handle,
    };
    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = attributes, .len = sizeof(psa_key_attributes_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_get_key_attributes,
                          TFM_CRYPTO_GET_KEY_ATTRIBUTES);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

void psa_reset_key_attributes(psa_key_attributes_t *attributes)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_RESET_KEY_ATTRIBUTES_SID,
    };
    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = attributes, .len = sizeof(psa_key_attributes_t)},
    };

#ifdef TFM_PSA_API
    psa_handle_t ipc_handle;
    ipc_handle = psa_connect(TFM_CRYPTO_SID, TFM_CRYPTO_VERSION);
    if (!PSA_HANDLE_IS_VALID(ipc_handle)) {
        return;
    }
#endif

    (void)API_DISPATCH(tfm_crypto_reset_key_attributes,
                          TFM_CRYPTO_RESET_KEY_ATTRIBUTES);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_export_key(psa_key_handle_t handle,
                            uint8_t *data,
                            size_t data_size,
                            size_t *data_length)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_EXPORT_KEY_SID,
        .key_handle = handle,
    };
    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = data, .len = data_size}
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_export_key,
                          TFM_CRYPTO_EXPORT_KEY);

    *data_length = out_vec[0].len;

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_export_public_key(psa_key_handle_t handle,
                                   uint8_t *data,
                                   size_t data_size,
                                   size_t *data_length)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_EXPORT_PUBLIC_KEY_SID,
        .key_handle = handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = data, .len = data_size}
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_export_public_key,
                          TFM_CRYPTO_EXPORT_PUBLIC_KEY);

    *data_length = out_vec[0].len;

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_copy_key(psa_key_handle_t source_handle,
                          const psa_key_attributes_t *attributes,
                          psa_key_handle_t *target_handle)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_COPY_KEY_SID,
        .key_handle = source_handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = attributes, .len = sizeof(psa_key_attributes_t)},
    };

    psa_outvec out_vec[] = {
        {.base = target_handle, .len = sizeof(psa_key_handle_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_copy_key,
                          TFM_CRYPTO_COPY_KEY);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_cipher_generate_iv(psa_cipher_operation_t *operation,
                                    unsigned char *iv,
                                    size_t iv_size,
                                    size_t *iv_length)
{
#ifdef TFM_CRYPTO_CIPHER_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_CIPHER_GENERATE_IV_SID,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
        {.base = iv, .len = iv_size},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_cipher_generate_iv,
                          TFM_CRYPTO_CIPHER_GENERATE_IV);

    *iv_length = out_vec[1].len;

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_CIPHER_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_cipher_set_iv(psa_cipher_operation_t *operation,
                               const unsigned char *iv,
                               size_t iv_length)
{
#ifdef TFM_CRYPTO_CIPHER_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_CIPHER_SET_IV_SID,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = iv, .len = iv_length},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_cipher_set_iv,
                          TFM_CRYPTO_CIPHER_SET_IV);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_CIPHER_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_cipher_encrypt_setup(psa_cipher_operation_t *operation,
                                      psa_key_handle_t handle,
                                      psa_algorithm_t alg)
{
#ifdef TFM_CRYPTO_CIPHER_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_CIPHER_ENCRYPT_SETUP_SID,
        .key_handle = handle,
        .alg = alg,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_cipher_encrypt_setup,
                          TFM_CRYPTO_CIPHER_ENCRYPT_SETUP);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_CIPHER_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_cipher_decrypt_setup(psa_cipher_operation_t *operation,
                                      psa_key_handle_t handle,
                                      psa_algorithm_t alg)
{
#ifdef TFM_CRYPTO_CIPHER_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_CIPHER_DECRYPT_SETUP_SID,
        .key_handle = handle,
        .alg = alg,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_cipher_decrypt_setup,
                          TFM_CRYPTO_CIPHER_DECRYPT_SETUP);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_CIPHER_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_cipher_update(psa_cipher_operation_t *operation,
                               const uint8_t *input,
                               size_t input_length,
                               unsigned char *output,
                               size_t output_size,
                               size_t *output_length)
{
#ifdef TFM_CRYPTO_CIPHER_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_CIPHER_UPDATE_SID,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = input, .len = input_length},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
        {.base = output, .len = output_size}
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_cipher_update,
                          TFM_CRYPTO_CIPHER_UPDATE);

    *output_length = out_vec[1].len;

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_CIPHER_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_cipher_abort(psa_cipher_operation_t *operation)
{
#ifdef TFM_CRYPTO_CIPHER_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_CIPHER_ABORT_SID,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_cipher_abort,
                          TFM_CRYPTO_CIPHER_ABORT);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_CIPHER_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_cipher_finish(psa_cipher_operation_t *operation,
                               uint8_t *output,
                               size_t output_size,
                               size_t *output_length)
{
#ifdef TFM_CRYPTO_CIPHER_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_CIPHER_FINISH_SID,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
        {.base = output, .len = output_size},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_cipher_finish,
                          TFM_CRYPTO_CIPHER_FINISH);

    *output_length = out_vec[1].len;

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_CIPHER_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_hash_setup(psa_hash_operation_t *operation,
                            psa_algorithm_t alg)
{
#ifdef TFM_CRYPTO_HASH_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_HASH_SETUP_SID,
        .alg = alg,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_hash_setup,
                          TFM_CRYPTO_HASH_SETUP);

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_HASH_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_hash_update(psa_hash_operation_t *operation,
                             const uint8_t *input,
                             size_t input_length)
{
#ifdef TFM_CRYPTO_HASH_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_HASH_UPDATE_SID,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = input, .len = input_length},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_hash_update,
                          TFM_CRYPTO_HASH_UPDATE);

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_HASH_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_hash_finish(psa_hash_operation_t *operation,
                             uint8_t *hash,
                             size_t hash_size,
                             size_t *hash_length)
{
#ifdef TFM_CRYPTO_HASH_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_HASH_FINISH_SID,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
        {.base = hash, .len = hash_size},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_hash_finish,
                          TFM_CRYPTO_HASH_FINISH);

    *hash_length = out_vec[1].len;

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_HASH_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_hash_verify(psa_hash_operation_t *operation,
                             const uint8_t *hash,
                             size_t hash_length)
{
#ifdef TFM_CRYPTO_HASH_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_HASH_VERIFY_SID,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = hash, .len = hash_length},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_hash_verify,
                          TFM_CRYPTO_HASH_VERIFY);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_HASH_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_hash_abort(psa_hash_operation_t *operation)
{
#ifdef TFM_CRYPTO_HASH_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_HASH_ABORT_SID,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_hash_abort,
                          TFM_CRYPTO_HASH_ABORT);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_HASH_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_hash_clone(const psa_hash_operation_t *source_operation,
                            psa_hash_operation_t *target_operation)
{
#ifdef TFM_CRYPTO_HASH_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_HASH_CLONE_SID,
        .op_handle = source_operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = target_operation, .len = sizeof(psa_hash_operation_t)},
    };

    if (target_operation && (target_operation->handle != 0)) {
        return PSA_ERROR_BAD_STATE;
    }

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_hash_clone,
                          TFM_CRYPTO_HASH_CLONE);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_HASH_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_mac_sign_setup(psa_mac_operation_t *operation,
                                psa_key_handle_t handle,
                                psa_algorithm_t alg)
{
#ifdef TFM_CRYPTO_MAC_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_MAC_SIGN_SETUP_SID,
        .key_handle = handle,
        .alg = alg,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_mac_sign_setup,
                          TFM_CRYPTO_MAC_SIGN_SETUP);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_MAC_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_mac_verify_setup(psa_mac_operation_t *operation,
                                  psa_key_handle_t handle,
                                  psa_algorithm_t alg)
{
#ifdef TFM_CRYPTO_MAC_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_MAC_VERIFY_SETUP_SID,
        .key_handle = handle,
        .alg = alg,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_mac_verify_setup,
                          TFM_CRYPTO_MAC_VERIFY_SETUP);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_MAC_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_mac_update(psa_mac_operation_t *operation,
                            const uint8_t *input,
                            size_t input_length)
{
#ifdef TFM_CRYPTO_MAC_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_MAC_UPDATE_SID,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = input, .len = input_length},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_mac_update,
                          TFM_CRYPTO_MAC_UPDATE);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_MAC_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_mac_sign_finish(psa_mac_operation_t *operation,
                                 uint8_t *mac,
                                 size_t mac_size,
                                 size_t *mac_length)
{
#ifdef TFM_CRYPTO_MAC_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_MAC_SIGN_FINISH_SID,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
        {.base = mac, .len = mac_size},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_mac_sign_finish,
                          TFM_CRYPTO_MAC_SIGN_FINISH);

    *mac_length = out_vec[1].len;

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_MAC_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_mac_verify_finish(psa_mac_operation_t *operation,
                                   const uint8_t *mac,
                                   size_t mac_length)
{
#ifdef TFM_CRYPTO_MAC_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_MAC_VERIFY_FINISH_SID,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = mac, .len = mac_length},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_mac_verify_finish,
                          TFM_CRYPTO_MAC_VERIFY_FINISH);

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_MAC_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_mac_abort(psa_mac_operation_t *operation)
{
#ifdef TFM_CRYPTO_MAC_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_MAC_ABORT_SID,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_mac_abort,
                          TFM_CRYPTO_MAC_ABORT);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_MAC_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_aead_encrypt(psa_key_handle_t handle,
                              psa_algorithm_t alg,
                              const uint8_t *nonce,
                              size_t nonce_length,
                              const uint8_t *additional_data,
                              size_t additional_data_length,
                              const uint8_t *plaintext,
                              size_t plaintext_length,
                              uint8_t *ciphertext,
                              size_t ciphertext_size,
                              size_t *ciphertext_length)
{
#ifdef TFM_CRYPTO_AEAD_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_AEAD_ENCRYPT_SID,
        .key_handle = handle,
        .alg = alg,
        .aead_in = {.nonce = {0}, .nonce_length = nonce_length}
    };

    /* Sanitize the optional input */
    if ((additional_data == NULL) && (additional_data_length != 0)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    size_t idx = 0;
    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = plaintext, .len = plaintext_length},
        {.base = additional_data, .len = additional_data_length},
    };
    psa_outvec out_vec[] = {
        {.base = ciphertext, .len = ciphertext_size},
    };

    if (nonce_length > TFM_CRYPTO_MAX_NONCE_LENGTH) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (nonce != NULL) {
        for (idx = 0; idx < nonce_length; idx++) {
            iov.aead_in.nonce[idx] = nonce[idx];
        }
    }

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

#ifdef TFM_PSA_API
    size_t in_len = ARRAY_SIZE(in_vec);
    if (additional_data == NULL) {
        in_len--;
    }
    status = psa_call(ipc_handle, PSA_IPC_CALL, in_vec, in_len,
                      out_vec, ARRAY_SIZE(out_vec));
#else
    status = API_DISPATCH(tfm_crypto_aead_encrypt,
                          TFM_CRYPTO_AEAD_ENCRYPT);
#endif

    *ciphertext_length = out_vec[0].len;

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_AEAD_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_aead_decrypt(psa_key_handle_t handle,
                              psa_algorithm_t alg,
                              const uint8_t *nonce,
                              size_t nonce_length,
                              const uint8_t *additional_data,
                              size_t additional_data_length,
                              const uint8_t *ciphertext,
                              size_t ciphertext_length,
                              uint8_t *plaintext,
                              size_t plaintext_size,
                              size_t *plaintext_length)
{
#ifdef TFM_CRYPTO_AEAD_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_AEAD_DECRYPT_SID,
        .key_handle = handle,
        .alg = alg,
        .aead_in = {.nonce = {0}, .nonce_length = nonce_length}
    };

    /* Sanitize the optional input */
    if ((additional_data == NULL) && (additional_data_length != 0)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    size_t idx = 0;
    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = ciphertext, .len = ciphertext_length},
        {.base = additional_data, .len = additional_data_length},
    };
    psa_outvec out_vec[] = {
        {.base = plaintext, .len = plaintext_size},
    };

    if (nonce_length > TFM_CRYPTO_MAX_NONCE_LENGTH) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (nonce != NULL) {
        for (idx = 0; idx < nonce_length; idx++) {
            iov.aead_in.nonce[idx] = nonce[idx];
        }
    }

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

#ifdef TFM_PSA_API
    size_t in_len = ARRAY_SIZE(in_vec);
    if (additional_data == NULL) {
        in_len--;
    }
    status = psa_call(ipc_handle, PSA_IPC_CALL, in_vec, in_len,
                      out_vec, ARRAY_SIZE(out_vec));
#else
    status = API_DISPATCH(tfm_crypto_aead_decrypt,
                          TFM_CRYPTO_AEAD_DECRYPT);
#endif

    *plaintext_length = out_vec[0].len;

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_AEAD_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_asymmetric_sign(psa_key_handle_t handle,
                                 psa_algorithm_t alg,
                                 const uint8_t *hash,
                                 size_t hash_length,
                                 uint8_t *signature,
                                 size_t signature_size,
                                 size_t *signature_length)
{
    return psa_sign_hash(handle, alg, hash, hash_length, signature, signature_size, signature_length);
}

__attribute__((section("SFN")))
psa_status_t psa_sign_hash(psa_key_handle_t handle,
                           psa_algorithm_t alg,
                           const uint8_t *hash,
                           size_t hash_length,
                           uint8_t *signature,
                           size_t signature_size,
                           size_t *signature_length)
{
#ifdef TFM_CRYPTO_ASYMMETRIC_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_SIGN_HASH_SID,
        .key_handle = handle,
        .alg = alg,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = hash, .len = hash_length},
    };
    psa_outvec out_vec[] = {
        {.base = signature, .len = signature_size},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif
    status = API_DISPATCH(tfm_crypto_sign_hash,
                          TFM_CRYPTO_SIGN_HASH);

    *signature_length = out_vec[0].len;

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_ASYMMETRIC_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_asymmetric_verify(psa_key_handle_t handle,
                                   psa_algorithm_t alg,
                                   const uint8_t *hash,
                                   size_t hash_length,
                                   const uint8_t *signature,
                                   size_t signature_length)
{
    return psa_verify_hash(handle, alg, hash, hash_length, signature, signature_length);
}

__attribute__((section("SFN")))
psa_status_t psa_verify_hash(psa_key_handle_t handle,
                             psa_algorithm_t alg,
                             const uint8_t *hash,
                             size_t hash_length,
                             const uint8_t *signature,
                             size_t signature_length)
{
#ifdef TFM_CRYPTO_ASYMMETRIC_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_VERIFY_HASH_SID,
        .key_handle = handle,
        .alg = alg
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = hash, .len = hash_length},
        {.base = signature, .len = signature_length}
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH_NO_OUTVEC(tfm_crypto_verify_hash,
                                    TFM_CRYPTO_VERIFY_HASH);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_ASYMMETRIC_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_asymmetric_encrypt(psa_key_handle_t handle,
                                    psa_algorithm_t alg,
                                    const uint8_t *input,
                                    size_t input_length,
                                    const uint8_t *salt,
                                    size_t salt_length,
                                    uint8_t *output,
                                    size_t output_size,
                                    size_t *output_length)
{
#ifdef TFM_CRYPTO_ASYMMETRIC_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_ASYMMETRIC_ENCRYPT_SID,
        .key_handle = handle,
        .alg = alg
    };

    /* Sanitize the optional input */
    if ((salt == NULL) && (salt_length != 0)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = input, .len = input_length},
        {.base = salt, .len = salt_length}
    };

    psa_outvec out_vec[] = {
        {.base = output, .len = output_size},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

#ifdef TFM_PSA_API
    size_t in_len = ARRAY_SIZE(in_vec);
    if (salt == NULL) {
        in_len--;
    }
    status = psa_call(ipc_handle, PSA_IPC_CALL, in_vec, in_len,
                      out_vec, ARRAY_SIZE(out_vec));
#else
    status = API_DISPATCH(tfm_crypto_asymmetric_encrypt,
                          TFM_CRYPTO_ASYMMETRIC_ENCRYPT);
#endif

    *output_length = out_vec[0].len;

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_ASYMMETRIC_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_asymmetric_decrypt(psa_key_handle_t handle,
                                    psa_algorithm_t alg,
                                    const uint8_t *input,
                                    size_t input_length,
                                    const uint8_t *salt,
                                    size_t salt_length,
                                    uint8_t *output,
                                    size_t output_size,
                                    size_t *output_length)
{
#ifdef TFM_CRYPTO_ASYMMETRIC_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_ASYMMETRIC_DECRYPT_SID,
        .key_handle = handle,
        .alg = alg
    };

    /* Sanitize the optional input */
    if ((salt == NULL) && (salt_length != 0)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = input, .len = input_length},
        {.base = salt, .len = salt_length}
    };

    psa_outvec out_vec[] = {
        {.base = output, .len = output_size},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

#ifdef TFM_PSA_API
    size_t in_len = ARRAY_SIZE(in_vec);
    if (salt == NULL) {
        in_len--;
    }
    status = psa_call(ipc_handle, PSA_IPC_CALL, in_vec, in_len,
                      out_vec, ARRAY_SIZE(out_vec));
#else
    status = API_DISPATCH(tfm_crypto_asymmetric_decrypt,
                          TFM_CRYPTO_ASYMMETRIC_DECRYPT);
#endif

    *output_length = out_vec[0].len;

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_ASYMMETRIC_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_key_derivation_get_capacity(
                                const psa_key_derivation_operation_t *operation,
                                size_t *capacity)
{
#ifdef TFM_CRYPTO_GENERATOR_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_KEY_DERIVATION_GET_CAPACITY_SID,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };

    psa_outvec out_vec[] = {
        {.base = capacity, .len = sizeof(size_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_key_derivation_get_capacity,
                          TFM_CRYPTO_KEY_DERIVATION_GET_CAPACITY);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_GENERATOR_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_key_derivation_output_bytes(
                                      psa_key_derivation_operation_t *operation,
                                      uint8_t *output,
                                      size_t output_length)
{
#ifdef TFM_CRYPTO_GENERATOR_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_KEY_DERIVATION_OUTPUT_BYTES_SID,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };

    psa_outvec out_vec[] = {
        {.base = output, .len = output_length},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_key_derivation_output_bytes,
                          TFM_CRYPTO_KEY_DERIVATION_OUTPUT_BYTES);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_GENERATOR_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_key_derivation_input_key(
                                      psa_key_derivation_operation_t *operation,
                                      psa_key_derivation_step_t step,
                                      psa_key_handle_t handle)
{
#ifdef TFM_CRYPTO_GENERATOR_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_KEY_DERIVATION_INPUT_KEY_SID,
        .key_handle = handle,
        .step = step,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH_NO_OUTVEC(tfm_crypto_key_derivation_input_key,
                                    TFM_CRYPTO_KEY_DERIVATION_INPUT_KEY);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_GENERATOR_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_key_derivation_abort(psa_key_derivation_operation_t *operation)
{
#ifdef TFM_CRYPTO_GENERATOR_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_KEY_DERIVATION_ABORT_SID,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };

    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_key_derivation_abort,
                          TFM_CRYPTO_KEY_DERIVATION_ABORT);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_GENERATOR_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_key_derivation_key_agreement(
                                      psa_key_derivation_operation_t *operation,
                                      psa_key_derivation_step_t step,
                                      psa_key_handle_t private_key,
                                      const uint8_t *peer_key,
                                      size_t peer_key_length)
{
#ifdef TFM_CRYPTO_GENERATOR_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_KEY_DERIVATION_KEY_AGREEMENT_SID,
        .key_handle = private_key,
        .step = step,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = peer_key, .len = peer_key_length},
    };

    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_key_derivation_key_agreement,
                          TFM_CRYPTO_KEY_DERIVATION_KEY_AGREEMENT);

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_GENERATOR_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_generate_random(uint8_t *output,
                                 size_t output_size)
{
#ifdef TFM_CRYPTO_GENERATOR_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_GENERATE_RANDOM_SID,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };

    psa_outvec out_vec[] = {
        {.base = output, .len = output_size},
    };

    if (output_size == 0) {
        return PSA_SUCCESS;
    }

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_generate_random,
                          TFM_CRYPTO_GENERATE_RANDOM);

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_GENERATOR_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_generate_key(const psa_key_attributes_t *attributes,
                              psa_key_handle_t *handle)
{
#ifdef TFM_CRYPTO_GENERATOR_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_GENERATE_KEY_SID,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = attributes, .len = sizeof(psa_key_attributes_t)},
    };

    psa_outvec out_vec[] = {
        {.base = handle, .len = sizeof(psa_key_handle_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_generate_key,
                          TFM_CRYPTO_GENERATE_KEY);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_GENERATOR_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_set_key_domain_parameters(psa_key_attributes_t *attributes,
                                           psa_key_type_t type,
                                           const uint8_t *data,
                                           size_t data_length)
{
    psa_status_t status;

    status = PSA_ERROR_NOT_SUPPORTED;

    return status;
}

__attribute__((section("SFN")))
psa_status_t psa_get_key_domain_parameters(
                                         const psa_key_attributes_t *attributes,
                                         uint8_t *data,
                                         size_t data_size,
                                         size_t *data_length)
{
    psa_status_t status;

    status = PSA_ERROR_NOT_SUPPORTED;

    return status;
}

__attribute__((section("SFN")))
psa_status_t psa_hash_compare(psa_algorithm_t alg,
                              const uint8_t *input,
                              size_t input_length,
                              const uint8_t *hash,
                              const size_t hash_length)
{
    psa_status_t status;

    status = PSA_ERROR_NOT_SUPPORTED;

    return status;
}

__attribute__((section("SFN")))
psa_status_t psa_aead_update_ad(psa_aead_operation_t *operation,
                                const uint8_t *input,
                                size_t input_length)
{
    psa_status_t status;

    status = PSA_ERROR_NOT_SUPPORTED;

    return status;
}

__attribute__((section("SFN")))
psa_status_t psa_aead_finish(psa_aead_operation_t *operation,
                             uint8_t *ciphertext,
                             size_t ciphertext_size,
                             size_t *ciphertext_length,
                             uint8_t *tag,
                             size_t tag_size,
                             size_t *tag_length)
{
    psa_status_t status;

    status = PSA_ERROR_NOT_SUPPORTED;

    return status;
}

__attribute__((section("SFN")))
psa_status_t psa_aead_verify(psa_aead_operation_t *operation,
                             uint8_t *plaintext,
                             size_t plaintext_size,
                             size_t *plaintext_length,
                             const uint8_t *tag,
                             size_t tag_length)
{
    psa_status_t status;

    status = PSA_ERROR_NOT_SUPPORTED;

    return status;
}

__attribute__((section("SFN")))
psa_status_t psa_aead_abort(psa_aead_operation_t *operation)
{
    psa_status_t status;

    status = PSA_ERROR_NOT_SUPPORTED;

    return status;
}

__attribute__((section("SFN")))
psa_status_t psa_mac_compute(psa_key_handle_t handle,
                             psa_algorithm_t alg,
                             const uint8_t *input,
                             size_t input_length,
                             uint8_t *mac,
                             size_t mac_size,
                             size_t *mac_length)
{
    psa_status_t status;

    status = PSA_ERROR_NOT_SUPPORTED;

    return status;
}

__attribute__((section("SFN")))
psa_status_t psa_mac_verify(psa_key_handle_t handle,
                            psa_algorithm_t alg,
                            const uint8_t *input,
                            size_t input_length,
                            const uint8_t *mac,
                            const size_t mac_length)
{
    psa_status_t status;

    status = PSA_ERROR_NOT_SUPPORTED;

    return status;
}

__attribute__((section("SFN")))
psa_status_t psa_cipher_encrypt(psa_key_handle_t handle,
                                psa_algorithm_t alg,
                                const uint8_t *input,
                                size_t input_length,
                                uint8_t *output,
                                size_t output_size,
                                size_t *output_length)
{
    psa_status_t status;

    status = PSA_ERROR_NOT_SUPPORTED;

    return status;
}

__attribute__((section("SFN")))
psa_status_t psa_cipher_decrypt(psa_key_handle_t handle,
                                psa_algorithm_t alg,
                                const uint8_t *input,
                                size_t input_length,
                                uint8_t *output,
                                size_t output_size,
                                size_t *output_length)
{
    psa_status_t status;

    status = PSA_ERROR_NOT_SUPPORTED;

    return status;
}

__attribute__((section("SFN")))
psa_status_t psa_raw_key_agreement(psa_algorithm_t alg,
                                   psa_key_handle_t private_key,
                                   const uint8_t *peer_key,
                                   size_t peer_key_length,
                                   uint8_t *output,
                                   size_t output_size,
                                   size_t *output_length)
{
#ifdef TFM_CRYPTO_GENERATOR_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_RAW_KEY_AGREEMENT_SID,
        .alg = alg,
        .key_handle = private_key
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = peer_key, .len = peer_key_length},
    };

    psa_outvec out_vec[] = {
        {.base = output, .len = output_size},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif
    status = API_DISPATCH(tfm_crypto_raw_key_agreement,
                          TFM_CRYPTO_RAW_KEY_AGREEMENT);

    *output_length = out_vec[0].len;

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_GENERATOR_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_key_derivation_setup(psa_key_derivation_operation_t *operation,
                                      psa_algorithm_t alg)
{
#ifdef TFM_CRYPTO_GENERATOR_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_KEY_DERIVATION_SETUP_SID,
        .alg = alg,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_key_derivation_setup,
                          TFM_CRYPTO_KEY_DERIVATION_SETUP);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_GENERATOR_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_key_derivation_set_capacity(
                                      psa_key_derivation_operation_t *operation,
                                      size_t capacity)
{
#ifdef TFM_CRYPTO_GENERATOR_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_KEY_DERIVATION_SET_CAPACITY_SID,
        .capacity = capacity,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH_NO_OUTVEC(tfm_crypto_key_derivation_set_capacity,
                                    TFM_CRYPTO_KEY_DERIVATION_SET_CAPACITY);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_GENERATOR_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_key_derivation_input_bytes(
                                      psa_key_derivation_operation_t *operation,
                                      psa_key_derivation_step_t step,
                                      const uint8_t *data,
                                      size_t data_length)
{
#if (TFM_CRYPTO_GENERATOR_MODULE_DISABLED != 0)
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_KEY_DERIVATION_INPUT_BYTES_SID,
        .step = step,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = data, .len = data_length},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH_NO_OUTVEC(tfm_crypto_key_derivation_input_bytes,
                                    TFM_CRYPTO_KEY_DERIVATION_INPUT_BYTES);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_GENERATOR_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_key_derivation_output_key(
                                      const psa_key_attributes_t *attributes,
                                      psa_key_derivation_operation_t *operation,
                                      psa_key_handle_t *handle)
{
#if (TFM_CRYPTO_GENERATOR_MODULE_DISABLED != 0)
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_KEY_DERIVATION_OUTPUT_KEY_SID,
        .op_handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = attributes, .len = sizeof(psa_key_attributes_t)},
    };

    psa_outvec out_vec[] = {
        {.base = handle, .len = sizeof(psa_key_handle_t)}
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_key_derivation_output_key,
                          TFM_CRYPTO_KEY_DERIVATION_OUTPUT_KEY);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
#endif /* TFM_CRYPTO_GENERATOR_MODULE_DISABLED */
}

__attribute__((section("SFN")))
psa_status_t psa_hash_compute(psa_algorithm_t alg,
                              const uint8_t *input,
                              size_t input_length,
                              uint8_t *hash,
                              size_t hash_size,
                              size_t *hash_length)
{
    psa_status_t status;

    status = PSA_ERROR_NOT_SUPPORTED;

    return status;
}

__attribute__((section("SFN")))
psa_status_t psa_aead_encrypt_setup(psa_aead_operation_t *operation,
                                    psa_key_handle_t handle,
                                    psa_algorithm_t alg)
{
    psa_status_t status;

    status = PSA_ERROR_NOT_SUPPORTED;

    return status;
}

__attribute__((section("SFN")))
psa_status_t psa_aead_decrypt_setup(psa_aead_operation_t *operation,
                                    psa_key_handle_t handle,
                                    psa_algorithm_t alg)
{
    psa_status_t status;

    status = PSA_ERROR_NOT_SUPPORTED;

    return status;
}

__attribute__((section("SFN")))
psa_status_t psa_aead_generate_nonce(psa_aead_operation_t *operation,
                                     uint8_t *nonce,
                                     size_t nonce_size,
                                     size_t *nonce_length)
{
    psa_status_t status;

    status = PSA_ERROR_NOT_SUPPORTED;

    return status;
}

__attribute__((section("SFN")))
psa_status_t psa_aead_set_nonce(psa_aead_operation_t *operation,
                                const uint8_t *nonce,
                                size_t nonce_length)
{
    psa_status_t status;

    status = PSA_ERROR_NOT_SUPPORTED;

    return status;
}

__attribute__((section("SFN")))
psa_status_t psa_aead_set_lengths(psa_aead_operation_t *operation,
                                  size_t ad_length,
                                  size_t plaintext_length)
{
    psa_status_t status;

    status = PSA_ERROR_NOT_SUPPORTED;

    return status;
}

__attribute__((section("SFN")))
psa_status_t psa_aead_update(psa_aead_operation_t *operation,
                             const uint8_t *input,
                             size_t input_length,
                             uint8_t *output,
                             size_t output_size,
                             size_t *output_length)
{
    psa_status_t status;

    status = PSA_ERROR_NOT_SUPPORTED;

    return status;
}
