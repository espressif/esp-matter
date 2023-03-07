/***************************************************************************//**
 * @file
 * @brief MPU Simple example functions
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

#ifndef MPU_SIMPLE_H
#define MPU_SIMPLE_H

/***************************************************************************//**
 * Initialize MPU Simple
 ******************************************************************************/
void mpu_simple_init(void);

/***************************************************************************//**
 * MPU Simple ticking function
 ******************************************************************************/
void mpu_simple_process_action(void);

#endif  // MPU_SIMPLE_H
