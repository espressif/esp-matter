/***************************************************************************//**
 * @file
 * @brief Code for backing up and restoring a trust center.
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

#include "app/framework/util/af-main.h"
#include "app/framework/security/af-security.h"

// *****************************************************************************
// Globals

// *****************************************************************************
// Functions

EmberStatus emberTrustCenterExportBackupData(EmberAfTrustCenterBackupData* backup)
{
  EmberNodeType nodeType;
  EmberNetworkParameters params;
  uint8_t i;
  uint8_t keyTableSize = emberAfGetKeyTableSize();
  backup->keyListLength = 0;

  if (backup->maxKeyListLength < keyTableSize) {
    return EMBER_TABLE_FULL;
  }

  if (emberAfGetNodeId() != EMBER_TRUST_CENTER_NODE_ID) {
    return EMBER_INVALID_CALL;
  }

  emberAfGetNetworkParameters(&nodeType,
                              &params);
  MEMMOVE(backup->extendedPanId,
          params.extendedPanId,
          EUI64_SIZE);

  for (i = 0; i < keyTableSize; i++) {
    EmberKeyStruct keyStruct;
    EmberStatus status = emberGetKeyTableEntry(i, &keyStruct);
    if (status == EMBER_SUCCESS) {
      MEMMOVE(backup->keyList[backup->keyListLength].deviceId,
              keyStruct.partnerEUI64,
              EUI64_SIZE);
      // Rather than give the real link key, the backup
      // contains a hashed version of the key.  This is done
      // to prevent a compromise of the backup data from compromising
      // the current link keys.  This is per the Smart Energy spec.
      emberAesHashSimple(EMBER_ENCRYPTION_KEY_SIZE,
                         emberKeyContents(&(keyStruct.key)),
                         emberKeyContents(&(backup->keyList[backup->keyListLength].key)));
      backup->keyListLength++;
    }
  }
  return EMBER_SUCCESS;
}

EmberStatus emberTrustCenterImportBackupAndStartNetwork(const EmberAfTrustCenterBackupData* backup)
{
  // 1. Check that the network is down.
  // 2. Add keys
  // 3. Form the network.
  // 4. Profit!

  uint8_t i;
  uint8_t keyTableSize = emberAfGetKeyTableSize();
  EmberStatus status;

  if (EMBER_NO_NETWORK != emberAfNetworkState()) {
    emberAfSecurityPrintln("%p: Cannot import TC data while network is up.");
    return EMBER_INVALID_CALL;
  }

  if (backup->keyListLength > keyTableSize) {
    emberAfSecurityPrintln("%p: Current key table of %d too small for import of backup (%d)!",
                           "Error",
                           keyTableSize,
                           backup->keyListLength);
    return EMBER_ERR_FATAL;
  }

  for (i = 0; i < keyTableSize; i++) {
    if (i >= backup->keyListLength) {
      status = emberEraseKeyTableEntry(i);
    } else {
      // Copy key data to a local to get around compiler warning about
      // passing "const" data into 'emberSetKeyTableEntry()'
      EmberKeyData key;
      MEMMOVE(emberKeyContents(&key),
              emberKeyContents(&(backup->keyList[i].key)),
              EMBER_ENCRYPTION_KEY_SIZE);

      status = emberSetKeyTableEntry(i,
                                     backup->keyList[i].deviceId,
                                     true,  // link key?
                                     &key);
    }

    if (status != EMBER_SUCCESS) {
      emberAfSecurityPrintln("%p: Failed to %p key table entry at index %d: 0%X",
                             "Error",
                             ((i >= backup->keyListLength)
                              ? "erase"
                              : "set"),
                             i,
                             status);
      emberAfSecurityPrintln("TC Import failed");
      return status;
    }
  }

  emberAfSecurityPrintln("Imported %d keys",
                         backup->keyListLength);

  // Disable clearing the link key table.
  emberAfClearLinkKeyTableUponFormingOrJoining = false;

  // This EUI64 is used by the Network-find plugin when forming a network.
  emberAfSetFormAndJoinExtendedPanIdCallback(backup->extendedPanId);
  emberAfSecurityPrintln("Starting search for unused short PAN ID");
  status = emberAfFindUnusedPanIdAndFormCallback();
  if (status != EMBER_SUCCESS) {
    emberAfSecurityPrintln("Failed to start PAN ID search.");
  }
  return status;
}
