/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/**********************************************************************************
   Functions in this file are exactly that:  functions, not methods of any class.
   That is, they are stand-alone, utility functions linked in to the executable,
   and available to whomever needs them.
**********************************************************************************/

#include "randomization.hpp"

/**
 * \brief Selects and returns a random key_usage_t value.
 *
 * \details
 *
 * \note
 *
 */
string rand_key_usage (void)
{
    switch (rand() % 6) {
        case 0:  return "PSA_KEY_USAGE_EXPORT";
        case 1:  return "PSA_KEY_USAGE_ENCRYPT";
        case 2:  return "PSA_KEY_USAGE_DECRYPT";
        case 3:  return "PSA_KEY_USAGE_SIGN";
        case 4:  return "PSA_KEY_USAGE_VERIFY";
        case 5:  return "PSA_KEY_USAGE_DERIVE";
    }
    return "";  /* placate compiler */
}

/**
 * \brief Selects and returns a random psa_algorithm_t value.
 *
 * \details
 *
 * \note
 *
 */
/* TODO:  Likely want to make additional versions of these specific for TLS,
   asymmetric, symmetric... */
string rand_key_algorithm (void)
{
    switch (rand() % 47) {
        case  0:  return "PSA_ALG_VENDOR_FLAG";
        case  1:  return "PSA_ALG_CATEGORY_MASK";
        case  2:  return "PSA_ALG_CATEGORY_HASH";
        case  3:  return "PSA_ALG_CATEGORY_MAC";
        case  4:  return "PSA_ALG_CATEGORY_CIPHER";
        case  5:  return "PSA_ALG_CATEGORY_AEAD";
        case  6:  return "PSA_ALG_CATEGORY_SIGN";
        case  7:  return "PSA_ALG_CATEGORY_ASYMMETRIC_ENCRYPTION";
        case  8:  return "PSA_ALG_CATEGORY_KEY_AGREEMENT";
        case  9:  return "PSA_ALG_CATEGORY_KEY_DERIVATION";
        case 10:  return "PSA_ALG_CATEGORY_KEY_SELECTION";
        case 11:  return "PSA_ALG_KEY_SELECTION_FLAG";
        case 12:  return "PSA_ALG_HASH_MASK";
        case 13:  return "PSA_ALG_MD2";
        case 14:  return "PSA_ALG_MD4";
        case 15:  return "PSA_ALG_MD5";
        case 16:  return "PSA_ALG_RIPEMD160";
        case 17:  return "PSA_ALG_SHA_1";
        case 18:  return "PSA_ALG_SHA_224";
        case 19:  return "PSA_ALG_SHA_256";
        case 20:  return "PSA_ALG_SHA_384";
        case 21:  return "PSA_ALG_SHA_512";
        case 22:  return "PSA_ALG_SHA_512_224";
        case 23:  return "PSA_ALG_SHA_512_256";
        case 24:  return "PSA_ALG_SHA3_224";
        case 25:  return "PSA_ALG_SHA3_256";
        case 26:  return "PSA_ALG_SHA3_384";
        case 27:  return "PSA_ALG_SHA3_512";
        case 28:  return "PSA_ALG_ANY_HASH";
        case 29:  return "PSA_ALG_MAC_SUBCATEGORY_MASK";
        case 30:  return "PSA_ALG_HMAC_BASE";
        case 31:  return "PSA_ALG_MAC_TRUNCATION_MASK";
        case 32:  return "PSA_ALG_CIPHER_MAC_BASE";
        case 33:  return "PSA_ALG_CBC_MAC";
        case 34:  return "PSA_ALG_CMAC";
        case 35:  return "PSA_ALG_GMAC";
        case 36:  return "PSA_ALG_CIPHER_STREAM_FLAG";
        case 37:  return "PSA_ALG_CIPHER_FROM_BLOCK_FLAG";
        case 38:  return "PSA_ALG_ARC4";
        case 39:  return "PSA_ALG_CTR";
        case 40:  return "PSA_ALG_CFB";
        case 41:  return "PSA_ALG_OFB";
        case 42:  return "PSA_ALG_XTS";
        case 43:  return "PSA_ALG_CBC_NO_PADDING";
        case 44:  return "PSA_ALG_CBC_PKCS7";
        case 45:  return "PSA_ALG_CCM";
        case 46:  return "PSA_ALG_GCM";
    }
    return "";  /* placate compiler */
}


/**
 * \brief Selects and returns a random psa_key_type_t value.
 *
 * \details
 *
 * \note
 *
 */
string rand_key_type (void)
{
    switch (rand() % 22) {
        case 0:  return "PSA_KEY_TYPE_NONE";
        case 1:  return "PSA_KEY_TYPE_VENDOR_FLAG";
        case 2:  return "PSA_KEY_TYPE_CATEGORY_MASK";
        case 3:  return "PSA_KEY_TYPE_CATEGORY_SYMMETRIC";
        case 4:  return "PSA_KEY_TYPE_CATEGORY_RAW";
        case 5:  return "PSA_KEY_TYPE_CATEGORY_PUBLIC_KEY";
        case 6:  return "PSA_KEY_TYPE_CATEGORY_KEY_PAIR";
        case 7:  return "PSA_KEY_TYPE_CATEGORY_FLAG_PAIR";
        case 8:  return "PSA_KEY_TYPE_RAW_DATA";
        case 9:  return "PSA_KEY_TYPE_HMAC";
        case 10:  return "PSA_KEY_TYPE_DERIVE";
        case 11:  return "PSA_KEY_TYPE_AES";
        case 12:  return "PSA_KEY_TYPE_DES";
        case 13:  return "PSA_KEY_TYPE_CAMELLIA";
        case 14:  return "PSA_KEY_TYPE_ARC4";
        case 15:  return "PSA_KEY_TYPE_RSA_PUBLIC_KEY";
        case 16:  return "PSA_KEY_TYPE_RSA_KEYPAIR";
        case 17:  return "PSA_KEY_TYPE_DSA_PUBLIC_KEY";
        case 18:  return "PSA_KEY_TYPE_DSA_KEYPAIR";
        case 19:  return "PSA_KEY_TYPE_ECC_PUBLIC_KEY_BASE";
        case 20:  return "PSA_KEY_TYPE_ECC_KEYPAIR_BASE";
        case 21:  return "PSA_KEY_TYPE_ECC_CURVE_MASK";
        default:  return "";
    }
    return "";  /* placate compiler */
}

