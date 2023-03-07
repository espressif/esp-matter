/***************************************************************************/ /**
 * @file
 * @brief CPC Security Endpoint of the Secondary
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

#ifndef SLI_CPC_SECURITY_SECONDARY_H
#define SLI_CPC_SECURITY_SECONDARY_H

#include "sl_cpc_security.h"

#define SLI_CPC_SECURITY_PROTOCOL_BINDING_TYPE_PLAINTEXT 0x00
#define SLI_CPC_SECURITY_PROTOCOL_BINDING_TYPE_ECDH      0x01

#define SLI_CPC_SECURITY_PROTOCOL_RESPONSE_MASK 0x8000

#define SLI_SECURITY_BINDING_KEY_LENGTH_BYTES         16
#define SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES          32
#define SLI_SECURITY_SESSION_KEY_LENGTH_BYTES         32
#define SLI_SECURITY_SESSION_ID_LENGTH_BYTES          7
#define SLI_SECURITY_SESSION_INIT_RANDOM_LENGTH_BYTES 64
#define SLI_SECURITY_SHA256_LENGTH_BYTES              32
#define SLI_SECURITY_TAG_LENGTH_BYTES                 8
#define SLI_SECURITY_NONCE_FRAME_COUNTER_MAX_VALUE    (1UL << 29)

#define SLI_SECURITY_PROTOCOL_PAYLOAD_MAX_LENGTH (sizeof(sl_status_t) + SLI_SECURITY_SESSION_INIT_RANDOM_LENGTH_BYTES)
#define SLI_SECURITY_PROTOCOL_HEADER_LENGTH (sizeof(uint16_t) + sizeof(sli_cpc_security_id_t))

SL_ENUM_GENERIC(sli_cpc_security_id_t, uint16_t)
{
  BINDING_REQUEST_ID       = 0x0001,
  PLAIN_TEXT_KEY_SHARE_ID  = 0x0002,
  PUBLIC_KEY_SHARE_ID      = 0x0003,
  SESSION_INIT_ID          = 0x0004,
  UNBIND_REQUEST_ID        = 0x0005
};

SL_ENUM_GENERIC(sli_cpc_security_binding_request_t, uint8_t)
{
  PLAIN_TEXT_KEY_SHARE_BINDING_REQUEST = 0x00,
  ECDH_BINDING_REQUEST = 0x01
};

typedef struct {
  uint16_t request_len;
  uint16_t response_len;
  sli_cpc_security_id_t command_id;
} sli_cpc_security_protocol_cmd_info_t;

typedef __PACKED_STRUCT {
  uint16_t len;
  sli_cpc_security_id_t command_id;
  uint8_t payload[SLI_SECURITY_PROTOCOL_PAYLOAD_MAX_LENGTH];
} sli_cpc_security_protocol_cmd_t;

typedef __PACKED_STRUCT {
  sl_status_t status;
  uint8_t random2[SLI_SECURITY_SESSION_INIT_RANDOM_LENGTH_BYTES];
} sli_cpc_security_session_init_response_t;

typedef __PACKED_STRUCT {
  uint8_t endpoint_id;
  uint8_t session_id[7];
  uint32_t frame_counter;
} sli_cpc_security_nonce_t;

#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************************************/ /**
 * Security endpoint init
 ******************************************************************************/
sl_status_t sli_cpc_security_init(void);

/***************************************************************************/ /**
 * Security endpoint process action
 ******************************************************************************/
void sli_cpc_security_process(void);

/***************************************************************************/ /**
 * Encrypt a message.
 *
 * The security subsystem must be initialized for this function to work
 * properly. Upon success, the content in the payload buffer is replaced with
 * encrypted content.
 *
 * @param[in] header            buffer containing authenticated and non-encrypted data.
 * @param[in] header_len        length of the authenticated data buffer.
 * @param[in,out] payload       buffer containing data to be encrypted.
 * @param[in] payload_len       length of cleartext data in the payload buffer.
 * @param[out] tag              buffer to store the security tag.
 * @param[in] tag_len           length of the security tag buffer.
 ******************************************************************************/
sl_status_t sli_cpc_security_encrypt(const uint8_t address,
                                     const uint8_t *header, const size_t header_len,
                                     uint8_t *paylaod, const size_t payload_len,
                                     uint8_t *tag, const size_t tag_len);

/***************************************************************************/ /**
 * Decrypt a message.
 *
 * The security subsystem must be initialized for this function to work
 * properly.
 *
 * @param[in] header            buffer containing authenticated and non-encrypted data.
 * @param[in] header_len        length of the authenticated data buffer.
 * @param[in,out] payload       buffer containing encrypted data.
 * @param[in] buffer_size       size of payload buffer, to prevent overflow when decrypting.
 * @param[in] payload_len       length of encrypted data in the payload buffer.
 * @param[in,out] output_len    length of the plaintext data upon successful decryption.
 ******************************************************************************/
sl_status_t sli_cpc_security_decrypt(const uint8_t *header, const size_t header_len,
                                     uint8_t *payload, const size_t buffer_size,
                                     const size_t payload_len, size_t *output_len);

/***************************************************************************/ /**
 * Abort a received encrypted message.
 *
 * The security subsystem uses a frame_counter to track the amount of frames sent
 * from the remote. When rejecting a frame, we must still take it into account
 * even if it is not decrypted.
 ******************************************************************************/
void sli_cpc_security_drop_incoming_packet(void);

#ifdef __cplusplus
}
#endif

#endif  // SLI_CPC_SECURITY_SECONDARY_H
