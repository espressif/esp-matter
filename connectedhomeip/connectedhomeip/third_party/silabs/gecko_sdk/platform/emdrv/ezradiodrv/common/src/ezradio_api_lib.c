/***************************************************************************//**
 * @file
 * @brief This file contains the common API library of the EZRadio and
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
#include "ustimer.h"

#include "ezradio_cmd.h"
#include "ezradio_prop.h"
#include "ezradio_comm.h"
#include "ezradio_hal.h"
#include "ezradio_api_lib.h"

/**
 * This functions is used to reset the EZRadio device by applying shutdown and
 * releasing it.  After this function @ref ezradio_power_up or @ref ezradio_configuration_init
 * should be called. The user can check if POR has completed by waiting 4 ms or by polling
 * radio GPIO 0, 2, or 3. When these GPIOs are high, it is safe to call the
 * above mentioned functions.
 */
void ezradio_reset(void)
{
  /* Initialize ustimer */
  USTIMER_Init();
  /* Put radio in shutdown */
  ezradio_hal_AssertShutdown();
  /* Delay for 20us */
  USTIMER_Delay(10u);
  /* Release radio from shutdown */
  ezradio_hal_DeassertShutdown();
  /* Delay for 100us */
  USTIMER_Delay(100u);
  /* Clear CTS signal. */
  ezradio_comm_ClearCTS();
  /* Deinit ustimer */
  USTIMER_DeInit();
}

/**
 * This function is used to initialize after power-up the radio chip.
 * Before this function ezradio_reset should be called.
 *
 * @param[in] boot_options   Boot options input to the power up command.
 * @param[in] xtal_options   XTAL options input to the power up command.
 * @param[in] xo_freq   XO frequency input to the power up command.
 */
void ezradio_power_up(uint8_t boot_options, uint8_t xtal_options, uint32_t xo_freq)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[7u];

  ezradioCmd[0] = EZRADIO_CMD_ID_POWER_UP;
  ezradioCmd[1] = boot_options;
  ezradioCmd[2] = xtal_options;
  ezradioCmd[3] = (uint8_t)(xo_freq >> 24);
  ezradioCmd[4] = (uint8_t)(xo_freq >> 16);
  ezradioCmd[5] = (uint8_t)(xo_freq >> 8);
  ezradioCmd[6] = (uint8_t)(xo_freq);

  ezradio_comm_SendCmd(EZRADIO_CMD_ARG_COUNT_POWER_UP, ezradioCmd);
}

/** This function sends the PART_INFO command to the radio and receives the answer
 *  into ezradioReply union.
 *
 *  @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_part_info(ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_PART_INFO];

  ezradioCmd[0] = EZRADIO_CMD_ID_PART_INFO;

  ezradio_comm_SendCmdGetResp(EZRADIO_CMD_ARG_COUNT_PART_INFO,
                              ezradioCmd,
                              EZRADIO_CMD_REPLY_COUNT_PART_INFO,
                              ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->PART_INFO.CHIPREV         = ezradioCmd[0];
    ezradioReply->PART_INFO.PART            = (((uint16_t)ezradioCmd[1] << 8) & 0xFF00)
                                              + ((uint16_t)ezradioCmd[2] & 0x00FF);
    ezradioReply->PART_INFO.PBUILD          = ezradioCmd[3];
    ezradioReply->PART_INFO.ID              = (((uint16_t)ezradioCmd[4] << 8) & 0xFF00)
                                              + ((uint16_t)ezradioCmd[5] & 0x00FF);
    ezradioReply->PART_INFO.CUSTOMER        = ezradioCmd[6];
    ezradioReply->PART_INFO.ROMID           = ezradioCmd[7];
  }
}

/** Sends START_TX command to the radio.
 *
 * @param[in] channel   Channel number.
 * @param[in] condition Start TX condition.
 * @param[in] tx_len    Payload length (exclude the PH generated CRC).
 */
void ezradio_start_tx(uint8_t channel, uint8_t condition, uint16_t tx_len)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[7u];

  ezradioCmd[0] = EZRADIO_CMD_ID_START_TX;
  ezradioCmd[1] = channel;
  ezradioCmd[2] = condition;
  ezradioCmd[3] = (uint8_t)(tx_len >> 8);
  ezradioCmd[4] = (uint8_t)(tx_len);
  ezradioCmd[5] = 0x00;

  // Don't repeat the packet,
  // ie. transmit the packet only once
  ezradioCmd[6] = 0x00;

  ezradio_comm_SendCmd(EZRADIO_CMD_ARG_COUNT_START_TX, ezradioCmd);
}

/**
 * Sends START_RX command to the radio.
 *
 * @param[in] channel     Channel number.
 * @param[in] condition   Start RX condition.
 * @param[in] rx_len      Payload length (exclude the PH generated CRC).
 * @param[in] next_state1 Next state when Preamble Timeout occurs.
 * @param[in] next_state2 Next state when a valid packet received.
 * @param[in] next_state3 Next state when invalid packet received (e.g. CRC error).
 */
void ezradio_start_rx(uint8_t channel, uint8_t condition, uint16_t rx_len, uint8_t next_state1, uint8_t next_state2, uint8_t next_state3)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[8u];

  ezradioCmd[0] = EZRADIO_CMD_ID_START_RX;
  ezradioCmd[1] = channel;
  ezradioCmd[2] = condition;
  ezradioCmd[3] = (uint8_t)(rx_len >> 8);
  ezradioCmd[4] = (uint8_t)(rx_len);
  ezradioCmd[5] = next_state1;
  ezradioCmd[6] = next_state2;
  ezradioCmd[7] = next_state3;

  ezradio_comm_SendCmd(EZRADIO_CMD_ARG_COUNT_START_RX, ezradioCmd);
}

/**
 * Get the Interrupt status/pending flags form the radio and clear flags if requested.
 *
 * @param[in] ph_clr_pend     Packet Handler pending flags clear.
 * @param[in] modem_clr_pend  Modem Status pending flags clear.
 * @param[in] chip_clr_pend   Chip State pending flags clear.
 * @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_get_int_status(uint8_t ph_clr_pend, uint8_t modem_clr_pend, uint8_t chip_clr_pend,
                            ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_GET_INT_STATUS];

  ezradioCmd[0] = EZRADIO_CMD_ID_GET_INT_STATUS;
  ezradioCmd[1] = ph_clr_pend;
  ezradioCmd[2] = modem_clr_pend;
  ezradioCmd[3] = chip_clr_pend;

  ezradio_comm_SendCmdGetResp(EZRADIO_CMD_ARG_COUNT_GET_INT_STATUS,
                              ezradioCmd,
                              EZRADIO_CMD_REPLY_COUNT_GET_INT_STATUS,
                              ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->GET_INT_STATUS.INT_PEND       = ezradioCmd[0];
    ezradioReply->GET_INT_STATUS.INT_STATUS     = ezradioCmd[1];
    ezradioReply->GET_INT_STATUS.PH_PEND        = ezradioCmd[2];
    ezradioReply->GET_INT_STATUS.PH_STATUS      = ezradioCmd[3];
    ezradioReply->GET_INT_STATUS.MODEM_PEND     = ezradioCmd[4];
    ezradioReply->GET_INT_STATUS.MODEM_STATUS   = ezradioCmd[5];
    ezradioReply->GET_INT_STATUS.CHIP_PEND      = ezradioCmd[6];
    ezradioReply->GET_INT_STATUS.CHIP_STATUS    = ezradioCmd[7];
  }
}

/**
 * Send GPIO pin config command to the radio and reads the answer into
 * ezradioReply union.
 *
 * @param[in] gpio0       GPIO0 configuration.
 * @param[in] gpio1       GPIO1 configuration.
 * @param[in] gpio2       GPIO2 configuration.
 * @param[in] gpio3       GPIO3 configuration.
 * @param[in] nirq        NIRQ configuration.
 * @param[in] sdo         SDO configuration.
 * @param[in] gen_config  General pin configuration.
 * @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_gpio_pin_cfg(uint8_t gpio0, uint8_t gpio1, uint8_t gpio2, uint8_t gpio3, uint8_t nirq, uint8_t sdo, uint8_t gen_config,
                          ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[EZRADIO_CMD_ARG_COUNT_GPIO_PIN_CFG];

  ezradioCmd[0] = EZRADIO_CMD_ID_GPIO_PIN_CFG;
  ezradioCmd[1] = gpio0;
  ezradioCmd[2] = gpio1;
  ezradioCmd[3] = gpio2;
  ezradioCmd[4] = gpio3;
  ezradioCmd[5] = nirq;
  ezradioCmd[6] = sdo;
  ezradioCmd[7] = gen_config;

  ezradio_comm_SendCmdGetResp(EZRADIO_CMD_ARG_COUNT_GPIO_PIN_CFG,
                              ezradioCmd,
                              EZRADIO_CMD_REPLY_COUNT_GPIO_PIN_CFG,
                              ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->GPIO_PIN_CFG.gpIO[0]      = ezradioCmd[0];
    ezradioReply->GPIO_PIN_CFG.gpIO[1]      = ezradioCmd[1];
    ezradioReply->GPIO_PIN_CFG.gpIO[2]      = ezradioCmd[2];
    ezradioReply->GPIO_PIN_CFG.gpIO[3]      = ezradioCmd[3];
    ezradioReply->GPIO_PIN_CFG.NIRQ         = ezradioCmd[4];
    ezradioReply->GPIO_PIN_CFG.SDO          = ezradioCmd[5];
    ezradioReply->GPIO_PIN_CFG.GEN_CONFIG   = ezradioCmd[6];
  }
}

/**
 * Send SET_PROPERTY command to the radio.
 *
 * @param[in] group       Property group.
 * @param[in] num_props   Number of property to be set. The properties must be in ascending order
 *                    in their sub-property aspect. Max. 12 properties can be set in one command.
 * @param[in] start_prop  Start sub-property address.
 * @param[in] ...  Variable number of properties to be set.
 */
#ifdef __C51__
#pragma maxargs (13)  /* allow 13 bytes for parameters */
#endif
void ezradio_set_property(uint8_t group, uint8_t num_props, uint8_t start_prop, ...)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[16u];

  va_list argList;
  uint8_t cmdIndex;

  ezradioCmd[0] = EZRADIO_CMD_ID_SET_PROPERTY;
  ezradioCmd[1] = group;
  ezradioCmd[2] = num_props;
  ezradioCmd[3] = start_prop;

  va_start(argList, start_prop);
  cmdIndex = 4;
  while (num_props--) {
    ezradioCmd[cmdIndex] = (uint8_t)va_arg(argList, int);
    cmdIndex++;
  }
  va_end(argList);

  ezradio_comm_SendCmd(cmdIndex, ezradioCmd);
}

/**
 * Issue a change state command to the radio.
 *
 * @param[in] next_state1 Next state.
 */
void ezradio_change_state(uint8_t next_state1)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[2u];

  ezradioCmd[0] = EZRADIO_CMD_ID_CHANGE_STATE;
  ezradioCmd[1] = next_state1;

  ezradio_comm_SendCmd(EZRADIO_CMD_ARG_COUNT_CHANGE_STATE, ezradioCmd);
}

#ifdef EZRADIO_DRIVER_EXTENDED_SUPPORT
/* Extended driver support functions */

/**
 * Sends NOP command to the radio. Can be used to maintain SPI communication.
 */
void ezradio_nop(void)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[1u];

  ezradioCmd[0] = EZRADIO_CMD_ID_NOP;

  ezradio_comm_SendCmd(EZRADIO_CMD_ARG_COUNT_NOP, ezradioCmd);
}

/**
 * Send the FIFO_INFO command to the radio. Optionally resets the TX/RX FIFO. Reads the radio response back
 * into ezradioReply.
 *
 * @param[in] fifo  RX/TX FIFO reset flags.
 * @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_fifo_info(uint8_t fifo,
                       ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_FIFO_INFO];

  ezradioCmd[0] = EZRADIO_CMD_ID_FIFO_INFO;
  ezradioCmd[1] = fifo;

  ezradio_comm_SendCmdGetResp(EZRADIO_CMD_ARG_COUNT_FIFO_INFO,
                              ezradioCmd,
                              EZRADIO_CMD_REPLY_COUNT_FIFO_INFO,
                              ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->FIFO_INFO.RX_FIFO_COUNT   = ezradioCmd[0];
    ezradioReply->FIFO_INFO.TX_FIFO_SPACE   = ezradioCmd[1];
  }
}

/**
 * The function can be used to load data into TX FIFO.
 *
 * @param[in] numBytes  Data length to be load.
 * @param[in] pTxData   Pointer to the data (uint8_t*).
 */
void ezradio_write_tx_fifo(uint8_t numBytes, uint8_t* pTxData)
{
  ezradio_comm_WriteData(EZRADIO_CMD_ID_WRITE_TX_FIFO, 0, numBytes, pTxData);
}

/**
 * Reads the RX FIFO content from the radio.
 *
 * @param[in] numBytes  Data length to be read.
 * @param[in] pRxData   Pointer to the buffer location.
 */
void ezradio_read_rx_fifo(uint8_t numBytes, uint8_t* pRxData)
{
  ezradio_comm_ReadData(EZRADIO_CMD_ID_READ_RX_FIFO, 0, numBytes, pRxData);
}

/**
 * Get property values from the radio. Reads them into ezradioReply union.
 *
 * @param[in] group       Property group number.
 * @param[in] num_props   Number of properties to be read.
 * @param[in] start_prop  Starting sub-property number.
 * @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_get_property(uint8_t group, uint8_t num_props, uint8_t start_prop,
                          ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[16u];

  ezradioCmd[0] = EZRADIO_CMD_ID_GET_PROPERTY;
  ezradioCmd[1] = group;
  ezradioCmd[2] = num_props;
  ezradioCmd[3] = start_prop;

  ezradio_comm_SendCmdGetResp(EZRADIO_CMD_ARG_COUNT_GET_PROPERTY,
                              ezradioCmd,
                              ezradioCmd[2],
                              ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->GET_PROPERTY.DATA[0]   = ezradioCmd[0];
    ezradioReply->GET_PROPERTY.DATA[1]   = ezradioCmd[1];
    ezradioReply->GET_PROPERTY.DATA[2]   = ezradioCmd[2];
    ezradioReply->GET_PROPERTY.DATA[3]   = ezradioCmd[3];
    ezradioReply->GET_PROPERTY.DATA[4]   = ezradioCmd[4];
    ezradioReply->GET_PROPERTY.DATA[5]   = ezradioCmd[5];
    ezradioReply->GET_PROPERTY.DATA[6]   = ezradioCmd[6];
    ezradioReply->GET_PROPERTY.DATA[7]   = ezradioCmd[7];
    ezradioReply->GET_PROPERTY.DATA[8]   = ezradioCmd[8];
    ezradioReply->GET_PROPERTY.DATA[9]   = ezradioCmd[9];
    ezradioReply->GET_PROPERTY.DATA[10]  = ezradioCmd[10];
    ezradioReply->GET_PROPERTY.DATA[11]  = ezradioCmd[11];
    ezradioReply->GET_PROPERTY.DATA[12]  = ezradioCmd[12];
    ezradioReply->GET_PROPERTY.DATA[13]  = ezradioCmd[13];
    ezradioReply->GET_PROPERTY.DATA[14]  = ezradioCmd[14];
    ezradioReply->GET_PROPERTY.DATA[15]  = ezradioCmd[15];
  }
}

#ifdef EZRADIO_DRIVER_FULL_SUPPORT
/* Full driver support functions */

/**
 * Sends the FUNC_INFO command to the radio, then reads the resonse into ezradioReply union.
 *
 * @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_func_info(ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_FUNC_INFO];

  ezradioCmd[0] = EZRADIO_CMD_ID_FUNC_INFO;

  ezradio_comm_SendCmdGetResp(EZRADIO_CMD_ARG_COUNT_FUNC_INFO,
                              ezradioCmd,
                              EZRADIO_CMD_REPLY_COUNT_FUNC_INFO,
                              ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->FUNC_INFO.REVEXT          = ezradioCmd[0];
    ezradioReply->FUNC_INFO.REVBRANCH       = ezradioCmd[1];
    ezradioReply->FUNC_INFO.REVINT          = ezradioCmd[2];
    ezradioReply->FUNC_INFO.FUNC            = ezradioCmd[5];
  }
}

/**
 * Reads the Fast Response Registers starting with A register into ezradioReply union.
 *
 * @param[in] respByteCount Number of Fast Response Registers to be read.
 * @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_frr_a_read(uint8_t respByteCount, ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[4u];

  ezradio_comm_ReadData(EZRADIO_CMD_ID_FRR_A_READ,
                        0,
                        respByteCount,
                        ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->FRR_A_READ.FRR_A_VALUE = ezradioCmd[0];
    ezradioReply->FRR_A_READ.FRR_B_VALUE = ezradioCmd[1];
    ezradioReply->FRR_A_READ.FRR_C_VALUE = ezradioCmd[2];
    ezradioReply->FRR_A_READ.FRR_D_VALUE = ezradioCmd[3];
  }
}

/**
 * Reads the Fast Response Registers starting with B register into ezradioReply union.
 *
 * @param[in] respByteCount Number of Fast Response Registers to be read.
 * @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_frr_b_read(uint8_t respByteCount, ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[4u];

  ezradio_comm_ReadData(EZRADIO_CMD_ID_FRR_B_READ,
                        0,
                        respByteCount,
                        ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->FRR_B_READ.FRR_B_VALUE = ezradioCmd[0];
    ezradioReply->FRR_B_READ.FRR_C_VALUE = ezradioCmd[1];
    ezradioReply->FRR_B_READ.FRR_D_VALUE = ezradioCmd[2];
    ezradioReply->FRR_B_READ.FRR_A_VALUE = ezradioCmd[3];
  }
}

/**
 * Reads the Fast Response Registers starting with C register into ezradioReply union.
 *
 * @param[in] respByteCount Number of Fast Response Registers to be read.
 * @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_frr_c_read(uint8_t respByteCount, ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[4u];

  ezradio_comm_ReadData(EZRADIO_CMD_ID_FRR_C_READ,
                        0,
                        respByteCount,
                        ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->FRR_C_READ.FRR_C_VALUE = ezradioCmd[0];
    ezradioReply->FRR_C_READ.FRR_D_VALUE = ezradioCmd[1];
    ezradioReply->FRR_C_READ.FRR_A_VALUE = ezradioCmd[2];
    ezradioReply->FRR_C_READ.FRR_B_VALUE = ezradioCmd[3];
  }
}

/**
 * Reads the Fast Response Registers starting with D register into ezradioReply union.
 *
 * @param[in] respByteCount Number of Fast Response Registers to be read.
 * @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_frr_d_read(uint8_t respByteCount, ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[4u];

  ezradio_comm_ReadData(EZRADIO_CMD_ID_FRR_D_READ,
                        0,
                        respByteCount,
                        ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->FRR_D_READ.FRR_D_VALUE = ezradioCmd[0];
    ezradioReply->FRR_D_READ.FRR_A_VALUE = ezradioCmd[1];
    ezradioReply->FRR_D_READ.FRR_B_VALUE = ezradioCmd[2];
    ezradioReply->FRR_D_READ.FRR_C_VALUE = ezradioCmd[3];
  }
}

/**
 * Requests the current state of the device and lists pending TX and RX requests.
 *
 * @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_request_device_state(ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_REQUEST_DEVICE_STATE];

  ezradioCmd[0] = EZRADIO_CMD_ID_REQUEST_DEVICE_STATE;

  ezradio_comm_SendCmdGetResp(EZRADIO_CMD_ARG_COUNT_REQUEST_DEVICE_STATE,
                              ezradioCmd,
                              EZRADIO_CMD_REPLY_COUNT_REQUEST_DEVICE_STATE,
                              ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->REQUEST_DEVICE_STATE.CURR_STATE       = ezradioCmd[0];
    ezradioReply->REQUEST_DEVICE_STATE.CURRENT_CHANNEL  = ezradioCmd[1];
  }
}

/**
 * Reads the command buffer
 *
 * @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_read_cmd_buff(ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_READ_CMD_BUFF];

  ezradioCmd[0] = EZRADIO_CMD_ID_READ_CMD_BUFF;

  ezradio_comm_SendCmdGetResp(EZRADIO_CMD_ARG_COUNT_READ_CMD_BUFF,
                              ezradioCmd,
                              EZRADIO_CMD_REPLY_COUNT_READ_CMD_BUFF,
                              ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->READ_CMD_BUFF.BYTE[0]   = ezradioCmd[0];
    ezradioReply->READ_CMD_BUFF.BYTE[1]   = ezradioCmd[1];
    ezradioReply->READ_CMD_BUFF.BYTE[2]   = ezradioCmd[2];
    ezradioReply->READ_CMD_BUFF.BYTE[3]   = ezradioCmd[3];
    ezradioReply->READ_CMD_BUFF.BYTE[4]   = ezradioCmd[4];
    ezradioReply->READ_CMD_BUFF.BYTE[5]   = ezradioCmd[5];
    ezradioReply->READ_CMD_BUFF.BYTE[6]   = ezradioCmd[6];
    ezradioReply->READ_CMD_BUFF.BYTE[7]   = ezradioCmd[7];
    ezradioReply->READ_CMD_BUFF.BYTE[8]   = ezradioCmd[8];
    ezradioReply->READ_CMD_BUFF.BYTE[9]   = ezradioCmd[9];
    ezradioReply->READ_CMD_BUFF.BYTE[10]  = ezradioCmd[10];
    ezradioReply->READ_CMD_BUFF.BYTE[11]  = ezradioCmd[11];
    ezradioReply->READ_CMD_BUFF.BYTE[12]  = ezradioCmd[12];
    ezradioReply->READ_CMD_BUFF.BYTE[13]  = ezradioCmd[13];
    ezradioReply->READ_CMD_BUFF.BYTE[14]  = ezradioCmd[14];
    ezradioReply->READ_CMD_BUFF.BYTE[15]  = ezradioCmd[15];
  }
}

/**
 * Gets the Packet Handler status flags. Optionally clears them.
 *
 * @param[in] ph_clr_pend Flags to clear.
 * @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_get_ph_status(uint8_t ph_clr_pend, ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_GET_PH_STATUS];

  ezradioCmd[0] = EZRADIO_CMD_ID_GET_PH_STATUS;
  ezradioCmd[1] = ph_clr_pend;

  ezradio_comm_SendCmdGetResp(EZRADIO_CMD_ARG_COUNT_GET_PH_STATUS,
                              ezradioCmd,
                              EZRADIO_CMD_REPLY_COUNT_GET_PH_STATUS,
                              ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->GET_PH_STATUS.PH_PEND        = ezradioCmd[0];
    ezradioReply->GET_PH_STATUS.PH_STATUS      = ezradioCmd[1];
  }
}

/**
 * Gets the Modem status flags. Optionally clears them.
 *
 * @param[in] modem_clr_pend Flags to clear.
 * @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_get_modem_status(uint8_t modem_clr_pend, ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_GET_MODEM_STATUS];

  ezradioCmd[0] = EZRADIO_CMD_ID_GET_MODEM_STATUS;
  ezradioCmd[1] = modem_clr_pend;

  ezradio_comm_SendCmdGetResp(EZRADIO_CMD_ARG_COUNT_GET_MODEM_STATUS,
                              ezradioCmd,
                              EZRADIO_CMD_REPLY_COUNT_GET_MODEM_STATUS,
                              ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->GET_MODEM_STATUS.MODEM_PEND   = ezradioCmd[0];
    ezradioReply->GET_MODEM_STATUS.MODEM_STATUS = ezradioCmd[1];
    ezradioReply->GET_MODEM_STATUS.CURR_RSSI    = ezradioCmd[2];
    ezradioReply->GET_MODEM_STATUS.LATCH_RSSI   = ezradioCmd[3];
    ezradioReply->GET_MODEM_STATUS.ANT1_RSSI    = ezradioCmd[4];
    ezradioReply->GET_MODEM_STATUS.ANT2_RSSI    = ezradioCmd[5];
    ezradioReply->GET_MODEM_STATUS.AFC_FREQ_OFFSET =  ((uint16_t)ezradioCmd[6] << 8) & 0xFF00;
    ezradioReply->GET_MODEM_STATUS.AFC_FREQ_OFFSET |= (uint16_t)ezradioCmd[7] & 0x00FF;
  }
}

/**
 * Gets the Chip status flags. Optionally clears them.
 *
 * @param[in] chip_clr_pend Flags to clear.
 * @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_get_chip_status(uint8_t chip_clr_pend, ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_GET_CHIP_STATUS];

  ezradioCmd[0] = EZRADIO_CMD_ID_GET_CHIP_STATUS;
  ezradioCmd[1] = chip_clr_pend;

  ezradio_comm_SendCmdGetResp(EZRADIO_CMD_ARG_COUNT_GET_CHIP_STATUS,
                              ezradioCmd,
                              EZRADIO_CMD_REPLY_COUNT_GET_CHIP_STATUS,
                              ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->GET_CHIP_STATUS.CHIP_PEND         = ezradioCmd[0];
    ezradioReply->GET_CHIP_STATUS.CHIP_STATUS       = ezradioCmd[1];
    ezradioReply->GET_CHIP_STATUS.CMD_ERR_STATUS    = ezradioCmd[2];
  }
}

/**
 * Receives information from the radio of the current packet. Optionally can be used to modify
 * the Packet Handler properties during packet reception.
 *
 * @param[in] field_number_mask Packet Field number mask value.
 * @param[in] len               Length value.
 * @param[in] diff_len          Difference length.
 * @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_get_packet_info(uint8_t field_number_mask, uint16_t len, int16_t diff_len, ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[EZRADIO_CMD_ARG_COUNT_PACKET_INFO];

  ezradioCmd[0] = EZRADIO_CMD_ID_PACKET_INFO;
  ezradioCmd[1] = field_number_mask;
  ezradioCmd[2] = (uint8_t)(len >> 8);
  ezradioCmd[3] = (uint8_t)(len);
  // the different of the byte, althrough it is signed, but to command hander
  // it can treat it as unsigned
  ezradioCmd[4] = (uint8_t)((uint16_t)diff_len >> 8);
  ezradioCmd[5] = (uint8_t)(diff_len);

  ezradio_comm_SendCmdGetResp(EZRADIO_CMD_ARG_COUNT_PACKET_INFO,
                              ezradioCmd,
                              EZRADIO_CMD_REPLY_COUNT_PACKET_INFO,
                              ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->PACKET_INFO.LENGTH = ((uint16_t)ezradioCmd[0] << 8) & 0xFF00;
    ezradioReply->PACKET_INFO.LENGTH |= (uint16_t)ezradioCmd[1] & 0x00FF;
  }
}

/*
 * Faster versions of the above commands.
 */

/** Sends START_TX command ID to the radio with no input parameters
 *
 */
void ezradio_start_tx_fast(void)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[1u];

  ezradioCmd[0] = EZRADIO_CMD_ID_START_TX;

  ezradio_comm_SendCmd(1u, ezradioCmd);
}

/**
 * Sends START_RX command ID to the radio with no input parameters
 *
 */
void ezradio_start_rx_fast(void)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[1u];

  ezradioCmd[0] = EZRADIO_CMD_ID_START_RX;

  ezradio_comm_SendCmd(1u, ezradioCmd);
}

/**
 * Clear all Interrupt status/pending flags. Does NOT read back interrupt flags
 *
 */
void ezradio_get_int_status_fast_clear(void)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[1u];

  ezradioCmd[0] = EZRADIO_CMD_ID_GET_INT_STATUS;

  ezradio_comm_SendCmd(1u, ezradioCmd);
}

/**
 * Clear and read all Interrupt status/pending flags
 *
 * @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_get_int_status_fast_clear_read(ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_GET_INT_STATUS];

  ezradioCmd[0] = EZRADIO_CMD_ID_GET_INT_STATUS;

  ezradio_comm_SendCmdGetResp(1u,
                              ezradioCmd,
                              EZRADIO_CMD_REPLY_COUNT_GET_INT_STATUS,
                              ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->GET_INT_STATUS.INT_PEND       = ezradioCmd[0];
    ezradioReply->GET_INT_STATUS.INT_STATUS     = ezradioCmd[1];
    ezradioReply->GET_INT_STATUS.PH_PEND        = ezradioCmd[2];
    ezradioReply->GET_INT_STATUS.PH_STATUS      = ezradioCmd[3];
    ezradioReply->GET_INT_STATUS.MODEM_PEND     = ezradioCmd[4];
    ezradioReply->GET_INT_STATUS.MODEM_STATUS   = ezradioCmd[5];
    ezradioReply->GET_INT_STATUS.CHIP_PEND      = ezradioCmd[6];
    ezradioReply->GET_INT_STATUS.CHIP_STATUS    = ezradioCmd[7];
  }
}

/**
 * Reads back current GPIO pin configuration. Does NOT configure GPIO pins
 *
 * @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_gpio_pin_cfg_fast(ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_GPIO_PIN_CFG];

  ezradioCmd[0] = EZRADIO_CMD_ID_GPIO_PIN_CFG;

  ezradio_comm_SendCmdGetResp(1u,
                              ezradioCmd,
                              EZRADIO_CMD_REPLY_COUNT_GPIO_PIN_CFG,
                              ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->GPIO_PIN_CFG.gpIO[0]      = ezradioCmd[0];
    ezradioReply->GPIO_PIN_CFG.gpIO[1]      = ezradioCmd[1];
    ezradioReply->GPIO_PIN_CFG.gpIO[2]      = ezradioCmd[2];
    ezradioReply->GPIO_PIN_CFG.gpIO[3]      = ezradioCmd[3];
    ezradioReply->GPIO_PIN_CFG.NIRQ         = ezradioCmd[4];
    ezradioReply->GPIO_PIN_CFG.SDO          = ezradioCmd[5];
    ezradioReply->GPIO_PIN_CFG.GEN_CONFIG   = ezradioCmd[6];
  }
}

/**
 * Clear all Packet Handler status flags. Does NOT read back interrupt flags
 *
 */
void ezradio_get_ph_status_fast_clear(void)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[2u];

  ezradioCmd[0] = EZRADIO_CMD_ID_GET_PH_STATUS;
  ezradioCmd[1] = 0;

  ezradio_comm_SendCmd(2u, ezradioCmd);
}

/**
 * Clear and read all Packet Handler status flags.
 *
 * @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_get_ph_status_fast_clear_read(ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_GET_PH_STATUS];

  ezradioCmd[0] = EZRADIO_CMD_ID_GET_PH_STATUS;

  ezradio_comm_SendCmdGetResp(1u,
                              ezradioCmd,
                              EZRADIO_CMD_REPLY_COUNT_GET_PH_STATUS,
                              ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->GET_PH_STATUS.PH_PEND        = ezradioCmd[0];
    ezradioReply->GET_PH_STATUS.PH_STATUS      = ezradioCmd[1];
  }
}

/**
 * Clear all Modem status flags. Does NOT read back interrupt flags
 *
 */
void ezradio_get_modem_status_fast_clear(void)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[2u];

  ezradioCmd[0] = EZRADIO_CMD_ID_GET_MODEM_STATUS;
  ezradioCmd[1] = 0;

  ezradio_comm_SendCmd(2u, ezradioCmd);
}

/**
 * Clear and read all Modem status flags.
 *
 * @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_get_modem_status_fast_clear_read(ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_GET_MODEM_STATUS];

  ezradioCmd[0] = EZRADIO_CMD_ID_GET_MODEM_STATUS;

  ezradio_comm_SendCmdGetResp(1u,
                              ezradioCmd,
                              EZRADIO_CMD_REPLY_COUNT_GET_MODEM_STATUS,
                              ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->GET_MODEM_STATUS.MODEM_PEND   = ezradioCmd[0];
    ezradioReply->GET_MODEM_STATUS.MODEM_STATUS = ezradioCmd[1];
    ezradioReply->GET_MODEM_STATUS.CURR_RSSI    = ezradioCmd[2];
    ezradioReply->GET_MODEM_STATUS.LATCH_RSSI   = ezradioCmd[3];
    ezradioReply->GET_MODEM_STATUS.ANT1_RSSI    = ezradioCmd[4];
    ezradioReply->GET_MODEM_STATUS.ANT2_RSSI    = ezradioCmd[5];
    ezradioReply->GET_MODEM_STATUS.AFC_FREQ_OFFSET = ((uint16_t)ezradioCmd[6] << 8) & 0xFF00;
    ezradioReply->GET_MODEM_STATUS.AFC_FREQ_OFFSET |= (uint16_t)ezradioCmd[7] & 0x00FF;
  }
}

/**
 * Clear all Chip status flags. Does NOT read back interrupt flags
 *
 */
void ezradio_get_chip_status_fast_clear(void)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[2u];

  ezradioCmd[0] = EZRADIO_CMD_ID_GET_CHIP_STATUS;
  ezradioCmd[1] = 0;

  ezradio_comm_SendCmd(2u, ezradioCmd);
}

/**
 * Clear and read all Chip status flags.
 *
 * @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_get_chip_status_fast_clear_read(ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_GET_CHIP_STATUS];

  ezradioCmd[0] = EZRADIO_CMD_ID_GET_CHIP_STATUS;

  ezradio_comm_SendCmdGetResp(1u,
                              ezradioCmd,
                              EZRADIO_CMD_REPLY_COUNT_GET_CHIP_STATUS,
                              ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->GET_CHIP_STATUS.CHIP_PEND         = ezradioCmd[0];
    ezradioReply->GET_CHIP_STATUS.CHIP_STATUS       = ezradioCmd[1];
    ezradioReply->GET_CHIP_STATUS.CMD_ERR_STATUS    = ezradioCmd[2];
  }
}

/**
 * Resets the RX/TX FIFO. Does not read back anything from TX/RX FIFO
 *
 * @param[in] fifo FIFO to be reset.
 */
void ezradio_fifo_info_fast_reset(uint8_t fifo)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[2u];

  ezradioCmd[0] = EZRADIO_CMD_ID_FIFO_INFO;
  ezradioCmd[1] = fifo;

  ezradio_comm_SendCmd(2u, ezradioCmd);
}

/**
 * Reads RX/TX FIFO count space. Does NOT reset RX/TX FIFO
 *
 * @param[out] ezradioReply   Reply structure of the command.
 */
void ezradio_fifo_info_fast_read(ezradio_cmd_reply_t *ezradioReply)
{
  /* EZRadio command buffer */
  uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_FIFO_INFO];

  ezradioCmd[0] = EZRADIO_CMD_ID_FIFO_INFO;

  ezradio_comm_SendCmdGetResp(1u,
                              ezradioCmd,
                              EZRADIO_CMD_REPLY_COUNT_FIFO_INFO,
                              ezradioCmd);

  if (ezradioReply != NULL) {
    ezradioReply->FIFO_INFO.RX_FIFO_COUNT   = ezradioCmd[0];
    ezradioReply->FIFO_INFO.TX_FIFO_SPACE   = ezradioCmd[1];
  }
}

#endif /* EZRADIO_DRIVER_FULL_SUPPORT */

#endif /* EZRADIO_DRIVER_EXTENDED_SUPPORT */
