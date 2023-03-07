/***************************************************************************//**
 * @file
 * @brief File System - Crc Utility Library
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_FS_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <fs/source/shared/crc/crc_utils.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Pop cnt algorithms.
#define CRC_UTIL_POPCNT_METHOD_FAST_MULT        0
#define CRC_UTIL_POPCNT_METHOD_SLOW_MULT        1

#define CRC_UTIL_POPCNT_METHOD                  CRC_UTIL_POPCNT_METHOD_FAST_MULT

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           CRCUtil_PopCnt_32()
 *
 * @brief    Compute population count (hamming weight) for value (number of bits set).
 *
 * @param    value   Value to compute population count on.
 *
 * @return   value's population count.
 *
 * @note     (1) Algorithm taken from http://en.wikipedia.org/wiki/Hamming_weight
 *******************************************************************************************************/
CPU_INT08U CRCUtil_PopCnt_32(CPU_INT32U value)
{
  CPU_INT32U even_cnt;
  CPU_INT32U odd_cnt;
  CPU_INT32U result;

  odd_cnt = (value >> 1u) & CRC_UTIL_POPCNT_MASK01010101_32;    // 2-bits pieces.
  result = value - odd_cnt;                                     // Same result as result=odd_cnt+(value & 0x55555555).

  even_cnt = result & CRC_UTIL_POPCNT_MASK00110011_32;          // 4-bits pieces.
  odd_cnt = (result >> 2u) & CRC_UTIL_POPCNT_MASK00110011_32;
  result = even_cnt + odd_cnt;

  even_cnt = result & CRC_UTIL_POPCNT_MASK00001111_32;          // 8-bits pieces.
  odd_cnt = (result >> 4u) & CRC_UTIL_POPCNT_MASK00001111_32;
  result = even_cnt + odd_cnt;

#if CRC_UTIL_POPCNT_METHOD == CRC_UTIL_POPCNT_METHOD_SLOW_MULT
  result += result >> 8u;                                       // 16-bits pieces into their lowest 8 bits
  result += result >> 16u;                                      // Add together both counts.

  result &= 0x3Fu
            return (result);                                    // Mask unwanted bits.
#else
  result = (result * CRC_UTIL_POPCNT_POWERSOF256_32) >> 24u;
  return (result);
#endif
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
