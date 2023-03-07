#ifndef BSPHALCONFIG_H
#define BSPHALCONFIG_H

#include "hal-config.h"

// HAL config is only supported on STK platform
#define BSP_STK

#ifdef BSP_LED_PRESENT
// HAL Config LED module enabled
#define BSP_GPIO_LEDS
#define BSP_NO_OF_LEDS                BSP_LED_COUNT
#define BSP_GPIO_LEDARRAY_INIT        BSP_LED_INIT
#endif

#ifdef BSP_EXTLED_PRESENT
// HAL Config EXTLED module enabled
#define BSP_GPIO_LEDS
#define BSP_NO_OF_LEDS                BSP_EXTLED_COUNT
#define BSP_GPIO_EXTLEDARRAY_INIT     BSP_EXTLED_INIT
#endif

#ifdef BSP_BUTTON_PRESENT
// HAL Config Button module enabled
#define BSP_GPIO_BUTTONS
#define BSP_NO_OF_BUTTONS             BSP_BUTTON_COUNT
#define BSP_GPIO_BUTTONARRAY_INIT     BSP_BUTTON_INIT
#endif

#ifdef HAL_IOEXP_ENABLE
#define BSP_IO_EXPANDER               1
#if (BSP_IOEXP_PERIPHERAL == HAL_I2C_PORT_I2C0)
#define BSP_IOEXP_I2C_DEVICE          I2C0
#elif (BSP_IOEXP_PERIPHERAL == HAL_I2C_PORT_I2C1)
#define BSP_IOEXP_I2C_DEVICE          I2C1
#elif (BSP_IOEXP_PERIPHERAL == HAL_I2C_PORT_I2C2)
#define BSP_IOEXP_I2C_DEVICE          I2C2
#elif (BSP_IOEXP_PERIPHERAL == HAL_I2C_PORT_I2C3)
#define BSP_IOEXP_I2C_DEVICE          I2C3
#else
#error "I2C expander peripheral not configured"
#endif

#endif

#if HAL_PTI_ENABLE && !defined(RAIL_PTI_CONFIG)

// Series 2 devices do not have LOC settings, so HWCONF will not define the
// following macros for those devices. The RAIL structure, on the other hand,
// stayed the same for backwards compatibility reasons. RAIL will ignore the
// LOC setting for series 2 devices, so we give them any value here to avoid
// compilation errors.
#if defined(_SILICON_LABS_32B_SERIES_2)
#define BSP_PTI_DFRAME_LOC 0
#define BSP_PTI_DOUT_LOC 0
#define BSP_PTI_DCLK_LOC 0
#endif

#if HAL_PTI_MODE == HAL_PTI_MODE_SPI
#define RAIL_PTI_CONFIG                                                   \
  {                                                                       \
    RAIL_PTI_MODE_SPI,     /* SPI mode */                                 \
    HAL_PTI_BAUD_RATE,     /* Baud rate */                                \
    BSP_PTI_DOUT_LOC,      /* DOUT location */                            \
    BSP_PTI_DOUT_PORT,     /* Get the port for this loc */                \
    BSP_PTI_DOUT_PIN,      /* Get the pin, location should match above */ \
    BSP_PTI_DCLK_LOC,      /* DCLK location */                            \
    BSP_PTI_DCLK_PORT,     /* Get the port for this loc */                \
    BSP_PTI_DCLK_PIN,      /* Get the pin, location should match above */ \
    BSP_PTI_DFRAME_LOC,    /* DFRAME location */                          \
    BSP_PTI_DFRAME_PORT,   /* Get the port for this loc */                \
    BSP_PTI_DFRAME_PIN,    /* Get the pin, location should match above */ \
  }
#elif HAL_PTI_MODE == HAL_PTI_MODE_UART
#define RAIL_PTI_CONFIG                                                   \
  {                                                                       \
    RAIL_PTI_MODE_UART,    /* UART mode */                                \
    HAL_PTI_BAUD_RATE,     /* Baud rate */                                \
    BSP_PTI_DOUT_LOC,      /* DOUT location */                            \
    BSP_PTI_DOUT_PORT,     /* Get the port for this loc */                \
    BSP_PTI_DOUT_PIN,      /* Get the pin, location should match above */ \
    0,                     /* No DCLK in UART mode */                     \
    0,                     /* No DCLK in UART mode */                     \
    0,                     /* No DCLK in UART mode */                     \
    BSP_PTI_DFRAME_LOC,    /* DFRAME location */                          \
    BSP_PTI_DFRAME_PORT,   /* Get the port for this loc */                \
    BSP_PTI_DFRAME_PIN,    /* Get the pin, location should match above */ \
  }
#elif HAL_PTI_MODE == HAL_PTI_MODE_UART_ONEWIRE
#define RAIL_PTI_CONFIG                                                        \
  {                                                                            \
    RAIL_PTI_MODE_UART_ONEWIRE, /* UART onewire mode */                        \
    HAL_PTI_BAUD_RATE,          /* Baud rate */                                \
    BSP_PTI_DOUT_LOC,           /* DOUT location */                            \
    BSP_PTI_DOUT_PORT,          /* Get the port for this loc */                \
    BSP_PTI_DOUT_PIN,           /* Get the pin, location should match above */ \
    0,                          /* No DCLK in UART onewire mode */             \
    0,                          /* No DCLK in UART onewire mode */             \
    0,                          /* No DCLK in UART onewire mode */             \
    0,                          /* No DFRAME in UART onewire mode */           \
    0,                          /* No DFRAME in UART onewire mode */           \
    0,                          /* No DFRAME in UART onewire mode */           \
  }
#else
  #error "Invalid PTI mode (HAL_PTI_MODE)"
#endif
#endif

#if !defined(RAIL_PA_2P4_CONFIG)
// HAL Config 2.4 GHz PA configuration enabled
#define RAIL_PA_2P4_CONFIG                                               \
  {                                                                      \
    RAIL_TX_POWER_MODE_2P4_HP,    /* Power Amplifier mode */             \
    BSP_PA_VOLTAGE,               /* Power Amplifier vPA Voltage mode */ \
    HAL_PA_RAMP,                  /* Desired ramp time in us */          \
  }
#endif

#if !defined(RAIL_PA_SUBGIG_CONFIG)
// HAL Config sub-GHz PA configuration enabled
#define RAIL_PA_SUBGIG_CONFIG                                            \
  {                                                                      \
    RAIL_TX_POWER_MODE_SUBGIG,    /* Power Amplifier mode */             \
    BSP_PA_VOLTAGE,               /* Power Amplifier vPA Voltage mode */ \
    HAL_PA_RAMP,                  /* Desired ramp time in us */          \
  }
#endif

#if defined(HAL_PA_POWER) && !defined(RAIL_PA_DEFAULT_POWER)
#define RAIL_PA_DEFAULT_POWER         HAL_PA_POWER
#endif

#if defined(HAL_PA_CURVE_HEADER) && !defined(RAIL_PA_CURVES)
#define RAIL_PA_CURVES                HAL_PA_CURVE_HEADER
#endif

#define BSP_BCP_VERSION 2
#include "bsp_bcp.h"

#endif // BSPHALCONFIG_H
