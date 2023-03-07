/***************************************************************************//**
 * @file
 * @brief mbedTLS ECDH example functions
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

#ifndef APP_ECDH_H
#define APP_ECDH_H

/***************************************************************************//**
 * Initialize ECDH example
 ******************************************************************************/
void app_ecdh_init(void);

/***************************************************************************//**
 * ECDH process function
 ******************************************************************************/
void app_ecdh_process_action(void);

#endif // APP_ECDH_H
