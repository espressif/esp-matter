/***************************************************************************//**
 * @file
 * @brief SDIODRV API definition
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
#ifndef SDIODRV_H
#define SDIODRV_H

#include "em_device.h"

#if defined(SDIO_PRESENT)
#include "sdio.h"
#else
#error "SDIO unsupported."
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "em_cmu.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#define SDIODRV_ERROR_NONE                (0x00000000)
#define SDIODRV_ERROR_NOT_INIT            (0x00000001)
#define SDIODRV_ERROR_PARAM               (0x00000002)
#define SDIODRV_ERROR_CMDRESP             (0x00000004)
#define SDIODRV_ERROR_RESP_OUT_OF_RANGE   (0x00000008)
#define SDIODRV_ERROR_RESP_FUNC_NB        (0x00000010)
#define SDIODRV_ERROR_RESP_UNKNOWN        (0x00000020)
#define SDIODRV_ERROR_RESP_WRONG_STATE    (0x00000040)
#define SDIODRV_ERROR_RESP_ILLEGAL_CMD    (0x00000080)
#define SDIODRV_ERROR_RESP_CRC            (0x00000100)
#define SDIODRV_ERROR_RCA                 (0x00000200)

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

  typedef enum
  {
      SL_GPIO_PORT_A = 0,
      SL_GPIO_PORT_B = 1,
      SL_GPIO_PORT_C = 2,
      SL_GPIO_PORT_D = 3,
      SL_GPIO_PORT_E = 4,
      SL_GPIO_PORT_F = 5,
      SL_GPIO_PORT_G = 6,
      SL_GPIO_PORT_H = 7,
      SL_GPIO_PORT_I = 8,
      SL_GPIO_PORT_J = 9,
      SL_GPIO_PORT_K = 10,
  } sl_gpio_port_id_t;


/** IO operation selection. */
typedef enum SDIODRV_IO_Operation_s {
  SDIODRV_IO_OP_READ = 0,
  SDIODRV_IO_OP_WRITE
} SDIODRV_IO_Operation_t;

/** Driver event types. */
typedef enum SDIODRV_Event_s {
  SDIODRV_EVENT_COM_ERROR       = 0x0001,
  SDIODRV_EVENT_CMD_COMPLETE    = 0x0002,
  SDIODRV_EVENT_TRANS_COMPLETE  = 0x0004,
  SDIODRV_EVENT_CARD_INSERTION  = 0x0008,
  SDIODRV_EVENT_CARD_REMOVAL    = 0x0010,
  SDIODRV_EVENT_CARD_INTERRUPT  = 0x0020,
  SDIODRV_EVENT_BOOT_ACK_RCV    = 0x0040,
  SDIODRV_EVENT_BOOT_TERM       = 0x0080,
} SDIODRV_Event_t;

/*******************************************************************************
 ******************************   CALLBACKS   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   SDIODRV Communication callback function.
 *
 * @details
 *   The callback is called when a command/transfer completes or an error occurs.
 *
 * @param[in] evt
 *   Event type.
 *
 * @param[in] error
 *   Additional information in case of an error.
 ******************************************************************************/
typedef void (*SDIODRV_Com_Callback_t)(SDIODRV_Event_t evt, uint32_t error);

/***************************************************************************//**
 * @brief
 *   SDIODRV Event callback function.
 *
 * @details
 *   The callback is called when the associated event occurs.
 ******************************************************************************/
typedef void (*SDIODRV_Evt_Callback_t)(void);

/***************************************************************************//**
 * @brief
 *   SDIODRV Yield function.
 *
 * @details
 *   Hook called at the handler's end allowing to yield (required by some OS
 *   e.g FreeRTOS).
 ******************************************************************************/
typedef void (*SDIODRV_Yield_t)(void);

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/

/**
 * SDIODRV initialization structure containing a number of SDIODRV
 * configuration options. This structure is passed to @ref SDIODRV_Init()
 * when initializing a SDIODRV instance.
 */
typedef struct SDIODRV_Init_s {
  SDIO_TypeDef         *instance;
  uint32_t              freq;
  uint8_t               portLocationClk;
  uint8_t               portLocationCmd;
  uint8_t               portLocationDat;
  uint8_t               portLocationCd;
  uint8_t               portLocationWp;
  CMU_Select_TypeDef    clockSource;
  SDIO_Transfer_Width_t transferWidth;
  SDIODRV_Yield_t       yield_fn;
} SDIODRV_Init_t;

/**
 * An SDIO driver instance handle data structure.
 * The handle is allocated by the application using the SDIODRV.
 * The application is neither supposed to write or read
 * the contents of the handle.
 */
typedef struct SDIODRV_Handle_s {
  SDIODRV_Init_t          init;
  // Internal function to call before the application callback
  SDIODRV_Com_Callback_t  appComEvtCb;
  // Current User Data address
  uint8_t                *dataAddr;
  // Current operation
  SDIODRV_IO_Operation_t  op;
  uint8_t                 cmd_idx;
} SDIODRV_Handle_t;


/** SDIODRV callback functions. */
typedef struct SDIODRV_Callbacks_s {
  SDIODRV_Com_Callback_t comEvtCb;
  SDIODRV_Evt_Callback_t cardInsertionCb;
  SDIODRV_Evt_Callback_t cardRemovalCb;
  SDIODRV_Evt_Callback_t cardInterruptCb;
  SDIODRV_Evt_Callback_t bootAckRcvCb;
  SDIODRV_Evt_Callback_t bootTerminateCb;
} SDIODRV_Callbacks_t;

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

uint32_t SDIODRV_Init(SDIODRV_Handle_t *handle, SDIODRV_Init_t *init);
uint32_t SDIODRV_DeInit(SDIODRV_Handle_t *handle);
uint32_t SDIODRV_Enable(SDIODRV_Handle_t *handle, bool state);
uint32_t SDIODRV_DeviceInitAndIdent(SDIODRV_Handle_t *handle, uint16_t *rca);
uint32_t SDIODRV_SelectCard(SDIODRV_Handle_t *handle, uint16_t rca);
uint32_t SDIODRV_IOReadWriteDirect(SDIODRV_Handle_t *handle,
                                   SDIODRV_IO_Operation_t op,
                                   uint8_t function,
                                   uint32_t address,
                                   uint8_t *data);
uint32_t SDIODRV_IOReadWriteDirect_rsi(SDIODRV_Handle_t *handle,
                                   SDIODRV_IO_Operation_t op,
                                   uint8_t function,
                                   uint32_t address,
                                   uint8_t *data);

uint32_t SDIODRV_IOReadWriteExtendedBytes(SDIODRV_Handle_t *handle,
                                          SDIODRV_IO_Operation_t op,
                                          uint8_t function,
                                          uint32_t address,
                                          uint16_t bytesCnt,
                                          uint8_t *data);
uint32_t SDIODRV_IOReadWriteExtendedBlocks(SDIODRV_Handle_t *handle,
                                           SDIODRV_IO_Operation_t op,
                                           uint8_t function,
                                           uint32_t address,
                                           uint16_t blocksCnt,
                                           uint16_t *data);
uint32_t SDIODRV_Abort(SDIODRV_Handle_t *handle, uint8_t function_to_abort);
uint32_t SDIODRV_EnableHighSpeed(SDIODRV_Handle_t *handle, bool state);
uint32_t SDIODRV_EnableInterrupts(SDIODRV_Handle_t *handle,
                                  uint32_t interrupts,
                                  bool state);
void SDIODRV_RegisterCallbacks(SDIODRV_Callbacks_t *callbacks);

#ifdef __cplusplus
}
#endif

#endif //SDIODRV_H
