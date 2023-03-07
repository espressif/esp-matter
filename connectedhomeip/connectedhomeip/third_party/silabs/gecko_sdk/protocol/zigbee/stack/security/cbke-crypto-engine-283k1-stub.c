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

const EmberLibraryStatus emCbke283k1LibraryStatus = EMBER_LIBRARY_IS_STUB;

bool emRealECCLibraryPresent283k1(void)
{
  return false;
}

EmberStatus emberCalculateSmacs283k1(bool amInitiator,
                                     EmberCertificate283k1Data* partnerCert,
                                     EmberPublicKey283k1Data* partnerEphemeralPublicKey)
{
  (void)amInitiator;
  (void)partnerCert;
  (void)partnerEphemeralPublicKey;
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus emberGenerateCbkeKeys283k1(void)
{
  return EMBER_LIBRARY_NOT_PRESENT;
}

void generateCbkeKeys283k1(void)
{
}

void calculateSmacs283k1(void)
{
}

void invalidatePartnerData283k1(void)
{
}

void emInitializeCbke283k1(void)
{
}

EmberStatus emberGetCertificate283k1(EmberCertificate283k1Data* result)
{
  (void)result;
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus emberSetPreinstalledCbkeData283k1(EmberPublicKey283k1Data* caPublic,
                                              EmberCertificate283k1Data* myCert,
                                              EmberPrivateKey283k1Data* myKey)
{
  (void)caPublic;
  (void)myCert;
  (void)myKey;
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus emGetCbkeData283k1(EmberCertificate283k1Data* myCertPtr,
                               EmberPrivateKey283k1Data* myStaticPrivateKeyPtr,
                               EmberPublicKey283k1Data* caPublicKeyPtr)
{
  (void)myCertPtr;
  (void)myStaticPrivateKeyPtr;
  (void)caPublicKeyPtr;
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus emberClearTemporaryDataMaybeStoreLinkKey283k1(bool storeLinkKey)
{
  (void)storeLinkKey;
  return EMBER_LIBRARY_NOT_PRESENT;
}
