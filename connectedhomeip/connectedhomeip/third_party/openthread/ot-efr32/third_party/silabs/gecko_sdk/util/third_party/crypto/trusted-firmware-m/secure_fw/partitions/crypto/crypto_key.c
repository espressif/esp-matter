/*
 * Copyright (c) 2018-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include <stdint.h>

#include "tfm_mbedcrypto_include.h"

#include "tfm_crypto_api.h"
#include "tfm_crypto_defs.h"
#include "tfm_crypto_private.h"

/*!
 * \defgroup public Public functions
 *
 */
/*!@{*/
psa_status_t tfm_crypto_key_attributes_from_client(
                    const struct psa_client_key_attributes_s *client_key_attr,
                    int32_t client_id,
                    psa_key_attributes_t *key_attributes)
{
    psa_core_key_attributes_t *core;
#if !defined(MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER)
    (void) client_id;
#endif

    if (client_key_attr == NULL || key_attributes == NULL) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    *key_attributes = psa_key_attributes_init();
    core = &(key_attributes->MBEDTLS_PRIVATE(core));

    /* Copy core key attributes from the client core key attributes */
    core->MBEDTLS_PRIVATE(type) = client_key_attr->type;
    core->MBEDTLS_PRIVATE(lifetime) = client_key_attr->lifetime;
    core->MBEDTLS_PRIVATE(policy).MBEDTLS_PRIVATE(usage) =
                                                     client_key_attr->usage;
    core->MBEDTLS_PRIVATE(policy).MBEDTLS_PRIVATE(alg) =
                                                     client_key_attr->alg;
    core->MBEDTLS_PRIVATE(policy).MBEDTLS_PRIVATE(alg2) =
                                                     client_key_attr->alg2;
    core->MBEDTLS_PRIVATE(bits) = client_key_attr->bits;

    /* Use the client key id as the key_id and its partition id as the owner */
#if defined(MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER)
    core->MBEDTLS_PRIVATE(id).MBEDTLS_PRIVATE(key_id) = client_key_attr->id;
    core->MBEDTLS_PRIVATE(id).MBEDTLS_PRIVATE(owner) = client_id;
#else
    core->MBEDTLS_PRIVATE(id) = client_key_attr->id;
#endif
    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_key_attributes_to_client(
                        const psa_key_attributes_t *key_attributes,
                        struct psa_client_key_attributes_s *client_key_attr)
{
    if (client_key_attr == NULL || key_attributes == NULL) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    struct psa_client_key_attributes_s v = PSA_CLIENT_KEY_ATTRIBUTES_INIT;
    *client_key_attr = v;
    psa_core_key_attributes_t core = key_attributes->MBEDTLS_PRIVATE(core);

    /* Copy core key attributes from the client core key attributes */
    client_key_attr->type = core.MBEDTLS_PRIVATE(type);
    client_key_attr->lifetime = core.MBEDTLS_PRIVATE(lifetime);
    client_key_attr->usage = core.MBEDTLS_PRIVATE(policy).MBEDTLS_PRIVATE(usage);
    client_key_attr->alg = core.MBEDTLS_PRIVATE(policy).MBEDTLS_PRIVATE(alg);
    client_key_attr->alg2 = core.MBEDTLS_PRIVATE(policy).MBEDTLS_PRIVATE(alg2);
    client_key_attr->bits = core.MBEDTLS_PRIVATE(bits);

    /* Return the key_id as the client key id, do not return the owner */
#if defined(MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER)
    client_key_attr->id = core.MBEDTLS_PRIVATE(id).MBEDTLS_PRIVATE(key_id);
#else
    client_key_attr->id = core.MBEDTLS_PRIVATE(id);
#endif

    return PSA_SUCCESS;
}

#if defined(MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER)

psa_status_t tfm_crypto_encode_id_and_owner(psa_key_id_t key_id,
                                            mbedtls_svc_key_id_t *enc_key_ptr)
{
    int32_t partition_id = 0;
    psa_status_t status = tfm_crypto_get_caller_id(&partition_id);

    if (status != PSA_SUCCESS) {
        return status;
    }

    /* If Null Pointer, return PSA_ERROR_PROGRAMMER_ERROR */
    if (enc_key_ptr == NULL) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    /* Use the client key id as the key_id and its partition id as the owner */
    *enc_key_ptr = mbedtls_svc_key_id_make(partition_id, key_id);

    return PSA_SUCCESS;
}

#else // MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER

psa_status_t tfm_crypto_encode_id_and_owner(psa_key_id_t key_id,
                                            mbedtls_svc_key_id_t *enc_key_ptr)
{
    /*
       TODO:
       Consider to remove or guard calls to tfm_crypto_encode_id_and_owner() when
       MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER is not defined. For now we use this
       replacement function which just copies the key, in order to avoid many
       patches in the crypto specific veneer functions.
       The following ticket should further address the code size of different
       IOVEC approaches and the conclusion may require a new set of veneer functions
       that will not call tfm_crypto_encode_id_and_owner.
       https://jira.silabs.com/browse/PSEC-3329
    */
    *enc_key_ptr = key_id;

    return PSA_SUCCESS;
}

#endif // MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER

/*!@}*/
