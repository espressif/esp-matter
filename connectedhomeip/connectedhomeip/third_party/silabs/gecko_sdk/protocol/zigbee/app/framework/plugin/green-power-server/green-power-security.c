/***************************************************************************//**
 * @file
 * @brief GP security code
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

#include "stack/include/ccm-star.h"

extern void emLoadKeyIntoCore(const uint8_t* key);

#define EMBER_GP_SECURITY_MIC_LENGTH 4
#define SECURITY_BLOCK_SIZE 16

// A nonce contains the following fields:
#define STANDALONE_NONCE_SOURCE_ADDR_INDEX       0
#define STANDALONE_NONCE_FRAME_COUNTER_INDEX     8
#define STANDALONE_NONCE_SECURITY_CONTROL_INDEX 12

#ifndef MAX_PAYLOAD_LENGTH
#define MAX_PAYLOAD_LENGTH   70
#endif

// NWK FC := [Ext NWK Header = 0b1 || Auto-Commissioning =0b0|| ZigBee Protocol 0b0011 || Frame type =0b00] = 0x8c
#define HEADER_FLAG_NWK_FC 0x8C

//NWK FC Extended = [Direction = 0b0 || RxAfterTx = 0b0 || SecurityKey = 0b0 ||SecurityLevel(2 bits) || ApplID(3 bits)]
#define MAKE_HEADER_FLAG_NWK_FC_EXT(direction, rxAfterTx, keyType, securityLevel, appId) ((direction << 7) + (rxAfterTx << 6) + (keyType << 5) + (securityLevel << 3) + appId)

#define GPD_APPLICATION_ID_SRC_ID 0
#define GPD_APPLICATION_ID_IEEE   2
#define NONCE_SECURITY_CONTROL 0x05
#define NONCE_SECURITY_CONTROL_APP_ID2_OUTGOING 0xC5

// The direction
//   gpd->gpp : Incoming
//   gpp->gpd : Outgoing
typedef enum {
  DIRECTION_GPD_TO_GPP = 0x00,
  DIRECTION_GPP_TO_GPD = 0x01
} EmberGpdfDirection;

#ifndef EZSP_HOST
extern void emberHmacAesHash(const uint8_t *key,
                             const uint8_t *data,
                             uint8_t dataLength,
                             uint8_t *result);
#else
#define emberHmacAesHash(key,        \
                         data,       \
                         dataLength, \
                         result)
#endif

EmberAfStatus emberAfGreenPowerServerDeriveSharedKeyFromSinkAttribute(uint8_t * gpsSecurityKeyTypeAtrribute,
                                                                      EmberKeyData * gpSharedKeyAttribute,
                                                                      EmberGpAddress * gpdAddr);

static void initializeNonce(bool directionIncoming,
                            uint8_t * nonce,
                            EmberGpAddress * gpdAddr,
                            uint32_t gpdSecurityFrameCounter)
{
  // ScrId
  if (gpdAddr->applicationId == GPD_APPLICATION_ID_IEEE) {
    MEMMOVE(nonce,
            gpdAddr->id.gpdIeeeAddress,
            EUI64_SIZE);
  } else {
    if (directionIncoming) { // from GPD --> GPP/GPS : direction bit = 0 in NW FC Header
      emberStoreLowHighInt32u(&nonce[STANDALONE_NONCE_SOURCE_ADDR_INDEX], gpdAddr->id.sourceId);
      emberStoreLowHighInt32u(&nonce[STANDALONE_NONCE_SOURCE_ADDR_INDEX + 4], gpdAddr->id.sourceId);
    } else { // from GPP/GPS --> GPD : direction bit = 1 in NW FC Header
      emberStoreLowHighInt32u(&nonce[STANDALONE_NONCE_SOURCE_ADDR_INDEX], 0);
      emberStoreLowHighInt32u(&nonce[STANDALONE_NONCE_SOURCE_ADDR_INDEX + 4], gpdAddr->id.sourceId);
    }
  }
  // Frame counter.
  emberStoreLowHighInt32u(&nonce[STANDALONE_NONCE_FRAME_COUNTER_INDEX], gpdSecurityFrameCounter);
  //Security control
  if (gpdAddr->applicationId == GPD_APPLICATION_ID_SRC_ID
      || directionIncoming) {
    nonce[STANDALONE_NONCE_SECURITY_CONTROL_INDEX] = NONCE_SECURITY_CONTROL;
  } else if (!directionIncoming
             && gpdAddr->applicationId == GPD_APPLICATION_ID_IEEE) {
    nonce[STANDALONE_NONCE_SECURITY_CONTROL_INDEX] = NONCE_SECURITY_CONTROL_APP_ID2_OUTGOING;
  } else {
    // MISRA : terminating else , assign 0 to the member.
    nonce[STANDALONE_NONCE_SECURITY_CONTROL_INDEX] = 0;
  }
}

static uint8_t prepareHeader(EmberGpdfDirection direction,
                             bool rxAfterTx,
                             uint8_t * header,
                             EmberGpAddress * gpdAddr,
                             uint8_t keyType,
                             uint8_t securityLevel,
                             uint32_t gpdSecurityFrameCounter)
{
  uint8_t headerLength = 0;
  header[headerLength++] = HEADER_FLAG_NWK_FC;
  header[headerLength++] = MAKE_HEADER_FLAG_NWK_FC_EXT(direction, rxAfterTx, keyType, securityLevel, gpdAddr->applicationId);
  if (gpdAddr->applicationId == GPD_APPLICATION_ID_IEEE) {
    header[headerLength++] = gpdAddr->endpoint;
  } else {
    emberStoreLowHighInt32u(&header[headerLength], gpdAddr->id.sourceId);
    headerLength += 4;
  }
  if (securityLevel >= EMBER_GP_SECURITY_LEVEL_FC_MIC) {
    emberStoreLowHighInt32u(&header[headerLength], gpdSecurityFrameCounter);
    headerLength += 4;
  }
  return headerLength;
}

static uint8_t appendPayload(uint8_t * dst,
                             uint8_t gpdCommandId,
                             uint8_t * gpdCommandPayload)
{
  if (NULL == dst) {
    return 0;
  }
  uint8_t length = 0;
  dst[length++] = gpdCommandId;
  if (NULL == gpdCommandPayload) {
    return length;
  }
  if (gpdCommandPayload[0] == 0xFF) {
    dst[length++] = 0xFF;
  } else if (gpdCommandPayload[0] < MAX_PAYLOAD_LENGTH) {
    for (int i = 0; i < gpdCommandPayload[0]; i++) {
      dst[length++] = gpdCommandPayload[1 + i];
    }
  } else {
    return 0;
  }
  return length;
}
// Secure outgoing GPDF frame creation
// Inputs :
//   gpdAddr                      : Gpd address
//   gpdSecurityFrameCounter      : Security framecounter
//   keyType                      : Key Type [Shared Key = 0, Individual Key = 1]
//   key                          : Key
//   securityLevel                : The security Level must be one valid level as 0, 2 or 3.
//   gpdCommandId                 : GPD Command Id
//   gpdCommandPayload            : Payload Format to specefy payload = [payloadlength, <payload bytes>]
//   securedOutgoingGpdf          : Memory pointer to collect the secured GPDF
//   securedOutgoingGpdfMaxLength : Allocated memory size to colllect the result.
// Outputs :
//   securedOutgoingGpdf          : Memory pointer to collect the secured GPDF
// Returns :
//   totalLength                  : Length of the secured GPDF
uint8_t emGpOutgoingCommandEncrypt(EmberGpAddress * gpdAddr,
                                   uint32_t gpdSecurityFrameCounter,
                                   uint8_t keyType,
                                   EmberKeyData * key,
                                   uint8_t securityLevel,
                                   uint8_t gpdCommandId,
                                   uint8_t * gpdCommandPayload,
                                   uint8_t * securedOutgoingGpdf,
                                   uint8_t securedOutgoingGpdfMaxLength)
{
  if (securedOutgoingGpdf == NULL) {
    return 0;
  }
  // GP Spec Ref : A.1.4.1 Generic GPDF Frame Format
  // The secure frame storage need, the supplied storage must be greater than the folowing min length
  // NWK FC | EXT NWK FC | EP/SRC ID | FC | GPD Command Id | Payload    | MIC |
  //   1    |    1       |  1/4      | 0/4|       1        | 0-Variable | 0/4 |
  uint8_t minframeLength = 1
                           + 1
                           + ((gpdAddr->applicationId == GPD_APPLICATION_ID_IEEE) ? 1 : 4)
                           + ((securityLevel == EMBER_GP_SECURITY_LEVEL_NONE) ? 0 : 4)
                           + 1
                           + ((gpdCommandPayload == NULL) ? 0 : ((gpdCommandPayload[0] == 0xFF) ? 1 : gpdCommandPayload[0]))
                           + ((securityLevel == EMBER_GP_SECURITY_LEVEL_NONE) ? 0 : 4);
  if (securedOutgoingGpdfMaxLength < minframeLength) {
    return 0;
  }
  uint8_t headerLength = prepareHeader(DIRECTION_GPP_TO_GPD,
                                       false,
                                       securedOutgoingGpdf,
                                       gpdAddr,
                                       keyType,
                                       securityLevel,
                                       gpdSecurityFrameCounter);
  uint8_t payloadLength = appendPayload(&securedOutgoingGpdf[headerLength],
                                        gpdCommandId,
                                        gpdCommandPayload);
  uint8_t totalLength = headerLength + payloadLength;
  if (securityLevel >= EMBER_GP_SECURITY_LEVEL_FC_MIC) {
    // The following code for the security level 2 or 3
    // Prepare nonce
    uint8_t nonce[SECURITY_BLOCK_SIZE] = { 0 };
    initializeNonce(false,
                    nonce,
                    gpdAddr,
                    gpdSecurityFrameCounter);
    uint8_t authenticationStartIndex = 0;
    uint8_t encryptionStartIndex;
    uint8_t authenticationLength;
    uint8_t encryptionLength;
    if (securityLevel == EMBER_GP_SECURITY_LEVEL_FC_MIC) {
      encryptionStartIndex = totalLength;
      authenticationLength = totalLength;
      encryptionLength = 0;
    } else {
      encryptionStartIndex = headerLength;
      authenticationLength = headerLength;
      encryptionLength = payloadLength;
    }
    emLoadKeyIntoCore(key->contents);
    // Calculate the MIC of unencrypted payload
    emberCcmCalculateAndEncryptMic(nonce,
                                   securedOutgoingGpdf + authenticationStartIndex,
                                   authenticationLength,
                                   securedOutgoingGpdf + encryptionStartIndex,
                                   encryptionLength,
                                   &securedOutgoingGpdf[totalLength]);
    totalLength += 4;
    if (securityLevel == EMBER_GP_SECURITY_LEVEL_FC_MIC_ENCRYPTED) {
      // Encrypt
      emberCcmEncryptBytes(&securedOutgoingGpdf[encryptionStartIndex], payloadLength, nonce);
    }
  }
  return totalLength;
}

bool emGpCalculateIncomingCommandDecrypt(EmberGpAddress * gpdAddr,
                                         uint32_t gpdSecurityFrameCounter,
                                         uint8_t payloadLength,
                                         uint8_t * payload,
                                         EmberKeyData * key)
{
  uint8_t nonce[SECURITY_BLOCK_SIZE] = { 0 };
  initializeNonce(true,
                  nonce,
                  gpdAddr,
                  gpdSecurityFrameCounter);
  emLoadKeyIntoCore(key->contents);
  emberCcmEncryptBytes(payload, payloadLength, nonce);
  return true;
}

bool emGpCalculateIncomingCommandMic(EmberGpAddress * gpdAddr,
                                     bool rxAfterTx,
                                     uint8_t keyType,
                                     uint8_t securityLevel,
                                     uint32_t gpdSecurityFrameCounter,
                                     uint8_t gpdCommandId,
                                     uint8_t * gpdCommandPayload,
                                     bool encryptedPayload,
                                     uint8_t mic[4],
                                     EmberKeyData * key)
{
  uint8_t nonce[SECURITY_BLOCK_SIZE] = { 0 };
  initializeNonce(true,
                  nonce,
                  gpdAddr,
                  gpdSecurityFrameCounter);
  uint8_t payload[MAX_PAYLOAD_LENGTH] = { 0 };
  uint8_t headerLength = prepareHeader(DIRECTION_GPD_TO_GPP,
                                       rxAfterTx,
                                       payload,
                                       gpdAddr,
                                       keyType,
                                       securityLevel,
                                       gpdSecurityFrameCounter);

  uint8_t payloadLength = appendPayload(&payload[headerLength],
                                        gpdCommandId,
                                        gpdCommandPayload);
  uint8_t totalLength = headerLength + payloadLength;
  uint8_t authenticationStartIndex = 0;
  uint8_t encryptionStartIndex;
  uint8_t authenticationLength;
  uint8_t encryptionLength;
  if (securityLevel == EMBER_GP_SECURITY_LEVEL_FC_MIC) {
    encryptionStartIndex = totalLength;
    authenticationLength = totalLength;
    encryptionLength = 0;
  } else {
    encryptionStartIndex = headerLength;
    authenticationLength = headerLength;
    encryptionLength = payloadLength;
  }
  emberAfGreenPowerClusterPrintln("Calculating MIC (%s) : ", __FUNCTION__);

  emberAfGreenPowerClusterPrint("Using KeyType = %d fc = %4x Key :[", keyType, gpdSecurityFrameCounter);
  for (int i = 0; i < 16; i++) {
    emberAfGreenPowerClusterPrint("%x ", key->contents[i]);
  }
  emberAfGreenPowerClusterPrint("]\n");
  emberAfGreenPowerClusterPrint("Prepared Nonce :[");
  for (int i = 0; i < NONCE_LENGTH; i++) {
    emberAfGreenPowerClusterPrint("%x ", nonce[i]);
  }
  emberAfGreenPowerClusterPrint("]\n");
  emberAfGreenPowerClusterPrint("Prepared Payload :[");
  for (int i = 0; i < totalLength; i++) {
    emberAfGreenPowerClusterPrint("%x ", payload[i]);
  }
  emberAfGreenPowerClusterPrint("]\n");
  emberAfGreenPowerClusterPrintln("encryptionStartIndex :%x,totalLength :%x payloadLength:%x",
                                  encryptionStartIndex,
                                  totalLength,
                                  payloadLength);
  emLoadKeyIntoCore(key->contents);
  if (securityLevel == EMBER_GP_SECURITY_LEVEL_FC_MIC_ENCRYPTED && encryptedPayload) {
    // Decrypt and then calculate the MIC
    emberCcmEncryptBytes(&payload[encryptionStartIndex], payloadLength, nonce);
    //emberAfGreenPowerClusterPrint("Decrypted (same as one more encryption) Payload :[");
    //for (int i = 0; i < payloadLength; i++) {
    // emberAfGreenPowerClusterPrint("%x ",payload[encryptionStartIndex + i]);
    //}
    //emberAfGreenPowerClusterPrint("]\n");
  }
  emberCcmCalculateAndEncryptMic(nonce,
                                 payload + authenticationStartIndex,
                                 authenticationLength,
                                 payload + encryptionStartIndex,
                                 encryptionLength,
                                 mic);
  return true;
}

static void initialiseKeyDerivationNonce(uint8_t * nonce,
                                         EmberGpAddress * gpdAddr,
                                         uint32_t gpdSecurityFrameCounter,
                                         bool directionIncoming)
{
  MEMSET(nonce, 0, SECURITY_BLOCK_SIZE);
  // SrcId : [1] - [8]
  if (gpdAddr->applicationId == GPD_APPLICATION_ID_IEEE) {
    MEMMOVE(nonce,
            gpdAddr->id.gpdIeeeAddress,
            EUI64_SIZE);
  } else {
    if (directionIncoming) {
      emberStoreLowHighInt32u(&(nonce[STANDALONE_NONCE_SOURCE_ADDR_INDEX]), gpdAddr->id.sourceId);
    } else {
      emberStoreLowHighInt32u(&(nonce[STANDALONE_NONCE_SOURCE_ADDR_INDEX]), 0);
    }
    emberStoreLowHighInt32u(&(nonce[STANDALONE_NONCE_SOURCE_ADDR_INDEX + 4]), gpdAddr->id.sourceId);
  }
  // Frame Counter : [9] - [12]
  if (directionIncoming) { // Decrypt
    if (gpdAddr->applicationId == GPD_APPLICATION_ID_IEEE) {
      MEMMOVE(nonce + STANDALONE_NONCE_FRAME_COUNTER_INDEX, gpdAddr->id.gpdIeeeAddress, 4);
    } else {
      emberStoreLowHighInt32u(&nonce[STANDALONE_NONCE_FRAME_COUNTER_INDEX], gpdAddr->id.sourceId);
    }
  } else { // encrypt
    emberStoreLowHighInt32u(&nonce[STANDALONE_NONCE_FRAME_COUNTER_INDEX], gpdSecurityFrameCounter);
  }
  // nonce[13] :
  // Security Control : 0x05 : all incoming and outgoing appId 0
  //                  : 0xC5 : outgoing appId 2
  if (gpdAddr->applicationId == GPD_APPLICATION_ID_IEEE
      && !directionIncoming) {
    nonce[STANDALONE_NONCE_SECURITY_CONTROL_INDEX] = NONCE_SECURITY_CONTROL_APP_ID2_OUTGOING;
  } else {
    nonce[STANDALONE_NONCE_SECURITY_CONTROL_INDEX] = NONCE_SECURITY_CONTROL;
  }
}
// Key Derivation for OOB Commissioing GPDF - incomming(decryption) and outgoing(encryption)
bool emGpKeyTcLkDerivation(EmberGpAddress * gpdAddr,
                           uint32_t gpdSecurityFrameCounter,
                           uint8_t mic[4],
                           const EmberKeyData* linkKey,
                           EmberKeyData * key,
                           bool directionIncoming)
{
  uint8_t nonce[SECURITY_BLOCK_SIZE] = { 0 };
  initialiseKeyDerivationNonce(nonce,
                               gpdAddr,
                               gpdSecurityFrameCounter,
                               directionIncoming);
  // The size is always 20 because the header is of 4 bytes and the Key is 16 bytes
  uint8_t payload[20] = { 0 };
  if (gpdAddr->applicationId == GPD_APPLICATION_ID_IEEE) {
    MEMMOVE(payload, gpdAddr->id.gpdIeeeAddress, 4);
  } else {
    emberStoreLowHighInt32u(payload, gpdAddr->id.sourceId);
  }
  emLoadKeyIntoCore(linkKey->contents);
  if (directionIncoming) {
    // Decrypt the incoming Key first then calculate the MIC
    emberCcmEncryptBytes(key->contents, EMBER_ENCRYPTION_KEY_SIZE, nonce);
    MEMCOPY(payload + 4, key->contents, EMBER_ENCRYPTION_KEY_SIZE);
  } else {
    // Take the Key in payload for MIC calculation first then encrypt the keys
    MEMCOPY(payload + 4, key->contents, EMBER_ENCRYPTION_KEY_SIZE);
    emberCcmEncryptBytes(key->contents, EMBER_ENCRYPTION_KEY_SIZE, nonce);
  }
  emberCcmCalculateAndEncryptMic(nonce, payload, 4, payload + 4, 16, mic);
  return true;
}

void emGpTestSecurity(void)
{
  EmberKeyData tcLk    = { { 0x5A, 0x69, 0x67, 0x42, 0x65, 0x65, 0x41, 0x6C, 0x6C, 0x69, 0x61, 0x6E, 0x63, 0x65, 0x30, 0x39 } };
  uint8_t mic[4] = { 0 };
  //uint128_t testIeee = 0x8877665544332211; in Little endian order
  uint8_t testIeee[] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
  EmberGpAddress gpdAddr;
  uint32_t fc = 0;
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //A.1.5.6 Security test  vectors for ApplicationID = 0b000 and bidirectional operation
  //A.1.5.6.2 Security test vectors for a shared key
  //A.1.5.6.2.1 SecurityLevel = 0b10
  //Outgoing
  emberAfGreenPowerClusterPrintln("\nTest Vector (A.1.5.6.2.1) Secured outGoing GPDF command id = 0xF3 Payload={0} SharedKey, seclevel = 0b10 Application Id 0");
  EmberKeyData testKey5 = { { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF } };
  gpdAddr.id.sourceId = 0x87654321;
  gpdAddr.applicationId = 0;
  fc = 0x44332211;
  uint8_t gpdCommandId = 0xF3;
  uint8_t gpdCommandPayload[2] = { 1, 0 }; //Format to specefy payload = [payloadlength, <payload bytes>]
  uint8_t securedOutgoingGpdf[100] = { 0 };

  uint8_t securedGpdfFrameLength = emGpOutgoingCommandEncrypt(&gpdAddr,
                                                              fc,
                                                              0, // Key type : Shared Key = 0, Individual Key = 1
                                                              &testKey5,
                                                              2, // Security Level : Encrypted = 3, MIC only = 2
                                                              gpdCommandId,
                                                              gpdCommandPayload, // format [payloadlength, <payload bytes>]
                                                              securedOutgoingGpdf, // Collector for the secured frame
                                                              100);

  emberAfGreenPowerClusterPrint("Secured Frame  :");
  for (int i = 0; i < securedGpdfFrameLength; i++) {
    emberAfGreenPowerClusterPrint("%x ", securedOutgoingGpdf[i]);
  }
  emberAfGreenPowerClusterPrint("");
  emberAfGreenPowerClusterPrintln("\nExpected Frame :8C 90 21 43 65 87 11 22 33 44 F3 00 CC A0 BB 2E ");
  emberAfGreenPowerClusterPrintln("");

  //A.1.5.6.2.2 SecurityLevel = 0b11
  //Outgoing
  emberAfGreenPowerClusterPrintln("\nTest Vector (A.1.5.6.2.2) Secured outGoing GPDF command id = 0xF3 Payload={0} SharedKey, seclevel = 0b11 Application Id 0");
  securedGpdfFrameLength = emGpOutgoingCommandEncrypt(&gpdAddr,
                                                      fc,
                                                      0, // Key type : Shared Key = 0, Individual Key = 1
                                                      &testKey5,
                                                      3, // Security Level : Encrypted = 3, MIC only = 2
                                                      gpdCommandId,
                                                      gpdCommandPayload, // format [payloadlength, <payload bytes>]
                                                      securedOutgoingGpdf, // Collector for the secured frame
                                                      100);

  emberAfGreenPowerClusterPrint("Secured Frame  :");
  for (int i = 0; i < securedGpdfFrameLength; i++) {
    emberAfGreenPowerClusterPrint("%x ", securedOutgoingGpdf[i]);
  }
  emberAfGreenPowerClusterPrint("");
  emberAfGreenPowerClusterPrintln("\nExpected Frame :8C 98 21 43 65 87 11 22 33 44 9E 7E 14 0F B5 DA ");
  emberAfGreenPowerClusterPrintln("");
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  gpdAddr.id.sourceId = 0x12345678;
  gpdAddr.applicationId = 0;
  emberAfGreenPowerClusterPrintln("\n\nTest Vector (A.1.5.8.1) for TCLK Decryption of Incoming Key");
  emberAfGreenPowerClusterPrintln("Incoming decrypted key for App Id = %d GpdId= 0x%4X", gpdAddr.applicationId, gpdAddr.id.sourceId);
  EmberKeyData incomingEncryptedKey1 = { { 0x7D, 0x17, 0x7B, 0xD2, 0x9E, 0xA0, 0xFD, 0xA6, 0xB0, 0x17, 0x03, 0x65, 0x87, 0xDC, 0x26, 0x00 } };

  emGpKeyTcLkDerivation(&gpdAddr,
                        0,
                        mic,
                        &tcLk,
                        &incomingEncryptedKey1,
                        true);
  emberAfGreenPowerClusterPrint("Decrypted Key :");
  for (int i = 0; i < EMBER_ENCRYPTION_KEY_SIZE; i++) {
    emberAfCorePrint("%x ", incomingEncryptedKey1.contents[i]);
  }
  emberAfGreenPowerClusterPrint("\nExpected Key  :C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF");
  emberAfGreenPowerClusterPrint("\nGenerated MIC :");
  for (int i = 0; i < 4; i++) {
    emberAfGreenPowerClusterPrint("%x ", mic[i]);
  }
  emberAfGreenPowerClusterPrint("\nExpected MIC  :61 F1 63 A9");

  emberAfGreenPowerClusterPrintln("\n\nTest Vector (A.1.5.13.1) for TCLK Decryption of Incoming Key IEEE Address");
  emberAfGreenPowerClusterPrintln("Incoming decrypted key for App Id = 2 Ieee= 0x8877665544332211");
  MEMCOPY(gpdAddr.id.gpdIeeeAddress, testIeee, 8);
  gpdAddr.endpoint = 0x0A;
  gpdAddr.applicationId = 2;
  EmberKeyData incomingEncryptedKey11 = { { 0x2D, 0xF0, 0x67, 0xAF, 0xCD, 0x4D, 0x8C, 0xF0, 0xF5, 0x2E, 0x6C, 0x85, 0x8F, 0x31, 0x4E, 0x22 } };

  emGpKeyTcLkDerivation(&gpdAddr,
                        0,
                        mic,
                        &tcLk,
                        &incomingEncryptedKey11,
                        true);
  emberAfGreenPowerClusterPrint("Decrypted Key :");
  for (int i = 0; i < EMBER_ENCRYPTION_KEY_SIZE; i++) {
    emberAfCorePrint("%x ", incomingEncryptedKey11.contents[i]);
  }
  emberAfGreenPowerClusterPrint("\nExpected Key  :C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF");
  emberAfGreenPowerClusterPrint("\nGenerated MIC :");
  for (int i = 0; i < 4; i++) {
    emberAfGreenPowerClusterPrint("%x ", mic[i]);
  }
  emberAfGreenPowerClusterPrint("\nExpected MIC  :3F 9A E0 B5");

  emberAfGreenPowerClusterPrintln("\n\nTest Vector (A.1.5.8.3) for TCLK Protected Key for Commissioning Reply");
  emberAfGreenPowerClusterPrintln("OutGoing Protected Key for App Id = 0 GpdId= 0x12345678, fc =4 ");
  // Security Level 3 Key derivation Application Id 0 - Out going
  EmberKeyData testKey = { { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF } };
  gpdAddr.id.sourceId = 0x12345678;
  gpdAddr.applicationId = 0;
  fc = 4;
  emGpKeyTcLkDerivation(&gpdAddr,
                        fc,
                        mic,
                        &tcLk,
                        &testKey,
                        false);
  emberAfGreenPowerClusterPrint("Encrypted Key :");
  for (int i = 0; i < EMBER_ENCRYPTION_KEY_SIZE; i++) {
    emberAfCorePrint("%x ", testKey.contents[i]);
  }
  emberAfGreenPowerClusterPrint("\nExpected Key  :E9 00 06 63 1D 0D FD C6 38 06 8E 5E 69 67 D3 25");
  emberAfGreenPowerClusterPrint("\nGenerated MIC :");
  for (int i = 0; i < 4; i++) {
    emberAfGreenPowerClusterPrint("%x ", mic[i]);
  }
  emberAfGreenPowerClusterPrint("\nExpected MIC  :27 55 9F 75");

  emberAfGreenPowerClusterPrintln("\n\nTest Vector (A.1.5.13.2) for TCLK Protected Key for Commissioning Reply");
  emberAfGreenPowerClusterPrintln("OutGoing Protected Key for App Id = 2 Ieee= 0x8877665544332211, fc =3 ");
  // Commissioning Reply TC-LK Protected key App Id = 2
  EmberKeyData testKey11 = { { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF } };
  MEMCOPY(gpdAddr.id.gpdIeeeAddress, testIeee, 8);
  gpdAddr.endpoint = 0x0A;
  gpdAddr.applicationId = 2;
  fc = 3;

  emGpKeyTcLkDerivation(&gpdAddr,
                        fc,
                        mic,
                        &tcLk,
                        &testKey11,
                        false);
  emberAfGreenPowerClusterPrint("Encrypted Key :");
  for (int i = 0; i < EMBER_ENCRYPTION_KEY_SIZE; i++) {
    emberAfCorePrint("%x ", testKey11.contents[i]);
  }
  emberAfGreenPowerClusterPrint("\nExpected Key  :2D 23 8F 58 07 1C 07 8A B0 5C 23 5E 4D ED DF 3B ");
  emberAfGreenPowerClusterPrint("\nGenerated MIC :");
  for (int i = 0; i < 4; i++) {
    emberAfGreenPowerClusterPrint("%x ", mic[i]);
  }
  emberAfGreenPowerClusterPrint("\nExpected MIC  :DE F5 18 7D");

  // Shared Key Security Level 2 Application Id 0 - Incoming
  emberAfGreenPowerClusterPrintln("\n\nTest Vector (A.1.5.4.2) MIC of command id = 0x02 (No Payload) SharedKey seclevel = 0b10 Application Id 0");
  EmberKeyData testKey1 = { { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF } };
  gpdAddr.id.sourceId = 0x87654321;
  gpdAddr.applicationId = 0;
  fc = 2;
  emGpCalculateIncomingCommandMic(&gpdAddr,
                                  false,
                                  0,//EMBER_AF_GREEN_POWER_GP_SHARED_KEY,
                                  2,
                                  fc,
                                  0x20,
                                  NULL,
                                  false,
                                  mic,
                                  &testKey1);

  emberAfGreenPowerClusterPrint("Generated MIC:");
  for (int i = 0; i < 4; i++) {
    emberAfGreenPowerClusterPrint("%x ", mic[i]);
  }
  emberAfGreenPowerClusterPrintln("\nExpected MIC :CF 78 7E 72");

  // Shared Key Security Level 3 Application Id 0 - Incoming
  emberAfGreenPowerClusterPrintln("\n\nTest Vector (A.1.5.4.3) MIC of command id = 0x02 (No Payload) SharedKey seclevel = 0b11 Application Id 0");
  EmberKeyData testKey3 = { { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF } };

  gpdAddr.id.sourceId = 0x87654321;
  gpdAddr.applicationId = 0;
  fc = 2;
  emGpCalculateIncomingCommandMic(&gpdAddr,
                                  false,
                                  0,//EMBER_AF_GREEN_POWER_GP_INDIVIDUAL_KEY,
                                  3,
                                  fc,
                                  0x20,
                                  NULL,
                                  false,
                                  mic,
                                  &testKey3);
  emberAfGreenPowerClusterPrint("Generated MIC:");
  for (int i = 0; i < 4; i++) {
    emberAfGreenPowerClusterPrint("%x ", mic[i]);
  }
  emberAfGreenPowerClusterPrintln("\nExpected MIC :CA 43 24 DD");

  // Shared Key Security Level 2 Application Id 0 - Incoming
  emberAfGreenPowerClusterPrintln("\n\nTest Vector (A.1.5.5.2) MIC of command id = 0x02 (No Payload) IndividulaKey seclevel = 0b10 Application Id 0");
  EmberKeyData testKey1552 = { { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF } };
  gpdAddr.id.sourceId = 0x87654321;
  gpdAddr.applicationId = 0;
  fc = 2;
  emGpCalculateIncomingCommandMic(&gpdAddr,
                                  false,
                                  1,//EMBER_AF_GREEN_POWER_GP_INDIVIDUAL_KEY,
                                  2,
                                  fc,
                                  0x20,
                                  NULL,
                                  false,
                                  mic,
                                  &testKey1552);

  emberAfGreenPowerClusterPrint("Generated MIC:");
  for (int i = 0; i < 4; i++) {
    emberAfGreenPowerClusterPrint("%x ", mic[i]);
  }
  emberAfGreenPowerClusterPrintln("\nExpected MIC :AD 69 A9 78");

  // Shared Key Security Level 3 Application Id 0 - Incoming
  emberAfGreenPowerClusterPrintln("\n\nTest Vector (A.1.5.5.3) MIC of command id = 0x02 (No Payload) IndividualKey seclevel = 0b11 Application Id 0");
  EmberKeyData testKey1553 = { { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF } };

  gpdAddr.id.sourceId = 0x87654321;
  gpdAddr.applicationId = 0;
  fc = 2;
  emGpCalculateIncomingCommandMic(&gpdAddr,
                                  false,
                                  1,//EMBER_AF_GREEN_POWER_GP_SHARED_KEY,
                                  3,
                                  fc,
                                  0x20,
                                  NULL,
                                  false,
                                  mic,
                                  &testKey1553);
  emberAfGreenPowerClusterPrint("Generated MIC:");
  for (int i = 0; i < 4; i++) {
    emberAfGreenPowerClusterPrint("%x ", mic[i]);
  }
  emberAfGreenPowerClusterPrintln("\nExpected MIC :5F 1A 30 34");

  // Shared Key Security Level 2 Application Id 2 - Incoming
  emberAfGreenPowerClusterPrintln("\n\nTest Vector (A.1.5.9.2) MIC of command id = 0x02 (No Payload) SharedKey seclevel = 0b10 Application Id 2");
  EmberKeyData testKey2 = { { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF } };
  MEMCOPY(gpdAddr.id.gpdIeeeAddress, testIeee, 8);
  gpdAddr.endpoint = 0x0A;
  gpdAddr.applicationId = 2;
  fc = 2;
  emGpCalculateIncomingCommandMic(&gpdAddr,
                                  false,
                                  0,//EMBER_AF_GREEN_POWER_GP_SHARED_KEY,
                                  2,
                                  fc,
                                  0x20,
                                  NULL,
                                  false,
                                  mic,
                                  &testKey2);
  emberAfGreenPowerClusterPrint("Generated MIC:");
  for (int i = 0; i < 4; i++) {
    emberAfGreenPowerClusterPrint("%x ", mic[i]);
  }
  emberAfGreenPowerClusterPrintln("\nExpected MIC :C5 A8 3C 5E ");

  // Shared Key Security Level 3 Application Id 2 - Incoming
  emberAfGreenPowerClusterPrintln("\nTest Vector (A.1.5.9.3) MIC of command id = 0x02 (No Payload) SharedKey seclevel = 0b11 Application Id 2");
  EmberKeyData testKey4 = { { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF } };
  MEMCOPY(gpdAddr.id.gpdIeeeAddress, testIeee, 8);
  gpdAddr.applicationId = 2;
  fc = 2;
  emGpCalculateIncomingCommandMic(&gpdAddr,
                                  false,
                                  0,//EMBER_AF_GREEN_POWER_GP_SHARED_KEY,
                                  3,
                                  fc,
                                  0x20,
                                  NULL,
                                  false,
                                  mic,
                                  &testKey4);
  emberAfGreenPowerClusterPrint("Generated MIC:");
  for (int i = 0; i < 4; i++) {
    emberAfGreenPowerClusterPrint("%x ", mic[i]);
  }
  emberAfGreenPowerClusterPrintln("\nExpected MIC :D2 A2 36 1B");
  emberAfGreenPowerClusterPrintln(" ");

  // A.1.5.7.1 NWK-key derived GPD group key
  uint8_t UNUSED nwkKey[EMBER_ENCRYPTION_KEY_SIZE] = { 0x01, 0x03, 0x05, 0x07, 0x09, 0x0b, 0x0d, 0x0f, 0x00, 0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0d };
  // Hash it with 'ZGP'
  uint8_t result[EMBER_ENCRYPTION_KEY_SIZE] = { 0 };
  emberHmacAesHash(nwkKey,
                   (uint8_t *)"ZGP",
                   3,
                   result);
  emberAfGreenPowerClusterPrint("Network Derived Group Key:");
  for (int i = 0; i < 16; i++) {
    emberAfGreenPowerClusterPrint("%x ", result[i]);
  }
  emberAfGreenPowerClusterPrintln("\nExpected Key : BA 88 86 7f c0 09 39 87 eb 88 64 ce be 5f c6 13");
  emberAfGreenPowerClusterPrintln(" ");

  EmberKeyData groupKey = { { 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf } };
  (void) emberAfWriteAttribute(242,//GP_ENDPOINT,
                               ZCL_GREEN_POWER_CLUSTER_ID,
                               ZCL_GP_SERVER_GP_SHARED_SECURITY_KEY_ATTRIBUTE_ID,
                               CLUSTER_MASK_SERVER,
                               groupKey.contents,
                               ZCL_SECURITY_KEY_ATTRIBUTE_TYPE);

  uint8_t gpsSecurityKeyTypeAtrribute;
  EmberKeyData gpSharedKeyAttribute;
  uint8_t keyType = 1;
  (void) emberAfWriteAttribute(242,//GP_ENDPOINT,
                               ZCL_GREEN_POWER_CLUSTER_ID,
                               ZCL_GP_SERVER_GP_SHARED_SECURITY_KEY_TYPE_ATTRIBUTE_ID,
                               CLUSTER_MASK_SERVER,
                               (uint8_t *)(&keyType),
                               ZCL_BITMAP8_ATTRIBUTE_TYPE);

  MEMSET(&gpSharedKeyAttribute, 0, sizeof(EmberKeyData));
  emberAfGreenPowerServerDeriveSharedKeyFromSinkAttribute(&gpsSecurityKeyTypeAtrribute,
                                                          &gpSharedKeyAttribute,
                                                          &gpdAddr);
  emberAfGreenPowerClusterPrint("Shared Key Type = Network Key(%d):\nRetrived Key : ", gpsSecurityKeyTypeAtrribute);
  for (int i = 0; i < 16; i++) {
    emberAfGreenPowerClusterPrint("%x ", gpSharedKeyAttribute.contents[i]);
  }
  emberAfGreenPowerClusterPrintln(" ");

  keyType = 2;
  (void) emberAfWriteAttribute(242,//GP_ENDPOINT,
                               ZCL_GREEN_POWER_CLUSTER_ID,
                               ZCL_GP_SERVER_GP_SHARED_SECURITY_KEY_TYPE_ATTRIBUTE_ID,
                               CLUSTER_MASK_SERVER,
                               (uint8_t *)(&keyType),
                               ZCL_BITMAP8_ATTRIBUTE_TYPE);

  MEMSET(&gpSharedKeyAttribute, 0, sizeof(EmberKeyData));
  emberAfGreenPowerServerDeriveSharedKeyFromSinkAttribute(&gpsSecurityKeyTypeAtrribute,
                                                          &gpSharedKeyAttribute,
                                                          &gpdAddr);
  emberAfGreenPowerClusterPrint("Shared Key Type = Network Derived Group Key (%d):\nRetrived Key : ", gpsSecurityKeyTypeAtrribute);
  for (int i = 0; i < 16; i++) {
    emberAfGreenPowerClusterPrint("%x ", gpSharedKeyAttribute.contents[i]);
  }
  emberAfGreenPowerClusterPrintln(" ");

  keyType = 3;
  (void) emberAfWriteAttribute(242,//GP_ENDPOINT,
                               ZCL_GREEN_POWER_CLUSTER_ID,
                               ZCL_GP_SERVER_GP_SHARED_SECURITY_KEY_TYPE_ATTRIBUTE_ID,
                               CLUSTER_MASK_SERVER,
                               (uint8_t *)(&keyType),
                               ZCL_BITMAP8_ATTRIBUTE_TYPE);
  MEMSET(&gpSharedKeyAttribute, 0, sizeof(EmberKeyData));
  emberAfGreenPowerServerDeriveSharedKeyFromSinkAttribute(&gpsSecurityKeyTypeAtrribute,
                                                          &gpSharedKeyAttribute,
                                                          &gpdAddr);
  emberAfGreenPowerClusterPrint("Shared Key Type = Group Key(%d):\nRetrived Key : ", gpsSecurityKeyTypeAtrribute);
  for (int i = 0; i < 16; i++) {
    emberAfGreenPowerClusterPrint("%x ", gpSharedKeyAttribute.contents[i]);
  }
  emberAfGreenPowerClusterPrintln(" ");

  keyType = 7;
  (void) emberAfWriteAttribute(242,//GP_ENDPOINT,
                               ZCL_GREEN_POWER_CLUSTER_ID,
                               ZCL_GP_SERVER_GP_SHARED_SECURITY_KEY_TYPE_ATTRIBUTE_ID,
                               CLUSTER_MASK_SERVER,
                               (uint8_t *)(&keyType),
                               ZCL_BITMAP8_ATTRIBUTE_TYPE);

  gpdAddr.id.sourceId = 0x87654321;
  gpdAddr.applicationId = 0;
  MEMSET(&gpSharedKeyAttribute, 0, sizeof(EmberKeyData));
  emberAfGreenPowerServerDeriveSharedKeyFromSinkAttribute(&gpsSecurityKeyTypeAtrribute,
                                                          &gpSharedKeyAttribute,
                                                          &gpdAddr);
  emberAfGreenPowerClusterPrint("A.1.5.7.2 Derived individual GPD key =%d, AppId = %d:\nGenerated Key : ", gpsSecurityKeyTypeAtrribute, gpdAddr.applicationId);
  for (int i = 0; i < 16; i++) {
    emberAfGreenPowerClusterPrint("%x ", gpSharedKeyAttribute.contents[i]);
  }
  emberAfGreenPowerClusterPrintln("\nExpected Key : 7a 3a 73 43 8d 6e 47 55 28 81 a0 28 ad 59 23 2e");
  emberAfGreenPowerClusterPrintln(" ");

  MEMCOPY(gpdAddr.id.gpdIeeeAddress, testIeee, 8);
  gpdAddr.applicationId = 2;
  MEMSET(&gpSharedKeyAttribute, 0, sizeof(EmberKeyData));
  emberAfGreenPowerServerDeriveSharedKeyFromSinkAttribute(&gpsSecurityKeyTypeAtrribute,
                                                          &gpSharedKeyAttribute,
                                                          &gpdAddr);
  emberAfGreenPowerClusterPrint("A.1.5.12.1 Derived individual GPD key=%d, AppId = %d:\nGenerated Key : ", gpsSecurityKeyTypeAtrribute, gpdAddr.applicationId);
  for (int i = 0; i < 16; i++) {
    emberAfGreenPowerClusterPrint("%x ", gpSharedKeyAttribute.contents[i]);
  }
  emberAfGreenPowerClusterPrintln("\nExpected Key : 8a e7 5b 07 5f 7a 13 23 06 08 ff 7e 93 07 97 6d");
  emberAfGreenPowerClusterPrintln(" ");
}
