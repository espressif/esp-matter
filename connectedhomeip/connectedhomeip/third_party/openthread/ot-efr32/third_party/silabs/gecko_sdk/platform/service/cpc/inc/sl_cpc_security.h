/***************************************************************************/ /**
 * @file
 * @brief CPC Security Endpoint of the Secondary
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_CPC_SECURITY_SECONDARY_H
#define SL_CPC_SECURITY_SECONDARY_H

#include <stdint.h>
#include "sl_status.h"
#include "sl_enum.h"
#include "psa/crypto.h"
#include "sl_slist.h"
#include "psa_crypto_storage.h"
#include "sl_cpc_security_config.h"

/// The security state enabled bit mask
#define SL_CPC_SECURITY_STATE_ENABLE_MASK (1 << 0)

/// The security state bounded bit mask
#define SL_CPC_SECURITY_STATE_BOUND_MASK  (1 << 1)

// Binding types
#define  SL_CPC_SECURITY_BINDING_KEY_CUSTOMER_SPECIFIC    0x03
#define  SL_CPC_SECURITY_BINDING_KEY_ECDH                 0x02
#define  SL_CPC_SECURITY_BINDING_KEY_PLAINTEXT_SHARE      0x01
#define  SL_CPC_SECURITY_BINDING_KEY_NONE                 0x00

#define SL_CPC_SECURITY_OK_TO_UNBIND 0xAAAAAAAAAAAAAAAA

/***************************************************************************/ /**
 * @addtogroup cpc_security_secondary
 * @brief CPC Security Secondary
 * @details
 * ## Overview
 *
 *   TODO
 *
 * ## Initialization
 *
 *   TODO
 *
 * @{
 ******************************************************************************/

/// @brief Enumeration representing security state.
SL_ENUM_GENERIC(sl_cpc_security_state_t, uint8_t)
{
  SL_CPC_SECURITY_STATE_NOT_READY     = 0x00,         ///< Security is not yet setup, no I-frame allowed
  SL_CPC_SECURITY_STATE_DISABLED      = 0x01,         ///< Security is not enabled, cleartext I-frame are allowed
  SL_CPC_SECURITY_STATE_INITIALIZING  = 0x02,         ///< Security is being initialized, raw I-frame are only allowed on security endpoint
  SL_CPC_SECURITY_STATE_RESETTING     = 0x03,         ///< Security is being reset
  SL_CPC_SECURITY_STATE_WAITING_ON_TX_COMPLETE = 0x4, ///< Response to setup the security session is about to be sent, waiting for write completion
  SL_CPC_SECURITY_STATE_INITIALIZED   = 0x05,         ///< Security is initialized, only encrypted I-frame are allowed
};

/***************************************************************************//**
 * Typedef for the user-supplied callback function, which is called when
 * unbinding (authorized).
 *
 * @param data   User-specific argument.
 ******************************************************************************/
typedef void (*sl_cpc_unbind_notification_t)(void *data);

/// @brief Enumeration representing unbind notification handle.
typedef struct {
  sl_cpc_unbind_notification_t fnct;      ///< Notification Callback
  sl_slist_node_t node;                   ///< Single list node
  void *data;                             ///< User-specific argument
} sl_cpc_unbind_notification_handle_t;

#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************************************//**
 * Get the setup status of the security subsystem.
 *
 * @return The setup status of the security subsystem.
 ******************************************************************************/
sl_cpc_security_state_t sl_cpc_security_get_state(void);

/***************************************************************************//**
 * User callback to provide CPC with a binding key.
 *
 * SL_CPC_SECURITY_BINDING_KEY_METHOD config must be set to
 * SL_CPC_SECURITY_BINDING_KEY_CUSTOMER_SPECIFIC
 *
 * @param[out] key               The encryption key
 * @param[out] key_size          The encryption key size
 *                               Note: Only 16 bytes keys are supported for the moment
 ******************************************************************************/
void sl_cpc_security_fetch_user_specified_binding_key(uint8_t **key, uint16_t *key_size_in_bytes);

/***************************************************************************//**
 * Authorize an unbind request.
 *
 * @note Declared as a weak symbol. If no strong definition is given by the user,
 *       unbind requests are always denied. If the user gives a definition
 *       for this function, its return value will dictate if unbind requests
 *       are accepted.
 *
 * @return Whether to allow unbind or not, use SL_CPC_SECURITY_OK_TO_UNBIND as a return value
 *         to allow. Return anything else to deny.
 ******************************************************************************/
uint64_t sl_cpc_security_on_unbind_request(bool is_link_encrypted);

/***************************************************************************//**
 * Unbind device.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sl_cpc_security_unbind(void);

/***************************************************************************//**
 * Register a callback that will be called when an unbind event occurs.
 *
 * @param[in] handle pointer to the unbind notification handle.
 * @param[in] callback pointer to the unbind notification callback.
 * @param[in] data pointer to pass to the unbind notification callback.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sl_cpc_security_unbind_subscribe(sl_cpc_unbind_notification_handle_t *handle,
                                             sl_cpc_unbind_notification_t callback,
                                             void *data);

/***************************************************************************//**
 * Unregister a callback that will be called when an unbind event occurs.
 *
 * @param[in] handle pointer to the unbind notification handle.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sl_cpc_security_unbind_unsubscribe(sl_cpc_unbind_notification_handle_t *handle);

#ifdef __cplusplus
}
#endif

/** @} (end addtogroup cpc_security_secondary) */

#endif /* SL_CPC_SECURITY_SECONDARY_H */
