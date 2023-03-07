/***************************************************************************//**
 * @file
 * @brief Routines built on mbedTLS functions used by the GPD.
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
#include "gpd-components-common.h"

#if defined(CRYPTO_COUNT)
  #define MBEDTLS_DEVICE_COUNT CRYPTO_COUNT
#else
  #define MBEDTLS_DEVICE_COUNT 1
#endif

#define NONCE_SIZE                          13

#define ZIGBEE_SECURITY_CONTROL               0x05 // For all Tx and Rx for AppId = 0
#define ZIGBEE_SECURITY_CONTROL_IEEE_RX       0xC5 // For Rx with AppId = 2

// ZigBee Alliance Pre-configured TC Link Key - 'ZigBeeAlliance09'
static const uint8_t keyDefaultTCLink[] = {
  0x5a, 0x69, 0x67, 0x42, 0x65, 0x65, 0x41, 0x6c,
  0x6c, 0x69, 0x61, 0x6e, 0x63, 0x65, 0x30, 0x39
};

static uint8_t nonceTx[NONCE_SIZE];
static uint8_t nonceRx[NONCE_SIZE];

static mbedtls_ccm_context mbedtls_device_ctx[MBEDTLS_DEVICE_COUNT];
static int8_t device_instance = 0;

static int8_t gpdMbedtlsEncryptTagKeyWrapper(uint8_t* data,
                                             uint8_t* encryptedOut,
                                             uint8_t* mic)
{
  int status = 0;
  status = mbedtls_ccm_encrypt_and_tag(&mbedtls_device_ctx[device_instance], 16,
                                       (const unsigned char *)nonceTx, NONCE_SIZE,
                                       (const unsigned char *)data, 4,
                                       (const unsigned char *)&data[4], encryptedOut,
                                       mic, TAG_SIZE);
  if (status == 0) {
    return SUCCESS;
  }
  return FAILED;
}

static int8_t gpdMbedtlsAuthDecryptKeyWrapper(uint8_t* data,
                                              uint8_t* decryptedOut)
{
  int status = 0;
  status = mbedtls_ccm_auth_decrypt(&mbedtls_device_ctx[device_instance], 16,
                                    (const unsigned char *)nonceRx, NONCE_SIZE,
                                    (const unsigned char *)data, 4,
                                    (const unsigned char *)&data[4], decryptedOut,
                                    (const unsigned char *)&data[20], TAG_SIZE);
  if (status == 0) {
    return SUCCESS;
  }
  return FAILED;
}

void emberGpdSetFcIntoTxNonce(uint32_t value)
{
  emberGpdUtilityCopy4Bytes(&nonceTx[8], value);
}

void emberGpdSetFcIntoRxNonce(uint32_t value)
{
  emberGpdUtilityCopy4Bytes(&nonceRx[8], value);
}

int8_t emberGpdMbedtlsCcmSetkeyWraper(const uint8_t* key)
{
  int status = 0;
  status = mbedtls_ccm_setkey(&mbedtls_device_ctx[device_instance],
                              MBEDTLS_CIPHER_ID_AES,
                              key,
                              128);
  if (status == 0) {
    return SUCCESS;
  }
  return FAILED;
}

int8_t emberTagTxBuff(uint8_t* data,
                      uint8_t length,
                      uint8_t authStart,
                      uint8_t authLength)
{
  int status = 0;
  status = mbedtls_ccm_encrypt_and_tag(&mbedtls_device_ctx[device_instance], 0,
                                       (const unsigned char *)nonceTx, NONCE_SIZE,
                                       (const unsigned char *)&(data[authStart]), authLength,
                                       NULL, NULL, // no encryption input, no output
                                       (data + length), // mic
                                       TAG_SIZE);
  if (status == 0) {
    return SUCCESS;
  }
  return FAILED;
}

int8_t emberEncryptAndTagTxBuff(uint8_t* data,
                                uint8_t length,
                                uint8_t payloadLength,
                                uint8_t authStart,
                                uint8_t authLength)
{
  unsigned char tempDataOut[80];
  int status;

  status = mbedtls_ccm_encrypt_and_tag(&mbedtls_device_ctx[device_instance], payloadLength,
                                       (const unsigned char *)nonceTx, NONCE_SIZE,
                                       (const unsigned char *)&(data[authStart]), authLength,
                                       &(data[authStart + authLength]), tempDataOut,
                                       (data + length), TAG_SIZE);
  (void) memcpy(&(data[authStart + authLength]), tempDataOut, payloadLength);
  if (status == 0) {
    return SUCCESS;
  }
  return FAILED;
}

int8_t emberGpdSecurityEncryptKey(EmberGpdAddr_t * addr,
                                  const uint8_t* oobKey,
                                  uint8_t* oobScured,
                                  uint8_t* mic)
{
  // temporary input buffer to hold a = (header || key)
  uint8_t data[20];
  uint32_t frameCounter;

#if defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_SRC_ID)
  // Copy SrcID address in work buffer
  (void) memcpy(data, (uint8_t *)(&(addr->id.srcId)), 4);
  // set Security Frame Counter = SrcID
  frameCounter = addr->id.srcId;
#elif defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_IEEE_ID)
  // Copy IEEE address in work buffer
  (void) memcpy(data, addr->id.ieee, 4);
  // set Security Frame Counter = 4 LSB IEEE
  frameCounter = (uint32_t)(*((uint32_t*)addr->id.ieee));
#else
#error "Application Id Not supported"
#endif
  // Copy OOb Key to be secured in the input buffer
  (void) memcpy(&data[4], oobKey, 16);

  // Update Nonce TX
  emberGpdSetFcIntoTxNonce(frameCounter);
  // Secure OOB key data with default TC_LK ZA09
  emberGpdMbedtlsCcmSetkeyWraper(keyDefaultTCLink);
  return gpdMbedtlsEncryptTagKeyWrapper(data, oobScured, mic);
}

#if TEST_KEY_DECRYPTION
//set the following as input data and the decrypted keys will be [c0 c1...cf]
uint8_t decryptedKey[16] = { 0x88 };
uint8_t inData[24] = { 0x78, 0x56, 0x34, 0x12, 0xE9, 0x00, 0x06, 0x63, 0x1D, 0x0D, 0xFD, 0xC6, 0x38, 0x06, 0x8E, 0x5E, 0x69, 0x67, 0xD3, 0x25, 0x27, 0x55, 0x9F, 0x75 };
uint8_t inFc[4] = { 0x04, 0x00, 0x00, 0x00 };
#endif
int8_t emberGpdSecurityDecryptKey(EmberGpdAddr_t * addr,
                                  uint8_t * pKeyEncrypted,
                                  uint8_t * pKeyMic,
                                  uint8_t * pSecCounter,
                                  uint8_t * decryptedKey)
{
  // local variables
  uint8_t inData[24];
  uint32_t frameCounter = *((uint32_t *)pSecCounter);

  // Copy SrcID + OOb Key in work buffer
#if defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_SRC_ID)
  (void) memcpy(&inData[0], &(addr->id.srcId), 4);
#elif defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_IEEE_ID)
  (void) memcpy(inData, &(addr->id.ieee), 4);
#else
#error "Application Id Not supported"
#endif

  (void) memcpy(&inData[4], pKeyEncrypted, 16);
  (void) memcpy(&inData[20], pKeyMic, 4);

  // Update Nonce RX with Rx Security Counter
  emberGpdSetFcIntoRxNonce(frameCounter);
  // Set security Key with default TC_LK ZA09
  emberGpdMbedtlsCcmSetkeyWraper(keyDefaultTCLink);
  return gpdMbedtlsAuthDecryptKeyWrapper(inData, decryptedKey);
}

int8_t emberDecryptRxBuffer(uint8_t *rxMpdu,
                            uint8_t authStart,
                            uint8_t encrLength,
                            uint8_t authLength)
{
  int status = 0;
  uint8_t tempDataOut[80];
  status = mbedtls_ccm_auth_decrypt(&mbedtls_device_ctx[device_instance], encrLength,
                                    (const unsigned char *)nonceRx, NONCE_SIZE,
                                    (const unsigned char *)&rxMpdu[authStart], authLength,
                                    &rxMpdu[authStart + authLength], tempDataOut,
                                    (const unsigned char *)&rxMpdu[authStart + authLength + encrLength],
                                    TAG_SIZE);

  (void) memcpy(&rxMpdu[authStart + authLength], tempDataOut, encrLength);

  if (status != 0) {
    return FAILED;
  }
  return SUCCESS;
}

void emberGpdSecurityInit(EmberGpdAddr_t * addr,
                          uint8_t* securityKey,
                          uint32_t frameCounter)
{
  device_instance = 0;
  mbedtls_ccm_init(&mbedtls_device_ctx[device_instance]);

  // Initialise nonce bytes to 0
  for (int i = 0; i < NONCE_SIZE; i++) {
    nonceRx[i] = 0;
    nonceTx[i] = 0;
  }
  // Tx Nonce set up
#if defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_SRC_ID)
  emberGpdUtilityCopy4Bytes(&nonceTx[0], addr->id.srcId);
  emberGpdUtilityCopy4Bytes(&nonceTx[4], addr->id.srcId);
#elif defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_IEEE_ID)
  //TODO : check LSB first
  (void) memcpy(&nonceTx[0], addr->id.ieee, 8);
#else
#error "Unsupported GPD Application Id"
#endif
  nonceTx[12] = ZIGBEE_SECURITY_CONTROL;

  // Rx Nonce set up
#if defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_SRC_ID)
  emberGpdUtilityCopy4Bytes(&nonceRx[0], 0);
  emberGpdUtilityCopy4Bytes(&nonceRx[4], addr->id.srcId);

  nonceRx[12] = ZIGBEE_SECURITY_CONTROL;
#elif defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_IEEE_ID)
  //TODO : check LSB first
  (void) memcpy(&nonceRx[0], addr->id.ieee, 8);
  nonceRx[12] = ZIGBEE_SECURITY_CONTROL_IEEE_RX;
#else
#error "Unsupported GPD Application Id"
#endif

  // Set the Frane counters in the Nonce
  emberGpdSetFcIntoTxNonce(frameCounter);
  emberGpdSetFcIntoRxNonce(frameCounter);
  // Load the key in the CCM context
  emberGpdMbedtlsCcmSetkeyWraper(securityKey);
}
