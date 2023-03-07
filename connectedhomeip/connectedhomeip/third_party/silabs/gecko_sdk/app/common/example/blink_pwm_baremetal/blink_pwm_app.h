/***************************************************************************//**
 * @file
 * @brief Blink PWM examples functions
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

#ifndef BLINK_PWM_H
#define BLINK_PWM_H

/***************************************************************************//**
 * Initialize blink PWM
 ******************************************************************************/
void blink_pwm_init(void);

/***************************************************************************//**
 * Blink PWM ticking function
 ******************************************************************************/
void blink_pwm_process_action(void);

#endif  // BLINK_PWM_H
