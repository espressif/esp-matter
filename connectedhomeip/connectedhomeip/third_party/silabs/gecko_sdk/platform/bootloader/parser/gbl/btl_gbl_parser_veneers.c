/***************************************************************************//**
 * @file
 * @brief Non-secure GBL image parser veneer library.
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

// -----------------------------------------------------------------------------
// Includes

#include <string.h>
#include "core/btl_tz_utils.h"
#include "core/btl_bootload.h"
#include "parser/gbl/btl_gbl_parser.h"
#include "security/btl_security_types.h"

// -----------------------------------------------------------------------------
// Static

static const BootloaderParserCallbacks_t parse_Cb = {
  .context = NULL,
  .applicationCallback = bootload_applicationCallback,
  .metadataCallback = NULL,
  .bootloaderCallback = bootload_bootloaderCallback
};

static ImageProperties_t image_properties = {
  .contents = 0U,
  .instructions = 0xFFU,
  .imageCompleted = false,
  .imageVerified = false,
  .bootloaderVersion = 0,
  .application = { 0 },
#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
  .seUpgradeVersion = 0
#endif
};

static ParserContext_t parser_context = { 0 };
static DecryptContext_t decrypt_context = { 0 };
static AuthContext_t auth_context = { 0 };

// -----------------------------------------------------------------------------
// NSC functions

__attribute__((cmse_nonsecure_entry))
int32_t parser_nsc_init(uint8_t flags)
{
  memset(&image_properties, 0u, sizeof(image_properties));
  image_properties.instructions = 0xFFU;
  image_properties.imageCompleted = false;
  image_properties.imageVerified = false;
  return parser_init(&parser_context, &decrypt_context, &auth_context, flags);
}

__attribute__((cmse_nonsecure_entry))
int32_t parser_nsc_parse(uint8_t buffer[],
                         size_t length,
                         void *prop)
{
  ImageProperties_NS_t *ns_image_prop = (ImageProperties_NS_t *)prop;
  int32_t error_code = BOOTLOADER_ERROR_PARSER_UNEXPECTED;

  if (!bl_verify_ns_memory_access(buffer, length)) {
    bl_fatal_assert_action();
  }

  error_code = parser_parse(&parser_context,
                            &image_properties,
                            buffer,
                            length,
                            &parse_Cb);

  ns_image_prop->contents = image_properties.contents;
  ns_image_prop->imageCompleted = image_properties.imageCompleted;
  ns_image_prop->imageVerified = image_properties.imageVerified;
  ns_image_prop->bootloaderVersion = image_properties.bootloaderVersion;
  ns_image_prop->bootloaderUpgradeSize = image_properties.bootloaderUpgradeSize;
#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
  ns_image_prop->seUpgradeVersion = image_properties.seUpgradeVersion;
#endif

  return error_code;
}
