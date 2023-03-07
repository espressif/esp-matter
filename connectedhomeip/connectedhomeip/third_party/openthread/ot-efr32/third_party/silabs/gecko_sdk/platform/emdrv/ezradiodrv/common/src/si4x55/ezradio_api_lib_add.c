/***************************************************************************//**
 * @file
 * @brief This file contains the additional API library for the listed members of
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include "em_gpio.h"
#include "gpiointerrupt.h"

#include "ezradio_cmd.h"
#include "ezradio_prop.h"
#include "ezradio_hal.h"
#include "ezradio_comm.h"
#include "ezradio_api_lib.h"
#include "ezradio_api_lib_add.h"

/**
 * This function is used to load all properties and commands with a list of NULL terminated commands.
 * Before this function ezradio_reset should be called.
 *
 * @param[in] pSetPropCmd Pointer to the configuration array.
 */
EZRADIO_ConfigRet_t ezradio_configuration_init(const uint8_t* pSetPropCmd)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[16u];
  ezradio_cmd_reply_t ezradioReply;

  uint8_t col;
  uint8_t response;
  uint8_t numOfBytes;

  /* While cycle as far as the pointer points to a command */
  while (*pSetPropCmd != 0x00) {
    /* Commands structure in the array:
     * --------------------------------
     * LEN | <LEN length of data>
     */

    numOfBytes = *pSetPropCmd++;

    if (numOfBytes > 16u) {
      /* Initial configuration of Si4x55 */
      if ( EZRADIO_CMD_ID_EZCONFIG_ARRAY_WRITE == *pSetPropCmd ) {
        if (numOfBytes > 128u) {
          /* Number of command bytes exceeds maximal allowable length */
          return EZRADIO_CONFIG_COMMAND_ERROR;
        }

        /* Load array to the device */
        pSetPropCmd++;
        ezradio_write_ezconfig_array(numOfBytes - 1, (uint8_t*) pSetPropCmd);

        /* Point to the next command */
        pSetPropCmd += numOfBytes - 1;

        /* Continue command interpreter */
        continue;
      } else {
        /* Number of command bytes exceeds maximal allowable length */
        return EZRADIO_CONFIG_COMMAND_ERROR;
      }
    }

    for (col = 0u; col < numOfBytes; col++) {
      ezradioCmd[col] = *pSetPropCmd;
      pSetPropCmd++;
    }

    if (ezradio_comm_SendCmdGetResp(numOfBytes, ezradioCmd, 1, &response) != 0xFF) {
      /* Timeout occured */
      return EZRADIO_CONFIG_CTS_TIMEOUT;
    }

    /* Check response byte of EZCONFIG_CHECK command */
    if ( EZRADIO_CMD_ID_EZCONFIG_CHECK == ezradioCmd[0] ) {
      if (EZRADIO_CMD_EZCONFIG_CHECK_REP_RESULT_RESULT_ENUM_BAD_CHECKSUM == ezradioReply.EZCONFIG_CHECK.RESULT) {
        return EZRADIO_CONFIG_COMMAND_ERROR;
      } else if (EZRADIO_CMD_EZCONFIG_CHECK_REP_RESULT_RESULT_ENUM_INVALID_STATE == ezradioReply.EZCONFIG_CHECK.RESULT) {
        return EZRADIO_CONFIG_COMMAND_ERROR;
      }
    }

    if (ezradio_hal_NirqLevel() == 0) {
      /* Get and clear all interrupts.  An error has occured... */
      ezradio_get_int_status(0, 0, 0, &ezradioReply);
      if (ezradioReply.GET_INT_STATUS.CHIP_PEND & EZRADIO_CMD_GET_CHIP_STATUS_REP_CHIP_PEND_CMD_ERROR_PEND_MASK) {
        return EZRADIO_CONFIG_COMMAND_ERROR;
      }
    }
  }

  return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Writes data byte(s) to the EZConfig array.
 *
 * @param[in] numBytes  number of bytes to send
 * @param[in] pEzConfigArray pointer to the EZConfig configuration array
 */
void ezradio_write_ezconfig_array(uint8_t numBytes, uint8_t* pEzConfigArray)
{
  ezradio_comm_WriteData(EZRADIO_CMD_ID_EZCONFIG_ARRAY_WRITE, 1, numBytes, pEzConfigArray);
}

/**
 * Validates the EZConfig array was written correctly.
 *
 * @param[in] checksum  Checksum value
 * @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_ezconfig_check(uint16_t checksum, ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[3u];

  ezradioCmd[0] = EZRADIO_CMD_ID_EZCONFIG_CHECK;
  ezradioCmd[1] = (uint8_t) (checksum >> 8u);
  ezradioCmd[2] = (uint8_t) (checksum & 0x00FF);

  /* Do not check CTS after sending the ezconfig array */
  ezradio_comm_CtsWentHigh = true;

  ezradio_comm_SendCmdGetResp(EZRADIO_CMD_ARG_COUNT_EZCONFIG_CHECK,
                              ezradioCmd,
                              EZRADIO_CMD_REPLY_COUNT_EZCONFIG_CHECK,
                              ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->EZCONFIG_CHECK.RESULT = ezradioCmd[0];
  }
}

#ifdef EZRADIO_DRIVER_EXTENDED_SUPPORT
/* Extended driver support functions */

#ifdef EZRADIO_DRIVER_FULL_SUPPORT
/* Full driver support functions */

/**
 * Reads the ADC values from the radio into ezradioReply union.
 *
 * @param[in] adc_en  ADC enable parameter.
 * @param[in] adc_cfg  ADC configuration parameter.
 * @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_get_adc_reading(uint8_t adc_en, uint8_t adc_cfg, ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_GET_ADC_READING];

  ezradioCmd[0] = EZRADIO_CMD_ID_GET_ADC_READING;
  ezradioCmd[1] = adc_en;
  ezradioCmd[2] = adc_cfg;

  ezradio_comm_SendCmdGetResp(EZRADIO_CMD_ARG_COUNT_GET_ADC_READING,
                              ezradioCmd,
                              EZRADIO_CMD_REPLY_COUNT_GET_ADC_READING,
                              ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->GET_ADC_READING.GPIO_ADC         = ((uint16_t)ezradioCmd[0] << 8) & 0xFF00;
    ezradioReply->GET_ADC_READING.GPIO_ADC        |=  (uint16_t)ezradioCmd[1] & 0x00FF;
    ezradioReply->GET_ADC_READING.BATTERY_ADC      = ((uint16_t)ezradioCmd[2] << 8) & 0xFF00;
    ezradioReply->GET_ADC_READING.BATTERY_ADC     |=  (uint16_t)ezradioCmd[3] & 0x00FF;
  }
}

#endif /* EZRADIO_DRIVER_FULL_SUPPORT */

#endif /* EZRADIO_DRIVER_EXTENDED_SUPPORT */
