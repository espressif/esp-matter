/***************************************************************************//**
 * @file
 * @brief Spiflash-backed storage component with JEDEC support for Silicon Labs Bootloader.
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
#include "storage/spiflash_sfdp/btl_storage_spiflash_sfdp.h"
#include "storage/spiflash_sfdp/btl_storage_spiflash_configs_sfdp.h"

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

static void SFDP_sendCommand(uint32_t address)
{
  spi_writeByte(CMD_READ_SFDP);
  spi_write3Byte(address);
  spi_writeByte(0xff);
}

//Checking the support for the SFDP
static int32_t SFDP_checkSupport(void)
{
  uint32_t signature;

  spi_setCsActive();

  //Querying the SFDP header at address 0x0
  //to verify the SFDP signature (0x50444653 (“SFDP” string))
  SFDP_sendCommand(0x000000);

  signature = spi_readHalfword() << 16;
  signature |= spi_readHalfword();
  spi_setCsInactive();

  //Check if we received a valid SFDP signature from the flash device
  if (signature != SFDP_SIGNATURE) {
    return BOOTLOADER_ERROR_INIT_SFDP;
  }
  return BOOTLOADER_OK;
}

static uint32_t SFDP_getPTP(void)  //Parameter Table Pointer
{
  uint32_t ptp;
  waitUntilNotBusy();
  spi_setCsActive();

  //Querying the pointer to the default parameter table:PTP0
  //The Parameter Table (0) Pointer is located at the address 0x0C
  SFDP_sendCommand(0x0C);

  ptp = spi_readByte();
  ptp |= spi_readByte() << 8;
  ptp |= spi_readByte() << 16;

  spi_setCsInactive();

  return ptp;
}

static uint32_t SFDP_maxEraseChipTime(void)
{
  uint32_t ptp = SFDP_getPTP();
  uint8_t t_time;
  uint8_t count;
  uint8_t multiplier;
  uint32_t max_time;

  spi_setCsActive();

  //Querying the 11th DWORD in Parameter Table(0)
  SFDP_sendCommand(ptp + 0x28);

  //Query the multiplier D[3:0]
  multiplier = (spi_readByte() & 0x0F);
  spi_readHalfword();
  t_time = spi_readByte();

  spi_setCsInactive();

  multiplier = 2 * (multiplier + 1);

  // count : [28:24]
  count = (t_time & 0x1F);

  // unit : [30:29]
  // 00b: 16 ms
  // 01b: 256 ms
  // 10b: 4 s
  // 11b: 64 s
  uint8_t unit = (t_time & 0x60) >> 5;

  //Calculate the maximum time
  //Formula: typical time = (count + 1)*units
  //Formula: maximum time = typical time * 2 * (multiplier + 1)

  if (!unit) {
    max_time = 16 * (count + 1) * multiplier;
  } else if (unit & 0x1) {
    max_time = 256 * (count + 1) * multiplier;
  } else if (unit & 0x2) {
    max_time = 4 * (count + 1) * multiplier * 1000; //Converting seconds to ms
  } else if (unit & 0x3) {
    max_time = 64 * (count + 1) * multiplier * 1000; //Converting seconds to ms
  }
  return max_time;
}

static uint32_t SFDP_4KMaxEraseTime(void)
{
  uint32_t ptp = SFDP_getPTP();
  uint16_t t_arr;
  uint8_t count_m; //count max erase time
  uint8_t count_t; //count typical erase time

  spi_setCsActive();

  //Querying the 10th DWORD in PTP(0)
  SFDP_sendCommand(ptp + 0x24);
  t_arr = spi_readByte();
  t_arr |= spi_readByte();

  spi_setCsInactive();

  //[3:0] count_m
  count_m = (t_arr & 0x0f);
  //[8:4] count_t
  count_t = (t_arr & 0x01f0) >> 4;

  //[10:9] units
  //00b: 1ms
  //01b: 16ms
  //10b: 128 ms
  //11b: 1s
  uint8_t unit = (t_arr & 0x600) >> 9;
  uint16_t multiplier = 2 * (count_m + 1);
  uint32_t time;

  //Formula: Erase Type n (or Chip) erase maximum time =2 * (count + 1) *  Erase Type n (or Chip) erase typical time

  if (!unit) {
    time = 1 * (count_t + 1) * multiplier;
  } else if (unit & 0x1) {
    time = 16 * (count_t + 1) * multiplier;
  } else if (unit & 0x2) {
    time = 128 * (count_t + 1) * multiplier;
  } else if (unit & 0x3) {
    time = 1 * (count_t + 1) * multiplier * 1000; //Converting seconds to ms
  }
  return time;
}

static uint32_t SFDP_getSectorSize(void)
{
  uint32_t ptp = SFDP_getPTP();
  spi_setCsActive();

  //Querying the 8th DWORD in PTP(0)
  SFDP_sendCommand(ptp + 0x1C);

  uint32_t type1 = spi_readByte();

  //This field specifies 'N' and is used to calculate the erase type size = 2^N
  uint32_t sectorsize = (1U << type1);
  spi_setCsInactive();
  return sectorsize;
}

static uint32_t SFDP_getPageSize()
{
  uint32_t ptp = SFDP_getPTP();
  spi_setCsActive();

  //Querying the 11th DWORD of the PTP(0)
  SFDP_sendCommand(ptp + 0x28);

  //Page Size : D[7:4]
  uint32_t byte1 = spi_readByte() >> 4;

  //This field specifies 'N' and is used to calculate page size = 2^N bytes.
  uint32_t pageSize = 1U << byte1;
  spi_setCsInactive();
  return pageSize;
}

static uint32_t getDeviceSize(void)
{
  waitUntilNotBusy();

  uint32_t deviceSize;
  uint32_t ptp = SFDP_getPTP(); //find parameter table pointer.

  uint16_t size_N;
  spi_setCsActive();

  //Querying the 2nd DWORD of PTP(0)
  SFDP_sendCommand(ptp + 0x04);

  deviceSize = spi_readByte();
  deviceSize |= spi_readByte() << 8;
  deviceSize |= spi_readByte() << 16;
  deviceSize |= spi_readByte() << 24;
  spi_setCsInactive();

  //For densities 2 gigabits or less, bit-31 is set to 0b.
  //The field 30:0 defines the size in bits. Example: 00FFFFFFh = 16 megabits
  //For densities 4 gigabits and above, bit-31 is set to 1b.
  //The field 30:0 defines ‘N’ where the density is computed as 2^N bits (N must be >= 32).
  //Example: 80000021h = 2^33 = 8 gigabits

  uint32_t high_bit = deviceSize & (1U << 31);

  if (high_bit) { // if 1: [30:0] bits are N in 2^N. Should maybe check if N>32.
    size_N = deviceSize & ~(1U << 31);
    //Return flash size in bytes
    return deviceSize = ((1U << size_N) + 1) / 8;
  } else {
    //Return flash size in bytes
    return (deviceSize + 1) / 8;
  }
}

static bool verifyAddressRange(uint32_t                address,
                               uint32_t                length)
{
  uint32_t deviceSize = getDeviceSize();
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

//highest supported erase size
static uint32_t SFDP_eraseSize(void)
{
  uint32_t ptp = SFDP_getPTP();
  spi_setCsActive();

  //Querying the 8th DWORD of PTP(0)
  SFDP_sendCommand(ptp + 0x1C);
  uint32_t type1 = spi_readByte();
  spi_readByte();
  uint32_t type2 = spi_readByte();
  spi_readByte();
  //Querying the 9th DWORD of PTP(0)
  uint32_t type3 = spi_readByte();
  spi_readByte();
  uint32_t type4 = spi_readByte();
  spi_setCsInactive();

  //This field specifies ‘N’ and is used to calculate  erase type size = 2^N bytes
  if (type4) {
    return (1U << type4);
  } else if (type3) {
    return (1U << type3);
  } else if (type2) {
    return (1U << type2);
  } else {
    return (1U << type1);
  }
}

//Check if sectorerase is supported
static bool SFDP_sectorErase(void)
{
  uint8_t supported;
  uint32_t ptp = SFDP_getPTP(); //find parameter table pointer.
  spi_setCsActive();

  //Querying the 1st DWORD of PTP(0)
  SFDP_sendCommand(ptp);
  // Bits [1:0]
  supported = (spi_readByte() & 0x3);
  spi_setCsInactive();

  //01b: 4 kilobyte Erase is supported throughout the device
  if (supported & 0x1) {
    return true;
  } else {
    return false;
  }
}

BootloaderStorageImplementationInformation_t getDeviceInfo(void)
{
  BootloaderStorageImplementationInformation_t flashInfo;

  flashInfo.version = BOOTLOADER_STORAGE_IMPL_INFO_VERSION;
  flashInfo.capabilitiesMask = (BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED
                                | BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED);
  flashInfo.pageEraseMs = SFDP_4KMaxEraseTime();
  flashInfo.partEraseMs = SFDP_maxEraseChipTime();
  flashInfo.pageSize = SFDP_getSectorSize();
  //Check if 4K sector erase is supported by the device
  if (!SFDP_sectorErase()) {
    flashInfo.pageSize = SFDP_eraseSize();
  }
  flashInfo.partSize = getDeviceSize();
  flashInfo.partDescription = NULL;
  flashInfo.wordSizeBytes = DEVICE_WORD_SIZE;
  flashInfo.partType = BOOTLOADER_STORAGE_JEDEC;
  return flashInfo;
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
    case MFG_ID_ATMEL:
      // also MFG_ID_ADESTO
      switch (deviceId) {
#if defined(BTL_STORAGE_SPIFLASH_ATMEL_AT25DF041A)
        case DEVICE_ID_ATMEL_4M:
          return ATMEL_4M_DEVICE;
#endif
#if defined(BTL_STORAGE_SPIFLASH_ATMEL_AT25DF081A)
        case DEVICE_ID_ATMEL_8M:
          return ATMEL_8M_DEVICE;
#endif
        default:
          return UNKNOWN_DEVICE;
      }
    default:
      return UNKNOWN_DEVICE;
  }
}

int32_t storage_init(void)
{
  //Setup hardware configs
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

  //Check support for JEDEC SFDP support. Return error if not supported.
  uint32_t ret =  SFDP_checkSupport();
  if (ret != BOOTLOADER_OK) {
    return ret;
  }

  StorageSpiflashDevice_t deviceType;
  deviceType = getDeviceType();

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
  if (!verifyAddressRange(address, length)) {
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
  uint32_t pageSize = SFDP_getPageSize();
  uint32_t pageMask = pageSize - 1;
  // Ensure address is is within chip
  if (!verifyAddressRange(address, numBytes)) {
    return BOOTLOADER_ERROR_STORAGE_INVALID_ADDRESS;
  }
  // Ensure space is empty
  if (!verifyErased(address, numBytes)) {
    return BOOTLOADER_ERROR_STORAGE_NEEDS_ERASE;
  }

  if (address & pageMask) {
    // handle unaligned first block
    nextPageAddr = (address & (~pageMask)) + pageSize;
    if ((address + numBytes) < nextPageAddr) {
      // fits all within first block
      currentLength = numBytes;
    } else {
      currentLength = (uint16_t) (nextPageAddr - address);
    }
  } else {
    currentLength = (numBytes > pageSize) ? pageSize : numBytes;
  }
  while (numBytes) {
    writePage(address, data, currentLength);
    numBytes -= currentLength;
    address += currentLength;
    data += currentLength;
    currentLength = (numBytes > pageSize) ? pageSize : numBytes;
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
  uint32_t sectorSize = SFDP_getSectorSize();
  uint32_t sectorMask = sectorSize - 1;
  uint32_t deviceSize = getDeviceSize();
  uint32_t deviceBlockSize = SFDP_eraseSize();
  uint32_t deviceBlockMask = deviceBlockSize - 1;

  //Check if device only support block erase and not sector.
  if (!SFDP_sectorErase()) {
    sectorMask = deviceBlockMask;
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
  if (!verifyAddressRange(address, totalLength)) {
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
    address += sectorSize;
    totalLength -= sectorSize;
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
    address += sectorSize;
    totalLength -= sectorSize;
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
