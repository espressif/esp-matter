/***************************************************************************//**
 * @file
 * @brief Application interface for ember bootloader
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "em_core.h"
#include "em_device.h"
#include "em_emu.h"
#include "em_chip.h"

#include "ember_btl_interface.h"
#include "btl_errorcode.h"

extern const BootloaderAddressTable_t *bootloaderAddressTable;

int32_t bootloader_init(void)
{
#if SL_EMBER_BOOTLOADER_TYPE == SL_EMBER_BOOTLOADER_TYPE_APPLICATION
  if (bootloaderAddressTable->init() != BOOTLOADER_OK) {
    return BOOTLOADER_ERROR_INIT_STORAGE;
  }
#endif

  return BOOTLOADER_OK;
}

void bootloader_getInfo(BootloaderInformation_t *info)
{
#if SL_EMBER_BOOTLOADER_TYPE == SL_EMBER_BOOTLOADER_TYPE_APPLICATION
  BootloaderInformation_t *storageInfo;
  storageInfo = (BootloaderInformation_t *)bootloaderAddressTable->storageInfo();

  info->version = storageInfo->version;
  info->capabilitiesMask = storageInfo->capabilitiesMask;
  info->pageEraseMs = storageInfo->pageEraseMs;
  info->partEraseTime = storageInfo->partEraseTime;
  info->pageSize = storageInfo->pageSize;
  info->partSize = storageInfo->partSize;
  info->wordSizeBytes = storageInfo->wordSizeBytes;
#elif SL_EMBER_BOOTLOADER_TYPE == SL_EMBER_BOOTLOADER_TYPE_STANDALONE
  info->version = bootloaderAddressTable->bootloaderVersion;
#endif
}

int32_t bootloader_deinit(void)
{
#if SL_EMBER_BOOTLOADER_TYPE == SL_EMBER_BOOTLOADER_TYPE_APPLICATION
  bootloaderAddressTable->deinit();
#endif

  return BOOTLOADER_OK;
}

void bootloader_rebootAndInstall(void)
{
  CORE_ATOMIC_IRQ_DISABLE();

  // Set reset reason to bootloader entry
  BootloaderResetCause_t* resetCause = (BootloaderResetCause_t*) (RAM_MEM_BASE);
  resetCause->reason = EMBER_BOOTLOADER_RESET_REASON_BOOTLOAD;
  resetCause->signature = EMBER_BOOTLOADER_RESET_SIGNATURE_VALID;

#if defined(RMU_PRESENT)
  // Clear resetcause
  RMU->CMD = RMU_CMD_RCCLR;
  // Trigger a software system reset
  RMU->CTRL = (RMU->CTRL & ~_RMU_CTRL_SYSRMODE_MASK) | RMU_CTRL_SYSRMODE_FULL;
#endif

  CHIP_Reset();
}

#if SL_EMBER_BOOTLOADER_TYPE == SL_EMBER_BOOTLOADER_TYPE_APPLICATION
static uint8_t buff[EEPROM_PAGE_SIZE];
BootloaderStorageState_t storageState;
EblConfig_t eblConfig;
#endif

#if SL_EMBER_BOOTLOADER_TYPE == SL_EMBER_BOOTLOADER_TYPE_APPLICATION
int32_t bootloader_initVerifyImage(uint32_t slotId, void *context, size_t contextSize)
{
  (void)slotId;
  (void)context;
  (void)contextSize;

  storageState.address = 0;
  storageState.pages = 0;
  storageState.pageBufFinger = 0;
  storageState.pageBufLen = 0;
  bootloaderAddressTable->eblProcessInit(&eblConfig,
                                         &storageState,
                                         buff,
                                         EBL_MIN_TAG_SIZE,
                                         true);

  return BOOTLOADER_OK;
}
#endif

#if SL_EMBER_BOOTLOADER_TYPE == SL_EMBER_BOOTLOADER_TYPE_APPLICATION
int32_t bootloader_continueVerifyImage(void *context, BootloaderParserCallback_t metadataCallback)
{
  (void)context;
  (void)metadataCallback;

  uint8_t status;
  status = bootloaderAddressTable->eblProcess(bootloaderAddressTable->eblDataFunctions,
                                              &eblConfig,
                                              NULL);

  if (status == EMBER_BOOTLOADER_PARSE_CONTINUE) {
    return BOOTLOADER_ERROR_PARSE_CONTINUE;
  } else if (status == BOOTLOADER_OK) {
    return BOOTLOADER_ERROR_PARSE_SUCCESS;
  } else {
    return BOOTLOADER_ERROR_PARSE_FAILED;
  }
}
#endif

#if SL_EMBER_BOOTLOADER_TYPE == SL_EMBER_BOOTLOADER_TYPE_APPLICATION
int32_t bootloader_verifyImage(uint32_t slotId, BootloaderParserCallback_t metadataCallback)
{
  (void)slotId;
  (void)metadataCallback;

  int32_t retval;

  void *context = NULL;
  size_t contextSize = 0;
  bootloader_initVerifyImage(slotId, context, contextSize);

  do {
    retval = bootloader_continueVerifyImage(context, metadataCallback);
  } while (retval == BOOTLOADER_ERROR_PARSE_CONTINUE);

  if (retval == BOOTLOADER_ERROR_PARSE_SUCCESS) {
    return BOOTLOADER_OK;
  } else {
    return retval;
  }
}
#endif

#if SL_EMBER_BOOTLOADER_TYPE == SL_EMBER_BOOTLOADER_TYPE_APPLICATION
bool bootloader_storageIsBusy(void)
{
  return bootloaderAddressTable->storageBusy();
}
#endif

#if SL_EMBER_BOOTLOADER_TYPE == SL_EMBER_BOOTLOADER_TYPE_APPLICATION
int32_t bootloader_readRawStorage(uint32_t address, uint8_t *buffer, size_t length)
{
  if (bootloaderAddressTable->storageReadRaw(address, buffer, length) == BOOTLOADER_OK) {
    return BOOTLOADER_OK;
  } else {
    return BOOTLOADER_ERROR_STORAGE_GENERIC;
  }
}
#endif

#if SL_EMBER_BOOTLOADER_TYPE == SL_EMBER_BOOTLOADER_TYPE_APPLICATION
int32_t bootloader_writeRawStorage(uint32_t address, uint8_t *buffer, size_t length)
{
  if (bootloaderAddressTable->storageReadRaw(address, buffer, length) == BOOTLOADER_OK) {
    return BOOTLOADER_OK;
  } else {
    return BOOTLOADER_ERROR_STORAGE_GENERIC;
  }
}
#endif

#if SL_EMBER_BOOTLOADER_TYPE == SL_EMBER_BOOTLOADER_TYPE_APPLICATION
int32_t bootloader_eraseRawStorage(uint32_t address, size_t length)
{
  if (bootloaderAddressTable->storageEraseRaw(address, length) == BOOTLOADER_OK) {
    return BOOTLOADER_OK;
  } else {
    return BOOTLOADER_ERROR_STORAGE_GENERIC;
  }
}
#endif
