/***************************************************************************//**
 * @file
 * @brief Application interface for ember bootloader
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef EMBER_BTL_INTERFACE_H
#define EMBER_BTL_INTERFACE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "em_device.h"

#define SL_EMBER_BOOTLOADER_TYPE_APPLICATION              1U
#define SL_EMBER_BOOTLOADER_TYPE_STANDALONE               2U

#if !defined(SL_EMBER_BOOTLOADER_TYPE)
  #error SL_EMBER_BOOTLOADER_TYPE is undefined
#elif (SL_EMBER_BOOTLOADER_TYPE != SL_EMBER_BOOTLOADER_TYPE_APPLICATION)
&& (SL_EMBER_BOOTLOADER_TYPE != SL_EMBER_BOOTLOADER_TYPE_STANDALONE)
  #error SL_EMBER_BOOTLOADER_TYPE has an invalid value
#endif

#define EBL_MIN_TAG_SIZE                                  128U
#define IMAGE_STAMP_SIZE                                  8U
#define IMAGE_INFO_MAXLEN_OLD                             32U
#define IMAGE_INFO_MAXLEN                                 16U
#define AAT_MAX_SIZE                                      128U
#define NONCE_LENGTH                                      12U
#define SECURITY_BLOCK_SIZE                               16U
#define EEPROM_PAGE_SIZE                                  128U

#define EMBER_BOOTLOADER_PARSE_CONTINUE                   0x50U
#define EMBER_BOOTLOADER_RESET_SIGNATURE_VALID            0xF00FU
#define EMBER_BOOTLOADER_RESET_REASON_BOOTLOAD            0x0202U
#define EMBER_BOOTLOADER_CAPABILITIES_PART_ERASE_SECONDS  0x0010U

typedef struct EblHeader_s {
  uint16_t tag;                 /* = EBLTAG_HEADER              */
  uint16_t len;                 /* =                            */
  uint16_t version;             /* Version of the EBL format    */
  uint16_t signature;           /* Magic signature: 0xE350      */
  uint32_t flashAddr;           /* Address where the AAT is stored */
  uint32_t aatCrc;              /* CRC of the EBL header portion of the AAT */
  // aatBuff is oversized to account for the potential of the AAT to grow in
  //  the future.  Only the first 128 bytes of the AAT can be referenced as
  //  part of the EBL header, although the AAT itself may grow to 256 total
  uint8_t  aatBuff[AAT_MAX_SIZE];         /* buffer for the EBL portion of the AAT    */
} EblHdeader_t;

typedef struct CcmState_s {
  uint32_t msgLen;                     /* Length of the encrypted data */
  uint8_t  nonce[NONCE_LENGTH];        /* The random nonce for this message */
  uint8_t  mac[SECURITY_BLOCK_SIZE];   /* The full rolling MAC value */
  uint32_t blockCount;                 /* Current AES block we're processing in this message */
  uint8_t  blockOffset;                /* Offset within the current AES block [0, SECURITY_BLOCK_SIZE] */
  uint8_t  macOffset;                  /* Last byte written to in the MAC buffer */
} CcmState_t;

typedef struct {
  uint8_t (*eblGetData)(void *state, uint8_t *dataBytes, uint16_t len);
  uint8_t (*eblDataFinalize)(void *state);
} EblDataFunction_t;

typedef struct {
  uint32_t     fileCrc;
  bool         headerValid;
  EblHdeader_t eblHeader;
  uint16_t     encType;
  uint8_t      encEblStateMachine;
  bool         decryptEnabled;
  uint32_t     byteCounter;
  CcmState_t   encState;
} EblProcessState_t;

typedef struct {
  void              *dataState;
  uint8_t           *tagBuf;
  uint16_t          tagBufLen;
  bool              returnBetweenBlocks;
  EblProcessState_t eblState;
} EblConfig_t;

typedef uint8_t (*flashReadFunc)(uint32_t address);

typedef struct {
  uint8_t (*erase)(uint8_t eraseType, uint32_t address);
  uint8_t (*write)(uint32_t address, uint16_t * data, uint32_t length);
  flashReadFunc read;
} EblFlashFunction_t;

typedef tVectorEntry VectorTable_t;

typedef struct {
  void                *stackTop;
  void                (*resetVector)(void);
  void                (*nmiHandler)(void);
  void                (*hardFaultHandler)(void);
  uint16_t            type;
  uint16_t            version;
  const VectorTable_t *vectorTable;
} BaseAddressTable_t;

typedef struct {
  /** Version, either of the bootloader for standalone bootloaders or of the
      storage info struct for application bootloaders*/
  uint16_t version;

#if SL_EMBER_BOOTLOADER_TYPE == SL_EMBER_BOOTLOADER_TYPE_APPLICATION
  /** A bitmask describing the capabilities of this particular external EEPROM */
  uint16_t capabilitiesMask;

  /** Maximum time it takes to erase a page. (in 1024Hz Milliseconds) */
  uint16_t pageEraseMs;

  /** Maximum time it takes to erase the entire part. (in 1024Hz Milliseconds).
      Can be changed to be in seconds using EEPROM_CAPABILITIES_PART_ERASE_SECONDS */
  uint16_t partEraseTime;

  /** The size of a single erasable page in bytes */
  uint32_t pageSize;

  /** The total size of the external EEPROM in bytes */
  uint32_t partSize;

  /** Pointer to a string describing the attached external EEPROM */
  const char * const partDescription;

  /** The number of bytes in a word for the external EEPROM **/
  uint8_t wordSizeBytes;
#endif
} BootloaderInformation_t;

typedef struct {
  uint32_t address;
  uint16_t pages;
  uint16_t pageBufFinger;
  uint16_t pageBufLen;
  uint8_t  pageBuf[EEPROM_PAGE_SIZE];
} BootloaderStorageState_t;

typedef struct {
  uint16_t reason;
  uint16_t signature;
} BootloaderResetCause_t;

typedef struct pageRange8bit_s {
  uint8_t begPg;    /* First flash page in range    */
  uint8_t endPg;    /* Last  flash page in range    */
} pageRange8bit_t;

#define NUM_AAT_PAGE_RANGES 6

typedef struct {
  BaseAddressTable_t baseTable;
  // The following fields are used for EBL and bootloader processing.
  uint8_t  platInfo;          // type of platform
  uint8_t  microInfo;         // type of micro
  uint8_t  phyInfo;           // type of PHY
  uint8_t  aatSize;           // size of the AAT itself
  uint16_t softwareVersion;   // EmberZNet SOFTWARE_VERSION
  uint16_t softwareBuild;     // EmberZNet EMBER_BUILD_NUMBER
  uint32_t timestamp;         // Unix epoch time of .EBL file, filled in by EBL gen

  // String, filled in by EBL generation. Used to be larger, reduced so that the
  // app properties struct could be placed in the same word for apps regardless
  // of whether they had an AAT (which Bluetooth and MCU apps do not).
  uint8_t         imageInfo[IMAGE_INFO_MAXLEN];
  const void      *applicationProperties; // ptr to app properties struct for Gecko bootloader
  // eat up the remainder of the previous imageInfo space
  uint8_t         reserved[IMAGE_INFO_MAXLEN_OLD - IMAGE_INFO_MAXLEN - sizeof(void*)];
  uint32_t        imageCrc;  // CRC over following pageRanges, filled in by EBL gen
  pageRange8bit_t pageRanges[NUM_AAT_PAGE_RANGES];  // Flash pages used by app.
                                                    // Filled in by EBL gen.
                                                    // 2 bytes per struct

  void *simeeSegmentBegin;                     // assumed to be 4 bytes on Cortex M3

  uint32_t customerApplicationVersion;         // a version field for the customer

  void *internalStorageBottom;                 // assumed to be 4 bytes on Cortex M3

  // A non-cryptographic hash of the entire on-chip image,
  // including AAT.  It uses AES-MMO, which is a cryptographic
  // hash, but because the length is only 64-bit there is a
  // greater chance of collisions.  It is not recommended to
  // use this to prove integrity in a cryptographic sense.
  // It is provided as a simple way to verify an EBL file
  // is the same as the one on-chip.
  uint8_t imageStamp[IMAGE_STAMP_SIZE];

  uint8_t familyInfo;                          // type of family

  // reserve the remainder of the first 128 bytes of the AAT in case we need
  // to go back and add any values that the bootloader will need to reference,
  // since only the first 128 bytes of the AAT become part of the EBL header.
  uint8_t bootloaderReserved[35 - (NUM_AAT_PAGE_RANGES * sizeof(pageRange8bit_t))];

  // part of AAT, not included as part of EBL header
  void *reserved1;
  void *noInitSegmentBegin;
  void *reserved2;
  void *bssSegmentEnd;
  void *stackSegmentEnd;
  void *reserved3;
  void *textSegmentEnd;
  void *stackSegmentBegin;
  void *heapSegmentEnd;
  void *simeeSegmentEnd;
  void *reserved4;
} AppAddressTable_t;

// Description of the Bootloader Address Table (BAT)
typedef struct {
  BaseAddressTable_t      baseTable;
  uint16_t                bootloaderType;
  uint16_t                bootloaderVersion;
  const AppAddressTable_t *appAddressTable;

  // plat/micro/PHY info added in version 0x0104
  uint8_t platInfo;                            // type of platform, defined in micro.h
  uint8_t microInfo;                           // type of micro, defined in micro.h
  uint8_t phyInfo;                             // type of PHY, defined in micro.h
  uint8_t reserved;                            // reserved for future use

  // Moved to this location after plat/micro/PHY info added in version 0x0104
  void (*eblProcessInit)(EblConfig_t *config,
                         void *dataState,
                         uint8_t *tagBuf,
                         uint16_t tagBufLen,
                         bool returnBetweenBlocks);
  uint8_t (*eblProcess)(const EblDataFunction_t *dataFunctions,
                        EblConfig_t *config,
                        const EblFlashFunction_t *flashFunctions);
  EblDataFunction_t *eblDataFunctions;

  // These routines are app bootloader specific added in version 0x0105
  uint8_t     (*init)(void);
  uint8_t     (*storageReadRaw)(uint32_t address, uint8_t *data, uint16_t len);
  uint8_t     (*storageWriteRaw)(uint32_t address, uint8_t const *data, uint16_t len);
  void        (*deinit)(void);
  const void  *(*storageInfo)(void);
  uint8_t     (*storageEraseRaw)(uint32_t address, uint32_t len);
  bool        (*storageBusy)(void);

  // These variables hold extended version information added in version 0x0109
  uint16_t bootloaderBuild;                    // the build number associated with bootloaderVersion
  uint16_t reserved2;                          // reserved for future use
  uint32_t customerBootloaderVersion;          // hold a customer specific bootloader version
} BootloaderAddressTable_t;

typedef uint16_t BootloaderParserCallback_t;

/***************************************************************************//**
 * Get information about the bootloader on this device.
 *
 * The information returned is fetched from the main bootloader
 * information table.
 *
 * @param[out] info Pointer to the bootloader information struct.
 ******************************************************************************/
void bootloader_getInfo(BootloaderInformation_t *info);

/***************************************************************************//**
 * Initialize components of the bootloader so the app can use the interface.
 * For application bootloaders, this initializes the serial peripheral used for
 * communication with the external serial memory and powers it up if supported.
 *
 * @return Error code. @ref BOOTLOADER_OK on success, else error code in
 *         @ref BOOTLOADER_ERROR_INIT_BASE range.
 ******************************************************************************/
int32_t bootloader_init(void);

/***************************************************************************//**
 * De-initialize components of the bootloader that were previously initialized.
 * For application bootloaders, this powers down the external serial memory if
 * supported and de-initializes the serial peripheral used for communication.
 *
 * @return Error code. @ref BOOTLOADER_OK on success, else error code in
 *         @ref BOOTLOADER_ERROR_INIT_BASE range.
 ******************************************************************************/
int32_t bootloader_deinit(void);

/***************************************************************************//**
 * Reboot into the bootloader to perform an install.
 *
 * If there is a storage component and a slot is marked for bootload, install
 * the image in that slot after verifying it.
 *
 * If a communication component is present, open the communication channel and
 * receive an image to be installed.
 ******************************************************************************/
void bootloader_rebootAndInstall(void);

/***************************************************************************//**
 * Initialize image verification. Only applies to application bootloaders.
 *
 * Initialize verification of an upgrade image stored in a bootloader storage
 * slot.
 *
 * @note This function must be called before calling
 *       @ref bootloader_continueVerifyImage in a loop.
 *
 * @note The context pointer must point to memory allocated by the caller.
 *       The required size of this context may depend on the version
 *       of the bootloader. The required size for the bootloader associated with
 *       this version of the application interface is given by the define
 *       @ref BOOTLOADER_STORAGE_VERIFICATION_CONTEXT_SIZE.
 *
 * @note Instead of calling @ref bootloader_initVerifyImage followed by
 *       @ref bootloader_continueVerifyImage, call
 *       @ref bootloader_verifyImage if no
 *       time-critical tasks are needed and sufficient stack space is
 *       available for the automatically allocated context. The purpose of the
 *       init-and-continue functions is to allow the caller to service system
 *       needs during verification.
 *
 * @param[in] slotId      ID of the slot to check.
 * @param     context     Pointer to memory used to hold the parser context.
 * @param[in] contextSize Size of the context. An error is returned if the
 *                        supplied context is too small.
 *
 * @return @ref BOOTLOADER_OK if the image parser was initialized, else error
 *         code.
 ******************************************************************************/
#if SL_EMBER_BOOTLOADER_TYPE == SL_EMBER_BOOTLOADER_TYPE_APPLICATION
int32_t bootloader_initVerifyImage(uint32_t slotId,
                                   void     *context,
                                   size_t   contextSize);
#endif

/***************************************************************************//**
 * Continue image verification. Only applies to application bootloaders.
 *
 * Continue verification of an upgrade image stored in a bootloader storage
 * slot.
 *
 * @note This function must be called in a loop until anything other than
 *       @ref BOOTLOADER_ERROR_PARSE_CONTINUE is returned.
 *
 * @note @ref bootloader_initVerifyImage must be called before calling this
 *       function to reset the parser.
 *
 * @note Instead of calling @ref bootloader_initVerifyImage followed by
 *       @ref bootloader_continueVerifyImage, call
 *       @ref bootloader_verifyImage if no
 *       time-critical tasks are needed. The purpose of the
 *       init-and-continue functions is to allow the caller to service system
 *       needs during verification.
 *
 * @param     context          Pointer to a context structure that has
 *                             been initialized by calling
 *                             @ref bootloader_initVerifyImage()
 * @param[in] metadataCallback Function pointer which gets called when
 *                             the binary metadata in the image is currently
 *                             verified. Set to NULL if not required.
 *
 * @return @ref BOOTLOADER_ERROR_PARSE_CONTINUE if parsing was successful, and
 *         the parser expects more data. @ref BOOTLOADER_ERROR_PARSE_SUCCESS if
 *         the parser has successfully parsed the image and it passes
 *         verification. Else error code.
 ******************************************************************************/
#if SL_EMBER_BOOTLOADER_TYPE == SL_EMBER_BOOTLOADER_TYPE_APPLICATION
int32_t bootloader_continueVerifyImage(void *context, BootloaderParserCallback_t metadataCallback);
#endif

/***************************************************************************//**
 * Verify that the image in the given storage slot is valid. Only applies to
 * application bootloaders.
 *
 * @param[in] slotId ID of the slot to check
 * @param[in] metadataCallback Function pointer which gets called when
 *                             binary metadata is present in the storage slot.
 *                             Set to NULL if not required.
 *
 * @note This function allocates a context structure of the size given by
 *       @ref BOOTLOADER_STORAGE_VERIFICATION_CONTEXT_SIZE on the caller's
 *       stack. To manage memory and allocate the
 *       context elsewhere (on the heap, as a global variable, and so on), use
 *       @ref bootloader_initVerifyImage and @ref bootloader_continueVerifyImage
 *       functions instead.
 *
 * @return @ref BOOTLOADER_OK if the image is valid, else error code.
 ******************************************************************************/
#if SL_EMBER_BOOTLOADER_TYPE == SL_EMBER_BOOTLOADER_TYPE_APPLICATION
int32_t bootloader_verifyImage(uint32_t slotId, BootloaderParserCallback_t metadataCallback);
#endif

/***************************************************************************//**
 * Check whether the bootloader storage is busy. Only applies to application
 * bootloaders.
 *
 * @return True if the storage is busy
 ******************************************************************************/
#if SL_EMBER_BOOTLOADER_TYPE == SL_EMBER_BOOTLOADER_TYPE_APPLICATION
bool bootloader_storageIsBusy(void);
#endif

/***************************************************************************//**
 * Read raw data from storage. Only applies to application bootloaders.
 *
 * @param[in]  address Address to start reading from
 * @param[out] buffer  Buffer to store the data
 * @param[in]  length  Amount of data to read
 *
 * @return @ref BOOTLOADER_OK on success, else error code in
 *         @ref BOOTLOADER_ERROR_STORAGE_BASE range
 ******************************************************************************/
#if SL_EMBER_BOOTLOADER_TYPE == SL_EMBER_BOOTLOADER_TYPE_APPLICATION
int32_t bootloader_readRawStorage(uint32_t address, uint8_t *buffer, size_t length);
#endif

/***************************************************************************//**
 * Write data to storage. Only applies to application bootloaders.
 *
 * @param[in] address Address to start writing to
 * @param[in] buffer  Buffer to read data to write from
 * @param[in] length  Amount of data to write
 *
 * @return @ref BOOTLOADER_OK on success, else error code in
 *         @ref BOOTLOADER_ERROR_STORAGE_BASE range
 ******************************************************************************/
#if SL_EMBER_BOOTLOADER_TYPE == SL_EMBER_BOOTLOADER_TYPE_APPLICATION
int32_t bootloader_writeRawStorage(uint32_t address, uint8_t *buffer, size_t length);
#endif

/***************************************************************************//**
 * Erase data from storage. Only applies to application bootloaders.
 *
 * @note Erasing storage must adhere to the limitations of the underlying
 *       storage medium, such as requiring full page erases. Use
 *       @ref bootloader_getStorageInfo to learn the limitations of the
 *       configured storage medium.
 *
 * @param[in] address Address to start erasing from
 * @param[in] length  Length of data to erase
 *
 * @return @ref BOOTLOADER_OK on success, else error code in
 *         @ref BOOTLOADER_ERROR_STORAGE_BASE range
 ******************************************************************************/
#if SL_EMBER_BOOTLOADER_TYPE == SL_EMBER_BOOTLOADER_TYPE_APPLICATION
int32_t bootloader_eraseRawStorage(uint32_t address, size_t length);
#endif

#endif // EMBER_BTL_INTERFACE_H
