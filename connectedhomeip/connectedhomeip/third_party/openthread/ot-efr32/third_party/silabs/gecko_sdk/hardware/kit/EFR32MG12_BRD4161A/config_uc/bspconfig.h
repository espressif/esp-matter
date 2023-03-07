#ifndef BSPCONFIG_H
#define BSPCONFIG_H

// This configuration file is for the STK BSP
#define BSP_STK

// BCC is not supported

// Default BSP init uses no flags
#define BSP_INIT_DEFAULT 0

// TODO: Get rid of these, use power/clock manager
#define EMU_DCDCINIT_WSTK_DEFAULT EMU_DCDCINIT_DEFAULT
#define CMU_HFXOINIT_WSTK_DEFAULT CMU_HFXOINIT_DEFAULT

// --------------------------------
// Feature enables

// <<< sl:start pin_tool >>>

// <gpio optional=true> BSP_VCOM_ENABLE
// $[GPIO_BSP_VCOM_ENABLE]
#define BSP_VCOM_ENABLE_PORT    gpioPortA
#define BSP_VCOM_ENABLE_PIN     5
// [GPIO_BSP_VCOM_ENABLE]$

// <gpio optional=true> BSP_DISP_ENABLE
// $[GPIO_BSP_DISP_ENABLE]
#define BSP_DISP_ENABLE_PORT    gpioPortD
#define BSP_DISP_ENABLE_PIN     15
// [GPIO_BSP_DISP_ENABLE]$

// <gpio optional=true> BSP_SENSOR_ENABLE
// $[GPIO_BSP_SENSOR_ENABLE]
#define BSP_SENSOR_ENABLE_PORT  gpioPortD
#define BSP_SENSOR_ENABLE_PIN   15
// [GPIO_BSP_SENSOR_ENABLE]$

// <<< sl:end pin_tool >>>

// Backwards compat wrapper for VCOM enable
#define BSP_BCC_ENABLE_PORT     BSP_VCOM_ENABLE_PORT
#define BSP_BCC_ENABLE_PIN      BSP_VCOM_ENABLE_PIN

// --------------------------------
// LEDs

#include "bsp_led_config.h"

// --------------------------------
// Buttons

#include "bsp_button_config.h"

// --------------------------------
// Radio PTI

#include "rail_pti_config.h"

// --------------------------------
// Radio PA

#include "rail_pa_config.h"

#endif // BSPCONFIG_H
