/**
 * Provides support for BRD ZGM130s (FG13 radio board)
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef RADIO_BOARD_ZGM230S_H
#define RADIO_BOARD_ZGM230S_H

/*************************************************************************/
/* Configure RGB LED                                                     */
/*************************************************************************/

/* Don't use this RGB LED if an extension board has already defined its
 * own RGB LED
 */
#if !defined(RGB1_LABEL)
  #define RGB1_LABEL          "LED100"

  #define RGB1_R_GPIO_PORT    gpioPortB
  #define RGB1_R_GPIO_PIN     4
  #define RGB1_R_ON_VALUE     0

  #define RGB1_G_GPIO_PORT    gpioPortB
  #define RGB1_G_GPIO_PIN     5
  #define RGB1_G_ON_VALUE     0

  #define RGB1_B_GPIO_PORT    gpioPortB
  #define RGB1_B_GPIO_PIN     6
  #define RGB1_B_ON_VALUE     0
#endif

#endif /* RADIO_BOARD_ZGM230S_H */
