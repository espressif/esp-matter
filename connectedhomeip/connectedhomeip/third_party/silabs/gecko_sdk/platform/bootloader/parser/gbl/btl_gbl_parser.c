/***************************************************************************//**
 * @file
 * @brief GBL image file parser
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
#include <string.h>

#include "config/btl_config.h"

#include "parser/gbl/btl_gbl_parser.h"
#include "btl_gbl_format.h"
#include "btl_gbl_custom_tags.h"

#include "api/btl_interface.h"

#include "security/btl_security_aes.h"
#include "security/btl_security_sha256.h"
#include "security/btl_security_ecdsa.h"
#include "security/btl_crc32.h"
#include "security/btl_security_tokens.h"
#include "security/btl_security_types.h"

#include "core/btl_util.h"
#include "core/btl_bootload.h"

MISRAC_DISABLE
#include "em_device.h"
MISRAC_ENABLE

// Debug
#include "debug/btl_debug.h"

// -----------------------------------------------------------------------------
// Macros

#define GBL_PARSER_ARRAY_TO_U32(array, offset)         \
  ((uint32_t)((uint32_t)((array)[(offset) + 3]) << 24) \
   | ((uint32_t)((array)[(offset) + 2]) << 16)         \
   | ((uint32_t)((array)[(offset) + 1]) << 8)          \
   | ((uint32_t)((array)[(offset) + 0]) << 0))
#define GBL_PARSER_ARRAY_TO_U16(array, offset)                    \
  ((uint16_t)((uint16_t)((uint16_t)((array)[(offset) + 1]) << 8)) \
   | ((uint16_t)((uint16_t)((array)[(offset) + 0]) << 0)))

// -----------------------------------------------------------------------------
// Forward function declarations

// Save leftover bytes from a buffer to storage
static bool gbl_storeData(ParserContext_t  *context,
                          GblInputBuffer_t *input);

// Get bytes from storage (and input buffer if needed) into local buffer
static bool gbl_useData(ParserContext_t  *context,
                        uint8_t          outputBuffer[],
                        GblInputBuffer_t *input,
                        size_t           numberOfBytes);

// Get amount of bytes available for parsing in this round of parser_parse(..)
static size_t gbl_getBytesAvailable(ParserContext_t        *context,
                                    const GblInputBuffer_t *input);

// Tell the internal context that we retrieved this amount of bytes from the
// system.
static void gbl_advanceParser(ParserContext_t *context, size_t consumedBytes);

// Convenience function to retrieve an GBL tag header from storage/input buffer
static int32_t gbl_parseHeader(ParserContext_t  *context,
                               GblInputBuffer_t *input,
                               GblTagHeader_t   *gblTagHeader);

// -----------------------------------------------------------------------------
// Parser component function declarations

// Determine GBL file version
static int32_t parser_determineGblVersion(ParserContext_t  *parserContext,
                                          GblInputBuffer_t *input);
// Parse GBL version 3 header
static int32_t parser_parseGblHeader(ParserContext_t  *parserContext,
                                     GblInputBuffer_t *input);

#if defined(BTL_PARSER_SUPPORT_VERSION_DEPENDENCY_TAG)
// Parse GBL version dependency tag
static int32_t parser_parseVersionDependency(ParserContext_t  *parserContext,
                                             GblInputBuffer_t *input);
#endif

// Parse new tag header and update parserContext->internalState accordingly
static int32_t parser_parseNewTagHeader(ParserContext_t  *parserContext,
                                        GblInputBuffer_t *input);

// Initialize encryption context
#ifndef BTL_PARSER_NO_SUPPORT_ENCRYPTION
static int32_t parser_encryptionInit(ParserContext_t  *parserContext,
                                     GblInputBuffer_t *input);
#endif

// Parse Application info structure
static int32_t parser_parseApplicationInfo(ParserContext_t   *parserContext,
                                           GblInputBuffer_t  *input,
                                           ImageProperties_t *imageProperties);

// Parse SE upgrade tag header
#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
static int32_t parser_parseSe(ParserContext_t                   *parserContext,
                              GblInputBuffer_t                  *input,
                              ImageProperties_t                 *imageProperties,
                              const BootloaderParserCallbacks_t *callbacks);
#endif

// Parse Bootloader upgrade tag header
static int32_t parser_parseBootloader(ParserContext_t   *parserContext,
                                      GblInputBuffer_t  *input,
                                      ImageProperties_t *imageProperties);

// Parse Programming tag header
static int32_t parser_parseProg(ParserContext_t  *parserContext,
                                GblInputBuffer_t *input);

// Parse (Se|Prog|Bootloader|Metadata)Data
static int32_t parser_parseData(ParserContext_t                   *parserContext,
                                GblInputBuffer_t                  *input,
                                ImageProperties_t                 *imageProperties,
                                const BootloaderParserCallbacks_t *callbacks);

// Parse Custom tag
static int32_t parser_parseCustomTag(ParserContext_t                   *parserContext,
                                     GblInputBuffer_t                  *input,
                                     ImageProperties_t                 *imageProperties,
                                     const BootloaderParserCallbacks_t *callbacks);

// Parse and verify signatures located in Certificate and Signature tags
static int32_t parser_checkSignature(ParserContext_t   *parserContext,
                                     GblInputBuffer_t  *input,
                                     ImageProperties_t *imageProperties);

// Finalize parsing of GBL file after encountering an End tag
static int32_t parser_finalize(ParserContext_t                   *parserContext,
                               GblInputBuffer_t                  *input,
                               ImageProperties_t                 *imageProperties,
                               const BootloaderParserCallbacks_t *callbacks);

// -----------------------------------------------------------------------------
// Local functions

/***************************************************************************//**
 * Save remainder of input buffer to internal buffer
 *
 * @param context Context variable
 * @param buffer input buffer to copy from
 * @param offset current offset in input buffer (first byte to process)
 * @param length length of input buffer
 * @returns True if pushing succeeded, false if there would have been a problem.
 *
 * Saves the remainder of the input buffer to an internal buffer, to catch
 *  situations where the input buffer doesn't line up with GBL tag boundaries.
 ******************************************************************************/
static bool gbl_storeData(ParserContext_t  *context,
                          GblInputBuffer_t *input)
{
  size_t position;

  if (input->offset >= input->length) {
    return false; // Shouldn't happen, but be safe anyway
  }

  if ((input->length - input->offset) + context->bytesInInternalBuffer
      > sizeof(context->internalBuffer)) {
    return false; // Buffer overflow
  }

  while (input->offset < input->length) {
    position = ((size_t)context->internalBufferOffset + (size_t)context->bytesInInternalBuffer)
               % sizeof(context->internalBuffer);
    context->internalBuffer[position] = input->buffer[input->offset];
    input->offset++;
    context->bytesInInternalBuffer++;
  }

  return true;
}

/***************************************************************************//**
 * Copy bytes from internal + input buffer to output buffer
 *
 * @param context Context variable
 * @param outputBuffer buffer to copy to
 * @param inputBuffer input buffer to copy from
 * @param inputOffset offset indicating next byte to process in the inputbuffer
 * @param inputLength size of the input buffer in bytes
 * @param numberOfBytes number of bytes requested to be copied
 * @returns True if all bytes got copied, false if not.
 *
 * Gets the first n bytes in the input queue and copies them to a specified
 *   temporary buffer. The input queue is the concatenation of the internal
 *   buffer containing saved bytes from the previous call to the processing
 *   functions, and the input buffer from the current call to the processing
 *   function.
 * This function will also update the context accordingly.
 ******************************************************************************/
static bool gbl_useData(ParserContext_t  *context,
                        uint8_t          outputBuffer[],
                        GblInputBuffer_t *input,
                        size_t           numberOfBytes)
{
  size_t bytesProcessed = 0UL;
  // Get data from local buffer first
  while ((context->bytesInInternalBuffer > 0U)
         && (bytesProcessed < numberOfBytes)) {
    // Copy byte from localbuffer
    outputBuffer[bytesProcessed] =
      context->internalBuffer[context->internalBufferOffset];

    // Mark byte in storage as read
    context->internalBufferOffset = (context->internalBufferOffset + 1U)
                                    % sizeof(context->internalBuffer);

    context->bytesInInternalBuffer--;
    bytesProcessed++;
  }

  // Get data from new buffer when local buffer exhausted
  while ((input->offset < input->length) && (bytesProcessed < numberOfBytes)) {
    outputBuffer[bytesProcessed] = input->buffer[input->offset];
    input->offset++;
    bytesProcessed++;
  }

  if (bytesProcessed == numberOfBytes) {
    return true;
  } else {
    return false;
  }
}

/***************************************************************************//**
 * Get number of bytes available in internal buffer and external buffer combined
 *
 * @param context       Context variable
 * @param externalBytes Number of bytes available externally to the internal
 *                      buffer
 * @return Number of bytes available in internal buffer
 ******************************************************************************/
static size_t gbl_getBytesAvailable(ParserContext_t        *context,
                                    const GblInputBuffer_t *input)
{
  return context->bytesInInternalBuffer + input->length - input->offset;
}

/***************************************************************************//**
 * Update context with amount of parsed bytes
 *
 * @param context Context variable
 * @param consumedBytes Amount of bytes parsed by the parser
 *
 * This function will update the internal parser counters and kick the parser
 *   out of decryption mode when reaching the end of an encrypted block.
 ******************************************************************************/
static void gbl_advanceParser(ParserContext_t *context, size_t consumedBytes)
{
#ifndef BTL_PARSER_NO_SUPPORT_ENCRYPTION
  if (context->inEncryptedContainer) {
    context->offsetInEncryptedTag += consumedBytes;
    if (context->offsetInEncryptedTag >= context->lengthOfEncryptedTag) {
      context->inEncryptedContainer = false;
    }
  }
#endif
#if defined(BOOTLOADER_SE_UPGRADE_NO_STAGING) \
  && (BOOTLOADER_SE_UPGRADE_NO_STAGING == 1)
  context->offsetInGbl += consumedBytes;
#endif
  context->offsetInTag += consumedBytes;
}

/***************************************************************************//**
 * Look for tag header in available buffer
 ******************************************************************************/
static int32_t gbl_parseHeader(ParserContext_t  *context,
                               GblInputBuffer_t *input,
                               GblTagHeader_t   *gblTagHeader)
{
  uint8_t tagBuffer[8];
  size_t tagSize = sizeof(GblTagHeader_t);
  int32_t retval;

  // Get data
  // Do hashing ourselves to avoid hashing the signature/end tag
  // Do decryption ourselves to avoid hashing after decryption
  retval = gbl_getData(context,
                       input,
                       tagBuffer,
                       tagSize,
                       false,
                       false);

  if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
    return retval;
  }

#ifndef BTL_PARSER_NO_SUPPORT_ENCRYPTION
  if (context->inEncryptedContainer) {
    // Update SHA hash before decryption
    btl_updateSha256(context->shaContext, tagBuffer, tagSize);
    btl_processAesCtrData(context->aesContext,
                          tagBuffer,
                          tagBuffer,
                          tagSize);
  }
#endif

  // Get tag header from decrypted buffer
  // GBL uses little-endian 32-bit tag/length values
  gblTagHeader->tagId = GBL_PARSER_ARRAY_TO_U32(tagBuffer, 0);
  gblTagHeader->length = GBL_PARSER_ARRAY_TO_U32(tagBuffer, 4);

  // *INDENT-OFF*
#ifndef BTL_PARSER_NO_SUPPORT_ENCRYPTION
  if (!(context->inEncryptedContainer)) {
#endif
    // Update SHA hash if tag is not signature or end tag
    if ((gblTagHeader->tagId != GBL_TAG_ID_SIGNATURE_ECDSA_P256)
        && (gblTagHeader->tagId != GBL_TAG_ID_END)) {
      btl_updateSha256(context->shaContext, tagBuffer, tagSize);
    }
#ifndef BTL_PARSER_NO_SUPPORT_ENCRYPTION
  }
#endif
  // *INDENT-ON*

  // Save length of this tag
  context->lengthOfTag = gblTagHeader->length;
  context->offsetInTag = 0UL;

  BTL_DEBUG_PRINT("tag 0x");
  BTL_DEBUG_PRINT_WORD_HEX(gblTagHeader->tagId);
  BTL_DEBUG_PRINT(" len 0x");
  BTL_DEBUG_PRINT_WORD_HEX(gblTagHeader->length);
  BTL_DEBUG_PRINT_LF();

  return BOOTLOADER_ERROR_PARSER_PARSED;
}

// -----------------------------------------------------------------------------
// Global helper functions

/***************************************************************************//**
 * Get data from combined input buffer + internal buffer, and advance the
 * parser state
 ******************************************************************************/
int32_t gbl_getData(ParserContext_t  *context,
                    GblInputBuffer_t *input,
                    uint8_t          outputBuffer[],
                    size_t           outputLength,
                    bool             applySHA,
                    bool             decrypt)
{
  if (gbl_getBytesAvailable(context, input) < outputLength) {
    if (gbl_storeData(context, input)) {
      return BOOTLOADER_OK;
    } else {
      context->internalState = GblParserStateError;
      return BOOTLOADER_ERROR_PARSER_BUFFER;
    }
  }

  if (!gbl_useData(context,
                   outputBuffer,
                   input,
                   outputLength)) {
    context->internalState = GblParserStateError;
    return BOOTLOADER_ERROR_PARSER_BUFFER;
  }

  // Update checksum
  context->fileCrc = btl_crc32Stream(outputBuffer,
                                     outputLength,
                                     context->fileCrc);

  // Update SHA256 when requested
  if (applySHA) {
    btl_updateSha256(context->shaContext, outputBuffer, outputLength);
  }

#ifndef BTL_PARSER_NO_SUPPORT_ENCRYPTION
  // Decrypt data when requested
  if (decrypt && (context->inEncryptedContainer)) {
    btl_processAesCtrData(context->aesContext,
                          outputBuffer,
                          outputBuffer,
                          outputLength);
  }
#else
  (void) decrypt;
#endif

  // Update context value to indicate we retrieved this amount of data
  // into the parsing logic
  // Note: This has to happen after decryption, since the function updates
  //       the encryption container state
  gbl_advanceParser(context, outputLength);

  return BOOTLOADER_ERROR_PARSER_PARSED;
}

int32_t gbl_writeProgData(ParserContext_t *context,
                          uint8_t buffer[],
                          size_t length,
                          const BootloaderParserCallbacks_t *callbacks)
{
  uint32_t startWithhold, endWithhold, withholdSrcOffset, withholdDstOffset;

  if (callbacks->applicationCallback == NULL) {
    // Nothing to do
    return BOOTLOADER_OK;
  }

  if ((uint32_t)buffer & 0x3U) {
    BTL_DEBUG_PRINTLN("  Unaligned input buffer");
    // Buffer is unaligned
    return BOOTLOADER_ERROR_PARSER_BUFFER;
  }

  // If application initial PC or reset vectors are in this call, store them and
  // override to FF. Data will be passed to the callback at the end when entire
  // GBL is validated.
  uint32_t appPcAddress = (uint32_t) mainBootloaderTable->startOfAppSpace + 4UL;
  if ((context->programmingAddress + length >= appPcAddress)
      && (context->programmingAddress < appPcAddress + 24UL)) {
    // Absolute start address of region to withhold
    startWithhold = SL_MAX(context->programmingAddress, appPcAddress);
    // Absolute end address of region to withhold
    endWithhold = SL_MIN(context->programmingAddress + length,
                         appPcAddress + 24UL);
    // Offset into buffer of region to withhold
    withholdSrcOffset = startWithhold - context->programmingAddress;
    // Offset into withhold buffer for withheld region
    withholdDstOffset = startWithhold - appPcAddress;

    // Tuck away bytes to withhold
    (void) memcpy(&context->withheldApplicationVectors[withholdDstOffset],
                  &buffer[withholdSrcOffset],
                  endWithhold - startWithhold);
    // Replace with 0xFF now, only release after verification
    (void) memset(&buffer[withholdSrcOffset], 0xFF, endWithhold - startWithhold);
  }

  // If application touches bootloader upgrade location, store reset vector of
  // upgrade space and only flash it upon GBL verification. This is to avoid an
  // attack where a malicious BTL upgrade is encapsulated in a crafted GBL file
  // (which gets flashed before final validation)
  uint32_t btlPcAddress = BTL_UPGRADE_LOCATION + 4UL;
  if ((context->programmingAddress <= btlPcAddress)
      && ((context->programmingAddress + length) >= (btlPcAddress + 4UL))) {
    withholdSrcOffset = btlPcAddress - context->programmingAddress;
    // Tuck away bytes to withhold
    (void) memcpy(context->withheldUpgradeVectors,
                  &buffer[withholdSrcOffset],
                  4U);
    // Replace with 0xFF
    (void) memset(&buffer[withholdSrcOffset], 0xFF, 4U);
  }

  callbacks->applicationCallback(context->programmingAddress,
                                 buffer,
                                 length,
                                 callbacks->context);
  context->programmingAddress += length;
  return BOOTLOADER_OK;
}

// -----------------------------------------------------------------------------
// Parser implementation

/***************************************************************************//**
 * Initialize the parser context.
 ******************************************************************************/
int32_t parser_init(void *context, void *decryptContext, void *authContext, uint8_t flags)
{
  // Clean up internal state
  ParserContext_t* parserContext = (ParserContext_t*)context;
  parserContext->internalState = GblParserStateInit;
  parserContext->bytesInInternalBuffer = 0U;
  parserContext->internalBufferOffset = 0U;
  parserContext->inEncryptedContainer = false;
  parserContext->receivedFlags = 0U;
  parserContext->gotSignature = false;
  parserContext->flags = flags & PARSER_FLAGS_PUBLIC_MASK;
  parserContext->fileCrc = BTL_CRC32_START;
  parserContext->programmingAddress = 0UL;
  parserContext->tagAddress = 0UL;
  (void) memset(parserContext->withheldApplicationVectors, 0xFF, 24U);
  (void) memset(parserContext->withheldUpgradeVectors, 0xFF, 4U);
  (void) memset(parserContext->withheldBootloaderVectors, 0xFF, 4U);
  parserContext->aesContext = decryptContext;
  parserContext->shaContext = authContext;
#if defined(_SILICON_LABS_32B_SERIES_2)
  (void) memset(&(parserContext->certificate), 0, 136U);
  parserContext->gotCertificate = false;
  parserContext->offsetInGbl = 0UL;
  parserContext->offsetOfSeUpgradeTag = 0xFFFFFFFFUL; // Representing invalid offset

  parserContext->versionDependencyResultApp        = 1U;
  parserContext->versionDependencyResultBootloader = 1U;
  parserContext->versionDependencyResultSe         = 1U;
#endif

  if ((PARSER_REQUIRE_CONFIDENTIALITY) && (decryptContext == NULL)) {
    return BOOTLOADER_ERROR_PARSER_INIT;
  }

  if ((PARSER_REQUIRE_AUTHENTICITY) && (authContext == NULL)) {
    return BOOTLOADER_ERROR_PARSER_INIT;
  }

  if (authContext != NULL) {
    btl_initSha256(parserContext->shaContext);
  }

  return BOOTLOADER_OK;
}

/***************************************************************************//**
 * Parse GBL image to extract the binary and some metadata.
 ******************************************************************************/
int32_t parser_parse(void                              *context,
                     ImageProperties_t                 *imageProperties,
                     uint8_t                           buffer[],
                     size_t                            length,
                     const BootloaderParserCallbacks_t *callbacks)
{
  ParserContext_t *parserContext = (ParserContext_t *)context;
  GblInputBuffer_t input = {
    .buffer = buffer,
    .length = length,
    .offset = 0UL
  };
  volatile int32_t retval;

  // This is pretty much purely a state machine...
  while (input.offset < length) {
#if defined(__ICCARM__)
// Suppress MISRA warning that default case is missing
#pragma diag_suppress=Pm058
#endif
    switch (parserContext->internalState) {
      // Coming from an idle state means starting anew
      // Which means we're expecting a header tag
      case GblParserStateInit:
        retval = parser_determineGblVersion(parserContext, &input);
        if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
          return retval;
        }
        break;

      // Received a header tag, parse information from it.
      case GblParserStateHeader:
        retval = parser_parseGblHeader(parserContext, &input);
        if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
          return retval;
        }
        break;

      // We've already got the GBL Header tag, and are done with whatever
      // tag we were processing. Now waiting for a new tag.
      case GblParserStateIdle:
        retval = parser_parseNewTagHeader(parserContext, &input);
        if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
          return retval;
        }
        break;

      // Received an encryption initialization header,
      // so initialize the encryption state
      case GblParserStateEncryptionInit:
#ifndef BTL_PARSER_NO_SUPPORT_ENCRYPTION
        retval = parser_encryptionInit(parserContext, &input);
        if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
          return retval;
        }
#else
        parserContext->internalState = GblParserStateError;
        return BOOTLOADER_ERROR_PARSER_UNEXPECTED;
#endif
        break;

      // This tag contains encrypted tags, so set up decryption
      // and go one level down in the state machine
      case GblParserStateEncryptionContainer:
#ifndef BTL_PARSER_NO_SUPPORT_ENCRYPTION
        parserContext->inEncryptedContainer = true;
        parserContext->internalState = GblParserStateIdle;
#else
        parserContext->internalState = GblParserStateError;
        return BOOTLOADER_ERROR_PARSER_UNEXPECTED;
#endif
        break;

#if defined(BTL_PARSER_SUPPORT_VERSION_DEPENDENCY_TAG)
      case GblParserStateVersionDependency:
        retval = parser_parseVersionDependency(parserContext, &input);
        if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
          return retval;
        }
        break;
#endif
      case GblParserStateApplication:
        retval = parser_parseApplicationInfo(parserContext,
                                             &input,
                                             imageProperties);
        if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
          return retval;
        }
        break;

        // Received a tag with binary data to pass on.
        // If you have custom metadata in your GBL, we'll pass
        // it on to the application (through the bootloader).
        // Prog and Eraseprog tags are acted on by the bootloader.

#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
      case GblParserStateSe:
#if defined(BOOTLOADER_SE_UPGRADE_NO_STAGING) \
        && (BOOTLOADER_SE_UPGRADE_NO_STAGING == 1)
        parserContext->offsetOfSeUpgradeTag = parserContext->offsetInGbl
                                              - sizeof(GblTagHeader_t);
#endif
        retval = parser_parseSe(parserContext,
                                &input,
                                imageProperties,
                                callbacks);
        if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
          return retval;
        }
        break;
#endif
      case GblParserStateBootloader:
        retval = parser_parseBootloader(parserContext,
                                        &input,
                                        imageProperties);
        if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
          return retval;
        }
        break;

      case GblParserStateProg:
      case GblParserStateEraseProg:
        retval = parser_parseProg(parserContext, &input);
        if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
          return retval;
        }
        break;

      case GblParserStateMetadata:
        parserContext->tagAddress = 0UL;
        parserContext->internalState = GblParserStateMetadataData;
        break;
#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
      case GblParserStateSeData:
#endif
      case GblParserStateProgData:
      case GblParserStateBootloaderData:
      case GblParserStateMetadataData:
        retval = parser_parseData(parserContext,
                                  &input,
                                  imageProperties,
                                  callbacks);
        if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
          return retval;
        }
        break;

      case GblParserStateCustomTag:
        retval = parser_parseCustomTag(parserContext,
                                       &input,
                                       imageProperties,
                                       callbacks);
        if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
          return retval;
        }
        break;

        // Certificate and Signature tags used for image authentication
#if defined(_SILICON_LABS_32B_SERIES_2)
      case GblParserStateCertificate:
#endif
      // This tag contains the signature over the entire GBL,
      // accept no more data hereafter.
      case GblParserStateSignature:
        retval = parser_checkSignature(parserContext,
                                       &input,
                                       imageProperties);
        if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
          return retval;
        }
        break;

      // Received an end tag, start the cleanup process
      case GblParserStateFinalize:
        retval = parser_finalize(parserContext,
                                 &input,
                                 imageProperties,
                                 callbacks);
        return retval;
        break;

      // Completely done with the file, in this state we'll stop processing.
      case GblParserStateDone:
        return BOOTLOADER_ERROR_PARSER_EOF;
        break;

      case GblParserStateError:
        return BOOTLOADER_ERROR_PARSER_EOF;
        break;
        // No default statement here guarantees a compile-time check
        // that we caught all states
    }
  }

  // If we get here, we landed right on a tag boundary...
  return BOOTLOADER_OK;
}

/***************************************************************************//**
 * Verify GBL certificate.
 ******************************************************************************/
int32_t parser_verifyCertificate(void *context,
                                 void *input,
                                 void *blProperties,
                                 void *shaState)
{
#if (PARSER_REQUIRE_CERTIFICATE_AUTHENTICITY == true)
  ParserContext_t *parserContext = (ParserContext_t *)context;
  GblInputBuffer_t *inputBuffer = (GblInputBuffer_t*)input;
  ApplicationProperties_t *blAppProperties = (ApplicationProperties_t*)blProperties;
  Sha256Context_t *certShaState = (Sha256Context_t*)shaState;
  uint8_t tagBuffer[GBL_PARSER_BUFFER_SIZE];
  int32_t retval = BOOTLOADER_ERROR_PARSER_UNEXPECTED;

  volatile int32_t signatureVerifyResp = BOOTLOADER_ERROR_SECURITY_REJECTED;

  if (parserContext->offsetInTag < 4UL) {
    retval = gbl_getData(context,
                         inputBuffer,
                         tagBuffer,
                         4UL,
                         true,
                         false);
    if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
      return retval;
    }
    memcpy(&(parserContext->certificate.structVersion), tagBuffer, 4U);
  } else if (parserContext->offsetInTag < 68UL) {
    // Get ECDSA public key.
    retval = gbl_getData(context,
                         inputBuffer,
                         tagBuffer,
                         64UL,
                         true,
                         false);
    if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
      return retval;
    }
    // Save ECDSA public key in the context
    // for verification of the GBL.
    memcpy(parserContext->certificate.key, tagBuffer, 64U);
  } else if (parserContext->offsetInTag < 72UL) {
    // Get version of this certificate.
    retval = gbl_getData(context,
                         inputBuffer,
                         tagBuffer,
                         4UL,
                         true,
                         false);
    if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
      return retval;
    }
    memcpy(&(parserContext->certificate.version), tagBuffer, 4U);
    if ((blAppProperties->cert == NULL)) {
      return BOOTLOADER_ERROR_PARSER_SIGNATURE;
    }
    if ((blAppProperties->cert->version > parserContext->certificate.version)) {
      return BOOTLOADER_ERROR_PARSER_SIGNATURE;
    }
  } else {
    // Get signature of the certificate.
    retval = gbl_getData(context,
                         inputBuffer,
                         tagBuffer,
                         64UL,
                         true,
                         false);
    if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
      return retval;
    }
    memcpy(parserContext->certificate.signature, tagBuffer, 64U);
    // SHA-256 of the certificate.
    btl_initSha256(certShaState);
    btl_updateSha256(certShaState,
                     (const uint8_t*)&(parserContext->certificate),
                     72U);
    btl_finalizeSha256(certShaState);
    // Use the public key stored in the bootloader certificate to verify the GBL certificate.
    signatureVerifyResp = btl_verifyEcdsaP256r1(certShaState->sha,
                                                &(parserContext->certificate.signature[0]),
                                                &(parserContext->certificate.signature[32]),
                                                &(blAppProperties->cert->key[0]),
                                                &(blAppProperties->cert->key[32]));
#if defined(SEMAILBOX_PRESENT)
    if (signatureVerifyResp == BOOTLOADER_ERROR_SECURITY_REJECTED) {
      // Check if the OTP platform key can verify the certificate.
      signatureVerifyResp = btl_verifyEcdsaP256r1(certShaState->sha,
                                                  &(parserContext->certificate.signature[0]),
                                                  &(parserContext->certificate.signature[32]),
                                                  NULL,
                                                  NULL);
    }
#endif
    if (signatureVerifyResp != BOOTLOADER_OK) {
      return BOOTLOADER_ERROR_PARSER_SIGNATURE;
    }
    parserContext->gotCertificate = true;
    parserContext->internalState = GblParserStateIdle;
  }
  return retval;
#else
  (void)context;
  (void)input;
  (void)blProperties;
  (void)shaState;
  return BOOTLOADER_ERROR_PARSER_SIGNATURE;
#endif
}

// -----------------------------------------------------------------------------
// Parser component function definitions

/***************************************************************************//**
 * Determine and validate GBL version. Parses tag header and length.
 * As of now, only one version of the GBL format is supported.
 *
 * @param[in, out] parserContext Context variable
 * @param[in] input Input buffer
 * @returns BOOTLOADER_ERROR_PARSER_PARSED on success.
 *
 * Updates parserContext->internalState.
 * If supported GBL version:  GblParserStateHeader
 * Otherwise:                 GblParserStateError
 ******************************************************************************/
static int32_t parser_determineGblVersion(ParserContext_t  *parserContext,
                                          GblInputBuffer_t *input)
{
  volatile int32_t retval;
  GblTagHeader_t gblTagHeader;

  // First, get tag/length combo
  retval = gbl_parseHeader(parserContext,
                           input,
                           &gblTagHeader);

  if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
    return retval;
  }

  // Save length of this tag
  parserContext->lengthOfTag = gblTagHeader.length;
  parserContext->offsetInTag = 0UL;

  if (gblTagHeader.tagId == GBL_TAG_ID_HEADER_V3) {
    parserContext->internalState = GblParserStateHeader;
  } else { // GBL version not supported
    parserContext->internalState = GblParserStateError;
    return BOOTLOADER_ERROR_PARSER_UNEXPECTED;
  }

  return BOOTLOADER_ERROR_PARSER_PARSED;
}

/***************************************************************************//**
 * Parse GBL header.
 *
 * @param[in, out] parserContext Context variable
 * @param[in] input Input buffer
 * @returns BOOTLOADER_ERROR_PARSER_PARSED when successfully parsed.
 *
 * Updates parserContext->internalState.
 * Success:   GblParserStateIdle
 * Otherwise: GblParserStateError
 ******************************************************************************/
static int32_t parser_parseGblHeader(ParserContext_t  *parserContext,
                                     GblInputBuffer_t *input)
{
  volatile int32_t retval;
  uint32_t temporaryWord;
  uint8_t tagBuffer[GBL_PARSER_BUFFER_SIZE];

  // Get version, magic, app info
  while (parserContext->offsetInTag < 8UL) {
    // Get data
    // Header should be hashed, but never decrypted
    retval = gbl_getData(parserContext,
                         input,
                         tagBuffer,
                         8UL,
                         true,
                         false);
    if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
      return retval;
    }

    // 8 bytes:
    // - u32 version
    // - u32 type

    temporaryWord = GBL_PARSER_ARRAY_TO_U32(tagBuffer, 0);
    if ((temporaryWord & 0xFF000000UL)
        != GBL_COMPATIBILITY_MAJOR_VERSION) {
      parserContext->internalState = GblParserStateError;
      return BOOTLOADER_ERROR_PARSER_VERSION;
    }

    temporaryWord = GBL_PARSER_ARRAY_TO_U32(tagBuffer, 4);

#ifndef BTL_PARSER_NO_SUPPORT_ENCRYPTION
    if ((temporaryWord & GBL_TYPE_ENCRYPTION_AESCCM) != 0U) {
      parserContext->flags |= PARSER_FLAG_ENCRYPTED;
      BTL_DEBUG_PRINTLN("Enc");
    } else if (PARSER_REQUIRE_CONFIDENTIALITY) {
      parserContext->internalState = GblParserStateError;
      return BOOTLOADER_ERROR_PARSER_FILETYPE;
    } else {
      // Unencrypted GBL is allowed
    }
#else
    // No encryption supported, but encrypted GBL given
    if ((temporaryWord & GBL_TYPE_ENCRYPTION_AESCCM) != 0U) {
      return BOOTLOADER_ERROR_PARSER_FILETYPE;
    }
#endif

    if ((PARSER_REQUIRE_AUTHENTICITY)
        && ((temporaryWord & GBL_TYPE_SIGNATURE_ECDSA) == 0U)) {
      parserContext->internalState = GblParserStateError;
      return BOOTLOADER_ERROR_PARSER_FILETYPE;
    }
  }

  parserContext->internalState = GblParserStateIdle;
  return BOOTLOADER_ERROR_PARSER_PARSED;
}

#if defined(BTL_PARSER_SUPPORT_VERSION_DEPENDENCY_TAG)
/***************************************************************************//**
 * Parse and evaluate version dependency statements.
 * Terminate GBL parsing if any of the encountered version dependency statements
 * evaluates to false.
 *
 * @param[in, out] parserContext Context variable
 * @param[in] input Input buffer
 * @returns BOOTLOADER_ERROR_PARSER_PARSED when successfully parsed.
 *
 * Updates parserContext->internalState.
 * Success:   GblParserStateIdle
 * Otherwise: GblParserStateError
 ******************************************************************************/
static int32_t parser_parseVersionDependency(ParserContext_t  *parserContext,
                                             GblInputBuffer_t *input)
{
  volatile int32_t retval;
  uint8_t tagBuffer[GBL_PARSER_BUFFER_SIZE];
  const uint32_t numberOfStatements = parserContext->lengthOfTag / sizeof(VersionDependency_t);
  uint32_t currentStatementNumber = 0UL;
  uint32_t currentVersion = 0xFFFFFFFFUL;

  uint8_t resultApp        = 1U;
  uint8_t resultBootloader = 1U;
  uint8_t resultSe         = 1U;
  uint8_t *resultPtr       = NULL;
  uint8_t tmpResult;
  uint8_t operator, operatorType, operatorNegatorBit;
  uint8_t connective, connectiveType, connectiveNegatorBit;
  VersionDependency_t *versionDependency = NULL;

  while (parserContext->offsetInTag < parserContext->lengthOfTag) {
    // Parse the version dependency structs one-by-one
    retval = gbl_getData(parserContext,
                         input,
                         tagBuffer,
                         sizeof(VersionDependency_t),
                         true,
                         false);
    if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
      return retval;
    }

    // One VersionDependency_t struct contained in tagBuffer at this point
    versionDependency = (VersionDependency_t *) tagBuffer;

    switch (versionDependency->imageType) {
      case GBL_VERSION_DEPENDENCY_TYPE_APPLICATION:
        if (!bootload_getApplicationVersion(&currentVersion)) {
          // Set result to false when the application version cannot be read.
          // This can happen if there is no app present or version information
          // got corrupted after applying a (bootloader|SE) upgrade also
          // present in the GBL. These are expected legitimate scenarios and
          // should just falsify resultApp instead of setting an error state.
          resultApp = 0U;
        }
        resultPtr = &resultApp;
        break;
      case GBL_VERSION_DEPENDENCY_TYPE_BOOTLOADER:
        currentVersion = bootload_getBootloaderVersion();
        resultPtr = &resultBootloader;
        break;
#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
      case GBL_VERSION_DEPENDENCY_TYPE_SE:
        if (!bootload_getSeVersion(&currentVersion)) {
          parserContext->internalState = GblParserStateError;
          return BOOTLOADER_ERROR_PARSER_UNEXPECTED;
        }
        // Discard compatibility byte before version comparison
        currentVersion &= GBL_VERSION_DEPENDENCY_SE_VERSION_MASK;
        resultPtr = &resultSe;
        break;
#endif
      default:
        parserContext->internalState = GblParserStateError;
        return BOOTLOADER_ERROR_PARSER_UNEXPECTED; // invalid imageType
    }

    tmpResult = 0U;

    operator = (versionDependency->statement & GBL_VERSION_DEPENDENCY_OPERATOR_MASK)
               >> GBL_VERSION_DEPENDENCY_OPERATOR_SHIFT;
    operatorType       = (operator & GBL_VERSION_DEPENDENCY_OPERATOR_TYPE_MASK);
    operatorNegatorBit = (operator & GBL_VERSION_DEPENDENCY_OPERATOR_NEGATOR_BIT_MASK);

    connective = (versionDependency->statement & GBL_VERSION_DEPENDENCY_CONNECTIVE_MASK)
                 >> GBL_VERSION_DEPENDENCY_CONNECTIVE_SHIFT;
    connectiveType       = (connective & GBL_VERSION_DEPENDENCY_CONNECTIVE_TYPE_MASK);
    connectiveNegatorBit = (connective & GBL_VERSION_DEPENDENCY_CONNECTIVE_NEGATOR_BIT_MASK);

    switch (operatorType) {
      case GBL_VERSION_DEPENDENCY_OPERATOR_LT:
        if (currentVersion < versionDependency->version) {
          tmpResult = 1U;
        }
        break;
      case GBL_VERSION_DEPENDENCY_OPERATOR_LEQ:
        if (currentVersion <= versionDependency->version) {
          tmpResult = 1U;
        }
        break;
      case GBL_VERSION_DEPENDENCY_OPERATOR_EQ:
        if (currentVersion == versionDependency->version) {
          tmpResult = 1U;
        }
        break;
      case GBL_VERSION_DEPENDENCY_OPERATOR_GEQ:
        if (currentVersion >= versionDependency->version) {
          tmpResult = 1U;
        }
        break;
      case GBL_VERSION_DEPENDENCY_OPERATOR_GT:
        if (currentVersion > versionDependency->version) {
          tmpResult = 1U;
        }
        break;
      default:
        parserContext->internalState = GblParserStateError;
        return BOOTLOADER_ERROR_PARSER_UNEXPECTED; // invalid operator
    }

    tmpResult ^= operatorNegatorBit;

    switch (connectiveType) {
      case GBL_VERSION_DEPENDENCY_CONNECTIVE_AND:
        *resultPtr &= tmpResult;
        break;
      case GBL_VERSION_DEPENDENCY_CONNECTIVE_OR:
        *resultPtr |= tmpResult;
        break;
      default:
        parserContext->internalState = GblParserStateError;
        return BOOTLOADER_ERROR_PARSER_UNEXPECTED; // invalid connective
    }

    *resultPtr ^= connectiveNegatorBit;

    currentStatementNumber++;
  }

  // Check that all the version dependency statements were indeed parsed.
  if (currentStatementNumber != numberOfStatements) {
    parserContext->internalState = GblParserStateError;
    return BOOTLOADER_ERROR_PARSER_UNEXPECTED;
  }

  // Fail as early as possible. For storage bootloaders,
  // things get more complicated as we have to account for GBL files possibly
  // containing multiple upgrade images, each changing its corresponding
  // version number after the (app|bootloader|se) upgrade.
  // In that case, the final evaluation will take place during parser_finalize.
#if defined(BOOTLOADER_SUPPORT_COMMUNICATION)
  // Check that all the version dependency statements evaluated to true,
  // and reject the GBL image otherwise.
  if ((!resultApp) || (!resultBootloader) || (!resultSe)) {
    parserContext->internalState = GblParserStateError;
    return BOOTLOADER_ERROR_PARSER_VERSION;
  }
#endif

  parserContext->versionDependencyResultApp        = resultApp;
  parserContext->versionDependencyResultBootloader = resultBootloader;
  parserContext->versionDependencyResultSe         = resultSe;

  // Continue with GBL parsing.
  parserContext->internalState = GblParserStateIdle;
  return BOOTLOADER_ERROR_PARSER_PARSED;
}
#endif // defined(BTL_PARSER_SUPPORT_VERSION_DEPENDENCY_TAG)

/***************************************************************************//**
 * Parse tag header of the next tag to be parsed, coming from an idle state.
 * Update parserContext-internalState with the state corresponding to the tagId.
 *
 * @param[in, out] parserContext Context variable
 * @param[in] input Input buffer
 * @returns BOOTLOADER_ERROR_PARSER_PARSED when successfully parsed.
 *
 * Updates parserContext->internalState.
 * Success:   GblParserState* corresponding to the tag header that was read.
 * Otherwise: GblParserStateError
 ******************************************************************************/
static int32_t parser_parseNewTagHeader(ParserContext_t  *parserContext,
                                        GblInputBuffer_t *input)
{
  volatile int32_t retval;
  GblTagHeader_t gblTagHeader;
  const GblCustomTag_t *customTag = NULL;

  // First, get tag/length combo, for which we need 8 bytes
  retval = gbl_parseHeader(parserContext,
                           input,
                           &gblTagHeader);

  if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
    return retval;
  }

  // Check for unexpected tag after signature
  if (parserContext->gotSignature) {
    if (gblTagHeader.tagId == GBL_TAG_ID_END) {
      parserContext->internalState = GblParserStateFinalize;
      return BOOTLOADER_ERROR_PARSER_PARSED;
    } else {
      parserContext->internalState = GblParserStateError;
      return BOOTLOADER_ERROR_PARSER_UNEXPECTED;
    }
  }

#ifndef BTL_PARSER_NO_SUPPORT_ENCRYPTION
  // Check tagBuffer for valid tag/length
  if ((parserContext->flags & PARSER_FLAG_ENCRYPTED)
      && (!(parserContext->inEncryptedContainer))) {
    switch (gblTagHeader.tagId) {
      case GBL_TAG_ID_ENC_INIT:
        parserContext->internalState = GblParserStateEncryptionInit;
        break;
      case GBL_TAG_ID_ENC_GBL_DATA:
        parserContext->internalState = GblParserStateEncryptionContainer;
        parserContext->lengthOfEncryptedTag = parserContext->lengthOfTag;
        parserContext->offsetInEncryptedTag = 0UL;
        break;
#if defined(_SILICON_LABS_32B_SERIES_2)
      case GBL_TAG_ID_CERTIFICATE_ECDSA_P256:
        parserContext->internalState = GblParserStateCertificate;
        break;
#endif
      case GBL_TAG_ID_SIGNATURE_ECDSA_P256:
        parserContext->internalState = GblParserStateSignature;
        break;
      case GBL_TAG_ID_END:
        // Don't allow ending the GBL without a signature if not
        // explicitly configured to do so
        if (PARSER_REQUIRE_AUTHENTICITY) {
          parserContext->internalState = GblParserStateError;
          return BOOTLOADER_ERROR_PARSER_UNEXPECTED;
        } else {
          parserContext->internalState = GblParserStateFinalize;
        }
        break;

#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
      case GBL_TAG_ID_SE_UPGRADE:
#if defined(BOOTLOADER_SE_UPGRADE_NO_STAGING) \
        && (BOOTLOADER_SE_UPGRADE_NO_STAGING == 1)
        parserContext->internalState = GblParserStateSe;
        break;
#endif // BOOTLOADER_SE_UPGRADE_NO_STAGING
#endif // SEMAILBOX_PRESENT
      case GBL_TAG_ID_BOOTLOADER:
      case GBL_TAG_ID_APPLICATION:
      case GBL_TAG_ID_METADATA:
      case GBL_TAG_ID_PROG:
      case GBL_TAG_ID_ERASEPROG:
      case GBL_TAG_ID_HEADER_V3:
        parserContext->internalState = GblParserStateError;
        return BOOTLOADER_ERROR_PARSER_UNEXPECTED;
        break;
      default:
        parserContext->internalState = GblParserStateError;
        if (gbl_isCustomTag(&gblTagHeader)) {
          // Custom tag exists, so it's unexpected rather than unknown
          return BOOTLOADER_ERROR_PARSER_UNEXPECTED;
        } else {
          return BOOTLOADER_ERROR_PARSER_UNKNOWN_TAG;
        }
        break;
    }
  } else {
#endif // BTL_PARSER_NO_SUPPORT_ENCRYPTION

  switch (gblTagHeader.tagId) {
#if defined(BTL_PARSER_SUPPORT_VERSION_DEPENDENCY_TAG)
    case GBL_TAG_ID_VERSION_DEPENDENCY:
      parserContext->internalState = GblParserStateVersionDependency;
      break;
#endif // defined(BTL_PARSER_SUPPORT_VERSION_DEPENDENCY_TAG)
#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
    case GBL_TAG_ID_SE_UPGRADE:
#if !defined(BTL_PARSER_NO_SUPPORT_ENCRYPTION)     \
      && defined(BOOTLOADER_SE_UPGRADE_NO_STAGING) \
      && (BOOTLOADER_SE_UPGRADE_NO_STAGING == 1)
      if ((parserContext->flags & PARSER_FLAG_ENCRYPTED)
          && parserContext->inEncryptedContainer) {
        parserContext->internalState = GblParserStateError;
        return BOOTLOADER_ERROR_PARSER_UNEXPECTED;
      }
#endif // BTL_PARSER_NO_SUPPORT_ENCRYPTION, BOOTLOADER_SE_UPGRADE_NO_STAGING
      parserContext->internalState = GblParserStateSe;
      break;
#endif // SEMAILBOX_PRESENT
    case GBL_TAG_ID_BOOTLOADER:
      parserContext->internalState = GblParserStateBootloader;
      break;
    case GBL_TAG_ID_APPLICATION:
      parserContext->internalState = GblParserStateApplication;
      break;
    case GBL_TAG_ID_METADATA:
      parserContext->internalState = GblParserStateMetadata;
      break;
    case GBL_TAG_ID_PROG:
      parserContext->internalState = GblParserStateProg;
      break;
    case GBL_TAG_ID_ERASEPROG:
      parserContext->internalState = GblParserStateEraseProg;
      break;
    case GBL_TAG_ID_END:
      // Don't allow ending the GBL inside of an encrypted container
      // Don't allow ending the GBL without a signature if not
      // explicitly configured to do so
      if ((PARSER_REQUIRE_AUTHENTICITY)
          || (parserContext->inEncryptedContainer)) {
        parserContext->internalState = GblParserStateError;
        return BOOTLOADER_ERROR_PARSER_UNEXPECTED;
      }

      parserContext->internalState = GblParserStateFinalize;
      break;
#if defined(_SILICON_LABS_32B_SERIES_2)
    case GBL_TAG_ID_CERTIFICATE_ECDSA_P256:
      // Don't allow ending the GBL inside of an encrypted container
#ifndef BTL_PARSER_NO_SUPPORT_ENCRYPTION
      if (parserContext->inEncryptedContainer) {
        parserContext->internalState = GblParserStateError;
        return BOOTLOADER_ERROR_PARSER_UNEXPECTED;
      }
#endif
      parserContext->internalState = GblParserStateCertificate;
      break;
#endif
    case GBL_TAG_ID_SIGNATURE_ECDSA_P256:
      // Don't allow ending the GBL inside of an encrypted container
#ifndef BTL_PARSER_NO_SUPPORT_ENCRYPTION
      if (parserContext->inEncryptedContainer) {
        parserContext->internalState = GblParserStateError;
        return BOOTLOADER_ERROR_PARSER_UNEXPECTED;
      }
#endif
      parserContext->internalState = GblParserStateSignature;
      break;
    case GBL_TAG_ID_HEADER_V3:
    // Header tag is always the first, never encrypted, and
    // one GBL file never contains two headers.
    default:
      if (gbl_isCustomTag(&gblTagHeader)) {
        // Custom tag exists
        parserContext->customTagId = gblTagHeader.tagId;
        parserContext->internalState = GblParserStateCustomTag;
        customTag = gbl_getCustomTagProperties(gblTagHeader.tagId);
        if ((parserContext->flags & PARSER_FLAG_PARSE_CUSTOM_TAGS)
            && customTag && (customTag->enterTag)) {
          retval = customTag->enterTag(parserContext);
          if (retval != BOOTLOADER_OK) {
            return BOOTLOADER_ERROR_PARSER_UNEXPECTED;
          }
        }
      } else {
        parserContext->internalState = GblParserStateError;
        if (parserContext->inEncryptedContainer) {
          // Getting an unknown tag inside of encrypted container is most
          // probably due to lacking the correct decryption key
          return BOOTLOADER_ERROR_PARSER_KEYERROR;
        } else {
          return BOOTLOADER_ERROR_PARSER_UNKNOWN_TAG;
        }
      }
      break;
  }
#ifndef BTL_PARSER_NO_SUPPORT_ENCRYPTION
// *INDENT-OFF*
  }
// *INDENT-ON*
#endif
  return BOOTLOADER_ERROR_PARSER_PARSED;
}

/***************************************************************************//**
 * Initialize symmetric encryption context with key and nonce/IV.
 *
 * @param[in, out] parserContext Context variable
 * @param[in] input Input buffer
 * @returns BOOTLOADER_ERROR_PARSER_PARSED when successfully parsed.
 *
 * Updates parserContext->internalState.
 * Next state: GblParserStateIdle
 ******************************************************************************/
#ifndef BTL_PARSER_NO_SUPPORT_ENCRYPTION
// Initialize encryption context
static int32_t parser_encryptionInit(ParserContext_t  *parserContext,
                                     GblInputBuffer_t *input)
{
  volatile int32_t retval;
  uint8_t tagBuffer[GBL_PARSER_BUFFER_SIZE];

  // This is a fixed size header, so let's get it all at once.
  while (parserContext->offsetInTag < 16UL) {
    // Get data
    // Hash, but don't decrypt, since ENC_INIT is always in the clear
    retval = gbl_getData(parserContext,
                         input,
                         tagBuffer,
                         16UL,
                         true,
                         false);
    if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
      return retval;
    }

    // Initialize AES-CCM
    btl_initAesCcm(parserContext->aesContext,
                   0x02U,
                   &(tagBuffer[4]),
                   1UL, // Ember starts counter at 1?
                   btl_getImageFileEncryptionKeyPtr(),
                   128UL);
  }

  parserContext->internalState = GblParserStateIdle;
  return BOOTLOADER_ERROR_PARSER_PARSED;
}
#endif

/***************************************************************************//**
 * Parse information about the application contained in the GBL file.
 *
 * @param[in, out] parserContext Context variable
 * @param[in] input Input buffer
 * @param[in, out] imageProperties State of the image file being processed
 * @returns BOOTLOADER_ERROR_PARSER_PARSED when successfully parsed.
 *
 * Updates parserContext->internalState.
 * Success:   GblParserStateIdle
 * Otherwise: GblParserStateError
 ******************************************************************************/
static int32_t parser_parseApplicationInfo(ParserContext_t   *parserContext,
                                           GblInputBuffer_t  *input,
                                           ImageProperties_t *imageProperties)
{
  volatile int32_t retval;
  uint8_t tagBuffer[GBL_PARSER_BUFFER_SIZE];

  while (parserContext->offsetInTag < parserContext->lengthOfTag) {
    // Get data
    retval = gbl_getData(parserContext,
                         input,
                         tagBuffer,
                         parserContext->lengthOfTag,
                         true,
                         true);
    if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
      return retval;
    }

    // Received full application data struct
    (void) memcpy(&imageProperties->application,
                  tagBuffer,
                  sizeof(ApplicationData_t));

    imageProperties->contents |= BTL_IMAGE_CONTENT_APPLICATION;

#if defined(PARSER_HAS_APPLICATION_UPGRADE_VALIDATION_CALLBACK) \
    && (PARSER_HAS_APPLICATION_UPGRADE_VALIDATION_CALLBACK == true)
    if (parser_applicationUpgradeValidCallback(&imageProperties->application) == false) {
      // Application didn't check out
      parserContext->internalState = GblParserStateError;
      return BOOTLOADER_ERROR_PARSER_REJECTED;
    }
#endif // PARSER_HAS_APPLICATION_UPGRADE_VALIDATION_CALLBACK

#if !defined(_SILICON_LABS_GECKO_INTERNAL_SDID_80) && (PARSER_REQUIRE_ANTI_ROLLBACK_PROTECTION == true)
    if ((&imageProperties->application == NULL)
        || (bootload_verifyApplicationVersion(imageProperties->application.version, false) == false)) {
      parserContext->internalState = GblParserStateError;
      return BOOTLOADER_ERROR_PARSER_REJECTED;
    }
#endif
  }

  parserContext->internalState = GblParserStateIdle;
  return BOOTLOADER_ERROR_PARSER_PARSED;
}

/***************************************************************************//**
 * Parse Secure Engine upgrade tag header, set flags and prepare for upgrade.
 *
 * @param[in, out] parserContext Context variable
 * @param[in] input Input buffer
 * @param[out] imageProperties State of the image file being processed
 * @param[in] callbacks Callback functions
 * @returns BOOTLOADER_ERROR_PARSER_PARSED when successfully parsed.
 *
 * Updates parserContext->internalState.
 * Next state: GblParserStateSeData
 ******************************************************************************/
#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
static int32_t parser_parseSe(ParserContext_t                   *parserContext,
                              GblInputBuffer_t                  *input,
                              ImageProperties_t                 *imageProperties,
                              const BootloaderParserCallbacks_t *callbacks)
{
  volatile int32_t retval;
  uint8_t tagBuffer[GBL_PARSER_BUFFER_SIZE];

  while (parserContext->offsetInTag < 8UL) {
    // Get data
    retval = gbl_getData(parserContext,
                         input,
                         &tagBuffer[8],
                         8UL,
                         true, /* Do SHA hashing */
                         true /* Decrypt if necessary */);
    if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
      return retval;
    }

    imageProperties->contents |= BTL_IMAGE_CONTENT_SE;
    imageProperties->seUpgradeVersion =
      GBL_PARSER_ARRAY_TO_U32(tagBuffer, 12);

    if (imageProperties->instructions & BTL_IMAGE_INSTRUCTION_SE) {
      // Reinsert GBL tag header into data stream
      tagBuffer[0] = 0xEBU;
      tagBuffer[1] = 0x17U;
      tagBuffer[2] = 0xA6U;
      tagBuffer[3] = 0x5EU;

      // Save GBL tag length as withheld data
      memcpy(&parserContext->withheldBootloaderVectors, (void *)&parserContext->lengthOfTag, 4UL);

      parserContext->tagAddress = 0UL;
      parserContext->receivedFlags |= BTL_PARSER_RECEIVED_SE;

      // Pass 4 first words to SE upgrade
      if ((callbacks->bootloaderCallback != NULL)) {
        // SE data
#if !defined(BOOTLOADER_SE_UPGRADE_NO_STAGING) \
        || (BOOTLOADER_SE_UPGRADE_NO_STAGING == 0)
        callbacks->bootloaderCallback(parserContext->tagAddress,
                                      tagBuffer,
                                      4U,
                                      callbacks->context);
        callbacks->bootloaderCallback(parserContext->tagAddress + 8U,
                                      &tagBuffer[8],
                                      8U,
                                      callbacks->context);
#endif
        parserContext->tagAddress += 16U;
      }
    }

    parserContext->internalState = GblParserStateSeData;
  }

  return BOOTLOADER_ERROR_PARSER_PARSED;
}
#endif

/***************************************************************************//**
 * Parse Bootloader upgrade tag header, set flags and prepare for upgrade.
 *
 * @param[in, out] parserContext Context variable
 * @param[in] input Input buffer
 * @param[out] imageProperties State of the image file being processed
 * @returns BOOTLOADER_ERROR_PARSER_PARSED when successfully parsed.
 *
 * Updates parserContext->internalState.
 * Success:   GblParserStateBootloaderData
 * Otherwise: GblParserStateError
 ******************************************************************************/
static int32_t parser_parseBootloader(ParserContext_t   *parserContext,
                                      GblInputBuffer_t  *input,
                                      ImageProperties_t *imageProperties)
{
  volatile int32_t retval;
  uint32_t temporaryWord;
  uint8_t tagBuffer[GBL_PARSER_BUFFER_SIZE];

  while (parserContext->offsetInTag < 8UL) {
    // Get data
    retval = gbl_getData(parserContext,
                         input,
                         tagBuffer,
                         8UL,
                         true, /* Do SHA hashing */
                         true /* Decrypt if necessary */);
    if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
      return retval;
    }

    imageProperties->contents |= BTL_IMAGE_CONTENT_BOOTLOADER;
    imageProperties->bootloaderVersion = GBL_PARSER_ARRAY_TO_U32(
      tagBuffer,
      0);
    imageProperties->bootloaderUpgradeSize = parserContext->lengthOfTag - 8U;

    // Sanity check bootloader base address
    temporaryWord = GBL_PARSER_ARRAY_TO_U32(tagBuffer, 4);

#if defined(BOOTLOADER_HAS_FIRST_STAGE)
    if (firstBootloaderTable->header.type == BOOTLOADER_MAGIC_FIRST_STAGE) {
      if (temporaryWord != (uint32_t) firstBootloaderTable->mainBootloader) {
        parserContext->internalState = GblParserStateError;
        return BOOTLOADER_ERROR_PARSER_UNEXPECTED;
      }
    } else {
      return BOOTLOADER_ERROR_PARSER_UNEXPECTED;
    }
#else
    if (temporaryWord != FLASH_BASE) {
      // Bootloader has to start at beginning of flash
      parserContext->internalState = GblParserStateError;
      return BOOTLOADER_ERROR_PARSER_UNEXPECTED;
    }
#endif
    parserContext->internalState = GblParserStateBootloaderData;
    parserContext->receivedFlags |= BTL_PARSER_RECEIVED_BOOTLOADER;
    parserContext->tagAddress = 0UL;
  }

  return BOOTLOADER_ERROR_PARSER_PARSED;
}

/***************************************************************************//**
 * Parse Programming tag header.
 *
 * @param[in, out] parserContext Context variable
 * @param[in] input Input buffer
 * @returns BOOTLOADER_ERROR_PARSER_PARSED when successfully parsed.
 *
 * Updates parserContext->internalState.
 * Next state: GblParserStateProgData
 ******************************************************************************/
static int32_t parser_parseProg(ParserContext_t  *parserContext,
                                GblInputBuffer_t *input)
{
  volatile int32_t retval;
  uint32_t temporaryWord;
  uint8_t tagBuffer[GBL_PARSER_BUFFER_SIZE];

  while (parserContext->offsetInTag < 4UL) {
    // Get data
    retval = gbl_getData(parserContext,
                         input,
                         tagBuffer,
                         4UL,
                         true, /* Do SHA hashing */
                         true /* Decrypt if necessary */);
    if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
      return retval;
    }

    temporaryWord = GBL_PARSER_ARRAY_TO_U32(tagBuffer, 0);

    if (parserContext->lengthOfTag > 4UL) {
      // Only set programmingAddress if the tag actually contains data
      parserContext->programmingAddress = temporaryWord;
    }
  }

  parserContext->internalState = GblParserStateProgData;
  return BOOTLOADER_ERROR_PARSER_PARSED;
}

/***************************************************************************//**
 * Parse the data part of the (Se|Prog|Bootloader|Metadata) tags.
 *
 * @param[in, out] parserContext Context variable
 * @param[in] input Input buffer
 * @param[in] imageProperties State of the image file being processed
 * @param[in] callbacks Callback functions
 * @returns BOOTLOADER_ERROR_PARSER_PARSED when successfully parsed.
 *
 * Updates parserContext->internalState.
 * Success:   GblParserStateIdle
 * Otherwise: GblParserStateError
 ******************************************************************************/
static int32_t parser_parseData(ParserContext_t                   *parserContext,
                                GblInputBuffer_t                  *input,
                                ImageProperties_t                 *imageProperties,
                                const BootloaderParserCallbacks_t *callbacks)
{
  volatile int32_t retval;
  uint8_t tagBuffer[GBL_PARSER_BUFFER_SIZE];
  size_t tmpSize;

  while (parserContext->offsetInTag < parserContext->lengthOfTag) {
    // Get amount of bytes left in this tag
    tmpSize = parserContext->lengthOfTag - parserContext->offsetInTag;

    // Check buffer size vs. bytes we want to parse
    if (tmpSize >= 4UL) {
      // Always parse minimum one word
      if (gbl_getBytesAvailable(parserContext, input) < 4UL) {
        (void) gbl_storeData(parserContext, input);
        return BOOTLOADER_OK;
      }
    } else if (gbl_getBytesAvailable(parserContext, input) < tmpSize) {
      (void) gbl_storeData(parserContext, input);
      return BOOTLOADER_OK;
    } else {
      // There is less than a word left of this tag, and we have it all
    }
    // The amount of data we're going to parse in this cycle equals
    // min(bytes in buffer, bytes left in tag, size of internal buffer)
    if (tmpSize > GBL_PARSER_BUFFER_SIZE) {
      tmpSize = GBL_PARSER_BUFFER_SIZE;
    }
    if (tmpSize > gbl_getBytesAvailable(parserContext, input)) {
      tmpSize = gbl_getBytesAvailable(parserContext, input);
    }

    // Make sure to read word-sized chunks from the buffer for as long
    // as possible.
    // We can safely do the rounding down since we already verified
    // there are 4+ bytes available, or we're at the end of the tag.
    if (tmpSize >= 4UL) {
      tmpSize &= ~3UL;
    }

    // Consume data
    retval = gbl_getData(parserContext,
                         input,
                         tagBuffer,
                         tmpSize,
                         true,
                         true);
    if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
      return retval;
    }

    // Push back data
    if ((parserContext->internalState == GblParserStateMetadataData)
        && (callbacks->metadataCallback != NULL)) {
      callbacks->metadataCallback(parserContext->tagAddress,
                                  tagBuffer,
                                  tmpSize,
                                  callbacks->context);
      parserContext->tagAddress += tmpSize;
    } else {
      while (tmpSize < 4UL) {
        tagBuffer[tmpSize] = 0xFFU;
        tmpSize++;
      }

      if (parserContext->internalState == GblParserStateProgData) {
#if !defined(_SILICON_LABS_GECKO_INTERNAL_SDID_80) && (PARSER_REQUIRE_ANTI_ROLLBACK_PROTECTION == true)
        // Check for remaining application upgrades if rollback-protection is enabled.
        // Make sure that there are some remaining application upgrades left.
        if (callbacks->applicationCallback != NULL) {
          if ((&imageProperties->application == NULL)
              || (bootload_verifyApplicationVersion(imageProperties->application.version, true) == false)) {
            parserContext->internalState = GblParserStateError;
            return BOOTLOADER_ERROR_PARSER_REJECTED;
          }
        }
#endif
        // Application data
        retval = gbl_writeProgData(parserContext, tagBuffer, tmpSize, callbacks);
        if (retval != BOOTLOADER_OK) {
          return retval;
        }
      } else if ((parserContext->internalState == GblParserStateBootloaderData)
                 && (imageProperties->instructions & BTL_IMAGE_INSTRUCTION_BOOTLOADER)
                 && (callbacks->bootloaderCallback != NULL)) {
        // Bootloader data
        // If bootloader initial PC is in this call, store it and override
        // to FF. Initial PC will be passed to the callback at the end
        // when entire GBL is validated.
        if ((parserContext->tagAddress <= 4UL)
            && ((parserContext->tagAddress + tmpSize) >= 7UL)) {
          uint32_t bufferedBtlPcAddress =
            ((uint32_t)tagBuffer)
            + (4UL - parserContext->tagAddress);
          (void) memcpy(parserContext->withheldBootloaderVectors,
                        (void*)bufferedBtlPcAddress,
                        4U);
          (void) memset((void*)bufferedBtlPcAddress,
                        0xFF,
                        4U);
        }

        callbacks->bootloaderCallback(parserContext->tagAddress,
                                      tagBuffer,
                                      tmpSize,
                                      callbacks->context);
        parserContext->tagAddress += tmpSize;
#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
      } else if ((parserContext->internalState == GblParserStateSeData)
                 && (imageProperties->instructions & BTL_IMAGE_INSTRUCTION_SE)
                 && (callbacks->bootloaderCallback != NULL)) {
        // SE data
        // Re-use the bootloader callback
#if !defined(BOOTLOADER_SE_UPGRADE_NO_STAGING) \
        || (BOOTLOADER_SE_UPGRADE_NO_STAGING == 0)
        callbacks->bootloaderCallback(parserContext->tagAddress,
                                      tagBuffer,
                                      tmpSize,
                                      callbacks->context);
        parserContext->tagAddress += tmpSize;
#endif // BOOTLOADER_SE_UPGRADE_NO_STAGING
#endif // SEMAILBOX_PRESENT
      } else {
        // Not a valid tag
      }
    }
  }

  parserContext->internalState = GblParserStateIdle;
  return BOOTLOADER_ERROR_PARSER_PARSED;
}

/***************************************************************************//**
 * Parse Custom tag.
 *
 * @param[in, out] parserContext Context variable
 * @param[in] input Input buffer
 * @param[in] imageProperties State of the image file being processed
 * @param[in] callbacks Callback functions
 * @returns BOOTLOADER_ERROR_PARSER_PARSED when successfully parsed.
 *
 * Updates parserContext->internalState.
 * Success:   GblParserStateIdle
 * Otherwise: GblParserStateError
 ******************************************************************************/
static int32_t parser_parseCustomTag(ParserContext_t                   *parserContext,
                                     GblInputBuffer_t                  *input,
                                     ImageProperties_t                 *imageProperties,
                                     const BootloaderParserCallbacks_t *callbacks)
{
  volatile int32_t retval;
  uint8_t tagBuffer[GBL_PARSER_BUFFER_SIZE];
  size_t tmpSize;
  const GblCustomTag_t *customTag = NULL;

  if (parserContext->flags & PARSER_FLAG_PARSE_CUSTOM_TAGS) {
    customTag = gbl_getCustomTagProperties(parserContext->customTagId);
    if (!customTag) {
      return BOOTLOADER_ERROR_PARSER_UNEXPECTED;
    }
  }

#if !defined(_SILICON_LABS_GECKO_INTERNAL_SDID_80) && (PARSER_REQUIRE_ANTI_ROLLBACK_PROTECTION == true)
  // Check for remaining application upgrades if rollback-protection is enabled.
  // Make sure that there are some remaining application upgrades left.
  if (callbacks->applicationCallback != NULL) {
    if ((&imageProperties->application == NULL)
        || (bootload_verifyApplicationVersion(imageProperties->application.version, true) == false)) {
      parserContext->internalState = GblParserStateError;
      return BOOTLOADER_ERROR_PARSER_REJECTED;
    }
  }
#elif defined(__GNUC__)
  (void) imageProperties;
#endif

  while (parserContext->offsetInTag < parserContext->lengthOfTag) {
    if ((parserContext->flags & PARSER_FLAG_PARSE_CUSTOM_TAGS)
        && customTag && (customTag->numBytesRequired)) {
      tmpSize = customTag->numBytesRequired(parserContext);
    } else {
      tmpSize = 1UL;
    }
    if (gbl_getBytesAvailable(parserContext, input) < tmpSize) {
      // Not enough data available
      (void) gbl_storeData(parserContext, input);
      return BOOTLOADER_OK;
    }

    // The amount of data we're going to parse in this cycle equals
    // min(size of internal buffer, bytes left in tag, bytes in combined buffers)
    tmpSize = SL_MIN(GBL_PARSER_BUFFER_SIZE,
                     parserContext->lengthOfTag - parserContext->offsetInTag);
    tmpSize = SL_MIN(tmpSize, gbl_getBytesAvailable(parserContext, input));

    // Consume data
    retval = gbl_getData(parserContext,
                         input,
                         tagBuffer,
                         tmpSize,
                         true,
                         true);
    if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
      return retval;
    }

    if (parserContext->flags & PARSER_FLAG_PARSE_CUSTOM_TAGS) {
      if (customTag && (customTag->parseTag)) {
        retval = customTag->parseTag(parserContext,
                                     tagBuffer,
                                     tmpSize,
                                     callbacks);
        if (retval != BOOTLOADER_OK) {
          return retval;
        }
      } else {
        BTL_DEBUG_PRINTLN("No parse callback");
        return BOOTLOADER_ERROR_PARSER_UNEXPECTED;
      }
    } else {
      BTL_DEBUG_PRINTLN("Skipping tag");
    }
  }

  if (parserContext->flags & PARSER_FLAG_PARSE_CUSTOM_TAGS) {
    if (customTag && (customTag->exitTag)) {
      retval = customTag->exitTag(parserContext, callbacks);
      if (retval != BOOTLOADER_OK) {
        return retval;
      }
    }
  }

  parserContext->internalState = GblParserStateIdle;
  return BOOTLOADER_ERROR_PARSER_PARSED;
}

/***************************************************************************//**
 * Parse and verify signature from Certificate or Signature tag.
 *
 * @param[in, out] parserContext Context variable
 * @param[in] input Input buffer
 * @param[out] imageProperties State of the image file being processed
 * @returns BOOTLOADER_ERROR_PARSER_PARSED when successfully parsed.
 *
 * Updates parserContext->internalState.
 * Success:   GblParserStateIdle
 * Otherwise: GblParserStateError
 ******************************************************************************/
static int32_t parser_checkSignature(ParserContext_t   *parserContext,
                                     GblInputBuffer_t  *input,
                                     ImageProperties_t *imageProperties)
{
  volatile int32_t retval;
  uint8_t tagBuffer[GBL_PARSER_BUFFER_SIZE];

#if defined(_SILICON_LABS_32B_SERIES_2)
  // Access word 13 to read sl_app_properties of the bootloader.
  ApplicationProperties_t *blProperties = (ApplicationProperties_t *)(*(uint32_t *)(BTL_MAIN_STAGE_BASE + 52UL));
#if !defined(MAIN_BOOTLOADER_TEST)
  if ((uint32_t)blProperties > ((uint32_t)mainBootloaderTable->startOfAppSpace - sizeof(ApplicationProperties_t))) {
    // Make sure that this pointer is within the bootloader space.
    return BOOTLOADER_ERROR_PARSER_UNEXPECTED;
  }
#endif

  // Certificate tag
  if ((parserContext->internalState == GblParserStateCertificate)
      && (parserContext->gotCertificate == false)) {
    Sha256Context_t shaState;
    retval = parser_verifyCertificate(parserContext, input, blProperties, &shaState);
    if (retval == BOOTLOADER_ERROR_PARSER_SIGNATURE) {
      parserContext->internalState = GblParserStateError;
    }

    return retval;
  }
#endif

  // Signature tag
  if (parserContext->internalState == GblParserStateSignature) {
    // Make sure we have the necessary data
    if (!(parserContext->gotSignature)) {
      // Get data
      // No hashing (tag is unhashed), obviously no decryption
      retval = gbl_getData(parserContext,
                           input,
                           tagBuffer,
                           64UL,
                           false,
                           false);
      if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
        return retval;
      }

      btl_finalizeSha256(parserContext->shaContext);

#if defined(_SILICON_LABS_32B_SERIES_2)
      if (parserContext->gotCertificate) {
        retval = btl_verifyEcdsaP256r1(parserContext->shaContext,
                                       &tagBuffer[0],
                                       &tagBuffer[32],
                                       &(parserContext->certificate.key[0]),
                                       &(parserContext->certificate.key[32]));
      } else {
        // Received direct signed GBL
        // Here we have two options.
        // 1. The bootloader is configured to use public key of its certificate.
        // 2. The bootloader is configured to use either public key from the "lock bits" page or platform key.
        if (PARSER_REQUIRE_CERTIFICATE_AUTHENTICITY) {
          // The bootloader is configured to use public key of its certificate.
          retval = btl_verifyEcdsaP256r1(parserContext->shaContext,
                                         &tagBuffer[0],
                                         &tagBuffer[32],
                                         &(blProperties->cert->key[0]),
                                         &(blProperties->cert->key[32]));
#if defined(SEMAILBOX_PRESENT)
          if (retval != BOOTLOADER_OK) {
            // Check if the OTP platform key can verify the certificate.
            // The OTP platform key has higher level of trust.
            retval = btl_verifyEcdsaP256r1(parserContext->shaContext,
                                           &tagBuffer[0],
                                           &tagBuffer[32],
                                           NULL,
                                           NULL);
          }
#endif
        } else {
          // The bootloader is configured to use either public key from the "lock bits" page or platform key.
          retval = btl_verifyEcdsaP256r1(parserContext->shaContext,
                                         &tagBuffer[0],
                                         &tagBuffer[32],
                                         btl_getSignedBootloaderKeyXPtr(),
                                         btl_getSignedBootloaderKeyYPtr());
        }
      }
#else
      retval = btl_verifyEcdsaP256r1(parserContext->shaContext,
                                     &tagBuffer[0],
                                     &tagBuffer[32],
                                     btl_getSignedBootloaderKeyXPtr(),
                                     btl_getSignedBootloaderKeyYPtr());
#endif
      if (retval != BOOTLOADER_OK) {
        BTL_DEBUG_PRINTLN("GBL verify fail");
        imageProperties->imageVerified = false;
        parserContext->internalState = GblParserStateError;
        return BOOTLOADER_ERROR_PARSER_SIGNATURE;
      } else {
        imageProperties->imageVerified = true;
      }

      parserContext->gotSignature = true;
    }
  }

  parserContext->internalState = GblParserStateIdle;
  return BOOTLOADER_ERROR_PARSER_PARSED;
}

/***************************************************************************//**
 * Finalize parsing of GBL file after encountering an End tag:
 * - Check CRC checksum to verify image integrity.
 * - Flash withheld information, such as the program counter,
 * when the authenticity of the image has been verified (if required).
 *
 * @param[in, out] parserContext Context variable
 * @param[in] input Input buffer
 * @param[in, out] imageProperties State of the image file being processed
 * @param[in] callbacks Callback functions
 * @returns BOOTLOADER_OK when successfully parsed.
 *
 * Updates parserContext->internalState.
 * Success:   GblParserStateDone
 * Otherwise: GblParserStateError
 ******************************************************************************/
static int32_t parser_finalize(ParserContext_t                   *parserContext,
                               GblInputBuffer_t                  *input,
                               ImageProperties_t                 *imageProperties,
                               const BootloaderParserCallbacks_t *callbacks)
{
  volatile int32_t retval;
  uint32_t temporaryWord;
  uint8_t tagBuffer[GBL_PARSER_BUFFER_SIZE];

  // Get data
  // Don't hash, don't decrypt
  retval = gbl_getData(parserContext,
                       input,
                       tagBuffer,
                       4UL,
                       false,
                       false);
  if (retval != BOOTLOADER_ERROR_PARSER_PARSED) {
    return retval;
  }

  // Check CRC
  if (parserContext->fileCrc != BTL_CRC32_END) {
    parserContext->internalState = GblParserStateError;
    return BOOTLOADER_ERROR_PARSER_CRC;
  }

  if (!PARSER_REQUIRE_AUTHENTICITY) {
    // Set image as verified if verification is not required
    imageProperties->imageVerified = true;
  }

#if defined(BTL_PARSER_SUPPORT_VERSION_DEPENDENCY_TAG) && defined(BOOTLOADER_SUPPORT_STORAGE)
  // Version dependency check
  uint8_t resultApp        = parserContext->versionDependencyResultApp;
  uint8_t resultBootloader = parserContext->versionDependencyResultBootloader;
  uint8_t resultSe         = parserContext->versionDependencyResultSe;

  // If the GBL contains an SE or bootloader upgrade image and the running
  // (SE|bootloader) version matches the one stored in the image, this
  // indicates that an upgrade has taken place and the version dependency tag
  // has already been evaluated once.
  bool skipVersionDependencyCheck = false;

#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
  if (parserContext->receivedFlags & BTL_PARSER_RECEIVED_SE) {
    uint32_t runningSeVersion;
    if (!bootload_getSeVersion(&runningSeVersion)) {
      parserContext->internalState = GblParserStateError;
      return BOOTLOADER_ERROR_PARSER_UNEXPECTED;
    }
    if (runningSeVersion == imageProperties->seUpgradeVersion) {
      skipVersionDependencyCheck = true;
    }
  }
#endif
  if (parserContext->receivedFlags & BTL_PARSER_RECEIVED_BOOTLOADER) {
    if (bootload_getBootloaderVersion() == imageProperties->bootloaderVersion) {
      skipVersionDependencyCheck = true;
    }
  }

  if ((skipVersionDependencyCheck == false)
      && ((!resultApp) || (!resultBootloader) || (!resultSe))) {
    parserContext->internalState = GblParserStateError;
    return BOOTLOADER_ERROR_PARSER_VERSION;
  }
#endif // defined(BTL_PARSER_SUPPORT_VERSION_DEPENDENCY_TAG) && defined(BOOTLOADER_SUPPORT_STORAGE)

  // Flash withheld information now if authenticity was not required
  // or we have verified the signature. CRC is OK, otherwise we'd have
  // errored.
  if (imageProperties->imageVerified) {
    // We have a bootloader PC to write
    if ((parserContext->receivedFlags & BTL_PARSER_RECEIVED_BOOTLOADER)
        && (callbacks->bootloaderCallback != NULL)) {
      callbacks->bootloaderCallback(4UL,
                                    parserContext->withheldBootloaderVectors,
                                    4U,
                                    callbacks->context);
    }
#if !defined(BOOTLOADER_SE_UPGRADE_NO_STAGING) \
    || (BOOTLOADER_SE_UPGRADE_NO_STAGING == 0)
    // We have an SE length to write
    if ((parserContext->receivedFlags & BTL_PARSER_RECEIVED_SE)
        && (callbacks->bootloaderCallback != NULL)) {
      callbacks->bootloaderCallback(4UL,
                                    parserContext->withheldBootloaderVectors,
                                    4U,
                                    callbacks->context);
    }
#endif
    // If programmingAddress != 0, we have an application PC to write
    if ((parserContext->programmingAddress != 0UL)
        && (callbacks->applicationCallback != NULL)) {
      temporaryWord = 0xFFFFFFFFUL;
      if (memcmp(&temporaryWord, parserContext->withheldUpgradeVectors, 4U) != 0) {
        // Data has been withheld from the bootloader upgrade area
        uint32_t btlUpgradeAddress = BTL_UPGRADE_LOCATION;
        if (btlUpgradeAddress > 0UL) {
          // Bootloader upgrade address is valid
          callbacks->applicationCallback(btlUpgradeAddress + 4UL,
                                         parserContext->withheldUpgradeVectors,
                                         4U,
                                         callbacks->context);
        }
      }

      if (memcmp(&temporaryWord, parserContext->withheldApplicationVectors, 4U) != 0) {
        uint32_t startOfAppSpace = (uint32_t) mainBootloaderTable->startOfAppSpace;
        // Data has been withheld from the application vector table
        // Return everything but the program counter
        callbacks->applicationCallback(startOfAppSpace + 8UL,
                                       &parserContext->withheldApplicationVectors[4],
                                       20U,
                                       callbacks->context);
        // To ensure safe operation if a power loss occurs, return the
        // program counter last. If secure boot is not enabled, the
        // presence of a valid PC signals that the application is valid.
        callbacks->applicationCallback(startOfAppSpace + 4UL,
                                       parserContext->withheldApplicationVectors,
                                       4U,
                                       callbacks->context);
      }
    }
  }

  // Report done to bootloader
  imageProperties->imageCompleted = true;
  parserContext->internalState = GblParserStateDone;
  return BOOTLOADER_OK;
}
