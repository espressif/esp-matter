/***************************************************************************//**
 * @file
 * @brief Common values shared in this application
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

#ifndef COMMON_DECLARATION_H
#define COMMON_DECLARATION_H

#include <stdint.h>
#include <kernel/include/os.h>
#include "ogg/ogg.h"
#include "em_common.h"

// -----------------------------------------------------------------------------
// Define constants

#define MAIN_START_TASK_PRIO      10u
#define HTTP_SERVER_PRIO          21u
#define GRAPHIC_TASK_PRIO         30u

#define  NR_CLIENT_MAX     5u // Increased number MAY introduce latency

#define  READ_BUF_ONE      (OS_FLAGS)0x0002
#define  READ_BUF_TWO      (OS_FLAGS)0x0004

#define  TIME_MS           40u // Max 60ms. See IETF rfc6716
#define  EX_SAMPLE_FREQ    48000u // Max 48kHz. See IETF rfc6716
#define  SAMPLE_BUFFER_LEN (EX_SAMPLE_FREQ * TIME_MS / 1000)
#define  MIC_CH            MIC_CH1 // Select right or left mic
#define  CHANNELS          MONO // NB! Stereo is not implemented
#define  BIT_RATE          72000

#define  PRE_SKIP          3840 // 80 ms

#define  SERIALNO          1234 // Can be anything you want
#define  DC_CORRECTION     1900u // DC offset in mic samples

// -----------------------------------------------------------------------------
// Data types

enum mic_channels {
  MIC_CH1 = 0,
  MIC_CH2 = 1
};

enum sound_channels {
  MONO = 1,
  STEREO = 2
};

typedef struct MIC_Data {
  ogg_page *oggPage;
  uint16_t *pcmBuf;
  uint32_t counter;
  // Possibility for other fields to be shared
} MIC_Data_Typedef;

// NB! Opus fields is little endian
SL_PACK_START(1)
typedef struct {
  uint8_t magic_num[4];
  uint8_t version;
  uint8_t type;
  uint8_t granule[8];
  uint32_t serial;
  uint32_t sequence;
  uint32_t checksum;
  uint8_t segments;
  uint8_t size; // WARNING! This assumes just 1 segment
} SL_ATTRIBUTE_PACKED OGG_PageHeader_TypeDef;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  char signature[8];
  uint8_t version;
  uint8_t channels;
  uint16_t preskip;
  uint32_t samplerate;
  uint16_t gain;
  uint8_t chmap;
} SL_ATTRIBUTE_PACKED OPUS_IdHeader_TypeDef;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  char signature[8];
  uint32_t vendor_len; // WARNING! Must be 0 (see IETF rfc7845 section 5.)
  //char vendor_string[]; don't need this
  uint32_t list_len; // WARNING! Must be 0 (see IETF rfc7845 section 5.)
  //char user_comment[]; don't need this
} SL_ATTRIBUTE_PACKED OPUS_CommentHeader_TypeDef;
SL_PACK_END()

// -----------------------------------------------------------------------------
// Global variables

extern OS_MUTEX    audioMutex;
extern OS_FLAG_GRP micFlags;
extern OS_FLAG_GRP bufferFlags;

extern MIC_Data_Typedef micData;

#endif // COMMON_DECLARATION_H
