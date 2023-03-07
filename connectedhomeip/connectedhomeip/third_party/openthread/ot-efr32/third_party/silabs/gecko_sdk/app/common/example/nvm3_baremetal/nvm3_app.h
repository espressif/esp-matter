/***************************************************************************//**
 * @file
 * @brief NVM3 examples functions
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

#ifndef NVM3_APP_H
#define NVM3_APP_H

/***************************************************************************//**
 * Initialize NVM3 example
 ******************************************************************************/
void nvm3_app_init(void);

/***************************************************************************//**
 * NVM3 ticking function
 ******************************************************************************/
void nvm3_app_process_action(void);

#endif  // NVM3_APP_H
