/**
 * Provides support for BRD2603A (ZGM230S radio board)
 *
 * @copyright 2022 Silicon Laboratories Inc.
 */

#ifndef RADIO_BOARD_BRD2603A_H
#define RADIO_BOARD_BRD2603A_H

/*************************************************************************/
/* Configure LEDs                                                        */
/*************************************************************************/

#define LED1_LABEL           "LED0"
#define LED1_GPIO_PORT       gpioPortC
#define LED1_GPIO_PIN        8
#define LED1_ON_VALUE        1
/** 
 * @note This LED is on port C which is not available for LETIMER0. 
 * Lets make the GREEN RGB blink instead
 * Original code: #define LED1_LETIM0_OUT0_LOC 0x80002
 */
#define LED1_LETIM0_OUT0_LOC 0x00000

#define LED2_LABEL           "LED1"
#define LED2_GPIO_PORT       gpioPortC
#define LED2_GPIO_PIN        9
#define LED2_ON_VALUE        1
/** 
 * @note This LED is on port C which is not available for LETIMER0. 
 * Lets make the RED RGB blink instead
 * Original code: #define LED2_LETIM0_OUT0_LOC 0x90002
 */
#define LED2_LETIM0_OUT0_LOC 0x10001

/*************************************************************************/
/* Configure RGB LEDs                                                    */
/*************************************************************************/

#define RGB1_LABEL          "LED100"

#define RGB1_R_GPIO_PORT    gpioPortB
#define RGB1_R_GPIO_PIN     1
#define RGB1_R_ON_VALUE     0

#define RGB1_G_GPIO_PORT    gpioPortA
#define RGB1_G_GPIO_PIN     0
#define RGB1_G_ON_VALUE     0

#define RGB1_B_GPIO_PORT    gpioPortC
#define RGB1_B_GPIO_PIN     4
#define RGB1_B_ON_VALUE     0

/*************************************************************************/
/* Configure push buttons                                                */
/*************************************************************************/

#define PB1_LABEL           "BTN0"
#define PB1_GPIO_PORT       gpioPortB
#define PB1_GPIO_PIN        3
#define PB1_ON_VALUE        0
#define PB1_INT_NO          PB1_GPIO_PIN
#define PB1_CAN_WAKEUP_EM4  true

#define PB2_LABEL           "BTN1"
#define PB2_GPIO_PORT       gpioPortB
#define PB2_GPIO_PIN        2
#define PB2_ON_VALUE        0
#define PB2_INT_NO          PB2_GPIO_PIN
#define PB2_CAN_WAKEUP_EM4  false

/*************************************************************************/
/* Configure slider button                                               */
/*************************************************************************/

/*************************************************************************/
/* Map physical board IO devices to application LEDs and buttons         */
/*************************************************************************/

#define APP_LED_A         BOARD_LED1
#define APP_LED_INDICATOR BOARD_LED2  // Positioned opposite APP_BUTTON_LEARN_RESET

#define APP_BUTTON_A           BOARD_BUTTON_PB1 // Supports EM4 wakeup
#define APP_BUTTON_LEARN_RESET BOARD_BUTTON_PB2

#define APP_WAKEUP_BTN_SLDR    BOARD_BUTTON_PB1 // Use this one when wakeup capability is required and button is preferred to slider
#define APP_WAKEUP_SLDR_BTN    BOARD_BUTTON_PB1 // Use this one when wakeup capability is required and slider is preferred to button

#endif /* RADIO_BOARD_BRD2603A_H */
