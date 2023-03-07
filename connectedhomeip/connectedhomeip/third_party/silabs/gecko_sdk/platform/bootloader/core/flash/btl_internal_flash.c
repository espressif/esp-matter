/***************************************************************************//**
 * @file
 * @brief Abstraction of internal flash read and write routines.
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
#include "core/flash/btl_internal_flash.h"

#include "core/btl_util.h"

MISRAC_DISABLE
#include "em_cmu.h"
#include "em_msc.h"
MISRAC_ENABLE

#if !defined(_SILICON_LABS_32B_SERIES_2)
static MSC_Status_TypeDef writeHalfword(uint32_t address,
                                        uint16_t data);

static MSC_Status_TypeDef writeHalfwordDMA(uint32_t address,
                                           uint16_t data,
                                           int      ch);

static MSC_Status_TypeDef writeHalfword(uint32_t address,
                                        uint16_t data)
{
  uint32_t address32, data32;
  MSC_Status_TypeDef retval;

  address32 = address & ~3UL;
  if (address & 2UL) {
    data32 = 0x0000FFFFUL | ((uint32_t)data << 16UL);
  } else {
    data32 = 0xFFFF0000UL | (uint32_t)data;
  }

  retval = MSC_WriteWord((uint32_t *)address32, &data32, 4U);

  return retval;
}

static MSC_Status_TypeDef writeHalfwordDMA(uint32_t address,
                                           uint16_t data,
                                           int      ch)
{
  uint32_t address32, data32;
  MSC_Status_TypeDef retval;

  address32 = address & ~3UL;
  if (address & 2UL) {
    data32 = 0x0000FFFFUL | ((uint32_t)data << 16UL);
  } else {
    data32 = 0xFFFF0000UL | (uint32_t)data;
  }

  // ch is verified by flash_writeBuffer_dma, which calls this function.
  retval = MSC_WriteWordDma(ch, (uint32_t *)address32, &data32, 4U);

  return retval;
}
#endif // !defined(_SILICON_LABS_32B_SERIES_2)

bool flash_erasePage(uint32_t address)
{
#if defined(_CMU_CLKEN1_MASK)
  CMU->CLKEN1_SET = CMU_CLKEN1_MSC;
#endif
  MSC_Status_TypeDef retval = MSC_ErasePage((uint32_t *)address);
  if (retval == mscReturnOk) {
    return true;
  } else {
    return false;
  }
}

bool flash_writeBuffer_dma(uint32_t       address,
                           void           *data,
                           size_t         length,
                           int            ch)
{
  MSC_Status_TypeDef retval = mscReturnOk;

  if ((ch < 0) || (ch >= (int)DMA_CHAN_COUNT)) {
    return false;
  }
  MISRAC_DISABLE
  CMU_ClockEnable(cmuClock_LDMA, true);
#if defined(CMU_CLKEN0_LDMAXBAR)
  CMU_ClockEnable(cmuClock_LDMAXBAR, true);
#endif
  MISRAC_ENABLE

  if (length == 0UL) {
    // Attempt to write zero-length array, return immediately
    return true;
  }

#if defined(_SILICON_LABS_32B_SERIES_2)
  if ((address & 3UL) || (length & 3UL)) {
    // Unaligned write, return early
    return false;
  }

#if defined(_CMU_CLKEN1_MASK)
  CMU->CLKEN1_SET = CMU_CLKEN1_MSC;
#endif

  retval = MSC_WriteWordDma(ch, (uint32_t *)address, data, length);
#else
  uint16_t * data16 = (uint16_t *)data;

  if ((address & 1UL) || (length & 1UL)) {
    // Unaligned write, return early
    return false;
  }

  // Flash unaligned data at start
  if (address & 2UL) {
    if ((writeHalfwordDMA(address, *data16, ch)) != mscReturnOk) {
      return false;
    }
    address += 2UL;
    length -= 2UL;
    data16++;
  }

  // Flash word-aligned data
  if (length >= 4UL) {
    uint32_t length16 = (length & ~3UL);

    retval = MSC_WriteWordDma(ch, (uint32_t *)address, data16, length16);

    data16 += length16 / sizeof(uint16_t);
    address += length16;
    length -= length16;
  }

  if (retval != mscReturnOk) {
    return false;
  }

  // Flash unaligned data at end
  if (length > 0UL) {
    retval = writeHalfwordDMA(address, *data16, ch);
    address += 2UL;
    length -= 2UL;
  }
#endif // #if defined(_SILICON_LABS_32B_SERIES_2)

  if (retval == mscReturnOk) {
    return true;
  } else {
    return false;
  }
}

bool flash_writeBuffer(uint32_t       address,
                       void           *data,
                       size_t         length)
{
  MSC_Status_TypeDef retval = mscReturnOk;

  if (length == 0UL) {
    // Attempt to write zero-length array, return immediately
    return true;
  }

#if defined(_SILICON_LABS_32B_SERIES_2)
  if ((address & 3UL) || (length & 3UL)) {
    // Unaligned write, return early
    return false;
  }

#if defined(_CMU_CLKEN1_MASK)
  CMU->CLKEN1_SET = CMU_CLKEN1_MSC;
#endif

  retval = MSC_WriteWord((uint32_t *)address, data, length);
#else
  uint16_t * data16 = (uint16_t *)data;

  if ((address & 1UL) || (length & 1UL)) {
    // Unaligned write, return early
    return false;
  }

  // Flash unaligned data at start
  if (address & 2UL) {
    if ((writeHalfword(address, *data16)) != mscReturnOk) {
      return false;
    }
    address += 2UL;
    length -= 2UL;
    data16++;
  }

  // Flash word-aligned data
  if (length >= 4UL) {
    uint32_t length16 = (length & ~3UL);
    retval = MSC_WriteWord((uint32_t *)address, data16, length16);
    data16 += length16 / sizeof(uint16_t);
    address += length16;
    length -= length16;
  }

  if (retval != mscReturnOk) {
    return false;
  }

  // Flash unaligned data at end
  if (length > 0UL) {
    retval = writeHalfword(address, *data16);
    address += 2UL;
    length -= 2UL;
  }
#endif // #if defined(_SILICON_LABS_32B_SERIES_2)

  if (retval == mscReturnOk) {
    return true;
  } else {
    return false;
  }
}
