/***************************************************************************//**
 * @file
 * @brief Functionality specific to verifying a DSA signature at the end of a
 * message.
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

const EmberLibraryStatus emCbkeDsaVerifyLibraryStatus = EMBER_LIBRARY_IS_STUB;

EmberStatus emberDsaVerify(EmberMessageDigest* digest,
                           EmberCertificateData* signerCertificate,
                           EmberSignatureData* receivedSig)
{
  (void)digest;
  (void)signerCertificate;
  (void)receivedSig;
  return EMBER_LIBRARY_NOT_PRESENT;
}

void emDsaVerify(void)
{
}
