/***************************************************************************//**
 * @file
 * @brief HMAC (keyed-hash message authentication code) API.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef SILABS_APP_UTIL_SECURE_EZSP_HMAC_H
#define SILABS_APP_UTIL_SECURE_EZSP_HMAC_H

/*
 * Keyed hash function for message authentication
 *
 * This is HMAC (see FIPS PUB 198) using the AES hash.  HMAC is a
 * method for computing a hash from a key and a data message using
 * a message-only hash function.
 */
void emberHmacAesHash(const uint8_t *key,
                      const uint8_t *data,
                      uint8_t dataLength,
                      uint8_t *result);

#endif // SILABS_APP_UTIL_SECURE_EZSP_HMAC_H
