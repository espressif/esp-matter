/***************************************************************************//**
 * @file
 * @brief SDIO Peripheral API
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
#ifndef SDIO_H
#define SDIO_H

#include "em_device.h"

#if defined(SDIO_PRESENT)

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

/**
 * SDIO Transfer Width selection corresponding to the number
 * of DAT lines connected between the host and the device.
 */
typedef enum SDIO_Transfer_Width_s {
  SDIO_TRANSFER_WIDTH_1BIT = 0,
  SDIO_TRANSFER_WIDTH_4BIT,
  SDIO_TRANSFER_WIDTH_8BIT,
} SDIO_Transfer_Width_t;

/** Data direction selection. */
typedef enum SDIO_DataDirection_s {
  SDIO_DATA_DIRECTION_NODATA = 0,
  SDIO_DATA_DIRECTION_CARD_TO_HOST,
  SDIO_DATA_DIRECTION_HOST_TO_CARD,
} SDIO_DataDirection_t;

/** Expected response selection. */
typedef enum SDIO_Resp_Type_s {
  SDIO_RESP_TYPE_NORESP = 0,
  SDIO_RESP_TYPE_RESP136,
  SDIO_RESP_TYPE_RESP48,
  SDIO_RESP_TYPE_BUSYAFTRESP   //48bits
} SDIO_Reps_Type_t;

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/

/** SDIO initialization structure. */
typedef struct SDIO_Init_TypeDef_s {
  /**
   * SDIO reference clock used to compute the prescaler to set to
   * get a frequency the closest to the desired one.
   */
  uint32_t refFreq;
  /**
   * Desired frequency, the actual frequency generated may not have
   * exactly the same value.
   */
  uint32_t desiredFreq;
  /** Transfer width. */
  SDIO_Transfer_Width_t transferWidth;
} SDIO_Init_TypeDef_t;

/** SDIO command structure. */
typedef struct SDIO_Cmd_s {
  /** Command argument. */
  uint32_t args;
  /** Data direction. */
  SDIO_DataDirection_t dataDirection;
  /** Response expected. */
  SDIO_Reps_Type_t respType;
  /** Command index. */
  uint8_t index;
  /** Index checking indicator. */
  bool checkIndex;
  /** CRC checking indicator. */
  bool checkCrc;
  /** DMA enabling indicator. */
  bool enableDma;
  /** Multiblocks usage indicator. */
  bool multiblocks;
} SDIO_Cmd_t;

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

void SDIO_Init(SDIO_TypeDef *sdio, const SDIO_Init_TypeDef_t *init);
void SDIO_DeInit(SDIO_TypeDef *sdio);
void SDIO_SetClockFrequency(SDIO_TypeDef *sdio,
                            uint32_t refFreq,
                            uint32_t desiredFreq);
void SDIO_TxCmd(SDIO_TypeDef *sdio, SDIO_Cmd_t *cmd);
void SDIO_TxCmdB(SDIO_TypeDef *sdio, SDIO_Cmd_t *cmd);
uint32_t SDIO_GetCmdStatus(SDIO_TypeDef *sdio);
uint32_t SDIO_GetResp(SDIO_TypeDef *sdio, uint8_t respIndex);
void SDIO_ConfigureTransfer(SDIO_TypeDef *sdio,
                            uint16_t itemSize,
                            uint16_t itemCount);

#ifdef __cplusplus
}
#endif

#endif //defined( SDIO_PRESENT )
#endif //SDIO_H
