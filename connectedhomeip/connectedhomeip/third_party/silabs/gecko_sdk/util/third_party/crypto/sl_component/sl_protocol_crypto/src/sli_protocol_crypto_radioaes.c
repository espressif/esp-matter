/***************************************************************************//**
 * @file
 * @brief Accelerated cryptographic primitives using the RADIOAES peripheral.
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
#include "em_device.h"

#if defined(RADIOAES_PRESENT)
/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

#include "sli_radioaes_management.h"
#include "sli_protocol_crypto.h"
#include "em_core.h"

#define AES_BLOCK_BYTES       16U
#define AES_128_KEY_BYTES     16U
#define AES_256_KEY_BYTES     32U

#define RADIOAES_CONFIG_BYTES  4U

#ifndef RADIOAES_BLE_RPA_MAX_KEYS
#define RADIOAES_BLE_RPA_MAX_KEYS 32
#endif

/// value for sli_radioaes_dma_sg_descr.tag to direct data to parameters
#define DMA_SG_TAG_ISCONFIG 0x00000010
/// value for sli_radioaes_dma_sg_descr.tag to direct data to processing
#define DMA_SG_TAG_ISDATA   0x00000000
/// value for sli_radioaes_dma_sg_descr.tag specifying data as last
#define DMA_SG_TAG_ISLAST   0x00000020

/// macro to set the offset in the configuration for sli_radioaes_dma_sg_descr.tag
#define DMA_SG_TAG_SETCFGOFFSET(a) ((((a) & 0xFF) << 8))

/// value for sli_radioaes_dma_sg_descr.tag specifying data type payload (will be encrypted/decrypted and authenticated)
#define DMA_SG_TAG_DATATYPE_AESPAYLOAD    0x00000000
/// value for sli_radioaes_dma_sg_descr.tag specifying data type header (will only be authenticated, not encrypted/decrypted)
#define DMA_SG_TAG_DATATYPE_AESHEADER     0x00000040

/// macro to set the amount of invalid bytes in for sli_radioaes_dma_sg_descr.tag
#define DMA_SG_TAG_SETINVALIDBYTES(a) ((((a) & 0x1F) << 8))

#define DMA_AXI_DESCR_CONST_ADDR       0x10000000
#define DMA_AXI_DESCR_REALIGN          0x20000000
#define DMA_AXI_DESCR_DISCARD          0x40000000
#define DMA_AXI_DESCR_INT_ENABLE       0x80000000
#define DMA_AXI_DESCR_INT_DISABLE      0x00000000

#define DMA_AXI_DESCR_NEXT_STOP        0x00000001
#define DMA_AXI_DESCR_NEXT_CONTINUE    0x00000000
#define DMA_AXI_DESCR_MASK_NEXT_ADD    0xFFFFFFFC

/// value of flags to discard the data
#define BLOCK_S_DISCARD_DATA           0x40000000
/// value of flags to realign the data
#define BLOCK_S_REALIGN_DATA           0x20000000
/// value of flags to set addressing in constant mode (pointing to a FIFO)
#define BLOCK_S_CONST_ADDR             0x10000000
/// value of flags to set addressing in increment mode (pointing to a buffer)
#define BLOCK_S_INCR_ADDR              0x00000000
/// mask for flags to only get DMA-related options
#define BLOCK_S_FLAG_MASK_DMA_PROPS    0x70000000
/// value of flags mask for fetcher location destination
#define BLOCK_S_MASK_LOC_DEST          0x00FFFFFF

/// Config ///

/// BA411E offset for Configuration word in DMA Scatter-Gather Tag
#define AES_OFFSET_CFG        0
/// BA411E offset for Configuration word in DMA Scatter-Gather Tag
#define AES_OFFSET_KEY        8
/// BA411E offset for Configuration word in DMA Scatter-Gather Tag
#define AES_OFFSET_IV        40
/// BA411E offset for Configuration word in DMA Scatter-Gather Tag
#define AES_OFFSET_IV2       56
/// BA411E offset for Configuration word in DMA Scatter-Gather Tag
#define AES_OFFSET_KEY2      72
/// BA411E offset for Configuration word in DMA Scatter-Gather Tag
#define AES_OFFSET_MASK      104

/// BA411E Mode Register value for ECB mode of operation
#define AES_MODEID_ECB        0x00000100
/// BA411E Mode Register value for CBC mode of operation
#define AES_MODEID_CBC        0x00000200
/// BA411E Mode Register value for CTR mode of operation
#define AES_MODEID_CTR        0x00000400
/// BA411E Mode Register value for CCM mode of operation
#define AES_MODEID_CCM        0x00002000
/// BA411E Mode Register value for CMAC mode of operation
#define AES_MODEID_CMA        0x00010000

/// BA411E Mode Register value for AES context saving
#define AES_MODEID_CX_SAVE    0x00000020
/// BA411E Mode Register value for AES context loading
#define AES_MODEID_CX_LOAD    0x00000010
/// BA411E Mode Register value for AES no context
#define AES_MODEID_NO_CX      0x00000000

/// BA411E Mode Register value for AES keysize of 128 bits
#define AES_MODEID_AES128     0x00000000
/// BA411E Mode Register value for AES keysize of 256 bits
#define AES_MODEID_AES256     0x00000004
/// BA411E Mode Register value for AES keysize of 192 bits
#define AES_MODEID_AES192     0x00000008

/// BA411E Mode Register value for encryption mode
#define AES_MODEID_ENCRYPT    0x00000000
/// BA411E Mode Register value for decryption mode
#define AES_MODEID_DECRYPT    0x00000001

/// BA411E Size for IV in GCM mode
#define AES_IV_GCM_SIZE       12
/// BA411E Size for IV in all modes except GCM
#define AES_IV_SIZE           16
/// BA411E Size for Context in GCM and CCM modes
#define AES_CTX_xCM_SIZE      32
/// BA411E Size for Context in all modes except GCM and CCM
#define AES_CTX_SIZE          16

///
/// @brief Select which IP core the DMA will use. To set in descriptor sli_radioaes_dma_sg_descr.tag.
///
typedef enum {
  DMA_SG_ENGINESELECT_BYPASS = 0x00,  ///< direct bypass from input to output
  DMA_SG_ENGINESELECT_BA411E = 0x01,  ///< data flow through BA411E AES
  DMA_SG_ENGINESELECT_BA412  = 0x02,  ///< data flow through BA412 DES
  DMA_SG_ENGINESELECT_BA413  = 0x03,  ///< data flow through BA413 Hash
  DMA_SG_ENGINESELECT_BA417  = 0x04   ///< data flow through BA417 ChaChaPoly
} dma_engine_select_t;

///
/// @brief Structure that represent a descriptor for the DMA module
/// (in scatter-gather mode).
///
typedef struct {
  volatile uint32_t address;
  volatile uint32_t nextDescr;
  volatile uint32_t lengthAndIrq;
  volatile uint32_t tag;
} sli_radioaes_dma_descr_t;

#if defined(SLI_RADIOAES_REQUIRES_MASKING)
#define SLI_RADIOAES_MASK_DESCRIPTOR(next_descr_addr) \
  {                                                   \
    .address       = (uint32_t) &sli_radioaes_mask,   \
    .nextDescr     = next_descr_addr,                 \
    .lengthAndIrq  = 0x20000004UL,                    \
    .tag           = 0x00006811UL                     \
  };
#endif

#define DMA_AXI_DESCR_END_POINTER ((sli_radioaes_dma_descr_t*) DMA_AXI_DESCR_NEXT_STOP)

// Local CCM variables
static const uint32_t aes_ccm_config_encrypt = AES_MODEID_CCM
                                               | AES_MODEID_NO_CX
                                               | AES_MODEID_AES128
                                               | AES_MODEID_ENCRYPT;

static const uint32_t aes_ccm_config_decrypt = AES_MODEID_CCM
                                               | AES_MODEID_NO_CX
                                               | AES_MODEID_AES128
                                               | AES_MODEID_DECRYPT;
static const uint32_t zeros = 0;

static sl_status_t sli_radioaes_run_operation(sli_radioaes_dma_descr_t *first_fetch_descriptor,
                                              sli_radioaes_dma_descr_t *first_push_descriptor)
{
  sli_radioaes_state_t aes_ctx;
  #if defined(SLI_RADIOAES_REQUIRES_MASKING)
  sli_radioaes_dma_descr_t mask_descr = SLI_RADIOAES_MASK_DESCRIPTOR((uint32_t)first_fetch_descriptor);
  #endif

  sl_status_t status = sli_radioaes_acquire();
  if (status == SL_STATUS_ISR) {
    sli_radioaes_save_state(&aes_ctx);
  } else if (status != SL_STATUS_OK) {
    return status;
  }

  RADIOAES->CTRL = AES_CTRL_FETCHERSCATTERGATHER | AES_CTRL_PUSHERSCATTERGATHER;

  #if defined(SLI_RADIOAES_REQUIRES_MASKING)
  RADIOAES->FETCHADDR = (uint32_t) &mask_descr;
  #else
  RADIOAES->FETCHADDR = (uint32_t) first_fetch_descriptor;
  #endif
  RADIOAES->PUSHADDR  = (uint32_t) first_push_descriptor;

  RADIOAES->CMD = AES_CMD_STARTPUSHER | AES_CMD_STARTFETCHER;
  while (RADIOAES->STATUS & (AES_STATUS_FETCHERBSY | AES_STATUS_PUSHERBSY)) {
    // Wait for completion
  }

  if (status == SL_STATUS_ISR) {
    sli_radioaes_restore_state(&aes_ctx);
  }

  return sli_radioaes_release();
}

// CCM (and CCM-star) implementation
static sl_status_t aes_ccm_radio(bool                encrypt,
                                 const unsigned char *add_data,
                                 size_t              add_length,
                                 const unsigned char *data_in,
                                 unsigned char       *data_out,
                                 size_t              length,
                                 const unsigned char *key,
                                 const unsigned char *header,
                                 size_t              header_length,
                                 unsigned char       *tag,
                                 size_t              tag_length)

{
  // Assumptions:
  // * There is always header input, but the header input may be block-aligned (BLE-CCM)
  // * There may not always be ADD input (e.g. BLE-CCM)
  // * There may not always be data input (e.g. CCM in authenticated-only mode)
  // * The header input is pre-calculated by the caller of this function
  // * Data output may be NULL (in which case it is discarded)
  // * Tag length may be 0 (CCM-star), in which case the tag pointer is also allowed to be NULL

  // Setup ver_failed output buffer and initialize it in case of decryption to an invalid value
  volatile uint8_t ver_failed[AES_BLOCK_BYTES] = {[0 ... AES_BLOCK_BYTES - 1] = 0xFF };

  // Calculate padding bytes. Since the accelerator expects to see the AESPAYLOAD data type
  // at least once during the operation, ensure that we're emitting a padding block in case
  // no input data is present.
  size_t header_pad_bytes = (AES_BLOCK_BYTES - ((header_length + add_length) % AES_BLOCK_BYTES)) % AES_BLOCK_BYTES;
  size_t data_pad_bytes = (length > 0 ? (AES_BLOCK_BYTES - (length % AES_BLOCK_BYTES)) % AES_BLOCK_BYTES : 16);

  // Fetchers

  // Tag output. If used, always the last descriptor. Not used for CCM-* without tag, the
  // accelerator actually looks at the header and figures out whether or not to take in
  // tag input.
  sli_radioaes_dma_descr_t ccm_desc_fetcher_tag = {
    .address       = (uint32_t) tag,
    .nextDescr     = (uint32_t) DMA_AXI_DESCR_END_POINTER,
    .lengthAndIrq  = (uint32_t) tag_length
                     | BLOCK_S_INCR_ADDR
                     | BLOCK_S_REALIGN_DATA,
    .tag           = DMA_SG_ENGINESELECT_BA411E
                     | DMA_SG_TAG_ISDATA
                     | DMA_SG_TAG_ISLAST
                     | DMA_SG_TAG_DATATYPE_AESPAYLOAD
                     | DMA_SG_TAG_SETINVALIDBYTES(AES_BLOCK_BYTES - tag_length)
  };

  // Data input. Can be zero-length, in which case we'll issue a bogus descriptor instead.
  sli_radioaes_dma_descr_t ccm_desc_fetcher_data = {
    .address       = (uint32_t) (length > 0 ? data_in : ver_failed),
    .nextDescr     = (uint32_t) ((encrypt || tag_length == 0) ? DMA_AXI_DESCR_END_POINTER : &ccm_desc_fetcher_tag),
    .lengthAndIrq  = (uint32_t) (length > 0 ? length : data_pad_bytes)
                     | BLOCK_S_INCR_ADDR
                     | BLOCK_S_REALIGN_DATA,
    .tag           = DMA_SG_ENGINESELECT_BA411E
                     | DMA_SG_TAG_ISDATA
                     | DMA_SG_TAG_DATATYPE_AESPAYLOAD
                     | ((encrypt || tag_length == 0) ? DMA_SG_TAG_ISLAST : 0)
                     | DMA_SG_TAG_SETINVALIDBYTES(data_pad_bytes),
  };

  // Possible CCM AAD block (concatenated with the header). Can be zero-length, in which case
  // this descriptor should not be referenced but rather bypassed to data.
  sli_radioaes_dma_descr_t ccm_desc_fetcher_add = {
    .address       = (uint32_t) add_data,
    .nextDescr     = (uint32_t) &ccm_desc_fetcher_data,
    .lengthAndIrq  = (uint32_t) add_length
                     | BLOCK_S_INCR_ADDR
                     | BLOCK_S_REALIGN_DATA,
    .tag           = DMA_SG_ENGINESELECT_BA411E
                     | DMA_SG_TAG_ISDATA
                     | DMA_SG_TAG_DATATYPE_AESHEADER
                     | DMA_SG_TAG_SETINVALIDBYTES(header_pad_bytes)
  };

  // Header input block. Always present.
  sli_radioaes_dma_descr_t ccm_desc_fetcher_header = {
    .address       = (uint32_t) header,
    .nextDescr     = (uint32_t) (add_length > 0 ? &ccm_desc_fetcher_add : &ccm_desc_fetcher_data),
    .lengthAndIrq  = (uint32_t) header_length
                     | BLOCK_S_INCR_ADDR
                     | (add_length > 0 ? 0 : BLOCK_S_REALIGN_DATA),
    .tag           = DMA_SG_ENGINESELECT_BA411E
                     | DMA_SG_TAG_ISDATA
                     | DMA_SG_TAG_DATATYPE_AESHEADER
                     | (add_length > 0 ? 0 : DMA_SG_TAG_SETINVALIDBYTES(header_pad_bytes))
  };

  // Key input block. Always present.
  sli_radioaes_dma_descr_t ccm_desc_fetcher_key = {
    .address       = (uint32_t) key,
    .nextDescr     = (uint32_t) &ccm_desc_fetcher_header,
    .lengthAndIrq  = (uint32_t) AES_128_KEY_BYTES
                     | BLOCK_S_INCR_ADDR
                     | BLOCK_S_REALIGN_DATA,
    .tag           = DMA_SG_ENGINESELECT_BA411E
                     | DMA_SG_TAG_ISCONFIG
                     | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_KEY)
  };

  // Operation configuration word block. Always present.
  sli_radioaes_dma_descr_t ccm_desc_fetcher_config = {
    .address       = (uint32_t) (encrypt ? &aes_ccm_config_encrypt : &aes_ccm_config_decrypt),
    .nextDescr     = (uint32_t) &ccm_desc_fetcher_key,
    .lengthAndIrq  = (uint32_t) RADIOAES_CONFIG_BYTES
                     | BLOCK_S_INCR_ADDR
                     | BLOCK_S_REALIGN_DATA,
    .tag           = DMA_SG_ENGINESELECT_BA411E
                     | DMA_SG_TAG_ISCONFIG
                     | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_CFG)
  };

  // Pushers

  // Tag / verification output padding, only if 0 < tag length < 16 bytes.
  sli_radioaes_dma_descr_t ccm_desc_pusher_final_padding = {
    .address       = (uint32_t) NULL,
    .nextDescr     = (uint32_t) DMA_AXI_DESCR_END_POINTER,
    .lengthAndIrq  = (uint32_t) (AES_BLOCK_BYTES - tag_length)
                     | DMA_AXI_DESCR_DISCARD
  };

  // Tag output. Direct into tag buffer for encrypt, into local buffer for
  // decrypt-and-verify. This descriptor is not referenced with tag_length == 0 (CCM-*)
  sli_radioaes_dma_descr_t ccm_desc_pusher_tag = {
    .address       = (uint32_t) (encrypt ? tag : (unsigned char *)ver_failed),
    .nextDescr     = (uint32_t) ((AES_BLOCK_BYTES - tag_length) > 0 ? &ccm_desc_pusher_final_padding : DMA_AXI_DESCR_END_POINTER),
    .lengthAndIrq  = (uint32_t) tag_length
  };

  // Data padding output. There's guaranteed always at least one of data or data padding.
  sli_radioaes_dma_descr_t ccm_desc_pusher_data_padding = {
    .address       = (uint32_t) NULL,
    .nextDescr     = (uint32_t) (tag_length > 0 ? &ccm_desc_pusher_tag : DMA_AXI_DESCR_END_POINTER),
    .lengthAndIrq  = (uint32_t) data_pad_bytes
                     | DMA_AXI_DESCR_DISCARD,
  };

  // Data (ciphertext/plaintext) output. Pointer can be NULL, in which case we tell the
  // DMA to discard the data.
  sli_radioaes_dma_descr_t ccm_desc_pusher_data = {
    .address       = (uint32_t) data_out,
    .nextDescr     = (uint32_t) (data_pad_bytes > 0 ? &ccm_desc_pusher_data_padding : (tag_length > 0 ? &ccm_desc_pusher_tag : DMA_AXI_DESCR_END_POINTER)),
    .lengthAndIrq  = (uint32_t) length
                     | (data_out == NULL ? DMA_AXI_DESCR_DISCARD : 0),
  };

  // Discard all AAD input (which is reflected back to the output). There's guaranteed always a header.
  sli_radioaes_dma_descr_t ccm_desc_pusher_header_add = {
    .address       = (uint32_t) NULL,
    .nextDescr     = (uint32_t) (length > 0 ? &ccm_desc_pusher_data : &ccm_desc_pusher_data_padding),
    .lengthAndIrq  = (uint32_t) (header_length + add_length + header_pad_bytes)
                     | DMA_AXI_DESCR_DISCARD
  };

  sl_status_t status = sli_radioaes_run_operation(&ccm_desc_fetcher_config, &ccm_desc_pusher_header_add);

  if (status != SL_STATUS_OK) {
    return status;
  }

  // Check MIC
  if (!encrypt) {
    uint32_t accumulator = 0;
    for (size_t i = 0; i < tag_length; i++) {
      accumulator |= ver_failed[i];
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
  uint8_t b0b1[19];

  // Fill in B0 block according to BLE spec
  b0b1[0] = 0x49U;

  // Copy in the 13 bytes of nonce
  for (size_t i = 0; i < 13; i++) {
    b0b1[i + 1] = iv[i];
  }

  b0b1[14] = (uint8_t) length >> 8;
  b0b1[15] = (uint8_t) length;
  b0b1[16] = 0; // upper octet of AAD length
  b0b1[17] = 1; // lower octet of AAD length (BLE CCM always has only one byte of AAD)
  b0b1[18] = header; // AAD

  return aes_ccm_radio(encrypt,
                       NULL, 0,
                       data, data, length,
                       key,
                       b0b1, sizeof(b0b1),
                       tag, 4);
}

sl_status_t sli_aes_crypt_ctr_radio(const unsigned char    *key,
                                    unsigned int           keybits,
                                    const unsigned char    input[AES_BLOCK_BYTES],
                                    const unsigned char    iv_in[AES_BLOCK_BYTES],
                                    volatile unsigned char iv_out[AES_BLOCK_BYTES],
                                    volatile unsigned char output[AES_BLOCK_BYTES])
{
  uint32_t aes_config;
  static const uint32_t zero = 0;

  switch (keybits) {
    case 256:
      aes_config = AES_MODEID_CTR | AES_MODEID_CX_LOAD | (((uint32_t)iv_out != 0) ? AES_MODEID_CX_SAVE : 0) | AES_MODEID_AES256;
      break;
    case 192:
      return SL_STATUS_NOT_SUPPORTED;
    case 128:
      aes_config = AES_MODEID_CTR | AES_MODEID_CX_LOAD | (((uint32_t)iv_out != 0) ? AES_MODEID_CX_SAVE : 0) | AES_MODEID_AES128;
      break;
    default:
      return SL_STATUS_INVALID_KEY;
  }

  sli_radioaes_dma_descr_t aes_desc_pusher_ctx = {
    .address       = (uint32_t) iv_out,
    .nextDescr     = DMA_AXI_DESCR_NEXT_STOP,
    .lengthAndIrq  = AES_BLOCK_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISLAST
  };

  sli_radioaes_dma_descr_t aes_desc_pusher_data = {
    .address       = (uint32_t) output,
    .nextDescr     = (((uint32_t)iv_out != 0) ? (uint32_t) &aes_desc_pusher_ctx : DMA_AXI_DESCR_NEXT_STOP),
    .lengthAndIrq  = AES_BLOCK_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA
  };

  sli_radioaes_dma_descr_t aes_desc_fetcher_data = {
    .address       = (uint32_t) input,
    .nextDescr     = DMA_AXI_DESCR_NEXT_STOP,
    .lengthAndIrq  = AES_BLOCK_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISLAST | DMA_SG_TAG_ISDATA | DMA_SG_TAG_DATATYPE_AESPAYLOAD
  };

  sli_radioaes_dma_descr_t aes_desc_fetcher_no_ctx = {
    .address       = (uint32_t) &zero,
    .nextDescr     = (uint32_t) &aes_desc_fetcher_data,
    .lengthAndIrq  = AES_BLOCK_BYTES | (BLOCK_S_CONST_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_IV)
  };

  sli_radioaes_dma_descr_t aes_desc_fetcher_ctx = {
    .address       = (uint32_t) iv_in,
    .nextDescr     = (uint32_t) &aes_desc_fetcher_data,
    .lengthAndIrq  = AES_BLOCK_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_IV)
  };

  sli_radioaes_dma_descr_t aes_desc_fetcher_config = {
    .address       = (uint32_t) &aes_config,
    .nextDescr     = (((uint32_t)iv_in != 0) ? (uint32_t) &aes_desc_fetcher_ctx : (uint32_t) &aes_desc_fetcher_no_ctx),
    .lengthAndIrq  = sizeof(aes_config),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_CFG)
  };

  sli_radioaes_dma_descr_t aes_desc_fetcher_key = {
    .address       = (uint32_t) key,
    .nextDescr     = (uint32_t) &aes_desc_fetcher_config,
    .lengthAndIrq  = (uint32_t) (keybits / 8) | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_KEY)
  };

  // Start operation
  return sli_radioaes_run_operation(&aes_desc_fetcher_key, &aes_desc_pusher_data);
}

sl_status_t sli_aes_crypt_ecb_radio(bool                   encrypt,
                                    const unsigned char    *key,
                                    unsigned int           keybits,
                                    const unsigned char    input[AES_BLOCK_BYTES],
                                    volatile unsigned char output[AES_BLOCK_BYTES])
{
  uint32_t aes_config;

  switch (keybits) {
    case 256:
      aes_config = AES_MODEID_ECB | AES_MODEID_NO_CX | AES_MODEID_AES256;
      break;
    case 192:
      return SL_STATUS_NOT_SUPPORTED;
    case 128:
      aes_config = AES_MODEID_ECB | AES_MODEID_NO_CX | AES_MODEID_AES128;
      break;
    default:
      return SL_STATUS_INVALID_KEY;
  }

  aes_config |= encrypt ? AES_MODEID_ENCRYPT : AES_MODEID_DECRYPT;

  sli_radioaes_dma_descr_t aes_desc_pusher_data = {
    .address       = (uint32_t) output,
    .nextDescr     = DMA_AXI_DESCR_NEXT_STOP,
    .lengthAndIrq  = AES_BLOCK_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISLAST
  };

  sli_radioaes_dma_descr_t aes_desc_fetcher_data = {
    .address       = (uint32_t) input,
    .nextDescr     = DMA_AXI_DESCR_NEXT_STOP,
    .lengthAndIrq  = AES_BLOCK_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISLAST | DMA_SG_TAG_ISDATA | DMA_SG_TAG_DATATYPE_AESPAYLOAD
  };

  sli_radioaes_dma_descr_t aes_desc_fetcher_config = {
    .address       = (uint32_t) &aes_config,
    .nextDescr     = (uint32_t) &aes_desc_fetcher_data,
    .lengthAndIrq  = sizeof(aes_config),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_CFG)
  };

  sli_radioaes_dma_descr_t aes_desc_fetcher_key = {
    .address       = (uint32_t) key,
    .nextDescr     = (uint32_t) &aes_desc_fetcher_config,
    .lengthAndIrq  = (uint32_t) (keybits / 8) | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_KEY)
  };

  // Start operation
  return sli_radioaes_run_operation(&aes_desc_fetcher_key, &aes_desc_pusher_data);
}

sl_status_t sli_aes_cmac_radio(const unsigned char    *key,
                               unsigned int           keybits,
                               const unsigned char    *input,
                               unsigned int           length,
                               volatile unsigned char output[16])
{
  uint32_t aes_config;

  switch (keybits) {
    case 256:
      aes_config = AES_MODEID_CMA | AES_MODEID_NO_CX | AES_MODEID_AES256 | AES_MODEID_ENCRYPT;
      break;
    case 192:
      return SL_STATUS_NOT_SUPPORTED;
    case 128:
      aes_config = AES_MODEID_CMA | AES_MODEID_NO_CX | AES_MODEID_AES128 | AES_MODEID_ENCRYPT;
      break;
    default:
      return SL_STATUS_INVALID_KEY;
  }

  size_t pad_len = 16 - (length % 16);
  if (pad_len == 16 && length > 0) {
    pad_len = 0;
  }

  if (length == 0) {
    length = 16UL;
    input = (const unsigned char *)&zeros;
  } else {
    length = (length + 15) & ~0xFUL;
  }

  sli_radioaes_dma_descr_t aes_desc_pusher_data = {
    .address       = (uint32_t) output,
    .nextDescr     = DMA_AXI_DESCR_NEXT_STOP,
    .lengthAndIrq  = AES_BLOCK_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISLAST
  };

  sli_radioaes_dma_descr_t aes_desc_fetcher_data = {
    .address       = (uint32_t) input,
    .nextDescr     = DMA_AXI_DESCR_NEXT_STOP,
    .lengthAndIrq  = length | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISLAST | DMA_SG_TAG_ISDATA | DMA_SG_TAG_DATATYPE_AESPAYLOAD | DMA_SG_TAG_SETINVALIDBYTES(pad_len)
  };

  sli_radioaes_dma_descr_t aes_desc_fetcher_config = {
    .address       = (uint32_t) &aes_config,
    .nextDescr     = (uint32_t) &aes_desc_fetcher_data,
    .lengthAndIrq  = sizeof(aes_config),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_CFG)
  };

  sli_radioaes_dma_descr_t aes_desc_fetcher_key = {
    .address       = (uint32_t) key,
    .nextDescr     = (uint32_t) &aes_desc_fetcher_config,
    .lengthAndIrq  = (uint32_t) (keybits / 8) | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_KEY)
  };

  // Start operation
  return sli_radioaes_run_operation(&aes_desc_fetcher_key, &aes_desc_pusher_data);
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
  // length will always fit into a 16-bit length field, meaning
  // the header will always be either 16 or 18 bytes long.
  uint8_t header[18];

  // Start with the 'flags' byte. It encodes whether there is AAD,
  // and the length of the tag fields
  header[0] = 0x01 // always 2 bytes of message length
              | ((aad_len > 0) ? 0x40 : 0x00) // Set 'aflag' bit if there is AAD
              | ((tag_len >= 4) ? (((tag_len - 2) / 2) << 3) : 0); // Encode tag length

  for (size_t i = 0; i < 13; i++) {
    header[i + 1] = iv[i];
  }

  header[14] = (uint8_t) length >> 8;
  header[15] = (uint8_t) length;
  if (aad_len > 0) {
    header[16] = (uint8_t) aad_len >> 8; // upper octet of AAD length
    header[17] = (uint8_t) aad_len; // lower octet of AAD length
  }

  return aes_ccm_radio(encrypt,
                       aad,
                       aad_len,
                       data_in,
                       data_out,
                       length,
                       key,
                       header,
                       (aad_len > 0 ? 18 : 16),
                       tag,
                       tag_len);
}

//
// Process a table of BLE RPA device keys and look for a
// match against the supplied hash. Algorithm is AES-128.
//
int sli_process_ble_rpa(const unsigned char keytable[],
                        uint32_t            keymask,
                        uint32_t            prand,
                        uint32_t            hash)
{
  int block;
  int previous_block = -1, result = -1;
  static const uint32_t  aes_rpa_config = AES_MODEID_ECB
                                          | AES_MODEID_NO_CX
                                          | AES_MODEID_AES128
                                          | AES_MODEID_ENCRYPT;

  uint32_t rpa_data_in[AES_BLOCK_BYTES / sizeof(uint32_t)] = { 0 };
  volatile uint32_t rpa_data_out[AES_BLOCK_BYTES / sizeof(uint32_t)];
  sli_radioaes_state_t aes_ctx;
  CORE_DECLARE_IRQ_STATE;

  rpa_data_in[3] = __REV(prand);

  sli_radioaes_dma_descr_t aes_desc_pusher_data = {
    .address       = (uint32_t) rpa_data_out,
    .nextDescr     = DMA_AXI_DESCR_NEXT_STOP,
    .lengthAndIrq  = AES_BLOCK_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISLAST
  };

  sli_radioaes_dma_descr_t aes_desc_fetcher_data = {
    .address       = (uint32_t) rpa_data_in,
    .nextDescr     = DMA_AXI_DESCR_NEXT_STOP,
    .lengthAndIrq  = AES_BLOCK_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISLAST | DMA_SG_TAG_ISDATA | DMA_SG_TAG_DATATYPE_AESPAYLOAD
  };

  sli_radioaes_dma_descr_t aes_desc_fetcher_config = {
    .address       = (uint32_t) &aes_rpa_config,
    .nextDescr     = (uint32_t) &aes_desc_fetcher_data,
    .lengthAndIrq  = sizeof(aes_rpa_config),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_CFG)
  };

  volatile sli_radioaes_dma_descr_t aes_desc_fetcher_key = {
    .address       = (uint32_t) NULL, // Filled out in each round of RPA check
    .nextDescr     = (uint32_t) &aes_desc_fetcher_config,
    .lengthAndIrq  = (uint32_t) AES_128_KEY_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_KEY)
  };

  // Start operation
  sl_status_t status = sli_radioaes_acquire();
  if (status == SL_STATUS_ISR) {
    sli_radioaes_save_state(&aes_ctx);
  } else if (status != SL_STATUS_OK) {
    return -1;
  }

  RADIOAES->CTRL = AES_CTRL_FETCHERSCATTERGATHER | AES_CTRL_PUSHERSCATTERGATHER;

  #if defined(SLI_RADIOAES_REQUIRES_MASKING)
  // Start with feeding the mask input
  sli_radioaes_dma_descr_t mask_descr = SLI_RADIOAES_MASK_DESCRIPTOR(DMA_AXI_DESCR_NEXT_STOP);
  RADIOAES->FETCHADDR = (uint32_t) &mask_descr;
  RADIOAES->CMD = AES_CMD_STARTFETCHER;
  #endif

  // Start a critical section to avoid preemption in-between loading of the RPA key
  // and starting the corresponding data pusher.
  CORE_ENTER_CRITICAL();

  // Data output contains hash in the most significant word (WORD3).
  // Descriptors for blocks that are not included in key mask will be skipped.
  for (block = 0; block < RADIOAES_BLE_RPA_MAX_KEYS; block++) {
    if ( keymask & (1U << block) ) {  // Skip masked keys
      // Handle pending interrupts while the peripheral is in 'preemptable' state
      CORE_YIELD_CRITICAL();
      // Write key address and start operation
      while (RADIOAES->STATUS & AES_STATUS_FETCHERBSY) {
        // Wait for completion
      }
      aes_desc_fetcher_key.address = (uint32_t) &keytable[block * AES_128_KEY_BYTES];
      RADIOAES->FETCHADDR = (uint32_t) &aes_desc_fetcher_key;

      RADIOAES->CMD = AES_CMD_STARTFETCHER;

      // Wait for pusher from previous round to finish
      while (RADIOAES->STATUS & AES_STATUS_PUSHERBSY) {
        // Wait for completion
      }
      RADIOAES->PUSHADDR  = (uint32_t) &aes_desc_pusher_data;

      // Check previous results while AES is processing
      if ((previous_block >= 0) && ((rpa_data_out[3] & 0xFFFFFF00) == __REV(hash)) ) {
        // Make sure AES is finished before returning
        RADIOAES->CMD = AES_CMD_STARTPUSHER;
        result = previous_block;
        break;
      }

      // Start pusher so it is ready to push results when encryption is done
      RADIOAES->CMD = AES_CMD_STARTPUSHER;
      previous_block = block;
    }
  }

  CORE_EXIT_CRITICAL();

  // Wait for last data and check it
  while (RADIOAES->STATUS & AES_STATUS_PUSHERBSY) {
    // Wait for completion
  }

  if (status == SL_STATUS_ISR) {
    sli_radioaes_restore_state(&aes_ctx);
  }

  sli_radioaes_release();

  if (result >= 0) {
    return result;
  }

  if ((rpa_data_out[3] & 0xFFFFFF00) == __REV(hash) ) {
    return previous_block;
  }

  // No match
  return -1;
}

void sli_aes_seed_mask(void)
{
  // Acquiring and releasing the peripheral should ensure the mask is properly
  // set.
  (void) sli_radioaes_acquire();
  (void) sli_radioaes_release();
}

/// @endcond
#endif // defined(RADIOAES_PRESENT)
