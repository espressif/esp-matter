/***************************************************************************//**
 * @file btl_xmodem_config.h
 * @brief Status Uart Xmodem Functions Configuration - Configuration Template File
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

/*********************************************************************************************************
 *********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  BTL_XMODEM_CONFIG_H
#define  BTL_XMODEM_CONFIG_H

// <h> Options:

/********************************************************************************************************
 ********************************************************************************************************
 *                                             XMODEM UART CONFIGURATION
 *********************************************************************************************************
 ********************************************************************************************************/
// <o BTL_XMODEM_IDLE_TIMEOUT> Menu idle timeout (seconds)[0-60]
// <0-60:1>
// <i> Default: 0
// <i> If the host hasn't initiated communication with the bootloader within this window,
// <i> the bootloader will abort the firmware upgrade process and return to the application.
#define BTL_XMODEM_IDLE_TIMEOUT  0


// </h>

#endif // End of BTL_XMODEM_CONFIG_H module include.

// <<< end of configuration section >>>
