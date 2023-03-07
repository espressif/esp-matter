/***************************************************************************//**
 * @file
 * @brief Driver for the SPV1840LR5H-B MEMS Microphone
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

#include "em_device.h"
#include "em_cmu.h"
#include "em_prs.h"
#include "em_usart.h"
#include "em_ldma.h"
#include "dmadrv.h"

#include "thunderboard/util.h"
#include "thunderboard/board.h"
#include "thunderboard/mic.h"

#include <math.h>

/**************************************************************************//**
* @addtogroup TBSense_BSP
* @{
******************************************************************************/

/***************************************************************************//**
 * @defgroup Mic_i2s MIC - Microphone Driver (I2S)
 * @{
 * @brief Driver for the Invensense ICS-43434 MEMS Microphone
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

static unsigned int  dmaChannelLeft;       /**< The channel Id assigned by DMADRV                         */
static unsigned int  dmaChannelRight;      /**< The dummy channel Id assigned by DMADRV                   */
static int16_t      *sampleBuffer;         /**< Buffer used to store the microphone samples               */
static size_t        sampleBufferLen;      /**< The length of the sample buffer                           */
static size_t        sampleCount;          /**< The number of samples to collect                          */
static size_t        sampleIndex;
static volatile bool sampleBufferReady;    /**< Flag to show that the buffer is filled with new samples   */

static int16_t       sampleLeft;
static int16_t       sampleRight;

static LDMA_TransferCfg_t dmaTransferLeft =
  LDMA_TRANSFER_CFG_PERIPHERAL(MIC_DMA_LEFT_SIGNAL);

static LDMA_TransferCfg_t dmaTransferRight =
  LDMA_TRANSFER_CFG_PERIPHERAL(MIC_DMA_RIGHT_SIGNAL);

static LDMA_Descriptor_t dmaDescLeft[2] = {
  LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&MIC_USART->RXDOUBLE, &sampleLeft, 2047, 1),
  LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&MIC_USART->RXDOUBLE, &sampleLeft, 2047, -1)
};

static LDMA_Descriptor_t dmaDescRight[2] = {
  LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&MIC_USART->RXDOUBLE, &sampleRight, 2047, 1),
  LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&MIC_USART->RXDOUBLE, &sampleRight, 2047, -1)
};

/** @endcond */

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

static bool dmaCompleteCallback(unsigned int channel, unsigned int sequenceNo, void *userParam);

/** @endcond */

/***************************************************************************//**
 * @brief
 *    Initializes MEMS microphone and sets up the DMA, ADC and clocking
 *
 * @param[in] fs
 *    The desired sample rate in Hz
 *
 * @param[in] buffer
 *    Pointer to the sample buffer to store the ADC data
 *
 * @param[in] len
 *    The size of the sample buffer
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t MIC_init(uint32_t fs, uint16_t *buffer, size_t len)
{
  uint32_t status;
  USART_InitI2s_TypeDef usartInit = USART_INITI2S_DEFAULT;

  /* Enable microphone circuit and wait for it to settle properly */
  status = BOARD_micEnable(true);
  if ( status != BOARD_OK ) {
    return status;
  }

  /* Enable clocks */
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(MIC_USART_CLK, true);

  /* Setup GPIO pins */
  GPIO_PinModeSet(MIC_PORT_DATA, MIC_PIN_DATA, gpioModeInput, 0);
  GPIO_PinModeSet(MIC_PORT_CLK, MIC_PIN_CLK, gpioModePushPull, 0);
  GPIO_PinModeSet(MIC_PORT_WS, MIC_PIN_WS, gpioModePushPull, 0);

  /* Setup USART in I2S mode to get data from microphone */
  usartInit.sync.enable   = usartEnable;
  usartInit.sync.baudrate = fs * 64;
  usartInit.sync.autoTx   = true;
  usartInit.format        = usartI2sFormatW32D16;
  usartInit.dmaSplit      = true;
  USART_InitI2s(MIC_USART, &usartInit);

#if defined(_SILICON_LABS_32B_SERIES_2)
  GPIO->USARTROUTE->ROUTEEN = GPIO_USART_ROUTEEN_RXPEN | GPIO_USART_ROUTEEN_CLKPEN | GPIO_USART_ROUTEEN_CSPEN;
  GPIO->USARTROUTE->RXROUTE = (MIC_PORT_DATA << _GPIO_USART_RXROUTE_PORT_SHIFT) | (MIC_PIN_DATA << _GPIO_USART_RXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE->RXROUTE = (MIC_PORT_CLK << _GPIO_USART_CLKROUTE_PORT_SHIFT) | (MIC_PIN_CLK << _GPIO_USART_CLKROUTE_PIN_SHIFT);
  GPIO->USARTROUTE->RXROUTE = (MIC_PORT_WS << _GPIO_USART_CSROUTE_PORT_SHIFT) | (MIC_PIN_WS << _GPIO_USART_CSROUTE_PIN_SHIFT);
#else
  MIC_USART->ROUTELOC0 = (MIC_USART_LOC_DATA | MIC_USART_LOC_CLK | MIC_USART_LOC_WS);
  MIC_USART->ROUTEPEN  = (USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_CLKPEN | USART_ROUTEPEN_CSPEN);
#endif

  /* Setup DMA driver to move samples from USART to memory */
  DMADRV_Init();
  status = DMADRV_AllocateChannel(&dmaChannelLeft, NULL);
  if ( status != ECODE_EMDRV_DMADRV_OK ) {
    return status;
  }
  status = DMADRV_AllocateChannel(&dmaChannelRight, NULL);
  if ( status != ECODE_EMDRV_DMADRV_OK ) {
    return status;
  }

  /* Start DMA channel for I2S right (dummy) data */
  dmaDescRight[0].xfer.size   = ldmaCtrlSizeHalf;
  dmaDescRight[0].xfer.dstInc = ldmaCtrlDstIncNone;
  dmaDescRight[1].xfer.size   = ldmaCtrlSizeHalf;
  dmaDescRight[1].xfer.dstInc = ldmaCtrlDstIncNone;

  DMADRV_LdmaStartTransfer(dmaChannelRight,
                           &dmaTransferRight,
                           &dmaDescRight[0],
                           NULL,
                           NULL);

  /* Start DMA channel for I2S left (microphone) data */
  dmaDescLeft[0].xfer.size   = ldmaCtrlSizeHalf;
  dmaDescLeft[0].xfer.dstInc = ldmaCtrlDstIncNone;
  dmaDescLeft[1].xfer.size   = ldmaCtrlSizeHalf;
  dmaDescLeft[1].xfer.dstInc = ldmaCtrlDstIncNone;

  DMADRV_LdmaStartTransfer(dmaChannelLeft,
                           &dmaTransferLeft,
                           &dmaDescLeft[0],
                           dmaCompleteCallback,
                           NULL);

  sampleBufferReady = false;
  sampleBuffer = (int16_t *) buffer;
  sampleBufferLen = len;

  return status;
}

/***************************************************************************//**
 * @brief
 *    Powers down the MEMS microphone stops the ADC and frees up the DMA channel
 *
 * @return
 *    None
 ******************************************************************************/
void MIC_deInit(void)
{
  /* Stop sampling */
  DMADRV_StopTransfer(dmaChannelLeft);
  DMADRV_StopTransfer(dmaChannelRight);

  /* Reset USART peripheral and disable IO pins */
  USART_Reset(MIC_USART);
  MIC_USART->I2SCTRL = 0;

  GPIO_PinModeSet(MIC_PORT_CLK, MIC_PIN_CLK, gpioModeDisabled, 0);
  GPIO_PinModeSet(MIC_PORT_DATA, MIC_PIN_DATA, gpioModeDisabled, 0);
  GPIO_PinModeSet(MIC_PORT_WS, MIC_PIN_WS, gpioModeDisabled, 0);

  /* Power down microphone */
  BOARD_micEnable(false);

  /* Free resources */
  DMADRV_FreeChannel(dmaChannelLeft);
  DMADRV_FreeChannel(dmaChannelRight);

  return;
}

/***************************************************************************//**
 * @brief
 *    Starts taking samples using DMA from the microphone
 *
 * @param[in] nSamples
 *    The number of the samples to take
 *
 * @return
 *    None
 ******************************************************************************/
void MIC_start(uint32_t nSamples)
{
  if ( nSamples > sampleBufferLen ) {
    sampleCount = sampleBufferLen;
  } else {
    sampleCount = nSamples;
  }

  sampleIndex = 0;
  sampleBufferReady = false;
  return;
}

/***************************************************************************//**
 * @brief
 *    Gets the sample buffer
 *
 * @return
 *    Returns a pointer to the sample buffer
 ******************************************************************************/
uint16_t *MIC_getSampleBuffer(void)
{
  return (uint16_t *) sampleBuffer;
}

/***************************************************************************//**
 * @brief
 *    Gets the number of the samples to take
 *
 * @return
 *    Returns the number of the samples to take
 ******************************************************************************/
size_t MIC_getSampleCount(void)
{
  return sampleCount;
}

/***************************************************************************//**
 * @brief
 *    Calculates the average value of the samples in the buffer
 *
 * @return
 *    The mean value of the samples in the buffer
 ******************************************************************************/
float MIC_getMean(void)
{
  size_t i;
  float mean;

  mean = 0.0f;

  for ( i = 0; i < sampleCount; i++ ) {
    mean += (float) sampleBuffer[i];
  }
  mean = mean / (float) sampleCount;

  return mean;
}

/***************************************************************************//**
 * @brief
 *    Calculates the sound level
 *
 * @param[out] var
 *    The variance of the samples
 *
 * @return
 *    The sound level in dB
 ******************************************************************************/
float MIC_getSoundLevel(float *var)
{
  float sample;
  float mean;
  float power;
  float soundLevel;
  size_t i;

  soundLevel = 0;
  power = 0;

  if ( sampleBufferReady ) {
    /* Estimate mean */
    mean = MIC_getMean();

    /* Estimate variance */
    power = 0;
    for ( i = 0; i < sampleCount; i++ ) {
      sample = ( (float) sampleBuffer[i] - mean) / 32767.5;
      power += sample * sample;
    }
    power = power / (float) sampleCount;

    /* Convert to decibel*/
    soundLevel = 10.0f * log10f(power);
  }

  if ( var != NULL ) {
    *var = power;
  }

  return soundLevel;
}

/***************************************************************************//**
 * @brief
 *    Checks if the microphone is in use
 *
 * @return
 *    Returns true if the DMA is busy taking microphone samples
 ******************************************************************************/
bool MIC_isBusy(void)
{
  return !sampleBufferReady;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *    Called when the DMA complete interrupt fired
 *
 * @param[in] channel
 *    DMA channel
 *
 * @param[in] sequenceNo
 *    Sequence number
 *
 * @param[in] userParam
 *    User parameters
 *
 * @return
 *    Returns false to stop transfers
 ******************************************************************************/
static bool dmaCompleteCallback(unsigned int channel, unsigned int sequenceNo, void *userParam)
{
  LDMA_Descriptor_t *nextDesc;

  if ( !sampleBufferReady ) {
    /* Setup next descriptor */
    if ( sequenceNo & 0x01 ) {
      nextDesc = &dmaDescLeft[0];
    } else {
      nextDesc = &dmaDescLeft[1];
    }

    if ( sampleIndex < sampleCount ) {
      nextDesc->xfer.dstInc  = ldmaCtrlDstIncOne;
      nextDesc->xfer.dstAddr = (uint32_t) &sampleBuffer[sampleIndex];
      if ( (sampleCount - sampleIndex) > 2048 ) {
        nextDesc->xfer.xferCnt = 2047;
        sampleIndex += 2048;
      } else {
        nextDesc->xfer.xferCnt = (sampleCount - sampleIndex) - 1;
        sampleIndex += (sampleCount - sampleIndex);
      }
      sampleBufferReady = false;
    } else {
      nextDesc->xfer.dstInc = ldmaCtrlDstIncNone;
      nextDesc->xfer.xferCnt = 2047;
      nextDesc->xfer.dstAddr = (uint32_t) &sampleLeft;

      /* Sample buffer is complete if both descriptors have returned back to default */
      if ( (dmaDescLeft[0].xfer.dstInc == ldmaCtrlDstIncNone)
           && (dmaDescLeft[1].xfer.dstInc == ldmaCtrlDstIncNone) ) {
        sampleBufferReady = true;
      }
    }
  }

  return true;
}

/** @endcond */

/** @} {end defgroup Mic_i2s} */
/** @} {end addtogroup TBSense_BSP} */
