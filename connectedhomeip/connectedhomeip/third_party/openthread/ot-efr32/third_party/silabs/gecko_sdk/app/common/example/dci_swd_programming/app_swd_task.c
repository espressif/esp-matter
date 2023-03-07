/***************************************************************************//**
 * @file app_swd_task.c
 * @brief SWD interface task functions.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_swd_task.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Erases enough pages of flash to fit a image of the given size.
 *
 * @param start_addr Start address to erase (must in multiple of page size).
 * @param size Firmware image size in bytes.
 *
 * @note The size of flash pages is retrieved from the DI page and the pages are
 *       erased by writing directly to the MSC registers.
 ******************************************************************************/
static void erase_flash_page(uint32_t start_addr, uint32_t size);

/***************************************************************************//**
 * Writes a firmware image to internal flash of the target using direct writes
 * to the MSC registers.
 *
 * @param start_addr Start address to write (must in multiple of page size).
 * @param size Firmware image size in bytes.
 * @param ptr Pointer to firmware image address.
 ******************************************************************************/
static void write_fw_image(uint32_t start_addr, uint32_t size, uint32_t *ptr);

/***************************************************************************//**
 * Verifies the current firmware against the internal memory.
 *
 * @param start_addr Start address to verify (must in multiple of page size).
 * @param size Firmware image size in bytes.
 * @param ptr Pointer to firmware image address.
 ******************************************************************************/
static void verify_fw_image(uint32_t start_addr, uint32_t size, uint32_t *ptr);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Target device flash start address
static uint32_t flash_start_addr;

/// Target device flash size
static uint32_t flash_size;

/// Target device flash page size
static uint32_t flash_page_size;

/// Target device user data size
static uint32_t user_data_size;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Get start address of the device flash.
 ******************************************************************************/
uint32_t get_flash_start_addr(void)
{
  return(flash_start_addr);
}

/***************************************************************************//**
 * Retrieve the device information from the DI page of the target.
 ******************************************************************************/
void get_device_info(char *name, uint64_t *uid)
{
  uint8_t alpha;
  uint32_t buf0;
  uint32_t buf1;
  uint32_t buf2;

  // Set default flash start address to 0
  flash_start_addr = FLASH_MEM_BASE;

  // Get part information
  buf0 = read_mem((uint32_t)&(DEVINFO->PART));

  // Get device number
  alpha = 'A';
  buf1 = buf0 & _DEVINFO_PART_DEVICENUM_MASK;
  while (buf1 > 1000) {
    alpha++;
    buf1 -= 1000;
  }

  // Get user and flash page size
  buf2 = read_mem((uint32_t)&(DEVINFO->MEMINFO));
  user_data_size = ((buf2 & _DEVINFO_MEMINFO_UDPAGESIZE_MASK) >> _DEVINFO_MEMINFO_UDPAGESIZE_SHIFT) * 1024;
  buf2 = (buf2 & _DEVINFO_MEMINFO_FLASHPAGESIZE_MASK) >> _DEVINFO_MEMINFO_FLASHPAGESIZE_SHIFT;
  flash_page_size = 1 << ((buf2 + 10) & 0xff);

  // Get main flash size
  buf2 = read_mem((uint32_t)&(DEVINFO->MSIZE));
  buf2 = (buf2 & _DEVINFO_MSIZE_FLASH_MASK) >> _DEVINFO_MSIZE_FLASH_SHIFT;
  flash_size = buf2 * 1024;

  if ((buf0 & _DEVINFO_PART_FAMILY_MASK) == DEVINFO_PART_FAMILY_FG) {
    sprintf(name, "%s%2lu%c%03luF%lu", "EFR32FG",
            (buf0 & _DEVINFO_PART_FAMILYNUM_MASK) >> _DEVINFO_PART_FAMILYNUM_SHIFT,
            alpha, buf1, buf2);
  } else if ((buf0 & _DEVINFO_PART_FAMILY_MASK) == DEVINFO_PART_FAMILY_MG) {
    sprintf(name, "%s%2lu%c%03luF%lu", "EFR32MG",
            (buf0 & _DEVINFO_PART_FAMILYNUM_MASK) >> _DEVINFO_PART_FAMILYNUM_SHIFT,
            alpha, buf1, buf2);
  } else if ((buf0 & _DEVINFO_PART_FAMILY_MASK) == DEVINFO_PART_FAMILY_BG) {
    sprintf(name, "%s%2lu%c%03luF%lu", "EFR32BG",
            (buf0 & _DEVINFO_PART_FAMILYNUM_MASK) >> _DEVINFO_PART_FAMILYNUM_SHIFT,
            alpha, buf1, buf2);
  } else if ((buf0 & _DEVINFO_PART_FAMILY_MASK) == ((_DEVINFO_PART_FAMILY_BG + 1) << 24)) {
    sprintf(name, "%s%2lu%c%03luF%lu", "EFR32ZG",
            (buf0 & _DEVINFO_PART_FAMILYNUM_MASK) >> _DEVINFO_PART_FAMILYNUM_SHIFT,
            alpha, buf1, buf2);
  } else if ((buf0 & _DEVINFO_PART_FAMILY_MASK) == DEVINFO_PART_FAMILY_PG) {
    sprintf(name, "%s%2lu%c%03luF%lu", "EFM32PG",
            (buf0 & _DEVINFO_PART_FAMILYNUM_MASK) >> _DEVINFO_PART_FAMILYNUM_SHIFT,
            alpha, buf1, buf2);
  } else {
    RAISE(SWD_ERROR_UNKNOWN_DEVICE);
  }

  // Retrieve high and low part of unique ID
  *uid = read_mem((uint32_t)&(DEVINFO->EUI64H));
  *uid = *uid << 32;
  *uid |= read_mem((uint32_t)&(DEVINFO->EUI64L));

  // Enable MSC clock if device is xG22
  buf0 &= _DEVINFO_PART_FAMILYNUM_MASK;
  if (buf0 == XG22_FAMILY) {
    write_mem((uint32_t)&(CMU->CLKEN1_SET), CMU_CLKEN1_MSC);
    // Check UDLOCKBIT on xG22
    if (read_mem((uint32_t)&(MSC->MISCLOCKWORD)) & MSC_MISCLOCKWORD_UDLOCKBIT) {
      RAISE(SWD_ERROR_USERDATA_LOCK);
    }
  }

  // Enable MSC clock if device is xG23
  if (buf0 == XG23_FAMILY) {
    write_mem((uint32_t)&(CMU->CLKEN1_SET), CLKEN1_MSC_XG23);
    // Check UDLOCKBIT on xG23
    if (read_mem((uint32_t)&(MSC->MISCLOCKWORD)) & MSC_MISCLOCKWORD_UDLOCKBIT) {
      RAISE(SWD_ERROR_USERDATA_LOCK);
    }
    // Flash start address for xG23
    flash_start_addr = FLASH_BASE_XG23;
  }

  // Check MELOCKBIT on device
  if (read_mem((uint32_t)&(MSC->MISCLOCKWORD)) & MSC_MISCLOCKWORD_MELOCKBIT) {
    RAISE(SWD_ERROR_MASSERASE_LOCK);
  }
}

/***************************************************************************//**
 * Verifies the user data is cleared or against the internal memory.
 ******************************************************************************/
void verify_user_data(uint32_t *ptr)
{
  uint32_t addr;

  if (ptr == NULL) {
    // Blank check if NULL pointer
    addr = USERDATA_BASE;
    while (addr < (USERDATA_BASE + user_data_size)) {
      if (read_mem(addr) != 0xFFFFFFFF) {
        RAISE(SWD_ERROR_DEVICE_ERASE_FAILED);
      }
      addr += 4;
    }
  } else {
    verify_fw_image(USERDATA_BASE, user_data_size, (uint32_t *)ptr);
  }
}

/***************************************************************************//**
 * Erase the flash and verify that the flash is cleared.
 ******************************************************************************/
uint32_t erase_flash(bool userdata)
{
  uint32_t addr;

  DWT->CYCCNT = 0;

  if (userdata) {
    erase_flash_page(USERDATA_BASE, user_data_size);
    addr = USERDATA_BASE;
    while (addr < (USERDATA_BASE + user_data_size)) {
      if (read_mem(addr) != 0xFFFFFFFF) {
        RAISE(SWD_ERROR_DEVICE_ERASE_FAILED);
      }
      addr += 4;
    }
  } else {
    erase_flash_page(flash_start_addr, flash_size);
    addr = flash_start_addr;
    while (addr < flash_start_addr + flash_size) {
      if (read_mem(addr) != 0xFFFFFFFF) {
        RAISE(SWD_ERROR_DEVICE_ERASE_FAILED);
      }
      addr += flash_page_size;
    }
  }
  return(DWT->CYCCNT);
}

/***************************************************************************//**
 * Uses direct writes to MSC registers in order to program the flash.
 ******************************************************************************/
uint32_t prog_flash(uint32_t start_addr, uint32_t size, uint32_t *ptr)
{
  // Check start address is page aligned
  if ((start_addr & (flash_page_size - 1)) != 0) {
    RAISE(SWD_ERROR_WRONG_START_ADDR);
  }

  // Check size is within range
  if (start_addr == USERDATA_BASE) {
    if (size > user_data_size) {
      RAISE(SWD_ERROR_BINARY_SIZE);
    }
  } else {
    if ((start_addr + size) > (flash_start_addr + flash_size)) {
      RAISE(SWD_ERROR_BINARY_SIZE);
    }
  }

  DWT->CYCCNT = 0;

  // Erase all pages that we are going to write to
  erase_flash_page(start_addr, size);

  // Write firmware image from internal buffer
  write_fw_image(start_addr, size, ptr);

  // Verify application
  verify_fw_image(start_addr, size, ptr);

  return(DWT->CYCCNT);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Erases enough pages of flash to fit a image of the given size.
 ******************************************************************************/
static void erase_flash_page(uint32_t start_addr, uint32_t size)
{
  uint32_t timeout;
  uint32_t msc_status;

  // Enable write in MSC
  write_mem((uint32_t)&(MSC->WRITECTRL), MSC_WRITECTRL_WREN);

  // Adjust size with start address
  size += start_addr;

  // Not start from flash start address
  if (start_addr != flash_start_addr) {
    // Loop until enough pages have been erased
    while (start_addr < size) {
      // Enter the start address of the page
      write_mem((uint32_t)&(MSC->ADDRB), start_addr);

      // Start page erase operation
      write_mem((uint32_t)&(MSC->WRITECMD), MSC_WRITECMD_ERASEPAGE);

      // Wait until erase is complete
      sl_udelay_wait(ERASE_DELAY);
      timeout = ERASE_LOOPCNT;
      do {
        msc_status = read_mem((uint32_t)&(MSC->STATUS));
        timeout--;
      } while ((msc_status & MSC_STATUS_BUSY) && (timeout > 0));

      if (msc_status & MSC_STATUS_BUSY) {
        RAISE(SWD_ERROR_FLASH_WRITE_FAILED);
      }

      // Move to next page
      start_addr += flash_page_size;
    }
  } else {
    // Address is flash start address, unlock mass erase
    write_mem((uint32_t)&(MSC->LOCK), MSC_LOCK_LOCKKEY_UNLOCK);

    // Start mass erase operation
    write_mem((uint32_t)&(MSC->WRITECMD), MSC_WRITECMD_ERASEMAIN0);

    // Wait until erase is complete
    sl_udelay_wait(ERASE_DELAY);
    timeout = ERASE_LOOPCNT;
    do {
      msc_status = read_mem((uint32_t)&(MSC->STATUS));
      timeout--;
    } while ((msc_status & MSC_STATUS_BUSY) && (timeout > 0));

    if (msc_status & MSC_STATUS_BUSY) {
      RAISE(SWD_ERROR_FLASH_WRITE_FAILED);
    }
  }
}

/***************************************************************************//**
 * Writes a firmware image to internal flash of the target using direct writes
 * to the MSC registers.
 ******************************************************************************/
static void write_fw_image(uint32_t start_addr, uint32_t size, uint32_t *ptr)
{
  uint32_t w;
  uint32_t flash_page_size_mask;

  if (start_addr == USERDATA_BASE) {
    flash_page_size_mask = user_data_size - 1;
  } else {
    flash_page_size_mask = flash_page_size - 1;
  }

  // Adjust size with start address
  size += start_addr;

  // Start writing all words from the internal buffer
  for (w = start_addr; w < size; w += 4) {
    if ((w & flash_page_size_mask) == 0) {
      // Enter address
      write_mem((uint32_t)&(MSC->ADDRB), w);
    }

    // Write value to WDATA
    write_mem((uint32_t)&(MSC->WDATA), *ptr++);

#if (SKIP_POLLING == FALSE)
    uint32_t timeout;
    uint32_t msc_status;

    // Wait for write to complete
    timeout = MSC_TIMEOUT;
    do {
      msc_status = read_mem((uint32_t)&(MSC->STATUS));
      timeout--;
    } while (((msc_status & MSC_STATUS_WDATAREADY) == 0) && timeout);

    if (timeout == 0) {
      RAISE(SWD_ERROR_FLASH_WRITE_FAILED);
    }
#else
    sl_udelay_wait(WRITE_DELAY);
#endif
  }
}

/***************************************************************************//**
 * Verifies the current firmware against the internal memory.
 ******************************************************************************/
static void verify_fw_image(uint32_t start_addr, uint32_t size, uint32_t *ptr)
{
  uint32_t w;
  uint32_t value;

  // Adjust size with start address
  size += start_addr;

  // Set auto-increment on TAR
  write_ap(AP_CSW, AP_CSW_DEFAULT | AP_CSW_AUTO_INCREMENT);

  for (w = start_addr; w < size; w += 4, ptr++) {
    // TAR must be initialized at every TAR wrap boundary
    if ((w & TAR_WRAP_1K) == 0) {
      write_ap(AP_TAR, w);
      // Do one dummy read and subsequent reads will return the correct result
      read_ap(AP_DRW, &value);
    }

    // Read the value from address
    read_ap(AP_DRW, &value);

    // Verify that the read value matches what is expected
    if (value != *ptr) {
      RAISE(SWD_ERROR_VERIFY_FW_FAILED);
    }
  }

  // Disable auto-increment on TAR
  write_ap(AP_CSW, AP_CSW_DEFAULT);
}
