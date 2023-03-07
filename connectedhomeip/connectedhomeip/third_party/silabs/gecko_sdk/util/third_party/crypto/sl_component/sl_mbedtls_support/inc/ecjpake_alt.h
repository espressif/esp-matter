/***************************************************************************//**
 * @file
 * @brief Accelerated mbed TLS Elliptic Curve J-PAKE
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
#ifndef ECJPAKE_ALT_H
#define ECJPAKE_ALT_H

/***************************************************************************//**
 * \addtogroup sl_crypto
 * \{
 ******************************************************************************/

/***************************************************************************//**
 * \addtogroup sl_crypto_jpake Accelerated Elliptic Curve J-PAKE
 * \brief Accelerated Elliptic Curve J-PAKE for the mbed TLS API using the SE
 *        peripheral
 *
 * \{
 ******************************************************************************/

#if defined(MBEDTLS_ECJPAKE_ALT)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * EC J-PAKE context structure.
 *
 * J-PAKE is a symmetric protocol, except for the identifiers used in
 * Zero-Knowledge Proofs, and the serialization of the second message
 * (KeyExchange) as defined by the Thread spec.
 *
 * In order to benefit from this symmetry, we choose a different naming
 * convetion from the Thread v1.0 spec. Correspondance is indicated in the
 * description as a pair C: client name, S: server name
 */
typedef struct {
  uint32_t curve_flags;                 /**< Curve flags to use             */
  mbedtls_ecjpake_role role;            /**< Are we client or server?       */
  int point_format;                     /**< Format for point export        */

  char pwd[33];                         /**< J-PAKE password                */
  size_t pwd_len;                       /**< J-PAKE password length         */

  uint8_t r[32];                        /**< Random scalar for exchange     */
  uint8_t Xm1[64];                      /**< Our point 1 (round 1)          */
  uint8_t Xm2[64];                      /**< Our point 2 (round 1)          */
  uint8_t Xp1[64];                      /**< Their point 1 (round 1)        */
  uint8_t Xp2[64];                      /**< Their point 2 (round 1)        */
  uint8_t Xp[64];                       /**< Their point (round 2)          */
} mbedtls_ecjpake_context;

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_ECJPAKE_ALT */

/** \} (end addtogroup sl_crypto_jpake) */
/** \} (end addtogroup sl_crypto) */

#endif /* ECJPAKE_ALT_H */
