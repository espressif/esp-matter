/***************************************************************************//**
 * @file
 * @brief This file implements the routines for storing temporary data that
 * is needed for key establishment.  This is data is completely
 * public and is sent over-the-air and thus not required to be
 * closely protected.
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

// this file contains all the common includes for clusters in the zcl-util
#include "../../util/common.h"

#include "key-establishment-storage.h"
#include "stack/include/cbke-crypto-engine.h"

//------------------------------------------------------------------------------
// Globals

static EmberMessageBuffer smacBuffer             = EMBER_NULL_MESSAGE_BUFFER;
static EmberMessageBuffer certBuffer283k1        = EMBER_NULL_MESSAGE_BUFFER;

#define CERTIFICATE_OFFSET 0
#define PUBLIC_KEY_OFFSET  EMBER_CERTIFICATE_SIZE
#define PUBLIC_KEY_283K1_OFFSET EMBER_CERTIFICATE_283K1_SIZE
//------------------------------------------------------------------------------
// Forward Declarations

static void releaseAndNullBuffer(EmberMessageBuffer* buffer);

//------------------------------------------------------------------------------

bool storePublicPartnerData163k1(bool isCertificate,
                                 uint8_t* data)
{
  // The expectation is that the certificate must be stored first
  // and the public key is stored second.  The first time this is called
  // the buffer should be null while second time around it should not be.
  if (isCertificate
      ? certBuffer283k1 != EMBER_NULL_MESSAGE_BUFFER
      : certBuffer283k1 == EMBER_NULL_MESSAGE_BUFFER) {
    return false;
  }
  if (isCertificate) {
    certBuffer283k1 = emberFillLinkedBuffers(data,
                                             EMBER_CERTIFICATE_SIZE);
    if ( certBuffer283k1 == EMBER_NULL_MESSAGE_BUFFER ) {
      return false;
    }
  } else {
    if (EMBER_SUCCESS
        != emberAppendToLinkedBuffers(certBuffer283k1,
                                      data,
                                      EMBER_PUBLIC_KEY_SIZE)) {
      releaseAndNullBuffer(&certBuffer283k1);
      return false;
    }
  }
  return true;
}

bool retrieveAndClearPublicPartnerData163k1(EmberCertificateData* partnerCertificate,
                                            EmberPublicKeyData* partnerEphemeralPublicKey)
{
  uint8_t length = emberMessageBufferLength(certBuffer283k1);
  if ( certBuffer283k1 == EMBER_NULL_MESSAGE_BUFFER ) {
    return false;
  }

  if ((EMBER_CERTIFICATE_SIZE + EMBER_PUBLIC_KEY_SIZE) > length) {
    return false;
  }
  emberCopyFromLinkedBuffers(certBuffer283k1,
                             CERTIFICATE_OFFSET,
                             emberCertificateContents(partnerCertificate),
                             EMBER_CERTIFICATE_SIZE);

  emberCopyFromLinkedBuffers(certBuffer283k1,
                             PUBLIC_KEY_OFFSET,
                             emberPublicKeyContents(partnerEphemeralPublicKey),
                             EMBER_PUBLIC_KEY_SIZE);

  releaseAndNullBuffer(&certBuffer283k1);
  return true;
}

bool storePublicPartnerData283k1(bool isCertificate,
                                 uint8_t* data)
{
  if (isCertificate
      ? certBuffer283k1 != EMBER_NULL_MESSAGE_BUFFER
      : certBuffer283k1 == EMBER_NULL_MESSAGE_BUFFER) {
    return false;
  }

  if (isCertificate) {
    certBuffer283k1 = emberFillLinkedBuffers(data,
                                             EMBER_CERTIFICATE_283K1_SIZE);
    if (certBuffer283k1 == EMBER_NULL_MESSAGE_BUFFER) {
      return false;
    }
  } else {
    if (EMBER_SUCCESS
        != emberAppendToLinkedBuffers(certBuffer283k1,
                                      data,
                                      EMBER_PUBLIC_KEY_283K1_SIZE)) {
      releaseAndNullBuffer(&certBuffer283k1);
      return false;
    }
  }
  return true;
}

bool retrieveAndClearPublicPartnerData283k1(EmberCertificate283k1Data* partnerCertificate,
                                            EmberPublicKey283k1Data* partnerEphemeralPublicKey)
{
  uint8_t length = emberMessageBufferLength(certBuffer283k1);
  if ( certBuffer283k1 == EMBER_NULL_MESSAGE_BUFFER ) {
    return false;
  }
  if ((EMBER_CERTIFICATE_283K1_SIZE + EMBER_PUBLIC_KEY_283K1_SIZE) > length) {
    return false;
  }
  emberCopyFromLinkedBuffers(certBuffer283k1,
                             0, // Offset
                             emberCertificate283k1Contents(partnerCertificate),
                             EMBER_CERTIFICATE_283K1_SIZE);

  emberCopyFromLinkedBuffers(certBuffer283k1,
                             PUBLIC_KEY_283K1_OFFSET,
                             emberPublicKey283k1Contents(partnerEphemeralPublicKey),
                             EMBER_PUBLIC_KEY_283K1_SIZE);

  releaseAndNullBuffer(&certBuffer283k1);
  return true;
}

bool storeSmac(EmberSmacData* smac)
{
  if ( smacBuffer != EMBER_NULL_MESSAGE_BUFFER ) {
    emberReleaseMessageBuffer(smacBuffer);
  }
  emberAfKeyEstablishmentClusterPrintln("Storing SMAC");
  emberAfPrintZigbeeKey(emberKeyContents(smac));
  smacBuffer = emberFillLinkedBuffers(emberSmacContents(smac),
                                      EMBER_SMAC_SIZE);
  if ( smacBuffer == EMBER_NULL_MESSAGE_BUFFER ) {
    return false;
  }
  return true;
}

bool getSmacPointer(EmberSmacData** smacPtr)
{
  if ( smacBuffer == EMBER_NULL_MESSAGE_BUFFER ) {
    return false;
  }

  *smacPtr = (EmberSmacData*)emberMessageBufferContents(smacBuffer);

  return true;
}

void clearAllTemporaryPublicData(void)
{
  EmberMessageBuffer* buffer = &certBuffer283k1;
  uint8_t i;
  for ( i = 0; i < 2; i++ ) {
    if ( *buffer != EMBER_NULL_MESSAGE_BUFFER ) {
      releaseAndNullBuffer(buffer);
    }
    buffer = &smacBuffer;
  }
}

static void releaseAndNullBuffer(EmberMessageBuffer* buffer)
{
  emberReleaseMessageBuffer(*buffer);
  *buffer = EMBER_NULL_MESSAGE_BUFFER;
}

void emMarkKeyEstablishmentBuffers(void)
{
  emMarkBuffer(&smacBuffer);
  emMarkBuffer(&certBuffer283k1);
}
//------------------------------------------------------------------------------
