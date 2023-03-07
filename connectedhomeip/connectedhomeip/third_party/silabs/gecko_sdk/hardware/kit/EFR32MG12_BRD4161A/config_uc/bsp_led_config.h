#ifndef BSP_LED_CONFIG_H
#define BSP_LED_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <e BSP_USE_LEDS> Use LEDs
// <i> Default: 1
#define BSP_USE_LEDS            1

// <o BSP_NO_OF_LEDS> Number of LEDs
// <i> Default: 2
#define BSP_NO_OF_LEDS          2
// </e>

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>

// <gpio optional=true> BSP_GPIO_LED0
// $[GPIO_BSP_GPIO_LED0]
#define BSP_GPIO_LED0_PORT      gpioPortF
#define BSP_GPIO_LED0_PIN       4
// [GPIO_BSP_GPIO_LED0]$

// <gpio optional=true> BSP_GPIO_LED1
// $[GPIO_BSP_GPIO_LED1]
#define BSP_GPIO_LED1_PORT      gpioPortF
#define BSP_GPIO_LED1_PIN       5
// [GPIO_BSP_GPIO_LED1]$

// <<< sl:end pin_tool >>>

#if BSP_USE_LEDS
// Enable LED API
#define BSP_GPIO_LEDS

#if BSP_NO_OF_LEDS == 1
#define BSP_GPIO_LEDARRAY_INIT                 \
  {                                            \
    { BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN }, \
  }
#elif BSP_NO_OF_LEDS == 2
#define BSP_GPIO_LEDARRAY_INIT                 \
  {                                            \
    { BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN }, \
    { BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN }, \
  }
#else
#error "This config file only knows how to configure up to 2 LEDs"
#endif // BSP_NO_OF_LEDS
#endif // BSP_USE_LEDS

#endif // BSP_LED_CONFIG_H
