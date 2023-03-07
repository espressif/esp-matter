/***************************************************************************//**
 * @file
 * @brief Provide BSP (board support package) configuration parameters.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef BSPCONFIG_H
#define BSPCONFIG_H

#define BSP_STK
#define BSP_BRD4184B

#define BSP_BCC_USART         USART1
#define BSP_BCC_USART_INDEX   1
#define BSP_BCC_CLK           cmuClock_USART1
#define BSP_BCC_TXPORT        gpioPortA
#define BSP_BCC_TXPIN         5
#define BSP_BCC_RXPORT        gpioPortA
#define BSP_BCC_RXPIN         6

#define BSP_EXTFLASH_PRESENT  1

#define BSP_GPIO_LEDS
#define BSP_NO_OF_LEDS          1
#define BSP_GPIO_LED0_PORT      gpioPortA
#define BSP_GPIO_LED0_PIN       4
#define BSP_GPIO_LEDARRAY_INIT  { { BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN } }

#define BSP_GPIO_BUTTONS
#define BSP_NO_OF_BUTTONS       1
#define BSP_GPIO_PB0_PORT       gpioPortB
#define BSP_GPIO_PB0_PIN        3

#define BSP_GPIO_BUTTONARRAY_INIT { { BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN } }

#define BSP_INIT_DEFAULT        0

#if !defined(EMU_DCDCINIT_WSTK_DEFAULT)
/* Use emlib defaults */
#define EMU_DCDCINIT_WSTK_DEFAULT EMU_DCDCINIT_DEFAULT
#endif

/* The LFXO external crystal needs a load capacitance of 7.0 pF (CL=7.0 pF).
 * This means that we need 14 pF on each pin. Since this board has approximately
 * 4.5 pF parasitic capacitance on each pin we configure a 9.5 pF internal
 * capacitance
 *
 * (37+1) * 0.25 pF = 9.5 pF
 */
#define BSP_LFXO_CTUNE          37U
#define BSP_HFXO_CTUNE          120U

#if !defined(CMU_HFXOINIT_WSTK_DEFAULT)
/* HFXO initialization values for XTAL mode. */
#define CMU_HFXOINIT_WSTK_DEFAULT                                   \
  {                                                                 \
    cmuHfxoCbLsbTimeout_416us,                                      \
    cmuHfxoSteadyStateTimeout_833us,  /* First lock              */ \
    cmuHfxoSteadyStateTimeout_83us,   /* Subsequent locks        */ \
    0U,                         /* ctuneXoStartup                */ \
    0U,                         /* ctuneXiStartup                */ \
    32U,                        /* coreBiasStartup               */ \
    32U,                        /* imCoreBiasStartup             */ \
    cmuHfxoCoreDegen_None,                                          \
    cmuHfxoCtuneFixCap_Both,                                        \
    BSP_HFXO_CTUNE,             /* ctuneXoAna                    */ \
    BSP_HFXO_CTUNE,             /* ctuneXiAna                    */ \
    60U,                        /* coreBiasAna                   */ \
    false,                      /* enXiDcBiasAna                 */ \
    cmuHfxoOscMode_Crystal,                                         \
    false,                      /* forceXo2GndAna                */ \
    false,                      /* forceXi2GndAna                */ \
    false,                      /* DisOndemand                   */ \
    false,                      /* ForceEn                       */ \
    false                       /* Lock registers                */ \
  }
#endif

#if !defined(BSP_LFXOINIT_DEFAULT)
/* LFXO initialization values optimized for the board. */
#define BSP_LFXOINIT_DEFAULT                                                           \
  {                                                                                    \
    1,                            /* CL=7.0 pF which makes gain=1 a good value. */     \
    BSP_LFXO_CTUNE,               /* Optimal CTUNE value to get 32768 Hz. */           \
    cmuLfxoStartupDelay_2KCycles, /* Timeout before oscillation is stable. */          \
    cmuLfxoOscMode_Crystal,       /* Crystal mode */                                   \
    false,                        /* Disable high amplitude mode */                    \
    true,                         /* Enable AGC for automatic amplitude adjustment. */ \
    false,                        /* Disable failure detection in EM4. */              \
    false,                        /* Disable failure detection. */                     \
    false,                        /* LFXO starts on demand. */                         \
    false,                        /* LFXO starts on demand. */                         \
    false                         /* Don't lock registers.  */                         \
  }
#endif

#if !defined(RADIO_PTI_INIT)
#define RADIO_PTI_INIT                                                     \
  {                                                                        \
    RADIO_PTI_MODE_UART,    /* Simplest output mode is UART mode */        \
    1600000,                /* Choose 1.6 MHz for best compatibility */    \
    0,                      /* WSTK uses location x for DOUT */            \
    gpioPortC,              /* Get the port for this loc */                \
    4,                      /* Get the pin, location should match above */ \
    0,                      /* WSTK uses location x for DCLK */            \
    gpioPortB,              /* Get the port for this loc */                \
    11,                     /* Get the pin, location should match above */ \
    0,                      /* WSTK uses location x for DFRAME */          \
    gpioPortC,              /* Get the port for this loc */                \
    5,                      /* Get the pin, location should match above */ \
  }
#endif

#if !defined(RAIL_PTI_CONFIG)
#define RAIL_PTI_CONFIG                                                    \
  {                                                                        \
    RAIL_PTI_MODE_UART,     /* Simplest output mode is UART mode */        \
    1600000,                /* Choose 1.6 MHz for best compatibility */    \
    0,                      /* WSTK uses location x for DOUT */            \
    gpioPortC,              /* Get the port for this loc */                \
    4,                      /* Get the pin, location should match above */ \
    0,                      /* WSTK uses location x for DCLK */            \
    gpioPortB,              /* Get the port for this loc */                \
    11,                     /* Get the pin, location should match above */ \
    0,                      /* WSTK uses location x for DFRAME */          \
    gpioPortC,              /* Get the port for this loc */                \
    5,                      /* Get the pin, location should match above */ \
  }
#endif

#if !defined(RADIO_PA_2P4_INIT)
#define RADIO_PA_2P4_INIT                                    \
  {                                                          \
    PA_SEL_2P4_HP,    /* Power Amplifier mode */             \
    PA_VOLTMODE_VBAT, /* Power Amplifier vPA Voltage mode */ \
    60,              /* Desired output power in dBm * 10 */  \
    0,                /* Output power offset in dBm * 10 */  \
    2,                /* Desired ramp time in us */          \
  }
#endif

#if !defined(RAIL_PA_2P4_CONFIG)
#define RAIL_PA_2P4_CONFIG                                            \
  {                                                                   \
    RAIL_TX_POWER_MODE_2P4_HP, /* Power Amplifier mode */             \
    3300,                      /* Power Amplifier vPA Voltage mode */ \
    2,                         /* Desired ramp time in us */          \
  }
#endif

#if !defined(RAIL_PA_DEFAULT_POWER)
#define RAIL_PA_DEFAULT_POWER 60
#endif

/***************************************************************************//**
 * @defgroup BOARD_Config_Settings BOARD module configuration
 * @{
 * @brief BOARD module configuration macro definitions
 ******************************************************************************/

#define BOARD_LED_PORT            gpioPortA       /**< LED port                         */
#define BOARD_LED0_PORT           gpioPortA       /**< Red LED port                     */
#define BOARD_LED0_PIN            4               /**< Red LED pin                      */

#define BOARD_IMU_ENABLE_PORT       gpioPortB     /**< IMU enable port                  */
#define BOARD_IMU_ENABLE_PIN        4             /**< IMU enable pin                   */
#define BOARD_IMU_INT_PORT          gpioPortA     /**< IMU interrupt port               */
#define BOARD_IMU_INT_PIN           0             /**< IMU interrupt pin                */
#define BOARD_IMU_SPI_PORT          gpioPortC     /**< IMU SPI port                     */
#define BOARD_IMU_SPI_MOSI_PIN      0             /**< IMU SPI master out slave in pin  */
#define BOARD_IMU_SPI_MISO_PIN      1             /**< IMU SPI master in slave out pin  */
#define BOARD_IMU_SPI_SCLK_PIN      2             /**< IMU SPI serial clock pin         */
#define BOARD_IMU_SPI_CS_PIN        3             /**< IMU SPI chip select pin          */

#define BOARD_MIC_ENABLE_PORT       gpioPortC     /**< Microphone enable port           */
#define BOARD_MIC_ENABLE_PIN        7             /**< Microphone enable pin            */
#define BOARD_MIC_PDM_DATA_PORT     gpioPortB     /**< Microphone PDM data port         */
#define BOARD_MIC_PDM_DATA_PIN      1             /**< Microphone PDM data pin          */
#define BOARD_MIC_PDM_CLK_PORT      gpioPortB     /**< Microphone PDM clock port        */
#define BOARD_MIC_PDM_CLK_PIN       0             /**< Microphone PDM clock pin         */

#define BOARD_SENSOR_ENABLE_PORT       gpioPortC     /**< Environmental sensor enable port    */
#define BOARD_SENSOR_ENABLE_PIN        6             /**< Environmental sensor enable pin     */
#define BOARD_SENSOR_I2C_PORT          gpioPortD     /**< Environmental sensor I2C port       */
#define BOARD_SENSOR_I2C_SDA_PIN       2             /**< Environmental sensor I2C SDA pin    */
#define BOARD_SENSOR_I2C_SCL_PIN       3             /**< Environmental sensor I2C SCL pin    */

/* External interrupts */
#define EXTI_BUTTON0              1
#define EXTI_IMU_INT              3

#define BOARD_BUTTON_PORT          gpioPortB       /**< Pushbutton port                  */
#define BOARD_BUTTON_SHIFT         1               /**< Pushbutton shift value           */
#define BOARD_BUTTON0              0x01            /**< Left pushbutton value            */
#define BOARD_BUTTON_MASK          0x01            /**< Pushbutton mask                  */
#define BOARD_BUTTON0_PORT         gpioPortB       /**< Left pushbutton port             */
#define BOARD_BUTTON0_PIN          3               /**< Left pushbutton pin              */
#define BOARD_BUTTON_INT_FLAG      0x04            /**< Pushbutton interrupt flag value  */
#define BOARD_BUTTON_INT_ENABLE    true            /**< Pushbutton interrupt enable      */
#define BOARD_BUTTON0_EM4WUEN_MASK 0x08            /**< Mask to enable EM4 wake-up PB0   */

/** @} {end defgroup BOARD_Config_Setting} */

/* CMU settings */
#define BSP_CLK_HFXO_PRESENT                 (1)
#define BSP_CLK_HFXO_FREQ                    (38400000UL)
#define BSP_CLK_HFXO_INIT                     CMU_HFXOINIT_DEFAULT
#define BSP_CLK_HFXO_CTUNE                   (120)
#define BSP_CLK_LFXO_PRESENT                 (1)
#define BSP_CLK_LFXO_INIT                     CMU_LFXOINIT_DEFAULT
#define BSP_CLK_LFXO_FREQ                    (32768U)
#ifndef BSP_CLK_LFXO_CTUNE
  #define BSP_CLK_LFXO_CTUNE                 (37U)
#endif

/* DCDC settings */
#define BSP_DCDC_PRESENT                     (1)
#define BSP_DCDC_INIT                         EMU_DCDCINIT_DEFAULT

#define BSP_BCP_VERSION 2
#include "bsp_bcp.h"

#endif /* BSPCONFIG_H */
