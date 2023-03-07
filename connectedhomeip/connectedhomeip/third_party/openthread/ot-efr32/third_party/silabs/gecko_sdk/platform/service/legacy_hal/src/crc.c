/***************************************************************************//**
 * @file
 * @brief  Generic firmware source for Cyclic Redundancy Check calculations.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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
#include <stdint.h>
#include "crc.h"

/*
 *    16bit CRC notes:
 *    "CRC-CCITT"
 *     poly is g(X) = X^16 + X^12 + X^5 + 1  (0x1021)
 *     used in the FPGA (green boards and 15.4)
 *     initial remainder should be 0xFFFF
 */

//[[
// This CRC seems to take about the same amount of time as the table driven CRC
// which was timed at 34 cycles on the mega128 (8.5us @4MHz) and it utilizes
// much less flash.
//]]
uint16_t halCommonCrc16(uint8_t newByte, uint16_t prevResult)
{
  prevResult = ((uint16_t) (prevResult >> 8)) | ((uint16_t) (prevResult << 8));
  prevResult ^= newByte;
  prevResult ^= (prevResult & 0xff) >> 4;
  prevResult ^= (uint16_t) (((uint16_t) (prevResult << 8)) << 4);

  //[[ What I wanted is the following function:
  // prevResult ^= ((prevResult & 0xff) << 4) << 1;
  // Unfortunately the compiler does this in 46 cycles.  The next line of code
  // does the same thing, but the compiler uses only 10 cycles to implement it.
  //]]
  prevResult ^= ((uint8_t) (((uint8_t) (prevResult & 0xff)) << 5))
                | ((uint16_t) ((uint16_t) ((uint8_t) (((uint8_t) (prevResult & 0xff)) >> 3)) << 8));

  return prevResult;
}

//--------------------------------------------------------------
// CRC-32
#define POLYNOMIAL              (0xEDB88320UL)

uint32_t halCommonCrc32(uint8_t newByte, uint32_t prevResult)
{
  uint8_t jj;
  uint32_t previous;
  uint32_t oper;

  previous = (prevResult >> 8) & 0x00FFFFFFL;
  oper = (prevResult ^ newByte) & 0xFF;
  for (jj = 0; jj < 8; jj++) {
    oper = ((oper & 0x01)
            ? ((oper >> 1) ^ POLYNOMIAL)
            : (oper >> 1));
  }

  return (previous ^ oper);
}
