/**
 * Provides support for BRD8029A (Buttons and LEDs EXP Board)
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef EXTENSION_BOARD_8029A_EFR32ZG23_H
#define EXTENSION_BOARD_8029A_EFR32ZG23_H

/*************************************************************************/
/* Configure LEDs                                                        */
/*************************************************************************/

/* NB: Mounted in parallel with "LED0" on the mainboard.
 *
 *     EFR32 peripheral: (none)
 */
#define LED1_LABEL           "LED0"
#define LED1_GPIO_PORT       gpioPortA
#define LED1_GPIO_PIN        6
#define LED1_ON_VALUE        1
#define LED1_LETIM0_OUT0_LOC 0x60000

#define LED2_LABEL           "LED1"
#define LED2_GPIO_PORT       gpioPortA
#define LED2_GPIO_PIN        7
#define LED2_ON_VALUE        1
#define LED2_LETIM0_OUT0_LOC 0x70000

#define LED3_LABEL           "LED2"
#define LED3_GPIO_PORT       gpioPortA
#define LED3_GPIO_PIN        10
#define LED3_ON_VALUE        1
#define LED3_LETIM0_OUT0_LOC 29

/* NB: EFR32 peripheral: US0_CS#0
 */
#define LED4_LABEL           "LED3"
#define LED4_GPIO_PORT       gpioPortA
#define LED4_GPIO_PIN        0
#define LED4_ON_VALUE        1
#define LED4_LETIM0_OUT0_LOC 3


/*************************************************************************/
/* Configure RGB LEDs                                                    */
/*************************************************************************/

/* BRD8029A does not have any RGB led!
 * If paired with radio board ZGM13 then the RGB on that board can be used
 */

/*************************************************************************/
/* Configure push buttons                                                */
/*************************************************************************/

/* NB: BTN0 on BRD8029A is connected in parallel with PB0 on BRD4001A
 *     (Wireless Starter Kit mainboard). Not a problem, the two buttons
 *     simply provide the same functionality.
 */
#define PB1_LABEL           "BTN0"
#define PB1_GPIO_PORT       gpioPortA
#define PB1_GPIO_PIN        5
#define PB1_ON_VALUE        0
#define PB1_INT_NO          PB1_GPIO_PIN
#define PB1_CAN_WAKEUP_EM4  true

/* NB: BTN1 on BRD8029A is connected in parallel with PB1 on BRD4001A
 *     (Wireless Starter Kit mainboard). Not a problem, the two buttons
 *     simply provide the same functionality.
 */
#define PB2_LABEL           "BTN1"
#define PB2_GPIO_PORT       gpioPortD
#define PB2_GPIO_PIN        2
#define PB2_ON_VALUE        0
#define PB2_INT_NO          PB2_GPIO_PIN
#define PB2_CAN_WAKEUP_EM4  true

#define PB3_LABEL           "BTN2"
#define PB3_GPIO_PORT       gpioPortC
#define PB3_GPIO_PIN        5
#define PB3_ON_VALUE        0
#define PB3_INT_NO          4
#define PB3_CAN_WAKEUP_EM4  true

#define PB4_LABEL           "BTN3"
#define PB4_GPIO_PORT       gpioPortC
#define PB4_GPIO_PIN        7
#define PB4_ON_VALUE        0
#define PB4_INT_NO          PB4_GPIO_PIN
#define PB4_CAN_WAKEUP_EM4  true

/*************************************************************************/
/* Configure slider button                                               */
/*************************************************************************/

/* NB: SLIDER1 GPIO (PC0) conflicts with  a CS line (US1_CS#11)!!
 *     UART1 should not be used with this board.
 */
#define SLIDER1_LABEL          "SW1"
#define SLIDER1_GPIO_PORT      gpioPortC
#define SLIDER1_GPIO_PIN       0
#define SLIDER1_CAN_WAKEUP_EM4 false
#define SLIDER1_ON_VALUE       0
#define SLIDER1_INT_NO         SLIDER1_GPIO_PIN

/*************************************************************************/
/* Map physical board IO devices to application LEDs and buttons         */
/*************************************************************************/

/* Map application LEDs to board LEDs */
#define APP_LED_A              BOARD_LED1
#define APP_LED_INDICATOR      BOARD_LED2  // Positioned opposite APP_BUTTON_LEARN_RESET
#define APP_LED_B              BOARD_LED4
#define APP_LED_C              BOARD_LED3  // LED3 is currently "LED1" on the main board

#define APP_RGB_R              BOARD_RGB1_R
#define APP_RGB_G              BOARD_RGB1_G
#define APP_RGB_B              BOARD_RGB1_B

/* Mapping application buttons to board buttons */
#define APP_BUTTON_A           BOARD_BUTTON_PB1
#define APP_BUTTON_LEARN_RESET BOARD_BUTTON_PB2  // Supports EM4 wakeup
#define APP_BUTTON_B           BOARD_BUTTON_PB3  // Supports EM4 wakeup
#define APP_BUTTON_C           BOARD_BUTTON_PB4
#define APP_SLIDER_A           BOARD_BUTTON_SLIDER1

/* The next two are identical since on the BRD8029A only PB2 and PB3
 * can trigger a wakeup from EM4. PB2 is already used for learn/reset
 */
#define APP_WAKEUP_BTN_SLDR    BOARD_BUTTON_PB3 // Use this one when wakeup capability is required and button is preferred to slider
#define APP_WAKEUP_SLDR_BTN    BOARD_BUTTON_PB3 // Use this one when wakeup capability is required and slider is preferred to button

#endif /* EXTENSION_BOARD_8029A_EFR32ZG23_H */
