/**
 * Provides support for BRD4001A (Wireless Starter Kit Mainboard)
 *
 * Provides support for the limited number of LEDs and buttons on the WSK mainboard.
 * Only use in case a button extension board is not used.
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef EXTENSION_BOARD_4001A_H
#define EXTENSION_BOARD_4001A_H

/*************************************************************************/
/* Configure LEDs                                                        */
/*************************************************************************/

#define LED1_LABEL           "LED0"
#define LED1_GPIO_PORT       gpioPortF
#define LED1_GPIO_PIN        4
#define LED1_ON_VALUE        1
#define LED1_LETIM0_OUT0_LOC 28

#define LED2_LABEL           "LED1"
#define LED2_GPIO_PORT       gpioPortF
#define LED2_GPIO_PIN        5
#define LED2_ON_VALUE        1
#define LED2_LETIM0_OUT0_LOC 29

/*************************************************************************/
/* Configure push buttons                                                */
/*************************************************************************/

#define PB1_LABEL           "PB0"
#define PB1_GPIO_PORT       gpioPortF
#define PB1_GPIO_PIN        6
#define PB1_ON_VALUE        0
#define PB1_CAN_WAKEUP_EM4  false

#define PB2_LABEL           "PB1"
#define PB2_GPIO_PORT       gpioPortF
#define PB2_GPIO_PIN        7
#define PB2_ON_VALUE        0
#define PB2_CAN_WAKEUP_EM4  true

/*************************************************************************/
/* Map physical board IO devices to application LEDs and buttons         */
/*************************************************************************/

/* Map application LEDs to board LEDs */
#define APP_LED_INDICATOR      BOARD_LED1
#define APP_LED_A              BOARD_LED2

/* Mapping application buttons to board buttons */
#define APP_BUTTON_A           BOARD_BUTTON_PB1
#define APP_BUTTON_LEARN_RESET BOARD_BUTTON_PB2 // Supports EM4 wakeup

#endif /* EXTENSION_BOARD_4001A_H */
