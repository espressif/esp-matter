/***************************************************************************//**
 * @file
 * @brief Code for Si72xx-EXP demos
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

#ifndef EXP_SI72XX_H_
#define EXP_SI72XX_H_

uint32_t EXP_SI72XX_placeSensorsInSleepMode(void);
uint32_t EXP_SI72XX_placeSensorsInLatchMode(void);

void EXP_SI72XX_runAngleDemo(void);
void EXP_SI72XX_runRevolutionCounterDemo(void);
void EXP_SI72XX_disableRevolutionCounterDemo(void);
void EXP_SI72XX_runMenuScreen (void);

#endif /* EXP_SI72XX_H_ */
