/***************************************************************************//**
 * @file
 * @brief Application interface to the bootloader.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "btl_interface.h"

// -----------------------------------------------------------------------------
// Veneer NSC functions

extern void bootloader_nsc_getInfo(BootloaderInformation_t *info);
extern int32_t bootloader_nsc_init(void);
extern int32_t bootloader_nsc_deinit(void);
extern void bootloader_nsc_rebootAndInstall(void);
extern int32_t bootloader_nsc_parseImageInfo(uint8_t           data[],
                                             size_t            numBytes,
                                             ApplicationData_t *appInfo,
                                             uint32_t          *bootloaderVersion);
extern uint32_t bootloader_nsc_parserContextSize(void);
extern bool bootloader_nsc_verifyApplication(uint32_t startAddress);
extern bool bootloader_nsc_secureBootEnforced(void);
extern uint32_t bootloader_nsc_remainingApplicationUpgrades(void);
extern bool bootloader_nsc_getCertificateVersion(uint32_t *version);
extern void bootloader_nsc_getStorageInfo(BootloaderStorageInformation_t *info);
extern int32_t bootloader_nsc_getStorageSlotInfo(uint32_t                slotId,
                                                 BootloaderStorageSlot_t *slot);
extern int32_t bootloader_nsc_readStorage(uint32_t slotId,
                                          uint32_t offset,
                                          uint8_t  *buffer,
                                          size_t   length);
extern int32_t bootloader_nsc_writeStorage(uint32_t slotId,
                                           uint32_t offset,
                                           uint8_t  *buffer,
                                           size_t   length);
extern int32_t bootloader_nsc_eraseWriteStorage(uint32_t slotId,
                                                uint32_t offset,
                                                uint8_t  *buffer,
                                                size_t   length);
extern int32_t bootloader_nsc_eraseStorageSlot(uint32_t slotId);
extern int32_t bootloader_nsc_initChunkedEraseStorageSlot(uint32_t                slotId,
                                                          BootloaderEraseStatus_t *eraseStat);
extern int32_t bootloader_nsc_chunkedEraseStorageSlot(BootloaderEraseStatus_t *eraseStat);
extern int32_t bootloader_nsc_setImageToBootload(int32_t slotId);
extern int32_t bootloader_nsc_setImagesToBootload(int32_t *slotIds, size_t length);
extern int32_t bootloader_nsc_getImagesToBootload(int32_t *slotIds, size_t length);
extern int32_t bootloader_nsc_appendImageToBootloadList(int32_t slotId);
extern int32_t bootloader_nsc_initVerifyImage(uint32_t slotId);
extern int32_t bootloader_nsc_continueVerifyImage(void);
extern int32_t bootloader_nsc_verifyImage(uint32_t slotId);
extern int32_t bootloader_nsc_getImageInfo(uint32_t          slotId,
                                           ApplicationData_t *appInfo,
                                           uint32_t          *bootloaderVersion);
extern bool bootloader_nsc_storageIsBusy(void);
extern int32_t bootloader_nsc_getAllocatedDMAChannel(void);
extern BootloaderResetCause_t bootloader_nsc_getResetReason(void);

// -----------------------------------------------------------------------------
// Functions

BootloaderResetCause_t bootloader_getResetReason(void)
{
  return bootloader_nsc_getResetReason();
}

void bootloader_getInfo(BootloaderInformation_t *info)
{
  bootloader_nsc_getInfo(info);
}

int32_t bootloader_init(void)
{
  return bootloader_nsc_init();
}

int32_t bootloader_deinit(void)
{
  return bootloader_nsc_deinit();
}

void bootloader_rebootAndInstall(void)
{
  bootloader_nsc_rebootAndInstall();
}

int32_t bootloader_parseImageInfo(uint8_t           data[],
                                  size_t            numBytes,
                                  ApplicationData_t *appInfo,
                                  uint32_t          *bootloaderVersion)
{
  return bootloader_nsc_parseImageInfo(data, numBytes, appInfo, bootloaderVersion);
}

uint32_t bootloader_parserContextSize(void)
{
  return bootloader_nsc_parserContextSize();
}

bool bootloader_verifyApplication(uint32_t startAddress)
{
  return bootloader_nsc_verifyApplication(startAddress);
}

bool bootloader_secureBootEnforced(void)
{
  return bootloader_nsc_secureBootEnforced();
}

uint32_t bootloader_remainingApplicationUpgrades(void)
{
  return bootloader_nsc_remainingApplicationUpgrades();
}

bool bootloader_getCertificateVersion(uint32_t *version)
{
  return bootloader_nsc_getCertificateVersion(version);
}

void bootloader_getStorageInfo(BootloaderStorageInformation_t *info)
{
  bootloader_nsc_getStorageInfo(info);
}

int32_t bootloader_getStorageSlotInfo(uint32_t                slotId,
                                      BootloaderStorageSlot_t *slot)
{
  return bootloader_nsc_getStorageSlotInfo(slotId, slot);
}

int32_t bootloader_readStorage(uint32_t slotId,
                               uint32_t offset,
                               uint8_t  *buffer,
                               size_t   length)
{
  return bootloader_nsc_readStorage(slotId, offset, buffer, length);
}

int32_t bootloader_writeStorage(uint32_t slotId,
                                uint32_t offset,
                                uint8_t  *buffer,
                                size_t   length)
{
  return bootloader_nsc_writeStorage(slotId, offset, buffer, length);
}

int32_t bootloader_eraseWriteStorage(uint32_t slotId,
                                     uint32_t offset,
                                     uint8_t  *buffer,
                                     size_t   length)
{
  return bootloader_nsc_eraseWriteStorage(slotId, offset, buffer, length);
}

int32_t bootloader_eraseStorageSlot(uint32_t slotId)
{
  return bootloader_nsc_eraseStorageSlot(slotId);
}

int32_t bootloader_initChunkedEraseStorageSlot(uint32_t                slotId,
                                               BootloaderEraseStatus_t *eraseStat)
{
  return bootloader_nsc_initChunkedEraseStorageSlot(slotId, eraseStat);
}

int32_t bootloader_chunkedEraseStorageSlot(BootloaderEraseStatus_t *eraseStat)
{
  return bootloader_nsc_chunkedEraseStorageSlot(eraseStat);
}

int32_t bootloader_setImageToBootload(int32_t slotId)
{
  return bootloader_nsc_setImageToBootload(slotId);
}

int32_t bootloader_setImagesToBootload(int32_t *slotIds, size_t length)
{
  return bootloader_nsc_setImagesToBootload(slotIds, length);
}

int32_t bootloader_getImagesToBootload(int32_t *slotIds, size_t length)
{
  return bootloader_nsc_getImagesToBootload(slotIds, length);
}

int32_t bootloader_appendImageToBootloadList(int32_t slotId)
{
  return bootloader_nsc_appendImageToBootloadList(slotId);
}

int32_t bootloader_initVerifyImage(uint32_t slotId)
{
  return bootloader_nsc_initVerifyImage(slotId);
}

int32_t bootloader_continueVerifyImage(void)
{
  return bootloader_nsc_continueVerifyImage();
}

int32_t bootloader_verifyImage(uint32_t slotId)
{
  return bootloader_nsc_verifyImage(slotId);
}

int32_t bootloader_getImageInfo(uint32_t          slotId,
                                ApplicationData_t *appInfo,
                                uint32_t          *bootloaderVersion)
{
  return bootloader_nsc_getImageInfo(slotId, appInfo, bootloaderVersion);
}

bool bootloader_storageIsBusy(void)
{
  return bootloader_nsc_storageIsBusy();
}

int32_t bootloader_getAllocatedDMAChannel(void)
{
  return bootloader_nsc_getAllocatedDMAChannel();
}
