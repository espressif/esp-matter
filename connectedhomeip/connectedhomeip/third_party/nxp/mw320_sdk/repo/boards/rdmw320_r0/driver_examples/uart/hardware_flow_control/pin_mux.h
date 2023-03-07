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

#define BOARD_UART0_TX_PIN                                                    2   /*!< Routed pin */
#define BOARD_UART0_TX_PIN_FUNCTION_ID                   PINMUX_GPIO2_UART0_TXD   /*!< Pin function id */

#define BOARD_UART0_RX_PIN                                                    3   /*!< Routed pin */
#define BOARD_UART0_RX_PIN_FUNCTION_ID                   PINMUX_GPIO3_UART0_RXD   /*!< Pin function id */

#define BOARD_UART2_CTS_PIN                                                   46   /*!< Routed pin */
#define BOARD_UART2_CTS_PIN_FUNCTION_ID                  PINMUX_GPIO46_UART2_CTSn  /*!< Pin function id */

#define BOARD_UART2_RTS_PIN                                                   47   /*!< Routed pin */
#define BOARD_UART2_RTS_PIN_FUNCTION_ID                  PINMUX_GPIO47_UART2_RTSn  /*!< Pin function id */

#define BOARD_UART2_TX_PIN                                                    48   /*!< Routed pin */
#define BOARD_UART2_TX_PIN_FUNCTION_ID                   PINMUX_GPIO48_UART2_TXD   /*!< Pin function id */

#define BOARD_UART2_RX_PIN                                                    49   /*!< Routed pin */
#define BOARD_UART2_RX_PIN_FUNCTION_ID                   PINMUX_GPIO49_UART2_RXD   /*!< Pin function id */

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
