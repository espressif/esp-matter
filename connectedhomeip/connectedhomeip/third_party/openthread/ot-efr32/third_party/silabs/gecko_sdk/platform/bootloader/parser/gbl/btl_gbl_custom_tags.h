/***************************************************************************//**
 * @file
 * @brief Custom tags for Gecko Bootloader GBL parser
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
#ifndef BTL_GBL_CUSTOM_TAGS_H
#define BTL_GBL_CUSTOM_TAGS_H

#include "btl_gbl_parser.h"
#include "btl_gbl_format.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @addtogroup Components
 * @{
 * @addtogroup ImageParser
 * @{
 * @addtogroup GblParser
 * @{
 * @addtogroup CustomTags Custom GBL Tags
 * @brief Handle custom GBL tags for added functionality in the GBL parser
 * @details
 * @{
 */

/// Custom tag descriptor
typedef struct {
  /// GBL Tag ID of the custom tag
  uint32_t tagId;
  /// Function to call upon entering the tag
  int32_t (*enterTag)(ParserContext_t *ctx);
  /// Function to call while parsing the tag
  int32_t (*parseTag)(ParserContext_t *ctx,
                      void *data,
                      size_t length,
                      const BootloaderParserCallbacks_t *callbacks);
  /// Function to call upon exiting the tag
  int32_t (*exitTag)(ParserContext_t *ctx,
                     const BootloaderParserCallbacks_t *callbacks);
  /// @brief Function returning how many bytes should be collected before
  ///        calling parseTag the next time
  size_t (*numBytesRequired)(ParserContext_t *ctx);
} GblCustomTag_t;

/***************************************************************************//**
 * Indicate whether the GBL tag described by tagHeader is a custom tag.
 * @param tagHeader Pointer to the GBL tag header
 *
 * @return True if the tag is a custom tag, else false
 ******************************************************************************/
bool gbl_isCustomTag(GblTagHeader_t *tagHeader);

/***************************************************************************//**
 * Get properties for a custom GBL tag.
 * @param tagId GBL Tag ID of the custom tag
 *
 * @return Pointer to the custom tag descriptor
 ******************************************************************************/
const GblCustomTag_t * gbl_getCustomTagProperties(uint32_t tagId);

/** @} addtogroup CustomTags */
/** @} addtogroup GblParser */
/** @} addtogroup ImageParser */
/** @} addtogroup Components */

#endif // BTL_GBL_CUSTOM_TAGS_H
