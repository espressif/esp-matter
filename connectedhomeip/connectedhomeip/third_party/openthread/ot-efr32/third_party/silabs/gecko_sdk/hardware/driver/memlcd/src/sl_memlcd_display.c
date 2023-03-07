/***************************************************************************//**
 * @file
 * @brief Sharp Memory LCD driver
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_memlcd_display.h"

sl_status_t sl_memlcd_init(void)
{
  sl_memlcd_t memlcd = {
    .width = SL_MEMLCD_DISPLAY_WIDTH,
    .height = SL_MEMLCD_DISPLAY_HEIGHT,
    .bpp = SL_MEMLCD_DISPLAY_BPP,
    .color_mode = SL_MEMLCD_COLOR_MODE_MONOCHROME,
    .spi_freq = SL_MEMLCD_SCLK_FREQ,
    .extcomin_freq = SL_MEMLCD_EXTCOMIN_FREQUENCY,
    .setup_us = SL_MEMLCD_SCS_SETUP_US,
    .hold_us = SL_MEMLCD_SCS_HOLD_US,
  };

  return sl_memlcd_configure(&memlcd);
}
