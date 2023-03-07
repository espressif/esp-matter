/***************************************************************************//**
 * @file
 * @brief Driver for stereo I2S microphones
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

#ifndef MIC_H
#define MIC_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "mic_config.h"
#include "dmadrv.h"

/***************************************************************************//**
 * @addtogroup Mic
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @defgroup Mic_Config_Settings MEMS Microphone Configuration Settings
 * @{
 * @brief MEMS microphone configuration setting macro definitions
 ******************************************************************************/

#define DEFAULT_CALLBACK      0
#define CALLBACK_PORT         gpioPortE
#define UNUSED_PARAM(x)       (void)(x)

/** @} {end defgroup Mic_Config_Settings} */

/***************************************************************************//**
 * @defgroup Mic_Functions MEMS Microphone Functions
 * @{
 * @brief MEMS microphone driver and support functions
 ******************************************************************************/

// -----------------------------------------------------------------------------
// typedef

typedef struct _MIC_Context{
  LDMA_Descriptor_t *dmaDesc;
  uint16_t          *sampleBufOne;
  uint16_t          *sampleBufTwo;
  uint16_t           callBackCount;
  bool               firstBufferReady;
  bool               seconBufferReady;
  float              soundLevel;
  uint8_t            gpioPin;
} MIC_Context;

// -----------------------------------------------------------------------------
// Global Function Prototypes

uint32_t MIC_init(DMADRV_Callback_t userDefinedCallBack,
                  uint32_t fs,
                  uint16_t *leftSampleBufOne,
                  uint16_t *leftSampleBufTwo,
                  uint16_t *rightSampleBufOne,
                  uint16_t *rightSampleBufTwo,
                  size_t len);

void      MIC_deInit         (void);
void      MIC_start          (void);
bool      MIC_isBusy         (bool leftMic, bool bufOne);
float     MIC_getMean        (bool leftChannel, bool bufOne);
float     MIC_getSoundLevel  (float *var, bool leftChannel, bool bufOne);

/** @} {end defgroup Mic_Functions}*/

/** @} {end addtogroup Mic} */

#endif // MIC_H
