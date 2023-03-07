/***************************************************************************//**
 * @file
 * @brief Draws the graphics on the display
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

#include "sl_memlcd.h"
#include "em_gpio.h"
#include "dmd.h"
#include "glib.h"
#include "em_device.h"
#include "sl_sleeptimer.h"
#include <kernel/include/os.h>
#include <net/include/net_def.h>
#include <net/include/net_if.h>
#include <net/include/net_ipv4.h>
#include <net/include/net_ipv6.h>
#include <common/include/rtos_utils.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

static GLIB_Context_t glibContext;
static sl_sleeptimer_timer_handle_t lcdTimer;

#define GLIB_FONT_WIDTH           (glibContext.font.fontWidth + glibContext.font.charSpacing)
#define GLIB_FONT_HEIGHT          (glibContext.font.fontHeight)
#define CENTER_X                  (glibContext.pDisplayGeometry->xSize / 2)
#define CENTER_Y                  (glibContext.pDisplayGeometry->ySize / 2)
#define MAX_X                     (glibContext.pDisplayGeometry->xSize - 1)
#define MAX_Y                     (glibContext.pDisplayGeometry->ySize - 1)
#define MIN_X                     0
#define MIN_Y                     0
#define MAX_STR_LEN               48

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/
static void GRAPHICS_DrawString(const char * s, int y);
static void GRAPHICS_DrawLinkStatus(void);
static void GRAPHICS_DrawMacStatus(void);
static void GRAPHICS_DrawIPv4Status(void);
static void GRAPHICS_DrawIPv6Status(void);
static void GRAPHICS_DrawTxRxStatus(void);
static void GRAPHICS_DrawTitle(void);
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

/***************************************************************************//**
 * @brief Initializes the glib and DMD.
 ******************************************************************************/
void GRAPHICS_Init(void)
{
  EMSTATUS status;

  // Initialize the DMD module for the DISPLAY device driver.
  status = DMD_init(0);
  if (DMD_OK != status) {
    while (1) {}
  }

  status = GLIB_contextInit(&glibContext);
  if (GLIB_OK != status) {
    while (1) {}
  }

  // Pin toggle frequency of 1 Hz
  GPIO_PinModeSet(SL_MEMLCD_EXTCOMIN_PORT, SL_MEMLCD_EXTCOMIN_PIN, gpioModePushPull, 0);
  sl_sleeptimer_start_periodic_timer_ms(&lcdTimer, 500, GRAPHICS_PinToggle, NULL, 0, 0);
}

/***************************************************************************//**
 * @brief Update the whole display with current status.
 ******************************************************************************/
void GRAPHICS_ShowStatus(void)
{
  GLIB_clear(&glibContext);
  GRAPHICS_DrawTitle();
  GRAPHICS_DrawMacStatus();
  GRAPHICS_DrawLinkStatus();
  GRAPHICS_DrawIPv4Status();
  GRAPHICS_DrawIPv6Status();
  GRAPHICS_DrawTxRxStatus();
  DMD_updateDisplay();
}

/***************************************************************************//**
 * @brief Draw title
 ******************************************************************************/
static void GRAPHICS_DrawTitle(void)
{
  GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNormal8x8);
  GRAPHICS_DrawString("Micrium OS Net", 0);
  GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNarrow6x8);
}

/***************************************************************************//**
 * @brief Draw a string at a specific y coordinate
 ******************************************************************************/
static void GRAPHICS_DrawString(const char * s, int y)
{
  GLIB_drawString(&glibContext, s, strlen(s), 0, y, false);
}

/***************************************************************************//**
 * @brief Draw Link status, linke is either "up" or "down"
 ******************************************************************************/
static void GRAPHICS_DrawLinkStatus(void)
{
  RTOS_ERR err;
  NET_IF_LINK_STATE link;
  const char * msg;

  link = NetIF_LinkStateGet(1, &err);


  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE)
  {
    msg = "Link error";
  }
  else if (link == NET_IF_LINK_UP)
  {
    msg = "Link up";
  }
  else
  {
    msg = "Link down";
  }
  GRAPHICS_DrawString(msg, 20);
}

/***************************************************************************//**
 * @brief Print MAC Address
 ******************************************************************************/
static void GRAPHICS_DrawMacStatus(void)
{
  RTOS_ERR err;
  CPU_INT08U  addr_hw[NET_IF_802x_HW_ADDR_LEN];
  CPU_INT08U  addr_len = NET_IF_802x_HW_ADDR_LEN;
  CPU_CHAR    addr_str[NET_IF_802x_ADDR_SIZE_STR];
  const char * msg;

  NetIF_AddrHW_Get(1, addr_hw, &addr_len, &err);

  if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
    NetASCII_MAC_to_Str(addr_hw, addr_str, DEF_FALSE, DEF_TRUE, &err);
    msg = addr_str;
  } else {
    msg = "";
  }
  GRAPHICS_DrawString(msg, 10);
}

/***************************************************************************//**
 * @brief Draw IPv4 address
 ******************************************************************************/
static void GRAPHICS_DrawIPv4Status(void)
{
  RTOS_ERR          err;
  NET_IPv4_ADDR     addrTable[4];
  NET_IP_ADDRS_QTY  addrTableSize = 4;
  CPU_BOOLEAN       ok;
  CPU_CHAR          addrString[NET_ASCII_LEN_MAX_ADDR_IPv4];
  NET_IF_LINK_STATE link;

  link = NetIF_LinkStateGet(1, &err);

  if (link == NET_IF_LINK_UP) {

    ok = NetIPv4_GetAddrHost(1, addrTable, &addrTableSize, &err);

    if (!ok) {
      return;
    }

    if (addrTableSize > 0) {

      NetASCII_IPv4_to_Str(addrTable[0], addrString, DEF_NO, &err);
      APP_RTOS_ASSERT_CRITICAL((err.Code == RTOS_ERR_NONE), ;);
      GRAPHICS_DrawString(addrString, 30);
    }
  }
}

/***************************************************************************//**
 * @brief Draw IPv6 addresses
 ******************************************************************************/
static void GRAPHICS_DrawIPv6Status(void)
{
  RTOS_ERR          err;
  NET_IPv6_ADDR     addrTable[4];
  NET_IP_ADDRS_QTY  addrTableSize = 4;
  CPU_BOOLEAN       ok;
  CPU_CHAR          addrString[NET_ASCII_LEN_MAX_ADDR_IPv6];
  NET_IF_LINK_STATE link;

  link = NetIF_LinkStateGet(1, &err);

  if (link == NET_IF_LINK_UP) {

    ok = NetIPv6_GetAddrHost(1, addrTable, &addrTableSize, &err);

    if (!ok) {
      return;
    }

    int y = 40;
    for (int i = 0; i < addrTableSize; i++) {

      NetASCII_IPv6_to_Str(&addrTable[i], addrString, DEF_NO, DEF_NO, &err);
      APP_RTOS_ASSERT_CRITICAL((err.Code == RTOS_ERR_NONE), ;);

      /* IPv6 Address is too long to print on one line with narrow font */
      GLIB_drawString(&glibContext, &addrString[0], 20, 0, y, false);
      GLIB_drawString(&glibContext, &addrString[20], strlen(addrString) - 20, 0, y + 10, false);
      y += 20;
    }
  }
}

/***************************************************************************//**
 * @brief Draw Tx/Rx statistics
 ******************************************************************************/
static void GRAPHICS_DrawTxRxStatus(void)
{
  char str[20] = {0};
  static uint32_t framesTx = 0;
  static uint32_t framesRx = 0;

  framesTx += ETH->FRAMESTXED64;
  framesTx += ETH->FRAMESTXED65;
  framesTx += ETH->FRAMESTXED128;
  framesTx += ETH->FRAMESTXED256;
  framesTx += ETH->FRAMESTXED512;
  framesTx += ETH->FRAMESTXED1024;
  framesTx += ETH->FRAMESTXED1519;

  framesRx += ETH->FRAMESRXED64;
  framesRx += ETH->FRAMESRXED65;
  framesRx += ETH->FRAMESRXED128;
  framesRx += ETH->FRAMESRXED256;
  framesRx += ETH->FRAMESRXED512;
  framesRx += ETH->FRAMESRXED1024;
  framesRx += ETH->FRAMESRXED1519;

  snprintf(str, sizeof(str), "frames tx: %"PRIu32, framesTx);
  GRAPHICS_DrawString(str, 110);
  snprintf(str, sizeof(str), "frames rx: %"PRIu32, framesRx);
  GRAPHICS_DrawString(str, 120);
}
