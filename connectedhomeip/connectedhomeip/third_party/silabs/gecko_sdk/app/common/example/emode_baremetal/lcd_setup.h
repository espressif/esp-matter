/***************************************************************************//**
 * @file
 * @brief Setup LCD for energy mode demo, header file
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef LCD_SETUP_H_
#define LCD_SETUP_H_

/***************************************************************************//**
 * Function to initialize memory lcd.
 ******************************************************************************/
void lcd_init(void);

/***************************************************************************//**
 * Ticking function to update the display and start the test.
 ******************************************************************************/
void emode_app_process_action();

#endif /* LCD_SETUP_H_ */
