/*******************************************************************************
* @file  rsi_utils_rom.c
* @brief 
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

/*
  Include files
 */
#include "rsi_driver.h"
#ifndef ROM_WIRELESS
/*
  Global defines
 */

/** @addtogroup DRIVER12
* @{
*/
/*=============================================================================*/
/**
 * @fn              void ROM_WL_rsi_uint16_to_2bytes(global_cb_t *global_cb_p, uint8_t *dBuf, uint16_t val)
 * @brief           Convert uint16 to two byte array
 * @param[in]       global_cb_p - pointer to the common buffer
 * @param[in]       dBuf 		- pointer to buffer to put the data in
 * @param[in]       val 		- data to convert
 * @return          void
 */
/// @private
void ROM_WL_rsi_uint16_to_2bytes(global_cb_t *global_cb_p, uint8_t *dBuf, uint16_t val)
{
  if (global_cb_p->endian == IS_LITTLE_ENDIAN) {
    dBuf[0] = val & 0x00ff;
    dBuf[1] = (val >> 8) & 0x00ff;
  } else {
    dBuf[1] = val & 0x00ff;
    dBuf[0] = (val >> 8) & 0x00ff;
  }
}

/*=============================================================================*/
/**
 * @fn              void ROM_WL_rsi_uint32_to_4bytes(global_cb_t *global_cb_p,  uint8_t *dBuf, uint32_t val)
 * @brief           Convert uint32 to four byte array
 * @param[in]       global_cb_p - pointer to the common buffer
 * @param[in]       dBuf        - pointer to the buffer to put the data in
 * @param[in]       val      	- data to convert
 * @return          void
 */
/// @private
void ROM_WL_rsi_uint32_to_4bytes(global_cb_t *global_cb_p, uint8_t *dBuf, uint32_t val)
{
  if (global_cb_p->endian == IS_LITTLE_ENDIAN) {
    dBuf[0] = val & 0x000000ff;
    dBuf[1] = (val >> 8) & 0x000000ff;
    dBuf[2] = (val >> 16) & 0x000000ff;
    dBuf[3] = (val >> 24) & 0x000000ff;
  } else {
    dBuf[3] = val & 0x000000ff;
    dBuf[2] = (val >> 8) & 0x000000ff;
    dBuf[1] = (val >> 16) & 0x000000ff;
    dBuf[0] = (val >> 24) & 0x000000ff;
  }
}

/*=============================================================================*/
/**
 * @fn              uint16_t ROM_WL_rsi_bytes2R_to_uint16(global_cb_t *global_cb_p, uint8_t *dBuf)
 * @brief           Convert a 2 byte array to uint16, first byte in array is LSB
 * @param[in]       global_cb_p - pointer to the common buffer
 * @param[in]       dBuf        - pointer to a buffer to get the data from
 * @return          converted data
 */
/// @private
uint16_t ROM_WL_rsi_bytes2R_to_uint16(global_cb_t *global_cb_p, uint8_t *dBuf)
{
  uint16_t val;
  if (global_cb_p->endian == IS_LITTLE_ENDIAN) {
    val = dBuf[1];
    val <<= 8;
    val |= dBuf[0] & 0x000000ff;
  } else {
    val = dBuf[0];
    val <<= 8;
    val |= dBuf[1] & 0x000000ff;
  }
  return val;
}

/*=============================================================================*/
/**
 * @fn           uint32_t ROM_WL_rsi_bytes4R_to_uint32(global_cb_t *global_cb_p,  uint8_t *dBuf)
 * @brief        Convert a 4 byte array to uint32, first byte in array is LSB
 * @param[in]    global_cb_p - pointer to the common buffer
 * @param[in]    dBuf        - pointer to buffer to get the data from
 * @return       converted data
 */
/// @private
uint32_t ROM_WL_rsi_bytes4R_to_uint32(global_cb_t *global_cb_p, uint8_t *dBuf)
{
  // the 32-bit value to return
  uint32_t val;

  if (global_cb_p->endian == IS_LITTLE_ENDIAN) {
    val = dBuf[3];
    val <<= 8;
    val |= dBuf[2] & 0x000000ff;
    val <<= 8;
    val |= dBuf[1] & 0x000000ff;
    val <<= 8;
    val |= dBuf[0] & 0x000000ff;
  } else {
    val = dBuf[0];
    val <<= 8;
    val |= dBuf[1] & 0x000000ff;
    val <<= 8;
    val |= dBuf[2] & 0x000000ff;
    val <<= 8;
    val |= dBuf[3] & 0x000000ff;
  }

  return val;
}
#endif
/** @} */
