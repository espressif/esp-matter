/***************************************************************************//**
 * @file
 * @brief This file calls ECC curve specific functions from the cbke library
 * based on whether the 163k1 ECC curve or the 283k1 ECC curve is
 * being used for key establishment.
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

#include "key-establishment.h"
#include "key-establishment-storage.h"
#ifndef EZSP_HOST
  #include "stack/include/cbke-crypto-engine.h"
#endif

bool checkIssuer(uint8_t *issuer)
{
  uint8_t *ourIssuer;

  if (isCbkeKeyEstablishmentSuite163k1()) {
    EmberCertificateData localCert;
    if (emberGetCertificate(&localCert) != EMBER_SUCCESS) {
      emberAfKeyEstablishmentClusterPrintln("Could not retrieve certificate");
      cleanupAndStop(NO_LOCAL_RESOURCES);
      return false;
    }
    ourIssuer = &(localCert.contents[CERT_ISSUER_OFFSET]);
  } else if (isCbkeKeyEstablishmentSuite283k1()) {
    EmberCertificate283k1Data localCert;
    if (emberGetCertificate283k1(&localCert) != EMBER_SUCCESS) {
      emberAfKeyEstablishmentClusterPrintln("Could not retrieve 283k1 certificate");
      cleanupAndStop(NO_LOCAL_RESOURCES);
      return false;
    }
    ourIssuer = &(localCert.contents[CERT_ISSUER_OFFSET_283K1]);
  } else {
    //No valid Suite.
    return false;
  }

  if (MEMCOMPARE(issuer, ourIssuer, CERT_ISSUER_SIZE) == 0) {
    emberAfKeyEstablishmentClusterPrintln("Good Certificate Issuer");
    return true;
  }
  emberAfKeyEstablishmentClusterPrintln("Bad Certificate Issuer");
  cleanupAndStop(BAD_CERTIFICATE_ISSUER);
  return false;
}

bool storePublicPartnerData(bool isCertificate,
                            uint8_t* data)
{
  if (isCbkeKeyEstablishmentSuite163k1()) {
    return storePublicPartnerData163k1(isCertificate, data);
  } else if (isCbkeKeyEstablishmentSuite283k1()) {
    return storePublicPartnerData283k1(isCertificate, data);
  } else {
    //No valid suite
    return false;
  }
}

bool retrieveAndClearPublicPartnerData(EmberCertificate283k1Data* partnerCertificate,
                                       EmberPublicKey283k1Data* partnerEphemeralPublicKey)
{
  if (isCbkeKeyEstablishmentSuite163k1()) {
    return retrieveAndClearPublicPartnerData163k1((EmberCertificateData *)emberCertificate283k1Contents(partnerCertificate),
                                                  (EmberPublicKeyData *)emberPublicKey283k1Contents(partnerEphemeralPublicKey));
  } else if (isCbkeKeyEstablishmentSuite283k1()) {
    return retrieveAndClearPublicPartnerData283k1(partnerCertificate, partnerEphemeralPublicKey);
  } else {
    //No valid suite
    return false;
  }
}

EmberAfKeyEstablishmentNotifyMessage sendCertificate(void)
{
  if (isCbkeKeyEstablishmentSuite163k1()) {
    EmberCertificateData localCert;
    if (emberGetCertificate(&localCert) != EMBER_SUCCESS) {
      emberAfKeyEstablishmentClusterPrintln("Failed to retrieve certificate.");
      return NO_LOCAL_RESOURCES;
    }
    sendNextKeyEstablishMessage(ZCL_INITIATE_KEY_ESTABLISHMENT_REQUEST_COMMAND_ID,
                                emberCertificateContents(&localCert));
  } else if (isCbkeKeyEstablishmentSuite283k1()) {
    EmberCertificate283k1Data localCert;
    if (emberGetCertificate283k1(&localCert) != EMBER_SUCCESS) {
      emberAfKeyEstablishmentClusterPrintln("Failed to retrieve 283k1 certificate.");
      return NO_LOCAL_RESOURCES;
    }
    sendNextKeyEstablishMessage(ZCL_INITIATE_KEY_ESTABLISHMENT_REQUEST_COMMAND_ID,
                                emberCertificate283k1Contents(&localCert));
  } else {
    //Closest error message
    return NO_LOCAL_RESOURCES;
  }

  return NO_APP_MESSAGE;
}

EmberStatus emGenerateCbkeKeysForCurve(void)
{
  EmberStatus status;
  if (isCbkeKeyEstablishmentSuite163k1()) {
    status = emberGenerateCbkeKeys();
  } else if (isCbkeKeyEstablishmentSuite283k1()) {
    status = emberGenerateCbkeKeys283k1();
  } else {
    status = EMBER_ERR_FATAL;
  }
  return status;
}

EmberStatus emCalculateSmacsForCurve(bool amInitiator,
                                     EmberCertificate283k1Data* partnerCert,
                                     EmberPublicKey283k1Data* partnerEphemeralPublicKey)
{
  EmberStatus status;
  emberAfKeyEstablishmentClusterPrintln("calculateSmacs()");
  if (isCbkeKeyEstablishmentSuite163k1()) {
    status = emberCalculateSmacs(amInitiator,
                                 (EmberCertificateData *)emberCertificate283k1Contents(partnerCert),
                                 (EmberPublicKeyData *)emberPublicKey283k1Contents(partnerEphemeralPublicKey));
  } else if (isCbkeKeyEstablishmentSuite283k1()) {
    status = emberCalculateSmacs283k1(amInitiator,
                                      partnerCert,
                                      partnerEphemeralPublicKey);
  } else {
    status = EMBER_ERR_FATAL;
  }
  return status;
}

EmberStatus emberClearTemporaryDataMaybeStoreLinkKeyForCurve(bool storeLinkKey)
{
  EmberStatus status;
  if ( isCbkeKeyEstablishmentSuite163k1() ) {
    status = emberClearTemporaryDataMaybeStoreLinkKey(storeLinkKey);
  } else if ( isCbkeKeyEstablishmentSuite283k1()) {
    status = emberClearTemporaryDataMaybeStoreLinkKey283k1(storeLinkKey);
  } else {
    status = EMBER_ERR_FATAL;
  }
  return status;
}
