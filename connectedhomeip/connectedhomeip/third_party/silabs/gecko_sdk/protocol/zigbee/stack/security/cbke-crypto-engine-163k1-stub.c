/***************************************************************************//**
 * @file
 * @brief Stub version of the Certificate Based Key Exchange library.  See non-stub
 * file for more information.
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

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/library.h"

const EmberLibraryStatus emCbkeLibraryStatus = EMBER_LIBRARY_IS_STUB;

bool emRealECCLibraryPresent(void)
{
  return false;
}

EmberStatus emberCalculateSmacs(bool amInitiator,
                                EmberCertificateData* partnerCert,
                                EmberPublicKeyData* partnerEphemeralPublicKey)
{
  (void)amInitiator;
  (void)partnerCert;
  (void)partnerEphemeralPublicKey;
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus emberGenerateCbkeKeys(void)
{
  return EMBER_LIBRARY_NOT_PRESENT;
}

void generateCbkeKeys(void)
{
}

void calculateSmacs(void)
{
}

EmberStatus reallyCalculateSmacs(bool amInitiator,
                                 EmberCertificateData* partnerCert,
                                 EmberPublicKeyData* partnerEphemeralPublicKey,
                                 EmberSmacData* initiatorSmacReturn,
                                 EmberSmacData* responderSmacReturn)
{
  (void)amInitiator;
  (void)partnerCert;
  (void)partnerEphemeralPublicKey;
  (void)initiatorSmacReturn;
  (void)responderSmacReturn;
  return EMBER_LIBRARY_NOT_PRESENT;
}

void deriveMacKeyAndKeyData(bool amInitiator,
                            uint8_t* sharedSecret,
                            uint8_t* macKeyResult,
                            uint8_t* keyDataResult)
{
  (void)amInitiator;
  (void)sharedSecret;
  (void)macKeyResult;
  (void)keyDataResult;
}

void generateMacFromKey(bool amInitiator,
                        bool deriveAsInitiator,
                        uint8_t* macKey,
                        uint8_t* mac)
{
  (void)amInitiator;
  (void)deriveAsInitiator;
  (void)macKey;
  (void)mac;
}

void invalidatePartnerData(void)
{
}

// From cbke-crypto-engine-storage-{platform}.c
void emInitializeCbke(void)
{
}

EmberStatus emberSetPreinstalledCbkeData(EmberPublicKeyData* caPublic,
                                         EmberCertificateData* myCert,
                                         EmberPrivateKeyData* myKey)
{
  (void)caPublic;
  (void)myCert;
  (void)myKey;
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus emberGetCertificate(EmberCertificateData* result)
{
  (void)result;
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus emGetCbkeData(EmberCertificateData* myCertPtr,
                          EmberPrivateKeyData* myStaticPrivateKeyPtr,
                          EmberPublicKeyData* caPublicKeyPtr)
{
  (void)myCertPtr;
  (void)myStaticPrivateKeyPtr;
  (void)caPublicKeyPtr;
  return EMBER_LIBRARY_NOT_PRESENT;
}

bool emberGetStackCertificateEui64(EmberEUI64 certEui64)
{
  (void)certEui64;
  return false;
}

EmberStatus emRetrieveAndClearUnverifiedLinkKey(EmberKeyData* key,
                                                EmberEUI64 ieee)
{
  (void)key;
  (void)ieee;
  return EMBER_LIBRARY_NOT_PRESENT;
}

void emClearAllTemporaryCryptoData(void)
{
}

EmberStatus emberClearTemporaryDataMaybeStoreLinkKey(bool storeLinkKey)
{
  (void)storeLinkKey;
  return EMBER_LIBRARY_NOT_PRESENT;
}
