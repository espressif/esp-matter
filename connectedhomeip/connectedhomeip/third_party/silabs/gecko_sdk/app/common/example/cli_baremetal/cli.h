/***************************************************************************//**
 * @file
 * @brief cli baremetal examples functions
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

#ifndef CLI_BAREMETAL_H
#define CLI_BAREMETAL_H

/***************************************************************************//**
 * Initialize example
 ******************************************************************************/
void cli_app_init(void);

/***************************************************************************//**
 * Ticking function
 ******************************************************************************/
void cli_app_process_action();

#endif  // CLI_BAREMETAL_H
