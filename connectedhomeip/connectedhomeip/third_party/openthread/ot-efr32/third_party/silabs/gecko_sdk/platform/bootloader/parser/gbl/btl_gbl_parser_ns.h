/***************************************************************************//**
 * @file
 * @brief Non-secure GBL image parser library.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef BTL_GBL_PARSER_NS_H
#define BTL_GBL_PARSER_NS_H

// -----------------------------------------------------------------------------
// Includes

#include <inttypes.h>
#include <stddef.h>

// -----------------------------------------------------------------------------
// Defines

/// GBL file is encrypted
#define PARSER_FLAG_ENCRYPTED               (1U << 0U)
/// Parse custom tags rather than silently traversing them
#define PARSER_FLAG_PARSE_CUSTOM_TAGS       (1U << 5U)
/// Upgrade image contains application upgrade
#define BTL_IMAGE_CONTENT_APPLICATION       0x01U
/// Upgrade image contains bootloader upgrade
#define BTL_IMAGE_CONTENT_BOOTLOADER        0x02U
/// Upgrade image contains SE upgrade
#define BTL_IMAGE_CONTENT_SE                0x04U

// -----------------------------------------------------------------------------
// Structs

/// Structure containing state of the image file processed
typedef struct {
  /// Image contents
  uint8_t                     contents;
  /// Flag to indicate parsing has completed
  bool                        imageCompleted;
  /// Flag to indicate the image file has been validated
  bool                        imageVerified;
  /// Version number of main bootloader extracted from image file
  uint32_t                    bootloaderVersion;
  /// Size of the bootloader upgrade contained in the image file
  uint32_t                    bootloaderUpgradeSize;
#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
  /// Version number of SE upgrade extracted from image file
  uint32_t                    seUpgradeVersion;
#endif
} ImageProperties_t;

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
// Empty parser context definition
typedef struct _ParserContext ParserContext_t;

// -----------------------------------------------------------------------------
// Function declarations

int32_t parser_init(uint8_t flags);
int32_t parser_parse(uint8_t buffer[],
                     size_t length,
                     ImageProperties_t *prop);
/** @endcond */

#endif // BTL_GBL_PARSER_NS_H