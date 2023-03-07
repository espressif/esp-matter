/***************************************************************************//**
 * @file
 * @brief LZMA decompression functionality for Gecko Bootloader
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
#ifndef BTL_DECOMPRESS_LZMA_H
#define BTL_DECOMPRESS_LZMA_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "parser/gbl/btl_gbl_parser.h"
#include "api/btl_errorcode.h"

#include "lzma/LzmaDec.h"

/**
 * @addtogroup Components
 * @{
 * @addtogroup ImageParser
 * @{
 * @addtogroup GblParser
 * @{
 * @addtogroup CustomTags Custom GBL Tags
 * @{
 * @addtogroup LzmaProgTag LZMA Programming Tag
 * @brief Tag to handle LZMA compressed programming data
 * @details
 * @{
 */

#ifndef LZMA_COUNTER_SIZE_KB
/// @brief The maximum size of the array holding probability model counters.
/// The size given here sets the limit for the size of the LC and LP constants
/// used by the LZMA compressor. The necessary size of the counter array can be
/// found from size = 4 KiB + 1.5 KiB * (1 << (LC + LP)).
/// LZMA payloads with too large LC + LP can't be decompressed.
#define LZMA_COUNTER_SIZE_KB        (10UL)
#endif

#ifndef LZMA_DICT_SIZE_KB
/// @brief The maximum size of the dictionary.
/// The size given here sets the limit for the size of the dictionary used by
/// the LZMA compressor.
/// LZMA payloads with a dictionary that's too large  can't be decompressed.
#define LZMA_DICT_SIZE_KB           (8UL)
#endif

/***************************************************************************//**
 * Enter an LZMA compressed programming tag.
 * @param ctx Parser context
 *
 * @return Error code
 ******************************************************************************/
int32_t gbl_lzmaEnterProgTag(ParserContext_t *ctx);

/***************************************************************************//**
 * Parse a chunk of data from an LZMA compressed programming tag.
 * @param ctx       Parser context
 * @param data      Input data to parse
 * @param length    Length of data
 * @param callbacks Callbacks to call with parsed data
 *
 * @return Error code
 ******************************************************************************/
int32_t gbl_lzmaParseProgTag(ParserContext_t                   *ctx,
                             void                              *data,
                             size_t                            length,
                             const BootloaderParserCallbacks_t *callbacks);

/***************************************************************************//**
 * Exit an LZMA compressed programming tag.
 * @param ctx       Parser context
 * @param callbacks Callbacks to call with parsed data
 *
 * @return Error code
 ******************************************************************************/
int32_t gbl_lzmaExitProgTag(ParserContext_t                   *ctx,
                            const BootloaderParserCallbacks_t *callbacks);

/***************************************************************************//**
 * Number of bytes needed for next stage of parsing.
 * @param ctx Parser context
 *
 * @return Number of bytes required
 ******************************************************************************/
size_t gbl_lzmaNumBytesRequired(ParserContext_t *ctx);

/** @} addtogroup LzmaProgTag */
/** @} addtogroup CustomTags */
/** @} addtogroup GblParser */
/** @} addtogroup ImageParser */
/** @} addtogroup Components */

#endif // BTL_DECOMPRESS_LZMA_H
