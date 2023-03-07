/***************************************************************************//**
 * @file
 * @brief
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

#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <string.h>

#include "sl_memlcd.h"
#include "dmd.h"
#include "em_device.h"
#include "em_gpio.h"
#include "glib.h"
#include "graphics.h"
#include "common_declarations.h"
#include "sl_sleeptimer.h"

#include <kernel/include/os.h>
#include <net/include/net_if.h>
#include <net/include/net_ipv4.h>
#include <net/include/net_ipv6.h>
#include <common/include/rtos_utils.h>

#define GLIB_FONT_WIDTH   (glibContext.font.fontWidth + glibContext.font.charSpacing)
#define GLIB_FONT_HEIGHT  (glibContext.font.fontHeight)
#define CENTER_X          (glibContext.pDisplayGeometry->xSize / 2)
#define CENTER_Y          (glibContext.pDisplayGeometry->ySize / 2)
#define MAX_X             (glibContext.pDisplayGeometry->xSize - 1)
#define MAX_Y             (glibContext.pDisplayGeometry->ySize - 1)
#define MIN_X             0
#define MIN_Y             0
#define MAX_STR_LEN       48
#define PCM_OFFSET        1650  // Adjust for PCM sample offset
#define WAVEFORM_GAIN     1.5f  // Increase waveform response

typedef struct waveform {
  int head;
  float min[128];
  float max[128];
} waveform_t;

static waveform_t waveform;

// -----------------------------------------------------------------------------
// Local variables

static GLIB_Context_t glibContext;
static sl_sleeptimer_timer_handle_t lcdTimer;

// -----------------------------------------------------------------------------
// Local function declarations
static void GRAPHICS_DrawString(const char * s, int y);
static bool GRAPHICS_DrawIPv4Status(void);
static void GRAPHICS_DrawTitle(void);
static void GRAPHICS_DrawAudioWaveform(void);
static void GRAPHICS_PinToggle(sl_sleeptimer_timer_handle_t *handle, void *data);

/**************************************************************************//**
 * @brief Toggle the EXTCOMIN pin
 *****************************************************************************/
static void GRAPHICS_PinToggle(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  (void) handle;
  (void) data;
  GPIO_PinOutToggle(SL_MEMLCD_EXTCOMIN_PORT, SL_MEMLCD_EXTCOMIN_PIN);
}

// -----------------------------------------------------------------------------
// Global function definitions

/***************************************************************************//**
 * Initializes glib and DMD.
 ******************************************************************************/
void GRAPHICS_Init(void)
{
  EMSTATUS status;

  // Initialize the DMD module for the DISPLAY device driver.
  status = DMD_init(0);
  if (DMD_OK != status) {
    while (1) {
    }
  }

  status = GLIB_contextInit(&glibContext);
  if (GLIB_OK != status) {
    while (1) {
    }
  }

  // Pin toggle frequency of 1 Hz
  GPIO_PinModeSet(SL_MEMLCD_EXTCOMIN_PORT, SL_MEMLCD_EXTCOMIN_PIN, gpioModePushPull, 0);
  sl_sleeptimer_start_periodic_timer_ms(&lcdTimer, 500, GRAPHICS_PinToggle, NULL, 0, 0);
}

/***************************************************************************//**
 * Update the whole display with current status.
 ******************************************************************************/
void GRAPHICS_ShowStatus(void)
{
  GLIB_clear(&glibContext);

  glibContext.backgroundColor = White;
  glibContext.foregroundColor = Black;

  GRAPHICS_DrawTitle();
  GRAPHICS_DrawIPv4Status();

  GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNormal8x8);
  GRAPHICS_DrawString("----------------", 30);
  
  GRAPHICS_DrawAudioWaveform();
  
  DMD_updateDisplay();
}

static void GRAPHICS_AudioWaveformDataAdd(float min, float max)
{
  int i = waveform.head;
  waveform.min[i] = min;
  waveform.max[i] = max;
  waveform.head++;
  if (waveform.head == 128) {
    waveform.head = 0;
  }
}

static void GRAPHICS_DrawAudioWaveform(void)
{
  const int baseline = 79;  // y value of the middle line
  const int height = 79;    // height in pixels
  
  int x = 0;
  int i = waveform.head + 1;
  while (i != waveform.head) {
    float min = waveform.min[i] * WAVEFORM_GAIN;
    float max = waveform.max[i] * WAVEFORM_GAIN;
    
    int min_y = baseline + (int)(min * height);
    if (min_y < 40) {
      min_y = 40;
    }
    int max_y = baseline + (int)(max * height);
    
    GLIB_drawLineV(&glibContext, x, min_y, max_y);
    x++;
    i++;
    if (i == 128) {
      i = 0;
    }
  }
}

/***************************************************************************//**
 * Add audio samples to waveform display
 *
 * @param samples mic samples
 * @param len number of samples
 ******************************************************************************/
void GRAPHICS_AddAudioSamples(const int16_t *samples, size_t len)
{
  size_t i = 0;
  while (i < len) {
    int16_t min_value = 0;
    int16_t max_value = 0;
    
    // Find min and max value within a sample block
    size_t end = i + 80;
    while (i < len && i < end) {
      int16_t value = samples[i] + PCM_OFFSET;
      if (value < min_value) {
        min_value = value;
      }
      if (value > max_value) {
        max_value = value;
      }
      i++;
    }
    
    // map to value between [-1.0, 1.0]
    float min_norm = (float)min_value / INT16_MAX;
    float max_norm = (float)max_value / INT16_MAX;

    GRAPHICS_AudioWaveformDataAdd(min_norm, max_norm);
  }
}

/***************************************************************************//**
 * Draw title
 ******************************************************************************/
static void GRAPHICS_DrawTitle(void)
{
  GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNormal8x8);
  GRAPHICS_DrawString("Web Microphone", 0);
  GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNarrow6x8);
  GRAPHICS_DrawString("Micrium OS Network", 10);
}

/***************************************************************************//**
 * Draw a string at a specific y coordinate
 ******************************************************************************/
static void GRAPHICS_DrawString(const char * s, int y)
{
  GLIB_drawString(&glibContext, s, strlen(s), 0, y, false);
}

/***************************************************************************//**
 * Draw IPv4 address
 ******************************************************************************/
static bool GRAPHICS_DrawIPv4Status(void)
{
  RTOS_ERR          err;
  NET_IF_NBR        ifNbr;
  NET_IPv4_ADDR     addrTable[4];
  NET_IP_ADDRS_QTY  addrTableSize = 4;
  CPU_BOOLEAN       ok;
  CPU_CHAR          addrString[NET_ASCII_LEN_MAX_ADDR_IPv4];

  ifNbr = NetIF_NbrGetFromName("eth0");
  ok = NetIPv4_GetAddrHost(ifNbr, addrTable, &addrTableSize, &err);

  if (!ok) {
    return DEF_OFF;
  }

  if (addrTableSize > 0) {
    NetASCII_IPv4_to_Str(addrTable[0], addrString, DEF_NO, &err);
    APP_RTOS_ASSERT_CRITICAL((err.Code == RTOS_ERR_NONE),; );
    GRAPHICS_DrawString(addrString, 20);
    if (addrTable[0] == 0) {
      return DEF_OFF;
    } else {
      return DEF_ON;
    }
  }
  return DEF_OFF;
}
