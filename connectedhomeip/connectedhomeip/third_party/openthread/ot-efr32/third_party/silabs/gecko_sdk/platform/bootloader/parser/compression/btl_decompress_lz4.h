/***************************************************************************//**
 * @file
 * @brief LZ4 decompression functionality for Gecko Bootloader
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#ifndef BTL_DECOMPRESS_LZ4_H
#define BTL_DECOMPRESS_LZ4_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "parser/gbl/btl_gbl_parser.h"
#include "api/btl_errorcode.h"

/**
 * @addtogroup Components
 * @{
 * @addtogroup Decompressor Decompressor
 * @brief Decompressors for Gecko Bootloader
 * @details
 * @{
 * @addtogroup Lz4Decompressor LZ4 Decompressor
 * @brief LZ4 decompressor
 * @details
 * @{
 */

/// LZ4 state machine: Token byte
#define LZ4_STATE_TOKEN           00U
/// LZ4 state machine: Literal length byte
#define LZ4_STATE_LITERAL_LENGTH  10U
/// LZ4 state machine: Literal value byte
#define LZ4_STATE_LITERAL_VALUE   20U
/// LZ4 state machine: LSB of match offset
#define LZ4_STATE_OFFSET_LSB      30U
/// LZ4 state machine: MSB of match offset
#define LZ4_STATE_OFFSET_MSB      35U
/// LZ4 state machine: Match length
#define LZ4_STATE_MATCH_LENGTH    40U
/// LZ4 state machine: Backtracking to get matched data
#define LZ4_STATE_BACKTRACKING    50U

/// Function to output data from LZ4 decompressor
typedef int32_t (*Lz4DataWrite_t)(uint8_t *data, size_t length);
/// Function to read data into LZ4 decompressor
typedef int32_t (*Lz4DataRead_t)(size_t backtrackOffset, uint8_t *data, size_t length);

/// LZ4 decompressor context
typedef struct {
  /// Length of literals
  uint32_t literalLength;
  /// Length of match
  uint32_t matchLength;
  /// Offset from current write for match
  uint16_t backtrackOffset;
  /// Current decompressor state
  uint8_t state;
  /// Function to read previously written data into decompressor during backtracking
  Lz4DataRead_t readFunction;
} Lz4Context_t;

/***************************************************************************//**
 * Initialize the LZ4 decompressor.
 * @param ctx          Decompressor context
 * @param readFunction Function pointer to read back previously written data
 *
 * @return Error code
 ******************************************************************************/
int32_t lz4_init(Lz4Context_t *ctx, Lz4DataRead_t readFunction);

/***************************************************************************//**
 * Decompress a chunk of data.
 * @param ctx           Decompressor context
 * @param inputData     Compressed input data
 * @param inputLength   Length of inputData in bytes
 * @param writeFunction Function pointer that is called with decompressed data
 *
 * @return Error code
 ******************************************************************************/
int32_t lz4_decompress(Lz4Context_t *ctx,
                       void *inputData,
                       size_t inputLength,
                       Lz4DataWrite_t writeFunction);

/***************************************************************************//**
 * Finish decompressing data.
 * @param ctx Decompressor context
 *
 * @return Error code indicating success or failure
 * @retval ::BOOTLOADER_OK on success
 * @retval ::BOOTLOADER_ERROR_COMPRESSION_STATE if the last block wasn't
 *         completed
 ******************************************************************************/
int32_t lz4_finish(Lz4Context_t *ctx);

/** @} addtogroup Lz4Decompressor */
/** @} addtogroup Decompressor */
/** @} addtogroup Components */

/**
 * @addtogroup Components
 * @addtogroup ImageParser
 * @{
 * @addtogroup GblParser
 * @{
 * @addtogroup CustomTags Custom GBL Tags
 * @{
 * @addtogroup Lz4ProgTag LZ4 Programming Tag
 * @brief Tag to handle LZ4 compressed programming data
 * @details
 * @{
 */

/// LZ4 Compressed Programming GBL Tag Parser Context
typedef struct {
  /// Buffer to store unaligned decompressed data
  uint8_t outputBuffer[4];
  /// Offset into outputBuffer
  uint8_t outputOffset;
  /// Whether this is the first call to the parser for this tag
  bool    firstCall;
  /// Stored pointer to the GBL parser context
  ParserContext_t *parserContext;
  /// Stored pointer to the GBL parser callbacks
  const BootloaderParserCallbacks_t *parserCallbacks;
  /// Context of the LZ4 decompressor
  Lz4Context_t lz4Context;
} Lz4ParserContext_t;

/***************************************************************************//**
 * Enter an LZ4 compressed programming tag.
 * @param ctx Parser context
 *
 * @return Error code
 ******************************************************************************/
int32_t gbl_lz4EnterProgTag(ParserContext_t *ctx);

/***************************************************************************//**
 * Parse a chunk of data from an LZ4 compressed programming tag.
 * @param ctx       Parser context
 * @param data      Input data to parse
 * @param length    Length of data
 * @param callbacks Callbacks to call with parsed data
 *
 * @return Error code
 ******************************************************************************/
int32_t gbl_lz4ParseProgTag(ParserContext_t                   *ctx,
                            void                              *data,
                            size_t                            length,
                            const BootloaderParserCallbacks_t *callbacks);

/***************************************************************************//**
 * Exit an LZ4 compressed programming tag.
 * @param ctx       Parser context
 * @param callbacks Callbacks to call with parsed data
 *
 * @return Error code
 ******************************************************************************/
int32_t gbl_lz4ExitProgTag(ParserContext_t                   *ctx,
                           const BootloaderParserCallbacks_t *callbacks);

/***************************************************************************//**
 * Number of bytes needed for the next stage of parsing.
 * @param ctx Parser context
 *
 * @return Number of bytes required
 ******************************************************************************/
size_t gbl_lz4NumBytesRequired(ParserContext_t *ctx);

/** @} addtogroup Lz4ProgTag */
/** @} addtogroup CustomTags */
/** @} addtogroup GblParser */
/** @} addtogroup ImageParser */
/** @} addtogroup Components */

#endif // BTL_DECOMPRESS_LZ4_H
