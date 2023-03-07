/***************************************************************************//**
 * @file sl_token_manufacturing.c
 * @brief Manufacturing toke API implementation.
 * @version 1.0.0
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "sl_status.h"
#include "sl_token_api.h"
#include "sl_token_manager.h"
#include "sl_token_manufacturing_api.h"
#include "em_msc.h"

#if (_SILICON_LABS_32B_SERIES_2_CONFIG == 1)
#include "sl_se_manager_util.h"
#endif // (_SILICON_LABS_32B_SERIES_2_CONFIG == 1)

#define EMBER_SUCCESS (0x00u)
#define EMBER_ERR_FATAL (0x01u)
#define EMBER_ERR_FLASH_PROG_FAIL (0x4Bu)

#define DEFINETOKENS
#define TOKEN_MFG(name, creator, iscnt, isidx, type, arraysize, ...) \
  const uint16_t TOKEN_##name = TOKEN_##name##_ADDRESS;
// Multiple inclusion of unguarded token-related header files is by design; suppress violation.
//cstat !MISRAC2012-Dir-4.10
  #include "sl_token_manufacturing.h"
#undef TOKEN_DEF
#undef TOKEN_MFG
#undef DEFINETOKENS

#if defined(_SILICON_LABS_32B_SERIES_1)
  #define SILABS_DEVINFO_EUI64_LOW   (DEVINFO->UNIQUEL)
  #define SILABS_DEVINFO_EUI64_HIGH  (DEVINFO->UNIQUEH)
#elif defined(_SILICON_LABS_32B_SERIES_2)
  #include "em_se.h"
  #define SILABS_DEVINFO_EUI64_LOW   (DEVINFO->EUI64L)
  #define SILABS_DEVINFO_EUI64_HIGH  (DEVINFO->EUI64H)
#else
  #error Error: this micro is not yet supported by the manufacturing token code
#endif

static const uint8_t nullEui[] = { 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU };

static void getMfgTokenData(void *data,
                            uint16_t token,
                            uint8_t index,
                            uint32_t len)
{
  uint8_t *ram = (uint8_t*)data;

  //0x7F is a non-indexed token.  Remap to 0 for the address calculation
  index = (index == 0x7FU) ? 0U : index;

  if (token == MFG_EUI_64_LOCATION) {
    //There are two EUI64's stored in the flash, Ember and Custom.
    //MFG_EUI_64_LOCATION is the address used by the generic EUI64 token.
    //It is the low level routine's responbility to pick and return EUI64 from
    //either Ember or Custom.  Return the Custom EUI64 if it is not all FF's,
    //otherwise return the Ember EUI64.
    tokTypeMfgEui64 eui64;
    halCommonGetMfgToken(&eui64, TOKEN_MFG_CUSTOM_EUI_64);
    if (memcmp(eui64, nullEui, 8 /*EUI64_SIZE*/) == 0) {
      halCommonGetMfgToken(&eui64, TOKEN_MFG_EMBER_EUI_64);
    }
    memcpy(ram, eui64, 8 /*EUI64_SIZE*/);
  } else if (token == MFG_EMBER_EUI_64_LOCATION) {
    uint32_t low = SILABS_DEVINFO_EUI64_LOW;
    uint32_t high = SILABS_DEVINFO_EUI64_HIGH;
    uint8_t i = 0U;
    while ((i < 4U) && (len > 0U)) {
      ram[i] = low & 0xFFU;
      low >>= 8;
      len--;
      i++;
    }
    while ((i < 8U) && (len > 0U)) {
      ram[i] = high & 0xFFU;
      high >>= 8;
      len--;
      i++;
    }
  } else if ((token & 0xF000) == (USERDATA_TOKENS & 0xF000)) {
    uint32_t realAddress = ((USERDATA_BASE + (token & 0x0FFF)) + (len * index));
    uint8_t *flash = (uint8_t *)realAddress;

    memcpy(ram, flash, len);
  } else if (((token & 0xF000) == (LOCKBITS_TOKENS & 0xF000))
             || ((token & 0xF000) == (LOCKBITSDATA_TOKENS & 0xF000))) {
    uint32_t realAddress = ((LOCKBITS_BASE + (token & 0x0FFF)) + (len * index));
    uint8_t *flash = (uint8_t *)realAddress;

    memcpy(ram, flash, len);
  } else {
    // Sate MISRA
  }
}

/***************************************************************************//**
 * Read the data associated with the specified manufacturing token.
 ******************************************************************************/
sl_status_t sl_token_get_manufacturing_data(uint32_t token,
                                            uint32_t index,
                                            void *data,
                                            uint32_t length)
{
  if (length == 0U) {
    return SL_STATUS_INVALID_PARAMETER; // Nothing to do...
  }
  if (token == MFG_EUI_64_LOCATION) {
    //There are two EUI64's stored in the flash, Ember and Custom.
    //MFG_EUI_64_LOCATION is the address used by the generic EUI64 token.
    //It is the low level routine's responbility to pick and return EUI64 from
    //either Ember or Custom.  Return the Custom EUI64 if it is not all FF's,
    //otherwise return the Ember EUI64.
    if (length > sizeof(nullEui)) {
      length = sizeof(nullEui);
    }
    getMfgTokenData(data, MFG_CUSTOM_EUI_64_LOCATION, 0x7FU, length);
    if (memcmp(data, nullEui, length) != 0) {
      return SL_STATUS_OK;
    }
    token = MFG_EMBER_EUI_64_LOCATION;
  }
  getMfgTokenData(data, token, index, length);

  return SL_STATUS_OK;
}

/**
 * @brief Assign numerical value to the type of erasure requested.
 */
#define MFB_MASS_ERASE 0x01

// The parameter 'eraseType' chooses which erasure will be performed while
// the 'address' parameter chooses the page to be erased during MFB page erase.
EmberStatus halInternalFlashErase(uint8_t eraseType, uint32_t address)
{
  msc_Return_TypeDef ret = mscReturnOk;

  MSC_Init();
  if (eraseType == MFB_MASS_ERASE) {
#if defined (_SILICON_LABS_32B_SERIES_2)
    // masserase is currently not support on Series2 devices.
    return EMBER_ERR_FATAL;
#else
    ret = MSC_MassErase();
#endif // !defined (_SILICON_LABS_32B_SERIES_2)
  } else {
    ret = MSC_ErasePage((uint32_t *) address);
  }

  /* Check for errors. If there are errors, set the global error variable and
   * de-initialize the MSC */
  if (ret != mscReturnOk) {
    MSC_Deinit();
    return EMBER_ERR_FATAL;
  }

  /* De-initialize the MSC. This disables writing and locks the MSC */
  MSC_Deinit();

  return EMBER_SUCCESS;
}

// Helper function to create and write a 32b word from incoming 16b
// word/address.
// Does not check that address16 is properly word-aligned - assumes that the
// caller has already done this.

 #if !defined(_SILICON_LABS_32B_SERIES_2)
static msc_Return_TypeDef halInternalFlashWrite16(uint32_t address16, uint16_t data16)
{
  uint32_t           newData32;

  // Figure out corresponding 32 bit address, remove lowest 2 bits
  uint32_t address32 = address16 & 0xFFFFFFFCU;

  // Put 16bit data in 32b data.
  // According to the EFM32 reference manual, any flash bits that aren't being
  // changed should be written as '1'.
  if (address16 & 2) {
    newData32 = (0x0000FFFFU | (((uint32_t)data16) << 16));
  } else {
    newData32 = (0xFFFF0000U | data16);
  }

  return MSC_WriteWord((uint32_t *)address32, &newData32, 4);
}
 #endif //_SILICON_LABS_32B_SERIES_2

// The parameter 'address' defines the starting address of where the
// programming will occur - this parameter MUST be half-word aligned since all
// programming operations are HW.  The parameter 'data' is a pointer to a buffer
// containing the 16bit half-words to be written.  Length is the number of 16bit
// half-words contained in 'data' to be written to flash.
// NOTE: This function can NOT write the option bytes and will throw an error
// if that is attempted.
EmberStatus halInternalFlashWrite(uint32_t address, uint16_t *data, uint32_t length)
{
 #if defined(_SILICON_LABS_32B_SERIES_1)
  uint32_t byteCount, wordCount;
  uint16_t *dp16, *fp16;

  if (((uint32_t)data & 1U) != 0U || (address & 1U) != 0U) {
    return EMBER_ERR_FATAL; //UNALIGNED;
  }

  if (length > 0) {
    // First, do a pass over flash and data and verify that the current flash
    // value is 0xFFFF OR the data to be written is 0.  If this is untrue for
    // any 16-bit word that is being written, return a failure without writing
    // anything

    fp16      = (uint16_t *)address;
    dp16      = data;
    wordCount = length + 1;

    while (--wordCount) {
      if (*fp16 != 0xFFFFU && *dp16 != 0) {
        return EMBER_ERR_FLASH_PROG_FAIL;
      }
      ++fp16;
      ++dp16;
    }

    MSC_Init();

    // Is this a single 16-bit write or starting the write not aligned to
    // a longword address?

    if ((length == 1) || (address & 2U) != 0U) {
      if (halInternalFlashWrite16(address, *data) != mscReturnOk) {
        MSC_Deinit();
        return EMBER_ERR_FLASH_PROG_FAIL;
      }

      address += 2;
      ++data;
      --length;
    }

    if (length > 1) {
      // Calculate the number of 32-bit longwords we want to write, and
      // calculate the corresponding number of bytes.  If there is an odd
      // number of 16-bit words, ignore the last word for now.

      wordCount = (length & 0xFFFFFFFEU);
      byteCount = wordCount * 2;

      if (MSC_WriteWord((uint32_t *)address, (void const *)data, byteCount) != mscReturnOk) {
        MSC_Deinit();
        return EMBER_ERR_FLASH_PROG_FAIL;
      }

      address += byteCount;    // Address is bytes,
      data    += wordCount;    // But data pointer is a uint16_t *
      length  -= wordCount;
    }

    // Do we have an odd number of 16-bit words?
    if (length > 0) {
      // Yes, write the final 16-bit word as an update to the 32-bit
      // word that contains it.
      if (halInternalFlashWrite16(address, *data) != mscReturnOk) {
        MSC_Deinit();
        return EMBER_ERR_FLASH_PROG_FAIL;
      }
    }
    MSC_Deinit();
  }

 #elif defined(_SILICON_LABS_32B_SERIES_2)
  // halInternalFlashWriteSeries2 should be called for more efficient writing, the following
  // is for backwards compatibility
  uint32_t byteCount, *fp32, i, wordToWrite;
  uint16_t *dp16;

  // check if even number of 16 bit half words
  if ((length & 1U) != 0U) {
    return EMBER_ERR_FATAL;
  }

  // adjust length to number of 32 bit words
  length >>= 1;

  // hardcode byteCount to 4
  byteCount = 4U;

  // check if data address and address are 4 byte aligned
  if ((address & 3U) != 0U) {
    return EMBER_ERR_FATAL; //UNALIGNED;
  }

  // First, do a pass over flash and data and verify that the current flash
  // value is 0xFFFFFFFF.  If this is untrue for any 32-bit word that is being
  // written, return a failure without writing anything
  fp32      = (uint32_t *)address;
  i = 0;
  while (i < length) {
    if (*fp32 != 0xFFFFFFFFU) {
      return EMBER_ERR_FLASH_PROG_FAIL;
    }
    ++fp32;
    ++i;
  }

  i = 0;
  fp32 = (uint32_t *)address;
  dp16 = data;

  MSC_Init();

  // write data in 32 bit (1 word) increments
  while (i < length) {
    // create 32 bit wordToWrite and increment data address
    wordToWrite = (uint32_t)*dp16;
    ++dp16;
    wordToWrite = (wordToWrite) | ((uint32_t)*dp16 << 16);
    ++dp16;

    // attempt to write word
    if (MSC_WriteWord(fp32, (void const *)&wordToWrite, byteCount) != mscReturnOk) {
      MSC_Deinit();
      return EMBER_ERR_FLASH_PROG_FAIL;
    }

    // increment flash address
    ++fp32;

    // increment counter
    ++i;
  }

  MSC_Deinit();

 #else
 #error Unknown device series
 #endif

  return EMBER_SUCCESS;
}

EmberStatus halInternalFlashWrite(uint32_t address, uint16_t *data, uint32_t length);

#if (_SILICON_LABS_32B_SERIES == 2)
// Odd len is not supported by this function
static void flashWrite(uint32_t realAddress, void *data, uint32_t len)
{
  EmberStatus flashStatus = EMBER_ERR_FATAL;
  flashStatus = halInternalFlashWrite(realAddress, (uint16_t*)data, (len / 2));
  assert(flashStatus == EMBER_SUCCESS);
}
#endif

#if (_SILICON_LABS_32B_SERIES_2_CONFIG == 1)
static void flashWriteSE(uint32_t realAddress, void *data, uint32_t len)
{
  if ((realAddress & USERDATA_BASE) == USERDATA_BASE) {
    sl_status_t status = SL_STATUS_OK;
    sl_se_command_context_t cmd_ctx;
    status = sl_se_write_user_data(&cmd_ctx, (realAddress & 0x0FFF), data, len);
    assert(status == SL_STATUS_OK);
  } else {
    flashWrite(realAddress, data, len);
  }
}
#endif // (_SILICON_LABS_32B_SERIES_2_CONFIG == 1)

#if (_SILICON_LABS_32B_SERIES == 2)
  #if (_SILICON_LABS_32B_SERIES_2_CONFIG == 1)
    #define FLASHWRITE(realAddress, data, len) \
  (flashWriteSE((realAddress), (data), (len)))
  #elif (_SILICON_LABS_32B_SERIES_2_CONFIG == 2) \
  || (_SILICON_LABS_32B_SERIES_2_CONFIG == 3)    \
  || (_SILICON_LABS_32B_SERIES_2_CONFIG == 4)    \
  || (_SILICON_LABS_32B_SERIES_2_CONFIG == 7)
    #define FLASHWRITE(realAddress, data, len) \
  (flashWrite((realAddress), (data), (len)))
  #else
    #error Unknown device configuration
  #endif
#elif (_SILICON_LABS_32B_SERIES == 1)
//Remember, the flash library's token support operates in 16bit quantities,
//but the token system operates in 8bit quantities.  Hence the divide by 2.
//NOTE: The actual flash for the EFM32 device prefer to work with 32b writes,
//but 16b is possible and supported by the flash driver - primarily for the
//purposes of the SimEE/Token system that likes to use 8b and 16b.
// THIS LENGTH IS A 32 BIT LENGTH. Odd len is not supported by this define.
  #define FLASHWRITE(realAddress, data, len) \
  assert(halInternalFlashWrite((realAddress), (data), (len) / 2) == EMBER_SUCCESS)
#else
  #error Unknown device series
#endif

#if (_SILICON_LABS_32B_SERIES == 2)
static void writeStartWord(uint32_t realAddress, void *data)
{
  uint32_t startWordAddress, startWordToWrite;
  startWordAddress = realAddress - 2;
  // create word to write buffer bytes and first 2 bytes of token
  startWordToWrite = 0x0000FFFF | (*(uint16_t *)data << 16);
  FLASHWRITE(startWordAddress, &startWordToWrite, 4);
}

static void writeEndWord(uint32_t realAddress, void *data, uint32_t len)
{
  uint32_t endWordAddress, endWordToWrite;
  // address for end word at aligned address where the last two bytes of data would
  // be written
  endWordAddress = realAddress + len;
  // create word to write last 2 bytes of token and buffer bytes
  endWordToWrite = 0xFFFF0000 | *((uint16_t *)data + len / 2);
  FLASHWRITE(endWordAddress, &endWordToWrite, 4);
}
#endif

/***************************************************************************//**
 * Writes data to a manufacturing token.
 *
 * @note Only manufacturing token values that have not been written since
 * the last erase can be written.  For areas of flash that cannot be erased
 * by user code, those manufacturing tokens are effectively write-once.
 *
 * @param token The NVM3KEY or SimEE CREATOR define for the token.
 * @param data A pointer to the data being written.
 * @param length The size of the token data in number of bytes.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
sl_status_t sl_token_set_manufacturing_data(uint32_t token,
                                            void *data,
                                            uint32_t length)
{
  uint32_t realAddress = 0;
  //Initializing to a high memory address adds protection by causing a
  //hardfault if accidentally used.
  uint8_t *flash = (uint8_t *)0xFFFFFFF0U;
  uint32_t i;
  //The flash library requires the address and length to both
  //be multiples of 16bits.  Since this API is only valid for writing to
  //the UserPage or LockBits page, verify that the token+len falls within
  //acceptable addresses..
  assert((token & 1) != 1);
  assert((length & 1) != 1);

  if ((token & 0xF000) == (USERDATA_TOKENS & 0xF000)) {
    realAddress = ((USERDATA_BASE + (token & 0x0FFF)));
    flash = (uint8_t *)realAddress;
    assert((realAddress >= USERDATA_BASE) && ((realAddress + length - 1) < (USERDATA_BASE + FLASH_PAGE_SIZE)));
  } else if (((token & 0xF000) == (LOCKBITS_TOKENS & 0xF000))
             || ((token & 0xF000) == (LOCKBITSDATA_TOKENS & 0xF000))) {
    realAddress = ((LOCKBITS_BASE + (token & 0x0FFF)));
    flash = (uint8_t *)realAddress;
  } else {
    //in coming token parameter is an illegal address.
    assert(0);
  }

  //UserData and LockBits manufacturing tokens can only be written by on-chip
  //code if the token is currently unprogrammed.  Verify the entire token is
  //unwritten.  The flash library performs a similar check, but verifying here
  //ensures that the entire token is unprogrammed and will prevent partial
  //writes.
  //NOTE:  On chip code CANNOT erase LockBits Page.
  for (i = 0; i < length; i++) {
    assert(flash[i] == 0xFF);
  }

  // sate any compiler warnings about unused variable.
  (void) flash;

  #if (_SILICON_LABS_32B_SERIES == 2)
  // if address is 2 byte aligned instead of 4, write two buffer bytes of 0xFFFF with
  // the first 2 bytes of data separately from the rest of data 2 bytes before
  // realAddress. There is buffer space built into the token map to account for these
  // extra two bytes at the beginning
  if ((realAddress & 3U) != 0U) {
    writeStartWord(realAddress, data);
    // adjust realAddress, data pointer and byte count
    realAddress += 2;
    data = (uint16_t *)data + 1;
    length -= 2;
  }

  // If data size is 2 byte aligned instead of 4, write the last 2 bytes of data and
  // two buffer bytes of 0xFFFF at the end of the token separately from the rest of
  // data. Buffer space is built into the mfg token map to account for the extra bytes
  if ((length & 3U) != 0) {
    // adjust to new length
    length -= 2;
    writeEndWord(realAddress, data, length);
  }
  #endif

  if (length > 0) {
    FLASHWRITE(realAddress, data, length);
  }

  return SL_STATUS_OK;
}
