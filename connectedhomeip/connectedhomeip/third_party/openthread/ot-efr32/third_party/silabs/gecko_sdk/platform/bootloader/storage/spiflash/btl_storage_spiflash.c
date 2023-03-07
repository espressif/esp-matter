/***************************************************************************//**
 * @file
 * @brief Spiflash-backed storage component for Silicon Labs Bootloader.
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

#include "config/btl_config.h"

#include "storage/btl_storage.h"
#include "storage/btl_storage_internal.h"
#include "storage/spiflash/btl_storage_spiflash.h"

#include "storage/spiflash/btl_storage_spiflash_configs.h"

#include "driver/btl_driver_delay.h"
#include "driver/btl_driver_spi_controller.h"

#include "debug/btl_debug.h"

#ifndef BTL_CONFIG_FILE
#include "btl_storage_slot_cfg.h"
#endif
// -----------------------------------------------------------------------------
// Globals

const BootloaderStorageLayout_t storageLayout = {
  SPIFLASH,
  BTL_STORAGE_NUM_SLOTS,
  BTL_STORAGE_SLOTS
};

// -----------------------------------------------------------------------------
// Functions

static void waitUntilNotBusy(void)
{
  while (storage_isBusy()) {
    // Do nothing
  }
}

static void setWriteEnableLatch(void)
{
  spi_setCsActive();
  spi_writeByte(CMD_WRITE_ENABLE);
  spi_setCsInactive();
}

static StorageSpiflashDevice_t getDeviceType(void)
{
  uint8_t mfgId;
  uint16_t deviceId;

  // cannot check for busy in this API since it is used by
  //  init.  Callers must verify not busy individually.
  spi_setCsActive();
  // following implementation takes smaller buffer (3) of efm into account
  spi_writeByte(CMD_JEDEC_ID);
  mfgId = spi_readByte();
  deviceId = spi_readHalfword();
  spi_setCsInactive();

  switch (mfgId) {
    case MFG_ID_SPANSION:
      switch (deviceId) {
#if defined(BTL_STORAGE_SPIFLASH_SPANSION_S25FL208K) && (BTL_STORAGE_SPIFLASH_SPANSION_S25FL208K == 1)
        case DEVICE_ID_SPANSION_8M:
          return SPANSION_8M_DEVICE;
#endif
        default:
          return UNKNOWN_DEVICE;
      }
    case MFG_ID_WINBOND:
      switch (deviceId) {
#if defined(BTL_STORAGE_SPIFLASH_WINBOND_W25X20BV) && (BTL_STORAGE_SPIFLASH_WINBOND_W25X20BV == 1)
        case DEVICE_ID_WINBOND_2M:
          return WINBOND_2M_DEVICE;
#endif
#if defined(BTL_STORAGE_SPIFLASH_WINBOND_W25Q80BV) && (BTL_STORAGE_SPIFLASH_WINBOND_W25Q80BV == 1)
        case DEVICE_ID_WINBOND_8M:
          return WINBOND_8M_DEVICE;
#endif
        default:
          return UNKNOWN_DEVICE;
      }
    case MFG_ID_MACRONIX:
      switch (deviceId) {
#if defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25L2006E) && (BTL_STORAGE_SPIFLASH_MACRONIX_MX25L2006E == 1)
        case DEVICE_ID_MACRONIX_2M:
          return MACRONIX_2M_DEVICE;
#endif
#if defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25L4006E) && (BTL_STORAGE_SPIFLASH_MACRONIX_MX25L4006E == 1)
        case DEVICE_ID_MACRONIX_4M:
          return MACRONIX_4M_DEVICE;
#endif
#if defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25L8006E) && (BTL_STORAGE_SPIFLASH_MACRONIX_MX25L8006E == 1)
        case DEVICE_ID_MACRONIX_8M:
          return MACRONIX_8M_DEVICE;
#endif
#if defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25R8035F) && (BTL_STORAGE_SPIFLASH_MACRONIX_MX25R8035F == 1)
        case DEVICE_ID_MACRONIX_8M_LP:
          return MACRONIX_8M_LP_DEVICE;
#endif
#if defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25L1606E) && (BTL_STORAGE_SPIFLASH_MACRONIX_MX25L1606E == 1)
        case DEVICE_ID_MACRONIX_16M:
          return MACRONIX_16M_DEVICE;
#endif
#if defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25U1635E) && (BTL_STORAGE_SPIFLASH_MACRONIX_MX25U1635E == 1)
        case DEVICE_ID_MACRONIX_16M_2V:
          return MACRONIX_16M_2V_DEVICE;
#endif
#if defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25R3235F) && (BTL_STORAGE_SPIFLASH_MACRONIX_MX25R3235F == 1)
        case DEVICE_ID_MACRONIX_32M_LP:
          return MACRONIX_32M_LP_DEVICE;
#endif
#if defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25R6435F) && (BTL_STORAGE_SPIFLASH_MACRONIX_MX25R6435F == 1)
        case DEVICE_ID_MACRONIX_64M_LP:
          return MACRONIX_64M_LP_DEVICE;
#endif
        default:
          return UNKNOWN_DEVICE;
      }
    case MFG_ID_ATMEL:
      // also MFG_ID_ADESTO
      switch (deviceId) {
#if defined(BTL_STORAGE_SPIFLASH_ATMEL_AT25DF041A) && (BTL_STORAGE_SPIFLASH_ATMEL_AT25DF041A == 1)
        case DEVICE_ID_ATMEL_4M:
          return ATMEL_4M_DEVICE;
#endif
#if defined(BTL_STORAGE_SPIFLASH_ATMEL_AT25DF081A) && (BTL_STORAGE_SPIFLASH_ATMEL_AT25DF081A == 1)
        case DEVICE_ID_ATMEL_8M:
          return ATMEL_8M_DEVICE;
#endif
#if defined(BTL_STORAGE_SPIFLASH_ADESTO_AT25SF041) && (BTL_STORAGE_SPIFLASH_ADESTO_AT25SF041 == 1)
        case DEVICE_ID_ADESTO_4M:
          return ADESTO_4M_DEVICE;
#endif
        default:
          return UNKNOWN_DEVICE;
      }
    case MFG_ID_NUMONYX:
      switch (deviceId) {
#if defined(BTL_STORAGE_SPIFLASH_NUMONYX_M25P20) && (BTL_STORAGE_SPIFLASH_NUMONYX_M25P20 == 1)
        case DEVICE_ID_NUMONYX_2M:
          return NUMONYX_2M_DEVICE;
#endif
#if defined(BTL_STORAGE_SPIFLASH_NUMONYX_M25P40) && (BTL_STORAGE_SPIFLASH_NUMONYX_M25P40 == 1)
        case DEVICE_ID_NUMONYX_4M:
          return NUMONYX_4M_DEVICE;
#endif
#if defined(BTL_STORAGE_SPIFLASH_NUMONYX_M25P80) && (BTL_STORAGE_SPIFLASH_NUMONYX_M25P80 == 1)
        case DEVICE_ID_NUMONYX_8M:
          return NUMONYX_8M_DEVICE;
#endif
#if defined(BTL_STORAGE_SPIFLASH_NUMONYX_M25P16) && (BTL_STORAGE_SPIFLASH_NUMONYX_M25P16 == 1)
        case DEVICE_ID_NUMONYX_16M:
          return NUMONYX_16M_DEVICE;
#endif
        default:
          return UNKNOWN_DEVICE;
      }
    case MFG_ID_ISSI:
      switch (deviceId) {
#if defined(BTL_STORAGE_SPIFLASH_ISSI_IS25LQ025B) && (BTL_STORAGE_SPIFLASH_ISSI_IS25LQ025B == 1)
        case DEVICE_ID_ISSI_256K:
          return ISSI_256K_DEVICE;
#endif
#if defined(BTL_STORAGE_SPIFLASH_ISSI_IS25LQ512B) && (BTL_STORAGE_SPIFLASH_ISSI_IS25LQ512B == 1)
        case DEVICE_ID_ISSI_512K:
          return ISSI_512K_DEVICE;
#endif
#if defined(BTL_STORAGE_SPIFLASH_ISSI_IS25LQ010B) && (BTL_STORAGE_SPIFLASH_ISSI_IS25LQ010B == 1)
        case DEVICE_ID_ISSI_1M:
          return ISSI_1M_DEVICE;
#endif
#if defined(BTL_STORAGE_SPIFLASH_ISSI_IS25LQ020B) && (BTL_STORAGE_SPIFLASH_ISSI_IS25LQ020B == 1)
        case DEVICE_ID_ISSI_2M:
          return ISSI_2M_DEVICE;
#endif
#if defined(BTL_STORAGE_SPIFLASH_ISSI_IS25LQ040B) && (BTL_STORAGE_SPIFLASH_ISSI_IS25LQ040B == 1)
        case DEVICE_ID_ISSI_4M:
          return ISSI_4M_DEVICE;
#endif
        default:
          return UNKNOWN_DEVICE;
      }
    default:
      return UNKNOWN_DEVICE;
  }
}

BootloaderStorageImplementationInformation_t getDeviceInfo(void)
{
  waitUntilNotBusy();
  BootloaderStorageImplementationInformation_t unknownSPIFlash = { 0 };
  switch (getDeviceType()) {
#if defined(BTL_STORAGE_SPIFLASH_SPANSION_S25FL208K) && (BTL_STORAGE_SPIFLASH_SPANSION_S25FL208K == 1)
    case SPANSION_8M_DEVICE:
      return spansion8MInfo;
#endif
#if defined(BTL_STORAGE_SPIFLASH_WINBOND_W25X20BV) && (BTL_STORAGE_SPIFLASH_WINBOND_W25X20BV == 1)
    case WINBOND_2M_DEVICE:
      return windbond2MInfo;
#endif
#if defined(BTL_STORAGE_SPIFLASH_WINBOND_W25Q80BV) && (BTL_STORAGE_SPIFLASH_WINBOND_W25Q80BV == 1)
    case WINBOND_8M_DEVICE:
      return windbond8MInfo;
#endif
#if defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25L2006E) && (BTL_STORAGE_SPIFLASH_MACRONIX_MX25L2006E == 1)
    case MACRONIX_2M_DEVICE:
      return macronix2MInfo;
#endif
#if defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25L4006E) && (BTL_STORAGE_SPIFLASH_MACRONIX_MX25L4006E == 1)
    case MACRONIX_4M_DEVICE:
      return macronix4MInfo;
#endif
#if defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25L8006E) && (BTL_STORAGE_SPIFLASH_MACRONIX_MX25L8006E == 1)
    case MACRONIX_8M_DEVICE:
      return macronix8MInfo;
#endif
#if defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25R8035F) && (BTL_STORAGE_SPIFLASH_MACRONIX_MX25R8035F == 1)
    case MACRONIX_8M_LP_DEVICE:
      return macronix8MLPInfo;
#endif
#if defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25L1606E) && (BTL_STORAGE_SPIFLASH_MACRONIX_MX25L1606E == 1)
    case MACRONIX_16M_DEVICE:
      return macronix16MInfo;
#endif
#if defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25U1635E) && (BTL_STORAGE_SPIFLASH_MACRONIX_MX25U1635E == 1)
    case MACRONIX_16M_2V_DEVICE:
      return macronix16M2VInfo;
#endif
#if defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25R3235F) && (BTL_STORAGE_SPIFLASH_MACRONIX_MX25R3235F == 1)
    case MACRONIX_32M_LP_DEVICE:
      return macronix32MLPInfo;
#endif

#if defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25R6435F) && (BTL_STORAGE_SPIFLASH_MACRONIX_MX25R6435F == 1)
    case MACRONIX_64M_LP_DEVICE:
      return macronix64MLPInfo;
#endif
#if defined(BTL_STORAGE_SPIFLASH_ATMEL_AT25DF041A) && (BTL_STORAGE_SPIFLASH_ATMEL_AT25DF041A == 1)
    case ATMEL_4M_DEVICE:
      return atmel4MInfo;
#endif
#if defined(BTL_STORAGE_SPIFLASH_ATMEL_AT25DF081A) && (BTL_STORAGE_SPIFLASH_ATMEL_AT25DF081A == 1)
    case ATMEL_8M_DEVICE:
      return atmel8MInfo;
#endif
#if defined(BTL_STORAGE_SPIFLASH_ADESTO_AT25SF041) && (BTL_STORAGE_SPIFLASH_ADESTO_AT25SF041 == 1)
    case ADESTO_4M_DEVICE:
      return adesto4MInfo;
#endif
#if defined(BTL_STORAGE_SPIFLASH_NUMONYX_M25P20) && (BTL_STORAGE_SPIFLASH_NUMONYX_M25P20 == 1)
    case NUMONYX_2M_DEVICE:
      return numonyx2MInfo;
#endif
#if defined(BTL_STORAGE_SPIFLASH_NUMONYX_M25P40) && (BTL_STORAGE_SPIFLASH_NUMONYX_M25P40 == 1)
    case NUMONYX_4M_DEVICE:
      return numonyx4MInfo;
#endif
#if defined(BTL_STORAGE_SPIFLASH_NUMONYX_M25P80) && (BTL_STORAGE_SPIFLASH_NUMONYX_M25P80 == 1)
    case NUMONYX_8M_DEVICE:
      return numonyx8MInfo;
#endif
#if defined(BTL_STORAGE_SPIFLASH_NUMONYX_M25P16s) && (BTL_STORAGE_SPIFLASH_NUMONYX_M25P16s == 1)
    case NUMONYX_16M_DEVICE:
      return numonyx16MInfo;
#endif
#if defined(BTL_STORAGE_SPIFLASH_ISSI_IS25LQ025B) && (BTL_STORAGE_SPIFLASH_ISSI_IS25LQ025B == 1)
    case ISSI_256K_DEVICE:
      return issi256KInfo;
#endif
#if defined(BTL_STORAGE_SPIFLASH_ISSI_IS25LQ512B) && (BTL_STORAGE_SPIFLASH_ISSI_IS25LQ512B == 1)
    case ISSI_512K_DEVICE:
      return issi512KInfo;
#endif
#if defined(BTL_STORAGE_SPIFLASH_ISSI_IS25LQ010B) && (BTL_STORAGE_SPIFLASH_ISSI_IS25LQ010B == 1)
    case ISSI_1M_DEVICE:
      return issi1MInfo;
#endif
#if defined(BTL_STORAGE_SPIFLASH_ISSI_IS25LQ020B) && (BTL_STORAGE_SPIFLASH_ISSI_IS25LQ020B == 1)
    case ISSI_2M_DEVICE:
      return issi2MInfo;
#endif
#if defined(BTL_STORAGE_SPIFLASH_ISSI_IS25LQ040B) && (BTL_STORAGE_SPIFLASH_ISSI_IS25LQ040B == 1)
    case ISSI_4M_DEVICE:
      return issi4MInfo;
#endif
    default:
      return unknownSPIFlash;
  }
}

static uint32_t getDeviceSize(StorageSpiflashDevice_t *pDeviceType)
{
  StorageSpiflashDevice_t deviceType;
  waitUntilNotBusy();
  if (pDeviceType == NULL) {
    deviceType = getDeviceType();
  } else {
    deviceType = *pDeviceType;
    if (deviceType == UNKNOWN_DEVICE) {
      deviceType = getDeviceType();
      *pDeviceType = deviceType;
    }
  }
  switch (deviceType) {
    case ISSI_256K_DEVICE:
      return DEVICE_SIZE_256K;
    case ISSI_512K_DEVICE:
      return DEVICE_SIZE_512K;
    case ISSI_1M_DEVICE:
      return DEVICE_SIZE_1M;
    case WINBOND_2M_DEVICE:
    case NUMONYX_2M_DEVICE:
    case MACRONIX_2M_DEVICE:
    case ISSI_2M_DEVICE:
      return DEVICE_SIZE_2M;
    case ATMEL_4M_DEVICE:
    case ADESTO_4M_DEVICE:
    case NUMONYX_4M_DEVICE:
    case ISSI_4M_DEVICE:
    case MACRONIX_4M_DEVICE:
      return DEVICE_SIZE_4M;
    case SPANSION_8M_DEVICE:
    case WINBOND_8M_DEVICE:
    case ATMEL_8M_DEVICE:
    case MACRONIX_8M_DEVICE:
    case MACRONIX_8M_LP_DEVICE:
    case NUMONYX_8M_DEVICE:
      return DEVICE_SIZE_8M;
    case MACRONIX_16M_DEVICE:
    case MACRONIX_16M_2V_DEVICE:
    case NUMONYX_16M_DEVICE:
      return DEVICE_SIZE_16M;
    case MACRONIX_32M_LP_DEVICE:
      return DEVICE_SIZE_32M;
    case MACRONIX_64M_LP_DEVICE:
      return DEVICE_SIZE_64M;
    default:
      return 0;
  }
}

static bool verifyAddressRange(uint32_t                address,
                               uint32_t                length,
                               StorageSpiflashDevice_t *pDeviceType)
{
  uint32_t deviceSize = getDeviceSize(pDeviceType);
  if ((length > deviceSize)
      || (address > deviceSize)) {
    return false;
  }

  if ((address + length) <= deviceSize) {
    return true;
  }

  // out of range
  return false;
}

static void sendCommand(uint8_t command, uint32_t address)
{
  spi_writeByte(command);
  spi_write3Byte(address);
}

static bool verifyErased(uint32_t address, uint32_t len)
{
  waitUntilNotBusy();

  spi_setCsActive();
  sendCommand(CMD_READ_DATA, address);

  while (len--) {
    if (spi_readByte() != 0xFF) {
      return false;
    }
  }
  spi_setCsInactive();
  return true;
}

static void writePage(uint32_t address, const uint8_t *data, uint32_t length)
{
  waitUntilNotBusy();
  setWriteEnableLatch();

  spi_setCsActive();
  sendCommand(CMD_PAGE_PROG, address);

  while (length--) {
    spi_writeByte(*data++);
  }
  spi_setCsInactive();
}

static void eraseCommand(uint8_t command, uint32_t address)
{
  waitUntilNotBusy();
  setWriteEnableLatch();
  spi_setCsActive();
  sendCommand(command, address);
  spi_setCsInactive();
}

int32_t storage_init(void)
{
  StorageSpiflashDevice_t deviceType;

  spi_init();

  // Perform custom storage init (power pins, write protection pins etc)
  storage_customInit();

  // Ensure the device is ready to access after applying power
  // We delay even if shutdown control isn't used to play it safe
  // since we don't know how quickly init may be called after boot
  delay_microseconds(TIMING_POWERDOWN_MAX_US);

  // Release the chip from powerdown mode
  spi_setCsActive();
  spi_writeByte(CMD_POWER_UP);
  spi_setCsInactive();

  delay_microseconds(TIMING_POWERON_MAX_US);

  deviceType = getDeviceType();
  if (deviceType == UNKNOWN_DEVICE) {
    return BOOTLOADER_ERROR_INIT_STORAGE;
  }

  // For Atmel devices, need to unprotect them because default is protected
  if ((deviceType >= ATMEL_4M_DEVICE) && (deviceType <= ATMEL_8M_DEVICE)) {
    setWriteEnableLatch();
    spi_setCsActive();
    spi_writeByte(CMD_WRITE_STATUS);
    spi_writeByte(0); // No protect bits set
    spi_setCsInactive();
  }
  return BOOTLOADER_OK;
}

bool storage_isBusy(void)
{
  uint8_t status;

  spi_setCsActive();
  spi_writeByte(CMD_READ_STATUS);
  status = spi_readByte();
  spi_setCsInactive();

  return (bool)(status & STATUS_BUSY_MASK);
}

int32_t storage_readRaw(uint32_t address, uint8_t *data, size_t length)
{
  // Ensure address is is within chip
  if (!verifyAddressRange(address, length, NULL)) {
    return BOOTLOADER_ERROR_STORAGE_INVALID_ADDRESS;
  }

  waitUntilNotBusy();

  spi_setCsActive();
  sendCommand(CMD_READ_DATA, address);

  while (length--) {
    *data++ = spi_readByte();
  }
  spi_setCsInactive();

  return BOOTLOADER_OK;
}

int32_t storage_writeRaw(uint32_t address, uint8_t *data, size_t numBytes)
{
  uint32_t nextPageAddr;
  uint32_t currentLength;

  // Ensure address is is within chip
  if (!verifyAddressRange(address, numBytes, NULL)) {
    return BOOTLOADER_ERROR_STORAGE_INVALID_ADDRESS;
  }
  // Ensure space is empty
  if (!verifyErased(address, numBytes)) {
    return BOOTLOADER_ERROR_STORAGE_NEEDS_ERASE;
  }

  if (address & DEVICE_PAGE_MASK) {
    // handle unaligned first block
    nextPageAddr = (address & (~DEVICE_PAGE_MASK)) + DEVICE_PAGE_SIZE;
    if ((address + numBytes) < nextPageAddr) {
      // fits all within first block
      currentLength = numBytes;
    } else {
      currentLength = (uint16_t) (nextPageAddr - address);
    }
  } else {
    currentLength = (numBytes > DEVICE_PAGE_SIZE) ? DEVICE_PAGE_SIZE : numBytes;
  }
  while (numBytes) {
    writePage(address, data, currentLength);
    numBytes -= currentLength;
    address += currentLength;
    data += currentLength;
    currentLength = (numBytes > DEVICE_PAGE_SIZE) ? DEVICE_PAGE_SIZE : numBytes;
  }

  return BOOTLOADER_OK;
}

int32_t storage_getDMAchannel(void)
{
  return -1;
}

uint32_t storage_getSpiUsartPPUSATD(uint32_t *ppusatdNr) {
  return spi_getUsartPPUSATD(ppusatdNr);
}

int32_t storage_eraseRaw(uint32_t address, size_t totalLength)
{
  // Get device characteristics
  StorageSpiflashDevice_t deviceType = UNKNOWN_DEVICE;
  uint32_t sectorMask = DEVICE_SECTOR_MASK;
  uint32_t deviceSize = getDeviceSize(&deviceType);
  uint32_t deviceBlockSize = DEVICE_BLOCK_SIZE_64K;
  uint32_t deviceBlockMask = DEVICE_BLOCK_MASK_64K;
  // Numonyx/Micron parts only support block erase, not sector
  if ((deviceType >= NUMONYX_2M_DEVICE)
      && (deviceType <= NUMONYX_16M_DEVICE)) {
    sectorMask = DEVICE_BLOCK_MASK_64K;
  } else if ((deviceType >= ISSI_256K_DEVICE)
             && (deviceType <= ISSI_512K_DEVICE)) {
    deviceBlockSize = DEVICE_BLOCK_SIZE_32K;
    deviceBlockMask = DEVICE_BLOCK_MASK_32K;
  }

  // Validate that it's possible to erase the slot
  // Length must be a multiple of the sector size
  if (totalLength & sectorMask) {
    return BOOTLOADER_ERROR_STORAGE_NEEDS_ALIGN;
  }
  // Address must be sector aligned
  if (address & sectorMask) {
    return BOOTLOADER_ERROR_STORAGE_NEEDS_ALIGN;
  }
  // Address and length must be in range
  if (!verifyAddressRange(address, totalLength, &deviceType)) {
    return BOOTLOADER_ERROR_STORAGE_INVALID_ADDRESS;
  }

  // Test for full chip erase
  if ((address == 0) && (totalLength == deviceSize)) {
    waitUntilNotBusy();
    setWriteEnableLatch();
    spi_setCsActive();
    spi_writeByte(CMD_ERASE_CHIP);
    spi_setCsInactive();
    return BOOTLOADER_OK;
  }

  // first handle leading partial blocks
  while (totalLength && (address & deviceBlockMask)) {
    eraseCommand(CMD_ERASE_SECTOR, address);
    address += DEVICE_SECTOR_SIZE;
    totalLength -= DEVICE_SECTOR_SIZE;
  }
  // handle any full blocks
  while (totalLength >= deviceBlockSize) {
    eraseCommand(CMD_ERASE_BLOCK, address);
    address += deviceBlockSize;
    totalLength -= deviceBlockSize;
  }
  // finally handle any trailing partial blocks
  while (totalLength) {
    eraseCommand(CMD_ERASE_SECTOR, address);
    address += DEVICE_SECTOR_SIZE;
    totalLength -= DEVICE_SECTOR_SIZE;
  }
  return BOOTLOADER_OK;
}

int32_t storage_shutdown(void)
{
  // wait for any outstanding operations to complete before pulling the plug
  waitUntilNotBusy();

  // always enter low power mode, even if using shutdown control
  // since sometimes leakage prevents shutdown control from
  // completely turning off the part.
  spi_setCsActive();
  spi_writeByte(CMD_POWER_DOWN);
  spi_setCsInactive();

  // Perform custom shutdown (power pins, etc)
  storage_customShutdown();
#if defined(BTL_SPI_EUSART_ENABLE)
  spi_deinit();
#endif
  return BOOTLOADER_OK;
}
