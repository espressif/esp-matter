/***************************************************************************//**
 * @file
 * @brief CCM* as used by 802.15.4 security.
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

// EMZIGBEE-3639 - remove this file once platform references have been updated

// Offsets into the initial block.  The code calls this block the
// nonce.  Strictly speaking, the nonce is the source address, frame
// counter, and security control.  It does not include the flags and
// the variable field (which is used to encode the length of the
// message.

#define STANDALONE_FLAGS_INDEX                   0
#define STANDALONE_NONCE_SOURCE_ADDR_INDEX       1
#define STANDALONE_NONCE_FRAME_COUNTER_INDEX     9
#define STANDALONE_NONCE_SECURITY_CONTROL_INDEX 13
#define STANDALONE_VARIABLE_FIELD_INDEX_HIGH    14
#define STANDALONE_VARIABLE_FIELD_INDEX_LOW     15

// The core encryption function.
void emCcmEncrypt(const uint8_t *nonce,
                  uint8_t *authenticate,
                  uint16_t authenticateLength,
                  uint8_t *encrypt,
                  uint16_t encryptLength,
                  uint8_t *mic,
                  uint8_t packetMicLength);

// // Wrapper used when the authenticated, encrypted, and mic sections are
// // adjacent.
void emCcmEncryptPacket(const uint8_t *nonce,
                        uint8_t *packet,
                        uint16_t authenticateLength,
                        uint16_t encryptLength,
                        uint8_t micLength);

// // Decrypts the packet in place.  The security subframe is removed.
// //
// // Returns true if successful and false otherwise.

bool emCcmDecryptPacket(const uint8_t *nonce,
                        uint8_t *packet,
                        uint16_t authenticateLength,
                        uint8_t *encrypt,
                        uint16_t encryptLength,
                        uint8_t packetMicLength);
