/*****************************************************************************/
/**
 * Copyright 2021 Silicon Laboratories, Inc.
 *
 *****************************************************************************/
//
// *** Generated file. Do not edit! ***
//
// Description: Handlers for the EZSP frames that directly correspond to Ember
// API calls.

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "ezsp-enum.h"
#include "secure-ezsp-types.h"
#include "app/em260/command-context.h"
#include "stack/include/cbke-crypto-engine.h"
#include "stack/include/mfglib.h"
#include "stack/include/binding-table.h"
#include "stack/include/message.h"
#include "app/util/ezsp/ezsp-frame-utilities.h"
#include "app/em260/command-handlers-cbke.h"
#include "app/em260/command-handlers-binding.h"
#include "app/em260/command-handlers-mfglib.h"
#include "app/em260/command-handlers-security.h"
#include "app/em260/command-handlers-zll.h"
#include "app/em260/command-handlers-zigbee-pro.h"
#include "child.h"
#include "message.h"
#include "zll-api.h"
#include "security.h"
#include "stack-info.h"
#include "network-formation.h"
#include "zigbee-device-stack.h"
#include "ember-duty-cycle.h"
#include "multi-phy.h"

bool emAfProcessEzspCommandCertificateBasedKeyExchangeCbke(uint16_t commandId)
{
  switch (commandId) {
//------------------------------------------------------------------------------

    case EZSP_GENERATE_CBKE_KEYS: {
      EmberStatus status;
      status = emberGenerateCbkeKeys();
      appendInt8u(status);
      break;
    }

    case EZSP_CALCULATE_SMACS: {
      EmberStatus status;
      bool amInitiator;
      EmberCertificateData partnerCertificate;
      EmberPublicKeyData partnerEphemeralPublicKey;
      amInitiator = fetchInt8u();
      fetchEmberCertificateData(&partnerCertificate);
      fetchEmberPublicKeyData(&partnerEphemeralPublicKey);
      status = emberCalculateSmacs(amInitiator, &partnerCertificate, &partnerEphemeralPublicKey);
      appendInt8u(status);
      break;
    }

    case EZSP_GENERATE_CBKE_KEYS283K1: {
      EmberStatus status;
      status = emberGenerateCbkeKeys283k1();
      appendInt8u(status);
      break;
    }

    case EZSP_CALCULATE_SMACS283K1: {
      EmberStatus status;
      bool amInitiator;
      EmberCertificate283k1Data partnerCertificate;
      EmberPublicKey283k1Data partnerEphemeralPublicKey;
      amInitiator = fetchInt8u();
      fetchEmberCertificate283k1Data(&partnerCertificate);
      fetchEmberPublicKey283k1Data(&partnerEphemeralPublicKey);
      status = emberCalculateSmacs283k1(amInitiator, &partnerCertificate, &partnerEphemeralPublicKey);
      appendInt8u(status);
      break;
    }

    case EZSP_CLEAR_TEMPORARY_DATA_MAYBE_STORE_LINK_KEY: {
      EmberStatus status;
      bool storeLinkKey;
      storeLinkKey = fetchInt8u();
      status = emberClearTemporaryDataMaybeStoreLinkKey(storeLinkKey);
      appendInt8u(status);
      break;
    }

    case EZSP_CLEAR_TEMPORARY_DATA_MAYBE_STORE_LINK_KEY283K1: {
      EmberStatus status;
      bool storeLinkKey;
      storeLinkKey = fetchInt8u();
      status = emberClearTemporaryDataMaybeStoreLinkKey283k1(storeLinkKey);
      appendInt8u(status);
      break;
    }

    case EZSP_GET_CERTIFICATE: {
      EmberStatus status;
      EmberCertificateData localCert;
      status = emberGetCertificate(&localCert);
      appendInt8u(status);
      appendEmberCertificateData(&localCert);
      break;
    }

    case EZSP_GET_CERTIFICATE283K1: {
      EmberStatus status;
      EmberCertificate283k1Data localCert;
      status = emberGetCertificate283k1(&localCert);
      appendInt8u(status);
      appendEmberCertificate283k1Data(&localCert);
      break;
    }

    case EZSP_DSA_SIGN: {
      EmberStatus status;
      uint8_t messageLength;
      uint8_t *messageContents;
      messageLength = fetchInt8u();
      messageContents = (uint8_t *)fetchInt8uPointer(messageLength);
      status = emberAfEzspDsaSignCommandCallback(messageLength, messageContents);
      appendInt8u(status);
      break;
    }

    case EZSP_DSA_VERIFY: {
      EmberStatus status;
      EmberMessageDigest digest;
      EmberCertificateData signerCertificate;
      EmberSignatureData receivedSig;
      fetchEmberMessageDigest(&digest);
      fetchEmberCertificateData(&signerCertificate);
      fetchEmberSignatureData(&receivedSig);
      status = emberDsaVerify(&digest, &signerCertificate, &receivedSig);
      appendInt8u(status);
      break;
    }

    case EZSP_DSA_VERIFY283K1: {
      EmberStatus status;
      EmberMessageDigest digest;
      EmberCertificate283k1Data signerCertificate;
      EmberSignature283k1Data receivedSig;
      fetchEmberMessageDigest(&digest);
      fetchEmberCertificate283k1Data(&signerCertificate);
      fetchEmberSignature283k1Data(&receivedSig);
      status = emberDsaVerify283k1(&digest, &signerCertificate, &receivedSig);
      appendInt8u(status);
      break;
    }

    case EZSP_SET_PREINSTALLED_CBKE_DATA: {
      EmberStatus status;
      EmberPublicKeyData caPublic;
      EmberCertificateData myCert;
      EmberPrivateKeyData myKey;
      fetchEmberPublicKeyData(&caPublic);
      fetchEmberCertificateData(&myCert);
      fetchEmberPrivateKeyData(&myKey);
      status = emberSetPreinstalledCbkeData(&caPublic, &myCert, &myKey);
      appendInt8u(status);
      break;
    }

    case EZSP_SAVE_PREINSTALLED_CBKE_DATA283K1: {
      EmberStatus status;
      status = emberAfEzspSavePreinstalledCbkeData283k1CommandCallback();
      appendInt8u(status);
      break;
    }

//------------------------------------------------------------------------------

    default: {
      return false;
    }
  }

  return true;
}
