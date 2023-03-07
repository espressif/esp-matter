/***************************************************************************/ /**
 * @file
 * @brief CPC CRC implementation.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "sli_cpc_crc.h"
#include "em_cmu.h"
#include "em_gpcrc.h"

#ifdef CPC_TEST_WITH_INVALID_CRC
#include "stdio.h"
#endif

/*******************************************************************************
 *********************************   DEFINES   *********************************
 ******************************************************************************/

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

static uint16_t sli_cpc_compute_crc16(uint8_t new_byte, uint16_t prev_result);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

#if 0
/***************************************************************************/ /**
 * Initiate a CRC calculation on a given buffer.
 ******************************************************************************/
sl_status_t sli_cpc_compute_crc_init(void)
{
  Ecode_t status;
  GPCRC_Init_TypeDef init = GPCRC_INIT_DEFAULT;

  CMU_ClockEnable(cmuClock_GPCRC, true);

  init.crcPoly = 0x1021;
  init.initValue = 0xffff;
  init.reverseBits = true;
  init.reverseByteOrder = false;
  GPCRC_Init(GPCRC, &init);

  status = DMADRV_AllocateChannel(&crc_dma_channel, NULL);
  if (status != ECODE_EMDRV_DMADRV_OK) {
    return SL_STATUS_ALLOCATION_FAILED;
  }

  GPCRC_Enable(GPCRC, true);

  return SL_STATUS_OK;
}

sl_status_t sli_cpc_compute_crc_start(const void* buffer, uint16_t buffer_length)
{
  Ecode_t code = DMADRV_MemoryPeripheral(crc_dma_channel,
                                         dmadrvPeripheralSignal_NONE,
                                         &(GPCRC->INPUTDATA),
                                         buffer,
                                         false,
                                         buffer_length - 2,
                                         4,
                                         0,
                                         0);
  if (code != ECODE_EMDRV_DMADRV_OK) {
    return SL_STATUS_FAIL;
  }
}
#endif

/***************************************************************************//**
 * Computes CRC-16 CCITT (XMODEM) on given buffer. Software implementation.
 ******************************************************************************/
uint16_t sli_cpc_get_crc_sw(const void* buffer, uint16_t buffer_length)
{
  uint16_t i;
  uint16_t crc = 0;

  for (i = 0; i < buffer_length; i++) {
    crc = sli_cpc_compute_crc16((uint8_t)((uint8_t *)buffer)[i], crc);
  }

#ifdef CPC_TEST_WITH_INVALID_CRC
  static uint8_t count;

  count++;
  if ( count % CPC_TEST_WITH_INVALID_CRC_FREQUENCY == 0) {
    count = 0;
    printf("Invalidated the CRC\n");
    return 0;
  }
#endif

  return crc;
}

uint16_t sli_cpc_get_crc_sw_with_security(const void *buffer, uint16_t buffer_length,
                                          const void *security_tag, uint16_t security_tag_length)
{
  uint16_t i;
  uint16_t crc = 0;

  for (i = 0; i < buffer_length; i++) {
    crc = sli_cpc_compute_crc16((uint8_t)((uint8_t *)buffer)[i], crc);
  }

  for (i = 0; i < security_tag_length; i++) {
    crc = sli_cpc_compute_crc16((uint8_t)((uint8_t *)security_tag)[i], crc);
  }

  return crc;
}

/***************************************************************************//**
 * Validates CRC-16 CCITT (XMODEM) on given buffer. Software implementation.
 ******************************************************************************/
bool sli_cpc_validate_crc_sw(const void* buffer, uint16_t buffer_length, uint16_t expected_crc)
{
  uint16_t computed_crc;

  computed_crc = sli_cpc_get_crc_sw(buffer, buffer_length);

  return (computed_crc == expected_crc);
}

static uint16_t sli_cpc_compute_crc16(uint8_t new_byte, uint16_t prev_result)
{
  prev_result = ((uint16_t) (prev_result >> 8)) | ((uint16_t) (prev_result << 8));
  prev_result ^= new_byte;
  prev_result ^= (prev_result & 0xff) >> 4;
  prev_result ^= (uint16_t) (((uint16_t) (prev_result << 8)) << 4);
  prev_result ^= ((uint8_t) (((uint8_t) (prev_result & 0xff)) << 5))
                 | ((uint16_t) ((uint16_t) ((uint8_t) (((uint8_t) (prev_result & 0xff)) >> 3)) << 8));

  return prev_result;
}
