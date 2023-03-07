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
#include "ezradio_comm.h"
#include "ezradio_hal.h"
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
      /* Number of command bytes exceeds maximal allowable length */
      return EZRADIO_CONFIG_COMMAND_ERROR;
    }

    for (col = 0u; col < numOfBytes; col++) {
      ezradioCmd[col] = *pSetPropCmd;
      pSetPropCmd++;
    }

    if (ezradio_comm_SendCmdGetResp(numOfBytes, ezradioCmd, 1, &response) != 0xFF) {
      /* Timeout occured */
      return EZRADIO_CONFIG_CTS_TIMEOUT;
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
    ezradioReply->GET_ADC_READING.TEMP_ADC         = ((uint16_t)ezradioCmd[4] << 8) & 0xFF00;
    ezradioReply->GET_ADC_READING.TEMP_ADC        |=  (uint16_t)ezradioCmd[5] & 0x00FF;
  }
}

/**
 * Performs image rejection calibration. Completion can be monitored by polling CTS or waiting for CHIP_READY interrupt source.
 *
 * @param[in] searching_step_size
 * @param[in] searching_rssi_avg
 * @param[in] rx_chain_setting1
 * @param[in] rx_chain_setting2
 */
void ezradio_ircal(uint8_t searching_step_size, uint8_t searching_rssi_avg, uint8_t rx_chain_setting1, uint8_t rx_chain_setting2)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[5u];

  ezradioCmd[0] = EZRADIO_CMD_ID_IRCAL;
  ezradioCmd[1] = searching_step_size;
  ezradioCmd[2] = searching_rssi_avg;
  ezradioCmd[3] = rx_chain_setting1;
  ezradioCmd[4] = rx_chain_setting2;

  ezradio_comm_SendCmd(EZRADIO_CMD_ARG_COUNT_IRCAL, ezradioCmd);
}

/**
 * Image rejection calibration. Forces a specific value for IR calibration, and reads back calibration values from previous calibrations
 *
 * @param[in] ircal_amp
 * @param[in] ircal_ph
 * @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_ircal_manual(uint8_t ircal_amp, uint8_t ircal_ph, ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[EZRADIO_CMD_ARG_COUNT_IRCAL_MANUAL];

  ezradioCmd[0] = EZRADIO_CMD_ID_IRCAL_MANUAL;
  ezradioCmd[1] = ircal_amp;
  ezradioCmd[2] = ircal_ph;

  ezradio_comm_SendCmdGetResp(EZRADIO_CMD_ARG_COUNT_IRCAL_MANUAL,
                              ezradioCmd,
                              EZRADIO_CMD_REPLY_COUNT_IRCAL_MANUAL,
                              ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->IRCAL_MANUAL.IRCAL_AMP_REPLY   = ezradioCmd[0];
    ezradioReply->IRCAL_MANUAL.IRCAL_PH_REPLY    = ezradioCmd[1];
  }
}

/**
 * While in TX state this will hop to the frequency specified by the parameters
 *
 * @param[in] inte      new inte register value.
 * @param[in] frac2     new frac2 register value.
 * @param[in] frac1     new frac1 register value.
 * @param[in] frac0     new frac0 register value.
 * @param[in] vco_cnt1  new vco_cnt1 register value.
 * @param[in] vco_cnt0  new vco_cnt0 register value.
 * @param[in] pll_settle_time1  new pll_settle_time1 register value.
 * @param[in] pll_settle_time0  new pll_settle_time0 register value.
 */
void ezradio_tx_hop(uint8_t inte, uint8_t frac2, uint8_t frac1, uint8_t frac0, uint8_t vco_cnt1, uint8_t vco_cnt0, uint8_t pll_settle_time1, uint8_t pll_settle_time0)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[9u];

  ezradioCmd[0] = EZRADIO_CMD_ID_TX_HOP;
  ezradioCmd[1] = inte;
  ezradioCmd[2] = frac2;
  ezradioCmd[3] = frac1;
  ezradioCmd[4] = frac0;
  ezradioCmd[5] = vco_cnt1;
  ezradioCmd[6] = vco_cnt0;
  ezradioCmd[7] = pll_settle_time1;
  ezradioCmd[8] = pll_settle_time0;

  ezradio_comm_SendCmd(EZRADIO_CMD_ARG_COUNT_TX_HOP, ezradioCmd);
}

/**
 * While in RX state this will hop to the frequency specified by the parameters and start searching for a preamble.
 *
 * @param[in] inte      new inte register value.
 * @param[in] frac2     new frac2 register value.
 * @param[in] frac1     new frac1 register value.
 * @param[in] frac0     new frac0 register value.
 * @param[in] vco_cnt1  new vco_cnt1 register value.
 * @param[in] vco_cnt0  new vco_cnt0 register value.
 */
void ezradio_rx_hop(uint8_t inte, uint8_t frac2, uint8_t frac1, uint8_t frac0, uint8_t vco_cnt1, uint8_t vco_cnt0)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[7u];

  ezradioCmd[0] = EZRADIO_CMD_ID_RX_HOP;
  ezradioCmd[1] = inte;
  ezradioCmd[2] = frac2;
  ezradioCmd[3] = frac1;
  ezradioCmd[4] = frac0;
  ezradioCmd[5] = vco_cnt1;
  ezradioCmd[6] = vco_cnt0;

  ezradio_comm_SendCmd(EZRADIO_CMD_ARG_COUNT_RX_HOP, ezradioCmd);
}

#endif /* EZRADIO_DRIVER_FULL_SUPPORT */

#endif /* EZRADIO_DRIVER_EXTENDED_SUPPORT */
