/***************************************************************************//**
 * @file
 * @brief Application interface to the bootloader parser.
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

#ifndef BTL_INTERFACE_PARSER_H
#define BTL_INTERFACE_PARSER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/***************************************************************************//**
 * @addtogroup Interface
 * @{
 * @addtogroup ParserInterface Application Parser Interface
 * @brief Application interface for the bootloader image
 *        parser.
 * @details The Parser Interface can be used to parse upgrade images from the
 *          context of the application.
 * @{
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Typedefs

/***************************************************************************//**
 * Bootloader parser callback
 *
 * @param address Address of the data
 * @param data    Raw data
 * @param length  Size in bytes of raw data.
 * @param context A context variable defined by the implementation that
 *                is implementing this callback.
 ******************************************************************************/
typedef void (*BootloaderParserCallback_t)(uint32_t address,
                                           uint8_t  *data,
                                           size_t   length,
                                           void     *context);

/// Context for the bootloader image parser routine.
typedef struct BootloaderParserContext BootloaderParserContext_t;

/// Function pointers to parser callbacks
typedef struct {
  /// Opaque pointer passed to the callback functions
  void                        *context;
  /// Callback function pointer for application image data
  BootloaderParserCallback_t  applicationCallback;
  /// Callback function pointer for image metadata
  BootloaderParserCallback_t  metadataCallback;
  /// Callback function pointer for bootloader upgrade image data
  BootloaderParserCallback_t  bootloaderCallback;
} BootloaderParserCallbacks_t;

/***************************************************************************//**
 * Initialize the image parser.
 *
 * @param[in] context     Pointer to the parser context struct.
 * @param[in] contextSize Size of the context struct.
 *
 * @return BOOTLOADER_OK if success, BOOTLOADER_ERROR_PARSE_CONTEXT if context
 *         struct is too small.
 ******************************************************************************/
int32_t bootloader_initParser(BootloaderParserContext_t *context,
                              size_t                    contextSize);

#if !defined(SL_TRUSTZONE_NONSECURE)
/***************************************************************************//**
 * Parse a buffer.
 * @param[in] context   Pointer to the parser context struct.
 * @param[in] callbacks Callbacks to be called by the parser.
 * @param[in] data      Data to be parsed.
 * @param[in] numBytes  Size of the data buffer.
 *
 * @return BOOTLOADER_ERROR_PARSE_CONTINUE if the chunk was parsed correctly,
 *         and a new chunk is expected. BOOTLOADER_ERROR_PARSE_ERROR if
 *         something went wrong while parsing. BOOTLOADER_ERROR_PARSE_SUCCESS
 *         if the entire file was successfully parsed.
 ******************************************************************************/
int32_t bootloader_parseBuffer(BootloaderParserContext_t   *context,
                               BootloaderParserCallbacks_t *callbacks,
                               uint8_t                     data[],
                               size_t                      numBytes);
#endif // SL_TRUSTZONE_NONSECURE

/***************************************************************************//**
 * Parse a buffer and get application and bootloader upgrade metadata
 * from the buffer.
 *
 * @note \p appInfo and \p bootloaderVersion will default to zeros.
 *
 * @param[in]  context           Pointer to the parser context struct.
 * @param[in]  data              Data to be parsed.
 * @param[in]  numBytes          Size of the data buffer.
 * @param[out] appInfo           Pointer to @ref ApplicationData_t struct.
 * @param[out] bootloaderVersion Pointer to an integer representing bootloader
 *                               version.
 *
 * @return @ref BOOTLOADER_OK if metadata was filled successfully.
 ******************************************************************************/
#if !defined(SL_TRUSTZONE_NONSECURE)
int32_t bootloader_parseImageInfo(BootloaderParserContext_t *context,
                                  uint8_t                   data[],
                                  size_t                    numBytes,
                                  ApplicationData_t         *appInfo,
                                  uint32_t                  *bootloaderVersion);
#else
int32_t bootloader_parseImageInfo(uint8_t                   data[],
                                  size_t                    numBytes,
                                  ApplicationData_t         *appInfo,
                                  uint32_t                  *bootloaderVersion);
#endif // SL_TRUSTZONE_NONSECURE

/***************************************************************************//**
 * Find the size of the context struct BootloaderParserContext used by the bootloader
 * image parser to store parser state.
 *
 * @return size of BootloaderParserContext, returns 0 if something went wrong.
 ******************************************************************************/
uint32_t bootloader_parserContextSize(void);

/** @} (end addtogroup ParserInterface) */
/** @} (end addtogroup Interface) */

#endif // BTL_INTERFACE_PARSER_H
