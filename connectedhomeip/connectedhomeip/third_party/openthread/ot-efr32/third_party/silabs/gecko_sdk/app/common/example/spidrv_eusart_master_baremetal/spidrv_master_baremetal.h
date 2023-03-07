/***************************************************************************//**
 * @file
 * @brief spidrv baremetal examples functions
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

#ifndef SPIDRV_BAREMETAL_H
#define SPIDRV_BAREMETAL_H

/***************************************************************************//**
 * Initialize example
 ******************************************************************************/
void spidrv_app_init(void);

/***************************************************************************//**
 * Ticking function
 ******************************************************************************/
void spidrv_app_process_action();

#endif  // SPIDRV_BAREMETAL_H
