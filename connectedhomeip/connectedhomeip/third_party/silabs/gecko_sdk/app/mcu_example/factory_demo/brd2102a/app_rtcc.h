/***************************************************************************//**
 * @file
 * @brief Helper functions for timekeeping using the RTCC
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

#ifndef APP_RTCC_H
#define APP_RTCC_H

// Function prototypes
void setupRTCC(uint32_t resetVal);
uint32_t millis(void);

#endif /* APP_RTCC_H */
