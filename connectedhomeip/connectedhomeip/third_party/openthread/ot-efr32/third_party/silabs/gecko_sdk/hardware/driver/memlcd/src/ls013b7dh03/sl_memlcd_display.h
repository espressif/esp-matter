/***************************************************************************//**
 * @file
 * @brief Sharp Memory LCD LS013B7DH03 driver
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
#ifndef SL_MEMLCD_DISPLAY_H
#define SL_MEMLCD_DISPLAY_H

#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SL_MEMLCD_LS013B7DH03             1

#define SL_MEMLCD_DISPLAY_WIDTH           128
#define SL_MEMLCD_DISPLAY_HEIGHT          128
#define SL_MEMLCD_DISPLAY_BPP             1
#define SL_MEMLCD_DISPLAY_RGB_3BIT        0

/* Max supported frequency is 1.1 MHz */
#define SL_MEMLCD_SCLK_FREQ               1100000

/* EXTCOMIN pin signal must have a frequency in the range of 54-65 Hz */
#define SL_MEMLCD_EXTCOMIN_FREQUENCY      60

#define SL_MEMLCD_SCS_SETUP_US            6
#define SL_MEMLCD_SCS_HOLD_US             2

/**************************************************************************//**
 * @brief
 *   Initialization function for the LS013B7DH03 device driver.
 *
 * @return
 *   If all operations completed sucessfully SL_STATUS_OK is returned. On
 *   failure a different status code is returned specifying the error.
 *****************************************************************************/
sl_status_t sl_memlcd_init(void);

#ifdef __cplusplus
}
#endif

#endif
