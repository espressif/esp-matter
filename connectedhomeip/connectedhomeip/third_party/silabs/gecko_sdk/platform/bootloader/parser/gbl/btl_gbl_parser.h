/***************************************************************************//**
 * @file
 * @brief GBL image file parser.
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
#ifndef BTL_GBL_PARSER_H
#define BTL_GBL_PARSER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "security/btl_security_aes.h"
#include "security/btl_security_sha256.h"
#include "security/btl_security_types.h"

#include "config/btl_config.h"

#include "api/btl_interface.h"
#include "api/btl_interface_parser.h"
#include "api/application_properties.h"

/***************************************************************************//**
 * @addtogroup Components
 * @{
 * @addtogroup ImageParser Image Parser
 * @{
 * @addtogroup GblParser GBL Parser
 * @{
 * @brief GBL parser implementation.
 * @details
 *   Image parser for GBL files. Parses GBL files based on the
 *   [GBL file format specification](@ref GblParserFormat). Callbacks are used
 *   to present data and metadata contents of the GBL file to the bootloader.
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Defines

/// GBL file is encrypted
#define PARSER_FLAG_ENCRYPTED               (1U << 0U)
/// Parse custom tags rather than silently traversing them
#define PARSER_FLAG_PARSE_CUSTOM_TAGS       (1U << 5U)

/// Some flags are public, some are internal to the parser
#define PARSER_FLAGS_PUBLIC_MASK            PARSER_FLAG_PARSE_CUSTOM_TAGS

/// GBL parser buffer size
#define GBL_PARSER_BUFFER_SIZE              64UL

/***************************************************************************//**
 * Bootloader/parser configurations
 ******************************************************************************/
/// Parser requires upgrade images to be signed, providing authenticity, if true.
#if defined(BOOTLOADER_ENFORCE_SIGNED_UPGRADE) && (BOOTLOADER_ENFORCE_SIGNED_UPGRADE == 1)
#define PARSER_REQUIRE_AUTHENTICITY (true)
#else
#define PARSER_REQUIRE_AUTHENTICITY (false)
#endif

/// Parser requires upgrade images to be encrypted, providing confidentiality, if true.
#if defined(BOOTLOADER_ENFORCE_ENCRYPTED_UPGRADE) && (BOOTLOADER_ENFORCE_ENCRYPTED_UPGRADE == 1)
#define PARSER_REQUIRE_CONFIDENTIALITY (true)
#else
#define PARSER_REQUIRE_CONFIDENTIALITY (false)
#endif

/// Parser requires upgrade images to be authenticated by the bootloader certificate, if true.
#ifndef PARSER_REQUIRE_CERTIFICATE_AUTHENTICITY
#if defined(BOOTLOADER_SUPPORT_CERTIFICATES) && (BOOTLOADER_SUPPORT_CERTIFICATES == 1)
#define PARSER_REQUIRE_CERTIFICATE_AUTHENTICITY (true)
#else
#define PARSER_REQUIRE_CERTIFICATE_AUTHENTICITY (false)
#endif
#endif

/// Parser requires rollback protection of applications, if true.
#if defined(BOOTLOADER_ROLLBACK_PROTECTION) && (BOOTLOADER_ROLLBACK_PROTECTION == 1)
#define PARSER_REQUIRE_ANTI_ROLLBACK_PROTECTION (true)
#else
#define PARSER_REQUIRE_ANTI_ROLLBACK_PROTECTION (false)
#endif

/// Defines the minimum application version that can be accepted.
#if defined(BOOTLOADER_ROLLBACK_PROTECTION_MINIMUM_VERSION) && (BOOTLOADER_ROLLBACK_PROTECTION_MINIMUM_VERSION != 0)
#define PARSER_APPLICATION_MINIMUM_VERSION_VALID  BOOTLOADER_ROLLBACK_PROTECTION_MINIMUM_VERSION
#else
#define PARSER_APPLICATION_MINIMUM_VERSION_VALID (0UL)
#endif

// -----------------------------------------------------------------------------
// Enums

/// State in the GBL parser state machine
typedef enum {
  GblParserStateInit,                 ///< Initial state
  GblParserStateIdle,                 ///< Idle state
  GblParserStateHeader,               ///< Parsing header tag
  GblParserStateBootloader,           ///< Parsing bootloader tag
  GblParserStateBootloaderData,       ///< Parsing bootloader tag data
  GblParserStateApplication,          ///< Parsing application tag
  GblParserStateMetadata,             ///< Parsing metadata tag
  GblParserStateMetadataData,         ///< Parsing metadata tag data
  GblParserStateProg,                 ///< Parsing flash program tag
  GblParserStateProgData,             ///< Parsing flash program tag data
#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
  GblParserStateSe,                   ///< Parsing SE tag
  GblParserStateSeData,               ///< Parsing SE tag data
#endif
  GblParserStateEraseProg,            ///< Parsing flash erase&program tag
  GblParserStateFinalize,             ///< Finalizing file
  GblParserStateDone,                 ///< Parsing complete
  GblParserStateEncryptionInit,       ///< Parsing encryption init tag
  GblParserStateEncryptionContainer,  ///< Parsing encryption data tag
#if defined(_SILICON_LABS_32B_SERIES_2)
  GblParserStateCertificate,          ///< Parsing certificate tag
#endif
#if defined(BTL_PARSER_SUPPORT_VERSION_DEPENDENCY_TAG)
  GblParserStateVersionDependency,    ///< Parsing version dependency tag
#endif
  GblParserStateSignature,            ///< Parsing signature tag
  GblParserStateCustomTag,            ///< Parsing custom tag
  GblParserStateError                 ///< Error state
} GblParserState_t;

// -----------------------------------------------------------------------------
// Structs

/// Structure containing state of the image file processed
typedef struct {
  /// Image contents
  uint8_t                     contents;
  /// Parser instructions
  uint8_t                     instructions;
  /// Flag to indicate parsing has completed
  bool                        imageCompleted;
  /// Flag to indicate the image file has been validated
  bool                        imageVerified;
  /// Version number of main bootloader extracted from image file
  uint32_t                    bootloaderVersion;
  /// Information about the application
  ApplicationData_t           application;
  /// Size of the bootloader upgrade contained in the image file
  uint32_t                    bootloaderUpgradeSize;
#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
  /// Version number of SE upgrade extracted from image file
  uint32_t                    seUpgradeVersion;
#endif
} ImageProperties_t;

#if defined(BOOTLOADER_SECURE)
/// NS Structure containing state of the image file processed.
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
} ImageProperties_NS_t;
#endif

/// Upgrade image contains application upgrade
#define BTL_IMAGE_CONTENT_APPLICATION       0x01U
/// Upgrade image contains bootloader upgrade
#define BTL_IMAGE_CONTENT_BOOTLOADER        0x02U
/// Upgrade image contains SE upgrade
#define BTL_IMAGE_CONTENT_SE                0x04U

/// Application upgrade should be applied from upgrade image
#define BTL_IMAGE_INSTRUCTION_APPLICATION   0x01U
/// Bootloader upgrade should be applied from upgrade image
#define BTL_IMAGE_INSTRUCTION_BOOTLOADER    0x02U
/// SE upgrade should be applied from upgrade image
#define BTL_IMAGE_INSTRUCTION_SE            0x04U

/// Image parser context definition
typedef struct {
  /// Buffer contents
  uint8_t             internalBuffer[64];
  /// Amount of bytes present in buffer
  uint8_t             bytesInInternalBuffer;
  /// Current reading offset into the buffer (circular)
  uint8_t             internalBufferOffset;
  /// Parser flags
  uint8_t             flags;
  /// Parser is currently inside an encrypted tag
  bool                inEncryptedContainer;
  /// Parser has received and verified signature
  bool                gotSignature;
  /// Parser has received bootloader upgrade tag
  uint8_t             receivedFlags;
  /// State of the GBL parser state machine
  GblParserState_t    internalState;
  /// AES-CCM decryption (= AES-CTR) context
  void                *aesContext;
  /// SHA256 hashing context
  void                *shaContext;
  /// Total length of the tag currently being parsed
  size_t              lengthOfTag;
  /// Current offset into tag being parsed
  size_t              offsetInTag;
  /// Total length of current encrypted data block
  size_t              lengthOfEncryptedTag;
  /// Offset into current encrypted data block
  size_t              offsetInEncryptedTag;
  /// Current address the image needs to be written to
  uint32_t            programmingAddress;
  /// Current offset of metadata/bootloader being handled (starts at 0)
  uint32_t            tagAddress;
  /// Withheld application data
  uint8_t             withheldApplicationVectors[24];
  /// Withheld bootloader upgrade data during app parsing
  uint8_t             withheldUpgradeVectors[4];
  /// Withheld bootloader upgrade data during bootloader parsing
  uint8_t             withheldBootloaderVectors[4];
  /// Running CRC-32 over the incoming GBL file
  uint32_t            fileCrc;
  /// Context for custom tag
  uint32_t            customTagId;
#if defined(_SILICON_LABS_32B_SERIES_2)
  /// GBL Certificate
  ApplicationCertificate_t certificate;
  /// Parser has received GBL certificate
  bool                gotCertificate;
  /// Reserved flags
  uint8_t             reservedFlags[3];
  /// Current offset in the GBL file being parsed
  size_t              offsetInGbl;
  /// Offset of SE upgrade tag in the GBL file
  size_t              offsetOfSeUpgradeTag;
  /// Version dependency result for application
  uint8_t             versionDependencyResultApp;
  /// Version dependency result for bootloader
  uint8_t             versionDependencyResultBootloader;
  /// Version dependency result for SE
  uint8_t             versionDependencyResultSe;
  /// Reserved byte
  uint8_t             reservedByte;
#endif
} ParserContext_t;

/// GBL parser input buffer
typedef struct {
  /// Pointer to a buffer
  const uint8_t *buffer;
  /// Length of the buffer
  const size_t  length;
  /// Offset of the buffer
  size_t        offset;
} GblInputBuffer_t;

/// @cond EXCLUDE_FROM_DOC
#define BTL_PARSER_RECEIVED_BOOTLOADER   1U
#define BTL_PARSER_RECEIVED_SE           2U
/// @endcond

/***************************************************************************//**
 * Initialize the parser's context.
 *
 * @param context         Pointer to context for the parser implementation
 * @param decryptContext  Pointer to context for decryption of parsed file
 * @param authContext     Pointer to context for authentication of parsed file
 * @param flags           Flags for parser support
 *
 * @return @ref BOOTLOADER_OK if OK, error code otherwise.
 ******************************************************************************/
int32_t parser_init(void *context,
                    void *decryptContext,
                    void *authContext,
                    uint8_t flags);

/***************************************************************************//**
 * Parse an image file to extract the binary and some metadata.
 *
 * Pushes data into the image file parser to be parsed.
 *
 * @param context Pointer to the specific parser's context variable
 * @param imageProperties Pointer to the image file state variable
 * @param buffer Pointer to byte array containing data to parse
 * @param length Size in bytes of the data in buffer
 * @param callbacks Struct containing function pointers to be called by the
 *   parser to pass the extracted binary data back to BTL.
 *
 * @return @ref BOOTLOADER_OK if OK, error code otherwise.
 ******************************************************************************/
int32_t parser_parse(void                              *context,
                     ImageProperties_t                 *imageProperties,
                     uint8_t                           buffer[],
                     size_t                            length,
                     const BootloaderParserCallbacks_t *callbacks);

#if defined(PARSER_HAS_APPLICATION_UPGRADE_VALIDATION_CALLBACK) \
  && (PARSER_HAS_APPLICATION_UPGRADE_VALIDATION_CALLBACK == 1)
/***************************************************************************//**
 * Callback to tell the parser whether to accept the application upgrade
 * present in the upgrade image.
 *
 * @param[in] app Pointer to the application data structure contained in the
 *                upgrade image.
 *
 * @return True if the application is accepted and the parser should continue.
 *         False if the application is rejected. If rejected, the parser will
 *         abort parsing the image file and return
 *         ::BOOTLOADER_ERROR_PARSER_REJECTED.
 ******************************************************************************/
bool parser_applicationUpgradeValidCallback(ApplicationData_t *app);
#endif // PARSER_HAS_APPLICATION_UPGRADE_VALIDATION_CALLBACK

/***************************************************************************//**
 * Verify the GBL certificate.
 *
 * @note The behavior of this function depends on the context state.
 *
 * @param[in,out]  context         GBL parser context
 * @param[in]      input           Input data
 * @param[in]      blProperties    Pointer to ::ApplicationProperties_t of
 *                                 bootloader
 * @param[in,out]  shaState        Pointer to ::Sha256Context_t used to store
 *                                 SHA256 of GBL certificate
 *
 * @return @ref BOOTLOADER_ERROR_PARSER_PARSED if done parsing the current
 *         input buffer. @ref BOOTLOADER_OK if input data is stored in the
 *         internal buffer. @ref BOOTLOADER_OK if the certificate
 *         in GBL is accepted. @ref BOOTLOADER_ERROR_PARSER_SIGNATURE
 *         if the certificate in GBL is rejected.
 ******************************************************************************/
int32_t parser_verifyCertificate(void *context,
                                 void *input,
                                 void *blProperties,
                                 void *shaState);

/***************************************************************************//**
 * Write application data. This function is called when parsing any tag with
 * @ref GblProg_t structured content.
 *
 * @param context     GBL parser context
 * @param buffer      Input buffer containing data to be written
 * @param length      Size of input buffer
 * @param callbacks   GBL Parser callbacks for writing data
 *
 * @return Error code
 ******************************************************************************/
int32_t gbl_writeProgData(ParserContext_t *context,
                          uint8_t buffer[],
                          size_t length,
                          const BootloaderParserCallbacks_t *callbacks);

/***************************************************************************//**
 * Get data from storage and internal input buffer. This function advances the
 * parser state.
 *
 * @param context         GBL parser context
 * @param input           Input data
 * @param outputBuffer    Output data
 * @param outputLength    Output data length
 * @param applySHA        Update SHA256 in the GBL parser context
 * @param decrypt         Decrypt the output data
 *
 * @return Error code
 ******************************************************************************/
int32_t gbl_getData(ParserContext_t  *context,
                    GblInputBuffer_t *input,
                    uint8_t          outputBuffer[],
                    size_t           outputLength,
                    bool             applySHA,
                    bool             decrypt);

/** @} addtogroup GblParser */
/** @} addtogroup ImageParser */
/** @} addtogroup Components */

#endif // BTL_GBL_PARSER_H
