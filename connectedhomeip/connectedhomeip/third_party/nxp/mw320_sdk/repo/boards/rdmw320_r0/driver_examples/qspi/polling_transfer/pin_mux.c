/*
 * Copyright 2020 NXP.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_pinmux.h"
#include "pin_mux.h"

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitBootPins
 * Description   : Calls initialization functions.
 *
 * END ****************************************************************************************************************/
void BOARD_InitBootPins(void)
{
    BOARD_InitPins();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitPins(void) {                                /*!< Function assigned for the core: Cortex-M4[cm4] */
    /* UART pin setting. */
    PINMUX_PinMuxSet(BOARD_UART0_TX_PIN, BOARD_UART0_TX_PIN_FUNCTION_ID | PINMUX_MODE_DEFAULT);
    PINMUX_PinMuxSet(BOARD_UART0_RX_PIN, BOARD_UART0_RX_PIN_FUNCTION_ID | PINMUX_MODE_DEFAULT);

    /* QSPI pin setting. */
    PINMUX_PinMuxSet(BOARD_QSPI_SSn_PIN, BOARD_QSPI_SSn_PIN_FUNCTION_ID | PINMUX_MODE_DEFAULT);
    PINMUX_PinMuxSet(BOARD_QSPI_CLK_PIN, BOARD_QSPI_CLK_PIN_FUNCTION_ID | PINMUX_MODE_DEFAULT);
    PINMUX_PinMuxSet(BOARD_QSPI_D0_PIN, BOARD_QSPI_D0_PIN_FUNCTION_ID | PINMUX_MODE_DEFAULT);
    PINMUX_PinMuxSet(BOARD_QSPI_D1_PIN, BOARD_QSPI_D1_PIN_FUNCTION_ID | PINMUX_MODE_DEFAULT);
    PINMUX_PinMuxSet(BOARD_QSPI_D2_PIN, BOARD_QSPI_D2_PIN_FUNCTION_ID | PINMUX_MODE_DEFAULT);
    PINMUX_PinMuxSet(BOARD_QSPI_D3_PIN, BOARD_QSPI_D3_PIN_FUNCTION_ID | PINMUX_MODE_DEFAULT);

}

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
