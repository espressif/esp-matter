/***************************************************************************//**
 * @file
 * @brief Code for Si72xx-EXP calibration
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

#ifndef EXP_SI72XX_CALIBRATION_H_
#define EXP_SI72XX_CALIBRATION_H_

void CAL_runAngleCalibration(void);
int16_t CAL_getValue (int device, int index);

#endif /* EXP_SI72XX_CALIBRATION_H_ */
