/***************************************************************************//**
 * @file
 * @brief Ethernet Microphone demo
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
#include <bsp_os.h>

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <cpu/include/cpu.h>
#include <common/include/common.h>
#include <kernel/include/os.h>
#include <kernel/include/os_trace.h>
#include <common/include/lib_def.h>
#include <common/include/rtos_utils.h>
#include <common/include/toolchains.h>
#include <net/source/http/server/http_server_priv.h>
#include <net/include/net_if.h>
#include <net/include/net_ipv4.h>

#include "network.h"
#include "webserver.h"

#include "glib.h"
#include "graphics.h"

#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "mic_i2s.h"

#include "common_declarations.h"

#include "opus.h"
#include "ogg/ogg.h"

#include  "sl_system_init.h"
#include  "sl_system_kernel.h"
#include  "sl_board_init.h"

#define MAIN_START_TASK_STK_SIZE  34 * 1024u
#define GRAPHIC_TASK_STK_SIZE     512u

OS_MUTEX        audioMutex;
OS_FLAG_GRP     bufferFlags;  // Mic sample buffer ready
OS_FLAG_GRP     micFlags;     // Audio ready to be tranfered

// Struct that contains oggpage
MIC_Data_Typedef micData;

// Start Task Stack.
static CPU_STK MainStartTaskStack[MAIN_START_TASK_STK_SIZE];
static OS_TCB MainStartTaskTCB;

static CPU_STK GraphicTaskStack[GRAPHIC_TASK_STK_SIZE];
static OS_TCB GraphicTaskTCB;

static uint16_t leftSampleBufOne[SAMPLE_BUFFER_LEN];
static uint16_t leftSampleBufTwo[SAMPLE_BUFFER_LEN];

static uint16_t rightSampleBufOne[SAMPLE_BUFFER_LEN];
static uint16_t rightSampleBufTwo[SAMPLE_BUFFER_LEN];

static uint32_t encode_count = 0;

static void MainStartTask(void *arg);
static void GraphicTask(void *arg);
static void micInit(void);
static OpusEncoder *createOpusEncoder(void);
static bool dmaCompleteCallback(unsigned int channel,
                                unsigned int sequenceNo,
                                void *userParam);
static void fillOggPacket(ogg_packet *packet, void *data, opus_int32 len,
                          ogg_int64_t granule, ogg_int64_t num);

int main(void)
{
  RTOS_ERR  err;

  sl_system_init();

  printf("Micrium OS WebMic\n");

  GRAPHICS_Init(); // Initialize Graphics

  OSMutexCreate(&audioMutex, "Audio Mutex", &err);
  OSFlagCreate(&micFlags, "New Mic Flag", 0, &err);
  OSFlagCreate(&bufferFlags, "Mic Buffer Ready", 0, &err);

  OSTaskCreate(&MainStartTaskTCB, // Create the Start Task
               "Main Start Task",
               MainStartTask,
               DEF_NULL,
               MAIN_START_TASK_PRIO,
               &MainStartTaskStack[0],
               (MAIN_START_TASK_STK_SIZE / 10u),
               MAIN_START_TASK_STK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);

  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

  sl_system_kernel_start();   

  return (1);
}

/***************************************************************************//**
 * This is the task that will be called by the Startup when all services
 * are initializes successfully.
 *
 * @param p_arg: Argument passed from task creation. Unused, in this case.
 ******************************************************************************/
static void MainStartTask(void *p_arg)
{
  PP_UNUSED_PARAM(p_arg); // Prevent compiler warning
  RTOS_ERR  err;
  CPU_TS ts;
  OpusEncoder *encoder;
  ogg_stream_state streamState = { 0 };
  ogg_packet oggPacket = { 0 };
  ogg_page oggPage;
  unsigned char encData[BIT_RATE * TIME_MS / 1000 / 8];
  opus_int32 packetLen;
  ogg_int64_t granule = 0;
  ogg_int64_t packetNum = 2;
  uint16_t *p_txBuf = 0;

#ifdef CPU_CFG_INT_DIS_MEAS_EN
  CPU_IntDisMeasMaxCurReset();
#endif

  Common_Init(&err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  BSP_OS_Init();

  // Initialize board control, which will also enable the display 
  sl_board_init();                                            

  Network_Init();    // Call Network core initialization example.
  Network_Start();   // Call network interface start example.
  Webserver_Init();  // Start the http Server
  Webserver_Start();

  // Application specific initialization
  micInit();
  encoder = createOpusEncoder();
  ogg_stream_init(&streamState, SERIALNO);

  // Start graphics task
  OSTaskCreate(&GraphicTaskTCB,
               "Graphic Task",
               GraphicTask,
               DEF_NULL,
               GRAPHIC_TASK_PRIO,
               &GraphicTaskStack[0],
               (GRAPHIC_TASK_STK_SIZE / 10u),
               GRAPHIC_TASK_STK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);

  // Start recording using mic's
  MIC_start();

  while (DEF_ON) {
    // Wait for one of the LDMA to be ready
    OSFlagPend(&bufferFlags,
               READ_BUF_ONE | READ_BUF_TWO,
               (OS_TICK )0,
               (OS_OPT)OS_OPT_PEND_FLAG_SET_ANY,
               &ts,
               &err);

    if (bufferFlags.Flags & READ_BUF_ONE) {
      if (MIC_CH == MIC_CH1) {
        p_txBuf = leftSampleBufOne;
      } else {
        p_txBuf = rightSampleBufOne;
      }

      OSFlagPost(&bufferFlags, READ_BUF_ONE, (OS_OPT)OS_OPT_POST_FLAG_CLR, &err);
    } else if (bufferFlags.Flags & READ_BUF_TWO) {
      if (MIC_CH == MIC_CH1) {
        p_txBuf = leftSampleBufTwo;
      } else {
        p_txBuf = rightSampleBufTwo;
      }

      OSFlagPost(&bufferFlags, READ_BUF_TWO, (OS_OPT)OS_OPT_POST_FLAG_CLR, &err);
    } else {
      APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE),; );
    }

    packetLen = opus_encode(encoder,
                            (const int16_t *)p_txBuf,
                            SAMPLE_BUFFER_LEN,
                            encData,
                            sizeof(encData));
    encode_count++;

    granule += SAMPLE_BUFFER_LEN;
    fillOggPacket(&oggPacket, encData, packetLen, granule, packetNum++);
    ogg_stream_packetin(&streamState, &oggPacket);
    ogg_stream_flush(&streamState, &oggPage);

    // Get mutex of micData struct
    OSMutexPend(&audioMutex,
                0,
                OS_OPT_PEND_BLOCKING,
                (CPU_TS *)0,
                &err);

    micData.oggPage = &oggPage;
    micData.pcmBuf = p_txBuf;
    micData.counter = encode_count;
    GRAPHICS_AddAudioSamples((int16_t *)p_txBuf, SAMPLE_BUFFER_LEN);

    OSMutexPost(&audioMutex,
                OS_OPT_POST_NONE,
                &err);

    // Set new mic data flag
    OSFlagPost(&micFlags,
               (OS_FLAGS)0xFFFF,
               (OS_OPT)OS_OPT_POST_FLAG_SET,
               &err);

    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE),; );
  }
}

static void GraphicTask(void *p_arg)
{
  PP_UNUSED_PARAM(p_arg);

  while (true) {
    GRAPHICS_ShowStatus();
  }
}

/***************************************************************************//**
 * Initialize microphones
 ******************************************************************************/
static void micInit(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true); // Enable clocks

  GPIO_PinModeSet(MIC_ENABLE_PORT, MIC_ENABLE_PIN, gpioModePushPull, 1);

  MIC_init(dmaCompleteCallback,
           EX_SAMPLE_FREQ,
           leftSampleBufOne,
           leftSampleBufTwo,
           rightSampleBufOne,
           rightSampleBufTwo,
           SAMPLE_BUFFER_LEN);
}

/***************************************************************************//**
 * Alloc and initialize an opus encoder
 *
 * @returns a ready to use opus encoder
 ******************************************************************************/
static OpusEncoder *createOpusEncoder(void)
{
  RTOS_ERR err;
  OpusEncoder *enc;

  enc = Mem_SegAlloc("opus encoder",
                     NULL,
                     opus_encoder_get_size(CHANNELS),
                     &err);
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE),; );

  opus_encoder_init(enc,
                    EX_SAMPLE_FREQ,
                    CHANNELS,
                    OPUS_APPLICATION_AUDIO);

  // LOFI low delay options
  opus_encoder_ctl(enc, OPUS_SET_COMPLEXITY(0));
  opus_encoder_ctl(enc, OPUS_SET_BITRATE(BIT_RATE));
  opus_encoder_ctl(enc, OPUS_SET_PREDICTION_DISABLED(1));

  return enc;
}

/***************************************************************************//**
 * Alloc and initialize an opus encoder
 *
 * @param packet struct holding raw data to send in ogg stream
 * @param data pointer to the actual data
 * @param len size of data (in bytes)
 * @param granule opus sample position
 * @param num packets sequence number in stream
 ******************************************************************************/
static void fillOggPacket(ogg_packet *packet, void *data, opus_int32 len,
                          ogg_int64_t granule, ogg_int64_t num)
{
  packet->packet = data;
  packet->bytes = len;
  packet->b_o_s = 0;
  packet->e_o_s = 0;
  packet->granulepos = granule;
  packet->packetno = num;
}

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
  RTOS_ERR  err;
  MIC_Context *context;

  context = (MIC_Context *) userParam;
  GPIO_PinOutToggle(CALLBACK_PORT, context->gpioPin);

  if (MIC_CH == channel) {
    if ((sequenceNo % 2) == 1) {
      OSFlagPost(&bufferFlags,
                 READ_BUF_TWO,
                 (OS_OPT)OS_OPT_POST_FLAG_CLR,
                 &err);

      OSFlagPost(&bufferFlags,
                 READ_BUF_ONE,
                 (OS_OPT)OS_OPT_POST_FLAG_SET,
                 &err);
    } else {
      OSFlagPost(&bufferFlags,
                 READ_BUF_ONE,
                 (OS_OPT)OS_OPT_POST_FLAG_CLR,
                 &err);

      OSFlagPost(&bufferFlags,
                 READ_BUF_TWO,
                 (OS_OPT)OS_OPT_POST_FLAG_SET,
                 &err);
    }
  }
  return true;
}
