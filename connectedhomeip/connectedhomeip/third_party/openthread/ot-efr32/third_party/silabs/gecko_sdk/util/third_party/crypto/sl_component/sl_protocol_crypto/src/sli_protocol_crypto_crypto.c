/***************************************************************************//**
 * @file
 * @brief Proprietary crypto primitivies optimized for Silicon Labs devices
 *        with a CRYPTO peripheral.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "em_device.h"

#if defined(CRYPTO_PRESENT)

#include "sli_protocol_crypto.h"
#include "crypto_management.h"
#include "em_crypto.h"

// CCM (and CCM-star) implementation optimized for radio protocol usecases
static sl_status_t aes_ccm_radio(bool                encrypt,
                                 const unsigned char *add_data,
                                 size_t              add_length,
                                 const unsigned char *data_in,
                                 unsigned char       *data_out,
                                 size_t              length,
                                 const unsigned char *key,
                                 uint32_t            *header,
                                 size_t              header_length_over_16,
                                 unsigned char       *tag,
                                 size_t              tag_length)
{
  // Assumptions:
  // * There is always header input, and the header buffer is 32 bytes and word-aligned
  // * The actual data content in the header is at least 16 bytes (full block) and
  //   at maximum 32 bytes (precalculated B0_B1 in BLE). The remainder of the header buffer
  //   is passed in zero-initialized.
  // * Input parameter header_length_over_16 reflects this: it says how much larger than 16 bytes the
  //   header input buffer is.
  // * The header is always word-aligned
  // * There may not always be ADD input (e.g. BLE-CCM)
  // * There may not always be data input (e.g. CCM in authenticated-only mode)
  // * The header input is pre-calculated by the caller of this function
  // * Data output may be NULL (in which case it is discarded)
  // * Tag length may be 0 (CCM-star), in which case the tag pointer is also allowed to be NULL

  size_t L = (((uint8_t*)header)[0] & 0x7) + 1;

  // Mangling DDATA1 (KEY), DDATA2 (= DATA0/DATA1), DDATA3 (=DATA2/DATA3),
  // DDATA4 (KEYBUF). Max execution sequence length = 20
  CRYPTO_TypeDef   *device =
    crypto_management_acquire_preemption(CRYPTO_MANAGEMENT_SAVE_DDATA1
                                         | CRYPTO_MANAGEMENT_SAVE_DDATA2
                                         | CRYPTO_MANAGEMENT_SAVE_DDATA3
                                         | CRYPTO_MANAGEMENT_SAVE_DDATA4
                                         | CRYPTO_MANAGEMENT_SAVE_UPTO_SEQ4);

  // Setup CRYPTO for AES-128 mode (256 not implemented here)
  device->CTRL      = CRYPTO_CTRL_AES_AES128;
  device->WAC       = 0UL;

  CRYPTO_KeyBufWriteUnaligned(device, key, cryptoKey128Bits);

  // Store CBC IV in device->DATA0
  CRYPTO_DataWrite(&device->DATA0, header);

  // Calculate Counter IV for encryption, which is basically
  // the CBC IV with the flags byte masked with 0x7, and zeroing
  // out the length field.
  ((uint8_t*)header)[0] &= 0x07;
  for (size_t i = 16 - L; i < 16; i++) {
    ((uint8_t*)header)[i] = 0;
  }

  // Store Counter IV in crypto->DATA1
  CRYPTO_DataWrite(&device->DATA1, header);

  // Start the CBC-MAC by encrypting the IV and storing the result in DATA
  device->CMD = CRYPTO_CMD_INSTR_AESENC;

  // Update the CBC-MAC with the remainder of the header and
  // the full AAD, up until block boundary
  if (header_length_over_16 + add_length > 0) {
    if (header_length_over_16 < 16) {
      // Fill up the remainder taking from the ADD (if any) and into the
      // second header block (guaranteed to have a 32-byte writeable buffer)
      if (add_length > 0) {
        size_t injected_add = add_length > (16 - header_length_over_16) ? 16 - header_length_over_16 : add_length;

        for (size_t i = 0; i < injected_add; i++) {
          (&((uint8_t*)&header[4])[header_length_over_16])[i] = add_data[i];
        }

        header_length_over_16 += injected_add;
        add_data += injected_add;
        add_length -= injected_add;
      }
    }

    // Accumulate the second header block into the CBC-MAC
    CRYPTO_DataWrite(&device->DATA3, &header[4]);

    // Accumulate: XOR the input block into the previously accumulated value in DATA0
    // and encrypt the result to get the updated accumulator in DATA0
    device->CMD = CRYPTO_CMD_INSTR_DATA3TODATA0XOR;
    device->CMD = CRYPTO_CMD_INSTR_AESENC;
    device->CMD = CRYPTO_CMD_INSTR_DATA0TODATA3;

    // Run the remainder of the ADD through the CBC-MAC accumulator
    if (add_length > 0) {
      // Set the iteration count
      device->SEQCTRL  = add_length;
      device->SEQCTRLB = 0;

      CRYPTO_EXECUTE_4(device,
                       CRYPTO_CMD_INSTR_DMA0TODATA,      // Load new block of data into DATA0
                       CRYPTO_CMD_INSTR_DATA3TODATA0XOR, // XOR the accumulated tag with the new block
                       CRYPTO_CMD_INSTR_AESENC,          // Encrypt the XOR result
                       CRYPTO_CMD_INSTR_DATA0TODATA3     // Store the new accumulated tag to DATA3
                       );
      while (add_length) {
        if (add_length < 16) {
          // Use writeable header buffer for zero padding
          header[0] = 0;
          header[1] = 0;
          header[2] = 0;
          header[3] = 0;
          // Since add_data is potentially unaligned, do a byte copy
          for (size_t i = 0; i < add_length; i++) {
            ((uint8_t*)header)[i] = add_data[i];
          }
          CRYPTO_DataWrite(&device->DATA0, header);
          add_length = 0;
        } else {
          CRYPTO_DataWriteUnaligned(&device->DATA0, add_data);
          add_length  -= 16;
          add_data    += 16;
        }
      }
    }
  } else {
    // Move the CBC-MAC accumulator to DATA3
    device->CMD = CRYPTO_CMD_INSTR_DATA0TODATA3;
  }

  // Set the iteration count
  device->SEQCTRL  = length;
  device->SEQCTRLB = 0;

  if (encrypt) {
    CRYPTO_EXECUTE_16(device,
                      CRYPTO_CMD_INSTR_EXECIFA,

                      // Read data input into DATA2
                      CRYPTO_CMD_INSTR_DMA0TODATA,
                      CRYPTO_CMD_INSTR_DATA0TODATA2,

                      // Accumulate the plaintext into the CBC-MAC
                      CRYPTO_CMD_INSTR_DATA3TODATA0XOR,
                      CRYPTO_CMD_INSTR_AESENC,
                      CRYPTO_CMD_INSTR_DATA0TODATA3,

                      // Calculate key stream block
                      CRYPTO_CMD_INSTR_DATA1INC,
                      CRYPTO_CMD_INSTR_DATA1TODATA0,
                      CRYPTO_CMD_INSTR_AESENC,

                      // XOR plaintext with key stream and output the result
                      CRYPTO_CMD_INSTR_DATA2TODATA0XOR,
                      CRYPTO_CMD_INSTR_DATATODMA0,

                      // Calculate tag into DATA0 when all data has been processed
                      CRYPTO_CMD_INSTR_EXECIFLAST,
                      CRYPTO_CMD_INSTR_DATA1INCCLR,
                      CRYPTO_CMD_INSTR_DATA1TODATA0,
                      CRYPTO_CMD_INSTR_AESENC,
                      CRYPTO_CMD_INSTR_DATA3TODATA0XOR
                      );
  } else {
    CRYPTO_EXECUTE_18(device,
                      CRYPTO_CMD_INSTR_EXECIFA,

                      // Calculate key stream block into DATA0
                      CRYPTO_CMD_INSTR_DATA1INC,
                      CRYPTO_CMD_INSTR_DATA1TODATA0,
                      CRYPTO_CMD_INSTR_AESENC,

                      // XOR data input with key stream and output the result
                      CRYPTO_CMD_INSTR_DATA0TODATA2,
                      CRYPTO_CMD_INSTR_DMA0TODATA,
                      CRYPTO_CMD_INSTR_DATA2TODATA0XORLEN,
                      CRYPTO_CMD_INSTR_DATATODMA0,

                      // Accumulate the plaintext into the CBC-MAC
                      CRYPTO_CMD_INSTR_DATA0TODATA2,
                      CRYPTO_CMD_INSTR_DATA3TODATA0,
                      CRYPTO_CMD_INSTR_DATA2TODATA0XORLEN,

                      CRYPTO_CMD_INSTR_AESENC,
                      CRYPTO_CMD_INSTR_DATA0TODATA3,

                      // Calculate tag into DATA0 when all data has been processed
                      CRYPTO_CMD_INSTR_EXECIFLAST,
                      CRYPTO_CMD_INSTR_DATA1INCCLR,
                      CRYPTO_CMD_INSTR_DATA1TODATA0,
                      CRYPTO_CMD_INSTR_AESENC,
                      CRYPTO_CMD_INSTR_DATA3TODATA0XOR
                      );
  }

  while (length) {
    if (length < 16) {
      // Use writeable header buffer for zero padding
      header[0] = 0;
      header[1] = 0;
      header[2] = 0;
      header[3] = 0;
      // Since input data is potentially unaligned, do a byte copy
      for (size_t i = 0; i < length; i++) {
        ((uint8_t*)header)[i] = data_in[i];
      }
      CRYPTO_DataWrite(&device->DATA0, header);
      CRYPTO_DataRead(&device->DATA0, header);
      if (data_out) {
        // Data output can be unaligned, too
        for (size_t i = 0; i < length; i++) {
          data_out[i] = ((uint8_t*)header)[i];
        }
        // In this case, we're guaranteed it is the last part of
        // the data and don't need to adjust the pointer anymore.
      }
      length = 0;
    } else {
      CRYPTO_DataWriteUnaligned(&device->DATA0, data_in);
      length  -= 16;
      data_in += 16;
      if (data_out) {
        CRYPTO_DataReadUnaligned(&device->DATA0, data_out);
        data_out += 16;
      } else {
        CRYPTO_DataRead(&device->DATA0, header);
      }
    }
  }

  // Read calculated authentication tag from DATA0 register
  CRYPTO_DataRead(&device->DATA0, header);
  crypto_management_release_preemption(device);

  if (encrypt) {
    // For encryption, return the requested amount of tag
    for (size_t i = 0; i < tag_length; i++) {
      tag[i] = ((uint8_t*)header)[i];
    }
  } else {
    // For decryption, verify the requested amount of tag
    uint32_t accumulator = 0;
    for (size_t i = 0; i < tag_length; i++) {
      accumulator |= ((uint8_t*)header)[i] ^ tag[i];
    }
    if (accumulator != 0) {
      return SL_STATUS_INVALID_SIGNATURE;
    }
  }

  return SL_STATUS_OK;
}

// Perform a CCM encrypt/decrypt operation with BLE parameters and input.
// This means:
// * 13 bytes IV
// * 1 byte AAD (parameter 'header')
// * AES-128 key (16 byte key)
// * in-place encrypt/decrypt with variable length plain/ciphertext
//   (up to 64 kB, uint16 overflow)
// * 4 byte tag
static sl_status_t aes_ccm_ble(bool                encrypt,
                               unsigned char       *data,
                               size_t              length,
                               const unsigned char *key,
                               const unsigned char *iv,
                               unsigned char       header,
                               unsigned char       *tag)

{
  // Use 32-byte word aligned buffer to bypass some time-consuming logic in aes_ccm_radio
  uint32_t b0b1_words[32 / sizeof(uint32_t)];
  uint8_t* b0b1_bytes = (uint8_t*)b0b1_words;

  // Fill in B0 block according to BLE spec
  b0b1_bytes[0] = 0x49U;

  // Copy in the 13 bytes of nonce
  for (size_t i = 0; i < 13; i++) {
    b0b1_bytes[i + 1] = iv[i];
  }

  b0b1_bytes[14] = (uint8_t) length >> 8;
  b0b1_bytes[15] = (uint8_t) length;
  b0b1_bytes[16] = 0; // upper octet of AAD length
  b0b1_bytes[17] = 1; // lower octet of AAD length (BLE CCM always has only one byte of AAD)
  b0b1_bytes[18] = header; // AAD
  b0b1_bytes[19] = 0;
  b0b1_words[5] = 0;
  b0b1_words[6] = 0;
  b0b1_words[7] = 0;

  return aes_ccm_radio(encrypt,
                       NULL, 0,
                       data, data, length,
                       key,
                       b0b1_words, 32 - 16,
                       tag, 4);
}

//
// CCM buffer authenticated decryption optimized for BLE
//
sl_status_t sli_ccm_auth_decrypt_ble(unsigned char       *data,
                                     size_t              length,
                                     const unsigned char *key,
                                     const unsigned char *iv,
                                     unsigned char       header,
                                     unsigned char       *tag)
{
  return aes_ccm_ble(false,
                     data,
                     length,
                     key,
                     iv,
                     header,
                     (uint8_t *) tag);
}

//
// CCM buffer encryption optimized for BLE
//
sl_status_t sli_ccm_encrypt_and_tag_ble(unsigned char       *data,
                                        size_t              length,
                                        const unsigned char *key,
                                        const unsigned char *iv,
                                        unsigned char       header,
                                        unsigned char       *tag)
{
  return aes_ccm_ble(true,
                     data,
                     length,
                     key,
                     iv,
                     header,
                     tag);
}

sl_status_t sli_ccm_zigbee(bool encrypt,
                           const unsigned char *data_in,
                           unsigned char       *data_out,
                           size_t              length,
                           const unsigned char *key,
                           const unsigned char *iv,
                           const unsigned char *aad,
                           size_t              aad_len,
                           unsigned char       *tag,
                           size_t              tag_len)
{
  // Validated assumption: for ZigBee, the authenticated data
  // length will always fit into a 16-bit length field (up to
  // 64 kB of data), meaning the header data size will always
  // be either 16 (no ciphertext) or 18 bytes long.

  // Use 32-byte word aligned buffer to bypass some time-consuming logic in aes_ccm_radio
  uint32_t header_words[32 / sizeof(uint32_t)];
  uint8_t* header_bytes = (uint8_t*)header_words;

  // Start with the 'flags' byte. It encodes whether there is AAD,
  // and the length of the tag fields
  header_bytes[0] = 0x01 // always 2 bytes of message length
                    | ((aad_len > 0) ? 0x40 : 0x00) // Set 'aflag' bit if there is AAD
                    | ((tag_len >= 4) ? (((tag_len - 2) / 2) << 3) : 0); // Encode tag length

  for (size_t i = 0; i < 13; i++) {
    header_bytes[i + 1] = iv[i];
  }

  header_bytes[14] = (uint8_t) length >> 8;
  header_bytes[15] = (uint8_t) length;
  header_words[4] = 0;
  header_words[5] = 0;
  header_words[6] = 0;
  header_words[7] = 0;
  if (aad_len > 0) {
    header_bytes[16] = (uint8_t) aad_len >> 8; // upper octet of AAD length
    header_bytes[17] = (uint8_t) aad_len; // lower octet of AAD length
  }

  return aes_ccm_radio(encrypt,
                       aad,
                       aad_len,
                       data_in,
                       data_out,
                       length,
                       key,
                       header_words,
                       (aad_len > 0 ? 2 : 0),
                       tag,
                       tag_len);
}

/*
 * Process a table of BLE RPA device keys and look for a
 * match against the supplied hash
 */
int sli_process_ble_rpa(const unsigned char   keytable[],
                        uint32_t              keymask,
                        uint32_t              prand,
                        uint32_t              hash)
{
  size_t index;
  uint32_t data_register[4] = { 0 };
  data_register[3] = __REV(prand);

  /* Mangling DDATA1 (KEY) and DDATA2 (= DATA0/DATA1). Max execution length = 2 */
  CRYPTO_TypeDef *device =
    crypto_management_acquire_preemption(CRYPTO_MANAGEMENT_SAVE_DDATA1
                                         | CRYPTO_MANAGEMENT_SAVE_DDATA2
                                         | CRYPTO_MANAGEMENT_SAVE_UPTO_SEQ0);
  /* Set up CRYPTO to do AES, and load prand */
  device->CTRL     = CRYPTO_CTRL_AES_AES128 | CRYPTO_CTRL_KEYBUFDIS;
  device->WAC      = 0UL;

  CRYPTO_DataWrite(&device->DATA1, (uint32_t*)data_register);

  /* For each key, execute AES encrypt operation and compare w hash */
  /* Read result of previous iteration first to minimize stall while waiting
     for AES to finish */
  int currentindex = -1;
  for ( index = 0; index < 32; index++ ) {
    if ( (keymask & (1U << index)) == 0 ) {
      continue;
    }

    CRYPTO_DataRead(&device->DATA0, data_register);
    CRYPTO_DataWrite(&device->KEY, (uint32_t*)(&keytable[index * 16]));
    CRYPTO_EXECUTE_2(device,
                     CRYPTO_CMD_INSTR_DATA1TODATA0,
                     CRYPTO_CMD_INSTR_AESENC);

    if ( (currentindex >= 0)
         && ( (data_register[3] & 0xFFFFFF00UL) == __REV(hash) ) ) {
      crypto_management_release_preemption(device);
      return currentindex;
    }

    currentindex = index;
  }

  /* Read result of last encryption and check for hash */
  CRYPTO_DataRead(&device->DATA0, data_register);
  crypto_management_release_preemption(device);

  if ( (data_register[3] & 0xFFFFFF00UL) == __REV(hash) ) {
    return currentindex;
  }

  return -1;
}

sl_status_t sli_aes_crypt_ecb_radio(bool                   encrypt,
                                    const unsigned char    *key,
                                    unsigned int           keybits,
                                    const unsigned char    input[16],
                                    volatile unsigned char output[16])
{
  /* process one ore more blocks of data */
  CRYPTO_TypeDef *device =
    crypto_management_acquire_preemption(CRYPTO_MANAGEMENT_SAVE_DDATA1
                                         | CRYPTO_MANAGEMENT_SAVE_DDATA2
                                         | CRYPTO_MANAGEMENT_SAVE_DDATA4);
  device->WAC = 0;
  device->CTRL = 0;

  CRYPTO_KeyBufWriteUnaligned(device, key,
                              (keybits == 128UL
                               ? cryptoKey128Bits : cryptoKey256Bits));

  if (!encrypt) {
    // Transform encryption to decryption key
    device->CMD = CRYPTO_CMD_INSTR_AESENC;
    device->CMD = CRYPTO_CMD_INSTR_DDATA1TODDATA4;
  }

  CRYPTO_DataWriteUnaligned(&device->DATA0, (const uint8_t *)input);

  if ( encrypt ) {
    device->CMD = CRYPTO_CMD_INSTR_AESENC;
  } else {
    device->CMD = CRYPTO_CMD_INSTR_AESDEC;
  }

  CRYPTO_DataReadUnaligned(&device->DATA0, (uint8_t *)output);

  crypto_management_release_preemption(device);

  return SL_STATUS_OK;
}

sl_status_t sli_aes_crypt_ctr_radio(const unsigned char   *key,
                                    unsigned int           keybits,
                                    const unsigned char    input[16],
                                    const unsigned char    iv_in[16],
                                    volatile unsigned char iv_out[16],
                                    volatile unsigned char output[16])
{
  /* process one ore more blocks of data */
  CRYPTO_TypeDef *device =
    crypto_management_acquire_preemption(CRYPTO_MANAGEMENT_SAVE_DDATA1
                                         | CRYPTO_MANAGEMENT_SAVE_DDATA2
                                         | CRYPTO_MANAGEMENT_SAVE_DDATA4);
  device->WAC = 0;
  device->CTRL = 0;

  CRYPTO_KeyBufWriteUnaligned(device, key,
                              (keybits == 128UL
                               ? cryptoKey128Bits : cryptoKey256Bits));

  if ((uint32_t)iv_in != 0) {
    CRYPTO_DataWriteUnaligned(&device->DATA1, (uint8_t *)iv_in);
  } else {
    uint32_t iv[4] = { 0, 0, 0, 0 };
    CRYPTO_DataWrite(&device->DATA1, iv);
  }

  device->CMD = CRYPTO_CMD_INSTR_DATA1TODATA0;
  device->CMD = CRYPTO_CMD_INSTR_AESENC;
  device->CMD = CRYPTO_CMD_INSTR_DATA1INC;

  CRYPTO_DataWriteUnaligned(&device->DATA0XOR, (uint8_t *)(input));
  CRYPTO_DataReadUnaligned(&device->DATA0, (uint8_t *)(output));

  if ((uint32_t)iv_out != 0) {
    CRYPTO_DataReadUnaligned(&device->DATA1, (uint8_t *)iv_out);
  }

  crypto_management_release_preemption(device);

  return SL_STATUS_OK;
}

#endif /* CRYPTO_PRESENT */
