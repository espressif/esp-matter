/***************************************************************************//**
 * @file
 * @brief Silicon Labs TrustZone secure ITS service (secure side).
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "psa/crypto.h"

#if defined(MBEDTLS_PSA_CRYPTO_STORAGE_C)

#include "sli_tz_service_its.h"
#include "psa/crypto_values.h"
#include "tfm_api.h"
#include "tfm_its_req_mngr.h"
#include "psa/internal_trusted_storage.h"

/**
 * \brief Indicates whether ITS has been initialised.
 */

psa_status_t tfm_its_set_req(psa_invec *in_vec, size_t in_len,
                             psa_outvec *out_vec, size_t out_len)
{
    psa_storage_uid_t uid;
    size_t data_length;
    uint8_t *p_data;
    psa_storage_create_flags_t create_flags;
    (void)out_vec;

    if ((in_len != 4) || (out_len != 0)) {
        /* The number of arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    if (in_vec[1].len != sizeof(uid) ||
        in_vec[3].len != sizeof(create_flags)) {
        /* The size of one of the arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    uid = *((psa_storage_uid_t *)in_vec[1].base);

    p_data = (uint8_t *)in_vec[2].base;
    data_length = in_vec[2].len;

    create_flags = *(psa_storage_create_flags_t *)in_vec[3].base;

    return psa_its_set(uid, data_length, p_data, create_flags);
}

psa_status_t tfm_its_get_req(psa_invec *in_vec, size_t in_len,
                             psa_outvec *out_vec, size_t out_len)
{
    psa_storage_uid_t uid;
    size_t data_offset;
    size_t data_size;
    uint8_t *p_data;
    size_t *p_data_length;

    if ((in_len != 3) || (out_len != 1)) {
        /* The number of arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    if (in_vec[1].len != sizeof(uid) ||
        in_vec[2].len != sizeof(data_offset)) {
        /* The size of one of the arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    uid = *((psa_storage_uid_t *)in_vec[1].base);

    data_offset = *(size_t *)in_vec[2].base;

    p_data = (uint8_t *)out_vec[0].base;
    data_size = out_vec[0].len;

    p_data_length = &out_vec[0].len;

    return psa_its_get(uid, data_offset, data_size, p_data, p_data_length);
}

psa_status_t tfm_its_get_info_req(psa_invec *in_vec, size_t in_len,
                                  psa_outvec *out_vec, size_t out_len)
{
    psa_storage_uid_t uid;
    struct psa_storage_info_t *p_info;

    if ((in_len != 2) || (out_len != 1)) {
        /* The number of arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    if (in_vec[1].len != sizeof(uid) ||
        out_vec[0].len != sizeof(*p_info)) {
        /* The size of one of the arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    uid = *((psa_storage_uid_t *)in_vec[1].base);

    p_info = (struct psa_storage_info_t *)out_vec[0].base;

    return psa_its_get_info(uid, p_info);
}

psa_status_t tfm_its_remove_req(psa_invec *in_vec, size_t in_len,
                                psa_outvec *out_vec, size_t out_len)
{
    psa_storage_uid_t uid;

    (void)out_vec;

    if ((in_len != 2) || (out_len != 0)) {
        /* The number of arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    if (in_vec[1].len != sizeof(uid)) {
        /* The input argument size is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    uid = *((psa_storage_uid_t *)in_vec[1].base);

    return psa_its_remove(uid);
}

psa_status_t sli_psa_its_encrypted_req(psa_invec *in_vec, size_t in_len,
                                       psa_outvec *out_vec, size_t out_len)
{
    (void)in_vec;
    (void)out_vec;

    if ((in_len != 1) || (out_len != 0)) {
        /* The number of arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    return sli_psa_its_encrypted();
}

#endif // #if defined(MBEDTLS_PSA_CRYPTO_STORAGE_C)
