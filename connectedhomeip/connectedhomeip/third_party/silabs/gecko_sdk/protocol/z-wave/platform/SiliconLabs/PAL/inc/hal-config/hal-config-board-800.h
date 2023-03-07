/**
 * @file
 * @brief Hal config header file for 800 series
 * @copyright 2019 Silicon Laboratories Inc.
 */
#ifndef HAL_CONFIG_BOARD_H
#define HAL_CONFIG_BOARD_H

#include "em_device.h"
#include "hal-config-types.h"
/*The 800 series board configuration is based on the brd4204c board*/

// $[BTL_BUTTON]
#define BSP_BTL_BUTTON_PIN                            (0U)
#define BSP_BTL_BUTTON_PORT                           (gpioPortB)
// [BTL_BUTTON]$

// $[BUTTON]
#define BSP_BUTTON_PRESENT                            (1)

#define BSP_BUTTON0_PIN                               (1U)
#define BSP_BUTTON0_PORT                              (gpioPortB)

#define BSP_BUTTON1_PIN                               (3U)
#define BSP_BUTTON1_PORT                              (gpioPortB)

#define BSP_BUTTON_COUNT                              (2U)
#define BSP_BUTTON_INIT                               { { BSP_BUTTON0_PORT, BSP_BUTTON0_PIN }, { BSP_BUTTON1_PORT, BSP_BUTTON1_PIN } }
#define BSP_BUTTON_GPIO_DOUT                          (HAL_GPIO_DOUT_LOW)
#define BSP_BUTTON_GPIO_MODE                          (HAL_GPIO_MODE_INPUT)
// [BUTTON]$

// $[CMU]
#define HAL_CLK_EM23CLK_SOURCE                (HAL_CLK_LFCLK_SOURCE_LFRCO)
#define HAL_CLK_HFCLK_SOURCE                  (HAL_CLK_HFCLK_SOURCE_HFXO)
#define HAL_CLK_HFXO_AUTOSTART                (HAL_CLK_HFXO_AUTOSTART_NONE)

#define HAL_CLK_HFCLK_SOURCE                  (HAL_CLK_HFCLK_SOURCE_HFXO)
#define HAL_CLK_HFXO_AUTOSTART                (HAL_CLK_HFXO_AUTOSTART_NONE)
#define BSP_CLK_HFXO_PRESENT                          (1)
#define BSP_CLK_HFXO_FREQ                             (39000000UL)
#define BSP_CLK_HFXO_INIT                              CMU_HFXOINIT_DEFAULT
#define BSP_CLK_HFXO_CTUNE                            (120)
#define BSP_CLK_LFXO_PRESENT                          (0)
#define BSP_CLK_LFXO_INIT                              CMU_LFXOINIT_DEFAULT
#define BSP_CLK_LFXO_FREQ                             (32768U)
#define BSP_CLK_LFXO_CTUNE                            (37U)
// [CMU]$

// $[DCDC]
#define BSP_DCDC_PRESENT                      (1)
#define HAL_DCDC_BYPASS                       (0)
#define BSP_DCDC_INIT                         EMU_DCDCINIT_DEFAULT
// [DCDC]$

// $[EMU]
#define HAL_EMU_ENABLE                        (1)
//#define HAL_EMU_EM23_VSCALE                   (HAL_EMU_EM23_VSCALE_FASTWAKEUP)
#define HAL_EMU_EM23_VSCALE                   HAL_EMU_EM23_VSCALE_LOWPOWER
#define HAL_EMU_EM01_VSCALE                   (0)
// [EMU]$


// $[EUSART0]
#define PORTIO_EUSART0_RX_PIN                          (9U)
#define PORTIO_EUSART0_RX_PORT                         (gpioPortA)

#define PORTIO_EUSART0_TX_PIN                          (8U)
#define PORTIO_EUSART0_TX_PORT                         (gpioPortA)

#define BSP_EUSART0_TX_PIN                             (8U)
#define BSP_EUSART0_TX_PORT                            (gpioPortA)

#define BSP_EUSART0_RX_PIN                             (9U)
#define BSP_EUSART0_RX_PORT                            (gpioPortA)
// [EUSART0]$

// $[EUSART1]
#define PORTIO_EUSART1_RX_PIN                          (2U)
#define PORTIO_EUSART1_RX_PORT                         (gpioPortC)

#define PORTIO_EUSART1_TX_PIN                          (1U)
#define PORTIO_EUSART1_TX_PORT                         (gpioPortC)

// [EUSART1]$

// $[EXTFLASH]
#define BSP_EXTFLASH_CS_PIN                           (4U)
#define BSP_EXTFLASH_CS_PORT                          (gpioPortC)

#define BSP_EXTFLASH_INTERNAL                         (0)
#define BSP_EXTFLASH_USART                            (HAL_SPI_PORT_USART1)
#define BSP_EXTFLASH_MOSI_PIN                         (1U)
#define BSP_EXTFLASH_MOSI_PORT                        (gpioPortC)

#define BSP_EXTFLASH_MISO_PIN                         (2U)
#define BSP_EXTFLASH_MISO_PORT                        (gpioPortC)

#define BSP_EXTFLASH_CLK_PIN                          (3U)
#define BSP_EXTFLASH_CLK_PORT                         (gpioPortC)
// [EXTFLASH]$


// $[GPIO]
#define PORTIO_GPIO_SWV_PIN                           (3U)
#define PORTIO_GPIO_SWV_PORT                          (gpioPortA)

#define BSP_TRACE_SWO_PIN                             (3U)
#define BSP_TRACE_SWO_PORT                            (gpioPortA)

// [GPIO]$

// $[I2C0]
#define PORTIO_I2C0_SCL_PIN                           (5U)
#define PORTIO_I2C0_SCL_PORT                          (gpioPortC)

#define PORTIO_I2C0_SDA_PIN                           (7U)
#define PORTIO_I2C0_SDA_PORT                          (gpioPortC)

#define BSP_I2C0_SCL_PIN                              (5U)
#define BSP_I2C0_SCL_PORT                             (gpioPortC)

#define BSP_I2C0_SDA_PIN                              (7U)
#define BSP_I2C0_SDA_PORT                             (gpioPortC)
// [I2C0]$

// $[I2CSENSOR]
#define BSP_I2CSENSOR_ENABLE_PIN                      (0U)
#define BSP_I2CSENSOR_ENABLE_PORT                     (gpioPortD)

#define BSP_I2CSENSOR_PERIPHERAL                      (HAL_I2C_PORT_I2C0)
#define BSP_I2CSENSOR_SCL_PIN                         (5U)
#define BSP_I2CSENSOR_SCL_PORT                        (gpioPortC)

#define BSP_I2CSENSOR_SDA_PIN                         (7U)
#define BSP_I2CSENSOR_SDA_PORT                        (gpioPortC)
// [I2CSENSOR]$


// $[LED]
#define BSP_LED_PRESENT                               (1)

#define BSP_LED0_PIN                                  (2U)
#define BSP_LED0_PORT                                 (gpioPortB)

#define BSP_LED1_PIN                                  (3U)
#define BSP_LED1_PORT                                 (gpioPortD)

#define BSP_LED_COUNT                                 (2U)
#define BSP_LED_INIT                                  { { BSP_LED0_PORT, BSP_LED0_PIN }, { BSP_LED1_PORT, BSP_LED1_PIN } }
#define BSP_LED_POLARITY                              (1)
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

// $[PTI]
#define PORTIO_PTI_DFRAME_PIN                         (5U)
#define PORTIO_PTI_DFRAME_PORT                        (gpioPortD)

#define PORTIO_PTI_DOUT_PIN                           (4U)
#define PORTIO_PTI_DOUT_PORT                          (gpioPortD)

#define BSP_PTI_DFRAME_PIN                            (5U)
#define BSP_PTI_DFRAME_PORT                           (gpioPortD)

#define BSP_PTI_DOUT_PIN                              (4U)
#define BSP_PTI_DOUT_PORT                             (gpioPortD)

// [PTI]$

// $[SPIDISPLAY]

#define BSP_SPIDISPLAY_CS_PIN                         (8U)
#define BSP_SPIDISPLAY_CS_PORT                        (gpioPortC)

#define BSP_SPIDISPLAY_ENABLE_PIN                     (9U)
#define BSP_SPIDISPLAY_ENABLE_PORT                    (gpioPortC)

#define BSP_SPIDISPLAY_EXTCOMIN_PIN                   (6U)
#define BSP_SPIDISPLAY_EXTCOMIN_PORT                  (gpioPortC)

#define BSP_SPIDISPLAY_DISPLAY                        (HAL_DISPLAY_SHARP_LS013B7DH03)
#define BSP_SPIDISPLAY_USART                          (HAL_SPI_PORT_USART0)
#define BSP_SPIDISPLAY_EXTCOMIN_CHANNEL               (4)
#define BSP_SPIDISPLAY_MOSI_PIN                       (0U)
#define BSP_SPIDISPLAY_MOSI_PORT                      (gpioPortC)

#define BSP_SPIDISPLAY_MISO_PIN                       (1U)
#define BSP_SPIDISPLAY_MISO_PORT                      (gpioPortC)

#define BSP_SPIDISPLAY_CLK_PIN                        (2U)
#define BSP_SPIDISPLAY_CLK_PORT                       (gpioPortC)

// [SPIDISPLAY]$

// $[UARTNCP]
#define BSP_UARTNCP_USART_PORT                        (HAL_SERIAL_PORT_USART0)
#define BSP_UARTNCP_TX_PIN                            (8U)
#define BSP_UARTNCP_TX_PORT                           (gpioPortA)

#define BSP_UARTNCP_RX_PIN                            (9U)
#define BSP_UARTNCP_RX_PORT                           (gpioPortA)

#define BSP_UARTNCP_CTS_PIN                           (10U)
#define BSP_UARTNCP_CTS_PORT                          (gpioPortA)

#define BSP_UARTNCP_RTS_PIN                           (0U)
#define BSP_UARTNCP_RTS_PORT                          (gpioPortA)

// [UARTNCP]$

// $[USART0]
#define PORTIO_USART0_CLK_PIN                         (2U)
#define PORTIO_USART0_CLK_PORT                        (gpioPortC)

#define PORTIO_USART0_CS_PIN                          (3U)
#define PORTIO_USART0_CS_PORT                         (gpioPortC)

#define PORTIO_USART0_RX_PIN                          (2U)
#define PORTIO_USART1_RX_PORT                         (gpioPortC)

#define PORTIO_USART0_TX_PIN                          (1U)
#define PORTIO_USART1_TX_PORT                         (gpioPortC)

#define BSP_USART0_MOSI_PIN                           (0U)
#define BSP_USART0_MOSI_PORT                          (gpioPortC)

#define BSP_USART0_MISO_PIN                           (1U)
#define BSP_USART0_MISO_PORT                          (gpioPortC)

#define BSP_USART0_CLK_PIN                            (2U)
#define BSP_USART0_CLK_PORT                           (gpioPortC)

#define BSP_USART0_CS_PIN                             (3U)
#define BSP_USART0_CS_PORT                            (gpioPortC)

#define HAL_USART0_RX_QUEUE_SIZE              (128UL)
#define HAL_USART0_BAUD_RATE                  (115200UL)
#define HAL_USART0_RXSTOP                     (16UL)
#define HAL_USART0_RXSTART                    (16UL)
#define HAL_USART0_TX_QUEUE_SIZE              (128UL)
#define HAL_USART0_FLOW_CONTROL               (HAL_USART_FLOW_CONTROL_NONE)
// [USART0]$

#if defined(_SILICON_LABS_MODULE)
// Currently there is no support for ZGM23 (22q2) in sl_module.h
#if !defined(_SILICON_LABS_32B_SERIES_2)
#include "sl_module.h"
#endif /* !defined(_SILICON_LABS_32B_SERIES_2) */
#endif

#endif /* HAL_CONFIG_BOARD_H */
