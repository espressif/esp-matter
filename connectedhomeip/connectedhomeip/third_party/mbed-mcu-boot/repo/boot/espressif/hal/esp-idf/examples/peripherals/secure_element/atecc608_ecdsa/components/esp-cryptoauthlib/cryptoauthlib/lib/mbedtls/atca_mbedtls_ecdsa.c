/**
 * \brief Replace mbedTLS ECDSA Functions with hardware acceleration &
 * hardware key security
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

/* mbedTLS boilerplate includes */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_ECDSA_C)

/* Cryptoauthlib Includes */
#include "cryptoauthlib.h"
#include "atca_basic.h"
#include <string.h>

#include "mbedtls/atca_mbedtls_wrap.h"
#include "mbedtls/ecdsa.h"


int atca_mbedtls_ecdsa_sign(const mbedtls_mpi* d, mbedtls_mpi* r, mbedtls_mpi* s,
                            const unsigned char* msg, size_t msg_len)
{
    int ret = 0;

    if (d && r && s && msg && ATCA_SHA256_DIGEST_SIZE <= msg_len)
    {
        atca_mbedtls_eckey_t key_info;
        uint8_t raw_sig[ATCA_ECCP256_SIG_SIZE];

        ret = mbedtls_mpi_write_binary(d, (unsigned char*)&key_info, sizeof(atca_mbedtls_eckey_t));

        if (!ret)
        {
            if (ATCA_SUCCESS != atcab_sign_ext(key_info.device, key_info.handle, msg, raw_sig))
            {
                ret = -1;
            }
        }

        if (!ret)
        {
            ret = mbedtls_mpi_read_binary(r, raw_sig, ATCA_ECCP256_SIG_SIZE / 2);
        }

        if (!ret)
        {
            ret = mbedtls_mpi_read_binary(s, &raw_sig[ATCA_ECCP256_SIG_SIZE / 2], ATCA_ECCP256_SIG_SIZE / 2);
        }
    }

    return ret;
}


#ifdef MBEDTLS_ECDSA_SIGN_ALT
/*
 * Compute ECDSA signature of a hashed message (SEC1 4.1.3)
 * Obviously, compared to SEC1 4.1.3, we skip step 4 (hash message)
 */
int mbedtls_ecdsa_sign(mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                       const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                       int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
    ((void)grp);
    ((void)f_rng);
    ((void)p_rng);

    return atca_mbedtls_ecdsa_sign(d, r, s, buf, blen);
}
#endif

#ifdef MBEDTLS_ECDSA_VERIFY_ALT
/*
 * Verify ECDSA signature of hashed message
 */
int mbedtls_ecdsa_verify(mbedtls_ecp_group *grp,
                         const unsigned char *buf, size_t blen,
                         const mbedtls_ecp_point *Q,
                         const mbedtls_mpi *r,
                         const mbedtls_mpi *s)
{
    int ret = 0;
    uint8_t raw_sig[ATCA_SIG_SIZE];
    bool verified = false;

    if (!grp || !buf || !Q || !r || !s)
    {
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (grp->id != MBEDTLS_ECP_DP_SECP256R1)
    {
        ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    /* Convert the signature to binary */
    if (!ret)
    {
        ret = mbedtls_mpi_write_binary(r, raw_sig, ATCA_SIG_SIZE / 2);
    }
    if (!ret)
    {
        ret = mbedtls_mpi_write_binary(s, &raw_sig[ATCA_SIG_SIZE / 2], ATCA_SIG_SIZE / 2);
    }

    if (Q->Z.n == 1)
    {
        uint8_t public_key[ATCA_PUB_KEY_SIZE];

        /* Convert the public key to it's uncompressed binary */
        if (!ret)
        {
            ret = mbedtls_mpi_write_binary(&(Q->X), public_key, ATCA_PUB_KEY_SIZE / 2);
        }
        if (!ret)
        {
            ret = mbedtls_mpi_write_binary(&(Q->Y), &public_key[ATCA_PUB_KEY_SIZE / 2], ATCA_PUB_KEY_SIZE / 2);
        }

        if (!ret)
        {
            ret = atcab_verify_extern(buf, raw_sig, public_key, &verified);

            if (!ret && !verified)
            {
                ret = MBEDTLS_ERR_ECP_VERIFY_FAILED;
            }
        }
    }
    else
    {
        atca_mbedtls_eckey_t key_info;
        if (!ret)
        {
            ret = mbedtls_mpi_write_binary(&Q->Z, (unsigned char*)&key_info, sizeof(atca_mbedtls_eckey_t));
        }

        if (!ret)
        {
            ret = calib_verify_stored(key_info.device, buf, raw_sig, key_info.handle, &verified);

            if (!ret && !verified)
            {
                ret = MBEDTLS_ERR_ECP_VERIFY_FAILED;
            }
        }
    }

    return ret;
}
#endif /* !MBEDTLS_ECDSA_VERIFY_ALT */

#endif /* MBEDTLS_ECDSA_C */
