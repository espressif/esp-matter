/***************************************************************************//**
 * @file
 * @brief Connect ECDH Key Exchange component configuration header.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

// <<< Use Configuration Wizard in Context Menu >>>

// <h>ECDH Configuration

// <o SL_CONNECT_ECDH_KEY_EXCHANGE_ECC_FAMILY> Key curve
// <PSA_ECC_FAMILY_SECP_R1=> SecpXXXr1
// <PSA_ECC_FAMILY_MONTGOMERY=> Montgomery
// <i> Default: PSA_ECC_FAMILY_SECP_R1
// <i> This option specifies asymmetric curve
#define SL_CONNECT_ECDH_KEY_EXCHANGE_ECC_FAMILY PSA_ECC_FAMILY_SECP_R1

// <h>SecpXXXr1 Configuration

// <o SL_CONNECT_ECDH_KEY_EXCHANGE_CURVE_SECPXXXR1_SIZE> SecpXXXr1 key size
// <SL_CONNECT_ECDH_KEY_EXCHANGE_CURVE_SECPXXXR1_SIZE_192=> 192
// <SL_CONNECT_ECDH_KEY_EXCHANGE_CURVE_SECPXXXR1_SIZE_256=> 256
// <i> Default: SL_CONNECT_ECDH_KEY_EXCHANGE_CURVE_SECPXXXR1_SIZE_256
// <i> This option specifies the key size
#define SL_CONNECT_ECDH_KEY_EXCHANGE_CURVE_SECPXXXR1_SIZE SL_CONNECT_ECDH_KEY_EXCHANGE_CURVE_SECPXXXR1_SIZE_256

// </h>

// <h>Montgomery Configuration

// <o SL_CONNECT_ECDH_KEY_EXCHANGE_CURVE_MONTGOMERY_SIZE> SecpXXXr1 key size
// <SL_CONNECT_ECDH_KEY_EXCHANGE_CURVE_MONTGOMERY_SIZE_CURVE25519=> 255
// <i> Default: SL_CONNECT_ECDH_KEY_EXCHANGE_CURVE_MONTGOMERY_SIZE_CURVE25519
// <i> This option specifies the key size
#define SL_CONNECT_ECDH_KEY_EXCHANGE_CURVE_MONTGOMERY_SIZE SL_CONNECT_ECDH_KEY_EXCHANGE_CURVE_MONTGOMERY_SIZE_CURVE25519

// </h>

// </h>

// <h>Hash Configuration

// <o SL_CONNECT_ECDH_KEY_EXCHANGE_HASH_ALG> Hash algorithm
// <PSA_ALG_MD2=> MD2
// <PSA_ALG_MD4=> MD4
// <PSA_ALG_MD5=> MD5
// <PSA_ALG_RIPEMD160=> RIPEMD-160
// <PSA_ALG_SHA_1=> SHA1
// <PSA_ALG_SHA_224=> SHA-224
// <PSA_ALG_SHA_256=> SHA-256
// <PSA_ALG_SHA_384=> SHA-384
// <PSA_ALG_SHA_512=> SHA-512
// <PSA_ALG_SHA_512_224=> SHA-512/224
// <PSA_ALG_SHA_512_256=> SHA-512/256
// <PSA_ALG_SHA3_224=> SHA3-224
// <PSA_ALG_SHA3_256=> SHA3-256
// <PSA_ALG_SHA3_384=> SHA3-384
// <PSA_ALG_SHA3_512=> SHA3-512
// <PSA_ALG_SHAKE256_512=> SHAKE256/512
// <i> Default: PSA_ALG_SHA_256
// <i> This option specifies the algorithm for hashing
#define SL_CONNECT_ECDH_KEY_EXCHANGE_HASH_ALG PSA_ALG_SHA_256

// </h>

// <<< end of configuration section >>>
