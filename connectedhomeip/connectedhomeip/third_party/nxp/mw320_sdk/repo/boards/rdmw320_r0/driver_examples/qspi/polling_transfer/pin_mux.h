/*
 * Copyright 2020 NXP.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_


/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

#define BOARD_UART0_TX_PIN                                             2   /*!< Routed pin */
#define BOARD_UART0_TX_PIN_FUNCTION_ID            PINMUX_GPIO2_UART0_TXD   /*!< Pin function id */

#define BOARD_UART0_RX_PIN                                             3   /*!< Routed pin */
#define BOARD_UART0_RX_PIN_FUNCTION_ID            PINMUX_GPIO3_UART0_RXD   /*!< Pin function id */

#define BOARD_QSPI_SSn_PIN                                            28   /*!< Routed pin */
#define BOARD_QSPI_SSn_PIN_FUNCTION_ID            PINMUX_GPIO28_QSPI_SSn   /*!< Pin function id */

#define BOARD_QSPI_CLK_PIN                                            29   /*!< Routed pin */
#define BOARD_QSPI_CLK_PIN_FUNCTION_ID            PINMUX_GPIO29_QSPI_CLK   /*!< Pin function id */

#define BOARD_QSPI_D0_PIN                                            30   /*!< Routed pin */
#define BOARD_QSPI_D0_PIN_FUNCTION_ID             PINMUX_GPIO30_QSPI_D0   /*!< Pin function id */

#define BOARD_QSPI_D1_PIN                                            31   /*!< Routed pin */
#define BOARD_QSPI_D1_PIN_FUNCTION_ID             PINMUX_GPIO31_QSPI_D1   /*!< Pin function id */

#define BOARD_QSPI_D2_PIN                                            32   /*!< Routed pin */
#define BOARD_QSPI_D2_PIN_FUNCTION_ID             PINMUX_GPIO32_QSPI_D2   /*!< Pin function id */

#define BOARD_QSPI_D3_PIN                                            33   /*!< Routed pin */
#define BOARD_QSPI_D3_PIN_FUNCTION_ID             PINMUX_GPIO33_QSPI_D3   /*!< Pin function id */

/*!
 * @addtogroup pin_mux
 * @{
 */

/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif


/*!
 * @brief Calls initialization functions.
 *
 */
void BOARD_InitBootPins(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPins(void);                                 /*!< Function assigned for the core: Cortex-M4[cm4] */

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
