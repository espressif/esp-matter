/***************************************************************************//**
 * @file
 * @brief I2S microphone driver
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
#include <math.h>
#include "em_cmu.h"
#include "em_usart.h"
#include "em_ldma.h"
#include "dmadrv.h"
#include "sl_mic.h"
#include "sl_mic_i2s_config.h"

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/* Concatenate preprocessor tokens A, B and C. */
#define SL_CONCAT(A, B, C) A ## B ## C
/* Generate the cmu clock symbol based on instance. */
#define MIC_I2S_USART_CLOCK(N) SL_CONCAT(cmuClock_USART, N, )
#define MIC_DMA_LEFT_SIGNAL(N) SL_CONCAT(ldmaPeripheralSignal_USART, N, _RXDATAV)
#define MIC_DMA_RIGHT_SIGNAL(N) SL_CONCAT(ldmaPeripheralSignal_USART, N, _RXDATAVRIGHT)

static sl_mic_buffer_ready_callback_t buffer_ready_callback = NULL;
static int16_t      *sample_buffer;         /**< Buffer used to store the microphone samples               */
static int16_t      *streaming_buffer[2];   /**< Buffers used to perform ping-pong operation               */
static uint32_t      sample_count;          /**< The number of samples to collect                          */
static uint32_t      sample_index;
static volatile bool reading_samples_to_buffer; /**< Flag to show when the buffer is currently being filled with samples */
static volatile bool streaming_in_progress; /**< Flag to show if driver is running in streaming mode */
static bool          mic_running;
static bool          initialized;
static uint8_t       num_channels;
static int16_t       sample_left;              // discard buffer
static int16_t       sample_right;             // discard buffer

static unsigned int  dma_channel_left;      /**< The channel Id assigned by DMADRV                         */
static unsigned int  dma_channel_right;     /**< The dummy channel Id assigned by DMADRV                   */

static LDMA_TransferCfg_t dma_transfer_left =
  LDMA_TRANSFER_CFG_PERIPHERAL(MIC_DMA_LEFT_SIGNAL(SL_MIC_I2S_PERIPHERAL_NO));

static LDMA_TransferCfg_t dma_transfer_right =
  LDMA_TRANSFER_CFG_PERIPHERAL(MIC_DMA_RIGHT_SIGNAL(SL_MIC_I2S_PERIPHERAL_NO));

static LDMA_Descriptor_t dma_descriptor_left[2] = {
  LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&SL_MIC_I2S_PERIPHERAL->RXDOUBLE, &sample_left, 2047, 1),
  LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&SL_MIC_I2S_PERIPHERAL->RXDOUBLE, &sample_left, 2047, -1)
};

static LDMA_Descriptor_t dma_descriptor_right[2] = {
  LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&SL_MIC_I2S_PERIPHERAL->RXDOUBLE, &sample_right, 2047, 1),
  LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&SL_MIC_I2S_PERIPHERAL->RXDOUBLE, &sample_right, 2047, -1)
};

/** @endcond */

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

static bool dma_complete(unsigned int channel, unsigned int sequenceNo, void *userParam);

/** @endcond */

/***************************************************************************//**
 *    Initializes the microphone
 ******************************************************************************/
sl_status_t sl_mic_init(uint32_t sample_rate, uint8_t n_channels)
{
  // Only one channel is supported
  if (n_channels < 1 || n_channels > 2) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  uint32_t status;
  USART_InitI2s_TypeDef usartInit = USART_INITI2S_DEFAULT;

  /* Enable clocks */
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(MIC_I2S_USART_CLOCK(SL_MIC_I2S_PERIPHERAL_NO), true);

  /* Setup GPIO pins */
  GPIO_PinModeSet(SL_MIC_I2S_RX_PORT, SL_MIC_I2S_RX_PIN, gpioModeInput, 0);
  GPIO_PinModeSet(SL_MIC_I2S_CLK_PORT, SL_MIC_I2S_CLK_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(SL_MIC_I2S_CS_PORT, SL_MIC_I2S_CS_PIN, gpioModePushPull, 0);

  /* Setup USART in I2S mode to get data from microphone */
  usartInit.sync.enable   = usartEnable;
  usartInit.sync.baudrate = sample_rate * 64; // 32-bit stereo frame
  usartInit.sync.autoTx   = true;
  usartInit.format        = usartI2sFormatW32D16;

  if (n_channels == 1) {
    // Split DMA requests to discard right-channel data
    usartInit.dmaSplit      = true;
  }

  USART_InitI2s(SL_MIC_I2S_PERIPHERAL, &usartInit);

#if defined(_SILICON_LABS_32B_SERIES_2)
  GPIO->USARTROUTE->ROUTEEN = GPIO_USART_ROUTEEN_RXPEN | GPIO_USART_ROUTEEN_CLKPEN | GPIO_USART_ROUTEEN_CSPEN;
  GPIO->USARTROUTE->RXROUTE = (SL_MIC_I2S_RX_PORT << _GPIO_USART_RXROUTE_PORT_SHIFT) | (SL_MIC_I2S_RX_PIN << _GPIO_USART_RXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE->CLKROUTE = (SL_MIC_I2S_CLK_PORT << _GPIO_USART_CLKROUTE_PORT_SHIFT) | (SL_MIC_I2S_CLK_PIN << _GPIO_USART_CLKROUTE_PIN_SHIFT);
  GPIO->USARTROUTE->CSROUTE = (SL_MIC_I2S_CS_PORT << _GPIO_USART_CSROUTE_PORT_SHIFT) | (SL_MIC_I2S_CS_PIN << _GPIO_USART_CSROUTE_PIN_SHIFT);
#else
  SL_MIC_I2S_PERIPHERAL->ROUTELOC0 = (SL_MIC_I2S_RX_LOC << _USART_ROUTELOC0_RXLOC_SHIFT | SL_MIC_I2S_CLK_LOC << _USART_ROUTELOC0_CLKLOC_SHIFT | SL_MIC_I2S_CS_LOC << _USART_ROUTELOC0_CSLOC_SHIFT);
  SL_MIC_I2S_PERIPHERAL->ROUTEPEN  = (USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_CLKPEN | USART_ROUTEPEN_CSPEN);
#endif

  /* Setup DMA driver to move samples from USART to memory */
  DMADRV_Init();

  /* Set up DMA channel for I2S left (microphone) data */
  status = DMADRV_AllocateChannel(&dma_channel_left, NULL);
  if ( status != ECODE_EMDRV_DMADRV_OK ) {
    return SL_STATUS_FAIL;
  }

  dma_descriptor_left[0].xfer.size   = ldmaCtrlSizeHalf;
  dma_descriptor_left[0].xfer.dstInc = ldmaCtrlDstIncNone;
  dma_descriptor_left[1].xfer.size   = ldmaCtrlSizeHalf;
  dma_descriptor_left[1].xfer.dstInc = ldmaCtrlDstIncNone;

  if (n_channels == 1 ) {
    /* Set up DMA channel to discard I2S right data */
    status = DMADRV_AllocateChannel(&dma_channel_right, NULL);
    if ( status != ECODE_EMDRV_DMADRV_OK ) {
      return SL_STATUS_FAIL;
    }

    dma_descriptor_right[0].xfer.size   = ldmaCtrlSizeHalf;
    dma_descriptor_right[0].xfer.dstInc = ldmaCtrlDstIncNone;
    dma_descriptor_right[1].xfer.size   = ldmaCtrlSizeHalf;
    dma_descriptor_right[1].xfer.dstInc = ldmaCtrlDstIncNone;
  }

  /* Driver parameters */
  num_channels = n_channels;
  reading_samples_to_buffer = false;
  streaming_in_progress = false;
  mic_running = false;
  initialized = true;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    De-initializes the microphone
 ******************************************************************************/
sl_status_t sl_mic_deinit(void)
{
  /* Stop sampling */
  sl_mic_stop();

  /* Reset USART peripheral and disable IO pins */
  USART_Reset(SL_MIC_I2S_PERIPHERAL);
  SL_MIC_I2S_PERIPHERAL->I2SCTRL = 0;

  GPIO_PinModeSet(SL_MIC_I2S_CLK_PORT, SL_MIC_I2S_CLK_PIN, gpioModeDisabled, 0);
  GPIO_PinModeSet(SL_MIC_I2S_RX_PORT, SL_MIC_I2S_RX_PIN, gpioModeDisabled, 0);
  GPIO_PinModeSet(SL_MIC_I2S_CS_PORT, SL_MIC_I2S_CS_PIN, gpioModeDisabled, 0);

  /* Free resources */
  DMADRV_FreeChannel(dma_channel_left);
  DMADRV_FreeChannel(dma_channel_right);

  initialized = false;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Read samples from the microphone sample buffer
 ******************************************************************************/
sl_status_t sl_mic_get_n_samples(void *buffer, uint32_t n_frames)
{
  if (!initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (reading_samples_to_buffer || streaming_in_progress) {
    return SL_STATUS_INVALID_STATE;
  }

  if (!mic_running) {
    sl_mic_start();
  }

  sample_buffer = (int16_t *)buffer;
  sample_count = num_channels * n_frames;
  sample_index = 0;
  reading_samples_to_buffer = true;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Start streaming
 ******************************************************************************/
sl_status_t sl_mic_start_streaming(void *buffer, uint32_t n_frames, sl_mic_buffer_ready_callback_t callback)
{
  if (!initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (reading_samples_to_buffer || streaming_in_progress) {
    return SL_STATUS_INVALID_STATE;
  }

  if ((num_channels * n_frames) > DMADRV_MAX_XFER_COUNT) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (!mic_running) {
    sl_mic_start();
  }

  sample_count = num_channels * n_frames;
  streaming_buffer[0] = (int16_t *)buffer;
  streaming_buffer[1] = &(((int16_t *)buffer)[sample_count]);
  buffer_ready_callback = callback;
  streaming_in_progress = true;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Starts the microphone
 ******************************************************************************/
sl_status_t sl_mic_start(void)
{
  if (!initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (mic_running) {
    return SL_STATUS_INVALID_STATE;
  }

  // Reset descriptors, drop the first 4096 samples
  dma_descriptor_left[0].xfer.dstInc = ldmaCtrlDstIncNone;
  dma_descriptor_left[0].xfer.xferCnt = 2047;
  dma_descriptor_left[0].xfer.dstAddr = (uint32_t) &sample_left;
  dma_descriptor_left[1].xfer.dstInc = ldmaCtrlDstIncNone;
  dma_descriptor_left[1].xfer.xferCnt = 2047;
  dma_descriptor_left[1].xfer.dstAddr = (uint32_t) &sample_left;

  /* Start DMA channel for I2S left (microphone) data */
  DMADRV_LdmaStartTransfer(dma_channel_left,
                           &dma_transfer_left,
                           &dma_descriptor_left[0],
                           dma_complete,
                           NULL);

  if (num_channels == 1) {
    /* Start DMA channel for I2S right (dummy) data */
    DMADRV_LdmaStartTransfer(dma_channel_right,
                             &dma_transfer_right,
                             &dma_descriptor_right[0],
                             NULL,
                             NULL);
  }

  mic_running = true;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Stops the microphone
 ******************************************************************************/
sl_status_t sl_mic_stop(void)
{
  if (!mic_running) {
    return SL_STATUS_INVALID_STATE;
  }

  /* Stop sampling */
  DMADRV_StopTransfer(dma_channel_left);

  if (num_channels == 1) {
    DMADRV_StopTransfer(dma_channel_right);
  }

  mic_running = false;
  reading_samples_to_buffer = false;
  streaming_in_progress = false;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Calculates the dBSPL value for a channel from a sample buffer
 ******************************************************************************/
sl_status_t sl_mic_calculate_sound_level(float *sound_level, const int16_t *buffer, uint32_t n_frames, uint8_t channel)
{
  float sample;
  float power;
  float mean;
  uint32_t i;

  if (channel >= num_channels) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  // Calculate mean
  mean = 0.0f;
  for ( i = channel; i < (n_frames * num_channels); i += num_channels) {
    mean += (float) buffer[i];
  }
  mean = mean / (float) n_frames;

  /* Estimate variance */
  power = 0;
  for ( i = channel; i < (n_frames * num_channels); i += num_channels) {
    sample = ( (float) buffer[i] - mean) / 32767.5f;
    power += sample * sample;
  }
  power = power / (float)n_frames;

  // Convert to dBSPL
  *sound_level = 10.0f * log10f(power) + 120;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Checks if the sample buffer is ready
 ******************************************************************************/
bool sl_mic_sample_buffer_ready(void)
{
  return (!reading_samples_to_buffer);
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
static bool dma_complete(unsigned int channel, unsigned int sequenceNo, void *userParam)
{
  (void)channel;
  (void)userParam;
  LDMA_Descriptor_t *nextDesc;

  if ( reading_samples_to_buffer ) {
    /* Setup next descriptor */
    if ( sequenceNo & 0x01 ) {
      nextDesc = &dma_descriptor_left[0];
    } else {
      nextDesc = &dma_descriptor_left[1];
    }

    if ( sample_index < sample_count ) {
      nextDesc->xfer.dstInc  = ldmaCtrlDstIncOne;
      nextDesc->xfer.dstAddr = (uint32_t) &sample_buffer[sample_index];

      if ((sample_count - sample_index) > 2048 ) {
        nextDesc->xfer.xferCnt = 2047;
        sample_index += 2048;
      } else {
        nextDesc->xfer.xferCnt = (sample_count - sample_index) - 1;
        sample_index += (sample_count - sample_index);
      }
    } else {
      nextDesc->xfer.dstInc = ldmaCtrlDstIncNone;
      nextDesc->xfer.xferCnt = 2047;
      nextDesc->xfer.dstAddr = (uint32_t) &sample_left;
    }
    /* Sample buffer is complete if both descriptors have returned back to default */
    if ( (dma_descriptor_left[0].xfer.dstInc == ldmaCtrlDstIncNone)
         && (dma_descriptor_left[1].xfer.dstInc == ldmaCtrlDstIncNone) ) {
      // Sample buffer is complete if both descriptors have returned back to default
      // Stop PDM when sample buffer is complete
      reading_samples_to_buffer = false;
      sl_mic_stop();
    }
  } else if (streaming_in_progress) {
    unsigned int idx_next = 1 - (sequenceNo % 2);

    if (dma_descriptor_left[idx_next].xfer.dstInc == ldmaCtrlDstIncNone) {
      // Initialize descriptor for streaming mode
      dma_descriptor_left[idx_next].xfer.dstInc = ldmaCtrlDstIncOne;
      dma_descriptor_left[idx_next].xfer.xferCnt = sample_count - 1;
      dma_descriptor_left[idx_next].xfer.dstAddr = (uint32_t)streaming_buffer[idx_next];
    } else {
      // Buffer ready
      if (buffer_ready_callback) {
        buffer_ready_callback(streaming_buffer[idx_next], sample_count / num_channels);
      }
    }
  }

  return true;
}

/** @endcond */

/** @} {end addtogroup Mic_i2s} */
