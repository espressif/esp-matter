/***************************************************************************/
/**
 * @file
 * @brief CMSIS RTOS2 adaptation for running OpenThread in RTOS
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 *
 * https://www.silabs.com/about-us/legal/master-software-license-agreement
 *
 * This software is distributed to you in Source Code format and is governed by
 * the sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef OPENTHREAD_RTOS
#define OPENTHREAD_RTOS

void sl_ot_rtos_init(void);

void sl_ot_rtos_application_tick(void);

#endif