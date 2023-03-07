/*******************************************************************************
* @file  sl_hal_mcu_logging.c
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


/**
 * Includes
 */
#include "rsi_driver.h"


/*==================================================================*/
/**
 * @fn         int32_t sl_hal_send_log(uint8_t *buffer, uint32_t buffer_length)
 * @param[in]  uint8_t *buffer, pointer to buffer
 * @param[in]  uint32_t buffer_length, number of bytes to send
 * @param[out] None
 * @return     0, 0=success
 * @section description
 * This API is used to send data to logging interface 
 */
int32_t sl_hal_send_log(uint8_t *buffer, uint32_t buffer_length)
{
  // insert the token for identification
  LOG_PRINT("\r\n#@$ ");
  for (int txCount = 0; txCount < (int)buffer_length; txCount++){
    LOG_PRINT("%02x ", *(buffer++));
  }
  LOG_PRINT("\r\n");
  return 0;
}
