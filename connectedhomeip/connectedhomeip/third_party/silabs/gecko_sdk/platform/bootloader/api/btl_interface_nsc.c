/***************************************************************************//**
 * @file
 * @brief Application NSC interface to the bootloader.
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

#include <arm_cmse.h>
#include <string.h>
#include "btl_interface.h"
#include "btl_interface_storage.h"
#include "btl_reset_info.h"

// -----------------------------------------------------------------------------
// Defines

#define BL_MPU_WRAPPER(func_name, ...) \
  disable_mpu();                       \
  func_name(__VA_ARGS__);              \
  enable_mpu();

#define BL_MPU_WRAPPER_RET(func_name, ret, ...) \
  disable_mpu();                                \
  ret = func_name(__VA_ARGS__);                 \
  enable_mpu();

// -----------------------------------------------------------------------------
// Global variable

static bool mpu_enabled = false;

// -----------------------------------------------------------------------------
// Global context buffer

static uint8_t context_buffer[BOOTLOADER_STORAGE_VERIFICATION_CONTEXT_SIZE] = { 0 };

// -----------------------------------------------------------------------------
// Static functions

static void disable_mpu(void)
{
  mpu_enabled = (bool)(MPU->CTRL & MPU_CTRL_ENABLE_Msk);
  if (mpu_enabled) {
    ARM_MPU_Disable();
  }
}

static void enable_mpu(void)
{
  if (mpu_enabled) {
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
  }
}

static void bl_fatal_assert_action(void)
{
  BootloaderResetCause_t* resetCause = (BootloaderResetCause_t*) (RAM_MEM_BASE);
  resetCause->reason = BOOTLOADER_RESET_REASON_TZ_FAULT;
  resetCause->signature = BOOTLOADER_RESET_SIGNATURE_VALID;
  NVIC_SystemReset();
  __builtin_unreachable();
}

// Check if non-secure memory is actually non-secure as expected
static bool non_secure_memory_access(const void *p, size_t s)
{
  int flags = CMSE_MPU_READWRITE | CMSE_NONSECURE;
  if (s == 0u) {
    return false;
  }

  if (cmse_check_address_range((void *)p, s, flags) != NULL) {
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
// NSC functions

__attribute__((cmse_nonsecure_entry))
BootloaderResetCause_t bootloader_nsc_getResetReason(void)
{
  BootloaderResetCause_t resetReason = { 0 };
  BL_MPU_WRAPPER_RET(bootloader_getResetReason, resetReason);
  return resetReason;
}

__attribute__((cmse_nonsecure_entry))
void bootloader_nsc_getInfo(BootloaderInformation_t *info)
{
  if (!non_secure_memory_access(info, sizeof(BootloaderInformation_t))) {
    bl_fatal_assert_action();
  }

  BL_MPU_WRAPPER(bootloader_getInfo, info);
}

__attribute__((cmse_nonsecure_entry))
int32_t bootloader_nsc_init(void)
{
  int32_t ret = -1;
  BL_MPU_WRAPPER_RET(bootloader_init, ret);
  return ret;
}

__attribute__((cmse_nonsecure_entry))
int32_t bootloader_nsc_deinit(void)
{
  int32_t ret = -1;
  BL_MPU_WRAPPER_RET(bootloader_deinit, ret);
  return ret;
}

__attribute__((cmse_nonsecure_entry))
void bootloader_nsc_rebootAndInstall(void)
{
  bootloader_rebootAndInstall();
}

__attribute__((cmse_nonsecure_entry))
int32_t bootloader_nsc_parseImageInfo(uint8_t           data[],
                                      size_t            numBytes,
                                      ApplicationData_t *appInfo,
                                      uint32_t          *bootloaderVersion)
{
  if (!(non_secure_memory_access(data, numBytes)
        && non_secure_memory_access(appInfo, sizeof(ApplicationData_t))
        && non_secure_memory_access(bootloaderVersion, sizeof(uint32_t)))) {
    bl_fatal_assert_action();
  }

  int32_t ret = -1;
  BL_MPU_WRAPPER_RET(bootloader_parseImageInfo,
                     ret,
                     (BootloaderParserContext_t *)context_buffer,
                     data,
                     numBytes,
                     appInfo,
                     bootloaderVersion);
  memset(context_buffer, 0u, BOOTLOADER_STORAGE_VERIFICATION_CONTEXT_SIZE);
  return ret;
}

__attribute__((cmse_nonsecure_entry))
uint32_t bootloader_nsc_parserContextSize(void)
{
  uint32_t ret = 0xFFFFFFFFUL;
  BL_MPU_WRAPPER_RET(bootloader_parserContextSize, ret);
  return ret;
}

__attribute__((cmse_nonsecure_entry))
bool bootloader_nsc_verifyApplication(uint32_t startAddress)
{
  bool ret = false;
  BL_MPU_WRAPPER_RET(bootloader_verifyApplication, ret, startAddress);
  return ret;
}

__attribute__((cmse_nonsecure_entry))
bool bootloader_nsc_secureBootEnforced(void)
{
  bool ret = false;
  BL_MPU_WRAPPER_RET(bootloader_secureBootEnforced, ret);
  return ret;
}

__attribute__((cmse_nonsecure_entry))
uint32_t bootloader_nsc_remainingApplicationUpgrades(void)
{
  uint32_t ret = 0xFFFFFFFFUL;
  BL_MPU_WRAPPER_RET(bootloader_remainingApplicationUpgrades, ret);
  return ret;
}

__attribute__((cmse_nonsecure_entry))
bool bootloader_nsc_getCertificateVersion(uint32_t *version)
{
  if (!(non_secure_memory_access(version, sizeof(uint32_t)))) {
    bl_fatal_assert_action();
  }

  bool ret = false;
  BL_MPU_WRAPPER_RET(bootloader_getCertificateVersion, ret, version);
  return ret;
}

__attribute__((cmse_nonsecure_entry))
void bootloader_nsc_getStorageInfo(BootloaderStorageInformation_t *info)
{
  if (!(non_secure_memory_access(info, sizeof(BootloaderStorageInformation_t)))) {
    bl_fatal_assert_action();
  }

  BL_MPU_WRAPPER(bootloader_getStorageInfo, info);
}

__attribute__((cmse_nonsecure_entry))
int32_t bootloader_nsc_getStorageSlotInfo(uint32_t                slotId,
                                          BootloaderStorageSlot_t *slot)
{
  if (!(non_secure_memory_access(slot, sizeof(BootloaderStorageSlot_t)))) {
    bl_fatal_assert_action();
  }
  int32_t ret = -1;
  BL_MPU_WRAPPER_RET(bootloader_getStorageSlotInfo, ret, slotId, slot);
  return ret;
}

__attribute__((cmse_nonsecure_entry))
int32_t bootloader_nsc_readStorage(uint32_t slotId,
                                   uint32_t offset,
                                   uint8_t  *buffer,
                                   size_t   length)
{
  if (!(non_secure_memory_access(buffer, length))) {
    bl_fatal_assert_action();
  }

  int32_t ret = -1;
  BL_MPU_WRAPPER_RET(bootloader_readStorage, ret, slotId, offset, buffer, length);
  return ret;
}

__attribute__((cmse_nonsecure_entry))
int32_t bootloader_nsc_writeStorage(uint32_t slotId,
                                    uint32_t offset,
                                    uint8_t  *buffer,
                                    size_t   length)
{
  if (!(non_secure_memory_access(buffer, length))) {
    bl_fatal_assert_action();
  }

  int32_t ret = -1;
  BL_MPU_WRAPPER_RET(bootloader_writeStorage, ret, slotId, offset, buffer, length);
  return ret;
}

__attribute__((cmse_nonsecure_entry))
int32_t bootloader_nsc_eraseWriteStorage(uint32_t slotId,
                                         uint32_t offset,
                                         uint8_t  *buffer,
                                         size_t   length)
{
  if (!(non_secure_memory_access(buffer, length))) {
    bl_fatal_assert_action();
  }

  int32_t ret = -1;
  BL_MPU_WRAPPER_RET(bootloader_eraseWriteStorage, ret, slotId, offset, buffer, length);
  return ret;
}

__attribute__((cmse_nonsecure_entry))
int32_t bootloader_nsc_eraseStorageSlot(uint32_t slotId)
{
  int32_t ret = -1;
  BL_MPU_WRAPPER_RET(bootloader_eraseStorageSlot, ret, slotId);
  return ret;
}

__attribute__((cmse_nonsecure_entry))
int32_t bootloader_nsc_initChunkedEraseStorageSlot(uint32_t                slotId,
                                                   BootloaderEraseStatus_t *eraseStat)
{
  if (!(non_secure_memory_access(eraseStat, sizeof(BootloaderEraseStatus_t)))) {
    bl_fatal_assert_action();
  }

  int32_t ret = -1;
  BL_MPU_WRAPPER_RET(bootloader_initChunkedEraseStorageSlot, ret, slotId, eraseStat);
  return ret;
}

__attribute__((cmse_nonsecure_entry))
int32_t bootloader_nsc_chunkedEraseStorageSlot(BootloaderEraseStatus_t *eraseStat)
{
  if (!(non_secure_memory_access(eraseStat, sizeof(BootloaderEraseStatus_t)))) {
    bl_fatal_assert_action();
  }

  int32_t ret = -1;
  BL_MPU_WRAPPER_RET(bootloader_chunkedEraseStorageSlot, ret, eraseStat);
  return ret;
}

__attribute__((cmse_nonsecure_entry))
int32_t bootloader_nsc_setImageToBootload(int32_t slotId)
{
  int32_t ret = -1;
  BL_MPU_WRAPPER_RET(bootloader_setImageToBootload, ret, slotId);
  return ret;
}

__attribute__((cmse_nonsecure_entry))
int32_t bootloader_nsc_setImagesToBootload(int32_t *slotIds, size_t length)
{
  if (!(non_secure_memory_access(slotIds, length))) {
    bl_fatal_assert_action();
  }
  int32_t ret = -1;
  BL_MPU_WRAPPER_RET(bootloader_setImagesToBootload, ret, slotIds, length);
  return ret;
}

__attribute__((cmse_nonsecure_entry))
int32_t bootloader_nsc_getImagesToBootload(int32_t *slotIds, size_t length)
{
  if (!(non_secure_memory_access(slotIds, length))) {
    bl_fatal_assert_action();
  }

  int32_t ret = -1;
  BL_MPU_WRAPPER_RET(bootloader_getImagesToBootload, ret, slotIds, length);
  return ret;
}

__attribute__((cmse_nonsecure_entry))
int32_t bootloader_nsc_appendImageToBootloadList(int32_t slotId)
{
  int32_t ret = -1;
  BL_MPU_WRAPPER_RET(bootloader_appendImageToBootloadList, ret, slotId);
  return ret;
}

__attribute__((cmse_nonsecure_entry))
int32_t bootloader_nsc_initVerifyImage(uint32_t slotId)
{
  int32_t ret = -1;
  BL_MPU_WRAPPER_RET(bootloader_initVerifyImage,
                     ret,
                     slotId,
                     context_buffer,
                     BOOTLOADER_STORAGE_VERIFICATION_CONTEXT_SIZE);
  return ret;
}

__attribute__((cmse_nonsecure_entry))
int32_t bootloader_nsc_continueVerifyImage(void)
{
  int32_t ret = -1;
  BL_MPU_WRAPPER_RET(bootloader_continueVerifyImage,
                     ret,
                     context_buffer,
                     NULL);
  return ret;
}

__attribute__((cmse_nonsecure_entry))
int32_t bootloader_nsc_verifyImage(uint32_t slotId)
{
  int32_t ret = -1;
  BL_MPU_WRAPPER_RET(bootloader_verifyImage,
                     ret,
                     slotId,
                     NULL);
  return ret;
}

__attribute__((cmse_nonsecure_entry))
int32_t bootloader_nsc_getImageInfo(uint32_t          slotId,
                                    ApplicationData_t *appInfo,
                                    uint32_t          *bootloaderVersion)
{
  if (!(non_secure_memory_access(appInfo, sizeof(ApplicationData_t))
        && non_secure_memory_access(bootloaderVersion, sizeof(uint32_t)))) {
    bl_fatal_assert_action();
  }

  int32_t ret = -1;
  BL_MPU_WRAPPER_RET(bootloader_getImageInfo,
                     ret,
                     slotId,
                     appInfo,
                     bootloaderVersion);
  return ret;
}

__attribute__((cmse_nonsecure_entry))
bool bootloader_nsc_storageIsBusy(void)
{
  bool ret = false;
  BL_MPU_WRAPPER_RET(bootloader_storageIsBusy,
                     ret);
  return ret;
}

__attribute__((cmse_nonsecure_entry))
int32_t bootloader_nsc_getAllocatedDMAChannel(void)
{
  int32_t ret = -1;
  BL_MPU_WRAPPER_RET(bootloader_getAllocatedDMAChannel,
                     ret);
  return ret;
}
