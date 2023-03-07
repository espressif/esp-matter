/**
 * \file
 * \brief Implementation of the PBKDF2 algorithm for use in generating password
 * hashes.
 *
 * \copyright (c) 2015-2020 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#include "cryptoauthlib.h"

/** \brief Calculate a PBKDF2 hash of a given password and salt
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_pbkdf2_sha256(
    const uint32_t  iter,           /**< [in] Number of iterations of the algorithm to perform */
    const uint8_t * password,       /**< [in] Password to hash */
    const size_t    password_len,   /**< [in] Length of the password bytes buffer */
    const uint8_t * salt,           /**< [in] Salt bytes to use */
    const size_t    salt_len,       /**< [in] Length of the salt bytes buffer */
    uint8_t *       result,         /**< [out] Output buffer to hold the derived key */
    size_t          result_len      /**< [in] Length of the key to derive */
    )
{
    ATCA_STATUS status = ATCA_BAD_PARAM;
    atcac_hmac_sha256_ctx ctx;
    uint32_t i, j;
    uint32_t counter = 1;
    uint8_t temp1_digest[ATCA_SHA256_DIGEST_SIZE];
    uint8_t temp2_digest[ATCA_SHA256_DIGEST_SIZE];

    for (; 0 < result_len; counter++)
    {
        size_t temp_size = ATCA_SHA256_DIGEST_SIZE;
        uint32_t temp_u32;

        if (ATCA_SUCCESS != (status = atcac_sha256_hmac_init(&ctx, password, (uint8_t)password_len)))
        {
            break;
        }

        if (ATCA_SUCCESS != (status = atcac_sha256_hmac_update(&ctx, salt, salt_len)))
        {
            break;
        }

        temp_u32 = ATCA_UINT32_HOST_TO_BE(counter);
        if (ATCA_SUCCESS != (status = atcac_sha256_hmac_update(&ctx, (uint8_t*)&temp_u32, 4)))
        {
            break;
        }

        if (ATCA_SUCCESS != (status = atcac_sha256_hmac_finish(&ctx, temp1_digest, &temp_size)))
        {
            break;
        }

        memcpy(temp2_digest, temp1_digest, ATCA_SHA256_DIGEST_SIZE);

        for (i = 1; i < iter; i++)
        {
            if (ATCA_SUCCESS != (status = atcac_sha256_hmac_init(&ctx, password, (uint8_t)password_len)))
            {
                break;
            }

            if (ATCA_SUCCESS != (status = atcac_sha256_hmac_update(&ctx, temp2_digest, ATCA_SHA256_DIGEST_SIZE)))
            {
                break;
            }

            if (ATCA_SUCCESS != (status = atcac_sha256_hmac_finish(&ctx, temp2_digest, &temp_size)))
            {
                break;
            }

            for (j = 0; j < ATCA_SHA256_DIGEST_SIZE; j++)
            {
                temp1_digest[j] ^= temp2_digest[j];
            }
        }

        if (ATCA_SUCCESS == status)
        {
            size_t copy_len = (result_len < ATCA_SHA256_DIGEST_SIZE) ? result_len : ATCA_SHA256_DIGEST_SIZE;
            memcpy(result, temp1_digest, copy_len);

            result_len -= copy_len;
            result += copy_len;
        }
    }
    return status;
}

/** \brief Calculate a PBKDF2 password hash using a stored key inside a device. The key length is
 *  determined by the device being used. ECCx08: 32 bytes, TA100: 16-64 bytes
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_pbkdf2_sha256_ext(
    ATCADevice     device,              /**< [in] Device context pointer */
    const uint32_t iter,                /**< [in] Number of iterations of the algorithm to perform */
    const uint16_t slot,                /**< [in] Slot/handle with a stored key (password) */
    const uint8_t* salt,                /**< [in] Salt bytes to use */
    const size_t   salt_len,            /**< [in] Length of the salt bytes buffer */
    uint8_t*       result,              /**< [out] Output buffer to hold the derived key */
    size_t         result_len           /**< [in] Length of the key to derive */
    )
{
    ATCA_STATUS status = ATCA_BAD_PARAM;
    uint32_t i, j;
    uint32_t counter = 1;
    uint8_t temp1_digest[ATCA_SHA256_DIGEST_SIZE];
    uint8_t temp2_digest[ATCA_SHA256_DIGEST_SIZE];
    uint8_t message[ATCA_SHA256_BLOCK_SIZE];

    for (; 0 < result_len; counter++)
    {
        uint32_t temp_u32;
        size_t msg_len;

        temp_u32 = ATCA_UINT32_HOST_TO_BE(counter);

        memcpy(message, salt, salt_len);
        memcpy(&message[salt_len], &temp_u32, 4);
        msg_len = salt_len + 4;

        if (ATCA_SUCCESS != (status = ATCA_TRACE(atcab_sha_hmac_ext(device, message, msg_len, slot, temp1_digest, 0), "")))
        {
            break;
        }

        memcpy(temp2_digest, temp1_digest, ATCA_SHA256_DIGEST_SIZE);

        for (i = 1; i < iter; i++)
        {
            if (ATCA_SUCCESS != (status = ATCA_TRACE(atcab_sha_hmac_ext(device, temp2_digest, ATCA_SHA256_DIGEST_SIZE, slot, temp2_digest, 0), "")))
            {
                break;
            }

            for (j = 0; j < ATCA_SHA256_DIGEST_SIZE; j++)
            {
                temp1_digest[j] ^= temp2_digest[j];
            }
        }

        if (ATCA_SUCCESS == status)
        {
            size_t copy_len = (result_len < ATCA_SHA256_DIGEST_SIZE) ? result_len : ATCA_SHA256_DIGEST_SIZE;
            memcpy(result, temp1_digest, copy_len);

            result_len -= copy_len;
            result += copy_len;
        }
    }
    return status;
}

/** \brief Calculate a PBKDF2 password hash using a stored key inside a device. The key length is
 *  determined by the device being used. ECCx08: 32 bytes, TA100: 16-64 bytes
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_pbkdf2_sha256(
    const uint32_t iter,                /**< [in] Number of iterations of the algorithm to perform */
    const uint16_t slot,                /**< [in] Slot/handle with a stored key (password) */
    const uint8_t* salt,                /**< [in] Salt bytes to use */
    const size_t   salt_len,            /**< [in] Length of the salt bytes buffer */
    uint8_t*       result,              /**< [out] Output buffer to hold the derived key */
    size_t         result_len           /**< [in] Length of the key to derive */
    )
{
    return atcab_pbkdf2_sha256_ext(atcab_get_device(), iter, slot, salt, salt_len, result, result_len);
}
