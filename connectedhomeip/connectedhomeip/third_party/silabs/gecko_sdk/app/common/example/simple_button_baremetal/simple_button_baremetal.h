/***************************************************************************//**
 * @file
 * @brief Simple button baremetal examples functions
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

#ifndef SIMPLE_BUTTON_BAREMETAL_H
#define SIMPLE_BUTTON_BAREMETAL_H

/***************************************************************************//**
 * Initialize example
 ******************************************************************************/
void simple_button_init(void);

/***************************************************************************//**
 * ticking function
 ******************************************************************************/
void simple_button_process_action(void);

#endif  // SIMPLE_BUTTON_BAREMETAL_H
