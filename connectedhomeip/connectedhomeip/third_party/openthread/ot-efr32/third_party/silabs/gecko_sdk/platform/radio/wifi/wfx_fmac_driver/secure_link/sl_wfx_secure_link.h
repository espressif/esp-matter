/**************************************************************************//**
 * Copyright 2022, Silicon Laboratories Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

/*
 *  All the routines the underlying platform needs to define to port wf200 driver
 */

#ifndef SL_WFX_SECURE_LINK_H
#define SL_WFX_SECURE_LINK_H

#include "sl_wfx.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef SL_WFX_USE_SECURE_LINK

void sl_wfx_init_secure_link_encryption_bitmap(uint8_t *bitmap);

sl_status_t sl_wfx_secure_link_set_mac_key(const uint8_t *sl_mac_key, sl_wfx_securelink_mac_key_dest_t destination);

sl_status_t sl_wfx_secure_link_exchange_keys(const uint8_t *sl_mac_key, uint8_t *sl_host_pub_key);

sl_status_t sl_wfx_secure_link_renegotiate_session_key(void);

sl_status_t sl_wfx_secure_link_configure(const uint8_t *encryption_bitmap, uint8_t disable_session_key_protection);

sl_status_t sl_wfx_secure_link_send(uint8_t command_id, sl_wfx_generic_message_t **request, uint16_t *request_length);

sl_status_t sl_wfx_secure_link_receive(sl_wfx_generic_message_t **network_rx_buffer, uint32_t read_length);

sl_status_t sl_wfx_secure_link_renegotiate(void);

void sl_wfx_secure_link_bitmap_set_all_encrypted(uint8_t *bitmap);

void sl_wfx_secure_link_bitmap_add_request_id(uint8_t *bitmap, uint8_t request_id);

void sl_wfx_secure_link_bitmap_remove_request_id(uint8_t *bitmap, uint8_t request_id);

uint8_t sl_wfx_secure_link_encryption_required_get(uint8_t request_id);

#endif //SL_WFX_USE_SECURE_LINK

#ifdef __cplusplus
} /*extern "C" */
#endif

#endif // SL_WFX_SECURE_LINK_H
