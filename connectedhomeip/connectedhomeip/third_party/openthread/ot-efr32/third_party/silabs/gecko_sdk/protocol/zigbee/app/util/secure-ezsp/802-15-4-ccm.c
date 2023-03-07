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

#include PLATFORM_HEADER

#include "stack/include/ember.h"
#include "hal/hal.h"
#include "aes.h"
#include "802-15-4-ccm.h"

#define SECURITY_BLOCK_SIZE 16
#define MIC_SIZE_MAX 4

// This implements the CCM* encryption algorithm, using AES-128 as the
// underlying block cipher.  AES-128 is essentially a hash function
// controlled by 128-bit (16 byte) keys.  The AES-128 algorithm takes a key
// and a 16-byte block of data and produces a 16-byte block of noise.
//
// The em250 implements AES-128 in hardware.  The hardware can also handle
// some of the other CCM* calculations, but at this point we only use the
// basic AES-128 functionality.
//
// CCM* uses AES-128 for authentication and encryption.  In authentication,
// AES-128 is used create a hash number from a message, called a MIC.
// The MIC is appended to the message before transmission.  The recipient
// hashes the message and verifies that it obtains the same MIC.  Changes
// to either the body of the message or the appended MIC will result in
// the recipient's MIC differing from the appended MIC.
//
// For encryption, AES-128 is used to create a series of pseudo-random
// blocks which are xor-ed with the message to produce the encrypted
// version.  The recipient performs the same operation, which results in
// the original message.  It is important that each message be encrypted
// with a different psuedo-random sequence.  If two messages used the
// same sequence, xor-ing the encrypted messages would produce the xor
// of the unencrypted messages.
//
// Both authentication and encryption begin by encrypting a special block
// of data called a 'nonce' (as in 'for the nonce').  The goal is that each
// use of a particular key will use a different nonce.  The nonce is:
//
// Offset Size
//   0     1    flags
//   1     8    source EUI64
//   9     4    frame counter
//  13     1    control byte
//  14     2    block count
//
// The frame counter is incremented for each message and the block count is
// incremented for each encryption block.  The flags and control byte ensure
// that different operations on the same message, such as MIC generation and
// encryption, do not use the same nonce.

// 802.15.4 and ZigBee use different byte orders for the source EUI64 and
// the frame counter in the nonce.  ZigBee uses the over-the-air order for
// both and 802.15.4 reverses them.  Hence the need for the following.

// Handy macro.  This is unsafe because it duplicates its arguments.
#define min(x, y) ((x) < (y) ? (x) : (y))

//----------------------------------------------------------------
// MIC encryption flags

#define COMMON_MIC_FLAGS \
  (STANDALONE_FLAGS_ADATA_FIELD_NONZERO | STANDALONE_FLAGS_L_FIELD_2_BYTES)

//----------------------------------------------------------------
// Performs an actual nonce encryption, after first setting the fields
// specific to this block.  We do a copy to avoid clobbering the (shared)
// nonce.

#define encryptMicBlock0(nonce, variableField, result) \
  (encryptNonce((nonce), 0xFF, (variableField), (result)))

#define encryptBlock0(nonce, variableField, result) \
  (encryptNonce((nonce), 0x03, (variableField), (result)))

static void encryptNonce(const uint8_t nonce[SECURITY_BLOCK_SIZE],
                         uint8_t flagsMask,
                         uint16_t variableField,
                         uint8_t block[SECURITY_BLOCK_SIZE])
{
  MEMCOPY(block, nonce, SECURITY_BLOCK_SIZE);

  block[STANDALONE_FLAGS_INDEX] &= flagsMask;
  block[STANDALONE_VARIABLE_FIELD_INDEX_HIGH] = HIGH_BYTE(variableField);
  block[STANDALONE_VARIABLE_FIELD_INDEX_LOW] = LOW_BYTE(variableField);

//  simPrint("nonce: %02X %02X %02X]\n", flags, HIGH_BYTE(variableField),
//           LOW_BYTE(variableField));

  emStandAloneEncryptBlock(block);
}

//----------------------------------------------------------------
// This performs the core of the MIC calculation.  'Count' bytes from
// 'bytes' are xor-ed into 'block' and then encrypted.  We start at
// 'blockIndex' in the block.
//
// The final blockIndex is returned.

static uint8_t xorBytesIntoBlock(uint8_t *block,
                                 uint8_t blockIndex,
                                 const uint8_t *bytes,
                                 uint16_t count)
{
  uint16_t i;

//  {
//    fprintf(stderr, "[xor %d [%02X", count, bytes[0]);
//    for (i = 1; i < count; i++)
//      fprintf(stderr, " %02X", bytes[i]);
//    fprintf(stderr, "]]\n");
//  }

  for (i = 0; i < count; ) {
    uint16_t needed = SECURITY_BLOCK_SIZE - blockIndex;
    uint16_t todo = count - i;
    uint16_t copied = min(todo, needed);
    uint16_t j;

    for (j = 0; j < copied; j++, blockIndex++) {
//      fprintf(stderr, "[%02x ^ %02X =", block[blockIndex], *bytes);
      block[blockIndex] ^= *bytes++;
//      fprintf(stderr, " %02x]\n", block[blockIndex]);
    }
    i += copied;

    if (blockIndex == SECURITY_BLOCK_SIZE) {
      emStandAloneEncryptBlock(block);
      blockIndex = 0;
    }
  }
  return blockIndex;
}

// Calculate the MIC by hashing first the authenticated portion of the
// packet and then the encrypted portion (which hasn't been encrypted yet).
//
// The encrypted bytes are processed on a block boundary, so we finish off
// the block at the end of the authenticated bytes.
//
// The 'for' loop goes around two times (authenticated bytes, encrypted bytes).

static void calculateMic(const uint8_t *authenticate,
                         uint16_t authenticateLength,
                         const uint8_t *encrypt,
                         uint16_t encryptLength,
                         const uint8_t nonce[SECURITY_BLOCK_SIZE],
                         uint8_t *micResult,
                         uint16_t micResultLength)
{
  uint8_t encryptionBlock[SECURITY_BLOCK_SIZE];
  uint8_t blockIndex = 2;      // skip over length
  const uint8_t *chunk = authenticate;
  uint16_t chunkLength;
  uint8_t phase;

  chunkLength = authenticateLength;
  encryptMicBlock0(nonce, encryptLength, encryptionBlock);

  // First two bytes are the 16-bit representation of the frame length,
  // high byte first.
  encryptionBlock[0] ^= HIGH_BYTE(authenticateLength);
  encryptionBlock[1] ^= LOW_BYTE(authenticateLength);

  // phase 0: authenticated bytes
  // phase 1: encrypted bytes

  for (phase = 0; phase < 2; phase++) {
    blockIndex = xorBytesIntoBlock(encryptionBlock,
                                   blockIndex,
                                   chunk,
                                   chunkLength);
    chunk = encrypt;
    chunkLength = encryptLength;

    // Finish off authentication if not on a block boundary.
    if (0 < blockIndex) {
//      simPrint("finish %d", blockIndex);
      emStandAloneEncryptBlock(encryptionBlock);
      blockIndex = 0;
    }
  }

  MEMCOPY(micResult, encryptionBlock, micResultLength);
}

// Encrypt the payload by xor-ing it with a series of AES-encrypted nonces.

static void encryptBytes(uint8_t* bytes,
                         uint16_t length,
                         uint16_t blockCount,
                         const uint8_t nonce[SECURITY_BLOCK_SIZE])
{
  uint8_t encryptionBlock[SECURITY_BLOCK_SIZE];

  for (; 0 < length; ) {
    uint8_t todo = min(length, SECURITY_BLOCK_SIZE);
    uint8_t i;

    encryptBlock0(nonce, blockCount, encryptionBlock);
    blockCount += 1;

    for (i = 0; i < todo; i++) {
      *bytes++ ^= encryptionBlock[i];
    }

    length -= todo;
  }
}

// The MIC gets encrypted as block zero of the message.

#define encryptMic(mic, nonce) \
  (encryptBytes((mic), macMicLength, 0, (nonce)))

// The payload gets encrypted starting from block 1.

#define encryptPayload(payload, length, nonce) \
  (encryptBytes((payload), (length), 1, (nonce)))

//----------------------------------------------------------------
// The core encryption function.

void emCcmEncrypt(const uint8_t *nonce,
                  uint8_t *authenticate,
                  uint16_t authenticateLength,
                  uint8_t *encrypt,
                  uint16_t encryptLength,
                  uint8_t *mic,
                  uint8_t packetMicLength)
{
  if (0 < packetMicLength) {
    calculateMic(authenticate,
                 authenticateLength,
                 encrypt,
                 encryptLength,
                 nonce,
                 mic,
                 packetMicLength);
    encryptBytes(mic, packetMicLength, 0, nonce);
  }

  if (0 < encryptLength) {
    encryptPayload(encrypt, encryptLength, nonce);
  }
}

void emCcmEncryptPacket(const uint8_t *nonce,
                        uint8_t *packet,
                        uint16_t authenticateLength,
                        uint16_t encryptLength,
                        uint8_t packetMicLength)
{
  emCcmEncrypt(nonce,
               packet,
               authenticateLength,
               packet + authenticateLength,
               encryptLength,
               packet + authenticateLength + encryptLength,
               packetMicLength);
}

//----------------------------------------------------------------
// Decryption.

// packetLength does not include the MIC.

bool emCcmDecryptPacket(const uint8_t *nonce,
                        uint8_t *packet,
                        uint16_t authenticateLength,
                        uint8_t *encrypt,
                        uint16_t encryptLength,
                        uint8_t packetMicLength)
{
  encryptPayload(encrypt, encryptLength, nonce);

  if (0 < packetMicLength) {
    uint8_t rxMic[MIC_SIZE_MAX];
    uint8_t calcMic[MIC_SIZE_MAX];

    MEMCOPY(rxMic,
            encrypt + encryptLength,
            packetMicLength);
    // encryption is self-reversing
    encryptBytes(rxMic, packetMicLength, 0, nonce);
    calculateMic(packet,
                 authenticateLength,
                 encrypt,
                 encryptLength,
                 nonce,
                 calcMic,
                 packetMicLength);
    return MEMCOMPARE(rxMic, calcMic, packetMicLength) == 0;
  } else {
    return true;
  }
}
