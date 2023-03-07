#ifndef BSP_BUTTON_CONFIG_H
#define BSP_BUTTON_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <e BSP_USE_BUTTONS> Use buttons
// <i> Default: 1
#define BSP_USE_BUTTONS            1

// <o BSP_NO_OF_BUTTONS> Number of buttons
// <i> Default: 2
#define BSP_NO_OF_BUTTONS          2
// </e>

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>

// <gpio optional=true> BSP_GPIO_BUTTON0
// $[GPIO_BSP_GPIO_BUTTON0]
#define BSP_GPIO_BUTTON0_PORT      gpioPortF
#define BSP_GPIO_BUTTON0_PIN       6
// [GPIO_BSP_GPIO_BUTTON0]$

// <gpio optional=true> BSP_GPIO_BUTTON1
// $[GPIO_BSP_GPIO_BUTTON1]
#define BSP_GPIO_BUTTON1_PORT      gpioPortF
#define BSP_GPIO_BUTTON1_PIN       7
// [GPIO_BSP_GPIO_BUTTON1]$

// <<< sl:end pin_tool >>>

#if BSP_USE_BUTTONS
// Enable button API
#define BSP_GPIO_BUTTONS

#if BSP_NO_OF_BUTTONS == 1
#define BSP_GPIO_BUTTONARRAY_INIT                    \
  {                                                  \
    { BSP_GPIO_BUTTON0_PORT, BSP_GPIO_BUTTON0_PIN }, \
  }
#elif BSP_NO_OF_BUTTONS == 2
#define BSP_GPIO_BUTTONARRAY_INIT                    \
  {                                                  \
    { BSP_GPIO_BUTTON0_PORT, BSP_GPIO_BUTTON0_PIN }, \
    { BSP_GPIO_BUTTON1_PORT, BSP_GPIO_BUTTON1_PIN }, \
  }
#else
#error "This config file only knows how to configure up to 2 BUTTONs"
#endif // BSP_NO_OF_BUTTONS
#endif // BSP_USE_BUTTONS

#endif // BSP_BUTTON_CONFIG_H
