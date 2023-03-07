/***************************************************************************//**
 * @file
 * @brief CRC32 functionality for Silicon Labs bootloader
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
#include "btl_crc32.h"
#include "em_device.h"

uint32_t btl_crc32Stream(const uint8_t *buffer,
                         size_t        length,
                         uint32_t      prevResult)
{
#if defined(_CMU_CLKEN0_MASK)
  CMU->CLKEN0_SET = CMU_CLKEN0_GPCRC;
#endif
#if defined(_SILICON_LABS_32B_SERIES_2)
  GPCRC->EN = GPCRC_EN_EN;
  GPCRC->CTRL = GPCRC_CTRL_POLYSEL_CRC32;
  GPCRC->INIT = prevResult;
  GPCRC->CMD = GPCRC_CMD_INIT;

  while (length--) {
    GPCRC->INPUTDATABYTE = *buffer++;
  }

  return GPCRC->DATA;
#else
  CMU->HFBUSCLKEN0 |= CMU_HFBUSCLKEN0_GPCRC;

  GPCRC->CTRL = GPCRC_CTRL_POLYSEL_CRC32 | GPCRC_CTRL_EN_ENABLE;
  GPCRC->INIT = prevResult;
  GPCRC->CMD = GPCRC_CMD_INIT;

  while (length--) {
    GPCRC->INPUTDATABYTE = *buffer++;
  }

  return GPCRC->DATA;
#endif
}
