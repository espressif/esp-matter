/***************************************************************************//**
 * @file
 * @brief wMBus support component
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
#include "sl_wmbus_support.h"
#include "sl_wmbus_support_config.h"
#include <string.h>

#include "em_gpcrc.h"
#include "em_cmu.h"
#include "em_common.h"

#include "mbedtls/aes.h"

//(1B length field + 255B payload + 17block * 2B CRC) * 3/2 3of6 ratio + 1B postamble
//if you need software Manchester, you should change it to (1+255+17*2)*2+1
#define WMBUS_MAX_ENCODED_LENGTH (1 + 255 + 17 * 2) * 3 / 2 + 1

//length of an encoded full block (16B+2B crc) using T mode's 3of6
#define ENC_FULLBLOCK_LENGTH_T 27
//length of an encoded first block (10B+2B crc) using T mode's 3of6
#define ENC_FIRSTBLOCK_LENGTH_T 18

//stores the current mbus Mode. Currently only used for modeT TX.
static WMBUS_Mode_t wmbusMode;

//tools for encription
static mbedtls_aes_context aes_ctx;
SL_ALIGN(4) static uint8_t aesKey[16] SL_ATTRIBUTE_ALIGN(4); //the aes key should be stored 32-bit aligned for mbedTls

/// Constant for accessibility setting used in the app
static WMBUS_accessibility_t wmbus_accessibility = SL_WMBUS_ACCESSIBLILTY;

/**
 * Get the current WMBUS accessibility settings
 *
 * @return the current value
 */
WMBUS_accessibility_t get_wmbus_accessibility(void)
{
  return wmbus_accessibility;
}

/**
 * Set the current WMBUS accessibility settings
 *
 * @param new_wmbus_accessibility to be applied
 */
void set_wmbus_accessibility(WMBUS_accessibility_t new_wmbus_accessibility)
{
  wmbus_accessibility = new_wmbus_accessibility;
}

#if 0 // unused helper function
/**
 * @brief
 *   Encodes a message block with Manchester coding according to 13757-4
 *   Also adds the encoded checksum after the block, and the postamble
 *
 * @param[in] input     Pointer to the block to be coded
 * @param[in] checksum  Checksum to be added after the payload
 * @param[in] length    Length of the input block, including the checksum
 * @param[out] output   Pointer where the function writes the encoded block. Can't be the same as the input
 * @return
 *              Returns the length of the encoded block, without postamble
 * @note
 *              This function is not actually used since it can be done with EFR32s
 *              frame controller. However, we'll keep it here for possible future use.
 */
static uint16_t WMBUS_encoderManchester(uint8_t *input, uint16_t checksum,
                                        uint8_t length, uint8_t *output)
{
  uint8_t lookUpTable[16] = { 0x55, 0x56, 0x59, 0x5a, 0x65, 0x66, 0x69, 0x6a,
                              0x95, 0x96, 0x99, 0x9a, 0xa5, 0xa6, 0xa9, 0xaa };

  for (int i = 0; i < length; i++) {
    if (i < length - 2) {
      output[2 * i] = lookUpTable[(input[i] >> 4) & 0x0f];
      output[2 * i + 1] = lookUpTable[input[i] & 0x0f];
    } else {
      if (i == length - 2) {
        output[2 * i] = lookUpTable[(checksum >> 12) & 0x0f];
        output[2 * i + 1] = lookUpTable[(checksum >> 8) & 0x0f];
      } else {
        output[2 * i] = lookUpTable[(checksum >> 4) & 0x0f];
        output[2 * i + 1] = lookUpTable[checksum & 0x0f];
      }
    }
  }
  output[2 * length] = 0x55; //postamble
  return 2 * length;
}
#endif

#if defined(_SILICON_LABS_32B_SERIES_1)
/**
 * @enum WMBUS_encode3OutOf6NibbleOut_t
 * @brief possible output alignments for 3of6 encoding. The order of this enum is the order of intended use
 */
typedef enum {
  WMBUS_3OF6OUT_MSBALIGN = 2,        /**< Output is aligned to the following mask: 0b11111100 */
  WMBUS_3OF6OUT_MSBTOLSB_2BIT = -4,  /**< Output is aligned to the following mask: 0b00000011 (lower bits cropped) */
  WMBUS_3OF6OUT_LSBTOMSB_4BIT = 4,   /**< Output is aligned to the following mask: 0b11110000 (upper bits cropped) */
  WMBUS_3OF6OUT_MSBTOLSB_4BIT = -2,  /**< Output is aligned to the following mask: 0b00001111 (lower bits cropped) */
  WMBUS_3OF6OUT_LSBTOMSB_2BIT = 6,   /**< Output is aligned to the following mask: 0b11000000 (upper bits cropped) */
  WMBUS_3OF6OUT_LSBALIGN = 0,        /**< Output is aligned to the following mask: 0b00111111 */
} WMBUS_encode3OutOf6NibbleOut_t;

static uint8_t WMBUS_encode3OutOf6Nibble(uint8_t inputbyte,
                                         bool highNibble,
                                         WMBUS_encode3OutOf6NibbleOut_t outMode)
{
  uint8_t lookuptable[16] = { 0x16, 0x0d, 0x0e, 0x0b, 0x1c, 0x19, 0x1a, 0x13,
                              0x2C, 0x25, 0x26, 0x23, 0x34, 0x31, 0x32, 0x29 };
  if ( highNibble ) {
    inputbyte >>= 4;
  } else {
    inputbyte &= 0x0F;
  }
  if ( outMode >= 0 ) {
    return lookuptable[inputbyte] << outMode;
  } else {
    return lookuptable[inputbyte] >> -outMode;
  }
}

/**
 * @brief
 *   Encodes a message block with 3 out of 6 coding according to 13757-4
 *   Also adds the encoded checksum after the block, and the postamble
 *
 * @param[in]  input          Pointer to the block to be coded
 * @param[in]  checksum           Checksum to be added after the payload
 * @param[in]  length       Length of the input block, not including the checksum. Odd length assumes addPostamble=true
 * @param[out] output       Pointer where the function writes the encoded block. Output can start at the same position as input
 * @param[in]  addPostamble Add postamble after the checksum
 * @return
 *              Returns the length of the encoded block, without postamble,
 *              unless postamble is coded as the last half byte
 */
static uint16_t WMBUS_encoder3OutOf6(uint8_t *input, uint16_t checksum,
                                     uint8_t length, uint8_t *output, bool addPostamble)
{
  int16_t i = length - 1;
  uint16_t outlength = (length + sizeof(checksum)) * 3 / 2; //+0.5 with odd length
  uint16_t o = outlength - 1;

  if ( length % 2 ) {
    o++; // 0.5B postamble and the missing 0.5B from the length calculation
    outlength++;
    output[o] =  WMBUS_encode3OutOf6Nibble(checksum, false, WMBUS_3OF6OUT_LSBTOMSB_4BIT);
    output[o] |= (output[o] & 0x10) ? 0x05 : 0x0A; //postamble
    o--;
    output[o] =  WMBUS_encode3OutOf6Nibble(checksum, false, WMBUS_3OF6OUT_MSBTOLSB_2BIT);
    output[o] |= WMBUS_encode3OutOf6Nibble(checksum, true, WMBUS_3OF6OUT_MSBALIGN);
    o--;
    output[o] =  WMBUS_encode3OutOf6Nibble(checksum >> 8, false, WMBUS_3OF6OUT_LSBALIGN);
    output[o] |= WMBUS_encode3OutOf6Nibble(checksum >> 8, true, WMBUS_3OF6OUT_LSBTOMSB_2BIT);
    o--;
    output[o] = WMBUS_encode3OutOf6Nibble(checksum >> 8, true, WMBUS_3OF6OUT_MSBTOLSB_4BIT);

    //encode last byte here, so we don't have to deal with odd length later
    output[o] |= WMBUS_encode3OutOf6Nibble(input[i], false, WMBUS_3OF6OUT_LSBTOMSB_4BIT);
    o--;
    output[o] =  WMBUS_encode3OutOf6Nibble(input[i], false, WMBUS_3OF6OUT_MSBTOLSB_2BIT);
    output[o] |= WMBUS_encode3OutOf6Nibble(input[i], true, WMBUS_3OF6OUT_MSBALIGN);
    o--;
    i--;
  } else {
    output[o] =  WMBUS_encode3OutOf6Nibble(checksum, false, WMBUS_3OF6OUT_LSBALIGN);
    if ( addPostamble ) {
      output[o + 1] =  (output[o] & 0x01) ? 0x55 : 0xAA;
      outlength++;
    }
    output[o] |= WMBUS_encode3OutOf6Nibble(checksum, true, WMBUS_3OF6OUT_LSBTOMSB_2BIT);
    o--;
    output[o] =  WMBUS_encode3OutOf6Nibble(checksum, true, WMBUS_3OF6OUT_MSBTOLSB_4BIT);
    output[o] |= WMBUS_encode3OutOf6Nibble(checksum >> 8, false, WMBUS_3OF6OUT_LSBTOMSB_4BIT);
    o--;
    output[o] =  WMBUS_encode3OutOf6Nibble(checksum >> 8, false, WMBUS_3OF6OUT_MSBTOLSB_2BIT);
    output[o] |= WMBUS_encode3OutOf6Nibble(checksum >> 8, true, WMBUS_3OF6OUT_MSBALIGN);
    o--;
  }

  for (; i > 0; i -= 2) {
    output[o] =  WMBUS_encode3OutOf6Nibble(input[i], false, WMBUS_3OF6OUT_LSBALIGN);
    output[o] |= WMBUS_encode3OutOf6Nibble(input[i], true, WMBUS_3OF6OUT_LSBTOMSB_2BIT);
    o--;
    output[o] =  WMBUS_encode3OutOf6Nibble(input[i], true, WMBUS_3OF6OUT_MSBTOLSB_4BIT);
    output[o] |= WMBUS_encode3OutOf6Nibble(input[i - 1], false, WMBUS_3OF6OUT_LSBTOMSB_4BIT);
    o--;
    // take special care in case this is the first byte: we're reading and writing the same
    uint8_t out;
    out =  WMBUS_encode3OutOf6Nibble(input[i - 1], false, WMBUS_3OF6OUT_MSBTOLSB_2BIT);
    out |= WMBUS_encode3OutOf6Nibble(input[i - 1], true, WMBUS_3OF6OUT_MSBALIGN);
    output[o] = out;
    o--;
  }

  return outlength;
}

/**
 * @brief
 *   Calculates CRC according to 13757-4, using the GPCRC engine
 *   While the feeding of the CRC engine could be used with any
 *   protocol, the readout is somewhat 13757-4 specific (e.g.
 *   inverted readout, endiannes)
 *   GPCRC should be correcly initialized before calling this.
 *
 * @param[in] input     Pointer to the block to be coded
 * @param[in] length    Length of the input block, including the checksum
 * @return
 *              Returns the CRC of the block
 * @note
 *              There's 3 ways of calculating CRC without the radio: Full software
 *              implementation, GPCRC fed manually, GPCRC fed through DMA.
 *              We benchmarked all, and for 16B blocks, the GPCRC fed manually
 *              were the fastest. DMA adds some latency, and it only useful for
 *              longer packets.
 */
static uint16_t WMBUS_crc(uint8_t *input, uint8_t length)
{
  uint16_t checksum;

  GPCRC_Start(GPCRC);

  uint8_t steps = length / 4;
  uint8_t i = 0;
  for (; i < steps; i++) {
    GPCRC_InputU32(GPCRC, *((uint32_t *) &(input[4 * i])));
  }

  switch (length % 4) {
    case 3:
      GPCRC_InputU16(GPCRC, *((uint16_t *) &(input[4 * i])));
      GPCRC_InputU8(GPCRC, (input[4 * i + 2]));
      break;
    case 2:
      GPCRC_InputU16(GPCRC, *((uint16_t *) &(input[4 * i])));
      break;
    case 1:
      GPCRC_InputU8(GPCRC, (input[4 * i]));
      break;
    default:
      break;
  }

  checksum = (uint16_t) GPCRC_DataReadBitReversed(GPCRC);
  checksum ^= 0xffff;

  return checksum;
}

#endif

uint16_t WMBUS_phy_software(uint8_t *buffer,
                            uint8_t length,
                            uint16_t buffer_length)
{
  if (buffer == NULL) {
    return 0;
  }
#if defined(_SILICON_LABS_32B_SERIES_1)
  if ( wmbusMode == WMBUS_MODE_T_METER ) {
    uint16_t crc;
    uint16_t actualsize = 0;

    uint8_t lastBlockSize = (length - 10) % 16; //if last block is 16B, we treat it as a normal one
    uint8_t wholeBlocks = (length - 10) / 16;
    //offsets points the end of the last full block
    int16_t rdOffset = 10 + wholeBlocks * 16;
    int16_t wtOffset = ENC_FIRSTBLOCK_LENGTH_T + ENC_FULLBLOCK_LENGTH_T * wholeBlocks;

    if ( buffer_length < wtOffset + (lastBlockSize + 2) * 3 / 2 ) {
      return 0; //no room to store the encoded packet
    }

    //handle last block
    if ( lastBlockSize ) {
      crc = WMBUS_crc(buffer + rdOffset, lastBlockSize);
      actualsize += WMBUS_encoder3OutOf6(buffer + rdOffset, crc, lastBlockSize, buffer + wtOffset, true);
    }

    //handle full blocks
    while ( rdOffset > 10 ) {
      rdOffset -= 16;
      wtOffset -= ENC_FULLBLOCK_LENGTH_T;

      crc = WMBUS_crc(buffer + rdOffset, 16);
      //add postamble if actualsize is still 0, which means this is the last block
      actualsize += WMBUS_encoder3OutOf6(buffer + rdOffset, crc, 16, buffer + wtOffset, actualsize == 0 ? true : false);
    }
    //handle first block
    crc = WMBUS_crc(buffer, 10);
    actualsize += WMBUS_encoder3OutOf6(buffer, crc, 10, buffer, false);
    return actualsize;
  } else {
#else
  (void)buffer_length;
  (void)buffer;
#endif
    //no other modes needs software encode currently
    return length;
#if defined(_SILICON_LABS_32B_SERIES_1)
  }
#endif
  }

  void WMBUS_Init(void)
  {
    // setting mode according to configuration
    WMBUS_SetMode(SL_WMBUS_TYPE);
  }

  void WMBUS_SetMode(WMBUS_Mode_t mode)
  {
    wmbusMode = mode;

#if defined(_SILICON_LABS_32B_SERIES_1)
    if ( wmbusMode == WMBUS_MODE_T_METER ) {
#if !defined(_SILICON_LABS_32B_SERIES_2)
      CMU_ClockEnable(cmuClock_HFPER, true);
#endif
      CMU_ClockEnable(cmuClock_GPCRC, true);

      GPCRC_Init_TypeDef GPCRC_init = GPCRC_INIT_DEFAULT;
      GPCRC_init.crcPoly = 0x3d65;
      GPCRC_init.reverseBits = true;
      GPCRC_init.reverseByteOrder = false;

      GPCRC_Init(GPCRC, &GPCRC_init);
    } else {
      CMU_ClockEnable(cmuClock_GPCRC, false);
    }
#endif
  }

  WMBUS_Mode_t WMBUS_GetSelectedMode(void)
  {
    return wmbusMode;
  }

  uint32_t WMBUS_getMeterLimitedAccRxStart(bool slowMode)
  {
    uint32_t return_value = 0UL;
    switch (wmbusMode) {
      case WMBUS_MODE_S:
      case WMBUS_MODE_R2:
        return_value = 3000UL;
        break;
      case WMBUS_MODE_T_METER:
      case WMBUS_MODE_T_COLLECTOR:
        return_value = 2000UL;
        break;
      case WMBUS_MODE_C:
      case WMBUS_MODE_F:
        if (slowMode) {
          return_value = 999500UL;
        } else {
          return_value = 99500UL;
        }
        break;
      case WMBUS_MODE_N_24:
        return_value = 2099500UL;
        break;
      case WMBUS_MODE_N_48:
      case WMBUS_MODE_N_192:
        return_value = 1099500UL;
        break;
      default:
        break;
    }
    return return_value;
  }

  uint32_t WMBUS_getMeterLimitedAccRxStop(bool slowMode)
  {
    uint32_t return_value = 0UL;
    switch (wmbusMode) {
      case WMBUS_MODE_S:
      case WMBUS_MODE_R2:
        return_value = 50000UL;
        break;
      case WMBUS_MODE_T_METER:
      case WMBUS_MODE_T_COLLECTOR:
        return_value = 3000UL;
        break;
      case WMBUS_MODE_C:
      case WMBUS_MODE_F:
        if (slowMode) {
          return_value = 1000500UL;
        } else {
          return_value = 100500UL;
        }
        break;
      case WMBUS_MODE_N_24:
        return_value = 2100500UL;
        break;
      case WMBUS_MODE_N_48:
      case WMBUS_MODE_N_192:
        return_value = 1100500UL;
        break;
      default:
        break;
    }
    return return_value;
  }

  uint16_t WMBUSframe_Chars2MField(const char* manuf)
  {
    uint16_t code = 0;

    if (manuf != NULL) {
      code = (uint16_t)((manuf[0] - 64) << 10);
      code |= (uint16_t)((manuf[1] - 64) << 5);
      code |= (uint16_t)(manuf[2] - 64);
    }
    return code;
  }

  void WMBUSframe_MField2Chars(const uint16_t mfield, char* manuf)
  {
    if (manuf != NULL) {
      manuf[0] = ((mfield >> 10) & 0x1F) + 64;
      manuf[1] = ((mfield >> 5) & 0x1F) + 64;
      manuf[2] = ((mfield >> 0) & 0x1F) + 64;
    }
  }

  uint8_t WMBUSframe_addPayloadI8(uint8_t *buffer,
                                  WMBUSframe_dibFunction_t dibFunction,
                                  uint8_t vib,
                                  int8_t value)
  {
    if (buffer == NULL) {
      return 0;
    }
    buffer[0] = (uint8_t)((dibFunction << 4) + 1); //DIF=1 is int8
    buffer[1] = vib;
    memcpy(buffer + 2, &value, 1);
    return 3;
  }

  uint8_t WMBUSframe_addPayloadI16(uint8_t *buffer,
                                   WMBUSframe_dibFunction_t dibFunction,
                                   uint8_t vib,
                                   int16_t value)
  {
    if (buffer == NULL) {
      return 0;
    }
    buffer[0] = (uint8_t)((dibFunction << 4) + 2); //DIF=2 is int16
    buffer[1] = vib;
    memcpy(buffer + 2, &value, 2);
    return 4;
  }

  uint8_t WMBUSframe_addPayloadI32(uint8_t *buffer,
                                   WMBUSframe_dibFunction_t dibFunction,
                                   uint8_t vib,
                                   int32_t value)
  {
    if (buffer == NULL) {
      return 0;
    }
    buffer[0] = (uint8_t)((dibFunction << 4) + 4); //DIF=4 is int32
    buffer[1] = vib;
    memcpy(buffer + 2, &value, 4);
    return 6;
  }

  uint16_t WMBUSframe_addIdleFiller(uint8_t *buffer, uint16_t fillerLength)
  {
    if (buffer == NULL) {
      return 0;
    }
    memset(buffer, 0x2f, fillerLength);
    return fillerLength;
  }

  void WMBUSframe_crypto5Init()
  {
    mbedtls_aes_init(&aes_ctx);
  }

  void WMBUSframe_crypto5SetKey(const uint8_t *newKey)
  {
    memcpy(aesKey, newKey, 16);
  }

  void WMBUSframe_crypto5GetKey(uint8_t *key)
  {
    memcpy(key, aesKey, 16);
  }

  uint8_t WMBUSframe_crypto5encrypt(uint8_t *input,
                                    uint8_t *output,
                                    uint8_t *iv,
                                    uint16_t length,
                                    uint8_t encryptedBlocks)
  {
    if (input == NULL || output == NULL || iv == NULL) {
      return 0;
    }

    mbedtls_aes_setkey_enc(&aes_ctx, aesKey, 128);

    if ( encryptedBlocks == 0 ) {
      encryptedBlocks = (uint8_t)(length / 16);
      if ( length % 16 ) {
        encryptedBlocks++;
      }
    }

    //add 0x2F padding as per EN13757-3
    if ( (uint16_t)encryptedBlocks * 16 > length) {
      WMBUSframe_addIdleFiller(input + length, (uint16_t)encryptedBlocks * 16 - length);
    }

    mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_ENCRYPT, (uint16_t)encryptedBlocks * 16, iv, input, output);

    return encryptedBlocks;
  }

  void WMBUSframe_crypto5decrypt(uint8_t *input,
                                 uint8_t *output,
                                 uint8_t *iv,
                                 uint16_t length)
  {
    if (input == NULL || output == NULL || iv == NULL) {
      return;
    }

    mbedtls_aes_setkey_dec(&aes_ctx, aesKey, 128);
    mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_DECRYPT, length, iv, input, output);
  }
