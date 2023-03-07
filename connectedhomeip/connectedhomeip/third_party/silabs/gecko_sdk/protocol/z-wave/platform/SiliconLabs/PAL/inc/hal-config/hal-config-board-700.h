/**
 * @file
 * @brief Hal config header file for 700 series
 * @copyright 2019 Silicon Laboratories Inc.
 */
#ifndef HAL_CONFIG_BOARD_700_H
#define HAL_CONFIG_BOARD_700_H

#include "em_device.h"
#include "hal-config-types.h"

// $[CMU]
#define HAL_CLK_HFCLK_SOURCE                  (HAL_CLK_HFCLK_SOURCE_HFXO)
#define HAL_CLK_HFXO_AUTOSTART                (HAL_CLK_HFXO_AUTOSTART_NONE)

#define HAL_CLK_LFECLK_SOURCE                 (HAL_CLK_LFCLK_SOURCE_LFRCO)
#define HAL_CLK_LFBCLK_SOURCE                 (HAL_CLK_LFCLK_SOURCE_LFRCO)
#define HAL_CLK_LFACLK_SOURCE                 (HAL_CLK_LFCLK_SOURCE_LFRCO)
#define BSP_CLK_LFXO_PRESENT                  (0)
#define BSP_CLK_LFXO_INIT                      CMU_LFXOINIT_DEFAULT
#define BSP_CLK_LFXO_CTUNE                    (0U)
#define BSP_CLK_LFXO_FREQ                     (32768U)
#define BSP_CLK_HFXO_PRESENT                  (1)
#define BSP_CLK_HFXO_FREQ                     (39000000UL)
#define BSP_CLK_HFXO_CTUNE                    (356U)
#define BSP_CLK_HFXO_INIT                     CMU_HFXOINIT_DEFAULT
// [CMU]$

// $[DCDC]
#define BSP_DCDC_PRESENT                      (1)

#define HAL_DCDC_BYPASS                       (0)
#define BSP_DCDC_INIT                          EMU_DCDCINIT_DEFAULT
// [DCDC]$

// $[EMU]
#define HAL_EMU_ENABLE                        (1)

//#define HAL_EMU_EM23_VSCALE                   (HAL_EMU_EM23_VSCALE_FASTWAKEUP)
#define HAL_EMU_EM23_VSCALE                   HAL_EMU_EM23_VSCALE_LOWPOWER
#define HAL_EMU_EM01_VSCALE                   (0)
// [EMU]$

// $[EZRADIOPRO]
// [EZRADIOPRO]$

// $[GPIO]
#define PORTIO_GPIO_SWV_PIN                   (2U)
#define PORTIO_GPIO_SWV_PORT                  (gpioPortF)
#define PORTIO_GPIO_SWV_LOC                   (0U)
// [GPIO]$

// $[LED]
#define BSP_LED_PRESENT                       (0)

#define BSP_LED0_PIN                          (4U)
#define BSP_LED0_PORT                         (gpioPortF)

#define BSP_LED1_PIN                          (5U)
#define BSP_LED1_PORT                         (gpioPortF)

#define HAL_LED_ENABLE                        { 0, 1 }
#define HAL_LED_COUNT                         (2U)
#define BSP_LED_COUNT                         (2U)
#define BSP_LED_INIT                          { { BSP_LED0_PORT, BSP_LED0_PIN }, { BSP_LED1_PORT, BSP_LED1_PIN } }
// [LED]$

// $[PA]
#define HAL_PA_ENABLE                         (1)

#define HAL_PA_RAMP                           (10UL)
#define HAL_PA_2P4_LOWPOWER                   (0)
#define HAL_PA_POWER                          (252U)

#define BSP_PA_VOLTAGE                        (1800U)

#define HAL_PA_VOLTAGE                        (1800U)
#define HAL_PA_CURVE_HEADER                    "pa_curves_efr32.h"
// [PA]$

// $[USART0]
#define PORTIO_USART0_CTS_PIN                 (2U)
#define PORTIO_USART0_CTS_PORT                (gpioPortA)
#define PORTIO_USART0_CTS_LOC                 (30U)

#define PORTIO_USART0_RTS_PIN                 (3U)
#define PORTIO_USART0_RTS_PORT                (gpioPortA)
#define PORTIO_USART0_RTS_LOC                 (30U)

#define PORTIO_USART0_RX_PIN                  (1U)
#define PORTIO_USART0_RX_PORT                 (gpioPortA)
#define PORTIO_USART0_RX_LOC                  (0U)

#define PORTIO_USART0_TX_PIN                  (0U)
#define PORTIO_USART0_TX_PORT                 (gpioPortA)
#define PORTIO_USART0_TX_LOC                  (0U)

#define HAL_USART0_ENABLE                     (0)

#define BSP_USART0_CTS_PIN                    (2U)
#define BSP_USART0_CTS_PORT                   (gpioPortA)
#define BSP_USART0_CTS_LOC                    (30U)

#define BSP_USART0_RX_PIN                     (1U)
#define BSP_USART0_RX_PORT                    (gpioPortA)
#define BSP_USART0_RX_LOC                     (0U)

#define BSP_USART0_TX_PIN                     (0U)
#define BSP_USART0_TX_PORT                    (gpioPortA)
#define BSP_USART0_TX_LOC                     (0U)

#define BSP_USART0_RTS_PIN                    (3U)
#define BSP_USART0_RTS_PORT                   (gpioPortA)
#define BSP_USART0_RTS_LOC                    (30U)

#define HAL_USART0_RX_QUEUE_SIZE              (128UL)
#define HAL_USART0_BAUD_RATE                  (115200UL)
#define HAL_USART0_RXSTOP                     (16UL)
#define HAL_USART0_RXSTART                    (16UL)
#define HAL_USART0_TX_QUEUE_SIZE              (128UL)
#define HAL_USART0_FLOW_CONTROL               (HAL_USART_FLOW_CONTROL_NONE)
// [USART0]$

// $[USART1]

#define PORTIO_USART1_CLK_PIN                 (8U)
#define PORTIO_USART1_CLK_PORT                (gpioPortC)
#define PORTIO_USART1_CLK_LOC                 (11U)

#define PORTIO_USART1_CS_PIN                  (9U)
#define PORTIO_USART1_CS_PORT                 (gpioPortC)
#define PORTIO_USART1_CS_LOC                  (11U)

#define PORTIO_USART1_RX_PIN                  (7U)
#define PORTIO_USART1_RX_PORT                 (gpioPortC)
#define PORTIO_USART1_RX_LOC                  (11U)

#define PORTIO_USART1_TX_PIN                  (6U)
#define PORTIO_USART1_TX_PORT                 (gpioPortC)
#define PORTIO_USART1_TX_LOC                  (11U)

#define HAL_USART1_ENABLE                     (1)

#define BSP_USART1_CS_PIN                     (9U)
#define BSP_USART1_CS_PORT                    (gpioPortC)
#define BSP_USART1_CS_LOC                     (11U)

#define BSP_USART1_CLK_PIN                    (8U)
#define BSP_USART1_CLK_PORT                   (gpioPortC)
#define BSP_USART1_CLK_LOC                    (11U)

#define BSP_USART1_MISO_PIN                   (7U)
#define BSP_USART1_MISO_PORT                  (gpioPortC)
#define BSP_USART1_MISO_LOC                   (11U)

#define BSP_USART1_MOSI_PIN                   (6U)
#define BSP_USART1_MOSI_PORT                  (gpioPortC)
#define BSP_USART1_MOSI_LOC                   (11U)

#define HAL_USART1_FREQUENCY                  (1000000UL)

// [USART1]$

#define BSP_ETM_TRACE              /* This board supports ETM trace. */
#define BSP_TRACE_ETM_CLKLOC     3 /* ETM_TCLK = PC6  */
#define BSP_TRACE_ETM_TD0LOC     3 /* ETM_TD0  = PC7  */
#define BSP_TRACE_ETM_TD1LOC     3 /* ETM_TD1  = PC8 */
#define BSP_TRACE_ETM_TD2LOC     3 /* ETM_TD2  = PC9 */
#define BSP_TRACE_ETM_TD3LOC     3 /* ETM_TD3  = PC10 */

#define BSP_TRACE_SWO_PIN                             (2U)
#define BSP_TRACE_SWO_PORT                            (gpioPortF)
#define BSP_TRACE_SWO_LOC                             (0U)

#endif /* HAL_CONFIG_BOARD_700_H */
