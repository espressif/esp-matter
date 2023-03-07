/***************************************************************************//**
 * @file
 * @brief Secure NCP host
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef NCP_SEC_HOST_H
#define NCP_SEC_HOST_H

#include "sl_status.h"

// Optstring argument for getopt.
#define NCP_SEC_OPTSTRING  "s"

// Usage info.
#define NCP_SEC_USAGE " [-s]"

// Options info.
#define NCP_SEC_OPTIONS \
  "    -s  Enable secure communication, default: disabled\n"

#define AES_CCM_KEY_SIZE 16
#define ECDH_PRIVATE_KEY_SIZE 32
#define PUBLIC_KEYPAIR_SIZE (2 * ECDH_PRIVATE_KEY_SIZE)
#define NONCE_SIZE 13
#define MAC_LEN 4
#define IV_SIZE 4

// Security state
typedef enum {
  SECURITY_STATE_UNDEFINED,
  SECURITY_STATE_UNENCRYPTED,
  SECURITY_STATE_INCREASE_SECURITY,
  SECURITY_STATE_ENCRYPTED
}security_state_t;

/**************************************************************************//**
 * Initialize security module.
 * @return  0 on success, -1 on failure.
 *****************************************************************************/
sl_status_t security_init(void);

/**************************************************************************//**
 * Function to reset security state.
 *****************************************************************************/
void security_reset(void);

/**************************************************************************//**
 * Start security.
 *****************************************************************************/
void security_start(void);

/**************************************************************************//**
 * Function to get current security state.
 * @return  security_state_t security state.
 *****************************************************************************/
security_state_t  get_security_state();

/**************************************************************************//**
 * Callback which is called when security state changes.
 * @param[in] state Security State.
 * @note  Override the weak implementation if needed.
 *****************************************************************************/
void security_state_change_cb(security_state_t state);

/**************************************************************************//**
 * Security handshake response.
 * @param[in] public_key Public Key.
 * @param[in] host_iv_to_target Initialization Vector to Target.
 * @param[in] host_iv_to_target Initialization Vector to Host.
 *****************************************************************************/
void security_increase_security_rsp(uint8_t *public_key,
                                    uint8_t *target_iv_to_target,
                                    uint8_t *target_iv_to_host);

/**************************************************************************//**
 * Function to decrypt a message.
 * @param[in] src pointer to ncoming encrypted packet.
 * @param[in] dst pointer to outgoing decrypted packet.
 * @param[in] len Length of data.
 *****************************************************************************/
void security_decrypt(char *src, char *dst, unsigned *len);

/**************************************************************************//**
 * Function to decrypt a packet.
 * @param[in] src pointer to ncoming encrypted packet.
 * @param[in] dst pointer to outgoing decrypted packet.
 * @param[in] len Length of data.
 *****************************************************************************/
void security_decrypt_packet(char *src, char *dst, unsigned *len);

/**************************************************************************//**
 * Function to encrypt a message.
 * @param[in] src pointer to incoming unencrypted packet.
 * @param[in] dst pointer to outgoing encrypted packet.
 * @param[in] len Length of data.
 *****************************************************************************/
void security_encrypt(char *src, char *dst, unsigned *len);

/**************************************************************************//**
 * Function to encrypt a packet.
 * @param[in] src pointer to incoming unencrypted packet.
 * @param[in] dst pointer to outgoing encrypted packet.
 * @param[in] len Length of data.
 *****************************************************************************/
void security_encrypt_packet(char *src, char *dst, unsigned *len);

#endif /* NCP_SEC_HOST_H */
