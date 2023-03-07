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
#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "em_ldma.h"
#include "dmadrv.h"
#include "mic_i2s.h"

#include <math.h>

// -----------------------------------------------------------------------------
// Defines

#define MAX_XFER_COUNT 2048
#define LEFT_MIC_GPIO  10
#define RIGHT_MIC_GPIO 11

/***************************************************************************//**
 * @defgroup Mic MIC - Microphone Driver
 * @{
 * @brief Driver for the Knowles SPV1840LR5H-B MEMS Microphone
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @defgroup Mic_Locals MEMS Microphone Local Variables
 * @{
 * @brief MEMS microphone local variables
 ******************************************************************************/

static unsigned int  dmaChannelLeft; // The channel Id assigned by DMADRV
static unsigned int  dmaChannelRight; // The channel Id assigned by DMADRV
static size_t        sampleBufferLen; // The length of the sample buffer
static uint8_t       nrDescNeeded;

static DMADRV_Callback_t dmaCallBack;

static int16_t       sampleLeft;
static int16_t       sampleRight;

static LDMA_TransferCfg_t dmaTransferLeft =
  LDMA_TRANSFER_CFG_PERIPHERAL(MIC_DMA_LEFT_SIGNAL);

static LDMA_TransferCfg_t dmaTransferRight =
  LDMA_TRANSFER_CFG_PERIPHERAL(MIC_DMA_RIGHT_SIGNAL);

static LDMA_Descriptor_t dmaDescLeft[2] = {
  LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&MIC_USART->RXDOUBLE,
                                   &sampleLeft,
                                   MAX_XFER_COUNT,
                                   1),
  LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&MIC_USART->RXDOUBLE,
                                   &sampleLeft,
                                   MAX_XFER_COUNT,
                                   -1)
};

static LDMA_Descriptor_t dmaDescRight[2] = {
  LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&MIC_USART->RXDOUBLE,
                                   &sampleRight,
                                   MAX_XFER_COUNT,
                                   1),
  LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&MIC_USART->RXDOUBLE,
                                   &sampleRight,
                                   MAX_XFER_COUNT,
                                   -1)
};

static MIC_Context leftContext;
static MIC_Context rightContext;

/** @} {end defgroup Mic_Locals} */

/** @endcond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @defgroup Mic_Functions MEMS Microphone Functions
 * @{
 * @brief MEMS microphone driver and support functions
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

static bool dmaCompleteCallback(unsigned int channel, unsigned int sequenceNo, void *userParam);

/** @endcond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *    Initializes MEMS microphone and sets up the DMA and clocking
 *
 * @param[in] userDefinedCallBack
 *    Function pointer to the DMA callback function
 *
 * @param[in] fs
 *    The desired sample rate in Hz
 *
 * @param[in] buffer
 *    Pointer to the sample buffer to store the data
 *
 * @param[in] len
 *    The size of the sample buffer
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t MIC_init(DMADRV_Callback_t userDefinedCallBack,
                  uint32_t fs,
                  uint16_t *leftSampleBufOne,
                  uint16_t *leftSampleBufTwo,
                  uint16_t *rightSampleBufOne,
                  uint16_t *rightSampleBufTwo,
                  size_t len)
{
  uint32_t status;
  USART_InitI2s_TypeDef usartInit = USART_INITI2S_DEFAULT;
  sampleBufferLen = len;
  nrDescNeeded = sampleBufferLen / MAX_XFER_COUNT;

  if (userDefinedCallBack == DEFAULT_CALLBACK) {
    dmaCallBack = dmaCompleteCallback;
  } else {
    dmaCallBack = userDefinedCallBack;
  }

  // Context init
  leftContext.sampleBufOne = leftSampleBufOne;
  leftContext.sampleBufTwo = leftSampleBufTwo;
  leftContext.dmaDesc = dmaDescLeft;
  leftContext.firstBufferReady = false;
  leftContext.seconBufferReady = true;
  leftContext.gpioPin = LEFT_MIC_GPIO;

  rightContext.sampleBufOne = rightSampleBufOne;
  rightContext.sampleBufTwo = rightSampleBufTwo;
  rightContext.dmaDesc = dmaDescRight;
  rightContext.firstBufferReady = false;
  rightContext.seconBufferReady = true;
  rightContext.gpioPin = RIGHT_MIC_GPIO;

  // Enable clocks
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(MIC_USART_CLK, true);

  // Setup GPIO pins
  GPIO_PinModeSet(MIC_PORT_DATA, MIC_PIN_DATA, gpioModeInput, 0);
  GPIO_PinModeSet(MIC_PORT_CLK, MIC_PIN_CLK, gpioModePushPull, 0);
  GPIO_PinModeSet(MIC_PORT_WS, MIC_PIN_WS, gpioModePushPull, 0);

  GPIO_PinModeSet(CALLBACK_PORT, leftContext.gpioPin, gpioModePushPull, 0);
  GPIO_PinModeSet(CALLBACK_PORT, rightContext.gpioPin, gpioModePushPull, 1);

  // Setup USART in I2S mode to get data from microphone
  usartInit.sync.enable   = usartEnable;
  usartInit.sync.baudrate = fs * 64;
  usartInit.sync.autoTx   = true;
  usartInit.format        = usartI2sFormatW32D16;
  usartInit.dmaSplit      = true;
  USART_InitI2s(MIC_USART, &usartInit);

  MIC_USART->ROUTELOC0 = (MIC_USART_LOC_DATA | MIC_USART_LOC_CLK | MIC_USART_LOC_WS);
  MIC_USART->ROUTEPEN  = (USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_CLKPEN | USART_ROUTEPEN_CSPEN);

  // Setup DMA driver to move samples from USART to memory
  DMADRV_Init();
  status = DMADRV_AllocateChannel(&dmaChannelLeft, NULL);
  if ( status != ECODE_EMDRV_DMADRV_OK ) {
    return status;
  }
  status = DMADRV_AllocateChannel(&dmaChannelRight, NULL);
  if ( status != ECODE_EMDRV_DMADRV_OK ) {
    return status;
  }

  return status;
}

/***************************************************************************//**
 * @brief
 *    Powers down the MEMS microphone frees up the DMA channel
 *
 * @return
 *    None
 ******************************************************************************/
void MIC_deInit(void)
{
  // Stop sampling
  DMADRV_StopTransfer(dmaChannelLeft);
  DMADRV_StopTransfer(dmaChannelRight);

  // Reset USART peripheral and disable IO pins
  USART_Reset(MIC_USART);
  MIC_USART->I2SCTRL = 0;

  GPIO_PinModeSet(MIC_PORT_CLK, MIC_PIN_CLK, gpioModeDisabled, 0);
  GPIO_PinModeSet(MIC_PORT_DATA, MIC_PIN_DATA, gpioModeDisabled, 0);
  GPIO_PinModeSet(MIC_PORT_WS, MIC_PIN_WS, gpioModeDisabled, 0);

  // Free resources
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
void MIC_start()
{
  // Setup left channel
  dmaDescLeft[0].xfer.dstInc = ldmaCtrlDstIncOne;
  dmaDescLeft[0].xfer.size   = ldmaCtrlSizeHalf;
  dmaDescLeft[1].xfer.dstInc = ldmaCtrlDstIncOne;
  dmaDescLeft[1].xfer.size   = ldmaCtrlSizeHalf;

  // Setup right channel
  dmaDescRight[0].xfer.dstInc = ldmaCtrlDstIncOne;
  dmaDescRight[0].xfer.size   = ldmaCtrlSizeHalf;
  dmaDescRight[1].xfer.dstInc = ldmaCtrlDstIncOne;
  dmaDescRight[1].xfer.size   = ldmaCtrlSizeHalf;

  if (nrDescNeeded == 0) {
    dmaDescLeft[0].xfer.xferCnt = sampleBufferLen - 1;
    dmaDescLeft[1].xfer.xferCnt = sampleBufferLen - 1;
    dmaDescLeft[0].xfer.dstAddr = (uint32_t) leftContext.sampleBufOne;
    dmaDescLeft[1].xfer.dstAddr = (uint32_t) leftContext.sampleBufTwo;
    dmaDescRight[0].xfer.xferCnt = sampleBufferLen - 1;
    dmaDescRight[1].xfer.xferCnt = sampleBufferLen - 1;
    dmaDescRight[0].xfer.dstAddr = (uint32_t) rightContext.sampleBufOne;
    dmaDescRight[1].xfer.dstAddr = (uint32_t) rightContext.sampleBufTwo;
  } else {
    dmaDescLeft[0].xfer.dstAddr = (uint32_t) leftContext.sampleBufOne;
    dmaDescLeft[1].xfer.dstAddr = (uint32_t) &leftContext.sampleBufOne[MAX_XFER_COUNT];
    dmaDescRight[0].xfer.dstAddr = (uint32_t) rightContext.sampleBufOne;
    dmaDescRight[1].xfer.dstAddr = (uint32_t) &rightContext.sampleBufOne[MAX_XFER_COUNT];
    if (nrDescNeeded == 1) {
      dmaDescLeft[0].xfer.xferCnt = MAX_XFER_COUNT - 1;
      dmaDescLeft[1].xfer.xferCnt = sampleBufferLen - MAX_XFER_COUNT - 1;
      dmaDescRight[0].xfer.xferCnt = MAX_XFER_COUNT - 1;
      dmaDescRight[1].xfer.xferCnt = sampleBufferLen - MAX_XFER_COUNT - 1;
    } else {
      dmaDescLeft[0].xfer.xferCnt = MAX_XFER_COUNT - 1;
      dmaDescLeft[1].xfer.xferCnt = MAX_XFER_COUNT - 1;
      dmaDescRight[0].xfer.xferCnt = MAX_XFER_COUNT - 1;
      dmaDescRight[1].xfer.xferCnt = MAX_XFER_COUNT - 1;
    }
  }

  DMADRV_LdmaStartTransfer(dmaChannelLeft,
                           &dmaTransferLeft,
                           &dmaDescLeft[0],
                           dmaCallBack,
                           (void *) &leftContext);

  DMADRV_LdmaStartTransfer(dmaChannelRight,
                           &dmaTransferRight,
                           &dmaDescRight[0],
                           dmaCallBack,
                           (void *) &rightContext);

  return;
}

/***************************************************************************//**
 * @brief
 *    Checks if the microphone is in use
 *
 * @return
 *    Returns true if the DMA is busy taking microphone samples
 ******************************************************************************/
bool MIC_isBusy(bool leftMic, bool bufOne)
{
  bool retValue;
  if (leftMic) {
    if (bufOne) {
      retValue = !leftContext.firstBufferReady;
    } else {
      retValue = !leftContext.seconBufferReady;
    }
  } else {
    if (bufOne) {
      retValue = !rightContext.firstBufferReady;
    } else {
      retValue = !rightContext.seconBufferReady;
    }
  }
  return retValue;
}
/***************************************************************************//**
 * @brief
 *    Calculates the average value of the samples in the buffer
 *
 * @return
 *    The mean value of the samples in the buffer
 ******************************************************************************/
float MIC_getMean(bool leftChannel, bool bufOne)
{
  size_t i;
  float mean;
  MIC_Context *context;

  if (leftChannel) {
    context = &leftContext;
  } else {
    context = &rightContext;
  }

  mean = 0.0f;

  for ( i = 0; i < sampleBufferLen; i++ ) {
    if (bufOne) {
      mean += (float) context->sampleBufOne[i];
    } else {
      mean += (float) context->sampleBufTwo[i];
    }
  }
  mean = mean / (float) sampleBufferLen;

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
 *    The sound level in dB, or 0 if no sample buffers are ready.
 ******************************************************************************/
float MIC_getSoundLevel(float *var, bool leftChannel, bool bufOne)
{
  float sample;
  float mean;
  float power;
  bool bufReady;
  size_t i;
  MIC_Context *context;

  if (leftChannel) {
    context = &leftContext;
  } else {
    context = &rightContext;
  }

  if (bufOne) {
    bufReady = context->firstBufferReady;
  } else {
    bufReady = context->seconBufferReady;
  }

  if (bufReady) {
    // Estimate mean
    mean = MIC_getMean(leftChannel, bufOne);

    // Estimate variance
    power = 0;
    if (bufOne) {
      for ( i = 0; i < sampleBufferLen; i++ ) {
        sample = ((float) context->sampleBufOne[i] - mean) / 32767.5f;
        power += sample * sample;
      }
    } else {
      for ( i = 0; i < sampleBufferLen; i++ ) {
        sample = ((float) context->sampleBufTwo[i] - mean) / 32767.5f;
        power += sample * sample;
      }
    }
    power = power / (float) sampleBufferLen;

    // Convert to decibel
    context->soundLevel = 10.0f * log10f(power);

    if ( var != NULL ) {
      *var = power;
    }

    return context->soundLevel;
  } else {
    return (float)0.0;
  }
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
 *    Sequence number (number of times the callback has been called since
 *    the dma transfer was started
 *
 * @param[in] userParam
 *    User parameters
 *
 * @return
 *    Returns false to stop transfers
 ******************************************************************************/
static bool dmaCompleteCallback(unsigned int channel,
                                unsigned int sequenceNo,
                                void *userParam)
{
  UNUSED_PARAM(channel); // Prevent compiler warning
  MIC_Context *context;
  uint8_t descNr;

  context = (MIC_Context *) userParam;
  GPIO_PinOutToggle(CALLBACK_PORT, context->gpioPin);

  if (context->callBackCount == nrDescNeeded) {
    context->firstBufferReady = !context->firstBufferReady;
    context->seconBufferReady = !context->seconBufferReady;

    if (context->callBackCount > 0) {
      if (context->firstBufferReady) {
        context->dmaDesc[0].xfer.dstAddr = (uint32_t) context->sampleBufTwo;
        context->dmaDesc[1].xfer.dstAddr = (uint32_t) &context->sampleBufTwo[MAX_XFER_COUNT];
      } else {
        context->dmaDesc[0].xfer.dstAddr = (uint32_t) context->sampleBufOne;
        context->dmaDesc[1].xfer.dstAddr = (uint32_t) &context->sampleBufOne[MAX_XFER_COUNT];
      }
      if (context->callBackCount != 1) {
        context->dmaDesc[0].xfer.xferCnt = MAX_XFER_COUNT - 1;
        context->dmaDesc[1].xfer.xferCnt = MAX_XFER_COUNT - 1;
      }
    }
    context->callBackCount = 0;
  } else {
    context->callBackCount++;
    descNr = (sequenceNo % 2);
    if (context->firstBufferReady) {
      context->dmaDesc[descNr].xfer.dstAddr = (uint32_t) &context->sampleBufTwo[MAX_XFER_COUNT * context->callBackCount];
    } else {
      context->dmaDesc[descNr].xfer.dstAddr = (uint32_t) &context->sampleBufOne[MAX_XFER_COUNT * context->callBackCount];
    }

    if (context->callBackCount == nrDescNeeded) {
      context->dmaDesc[descNr].xfer.xferCnt = sampleBufferLen - MAX_XFER_COUNT * nrDescNeeded - 1;
    }
  }
  return true;
}

/** @endcond DO_NOT_INCLUDE_WITH_DOXYGEN */

/** @} {end defgroup Mic_Functions} */

/** @} {end defgroup Mic} */
