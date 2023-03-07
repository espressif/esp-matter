/***************************************************************************//**
 * @file
 * @brief Functions to use the parser component  to parse an image
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

#include "btl_parse.h"

#include "api/btl_errorcode.h"

#include <string.h>

int32_t core_initParser(BootloaderParserContext_t *context,
                        size_t                    contextSize)
{
  if (sizeof(BootloaderParserContext_t) > contextSize) {
    // Context is not large enough
    return BOOTLOADER_ERROR_PARSE_CONTEXT;
  }

  // Zero the context
  memset(context, 0, sizeof(BootloaderParserContext_t));

  // Perform no operations
  context->imageProperties.instructions = 0x00U;

  return parser_init(&(context->parserContext),
                     &(context->decryptContext),
                     &(context->authContext),
                     0U);
}

int32_t core_parseBuffer(BootloaderParserContext_t         *context,
                         const BootloaderParserCallbacks_t *callbacks,
                         uint8_t                           data[],
                         size_t                            numBytes)
{
  context->errorCode = parser_parse(&(context->parserContext),
                                    &(context->imageProperties),
                                    data,
                                    numBytes,
                                    callbacks);

  if ((context->errorCode != BOOTLOADER_OK)
      && (context->errorCode != BOOTLOADER_ERROR_PARSER_EOF)) {
    // Unexpected error code from parser
    return context->errorCode;
  } else {
    if (context->imageProperties.imageCompleted) {
      // Done parsing image
      if (context->imageProperties.imageVerified) {
        // Verification passed
        return BOOTLOADER_ERROR_PARSE_SUCCESS;
      } else {
        // Verification failed
        return BOOTLOADER_ERROR_PARSE_FAILED;
      }
    } else {
      // Parsing not complete; request more data
      return BOOTLOADER_ERROR_PARSE_CONTINUE;
    }
  }
}

int32_t core_parseImageInfo(BootloaderParserContext_t *context,
                            uint8_t                    data[],
                            size_t                     numBytes,
                            ApplicationData_t          *appInfo,
                            uint32_t                   *bootloaderVersion)
{
  const BootloaderParserCallbacks_t parseCb = {
    .context = NULL,
    .applicationCallback = NULL,
    .metadataCallback = NULL,
    .bootloaderCallback = NULL,
  };

  // Default versions to zero
  *bootloaderVersion = 0UL;
  memset(appInfo, 0, sizeof(ApplicationData_t));

  (void)parser_parse(&(context->parserContext),
                     &(context->imageProperties),
                     data,
                     numBytes,
                     &parseCb);

  if (context->imageProperties.contents & BTL_IMAGE_CONTENT_BOOTLOADER) {
    *bootloaderVersion = context->imageProperties.bootloaderVersion;
  }
  if (context->imageProperties.contents & BTL_IMAGE_CONTENT_APPLICATION) {
    memcpy(appInfo,
           &context->imageProperties.application,
           sizeof(ApplicationData_t));
  }

  if (context->imageProperties.contents) {
    return BOOTLOADER_OK;
  } else {
    return BOOTLOADER_ERROR_STORAGE_NO_IMAGE;
  }
}

uint32_t core_parserContextSize(void) {
  return sizeof(BootloaderParserContext_t);
}
