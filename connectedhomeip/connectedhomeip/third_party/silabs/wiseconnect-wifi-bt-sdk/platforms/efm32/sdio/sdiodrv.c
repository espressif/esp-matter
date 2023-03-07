/***************************************************************************//**
 * @file
 * @brief SDIODRV API implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
#include <stdio.h>
#include "em_device.h"
#include "em_core.h"
#include "em_gpio.h"
#include "sdiodrv.h"
#include "rsi_driver.h"

#define SDIODRV_DFLT_FREQ_HZ        50000000

#define CMD7_RCA_MASK               0xFFFF0000
#define CMD7_RCA_SHIFT              16
#define CMD52_RW_FLAG_SHIFT         31
#define CMD52_FNCT_NB_MASK          0x70000000
#define CMD52_FNCT_NB_SHIFT         28
#define CMD52_REG_ADDR_MASK         0x03FFFE00
#define CMD52_REG_ADDR_SHIFT        9
#define CMD53_RW_FLAG_SHIFT         31
#define CMD53_FNCT_NB_MASK          0x70000000
#define CMD53_FNCT_NB_SHIFT         28
#define CMD53_BLOCK_MODE_MASK       0x08000000
#define CMD53_BLOCK_MODE_SHIFT      27
#define CMD53_OP_CODE_SHIFT         26
#define CMD53_REG_ADDR_MASK         0x03FFFE00
#define CMD53_REG_ADDR_SHIFT        9
#define CMD53_BYTE_BLOCK_CNT_MASK   0x000001FF
#define R5_RESP_FLAGS_MASK          0x0000FF00
#define R5_RESP_FLAGS_SHIFT         8
#define R5_RW_DATA_MASK             0x000000FF
#define R6_RCA_MASK                 0xFFFF0000
#define R6_RCA_SHIFT                16
#define R6_STATUS_MASK              0x0000FFFF

#define RESP_FLAG_OUT_OF_RANGE_MASK           0x01
#define RESP_FLAG_FUNC_NB_MASK                0x02
#define RESP_FLAG_ERROR_MASK                  0x08
#define RESP_FLAG_IO_CURRENT_STATE_MASK       0x30
#define RESP_FLAG_IO_CURRENT_STATE_DIS_MASK   0x00
#define RESP_FLAG_IO_CURRENT_STATE_CMD_MASK   0x10
#define RESP_FLAG_IO_CURRENT_STATE_TRN_MASK   0x20
#define RESP_FLAG_ILLEGAL_CMD_MASK            0x40
#define RESP_FLAG_COM_CRC_ERROR_MASK          0x80
//Added by Silabs
#define DEBUG_BREAK           __asm__("BKPT #0");

static bool sdiodrv_initialized = false;
static SDIODRV_Handle_t *sdiodrv_handle = NULL;
static SDIODRV_Callbacks_t sdiodrv_callbacks;



/** Configure the SDIO GPIOs. */
static void config_gpio (SDIODRV_Init_t *init)
{
  GPIO_Mode_TypeDef gpio_mode;

  gpio_mode = init->portLocationClk == 0 ?
               gpioModePushPull : gpioModePushPullAlternate;
   GPIO_PinModeSet((GPIO_Port_TypeDef)SL_GPIO_PORT_E, 13,
                   gpio_mode,
                   1);

   gpio_mode = init->portLocationCmd == 0 ?
               gpioModePushPull : gpioModePushPullAlternate;
   GPIO_PinModeSet((GPIO_Port_TypeDef)SL_GPIO_PORT_E, 12,
                   gpio_mode,
                   1);

   gpio_mode = init->portLocationDat == 0 ?
               gpioModePushPull : gpioModePushPullAlternate;
   GPIO_PinModeSet((GPIO_Port_TypeDef)SL_GPIO_PORT_E, 11,
                   gpio_mode,
                   1);

   if ((init->transferWidth == SDIO_TRANSFER_WIDTH_4BIT)
       || (init->transferWidth == SDIO_TRANSFER_WIDTH_8BIT)) {

     GPIO_PinModeSet((GPIO_Port_TypeDef)SL_GPIO_PORT_E, 10,
                     gpio_mode,
                     1);

     GPIO_PinModeSet((GPIO_Port_TypeDef)SL_GPIO_PORT_E, 9,
                     gpio_mode,
                     1);

     GPIO_PinModeSet((GPIO_Port_TypeDef)SL_GPIO_PORT_E, 8,
                     gpio_mode,
                     1);

    if (init->transferWidth == SDIO_TRANSFER_WIDTH_8BIT) {

      GPIO_PinModeSet((GPIO_Port_TypeDef)AF_SDIO_DAT4_PORT(init->portLocationDat),
                      AF_SDIO_DAT4_PIN(init->portLocationDat),
                      gpio_mode,
                      1);

      GPIO_PinModeSet((GPIO_Port_TypeDef)AF_SDIO_DAT5_PORT(init->portLocationDat),
                      AF_SDIO_DAT5_PIN(init->portLocationDat),
                      gpio_mode,
                      1);

      GPIO_PinModeSet((GPIO_Port_TypeDef)AF_SDIO_DAT6_PORT(init->portLocationDat),
                      AF_SDIO_DAT6_PIN(init->portLocationDat),
                      gpio_mode,
                      1);

      GPIO_PinModeSet((GPIO_Port_TypeDef)AF_SDIO_DAT7_PORT(init->portLocationDat),
                      AF_SDIO_DAT7_PIN(init->portLocationDat),
                      gpio_mode,
                      1);
    }
  }
}

/** Check command completion, to use only after blocking TX. */
static uint32_t check_command_completion (SDIODRV_Handle_t *handle)
{
  uint32_t ret = SDIODRV_ERROR_CMDRESP;
  uint32_t ifcr;

  ifcr = SDIO_GetCmdStatus(handle->init.instance);
  if (((ifcr & _SDIO_IFCR_CMDCOM_MASK) == _SDIO_IFCR_CMDCOM_MASK)
      && ((ifcr & _SDIO_IFCR_ERRINT_MASK) == 0)) {
    // Command success
    ret = SDIODRV_ERROR_NONE;
  }

  return ret;
}

/** Check response flags, useful after sending a command 52 or 53. */
static uint32_t check_response_flags (uint8_t flags, uint8_t expected_state)
{
  uint32_t ret = SDIODRV_ERROR_NONE;

  if (flags & RESP_FLAG_OUT_OF_RANGE_MASK) {
    ret |= SDIODRV_ERROR_RESP_OUT_OF_RANGE;
  }

  if (flags & RESP_FLAG_FUNC_NB_MASK) {
    ret |= SDIODRV_ERROR_RESP_FUNC_NB;
  }

  if (flags & RESP_FLAG_ERROR_MASK) {
    ret |= SDIODRV_ERROR_RESP_UNKNOWN;
  }

  if ((flags & expected_state) != expected_state) {
    ret |= SDIODRV_ERROR_RESP_WRONG_STATE;
  }

  if (flags & RESP_FLAG_ILLEGAL_CMD_MASK) {
    ret |= SDIODRV_ERROR_RESP_ILLEGAL_CMD;
  }

  if (flags & RESP_FLAG_COM_CRC_ERROR_MASK) {
    ret |= SDIODRV_ERROR_RESP_CRC;
  }

  return ret;
}

static uint32_t send_cmd0_go_idle_state (SDIODRV_Handle_t *handle)
{
  SDIO_Cmd_t cmd0 = {
    .index = 0,
    .args = 0,
    .dataDirection = SDIO_DATA_DIRECTION_NODATA,
    .respType = SDIO_RESP_TYPE_NORESP, // Some cards answer to this command
    .checkCrc = true,
    .checkIndex = true,
    .enableDma = false,
    .multiblocks = false
  };

  // Send the command
  SDIO_TxCmdB(handle->init.instance, &cmd0);
  rsi_delay_ms(100);
  // Check the command completion  (response not checked)
  return check_command_completion(handle);
}

static uint32_t send_cmd8_send_if_conf (SDIODRV_Handle_t *handle)
{
  SDIO_Cmd_t cmd8 = {
    .index = 8,
    .args = 0x00000100, //Host supply voltage range 2.7 - 3.6V, Check pattern 0x00
    .dataDirection = SDIO_DATA_DIRECTION_NODATA,
    .respType = SDIO_RESP_TYPE_NORESP,
    .checkCrc = true,
    .checkIndex = true,
    .enableDma = false,
    .multiblocks = false
  };

  // Send the command
  SDIO_TxCmdB(handle->init.instance, &cmd8);
  rsi_delay_ms(100);
  // Check the command completion (response not checked)
  return check_command_completion(handle);
}

static uint32_t send_cmd5_sendOperationCondition (SDIODRV_Handle_t *handle, uint32_t *ocr)
{
  uint32_t res, resp;
  SDIO_Cmd_t cmd5 = {
    .index = 5,
    .args = 0,
    .dataDirection = SDIO_DATA_DIRECTION_NODATA,
    .respType = SDIO_RESP_TYPE_RESP48,
    .checkCrc = false,
    .checkIndex = false,
    .enableDma = false,
    .multiblocks = false
  };

  // Send the command
  SDIO_TxCmdB(handle->init.instance, &cmd5);
  rsi_delay_ms(100);
  // Check the command completion
  res = check_command_completion(handle);
      resp = SDIO_GetResp(handle->init.instance, 0);
      //cmd5.args = resp | 0x00100000;
    *ocr = (uint32_t)(resp);// & R6_RCA_MASK) >> R6_RCA_SHIFT);
    if (*ocr == 0) {
      // Error
      res = SDIODRV_ERROR_RCA;
    }
  //}

  return res;
}

static uint32_t send_cmd5_sendOperationCondition_updated_vdd(SDIODRV_Handle_t *handle, uint32_t *ocr)
{
  uint32_t res, resp;
  resp = *ocr;
  SDIO_Cmd_t cmd5 = {
    .index = 5,
    .args = resp | 0x00100000,
    .dataDirection = SDIO_DATA_DIRECTION_NODATA,
    .respType = SDIO_RESP_TYPE_RESP48,
    .checkCrc = false,
    .checkIndex = false,
    .enableDma = false,
    .multiblocks = false
  };

  // Send the command
  SDIO_TxCmdB(handle->init.instance, &cmd5);
  rsi_delay_ms(100);
  // Check the command completion
  res = check_command_completion(handle);
      resp = SDIO_GetResp(handle->init.instance, 0);
    *ocr = (uint32_t)(resp);// & R6_RCA_MASK) >> R6_RCA_SHIFT);
    if (*ocr == 0) {
      // Error
      res = SDIODRV_ERROR_RCA;
    }
  //}

  return res;
}


static uint32_t send_cmd3_send_relative_addr (SDIODRV_Handle_t *handle, uint16_t *rca)
{
  uint32_t res, resp;
  SDIO_Cmd_t cmd3 = {
    .index = 3,
    .args = 0,
    .dataDirection = SDIO_DATA_DIRECTION_NODATA,
    .respType = SDIO_RESP_TYPE_RESP48,
    .checkCrc = false,
    .checkIndex = false,
    .enableDma = false,
    .multiblocks = false
  };

  // Send the command
  SDIO_TxCmdB(handle->init.instance, &cmd3);
  rsi_delay_ms(100);
  // Check the command completion
  res = check_command_completion(handle);
  if (res == SDIODRV_ERROR_NONE) {
    // Command successful, retrieve the RCA from the response
    resp = SDIO_GetResp(handle->init.instance, 0);
    *rca = (uint16_t)((resp & R6_RCA_MASK) >> R6_RCA_SHIFT);
    if (*rca == 0) {
      // Error
      res = SDIODRV_ERROR_RCA;
    }
  }

  return res;
}

/**
 * Internal function used to realize a response checking after a
 * read/write operation and abstract the SDIO specificities from the user.
 */
static void read_write_checking_callback (SDIODRV_Event_t evt, uint32_t error)
{
  uint32_t res;
  uint32_t resp;
  uint8_t  flags;

  (void)error;

  if ((evt == SDIODRV_EVENT_CMD_COMPLETE)
       && (sdiodrv_handle->cmd_idx == 52)) {

    // Retrieve the response and check the status flag
    resp = SDIO_GetResp(sdiodrv_handle->init.instance, 0);
    flags = ((resp & R5_RESP_FLAGS_MASK) >> R5_RESP_FLAGS_SHIFT);
    res = check_response_flags(flags, RESP_FLAG_IO_CURRENT_STATE_CMD_MASK);

    // Provide the result to the application
    if ((res == SDIODRV_ERROR_NONE)
        && (sdiodrv_handle->op == SDIODRV_IO_OP_READ)) {
      // Read success, update the user variable
      *sdiodrv_handle->dataAddr = (uint8_t)(resp & R5_RW_DATA_MASK);
    }

    // Communication finished, restore the application callback
    // and forward the event.
    sdiodrv_callbacks.comEvtCb = sdiodrv_handle->appComEvtCb;
    sdiodrv_callbacks.comEvtCb(evt, res);

  } else if (evt == SDIODRV_EVENT_TRANS_COMPLETE) {

    // Retrieve the response and check the status flag
    resp = SDIO_GetResp(sdiodrv_handle->init.instance, 0);
    flags = ((resp & R5_RESP_FLAGS_MASK) >> R5_RESP_FLAGS_SHIFT);
    res = check_response_flags(flags, RESP_FLAG_IO_CURRENT_STATE_TRN_MASK);

    // Communication finished, restore the application callback
    // and forward the event.
    sdiodrv_callbacks.comEvtCb = sdiodrv_handle->appComEvtCb;
    sdiodrv_callbacks.comEvtCb(evt, res);

  } else if (evt == SDIODRV_EVENT_COM_ERROR) {

    // Communication finished, restore the application callback
    // and forward the event.
    sdiodrv_callbacks.comEvtCb = sdiodrv_handle->appComEvtCb;
    sdiodrv_callbacks.comEvtCb(evt, SDIODRV_ERROR_CMDRESP);

  } else {
    // Communication not finished, forward the event to the application
    sdiodrv_handle->appComEvtCb(evt, SDIODRV_ERROR_NONE);
  }
}

/***************************************************************************//**
 * @brief
 *   Initialize a SDIODRV instance.
 *
 * @param[in] handle
 *   A pointer to the SDIODRV Handle.
 *
 * @param[in] init
 *   A pointer to the SDIODRV initialization structure.
 *
 * @return
 *   SDIODRV_ERROR_NONE on success or SDIODRV_ERROR_PARAM on failure.
 ******************************************************************************/
uint32_t SDIODRV_Init (SDIODRV_Handle_t *handle, SDIODRV_Init_t *init)
{
  SDIO_Init_TypeDef_t sdio_init;

  if ((handle == NULL) || (init == NULL)) {
    return SDIODRV_ERROR_PARAM;
  }

#if defined(SDIO)
  if (init->instance != SDIO) {
    return SDIODRV_ERROR_PARAM;
  }
#endif

  // Save the initialization configuration
  handle->init = *init;

  // Start the clocks necessary to the peripheral
  switch(init->clockSource) {

    case cmuSelect_HFRCO:
    case cmuSelect_HFXO:
    case cmuSelect_AUXHFRCO:
    case cmuSelect_USHFRCO:
      CMU->HFBUSCLKEN0 |= CMU_HFBUSCLKEN0_SDIO;
      CMU_ClockSelectSet(cmuClock_SDIOREF, init->clockSource);
      CMU_ClockEnable(cmuClock_SDIOREF, true);
      CMU_ClockEnable(cmuClock_GPIO, true);
     // CMU_ClockEnable(cmuClock_CORE, true);
    //CMU_ClockEnable(cmuClock_GPIO, true);
      break;

    default:
      // Wrong clock source
      return SDIODRV_ERROR_PARAM;
  }

  // Initialize the SDIO GPIOs
  config_gpio(init);

  // Configure the SDIO according to the BSP
  init->instance->ROUTELOC0 =   (init->portLocationClk  << _SDIO_ROUTELOC0_CLKLOC_SHIFT)
                              | (init->portLocationWp  << _SDIO_ROUTELOC0_WPLOC_SHIFT)
                              | (init->portLocationCd << _SDIO_ROUTELOC0_CDLOC_SHIFT)
                              | (init->portLocationDat  << _SDIO_ROUTELOC0_DATLOC_SHIFT);

  init->instance->ROUTELOC1 =  (init->portLocationCmd  << _SDIO_ROUTELOC1_CMDLOC_MASK);

  switch (init->transferWidth) {
    case SDIO_TRANSFER_WIDTH_1BIT:
      init->instance->ROUTEPEN =   SDIO_ROUTEPEN_CLKPEN
                                 | SDIO_ROUTEPEN_CMDPEN
                                 | SDIO_ROUTEPEN_D0PEN;
      break;
    case SDIO_TRANSFER_WIDTH_4BIT:
      init->instance->ROUTEPEN =   SDIO_ROUTEPEN_CLKPEN
                                 | SDIO_ROUTEPEN_CMDPEN
                                 | SDIO_ROUTEPEN_D0PEN
                                 | SDIO_ROUTEPEN_D1PEN
                                 | SDIO_ROUTEPEN_D2PEN
                                 | SDIO_ROUTEPEN_D3PEN;
      break;
    case SDIO_TRANSFER_WIDTH_8BIT:
      init->instance->ROUTEPEN =   SDIO_ROUTEPEN_CLKPEN
                                 | SDIO_ROUTEPEN_CMDPEN
                                 | SDIO_ROUTEPEN_D0PEN
                                 | SDIO_ROUTEPEN_D1PEN
                                 | SDIO_ROUTEPEN_D2PEN
                                 | SDIO_ROUTEPEN_D3PEN
                                 | SDIO_ROUTEPEN_D4PEN
                                 | SDIO_ROUTEPEN_D5PEN
                                 | SDIO_ROUTEPEN_D6PEN
                                 | SDIO_ROUTEPEN_D7PEN;
      break;
    default:
      return SDIODRV_ERROR_PARAM;
  }

  // Initialize the peripheral
  sdio_init.refFreq = CMU_ClockFreqGet(cmuClock_SDIOREF);
  sdio_init.desiredFreq = SDIODRV_DFLT_FREQ_HZ;
  sdio_init.transferWidth = init->transferWidth;
  SDIO_Init(init->instance, &sdio_init);



  sdiodrv_initialized = true;

  return SDIODRV_ERROR_NONE;
}

/***************************************************************************//**
 * @brief
 *   Denitialize a SDIODRV instance.
 *
 * @param[in] handle
 *   A pointer to the SDIODRV Handle.
 *
 * @return
 *   @ref SDIODRV_ERROR_NONE on success.
 ******************************************************************************/
uint32_t SDIODRV_DeInit (SDIODRV_Handle_t *handle)
{
  if (handle == NULL) {
    return SDIODRV_ERROR_PARAM;
  }

  // Deinitialize the peripheral
  SDIO_DeInit(handle->init.instance);
  // Stop the peripheral clock
  CMU_ClockEnable(cmuClock_SDIOREF, false);

  sdiodrv_initialized = false;
  return SDIODRV_ERROR_NONE;
}

/***************************************************************************//**
 * @brief
 *   Enable or disable a SDIO peripheral.
 *
 * @note
 *   The driver must be initialized before calling this function.
 *
 * @param[in] handle
 *   A pointer to the SDIODRV Handle.
 *
 * @param[in] state
 *   Enable or disable state indicator.
 *
 * @return
 *   @ref SDIODRV_ERROR_NONE on success, SDIODRV_ERROR_NOT_INIT if the
 *   driver has not yet been initialized.
 ******************************************************************************/
uint32_t SDIODRV_Enable (SDIODRV_Handle_t *handle, bool state)
{
  if (handle == NULL) {
    return SDIODRV_ERROR_PARAM;
  }

  if (!sdiodrv_initialized) {
    return SDIODRV_ERROR_NOT_INIT;
  }

  if (state) {
    // Start the peripheral clock
    CMU_ClockEnable(cmuClock_SDIOREF, true);
    while ((CMU->STATUS & CMU_STATUS_SDIOCLKENS) == 0);

    handle->init.instance->CLOCKCTRL |= SDIO_CLOCKCTRL_INTCLKEN;
    // Wait for the clock to be stable
    while (!(handle->init.instance->CLOCKCTRL & _SDIO_CLOCKCTRL_INTCLKSTABLE_MASK));
    handle->init.instance->CLOCKCTRL |= SDIO_CLOCKCTRL_SDCLKEN;
  } else {
    handle->init.instance->CLOCKCTRL &= ~( SDIO_CLOCKCTRL_SDCLKEN
                                         | SDIO_CLOCKCTRL_INTCLKEN);

    // Stop the peripheral clock
    CMU_ClockEnable(cmuClock_SDIOREF, false);
    while ((CMU->STATUS & CMU_STATUS_SDIOCLKENS) != 0);
  }
  return SDIODRV_ERROR_NONE;
}

/***************************************************************************//**
 * @brief
 *   Initialize and identify a SDIO capable device connected to the host.
 *
 * @note
 *   This is a blocking function.
 *
 * @param[in] handle
 *   A pointer to the SDIODRV Handle.
 *
 * @param[out] rca
 *   A pointer to a variable being able to store the relative card address
 *   of the identified device.
 *
 * @return
 *   @ref SDIODRV_ERROR_NONE on success.
 *   On failure, an appropriate error is returned.
 ******************************************************************************/
uint32_t SDIODRV_DeviceInitAndIdent (SDIODRV_Handle_t *handle, uint16_t *rca)
{
  uint32_t res;
  uint32_t ref_freq;
  uint32_t ocr_tmp;
  uint16_t rca_tmp;

  if (handle == NULL) {
    return SDIODRV_ERROR_PARAM;
  }

  // GO_IDLE_STATE (CMD0)
  res = send_cmd0_go_idle_state(handle);

  if (res == SDIODRV_ERROR_NONE) {
      rsi_delay_ms(100);
    // SEND_IF_CONF (CMD8)
      res = send_cmd8_send_if_conf(handle);

    if (res == SDIODRV_ERROR_NONE) {
        rsi_delay_ms(100);
      //TODO implement the generic initialization flow
      // IO_SEND_OP_COND (CMD5)
      // IO_SEND_OP_COND (CMD5)
      // (CMD55+ACMD41)
      // (CMD11)
      // (CMD2)
       res = send_cmd5_sendOperationCondition(handle,&ocr_tmp);
       uint8_t   r4_io_ready ,r4_nf, r4_mp;
            uint32_t  r4_ocr ;
            rsi_delay_ms(100);
       while(1){
           res = send_cmd5_sendOperationCondition_updated_vdd(handle,&ocr_tmp);
               r4_io_ready = ocr_tmp>>31;
                   r4_nf = (ocr_tmp>>28)&0x7;
                   r4_mp = (ocr_tmp>>27)&0x1;
                   r4_ocr = (ocr_tmp>>0)&0xffffff;
                   USED_PARAMETER(r4_ocr); //This statement is added only to resolve compilation warning, value is unchanged
                   USED_PARAMETER(r4_mp); //This statement is added only to resolve compilation warning, value is unchanged
                   USED_PARAMETER(r4_nf); //This statement is added only to resolve compilation warning, value is unchanged
                   if (r4_io_ready != 1)
                     {
                       continue;
                       //res = send_cmd0_go_idle_state(handle);
                     }
                     else
                       break;

       }


     //   if (res == SDIODRV_ERROR_NONE) {
      // SEND_RELATIVE_ADDRESS (CMD3)
       rsi_delay_ms(100);
      res = send_cmd3_send_relative_addr(handle, &rca_tmp);
      if (res == SDIODRV_ERROR_NONE) {
        *rca = rca_tmp;

        // Initialization success, switch to the bus frequency requested by the user
        ref_freq = CMU_ClockFreqGet(cmuClock_SDIOREF);
        SDIO_SetClockFrequency(handle->init.instance, ref_freq, handle->init.freq);
     // }
      }
    }
  }

  return res;
}

/***************************************************************************//**
 * @brief
 *   Select or deselect a SDIO capable device connected to the host.
 *
 * @note
 *   This is a blocking function.
 *
 * @param[in] handle
 *   A pointer to the SDIODRV Handle.
 *
 * @param[in] rca
 *   Relative card address identifying a SDIO capable device.
 *
 * @return
 *   @ref SDIODRV_ERROR_NONE on success.
 *   On failure, an appropriate error is returned.
 ******************************************************************************/
uint32_t SDIODRV_SelectCard (SDIODRV_Handle_t *handle, uint16_t rca)
{
  SDIO_Cmd_t cmd7 = {
    .index = 7,
    .args = (uint32_t)((rca << CMD7_RCA_SHIFT) & CMD7_RCA_MASK),
    .dataDirection = SDIO_DATA_DIRECTION_NODATA,
    .respType = SDIO_RESP_TYPE_BUSYAFTRESP,
    .checkCrc = true,
    .checkIndex = true,
    .enableDma = false,
    .multiblocks = false
  };

  if (handle == NULL) {
    return SDIODRV_ERROR_PARAM;
  }

  SDIO_TxCmdB(handle->init.instance, &cmd7);
  rsi_delay_ms(100);
  // Check the command completion  (response not checked)
  return check_command_completion(handle);
}

/***************************************************************************//**
 * @brief
 *   Execute a direct read or write operation.
 *
 * @note
 *   This is NOT a blocking function, an interrupt is generated at the command
 *   completion (or error) calling back the function registered by the application.
 *
 * @param[in] handle
 *   A pointer to the SDIODRV Handle.
 *
 * @param[in] op
 *   Operation indicator.
 *
 * @param[in] function
 *   Function addressed by the operation.
 *
 * @param[in] address
 *   Register address in the selected function.
 *
 * @param[in/out] data
 *   Data to write to the device or address to store the data
 *   read from the device.
 *
 * @return
 *   @ref SDIODRV_ERROR_NONE on success.
 *   On failure, an appropriate error is returned.
 ******************************************************************************/
uint32_t SDIODRV_IOReadWriteDirect (SDIODRV_Handle_t *handle,
                                    SDIODRV_IO_Operation_t op,
                                    uint8_t function,
                                    uint32_t address,
                                    uint8_t *data)
{
  SDIO_Cmd_t cmd52 = {
    .index = 52,
    .args = (uint32_t)(  ((function << CMD52_FNCT_NB_SHIFT) & CMD52_FNCT_NB_MASK)
                       | ((address << CMD52_REG_ADDR_SHIFT) & CMD52_REG_ADDR_MASK)),
    .dataDirection = SDIO_DATA_DIRECTION_NODATA,
    .respType = SDIO_RESP_TYPE_RESP48,
    .checkCrc = true,
    .checkIndex = true,
    .enableDma = false,
    .multiblocks = false
  };

  if ((handle == NULL) || (data == NULL)) {
    return SDIODRV_ERROR_PARAM;
  }

  // Save the current information
  handle->cmd_idx = cmd52.index;
  handle->op = op;
  handle->dataAddr = data;

  if (sdiodrv_callbacks.comEvtCb != read_write_checking_callback) {
    handle->appComEvtCb = sdiodrv_callbacks.comEvtCb;

    // Overload temporarily the application callback to perform
    // additional treatments before calling the real callback.
    sdiodrv_callbacks.comEvtCb = read_write_checking_callback;
  }

  sdiodrv_handle = handle;

  if (op == SDIODRV_IO_OP_WRITE) {
    cmd52.args |= (uint32_t)(  (!!op << CMD52_RW_FLAG_SHIFT)
                             | (*data));
  }

  // Send the command
  SDIO_TxCmd(handle->init.instance, &cmd52);
  return SDIODRV_ERROR_NONE;
}


/***************************************************************************//**
 * @brief
 *   Execute an extended read or write operation using the byte mode.
 *
 * @note
 *   This is NOT a blocking function, an interrupt is generated at the transfer
 *   completion (or error) calling back the function registered by the application.
 *
 * @param[in] handle
 *   A pointer to the SDIODRV Handle.
 *
 * @param[in] op
 *   Operation indicator.
 *
 * @param[in] function
 *   Function addressed by the operation.
 *
 * @param[in] address
 *   Register address in the selected function.
 *
 * @param[in] bytesCnt
 *   Number of bytes to read or write.
 *
 * @param[in/out] data
 *   Data to write to the device or address to store the data
 *   read from the device.
 *
 * @return
 *   @ref SDIODRV_ERROR_NONE on success.
 *   On failure, an appropriate error is returned.
 ******************************************************************************/
uint32_t SDIODRV_IOReadWriteExtendedBytes (SDIODRV_Handle_t *handle,
                                           SDIODRV_IO_Operation_t op,
                                           uint8_t function,
                                           uint32_t address,
                                           uint16_t bytesCnt,
                                           uint8_t *data)
{
  SDIO_Cmd_t cmd53 = {
    .index = 53,
    .args = (uint32_t)(  ((function << CMD53_FNCT_NB_SHIFT) & CMD53_FNCT_NB_MASK)
                       | (1 << CMD53_OP_CODE_SHIFT)
                       | ((address << CMD53_REG_ADDR_SHIFT) & CMD53_REG_ADDR_MASK))
                       | (bytesCnt & CMD53_BYTE_BLOCK_CNT_MASK),
    .dataDirection = SDIO_DATA_DIRECTION_CARD_TO_HOST,
    .respType = SDIO_RESP_TYPE_RESP48,
    .checkCrc = true,
    .checkIndex = true,
    .enableDma = true,
    .multiblocks = false
  };

  if ((handle == NULL) || (data == NULL)) {
    return SDIODRV_ERROR_PARAM;
  }

  // Save the current information
  handle->cmd_idx = cmd53.index;
  handle->op = op;
  handle->dataAddr = data;
  handle->init.instance->SDMASYSADDR = (uint32_t)data;

  if (sdiodrv_callbacks.comEvtCb != read_write_checking_callback) {
    handle->appComEvtCb = sdiodrv_callbacks.comEvtCb;

    // Overload temporarily the application callback to perform
    // additional treatments before calling the real callback.
    sdiodrv_callbacks.comEvtCb = read_write_checking_callback;
  }

  sdiodrv_handle = handle;

  if (op == SDIODRV_IO_OP_WRITE) {
    cmd53.args |= (uint32_t)(!!op << CMD53_RW_FLAG_SHIFT);
    cmd53.dataDirection = SDIO_DATA_DIRECTION_HOST_TO_CARD;
  }

  // Send the command
  SDIO_TxCmd(handle->init.instance, &cmd53);
  return SDIODRV_ERROR_NONE;
}

/***************************************************************************//**
 * @brief
 *   Execute an extended read or write operation using the block mode.
 *
 * @note
 *   This is NOT a blocking function, an interrupt is generated at the transfer
 *   completion (or error) calling back the function registered by the application.
 *
 * @param[in] handle
 *   A pointer to the SDIODRV Handle.
 *
 * @param[in] op
 *   Operation indicator.
 *
 * @param[in] function
 *   Function addressed by the operation.
 *
 * @param[in] address
 *   Register address in the selected function.
 *
 * @param[in] blocksCnt
 *   Number of blocks to read or write.
 *
 * @param[in/out] data
 *   Data to write to the device or address to store the data
 *   read from the device.
 *
 * @return
 *   @ref SDIODRV_ERROR_NONE on success.
 *   On failure, an appropriate error is returned.
 ******************************************************************************/
uint32_t SDIODRV_IOReadWriteExtendedBlocks (SDIODRV_Handle_t *handle,
                                            SDIODRV_IO_Operation_t op,
                                            uint8_t function,
                                            uint32_t address,
                                            uint16_t blocksCnt,
                                            uint16_t *data)
{
  SDIO_Cmd_t cmd53 = {
    .index = 53,
    .args = (uint32_t)(  ((function << CMD53_FNCT_NB_SHIFT) & CMD53_FNCT_NB_MASK)
                       | (1 << CMD53_BLOCK_MODE_SHIFT)
                       | (1 << CMD53_OP_CODE_SHIFT)
                       | ((address << CMD53_REG_ADDR_SHIFT) & CMD53_REG_ADDR_MASK))
                       | (blocksCnt & CMD53_BYTE_BLOCK_CNT_MASK),
    .dataDirection = SDIO_DATA_DIRECTION_CARD_TO_HOST,
    .respType = SDIO_RESP_TYPE_RESP48,
    .checkCrc = true,
    .checkIndex = true,
    .enableDma = true,
    .multiblocks = true
  };

  if ((handle == NULL) || (data == NULL)) {
    return SDIODRV_ERROR_PARAM;
  }

  // Update the handle
  handle->cmd_idx = cmd53.index;
  handle->op = op;
  handle->dataAddr = NULL;
  handle->init.instance->SDMASYSADDR = (uint32_t)data;

  if (sdiodrv_callbacks.comEvtCb != read_write_checking_callback) {
    handle->appComEvtCb = sdiodrv_callbacks.comEvtCb;

    // Overload temporarily the application callback to perform
    // additional treatments before calling the real callback.
    sdiodrv_callbacks.comEvtCb = read_write_checking_callback;
  }

  sdiodrv_handle = handle;

  if (op == SDIODRV_IO_OP_WRITE) {
    cmd53.args |= (uint32_t)(!!op << CMD53_RW_FLAG_SHIFT);
    cmd53.dataDirection = SDIO_DATA_DIRECTION_HOST_TO_CARD;
  }

  // Send the command
  SDIO_TxCmd(handle->init.instance, &cmd53);
  return SDIODRV_ERROR_NONE;
}

/***************************************************************************//**
 * @brief
 *   Abort a command or transfer ongoing.
 *
 * @param[in] handle
 *   A pointer to the SDIODRV Handle.
 *
 * @param[in] function_to_abort
 *   Function to abort.
 *
 * @return
 *   @ref SDIODRV_ERROR_NONE on success.
 *   On failure, an appropriate error is returned.
 ******************************************************************************/
uint32_t SDIODRV_Abort (SDIODRV_Handle_t *handle, uint8_t function_to_abort)
{
  uint32_t res;

  if (handle == NULL) {
    return SDIODRV_ERROR_PARAM;
  }

  // Start by resetting the CMD and DAT lines
  handle->init.instance->CLOCKCTRL |= ( SDIO_CLOCKCTRL_SFTRSTDAT
                                      | SDIO_CLOCKCTRL_SFTRSTCMD);
  while ((handle->init.instance->CLOCKCTRL
         & (SDIO_CLOCKCTRL_SFTRSTDAT | SDIO_CLOCKCTRL_SFTRSTCMD)) != 0);

  res = SDIODRV_IOReadWriteDirect(handle,
                                  SDIODRV_IO_OP_WRITE,
                                  0,
                                  0x06 /* I/O Function Abort */,
                                  &function_to_abort);

  return res;
}

/***************************************************************************//**
 * @brief
 *   Enable or disable the High Speed mode on the host side.
 *
 * @param[in] handle
 *   A pointer to the SDIODRV Handle.
 *
 * @param[in] state
 *   Enable or disable state indicator.
 *
 * @return
 *   @ref SDIODRV_ERROR_NONE on success.
 *   On failure, an appropriate error is returned.
 ******************************************************************************/
uint32_t SDIODRV_EnableHighSpeed (SDIODRV_Handle_t *handle, bool state)
{
  if (handle == NULL) {
    return SDIODRV_ERROR_PARAM;
  }

  // Enable HS mode in the host side
  if (state == true) {
    handle->init.instance->HOSTCTRL1 |= SDIO_HOSTCTRL1_HSEN;
  } else {
    handle->init.instance->HOSTCTRL1 &= ~SDIO_HOSTCTRL1_HSEN;
  }

  return SDIODRV_ERROR_NONE;
}

/***************************************************************************//**
 * @brief
 *   Enable or disable SDIO interrupts on the host side.
 *
 * @param[in] handle
 *   A pointer to the SDIODRV Handle.
 *
 * @param[in] interrupts
 *   Interrupt mask.
 *
 * @param[in] state
 *   Enable or disable state indicator.
 *
 * @return
 *   @ref SDIODRV_ERROR_NONE on success.
 *   On failure, an appropriate error is returned.
 ******************************************************************************/
uint32_t SDIODRV_EnableInterrupts (SDIODRV_Handle_t *handle,
                                   uint32_t interrupts,
                                   bool state)
{
  uint32_t ints = interrupts & _SDIO_IFENC_MASK;

  if (handle == NULL) {
    return SDIODRV_ERROR_PARAM;
  }

  if (state == true) {
    handle->init.instance->IEN |= ints;
    handle->init.instance->IFENC |= ints;
  } else {
    handle->init.instance->IEN &= ~(ints);
    handle->init.instance->IFENC &= ~(ints);
  }

  return SDIODRV_ERROR_NONE;
}

/***************************************************************************//**
 * @brief
 *   Enable or disable SDIO interrupts on the host side.
 *
 * @param[in] callbacks
 *   A pointer to a structure containing the application callbacks.
 ******************************************************************************/
void SDIODRV_RegisterCallbacks (SDIODRV_Callbacks_t *callbacks)
{
  if (callbacks != NULL) {
    sdiodrv_callbacks.comEvtCb              = callbacks->comEvtCb;
    sdiodrv_callbacks.cardInsertionCb       = callbacks->cardInsertionCb;
    sdiodrv_callbacks.cardRemovalCb         = callbacks->cardRemovalCb;
    sdiodrv_callbacks.cardInterruptCb       = callbacks->cardInterruptCb;
    sdiodrv_callbacks.bootAckRcvCb          = callbacks->bootAckRcvCb;
    sdiodrv_callbacks.bootTerminateCb       = callbacks->bootTerminateCb;
  }
}

/** SDIO Handler. */
void SDIO_IRQHandler (void)
{
  uint32_t ifcr_value;
  uint32_t pending;
  CORE_DECLARE_IRQ_STATE;


  CORE_ENTER_ATOMIC();
  // Get all enabled interrupts
  ifcr_value  = SDIO->IFCR;
  // Clear the interrupts
  SDIO->IFCR = ifcr_value;
  CORE_EXIT_ATOMIC();

  pending = ifcr_value & SDIO->IEN;

  if ((pending & SDIO_IFCR_ERRINT) == SDIO_IFCR_ERRINT) {
    if (sdiodrv_callbacks.comEvtCb) {
      sdiodrv_callbacks.comEvtCb(SDIODRV_EVENT_COM_ERROR,
                                 SDIODRV_ERROR_CMDRESP);
    }
  }

  if ((pending & SDIO_IFCR_CMDCOM) == SDIO_IFCR_CMDCOM) {
    if (sdiodrv_callbacks.comEvtCb) {
      sdiodrv_callbacks.comEvtCb(SDIODRV_EVENT_CMD_COMPLETE,
                                 SDIODRV_ERROR_NONE);
    }
  }

  if ((pending & SDIO_IFCR_TRANCOM) == SDIO_IFCR_TRANCOM) {
    if (sdiodrv_callbacks.comEvtCb) {
      sdiodrv_callbacks.comEvtCb(SDIODRV_EVENT_TRANS_COMPLETE,
                                 SDIODRV_ERROR_NONE);
    }
  }

  if ((pending & SDIO_IFCR_DMAINT) == SDIO_IFCR_DMAINT) {
    SDIO->SDMASYSADDR = SDIO->SDMASYSADDR;
  }

  if ((pending & SDIO_IFCR_CARDINS) == SDIO_IFCR_CARDINS) {
    if (sdiodrv_callbacks.cardInsertionCb) {
      sdiodrv_callbacks.cardInsertionCb();
    }
  }

  if ((pending & SDIO_IFCR_CARDRM) == SDIO_IFCR_CARDRM) {
    if (sdiodrv_callbacks.cardRemovalCb) {
      sdiodrv_callbacks.cardRemovalCb();
    }
  }

  if ((pending & SDIO_IFCR_CARDINT) == SDIO_IFCR_CARDINT) {
    // Disable the interrupt to prevent it firing in loop
    // Let the responsibility to the application to re-enable it
    SDIO->IFENC &= ~SDIO_IFENC_CARDINTEN;
    SDIO->IFCR &= ~SDIO_IFCR_CARDINT;
    if (sdiodrv_callbacks.cardInterruptCb) {
      sdiodrv_callbacks.cardInterruptCb();
    }
  }

  if ((pending & SDIO_IFCR_BOOTACKRCV) == SDIO_IFCR_BOOTACKRCV) {
    if (sdiodrv_callbacks.bootAckRcvCb) {
      sdiodrv_callbacks.bootAckRcvCb();
    }
  }

  if ((pending & SDIO_IFCR_BOOTTERMINATE) == SDIO_IFCR_BOOTTERMINATE) {
    if (sdiodrv_callbacks.bootTerminateCb) {
      sdiodrv_callbacks.bootTerminateCb();
    }
  }

  if (sdiodrv_handle->init.yield_fn != NULL) {
    sdiodrv_handle->init.yield_fn();
  }
}

