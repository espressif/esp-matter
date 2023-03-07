/***************************************************************************//**
 * @file
 * @brief API for backing up or restoring TC data from unix filesystem.
 * Files are imported/exported in plain text with the format:
 *   # Comments
 *   Extended Pan ID: <extended-pan-id-big-endian>
 *   Key: <eui64-big-endian>  <key-data>
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "app/framework/include/af.h"
#include "app/framework/util/common.h"
#include "app/framework/util/util.h"
#include "app/util/serial/sl_zigbee_command_interpreter.h"
#include "app/framework/plugin/trust-center-backup/trust-center-backup.h"

#include "app/framework/util/af-main.h"

#include <errno.h>
#include <strings.h>

// For some reason gcc/armgcc 10 does not declare strnlen in string(s).h
// Hence, declare it as extern here.
extern size_t strnlen (const char *, size_t);

#ifdef UC_BUILD
#include "trust-center-backup-config.h"
#if (EMBER_AF_PLUGIN_TRUST_CENTER_BACKUP_POSIX_FILE_BACKUP_SUPPORT == 1)
  #define POSIX_FILE_BACKUP_SUPPORT
#endif // EMBER_AF_PLUGIN_TRUST_CENTER_BACKUP_POSIX_FILE_BACKUP_SUPPORT
#endif // UC_BUILD

#if defined(EMBER_TEST)
  #define POSIX_FILE_BACKUP_SUPPORT
#endif

#if defined(POSIX_FILE_BACKUP_SUPPORT)

#ifdef EMBER_TEST
// Token header defines these, but for the simulation test build the token
// definitions are not pulled in.
#ifndef NVM3KEY_STACK_RESTORED_EUI64
#define NVM3KEY_STACK_RESTORED_EUI64 0x1E12A
#endif  // NVM3KEY_STACK_RESTORED_EUI64
#endif // EMBER_TEST

// *****************************************************************************
// Globals

#define MAX_LINE_LENGTH 255

#define MAX_BACKUP_LIST_LENGTH 20  // arbitrary.  Ideally we should use malloc()

// Windows requires the 'b' (binary) as part of the mode so that line endings
// are not truncated.  POSIX ignores this.
#define READ_FLAGS "rb"
#define WRITE_FLAGS "wb"

static const char extendedPanIdKeyWord[] = "Extended PAN ID";
static const char keyKeyWord[] = "Key";

// *****************************************************************************
// Forward Declarations

static bool writeHexData(FILE* output,
                         const uint8_t* data,
                         uint8_t length,
                         bool reverse);
static uint8_t* readHexDataIntoArray(uint8_t* result,
                                     uint8_t hexDataLength,
                                     uint8_t* line,
                                     uint8_t lineNumber);
static uint8_t* skipSpacesInLine(uint8_t* line);

// *****************************************************************************
// Functions

EmberStatus emberAfTrustCenterExportBackupToFile(const char* filepath)
{
  EmberStatus returnValue = EMBER_ERR_FATAL;
  if (filepath == NULL) {
    return returnValue;
  }

  emberAfSecurityPrintln("Opening file '%p'",
                         filepath);

  FILE* output = fopen(filepath, WRITE_FLAGS);
  if (NULL == output) {
    emberAfSecurityPrintln("Failed to open file: %p",
                           strerror(errno));
    return returnValue;
  }

  EmberAfLinkKeyBackupData exportKeyList[MAX_BACKUP_LIST_LENGTH];
  EmberAfTrustCenterBackupData export;
  EmberStatus status;

  export.maxKeyListLength = MAX_BACKUP_LIST_LENGTH;
  export.keyList = exportKeyList;
  status = emberTrustCenterExportBackupData(&export);

  if (status != EMBER_SUCCESS) {
    emberAfSecurityPrintln("%p: Failed to get TC backup data.", "Error");
    goto exportEnd;
  }

  EmberEUI64 myEui64;
  emberAfGetEui64(myEui64);

  fprintf(output,
          "# TC Backup File from: (>)%02X%02X%02X%02X%02X%02X%02X%02X\n",
          myEui64[7],
          myEui64[6],
          myEui64[5],
          myEui64[4],
          myEui64[3],
          myEui64[2],
          myEui64[1],
          myEui64[0]);
  uint8_t mfgString[MFG_STRING_MAX_LENGTH + 1];
  emberAfFormatMfgString(mfgString);
  fprintf(output,
          "# MFG String: %s\n\n",
          mfgString);

  fprintf(output, "# Format\n");
  fprintf(output, "# ------\n");
  fprintf(output, "# Extended Pan ID: <extended-pan-id-big-endian>\n");
  fprintf(output, "# Key: <EUI64-big-endian>  <Key Data>\n");
  fprintf(output, "# Key: ...\n");
  fprintf(output, "# Key: ...\n\n");

  fprintf(output,
          "Extended PAN ID: %02X%02X%02X%02X%02X%02X%02X%02X\n\n",
          export.extendedPanId[7],
          export.extendedPanId[6],
          export.extendedPanId[5],
          export.extendedPanId[4],
          export.extendedPanId[3],
          export.extendedPanId[2],
          export.extendedPanId[1],
          export.extendedPanId[0]);

  uint8_t i = 0;
  while (i < export.keyListLength) {
    fprintf(output, "Key: ");
    writeHexData(output,
                 export.keyList[i].deviceId,
                 EUI64_SIZE,
                 true);      // reverse order?
    fprintf(output, "  ");
    writeHexData(output,
                 emberKeyContents(&(export.keyList[i].key)),
                 EMBER_ENCRYPTION_KEY_SIZE,
                 false);     // reverse order?
    fprintf(output, "\n");
    fflush(output);
    i++;
  }

  emberAfSecurityPrintln("Wrote %d entries to file.", export.keyListLength);
  returnValue = EMBER_SUCCESS;

  exportEnd:
  fclose(output);
  return returnValue;
}

// The binary file format to save the tokens are
//
// Number of Tokens (1 byte)
// Token0 (4 bytes) Token0Size(1 byte) Token0ArraySize(1 byte) Token0Data(Token0Size * Token0ArraySize)
// :
// :
// TokenM (4 bytes) TokenMSize(1 byte) TokenMArraySize(1 byte) TokenMData(TokenMSize * TokenMArraySize)
//

EmberStatus emberAfTrustCenterBackupSaveTokensToFile(const char* filepath)
{
  EmberEUI64 blankEui = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  EmberEUI64 eui;
  emberAfGetEui64(eui);
  EmberStatus returnValue = EMBER_ERR_FATAL;
  printf("Opening file '%s'\n", filepath);

  FILE* output = fopen(filepath, WRITE_FLAGS);
  if (NULL == output) {
    printf("Failed to open file: %s", strerror(errno));
    return returnValue;
  }
  // ------- Token Data Saving to the provided File -----------
  uint8_t numberOfTokens = emberGetTokenCount();
  if (numberOfTokens) {
    fwrite(&numberOfTokens, 1, 1, output);
    for (uint8_t tokenIndex = 0; tokenIndex < numberOfTokens; tokenIndex++) {
      EmberTokenInfo tokenInfo;
      EmberStatus status = emberGetTokenInfo(tokenIndex, &tokenInfo);
      if (status == EMBER_SUCCESS) {
        fwrite(&(tokenInfo.nvm3Key), 4, 1, output);   // 4 Bytes Token Key
        fwrite(&(tokenInfo.size), 1, 1, output);      // 1 byte Token size
        fwrite(&(tokenInfo.arraySize), 1, 1, output); // 1 byte array size
        for (uint8_t arrayIndex = 0; arrayIndex < tokenInfo.arraySize; arrayIndex++) {
          uint8_t tokenDataDump[255];
          EmberTokenData tokenData;
          tokenData.size = 0;
          tokenData.data = (void *)tokenDataDump;
          EmberStatus getStatus = emberGetTokenData(tokenInfo.nvm3Key,
                                                    arrayIndex,
                                                    &tokenData);
          if (getStatus == EMBER_SUCCESS) {
            // Check the Key to see if the token to save is restoredEui64, in that case
            // check if it is blank, then save the node EUI64 in its place, else save the value
            // received from the API. Once it saves, during restore process the set token will
            // simply write the restoredEUI64 and the node will start to use that.
            if (tokenInfo.nvm3Key == NVM3KEY_STACK_RESTORED_EUI64  //0x1E12A
                && tokenData.size == sizeof(EmberEUI64)
                && (!MEMCOMPARE((uint8_t*)(tokenData.data), (uint8_t*)blankEui, sizeof(EmberEUI64)))
                ) {
              // Special case : Save the node EUI64 on the restoredEui64 token while saving.
              memcpy((uint8_t*)(tokenData.data), (uint8_t*)eui, sizeof(EmberEUI64));
            }
            fwrite((uint8_t*)(tokenData.data), (uint8_t)tokenData.size, 1, output);
          }
        }
      }
    }
  }
  fclose(output);
  return EMBER_SUCCESS;
}
#if !defined(EMBER_TEST)
static const uint16_t creators[] = {
  CREATOR_STACK_NVDATA_VERSION,
  CREATOR_STACK_BOOT_COUNTER,
  CREATOR_STACK_NONCE_COUNTER,
  CREATOR_STACK_ANALYSIS_REBOOT,
  CREATOR_STACK_KEYS,
  CREATOR_STACK_NODE_DATA,
  CREATOR_STACK_CLASSIC_DATA,
  CREATOR_STACK_ALTERNATE_KEY,
  CREATOR_STACK_APS_FRAME_COUNTER,
  CREATOR_STACK_TRUST_CENTER,
  CREATOR_STACK_NETWORK_MANAGEMENT,
  CREATOR_STACK_PARENT_INFO,
  CREATOR_STACK_PARENT_ADDITIONAL_INFO,
  CREATOR_STACK_MULTI_PHY_NWK_INFO,
  CREATOR_STACK_MIN_RECEIVED_RSSI,
  CREATOR_STACK_RESTORED_EUI64,
  CREATOR_MULTI_NETWORK_STACK_KEYS,
  CREATOR_MULTI_NETWORK_STACK_NODE_DATA,
  CREATOR_MULTI_NETWORK_STACK_ALTERNATE_KEY,
  CREATOR_MULTI_NETWORK_STACK_TRUST_CENTER,
  CREATOR_MULTI_NETWORK_STACK_NETWORK_MANAGEMENT,
  CREATOR_MULTI_NETWORK_STACK_PARENT_INFO,
  CREATOR_MULTI_NETWORK_STACK_NONCE_COUNTER,
  CREATOR_MULTI_NETWORK_STACK_PARENT_ADDITIONAL_INFO,
  CREATOR_STACK_GP_DATA,
  CREATOR_STACK_GP_PROXY_TABLE,
  CREATOR_STACK_GP_SINK_TABLE,
  CREATOR_STACK_GP_INCOMING_FC,
  CREATOR_STACK_GP_INCOMING_FC_IN_SINK,
  CREATOR_STACK_BINDING_TABLE,
  CREATOR_STACK_CHILD_TABLE,
  CREATOR_STACK_KEY_TABLE,
  CREATOR_STACK_CERTIFICATE_TABLE,
  CREATOR_STACK_ZLL_DATA,
  CREATOR_STACK_ZLL_SECURITY,
  CREATOR_STACK_ADDITIONAL_CHILD_DATA,
};

static const uint32_t nvm3Keys[] = {
  NVM3KEY_STACK_NVDATA_VERSION,
  NVM3KEY_STACK_BOOT_COUNTER,
  NVM3KEY_STACK_NONCE_COUNTER,
  NVM3KEY_STACK_ANALYSIS_REBOOT,
  NVM3KEY_STACK_KEYS,
  NVM3KEY_STACK_NODE_DATA,
  NVM3KEY_STACK_CLASSIC_DATA,
  NVM3KEY_STACK_ALTERNATE_KEY,
  NVM3KEY_STACK_APS_FRAME_COUNTER,
  NVM3KEY_STACK_TRUST_CENTER,
  NVM3KEY_STACK_NETWORK_MANAGEMENT,
  NVM3KEY_STACK_PARENT_INFO,
  NVM3KEY_STACK_PARENT_ADDITIONAL_INFO,
  NVM3KEY_STACK_MULTI_PHY_NWK_INFO,
  NVM3KEY_STACK_MIN_RECEIVED_RSSI,
  NVM3KEY_STACK_RESTORED_EUI64,
  NVM3KEY_MULTI_NETWORK_STACK_KEYS,
  NVM3KEY_MULTI_NETWORK_STACK_NODE_DATA,
  NVM3KEY_MULTI_NETWORK_STACK_ALTERNATE_KEY,
  NVM3KEY_MULTI_NETWORK_STACK_TRUST_CENTER,
  NVM3KEY_MULTI_NETWORK_STACK_NETWORK_MANAGEMENT,
  NVM3KEY_MULTI_NETWORK_STACK_PARENT_INFO,
  NVM3KEY_MULTI_NETWORK_STACK_NONCE_COUNTER,
  NVM3KEY_MULTI_NETWORK_STACK_PARENT_ADDITIONAL_INFO,
  NVM3KEY_STACK_GP_DATA,
  NVM3KEY_STACK_GP_PROXY_TABLE,
  NVM3KEY_STACK_GP_SINK_TABLE,
  NVM3KEY_STACK_GP_INCOMING_FC,
  NVM3KEY_STACK_BINDING_TABLE,
  NVM3KEY_STACK_CHILD_TABLE,
  NVM3KEY_STACK_KEY_TABLE,
  NVM3KEY_STACK_CERTIFICATE_TABLE,
  NVM3KEY_STACK_ZLL_DATA,
  NVM3KEY_STACK_ZLL_SECURITY,
  NVM3KEY_STACK_ADDITIONAL_CHILD_DATA,
  NVM3KEY_STACK_GP_INCOMING_FC_IN_SINK,
};

static uint16_t getCreatorFromNvm3Key(uint32_t nvm3Key)
{
  for (int i = 0; i < (sizeof(nvm3Keys) / sizeof(uint32_t)); i++) {
    if (nvm3Keys[i] == nvm3Key) {
      return creators[i];
    }
  }
  return 0xFFFF;
}
#else
static uint16_t getCreatorFromNvm3Key(uint32_t nvm3Key)
{
  return 0xFFFF;
}
#endif
// The following updates the ZIgbeed tokens from a NCP token set.
EmberStatus emberAfTrustCenterBackupWriteNcpTokenToZigbeedTokens(const char* filepath)
{
  EmberStatus returnValue = EMBER_ERR_FATAL;
  if (filepath == NULL) {
    return returnValue;
  }
  printf("Opening file '%s'", filepath);

  FILE* input = fopen(filepath, READ_FLAGS);
  if (input == NULL) {
    printf("Failed to open file: %p", strerror(errno));
    return returnValue;
  }
  // -------------- Read Token Data from File and add set it to the token.
  uint8_t numberOfTokens = 0;
  fread(&numberOfTokens, 1, 1, input);
  //printf("numberOfTokens = %d\n", numberOfTokens);
  for (uint8_t i = 0; i < numberOfTokens; i++) {
    uint32_t token = 0;
    uint8_t size = 0;
    uint8_t arraySize = 0;
    fread(&token, 4, 1, input);     // 4 bytes Token Key/Creator
    fread(&size, 1, 1, input);      // 1 byte token size
    fread(&arraySize, 1, 1, input); // 1 byte array size.
    //printf("Restoring Token=%08X Size=%d ArraySize=%d [", token, size, arraySize);
    for (uint8_t arrayIndex = 0; arrayIndex < arraySize; arrayIndex++) {
      uint8_t tokenDataDump[255];
      memset(tokenDataDump, 0, 255);
      fread(tokenDataDump, size, 1, input);
      //for (int j = 0; j < size; j++) {
      //  printf("%02X ",tokenDataDump[j]);
      //}
      //printf("]\n");
      EmberTokenData tokenData;
      tokenData.size = size;
      tokenData.data = (void*)tokenDataDump;
      uint16_t creator = getCreatorFromNvm3Key(token);
      EmberStatus setStatus = emberSetTokenData(creator,
                                                arrayIndex,
                                                &tokenData);
      //printf("[%d]-----Status = %d\n", arrayIndex, setStatus);
      (void)setStatus;
    }
  }
  fclose(input);
  return EMBER_SUCCESS;
}

EmberStatus emberAfTrustCenterBackupRestoreTokensFromFile(const char* filepath)
{
  EmberStatus returnValue = EMBER_ERR_FATAL;
  if (filepath == NULL) {
    return returnValue;
  }
  printf("Opening file '%s'", filepath);

  FILE* input = fopen(filepath, READ_FLAGS);
  if (input == NULL) {
    printf("Failed to open file: %p", strerror(errno));
    return returnValue;
  }
  // -------------- Read Token Data from File and add set it to the token.
  uint8_t numberOfTokens = 0;
  fread(&numberOfTokens, 1, 1, input);
  //printf("numberOfTokens = %d\n", numberOfTokens);
  for (uint8_t i = 0; i < numberOfTokens; i++) {
    uint32_t token = 0;
    uint8_t size = 0;
    uint8_t arraySize = 0;
    fread(&token, 4, 1, input);     // 4 bytes Token Key/Creator
    fread(&size, 1, 1, input);      // 1 byte token size
    fread(&arraySize, 1, 1, input); // 1 byte array size.
    //printf("Restoring Token=%08X Size=%d ArraySize=%d [", token, size, arraySize);
    for (uint8_t arrayIndex = 0; arrayIndex < arraySize; arrayIndex++) {
      uint8_t tokenDataDump[255];
      memset(tokenDataDump, 0, 255);
      fread(tokenDataDump, size, 1, input);
      //for (int j = 0; j < size; j++) {
      //  printf("%02X ",tokenDataDump[j]);
      //}
      //printf("]\n");
      EmberTokenData tokenData;
      tokenData.size = size;
      tokenData.data = (void*)tokenDataDump;
      EmberStatus setStatus = emberSetTokenData(token,
                                                arrayIndex,
                                                &tokenData);
      //printf("[%d]-----Status = %d\n", arrayIndex, setStatus);
      (void)setStatus;
    }
  }
  fclose(input);
  return EMBER_SUCCESS;
}

static bool writeHexData(FILE* output,
                         const uint8_t* data,
                         uint8_t length,
                         bool reverseOrder)
{
  uint8_t i;
  for (i = 0; i < length; i++) {
    uint8_t j = (reverseOrder
                 ? (length - i - 1)
                 : i);
    if (0 > fprintf(output, "%02X", data[j])) {
      return false;
    }
  }
  fflush(output);
  return true;
}

// I miss Perl.  Something to make me feel at home.  Remove trailing '\n' if
// present.  Depending on format that could be CR, LF, or both.
static int chomp(char* line)
{
  int length = strnlen(line, MAX_LINE_LENGTH);
  int newLength = length;
  uint8_t i;
  for (i = 1; i <= 2; i++) {
    char lastChar = line[length - i];
    if (lastChar == (char)0x0A        // LF
        || lastChar == (char)0x0D) {  // CR
      line[length - i] = '\0';
      newLength--;
    }
  }
  return newLength;
}

EmberStatus emberAfTrustCenterImportBackupFromFile(const char* filepath)
{
  EmberStatus returnValue = EMBER_ERR_FATAL;
  if (filepath == NULL) {
    return returnValue;
  }

  emberAfSecurityPrintln("Opening file '%p'",
                         filepath);

  FILE* input = fopen(filepath, READ_FLAGS);
  if (input == NULL) {
    emberAfSecurityPrintln("Failed to open file: %p",
                           strerror(errno));
    return returnValue;
  }

  EmberAfTrustCenterBackupData import;
  MEMSET(&import, 0, sizeof(EmberAfTrustCenterBackupData));

  EmberAfLinkKeyBackupData importKeyList[MAX_BACKUP_LIST_LENGTH];
  import.maxKeyListLength = MAX_BACKUP_LIST_LENGTH;
  import.keyList = importKeyList;

  uint8_t keyListIndex = 0;
  int lineNumber = 0;

  uint8_t line[MAX_LINE_LENGTH];

  while (NULL != fgets((char*)line, MAX_LINE_LENGTH, input)) {
    const char* keyWordPtr;
    uint8_t* linePtr = line;
    lineNumber++;
    chomp((char *)linePtr);
    linePtr = skipSpacesInLine(linePtr);
    if (strlen((const char *)line) == 0) {
      continue;
    }
    if (*linePtr == '#') { // comment line
      continue;
    }
    uint8_t* endToken;
    endToken = (uint8_t *)strchr((const char *)linePtr, (int)':');
    if (endToken == NULL) {
      emberAfSecurityPrintln("Error: Invalid format on line %d, must be <token>: <value>\n",
                             lineNumber);
      goto importEnd;
    }
    // truncate the line so that we can compare this token to known values.
    *endToken = '\0';
    if (0 == strncasecmp((const char *)linePtr,
                         extendedPanIdKeyWord,
                         strlen(extendedPanIdKeyWord) + 1)) { // +1 for '\0'
      if (0 != MEMCOMPARE(import.extendedPanId,
                          emberAfNullEui64,
                          EUI64_SIZE)) {
        emberAfSecurityPrintln("Error: Duplicate Extended PAN ID token on line %d.\n",
                               lineNumber);
        goto importEnd;
      }
      keyWordPtr = extendedPanIdKeyWord;
    } else if (0 == strncasecmp((const char *)linePtr,
                                keyKeyWord,
                                strlen(keyKeyWord) + 1)) { // +1 for '\0'
      keyWordPtr = keyKeyWord;
    } else {
      emberAfSecurityPrintln("Error: Unknown token '%p' on line %d\n",
                             linePtr,
                             lineNumber);
      goto importEnd;
    }

    uint8_t temp[EUI64_SIZE];
    uint8_t* data = ((keyWordPtr == extendedPanIdKeyWord)
                     ? import.extendedPanId
                     : import.keyList[keyListIndex].deviceId);

    linePtr = skipSpacesInLine(endToken + 1);
    linePtr = readHexDataIntoArray(temp,
                                   EUI64_SIZE,
                                   linePtr,
                                   lineNumber);
    if (NULL == linePtr) {
      emberAfSecurityPrintln("Error: Failed to parse %p on line %d.",
                             (keyWordPtr == extendedPanIdKeyWord
                              ? "Extended PAN ID"
                              : "key data"),
                             lineNumber);
      goto importEnd;
    }

    // Device EUI64 and PAN ID are written out in big-endian.  reverse
    // them so they are little-endian in the struct.
    emberReverseMemCopy(data, temp, EUI64_SIZE);

    if (keyWordPtr == extendedPanIdKeyWord) {
      emberAfSecurityPrintln("Imported extended PAN ID: (>)%02X%02X%02X%02X%02X%02X%02X%02X\n",
                             import.extendedPanId[7],
                             import.extendedPanId[6],
                             import.extendedPanId[5],
                             import.extendedPanId[4],
                             import.extendedPanId[3],
                             import.extendedPanId[2],
                             import.extendedPanId[1],
                             import.extendedPanId[0]);
      continue;
    }

    linePtr = skipSpacesInLine(linePtr);
    if (*linePtr == '\0') {
      emberAfSecurityPrintln("Missing key-data on line %d", lineNumber);
      goto importEnd;
    }

    if (NULL == readHexDataIntoArray(emberKeyContents(&(import.keyList[keyListIndex].key)),
                                     EMBER_ENCRYPTION_KEY_SIZE,
                                     linePtr,
                                     lineNumber)) {
      emberAfSecurityPrintln("Error: Failed to parse key-data on line %d.", lineNumber);
      goto importEnd;
    }
    emberAfSecurityPrint("Imported data for EUI64 ");
    emberAfPrintBigEndianEui64(import.keyList[keyListIndex].deviceId);
    emberAfSecurityPrint(", key ");
    emberAfPrintZigbeeKey(emberKeyContents(&(import.keyList[keyListIndex].key)));
    emberAfSecurityPrintln("");
    keyListIndex++;
  }
  import.keyListLength = keyListIndex;

  EmberStatus status = emberTrustCenterImportBackupAndStartNetwork(&import);
  if (status != EMBER_SUCCESS) {
    emberAfSecurityPrintln("%p: Failed to import backup data and form network.",
                           "Error");
  } else {
    emberAfSecurityPrintln("Import from file successful.");
    returnValue = EMBER_SUCCESS;
  }

  importEnd:
  fclose(input);
  return returnValue;
}

static uint8_t* skipSpacesInLine(uint8_t* line)
{
  while (*line == ' '
         || *line == '\t') {
    line++;
  }
  return line;
}

static uint8_t* readHexDataIntoArray(uint8_t* result,
                                     uint8_t hexDataLength,
                                     uint8_t* line,
                                     uint8_t lineNumber)
{
  uint8_t* ptr = result;
  uint8_t i;
  for (i = 0; i < hexDataLength * 2; i++) {
    uint8_t nibble;
    char data[2];
    int temp;

    if (line[i] == '\0') {
      emberAfSecurityPrintln("Error: hex data too short on line %d.",
                             lineNumber);
      return NULL;
    }

    data[0] = line[i];
    data[1] = '\0';

    if (1 != sscanf(data, "%x", &temp)) {
      emberAfSecurityPrintln("Error: Invalid character found on line %d.",
                             lineNumber);
      return NULL;
    }

    nibble = (uint8_t)temp;

    if (i % 2 == 0) {
      *ptr = (nibble << 4);
    } else {
      *ptr |= nibble;
      ptr++;
    }
  }
  return &(line[i]);
}

#endif // POSIX_FILE_BACKUP_SUPPORT
