/***************************************************************************//**
 * @file
 * @brief Functions for reading and writing command and response frames.
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

#ifndef SILABS_EZSP_FRAME_UTILITIES_H
#define SILABS_EZSP_FRAME_UTILITIES_H

#include "stack/include/zll-types.h"

// The contents of the current EZSP frame.  This pointer can be used inside
// ezspErrorHandler() to obtain information about the command that preceded
// the error (such as the command ID, index EZSP_FRAME_ID_INDEX).
extern uint8_t* ezspFrameContents;

// This pointer steps through the received frame as the contents are read.
extern uint8_t* ezspReadPointer;

// This pointer steps through the to-be-transmitted frame as the contents
// are written.
extern uint8_t* ezspWritePointer;

uint8_t fetchInt8u(void);
uint8_t *fetchInt8uPointer(uint8_t length);
void appendInt8u(uint8_t value);

// generated ezsp has a call to this
#define fetchInt16uPointer fetchInt8uPointer

uint16_t fetchInt16u(void);
void appendInt16u(uint16_t value);

void appendInt32u(uint32_t value);
uint32_t fetchInt32u(void);

void appendInt8uArray(uint8_t length, uint8_t *contents);
void fetchInt8uArray(uint8_t length, uint8_t *contents);

void appendInt16uArray(uint8_t length, uint16_t *contents);
void fetchInt16uArray(uint8_t length, uint16_t *contents);

// For functions that are used only once in the em260 code we use macros
// in order to save code space.

#define appendEmberNetworkParameters(value)                         \
  do {                                                              \
    appendInt8uArray(EXTENDED_PAN_ID_SIZE, (value)->extendedPanId); \
    appendInt16u((value)->panId);                                   \
    appendInt8u((value)->radioTxPower);                             \
    appendInt8u((value)->radioChannel);                             \
    appendInt8u((value)->joinMethod);                               \
    appendInt16u((value)->nwkManagerId);                            \
    appendInt8u((value)->nwkUpdateId);                              \
    appendInt32u((value)->channels);                                \
  } while (0)

#define appendEmberMultiPhyRadioParameters(value) \
  do {                                            \
    appendInt8u((value)->radioTxPower);           \
    appendInt8u((value)->radioPage);              \
    appendInt8u((value)->radioChannel);           \
  } while (0)

void fetchEmberNetworkParameters(EmberNetworkParameters *value);
void fetchEmberMultiPhyRadioParameters(EmberMultiPhyRadioParameters *parameters);

void appendEmberApsFrame(EmberApsFrame *value);
void fetchEmberApsFrame(EmberApsFrame *apsFrame);

#define appendEmberBindingTableEntry(value)            \
  do {                                                 \
    appendInt8u((value)->type);                        \
    appendInt8u((value)->local);                       \
    appendInt16u((value)->clusterId);                  \
    appendInt8u((value)->remote);                      \
    appendInt8uArray(EUI64_SIZE, (value)->identifier); \
    appendInt8u((value)->networkIndex);                \
  } while (0)

#define fetchEmberBindingTableEntry(value)            \
  do {                                                \
    (value)->type         = fetchInt8u();             \
    (value)->local        = fetchInt8u();             \
    (value)->clusterId    = fetchInt16u();            \
    (value)->remote       = fetchInt8u();             \
    fetchInt8uArray(EUI64_SIZE, (value)->identifier); \
    (value)->networkIndex = fetchInt8u();             \
  } while (0)

void appendEmberMulticastTableEntry(EmberMulticastTableEntry *value);
void fetchEmberMulticastTableEntry(EmberMulticastTableEntry *value);

void fetchEmberBeaconClassificationParams(EmberBeaconClassificationParams *value);
void appendEmberBeaconClassificationParams(EmberBeaconClassificationParams *value);

void appendEmberNeighborTableEntry(EmberNeighborTableEntry *value);
void fetchEmberNeighborTableEntry(EmberNeighborTableEntry *value);

#define appendEmberNeighborTableEntry(value)       \
  do {                                             \
    appendInt16u((value)->shortId);                \
    appendInt8u((value)->averageLqi);              \
    appendInt8u((value)->inCost);                  \
    appendInt8u((value)->outCost);                 \
    appendInt8u((value)->age);                     \
    appendInt8uArray(EUI64_SIZE, (value)->longId); \
  } while (0)

#define appendEmberRouteTableEntry(value)   \
  do {                                      \
    appendInt16u((value)->destination);     \
    appendInt16u((value)->nextHop);         \
    appendInt8u((value)->status);           \
    appendInt8u((value)->age);              \
    appendInt8u((value)->concentratorType); \
    appendInt8u((value)->routeRecordState); \
  } while (0)

void fetchEmberRouteTableEntry(EmberRouteTableEntry *value);

#define appendEmberKeyData(value) \
  (appendInt8uArray(EMBER_ENCRYPTION_KEY_SIZE, (value)->contents))

#define fetchEmberKeyData(value) \
  (fetchInt8uArray(EMBER_ENCRYPTION_KEY_SIZE, (value)->contents))

#define appendEmberTransientKeyData(value)        \
  do {                                            \
    appendInt8uArray(EUI64_SIZE, (value)->eui64); \
    appendEmberKeyData(&((value)->keyData));      \
    appendInt32u((value)->incomingFrameCounter);  \
    appendInt16u((value)->bitmask);               \
    appendInt16u((value)->remainingTimeSeconds);  \
    appendInt8u((value)->networkIndex);           \
  } while (0)

#define fetchEmberTransientKeyData(value)          \
  do {                                             \
    fetchInt8uArray(EUI64_SIZE, (value)->eui64);   \
    fetchEmberKeyData(&((value)->keyData));        \
    (value)->incomingFrameCounter = fetchInt32u(); \
    (value)->bitmask = fetchInt16u();              \
    (value)->remainingTimeSeconds = fetchInt16u(); \
    (value)->networkIndex = fetchInt8u();          \
  } while (0)

void appendEmberInitialSecurityState(EmberInitialSecurityState *value);

#define fetchEmberInitialSecurityState(value)                            \
  do {                                                                   \
    (value)->bitmask = fetchInt16u();                                    \
    fetchEmberKeyData(&((value)->preconfiguredKey));                     \
    fetchEmberKeyData(&((value)->networkKey));                           \
    (value)->networkKeySequenceNumber = fetchInt8u();                    \
    fetchInt8uArray(EUI64_SIZE, (value)->preconfiguredTrustCenterEui64); \
  } while (0)

#define appendEmberCurrentSecurityState(value)                     \
  do {                                                             \
    appendInt16u((value)->bitmask);                                \
    appendInt8uArray(EUI64_SIZE, (value)->trustCenterLongAddress); \
  } while (0)

void fetchEmberCurrentSecurityState(EmberCurrentSecurityState *value);

#define appendEmberKeyStruct(value)                      \
  do {                                                   \
    appendInt16u((value)->bitmask);                      \
    appendInt8u((value)->type);                          \
    appendEmberKeyData(&((value)->key));                 \
    appendInt32u((value)->outgoingFrameCounter);         \
    appendInt32u((value)->incomingFrameCounter);         \
    appendInt8u((value)->sequenceNumber);                \
    appendInt8uArray(EUI64_SIZE, (value)->partnerEUI64); \
  } while (0)

void fetchEmberKeyStruct(EmberKeyStruct *value);

#define appendEmberChildData(value)               \
  do {                                            \
    appendInt8uArray(EUI64_SIZE, (value)->eui64); \
    appendInt8u((value)->type);                   \
    appendInt16u((value)->id);                    \
    appendInt8u((value)->phy);                    \
    appendInt8u((value)->power);                  \
    appendInt8u((value)->timeout);                \
  } while (0)

void fetchEmberChildData(EmberChildData *value);

void fetchEmberZigbeeNetwork(EmberZigbeeNetwork *value);
void appendEmberZigbeeNetwork(EmberZigbeeNetwork *value);

#define appendEmberCertificateData(value) \
  (appendInt8uArray(EMBER_CERTIFICATE_SIZE, (value)->contents))
#define fetchEmberCertificateData(value) \
  (fetchInt8uArray(EMBER_CERTIFICATE_SIZE, (value)->contents))
#define appendEmberPublicKeyData(value) \
  (appendInt8uArray(EMBER_PUBLIC_KEY_SIZE, (value)->contents))
#define fetchEmberPublicKeyData(value) \
  (fetchInt8uArray(EMBER_PUBLIC_KEY_SIZE, (value)->contents))
#define appendEmberPrivateKeyData(value) \
  (appendInt8uArray(EMBER_PRIVATE_KEY_SIZE, (value)->contents))
#define fetchEmberPrivateKeyData(value) \
  (fetchInt8uArray(EMBER_PRIVATE_KEY_SIZE, (value)->contents))
#define appendEmberSmacData(value) \
  (appendInt8uArray(EMBER_SMAC_SIZE, (value)->contents))
#define fetchEmberSmacData(value) \
  (fetchInt8uArray(EMBER_SMAC_SIZE, (value)->contents))
#define appendEmberSignatureData(value) \
  (appendInt8uArray(EMBER_SIGNATURE_SIZE, (value)->contents))
#define fetchEmberSignatureData(value) \
  (fetchInt8uArray(EMBER_SIGNATURE_SIZE, (value)->contents))

#define appendEmberCertificate283k1Data(value) \
  (appendInt8uArray(EMBER_CERTIFICATE_283K1_SIZE, (value)->contents))
#define fetchEmberCertificate283k1Data(value) \
  (fetchInt8uArray(EMBER_CERTIFICATE_283K1_SIZE, (value)->contents))
#define appendEmberPublicKey283k1Data(value) \
  (appendInt8uArray(EMBER_PUBLIC_KEY_283K1_SIZE, (value)->contents))
#define fetchEmberPublicKey283k1Data(value) \
  (fetchInt8uArray(EMBER_PUBLIC_KEY_283K1_SIZE, (value)->contents))
#define appendEmberPrivateKey283k1Data(value) \
  (appendInt8uArray(EMBER_PRIVATE_KEY_283K1_SIZE, (value)->contents))
#define fetchEmberPrivateKey283k1Data(value) \
  (fetchInt8uArray(EMBER_PRIVATE_KEY_283K1_SIZE, (value)->contents))
#define appendEmberSignature283k1Data(value) \
  (appendInt8uArray(EMBER_SIGNATURE_283K1_SIZE, (value)->contents))
#define fetchEmberSignature283k1Data(value) \
  (fetchInt8uArray(EMBER_SIGNATURE_283K1_SIZE, (value)->contents))

void appendEmberAesMmoHashContext(EmberAesMmoHashContext* context);
void fetchEmberAesMmoHashContext(EmberAesMmoHashContext* context);

#define appendEmberMessageDigest(value) \
  (appendInt8uArray(EMBER_AES_HASH_BLOCK_SIZE, (value)->contents))
#define fetchEmberMessageDigest(value) \
  (fetchInt8uArray(EMBER_AES_HASH_BLOCK_SIZE, (value)->contents))

void appendEmberNetworkInitStruct(const EmberNetworkInitStruct* networkInitStruct);
void fetchEmberNetworkInitStruct(EmberNetworkInitStruct* networkInitStruct);

void appendEmberZllNetwork(EmberZllNetwork* network);
void fetchEmberZllNetwork(EmberZllNetwork* network);
void fetchEmberZllSecurityAlgorithmData(EmberZllSecurityAlgorithmData* data);
void fetchEmberTokTypeStackZllData(EmberTokTypeStackZllData *data);
void fetchEmberTokTypeStackZllSecurity(EmberTokTypeStackZllSecurity *security);
void appendEmberZllSecurityAlgorithmData(EmberZllSecurityAlgorithmData* data);
void appendEmberZllInitialSecurityState(EmberZllInitialSecurityState* state);
void appendEmberTokTypeStackZllData(EmberTokTypeStackZllData *data);
void appendEmberTokTypeStackZllSecurity(EmberTokTypeStackZllSecurity *security);

void appendEmberGpAddress(EmberGpAddress *value);
void fetchEmberGpAddress(EmberGpAddress *value);
void fetchEmberGpSinkList(EmberGpSinkListEntry *value);
void fetchEmberGpProxyTableEntry(EmberGpProxyTableEntry *value);
void appendEmberGpProxyTableEntry(EmberGpProxyTableEntry *value);

void fetchEmberGpSinkTableEntry(EmberGpSinkTableEntry *value);
void appendEmberGpSinkTableEntry(EmberGpSinkTableEntry *value);

void appendEmberDutyCycleLimits(EmberDutyCycleLimits* limits);
void fetchEmberDutyCycleLimits(EmberDutyCycleLimits* limits);
void appendEmberPerDeviceDutyCycle(uint8_t maxDevices,
                                   EmberPerDeviceDutyCycle* arrayOfDeviceDutyCycles);
void fetchEmberPerDeviceDutyCycle(EmberPerDeviceDutyCycle* arrayOfDeviceDutyCycles);
void formatBytesInToStructEmberPerDeviceDutyCycle(uint8_t* perDeviceDutyCycles,
                                                  EmberPerDeviceDutyCycle* arrayOfDeviceDutyCycles);
void formatStructEmberPerDeviceDutyCycleIntoBytes(uint8_t maxDevices,
                                                  EmberPerDeviceDutyCycle *arrayOfDeviceDutyCycles,
                                                  uint8_t* perDeviceDutyCycles);

#define fetchEmberZllDeviceInfoRecord(value)           \
  do {                                                 \
    fetchInt8uArray(EUI64_SIZE, (value)->ieeeAddress); \
    (value)->endpointId         = fetchInt8u();        \
    (value)->profileId          = fetchInt16u();       \
    (value)->deviceId           = fetchInt16u();       \
    (value)->version            = fetchInt8u();        \
    (value)->groupIdCount       = fetchInt8u();        \
  } while (0)

#define fetchEmberZllInitialSecurityState(value)     \
  do {                                               \
    (value)->bitmask = fetchInt32u();                \
    (value)->keyIndex = fetchInt8u();                \
    fetchEmberKeyData(&((value)->encryptionKey));    \
    fetchEmberKeyData(&((value)->preconfiguredKey)); \
  } while (0)

#define fetchEmberZllAddressAssignment(value) \
  do {                                        \
    (value)->nodeId         = fetchInt16u();  \
    (value)->freeNodeIdMin  = fetchInt16u();  \
    (value)->freeNodeIdMax  = fetchInt16u();  \
    (value)->groupIdMin     = fetchInt16u();  \
    (value)->groupIdMax     = fetchInt16u();  \
    (value)->freeGroupIdMin = fetchInt16u();  \
    (value)->freeGroupIdMax = fetchInt16u();  \
  } while (0)

// Secure EZSP frame utilities
#define appendSecureEzspRandomNumber(value) \
  (appendInt8uArray(SECURE_EZSP_RANDOM_NUMBER_SIZE, (value)->contents))
#define fetchSecureEzspRandomNumber(value) \
  (fetchInt8uArray(SECURE_EZSP_RANDOM_NUMBER_SIZE, (value)->contents))

#define appendEmberBeaconIterator(value)                                   \
  do {                                                                     \
    appendInt8u((value)->beacon.channel);                                  \
    appendInt8u((value)->beacon.lqi);                                      \
    appendInt8u((value)->beacon.rssi);                                     \
    appendInt8u((value)->beacon.depth);                                    \
    appendInt8u((value)->beacon.nwkUpdateId);                              \
    appendInt8u((value)->beacon.power);                                    \
    appendInt8u((value)->beacon.parentPriority);                           \
    appendInt8u((value)->beacon.enhanced);                                 \
    appendInt8u((value)->beacon.permitJoin);                               \
    appendInt8u((value)->beacon.hasCapacity);                              \
    appendInt16u((value)->beacon.panId);                                   \
    appendInt16u((value)->beacon.sender);                                  \
    appendInt8uArray(EXTENDED_PAN_ID_SIZE, (value)->beacon.extendedPanId); \
    appendInt8u((value)->index);                                           \
  } while (0)
#define fetchEmberBeaconIterator(value)                                   \
  do {                                                                    \
    MEMSET((value), 0, sizeof(EmberBeaconIterator));                      \
    (value)->beacon.channel        = fetchInt8u();                        \
    (value)->beacon.lqi            = fetchInt8u();                        \
    (value)->beacon.rssi           = fetchInt8u();                        \
    (value)->beacon.depth          = fetchInt8u();                        \
    (value)->beacon.nwkUpdateId    = fetchInt8u();                        \
    (value)->beacon.power          = fetchInt8u();                        \
    (value)->beacon.parentPriority = fetchInt8u();                        \
    (value)->beacon.enhanced       = fetchInt8u();                        \
    (value)->beacon.permitJoin     = fetchInt8u();                        \
    (value)->beacon.hasCapacity    = fetchInt8u();                        \
    (value)->beacon.panId          = fetchInt16u();                       \
    (value)->beacon.sender         = fetchInt16u();                       \
    fetchInt8uArray(EXTENDED_PAN_ID_SIZE, (value)->beacon.extendedPanId); \
    (value)->index                 = fetchInt8u();                        \
  } while (0)

#define appendEmberBeaconData(value)                                \
  do {                                                              \
    appendInt8u((value)->channel);                                  \
    appendInt8u((value)->lqi);                                      \
    appendInt8u((value)->rssi);                                     \
    appendInt8u((value)->depth);                                    \
    appendInt8u((value)->nwkUpdateId);                              \
    appendInt8u((value)->power);                                    \
    appendInt8u((value)->parentPriority);                           \
    appendInt8u((value)->enhanced);                                 \
    appendInt8u((value)->permitJoin);                               \
    appendInt8u((value)->hasCapacity);                              \
    appendInt16u((value)->panId);                                   \
    appendInt16u((value)->sender);                                  \
    appendInt8uArray(EXTENDED_PAN_ID_SIZE, (value)->extendedPanId); \
  } while (0)
#define fetchEmberBeaconData(value)                                \
  do {                                                             \
    MEMSET((value), 0, sizeof(EmberBeaconData));                   \
    (value)->channel        = fetchInt8u();                        \
    (value)->lqi            = fetchInt8u();                        \
    (value)->rssi           = fetchInt8u();                        \
    (value)->depth          = fetchInt8u();                        \
    (value)->nwkUpdateId    = fetchInt8u();                        \
    (value)->power          = fetchInt8u();                        \
    (value)->parentPriority = fetchInt8u();                        \
    (value)->enhanced       = fetchInt8u();                        \
    (value)->permitJoin     = fetchInt8u();                        \
    (value)->hasCapacity    = fetchInt8u();                        \
    (value)->panId          = fetchInt16u();                       \
    (value)->sender         = fetchInt16u();                       \
    fetchInt8uArray(EXTENDED_PAN_ID_SIZE, (value)->extendedPanId); \
  } while (0)

// Format the same as in command-interpreter2.h, with the addition of
// a 'p' for a pointer.
// Make sure to pass references to all of the variables! And if an array is
// passed, the length needs to come directly after it. This length should
// be a reference as well.
void emAfFetchOrAppend(bool fetch, const char * format, ...);

void appendEmberTokenData(EmberTokenData * tokenData);
void fetchEmberTokenData(EmberTokenData * tokenData);
void appendEmberTokenInfo(EmberTokenInfo *tokenInfo);
void fetchEmberTokenInfo(EmberTokenInfo *tokenInfo);
#endif // __EZSP_FRAME_UTILITIES_H__
