/***************************************************************************//**
 * @file
 * @brief Universal asynchronous receiver/transmitter (EUSART) peripheral API
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef EM_EUSART_H
#define EM_EUSART_H
#include "em_device.h"
#if defined(EUART_PRESENT) || defined(EUSART_PRESENT)
#include "em_eusart_compat.h"
#include <stdbool.h>

/* *INDENT-OFF* */
// *****************************************************************************
/// @addtogroup eusart EUSART - Extended USART
/// @brief Extended Universal Synchronous/Asynchronous Receiver/Transmitter
///
/// @li @ref eusart_intro
/// @li @ref eusart_example
/// @li @ref eusart_em2
///
///@n @section eusart_intro Introduction
///  This module contains functions to control the Enhanced Universal Synchronous
///  / Asynchronous Receiver / Transmitter controller(s) (EUSART) peripheral of Silicon
///  Labs' 32-bit MCUs and SoCs. EUSART can be used as a UART and can,
///  therefore, be connected to an external transceiver to communicate with
///  another host using the serial link.
///
///  It supports full duplex asynchronous UART communication as well as RS-485,
///  SPI, MicroWire, and 3-wire. It can also interface with ISO7816 Smart-Cards,
///  and IrDA devices.
///
///  EUSART has a wide selection of operating modes, frame formats, and baud rates.
///  All features are supported through the API of this module.
///
/// This module does not support DMA configuration. UARTDRV and SPIDRV drivers
/// provide full support for DMA and more.
///
///@n @section eusart_example Example
///
///  EUSART Async TX example:
///  @code{.c}
///  {
///    EUSART_UartInit_TypeDef init = EUSART_UART_INIT_DEFAULT_HF;
///
///    // Configure the clocks.
///    CMU_ClockSelectSet(cmuClock_EUSART0CLK, cmuSelect_EM01GRPCCLK);
///    CMU_ClockEnable(cmuClock_EUSART0CLK, true);
///    // Initialize the EUSART
///    EUSART_UartInitHf(EUSART0, &init);
///    EUSART_Tx(EUSART0, data);
///  }
///
///  @endcode
///
///  EUSART Sync SPI Transaction example:
///  @code{.c}
///  {
///    EUSART_SpiInit_TypeDef init_master = EUSART_SPI_MASTER_INIT_DEFAULT_HF;
///
///    // Configure the clocks.
///    CMU_ClockSelectSet(cmuClock_EM01GRPCCLK, cmuSelect_HFRCODPLL);
///    CMU_ClockEnable(cmuClock_EUSART1, true);
///    CMU_ClockEnable(cmuClock_GPIO, true);
///
///    //Configure the SPI ports
///    GPIO_PinModeSet(sclk_port, sclk_pin, gpioModePushPull, 0);
///    GPIO_PinModeSet(mosi_port, mosi_pin, gpioModePushPull, 0);
///    GPIO_PinModeSet(mosi_port, miso_pin, gpioModeInput, 0);
///
///    // Connect EUSART to ports
///    GPIO->EUSARTROUTE[EUSART_NUM(EUSART1)].TXROUTE = (mosi_port << _GPIO_EUSART_TXROUTE_PORT_SHIFT)
///                                                     | (mosi_pin << _GPIO_EUSART_TXROUTE_PIN_SHIFT);
///    GPIO->EUSARTROUTE[EUSART_NUM(EUSART1)].RXROUTE = (miso_port << _GPIO_EUSART_RXROUTE_PORT_SHIFT)
///                                                     | (miso_pin << _GPIO_EUSART_RXROUTE_PIN_SHIFT);
///    GPIO->EUSARTROUTE[EUSART_NUM(EUSART1)].SCLKROUTE = (sclk_port << _GPIO_EUSART_SCLKROUTE_PORT_SHIFT)
///                                                     | (sclk_pin << _GPIO_EUSART_SCLKROUTE_PIN_SHIFT);
///    GPIO->EUSARTROUTE[EUSART_NUM(EUSART1)].ROUTEEN = GPIO_EUSART_ROUTEEN_TXPEN | GPIO_EUSART_ROUTEEN_SCLKPEN;
///
///    // Initialize the EUSART
///    EUSART_SpiInit(EUSART1, &init_master);
///    EUSART_Spi_TxRx(EUSART1, data);
///  }
///
///  @endcode
///@n @section eusart_em2 EM2 guidelines for non EM2-Capable instances
///
/// @note  EUSART instances located in the PD1 power domain are non EM2-capable.
///  The <b>EUSART_EM2_CAPABLE()</b> and <b>EUSART_NOT_EM2_CAPABLE()</b> macros can be used
///  to determine whether or not a EUSART instance is EM2-Capable.
///
///  Follow theses steps when entering in EM2:
///  -# Wait for the current transaction to complete with TXCIF interrupt
///  -# Disable TX and RX using TXDIS and RXDIS cmd
///  -# Poll for EUSARTn_SYNCBUSY.TXDIS and EUSARTn_SYNCBUSY.RXDIS to go low
///  -# Wait for EUSARTn_STATUS.TXENS and EUSARTn_STATUS.RXENS to go low
///  -# Disable SCLKPEN and CSPEN in GPIO if they were previously enabled
///  -# Enter EM2
///
///  On wakeup from EM2, EUSART transmitter/receiver and relevant GPIO
///  (SCLKPEN and CSPEN) must be re-enabled. For example:
///
///  @code{.c}
///  {
///    // Enable TX and RX
///    EUSART_Enable(EUSART0, eusartEnable);
///    BUS_RegMaskedWrite(&GPIO->EUSARTROUTE[EUSART_NUM(EUSART0)].ROUTEEN,
///                       _GPIO_EUSART_ROUTEEN_TXPEN_MASK | _GPIO_EUSART_ROUTEEN_SCLKPEN_MASK,
///                       GPIO_EUSART_ROUTEEN_TXPEN | GPIO_EUSART_ROUTEEN_SCLKPEN);
///  }
///  @endcode
///
/// @{
// *****************************************************************************
/* *INDENT-ON* */

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

/// Enable selection.
typedef enum {
  /// Disable the peripheral.
  eusartDisable = 0x0,

  /// Enable receiver only, transmitter disabled.
  eusartEnableRx = (EUSART_CMD_RXEN | EUSART_CMD_TXDIS),

  /// Enable transmitter only, receiver disabled.
  eusartEnableTx = (EUSART_CMD_TXEN | EUSART_CMD_RXDIS),

  /// Enable both receiver and transmitter.
  eusartEnable = (EUSART_CMD_RXEN | EUSART_CMD_TXEN)
} EUSART_Enable_TypeDef;

/// Data bit selection.
typedef enum {
  eusartDataBits7 = EUSART_FRAMECFG_DATABITS_SEVEN,     ///< 7 data bits.
  eusartDataBits8 = EUSART_FRAMECFG_DATABITS_EIGHT,     ///< 8 data bits.
  eusartDataBits9 = EUSART_FRAMECFG_DATABITS_NINE,       ///< 9 data bits.
#if defined(EUSART_PRESENT)
  eusartDataBits10 = EUSART_FRAMECFG_DATABITS_TEN,      ///< 10 data bits, SPI mode only.
  eusartDataBits11 = EUSART_FRAMECFG_DATABITS_ELEVEN,   ///< 11 data bits, SPI mode only.
  eusartDataBits12 = EUSART_FRAMECFG_DATABITS_TWELVE,   ///< 12 data bits, SPI mode only.
  eusartDataBits13 = EUSART_FRAMECFG_DATABITS_THIRTEEN, ///< 13 data bits, SPI mode only.
  eusartDataBits14 = EUSART_FRAMECFG_DATABITS_FOURTEEN, ///< 14 data bits, SPI mode only.
  eusartDataBits15 = EUSART_FRAMECFG_DATABITS_FIFTEEN,  ///< 15 data bits, SPI mode only.
  eusartDataBits16 = EUSART_FRAMECFG_DATABITS_SIXTEEN,  ///< 16 data bits, SPI mode only.
#endif
} EUSART_Databits_TypeDef;

/// Parity selection.
typedef enum {
  eusartNoParity   = EUSART_FRAMECFG_PARITY_NONE,    ///< No parity.
  eusartEvenParity = EUSART_FRAMECFG_PARITY_EVEN,    ///< Even parity.
  eusartOddParity  = EUSART_FRAMECFG_PARITY_ODD      ///< Odd parity.
} EUSART_Parity_TypeDef;

/// Stop bits selection.
typedef enum {
  eusartStopbits0p5 = EUSART_FRAMECFG_STOPBITS_HALF,        ///< 0.5 stop bits.
  eusartStopbits1p5 = EUSART_FRAMECFG_STOPBITS_ONEANDAHALF, ///< 1.5 stop bits.
  eusartStopbits1 = EUSART_FRAMECFG_STOPBITS_ONE,           ///< 1 stop bits.
  eusartStopbits2 = EUSART_FRAMECFG_STOPBITS_TWO            ///< 2 stop bits.
} EUSART_Stopbits_TypeDef;

/// Oversampling selection, used for asynchronous operation.
typedef enum {
  eusartOVS16 = EUSART_CFG0_OVS_X16,     ///< 16x oversampling (normal).
  eusartOVS8  = EUSART_CFG0_OVS_X8,      ///< 8x oversampling.
  eusartOVS6  = EUSART_CFG0_OVS_X6,      ///< 6x oversampling.
  eusartOVS4  = EUSART_CFG0_OVS_X4,      ///< 4x oversampling.
  eusartOVS0  = EUSART_CFG0_OVS_DISABLE  ///< Oversampling disabled.
} EUSART_OVS_TypeDef;

/// HW flow control config.
typedef enum {
  eusartHwFlowControlNone = 0, ///< No HW Flow Control.
  eusartHwFlowControlCts,      ///< CTS HW Flow Control.
  eusartHwFlowControlRts,      ///< RTS HW Flow Control.
  eusartHwFlowControlCtsAndRts ///< CTS and RTS HW Flow Control.
} EUSART_HwFlowControl_TypeDef;

/// Loopback enable.
typedef enum {
  eusartLoopbackEnable  = EUSART_CFG0_LOOPBK,     ///< Enable loopback.
  eusartLoopbackDisable = _EUSART_CFG0_RESETVALUE ///< Disable loopback.
} EUSART_LoopbackEnable_TypeDef;

/// Majority vote enable.
typedef enum {
  eusartMajorityVoteEnable  = EUSART_CFG0_MVDIS_DEFAULT, ///< Enable majority vote for 16x, 8x and 6x oversampling modes.
  eusartMajorityVoteDisable = EUSART_CFG0_MVDIS          ///< Disable majority vote for 16x, 8x and 6x oversampling modes.
} EUSART_MajorityVote_TypeDef;

/// Block reception enable.
typedef enum {
  eusartBlockRxEnable  = EUSART_CMD_RXBLOCKEN, ///< Block reception enable, resulting in all incoming frames being discarded.
  eusartBlockRxDisable = EUSART_CMD_RXBLOCKDIS ///< Block reception disable, resulting in all incoming frames being loaded into the RX FIFO.
} EUSART_BlockRx_TypeDef;

/// TX output tristate enable.
typedef enum {
  eusartTristateTxEnable  = EUSART_CMD_TXTRIEN, ///< Tristates the transmitter output.
  eusartTristateTxDisable = EUSART_CMD_TXTRIDIS ///< Disables tristating of the transmitter output.
} EUSART_TristateTx_TypeDef;

/// IrDA filter enable.
typedef enum {
  eusartIrDARxFilterEnable  = EUSART_IRHFCFG_IRHFFILT_ENABLE, ///< Enable filter on demodulator.
  eusartIrDARxFilterDisable = EUSART_IRHFCFG_IRHFFILT_DISABLE ///< Disable filter on demodulator.
} EUSART_IrDARxFilterEnable_TypeDef;

/// Pulse width selection for IrDA mode.
typedef enum {
  /// IrDA pulse width is 1/16 for OVS=X16 and 1/8 for OVS=X8
  eusartIrDAPulseWidthOne   = EUSART_IRHFCFG_IRHFPW_ONE,

  /// IrDA pulse width is 2/16 for OVS=X16 and 2/8 for OVS=X8
  eusartIrDAPulseWidthTwo   = EUSART_IRHFCFG_IRHFPW_TWO,

  /// IrDA pulse width is 3/16 for OVS=X16 and 3/8 for OVS=X8
  eusartIrDAPulseWidthThree = EUSART_IRHFCFG_IRHFPW_THREE,

  /// IrDA pulse width is 4/16 for OVS=X16 and 4/8 for OVS=X8
  eusartIrDAPulseWidthFour  = EUSART_IRHFCFG_IRHFPW_FOUR
} EUSART_IrDAPulseWidth_Typedef;

/// PRS trigger enable.
typedef enum {
  /// Disable trigger on both receiver and transmitter.
  eusartPrsTriggerDisable = 0x0,

  /// Enable receive trigger only, transmit disabled.
  eusartPrsTriggerEnableRx = EUSART_TRIGCTRL_RXTEN,

  /// Enable transmit trigger only, receive disabled.
  eusartPrsTriggerEnableTx = EUSART_TRIGCTRL_TXTEN,

  /// Enable trigger on both receive and transmit.
  eusartPrsTriggerEnableRxTx = (EUSART_TRIGCTRL_RXTEN | EUSART_TRIGCTRL_TXTEN)
} EUSART_PrsTriggerEnable_TypeDef;

/// PRS Channel type.
typedef uint8_t EUSART_PrsChannel_TypeDef;

/// IO polarity selection.
typedef enum {
  /// Disable inversion on both RX and TX signals.
  eusartInvertIODisable = (EUSART_CFG0_RXINV_DISABLE | EUSART_CFG0_TXINV_DISABLE),

  /// Invert RX signal, before receiver.
  eusartInvertRxEnable = EUSART_CFG0_RXINV_ENABLE,

  /// Invert TX signal, after transmitter.
  eusartInvertTxEnable = EUSART_CFG0_TXINV_ENABLE,

  /// Enable trigger on both receive and transmit.
  eusartInvertIOEnable = (EUSART_CFG0_RXINV_ENABLE | EUSART_CFG0_TXINV_ENABLE)
} EUSART_InvertIO_TypeDef;

#if defined(EUSART_PRESENT)
/// Clock polarity/phase mode.
typedef enum {
  /// Clock idle low, sample on rising edge.
  eusartClockMode0 = EUSART_CFG2_CLKPOL_IDLELOW | EUSART_CFG2_CLKPHA_SAMPLELEADING,

  /// Clock idle low, sample on falling edge.
  eusartClockMode1 = EUSART_CFG2_CLKPOL_IDLELOW | EUSART_CFG2_CLKPHA_SAMPLETRAILING,

  /// Clock idle high, sample on falling edge.
  eusartClockMode2 = EUSART_CFG2_CLKPOL_IDLEHIGH | EUSART_CFG2_CLKPHA_SAMPLELEADING,

  /// Clock idle high, sample on rising edge.
  eusartClockMode3 = EUSART_CFG2_CLKPOL_IDLEHIGH | EUSART_CFG2_CLKPHA_SAMPLETRAILING
} EUSART_ClockMode_TypeDef;

/// Chip select polarity.
typedef enum {
  /// Chip select active low.
  eusartCsActiveLow = EUSART_CFG2_CSINV_AL,

  /// Chip select active high.
  eusartCsActiveHigh = EUSART_CFG2_CSINV_AH,
} EUSART_CsPolarity_TypeDef;

/// RX FIFO Interrupt ans Status Watermark.
typedef enum {
  eusartRxFiFoWatermark1Frame = EUSART_CFG1_RXFIW_ONEFRAME,
  eusartRxFiFoWatermark2Frame = EUSART_CFG1_RXFIW_TWOFRAMES,
  eusartRxFiFoWatermark3Frame = EUSART_CFG1_RXFIW_THREEFRAMES,
  eusartRxFiFoWatermark4Frame = EUSART_CFG1_RXFIW_FOURFRAMES,
#if (_SILICON_LABS_32B_SERIES_2_CONFIG > 2)
  eusartRxFiFoWatermark5Frame = EUSART_CFG1_RXFIW_FIVEFRAMES,
  eusartRxFiFoWatermark6Frame = EUSART_CFG1_RXFIW_SIXFRAMES,
  eusartRxFiFoWatermark7Frame = EUSART_CFG1_RXFIW_SEVENFRAMES,
  eusartRxFiFoWatermark8Frame = EUSART_CFG1_RXFIW_EIGHTFRAMES,
  eusartRxFiFoWatermark9Frame = EUSART_CFG1_RXFIW_NINEFRAMES,
  eusartRxFiFoWatermark10Frame = EUSART_CFG1_RXFIW_TENFRAMES,
  eusartRxFiFoWatermark11Frame = EUSART_CFG1_RXFIW_ELEVENFRAMES,
  eusartRxFiFoWatermark12Frame = EUSART_CFG1_RXFIW_TWELVEFRAMES,
  eusartRxFiFoWatermark13Frame = EUSART_CFG1_RXFIW_THIRTEENFRAMES,
  eusartRxFiFoWatermark14Frame = EUSART_CFG1_RXFIW_FOURTEENFRAMES,
  eusartRxFiFoWatermark15Frame = EUSART_CFG1_RXFIW_FIFTEENFRAMES,
  eusartRxFiFoWatermark16Frame = EUSART_CFG1_RXFIW_SIXTEENFRAMES
#endif
} EUSART_RxFifoWatermark_TypeDef;

/// TX FIFO Interrupt and Status Watermark.
typedef enum {
  eusartTxFiFoWatermark1Frame = EUSART_CFG1_TXFIW_ONEFRAME,
  eusartTxFiFoWatermark2Frame = EUSART_CFG1_TXFIW_TWOFRAMES,
  eusartTxFiFoWatermark3Frame = EUSART_CFG1_TXFIW_THREEFRAMES,
  eusartTxFiFoWatermark4Frame = EUSART_CFG1_TXFIW_FOURFRAMES,
#if (_SILICON_LABS_32B_SERIES_2_CONFIG > 2)
  eusartTxFiFoWatermark5Frame = EUSART_CFG1_TXFIW_FIVEFRAMES,
  eusartTxFiFoWatermark6Frame = EUSART_CFG1_TXFIW_SIXFRAMES,
  eusartTxFiFoWatermark7Frame = EUSART_CFG1_TXFIW_SEVENFRAMES,
  eusartTxFiFoWatermark8Frame = EUSART_CFG1_TXFIW_EIGHTFRAMES,
  eusartTxFiFoWatermark9Frame = EUSART_CFG1_TXFIW_NINEFRAMES,
  eusartTxFiFoWatermark10Frame = EUSART_CFG1_TXFIW_TENFRAMES,
  eusartTxFiFoWatermark11Frame = EUSART_CFG1_TXFIW_ELEVENFRAMES,
  eusartTxFiFoWatermark12Frame = EUSART_CFG1_TXFIW_TWELVEFRAMES,
  eusartTxFiFoWatermark13Frame = EUSART_CFG1_TXFIW_THIRTEENFRAMES,
  eusartTxFiFoWatermark14Frame = EUSART_CFG1_TXFIW_FOURTEENFRAMES,
  eusartTxFiFoWatermark15Frame = EUSART_CFG1_TXFIW_FIFTEENFRAMES,
  eusartTxFiFoWatermark16Frame = EUSART_CFG1_TXFIW_SIXTEENFRAMES
#endif
} EUSART_TxFifoWatermark_TypeDef;

#if defined(EUSART_DALICFG_DALIEN)
/// DALI TX databits (8-32).
typedef enum {
  eusartDaliTxDataBits8 = EUSART_DALICFG_DALITXDATABITS_EIGHT,        ///< Each frame contains 8  data bits.
  eusartDaliTxDataBits9 = EUSART_DALICFG_DALITXDATABITS_NINE,         ///< Each frame contains 9  data bits.
  eusartDaliTxDataBits10 = EUSART_DALICFG_DALITXDATABITS_TEN,         ///< Each frame contains 10 data bits.
  eusartDaliTxDataBits11 = EUSART_DALICFG_DALITXDATABITS_ELEVEN,      ///< Each frame contains 11 data bits.
  eusartDaliTxDataBits12 = EUSART_DALICFG_DALITXDATABITS_TWELVE,      ///< Each frame contains 12 data bits.
  eusartDaliTxDataBits13 = EUSART_DALICFG_DALITXDATABITS_THIRTEEN,    ///< Each frame contains 13 data bits.
  eusartDaliTxDataBits14 = EUSART_DALICFG_DALITXDATABITS_FOURTEEN,    ///< Each frame contains 14 data bits.
  eusartDaliTxDataBits15 = EUSART_DALICFG_DALITXDATABITS_FIFTEEN,     ///< Each frame contains 15 data bits.
  eusartDaliTxDataBits16 = EUSART_DALICFG_DALITXDATABITS_SIXTEEN,     ///< Each frame contains 16 data bits.
  eusartDaliTxDataBits17 = EUSART_DALICFG_DALITXDATABITS_SEVENTEEN,   ///< Each frame contains 17 data bits.
  eusartDaliTxDataBits18 = EUSART_DALICFG_DALITXDATABITS_EIGHTEEN,    ///< Each frame contains 18 data bits.
  eusartDaliTxDataBits19 = EUSART_DALICFG_DALITXDATABITS_NINETEEN,    ///< Each frame contains 19 data bits.
  eusartDaliTxDataBits20 = EUSART_DALICFG_DALITXDATABITS_TWENTY,      ///< Each frame contains 20 data bits.
  eusartDaliTxDataBits21 = EUSART_DALICFG_DALITXDATABITS_TWENTYONE,   ///< Each frame contains 21 data bits.
  eusartDaliTxDataBits22 = EUSART_DALICFG_DALITXDATABITS_TWENTYTWO,   ///< Each frame contains 22 data bits.
  eusartDaliTxDataBits23 = EUSART_DALICFG_DALITXDATABITS_TWENTYEIGHT, ///< Each frame contains 23 data bits.
  eusartDaliTxDataBits24 = EUSART_DALICFG_DALITXDATABITS_TWENTYFOUR,  ///< Each frame contains 24 data bits.
  eusartDaliTxDataBits25 = EUSART_DALICFG_DALITXDATABITS_TWENTYFIVE,  ///< Each frame contains 25 data bits.
  eusartDaliTxDataBits26 = EUSART_DALICFG_DALITXDATABITS_TWENTYSIX,   ///< Each frame contains 26 data bits.
  eusartDaliTxDataBits27 = EUSART_DALICFG_DALITXDATABITS_TWENTYSEVEN, ///< Each frame contains 27 data bits.
  eusartDaliTxDataBits28 = EUSART_DALICFG_DALITXDATABITS_TWENTYEIGHT, ///< Each frame contains 28 data bits.
  eusartDaliTxDataBits29 = EUSART_DALICFG_DALITXDATABITS_TWENTYNINE,  ///< Each frame contains 29 data bits.
  eusartDaliTxDataBits30 = EUSART_DALICFG_DALITXDATABITS_THIRTY,      ///< Each frame contains 30 data bits.
  eusartDaliTxDataBits31 = EUSART_DALICFG_DALITXDATABITS_THIRTYONE,   ///< Each frame contains 31 data bits.
  eusartDaliTxDataBits32 = EUSART_DALICFG_DALITXDATABITS_THIRTYTWO,   ///< Each frame contains 32 data bits.
} EUSART_DaliTxDatabits_TypeDef;

/// DALI RX databits (8-32).
typedef enum {
  eusartDaliRxDataBits8 = EUSART_DALICFG_DALIRXDATABITS_EIGHT,        ///< Each frame contains 8  data bits.
  eusartDaliRxDataBits9 = EUSART_DALICFG_DALIRXDATABITS_NINE,         ///< Each frame contains 9  data bits.
  eusartDaliRxDataBits10 = EUSART_DALICFG_DALIRXDATABITS_TEN,         ///< Each frame contains 10 data bits.
  eusartDaliRxDataBits11 = EUSART_DALICFG_DALIRXDATABITS_ELEVEN,      ///< Each frame contains 11 data bits.
  eusartDaliRxDataBits12 = EUSART_DALICFG_DALIRXDATABITS_TWELVE,      ///< Each frame contains 12 data bits.
  eusartDaliRxDataBits13 = EUSART_DALICFG_DALIRXDATABITS_THIRTEEN,    ///< Each frame contains 13 data bits.
  eusartDaliRxDataBits14 = EUSART_DALICFG_DALIRXDATABITS_FOURTEEN,    ///< Each frame contains 14 data bits.
  eusartDaliRxDataBits15 = EUSART_DALICFG_DALIRXDATABITS_FIFTEEN,     ///< Each frame contains 15 data bits.
  eusartDaliRxDataBits16 = EUSART_DALICFG_DALIRXDATABITS_SIXTEEN,     ///< Each frame contains 16 data bits.
  eusartDaliRxDataBits17 = EUSART_DALICFG_DALIRXDATABITS_SEVENTEEN,   ///< Each frame contains 17 data bits.
  eusartDaliRxDataBits18 = EUSART_DALICFG_DALIRXDATABITS_EIGHTEEN,    ///< Each frame contains 18 data bits.
  eusartDaliRxDataBits19 = EUSART_DALICFG_DALIRXDATABITS_NINETEEN,    ///< Each frame contains 19 data bits.
  eusartDaliRxDataBits20 = EUSART_DALICFG_DALIRXDATABITS_TWENTY,      ///< Each frame contains 20 data bits.
  eusartDaliRxDataBits21 = EUSART_DALICFG_DALIRXDATABITS_TWENTYONE,   ///< Each frame contains 21 data bits.
  eusartDaliRxDataBits22 = EUSART_DALICFG_DALIRXDATABITS_TWENTYTWO,   ///< Each frame contains 22 data bits.
  eusartDaliRxDataBits23 = EUSART_DALICFG_DALIRXDATABITS_TWENTYEIGHT, ///< Each frame contains 23 data bits.
  eusartDaliRxDataBits24 = EUSART_DALICFG_DALIRXDATABITS_TWENTYFOUR,  ///< Each frame contains 24 data bits.
  eusartDaliRxDataBits25 = EUSART_DALICFG_DALIRXDATABITS_TWENTYFIVE,  ///< Each frame contains 25 data bits.
  eusartDaliRxDataBits26 = EUSART_DALICFG_DALIRXDATABITS_TWENTYSIX,   ///< Each frame contains 26 data bits.
  eusartDaliRxDataBits27 = EUSART_DALICFG_DALIRXDATABITS_TWENTYSEVEN, ///< Each frame contains 27 data bits.
  eusartDaliRxDataBits28 = EUSART_DALICFG_DALIRXDATABITS_TWENTYEIGHT, ///< Each frame contains 28 data bits.
  eusartDaliRxDataBits29 = EUSART_DALICFG_DALIRXDATABITS_TWENTYNINE,  ///< Each frame contains 29 data bits.
  eusartDaliRxDataBits30 = EUSART_DALICFG_DALIRXDATABITS_THIRTY,      ///< Each frame contains 30 data bits.
  eusartDaliRxDataBits31 = EUSART_DALICFG_DALIRXDATABITS_THIRTYONE,   ///< Each frame contains 31 data bits.
  eusartDaliRxDataBits32 = EUSART_DALICFG_DALIRXDATABITS_THIRTYTWO,   ///< Each frame contains 32 data bits.
} EUSART_DaliRxDatabits_TypeDef;
#endif /* EUSART_DALICFG_DALIEN */
#endif /* EUSART_PRESENT */

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/
/// Advanced initialization structure.
typedef struct {
  /// Hardware flow control mode.
  EUSART_HwFlowControl_TypeDef hwFlowControl;

  /// Enable the collision Detection feature.
  /// Internal (setting loopbackEnable) or external loopback must be done to use this feature.
  bool collisionDetectEnable;

  /// If true, data will be send with most significant bit first.
  bool msbFirst;

  /// Enable inversion of RX and/or TX signals.
  EUSART_InvertIO_TypeDef invertIO;

  /// Enable the automatic wake up from EM2 to EM1 for DMA RX operation.
  bool dmaWakeUpOnRx;

  /// Enable the automatic wake up from EM2 to EM1 for DMA TX operation.
  bool dmaWakeUpOnTx;

  /// Enable DMA requests blocking while framing or parity errors.
  bool dmaHaltOnError;

  /// Start frame that will enable RX operation. 0x00 Disable this feature.
  uint8_t startFrame;

  /// Enable automatic tristating of transmistter output when there is nothing to transmit.
  bool txAutoTristate;

  /// Enable EUSART capability to use a PRS channel as an input data line for the receiver.
  /// The configured RX GPIO signal won't be routed to the EUSART receiver.
  bool prsRxEnable;

  /// PRS Channel used to transmit data from PRS to the EUSART.
  EUSART_PrsChannel_TypeDef prsRxChannel;

  /// Enable Multiprocessor mode. Address and data filtering using the 9th bit.
  bool multiProcessorEnable;

  /// Multiprocessor address bit value. If true, 9th bit of address frame must bit 1, 0 otherwise.
  bool multiProcessorAddressBitHigh;
} EUSART_AdvancedInit_TypeDef;

/// Initialization structure.
typedef struct {
  /// Specifies whether TX and/or RX will be enabled when initialization completes.
  EUSART_Enable_TypeDef enable;

  /// EUSART reference clock assumed when configuring baud rate setup. Set
  /// to 0 if using currently configured reference clock.
  uint32_t refFreq;

  /// Desired baud rate. If set to 0, Auto Baud feature is enabled and
  /// the EUSART will wait for (0x55) frame to detect the Baudrate.
  uint32_t baudrate;

  /// Oversampling used.
  EUSART_OVS_TypeDef oversampling;

  /// Number of data bits in frame.
  EUSART_Databits_TypeDef databits;

  /// Parity mode to use.
  EUSART_Parity_TypeDef   parity;

  /// Number of stop bits to use.
  EUSART_Stopbits_TypeDef stopbits;

  /// Majority Vote can be disabled for 16x, 8x and 6x oversampling modes.
  EUSART_MajorityVote_TypeDef majorityVote;

  /// Enable Loop Back configuration.
  EUSART_LoopbackEnable_TypeDef loopbackEnable;

  /// Advanced initialization structure pointer. It can be NULL.
  EUSART_AdvancedInit_TypeDef *advancedSettings;
} EUSART_UartInit_TypeDef;

/// IrDA Initialization structure.
typedef struct {
  /// General EUSART initialization structure.
  EUSART_UartInit_TypeDef init;

  /// Enable the IrDA low frequency mode. Only RX operation are enabled.
  bool irDALowFrequencyEnable;

  /// Set to enable filter on IrDA demodulator.
  EUSART_IrDARxFilterEnable_TypeDef irDARxFilterEnable;

  /// Configure the pulse width generated by the IrDA modulator as a fraction
  /// of the configured EUSART bit period.
  EUSART_IrDAPulseWidth_Typedef irDAPulseWidth;
} EUSART_IrDAInit_TypeDef;

/// PRS Trigger initialization structure.
typedef struct {
  /// PRS to EUSART trigger mode.
  EUSART_PrsTriggerEnable_TypeDef prs_trigger_enable;

  /// PRS channel to be used to trigger auto transmission.
  EUSART_PrsChannel_TypeDef prs_trigger_channel;
} EUSART_PrsTriggerInit_TypeDef;

#if defined(EUSART_PRESENT)
/// SPI Advanced initialization structure.
typedef struct {
  /// Chip select polarity
  EUSART_CsPolarity_TypeDef csPolarity;

  /// Enable inversion of RX and/or TX signals.
  EUSART_InvertIO_TypeDef invertIO;

  /// Enable automatic chip select. CS is managed by the peripheral.
  bool autoCsEnable;

  /// If true, data will be send with most significant bit first.
  bool msbFirst;

  /// Auto CS setup time (before transmission) in baud cycles. Acceptable value ( 0 to 7 baud cycle).
  uint8_t autoCsSetupTime;

  /// Auto CS hold time (after transmission) in baud cycles. Acceptable value ( 0 to 7 baud cycle).
  uint8_t autoCsHoldTime;

  /// Inter-frame time in baud cycles. Acceptable value ( 0 to 7 baud cycle).
  uint8_t autoInterFrameTime;

  /// Enable AUTOTX mode. Transmits as long as the RX FIFO is not full.
  ///  Generates underflow interrupt if the TX FIFO is empty.
  bool autoTxEnable;

  /// Default transmitted data when the TXFIFO is empty.
  uint16_t defaultTxData;

  /// Enable the automatic wake up from EM2 to EM1 for DMA RX operation.
  /// Only applicable to EM2 (low frequency) capable EUSART instances.
  bool dmaWakeUpOnRx;

  /// Enable EUSART capability to use a PRS channel as an input data line for the receiver.
  /// The configured RX GPIO signal won't be routed to the EUSART receiver.
  bool prsRxEnable;

  /// PRS Channel used to transmit data from PRS to the EUSART.
  EUSART_PrsChannel_TypeDef prsRxChannel;

  /// Enable EUSART capability to use a PRS channel as an input SPI Clock.
  /// Slave mode only.
  bool prsClockEnable;

  /// PRS Channel used to transmit SCLK from PRS to the EUSART.
  EUSART_PrsChannel_TypeDef prsClockChannel;

  /// Interrupt and status level of the Receive FIFO.
  EUSART_RxFifoWatermark_TypeDef RxFifoWatermark;

  /// Interrupt and status level of the Receive FIFO.
  EUSART_TxFifoWatermark_TypeDef TxFifoWatermark;

  /// Force load the first FIFO value.
  bool forceLoad;

  /// Setup window in bus clock cycles before the sampling edge of SCLK at word-boundary to avoid force load error.
  uint8_t setupWindow;
} EUSART_SpiAdvancedInit_TypeDef;

/// SPI Initialization structure.
typedef struct {
  /// Specifies whether TX and/or RX will be enabled when initialization completes.
  EUSART_Enable_TypeDef enable;

  /// EUSART reference clock assumed when configuring baud rate setup. Set
  /// to 0 if using currently configured reference clock.
  uint32_t refFreq;

  /// Desired bit rate in Hz.
  /// Depending on EUSART instance clock, not all bitrates
  /// are achievable as the divider is limited to 255.
  uint32_t bitRate;

  /// Number of data bits in frame.
  EUSART_Databits_TypeDef databits;

  /// Select to operate in master or slave mode.
  bool master;

  /// Clock polarity/phase mode.
  EUSART_ClockMode_TypeDef clockMode;

  /// Enable Loop Back configuration.
  EUSART_LoopbackEnable_TypeDef loopbackEnable;

  /// Advanced initialization structure pointer. It can be NULL.
  EUSART_SpiAdvancedInit_TypeDef *advancedSettings;
} EUSART_SpiInit_TypeDef;
#endif /* EUSART_PRESENT */

/// DALI Initialization structure.
typedef struct {
  /// General EUSART initialization structure.
  EUSART_UartInit_TypeDef init;

  /// Enable the DALI low frequency mode.
  bool daliLowFrequencyEnable;

#if defined(EUSART_DALICFG_DALIEN)
  /// Number of TX data bits in frame.
  EUSART_DaliTxDatabits_TypeDef TXdatabits;
  /// Number of RX data bits in frame.
  EUSART_DaliRxDatabits_TypeDef RXdatabits;
#endif
} EUSART_DaliInit_TypeDef;

/// Default configuration for EUSART initialization structure in UART mode with high-frequency clock.
#define EUSART_UART_INIT_DEFAULT_HF                                                                   \
  {                                                                                                   \
    eusartEnable,              /* Enable RX/TX when initialization completed. */                      \
    0,                         /* Use current configured reference clock for configuring baud rate.*/ \
    115200,                    /* 115200 bits/s. */                                                   \
    eusartOVS16,               /* Oversampling x16. */                                                \
    eusartDataBits8,           /* 8 data bits. */                                                     \
    eusartNoParity,            /* No parity. */                                                       \
    eusartStopbits1,           /* 1 stop bit. */                                                      \
    eusartMajorityVoteEnable,  /* Majority vote enabled. */                                           \
    eusartLoopbackDisable,     /* Loop back disabled. */                                              \
    NULL,                      /* Default advanced settings. */                                       \
  }

/// Default start frame configuration, i.e. feature disabled.
#define EUSART_DEFAULT_START_FRAME 0x00u

/// Default configuration for EUSART advanced initialization structure.
#define EUSART_ADVANCED_INIT_DEFAULT                                                         \
  {                                                                                          \
    eusartHwFlowControlNone,        /* Flow control disabled. */                             \
    false,                          /* Collision detection disabled. */                      \
    false,                          /* Data is sent with the least significant bit first. */ \
    eusartInvertIODisable,          /* RX and TX signal active high. */                      \
    false,                          /* No DMA wake up on reception. */                       \
    false,                          /* No DMA wake up on transmission. */                    \
    false,                          /* Halt DMA on error disabled. */                        \
    EUSART_DEFAULT_START_FRAME,     /* No start frame.  */                                   \
    false,                          /* TX auto tristate disabled. */                         \
    false,                          /* Do not use PRS signal as RX signal.*/                 \
    (EUSART_PrsChannel_TypeDef) 0u, /* EUSART RX connected to prs channel 0. */              \
    false,                          /* Multiprocessor mode disabled. */                      \
    false,                          /* Multiprocessor address bit : 0.*/                     \
  }

/// Default configuration for EUSART initialization structure in UART mode with low-frequency clock.
#define EUSART_UART_INIT_DEFAULT_LF                                                                    \
  {                                                                                                    \
    eusartEnable,               /* Enable RX/TX when initialization completed. */                      \
    0,                          /* Use current configured reference clock for configuring baud rate.*/ \
    9600,                       /* 9600 bits/s. */                                                     \
    eusartOVS0,                 /* Oversampling disabled. */                                           \
    eusartDataBits8,            /* 8 data bits. */                                                     \
    eusartNoParity,             /* No parity. */                                                       \
    eusartStopbits1,            /* 1 stop bit. */                                                      \
    eusartMajorityVoteDisable,  /* Majority vote enabled. */                                           \
    eusartLoopbackDisable,      /* Loop back disabled. */                                              \
    NULL,                       /* Default advanced settings. */                                       \
  }

/// Default configuration for EUSART initialization structure in IrDA mode with high-frequency clock.
#define EUSART_IRDA_INIT_DEFAULT_HF                                          \
  {                                                                          \
    EUSART_UART_INIT_DEFAULT_HF, /* Default high frequency configuration. */ \
    false,                       /* Disable IrDA low frequency mode. */      \
    eusartIrDARxFilterDisable,   /* RX Filter disabled. */                   \
    eusartIrDAPulseWidthOne,     /* Pulse width is set to 1/16. */           \
  }

/// Default configuration for EUSART initialization structure in IrDA mode with low-frequency clock.
#define EUSART_IRDA_INIT_DEFAULT_LF                                                                      \
  {                                                                                                      \
    {                                                                                                    \
      eusartEnableRx,             /* Enable RX when initialization completed (TX not allowed). */        \
      0,                          /* Use current configured reference clock for configuring baud rate.*/ \
      9600,                       /* 9600 bits/s. */                                                     \
      eusartOVS0,                 /* Oversampling disabled. */                                           \
      eusartDataBits8,            /* 8 data bits. */                                                     \
      eusartNoParity,             /* No parity. */                                                       \
      eusartStopbits1,            /* 1 stop bit. */                                                      \
      eusartMajorityVoteDisable,  /* Majority vote enabled. */                                           \
      eusartLoopbackDisable,      /* Loop back disabled. */                                              \
      NULL,                       /* Default advanced settings. */                                       \
    },                                                                                                   \
    true,                         /* Enable IrDA low frequency mode. */                                  \
    eusartIrDARxFilterDisable,    /* RX Filter disabled. */                                              \
    eusartIrDAPulseWidthOne,      /* Pulse width is set to 1. */                                         \
  }

#if defined(EUSART_PRESENT)
/// Default advanced configuration for EUSART initialization structure in SPI mode with high-frequency clock.
#define EUSART_SPI_ADVANCED_INIT_DEFAULT                                                                        \
  {                                                                                                             \
    eusartCsActiveLow,              /* CS active low. */                                                        \
    eusartInvertIODisable,          /* RX and TX signal active High. */                                         \
    true,                           /* AutoCS enabled. */                                                       \
    false,                          /* Data is sent with the least significant bit first. */                    \
    0u,                             /* CS setup time is 0 baud cycles */                                        \
    0u,                             /* CS hold time is 0 baud cycles */                                         \
    0u,                             /* Inter-frame time is 0 baud cycles */                                     \
    false,                          /* AutoTX disabled. */                                                      \
    0x0000,                         /* Default transmitted data is 0. */                                        \
    false,                          /* No DMA wake up on reception. */                                          \
    false,                          /* Do not use PRS signal as RX signal. */                                   \
    (EUSART_PrsChannel_TypeDef) 0u, /* EUSART RX tied to prs channel 0. */                                      \
    false,                          /* Do not use PRS signal as SCLK signal. */                                 \
    (EUSART_PrsChannel_TypeDef) 1u, /* EUSART SCLCK tied to prs channel 1. */                                   \
    eusartRxFiFoWatermark1Frame,    /* RXFL status/IF set when RX FIFO has at least one frame in it */          \
    eusartTxFiFoWatermark1Frame,    /* TXFL status/IF set when TX FIFO has space for at least one more frame */ \
    true,                           /* The first byte sent by the slave won't be the default value if a byte is made available \
                                       after chip select is asserted. */                                                            \
    0x04u,                          /* Setup window before the sampling edge of SCLK at word-boundary to avoid force load error. */ \
  }

/// Default configuration for EUSART initialization structure in SPI master mode with high-frequency clock.
#define EUSART_SPI_MASTER_INIT_DEFAULT_HF                                                             \
  {                                                                                                   \
    eusartEnable,              /* Enable RX/TX when initialization completed. */                      \
    0,                         /* Use current configured reference clock for configuring baud rate.*/ \
    10000000,                  /* 10 Mbits/s. */                                                      \
    eusartDataBits8,           /* 8 data bits. */                                                     \
    true,                      /* Master mode enabled. */                                             \
    eusartClockMode0,          /* Clock idle low, sample on rising edge. */                           \
    eusartLoopbackDisable,     /* Loop back disabled. */                                              \
    NULL,                      /* Default advanced settings. */                                       \
  }

/// Default configuration for EUSART initialization structure in SPI slave mode with high-frequency clock.
#define EUSART_SPI_SLAVE_INIT_DEFAULT_HF                                                              \
  {                                                                                                   \
    eusartEnable,              /* Enable RX/TX when initialization completed. */                      \
    0,                         /* Use current configured reference clock for configuring baud rate.*/ \
    10000000,                  /* 10 Mbits/s. */                                                      \
    eusartDataBits8,           /* 8 data bits. */                                                     \
    false,                     /* Master mode enabled. */                                             \
    eusartClockMode0,          /* Clock idle low, sample on rising edge. */                           \
    eusartLoopbackDisable,     /* Loop back disabled. */                                              \
    NULL,                      /* Default advanced settings. */                                       \
  }

#if defined(EUSART_DALICFG_DALIEN)
/// Default configuration for EUSART initialization structure in DALI mode with high-frequency clock.
/// Default configuration for EUSART advanced initialization structure.
#define EUSART_ADVANCED_DALI_INIT_DEFAULT                                                   \
  {                                                                                         \
    eusartHwFlowControlNone,        /* Flow control disabled. */                            \
    false,                          /* Collision detection disabled. */                     \
    true,                           /* Data is sent with the most significant bit first. */ \
    eusartInvertIODisable,          /* RX and TX signal active high. */                     \
    false,                          /* No DMA wake up on reception. */                      \
    false,                          /* No DMA wake up on transmission. */                   \
    false,                          /* Halt DMA on error disabled. */                       \
    EUSART_DEFAULT_START_FRAME,     /* No start frame.  */                                  \
    false,                          /* TX auto tristate disabled. */                        \
    false,                          /* Do not use PRS signal as RX signal.*/                \
    (EUSART_PrsChannel_TypeDef) 0u, /* EUSART RX connected to prs channel 0. */             \
    false,                          /* Multiprocessor mode disabled. */                     \
    false,                          /* Multiprocessor address bit : 0.*/                    \
  }

/// Default configuration for EUSART initialization structure in DALI mode with high-frequency clock.
#define EUSART_UART_DALI_INIT_DEFAULT_HF                                                              \
  {                                                                                                   \
    eusartEnable,              /* Enable RX/TX when initialization completed. */                      \
    0,                         /* Use current configured reference clock for configuring baud rate.*/ \
    1200,                      /* 1200 bits/s. */                                                     \
    eusartOVS16,               /* Oversampling x16. */                                                \
    eusartDataBits8,           /* 8 data bits. */                                                     \
    eusartNoParity,            /* No parity. */                                                       \
    eusartStopbits1,           /* 1 stop bit. */                                                      \
    eusartMajorityVoteEnable,  /* Majority vote enabled. */                                           \
    eusartLoopbackDisable,     /* Loop back disabled. */                                              \
    NULL,                      /* Default advanced settings. */                                       \
  }

/// Default configuration for EUSART initialization structure in DALI mode with low-frequency clock.
#define EUSART_UART_DALI_INIT_DEFAULT_LF                                                               \
  {                                                                                                    \
    eusartEnable,               /* Enable RX/TX when initialization completed. */                      \
    0,                          /* Use current configured reference clock for configuring baud rate.*/ \
    1200,                       /* 1200 bits/s. */                                                     \
    eusartOVS0,                 /* Oversampling disabled. */                                           \
    eusartDataBits8,            /* 8 data bits. */                                                     \
    eusartNoParity,             /* No parity. */                                                       \
    eusartStopbits1,            /* 1 stop bit. */                                                      \
    eusartMajorityVoteDisable,  /* Majority vote enabled. */                                           \
    eusartLoopbackDisable,      /* Loop back disabled. */                                              \
    NULL,                       /* Default advanced settings. */                                       \
  }

/// Default configuration for EUSART initialization structure in DALI mode with high-frequency clock.
#define EUSART_DALI_INIT_DEFAULT_HF                                    \
  {                                                                    \
    EUSART_UART_DALI_INIT_DEFAULT_HF,                                  \
    false,                      /* Disable DALI low frequency mode. */ \
    eusartDaliTxDataBits16,     /* TX 16 data bits. */                 \
    eusartDaliRxDataBits8,      /* RX 8 data bits. */                  \
  }                                                                    \

/// Default configuration for EUSART initialization structure in DALI mode with low-frequency clock.
#define EUSART_DALI_INIT_DEFAULT_LF                                   \
  {                                                                   \
    EUSART_UART_DALI_INIT_DEFAULT_LF,                                 \
    true,                       /* Enable DALI low frequency mode. */ \
    eusartDaliTxDataBits16,     /* TX 16 data bits. */                \
    eusartDaliRxDataBits8,      /* RX 8 data bits. */                 \
  }                                                                   \

#endif /* EUSART_DALICFG_DALIEN */
#endif /* EUSART_PRESENT */

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize EUSART when used in UART mode with the high frequency clock.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 * @param init A pointer to the initialization structure.
 ******************************************************************************/
void EUSART_UartInitHf(EUSART_TypeDef *eusart, const EUSART_UartInit_TypeDef *init);

/***************************************************************************//**
 * Initialize EUSART when used in UART mode with the low frequency clock.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 * @param init A pointer to the initialization structure.
 ******************************************************************************/
void EUSART_UartInitLf(EUSART_TypeDef *eusart, const EUSART_UartInit_TypeDef *init);

/***************************************************************************//**
 * Initialize EUSART when used in IrDA mode with the high or low
 * frequency clock.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 * @param irdaInit A pointer to the initialization structure.
 ******************************************************************************/
void EUSART_IrDAInit(EUSART_TypeDef *eusart,
                     const EUSART_IrDAInit_TypeDef *irdaInit);

#if defined(EUSART_PRESENT)
/***************************************************************************//**
 * Initialize EUSART when used in SPI mode.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 * @param init A pointer to the initialization structure.
 ******************************************************************************/
void EUSART_SpiInit(EUSART_TypeDef *eusart, const EUSART_SpiInit_TypeDef *init);

#if defined(EUSART_DALICFG_DALIEN)
/***************************************************************************//**
 * Initialize EUSART when used in DALI mode with the high or low
 * frequency clock.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 * @param daliInit A pointer to the initialization structure.
 ******************************************************************************/
void EUSART_DaliInit(EUSART_TypeDef *eusart,
                     const EUSART_DaliInit_TypeDef *daliInit);

#endif /* EUSART_DALICFG_DALIEN */
#endif /* EUSART_PRESENT */

/***************************************************************************//**
 * Configure EUSART to its reset state.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 ******************************************************************************/
void EUSART_Reset(EUSART_TypeDef *eusart);

/***************************************************************************//**
 * Enable/disable EUSART receiver and/or transmitter.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 * @param enable Select the status for the receiver and transmitter.
 ******************************************************************************/
void EUSART_Enable(EUSART_TypeDef *eusart, EUSART_Enable_TypeDef enable);

/***************************************************************************//**
 * Receive one 8 bit frame, (or part of 9 bit frame).
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 *
 * @note This function is normally used to receive one frame when operating with
 *       frame length of 8 bits. See EUSART_RxExt() for reception of 9 bit frames.
 *       Notice that possible parity/stop bits are not considered a part of the
 *       specified frame bit length.
 * @note This function will stall if buffer is empty until data is received.
 *
 * @return Data received.
 ******************************************************************************/
uint8_t EUSART_Rx(EUSART_TypeDef *eusart);

/***************************************************************************//**
 * Receive one 8-16 bit frame with extended information.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 *
 * @note This function is normally used to receive one frame and additional RX
 *       status information.
 * @note This function will stall if buffer is empty until data is received.
 *
 * @return Data received and receive status.
 ******************************************************************************/
uint16_t EUSART_RxExt(EUSART_TypeDef *eusart);

/***************************************************************************//**
 * Transmit one frame.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 * @param data Data to transmit.
 *
 * @note Depending on the frame length configuration, 8 (least significant) bits
 *       from @p data are transmitted. If the frame length is 9, 8 bits are
 *       transmitted from @p data. See EUSART_TxExt() for transmitting 9 bit frame
 *       with full control of all 9 bits.
 * @note This function will stall if the 4 frame FIFO is full, until the buffer
 *       becomes available.
 ******************************************************************************/
void EUSART_Tx(EUSART_TypeDef *eusart, uint8_t data);

/***************************************************************************//**
 * Transmit one 8-9 bit frame with extended control.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 * @param data Data to transmit.
 *
 * @note Possible parity/stop bits in asynchronous mode are not
 *       considered part of a specified frame bit length.
 * @note This function will stall if buffer is full until the buffer becomes
 *       available.
 ******************************************************************************/
void EUSART_TxExt(EUSART_TypeDef *eusart, uint16_t data);

#if defined(EUSART_PRESENT)
/***************************************************************************//**
 * Transmit one 8-16 bit frame and return received data.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 * @param data Data to transmit.
 *
 * @return Data received and receive status.
 *
 * @note SPI master mode only.
 * @note This function will stall if the TX buffer is full until the buffer becomes
 *       available.
 ******************************************************************************/
uint16_t EUSART_Spi_TxRx(EUSART_TypeDef *eusart, uint16_t data);

#if defined(EUSART_DALICFG_DALIEN)
/***************************************************************************//**
 * Transmit one DALI frame.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 * @param data Data to transmit.
 *
 * @note Depending on the TXdatabits configuration, N (least significant) bits
 *       from @p data are transmitted.
 * @note This function will stall if the 16 frame FIFO is full, until the buffer
 *       becomes available.
 ******************************************************************************/
void EUSART_Dali_Tx(EUSART_TypeDef *eusart, uint32_t data);

/***************************************************************************//**
 * Receive one 8-32 bit DALI frame.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 *
 * @note This function is normally used to receive one DALI frame (RXdatabits).
 * @note This function will stall if the 16 frame FIFO is empty until new
 *       data is received.
 *
 * @return Data received. Depending on the RXdatabits configuration, N
 *         (least significant) bits are returned.
 ******************************************************************************/
uint32_t EUSART_Dali_Rx(EUSART_TypeDef *eusart);
#endif /* EUSART_DALICFG_DALIEN */
#endif /* EUSART_PRESENT */

/***************************************************************************//**
 * Configure the baudrate (or as close as possible to a specified baudrate).
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 * @param refFreq The EUSART reference clock frequency in Hz that will be used.
 *                 If set to 0, the currently configured peripheral clock is
 *                 used.
 * @param baudrate A baudrate to try to achieve.
 ******************************************************************************/
void EUSART_BaudrateSet(EUSART_TypeDef *eusart,
                        uint32_t refFreq,
                        uint32_t baudrate);

/***************************************************************************//**
 * Get the current baudrate.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 *
 * @return The current baudrate.
 ******************************************************************************/
uint32_t EUSART_BaudrateGet(EUSART_TypeDef *eusart);

/***************************************************************************//**
 * Enable/Disable reception operation until the configured start frame is
 * received.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 * @param enable Select the receiver blocking status.
 ******************************************************************************/
void  EUSART_RxBlock(EUSART_TypeDef *eusart,
                     EUSART_BlockRx_TypeDef enable);

/***************************************************************************//**
 * Enable/Disable the tristating of the transmitter output.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 * @param enable Select the transmitter tristate status.
 ******************************************************************************/
void  EUSART_TxTristateSet(EUSART_TypeDef *eusart,
                           EUSART_TristateTx_TypeDef enable);

/***************************************************************************//**
 * Initialize the automatic enabling of transmissions and/or reception using
 * the PRS as a trigger.
 * @note
 *   Initialize EUSART with sl_eusart_initHf() or sl_eusart_initLf() before
 *   enabling the PRS trigger.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 * @param init Pointer to the initialization structure.
 ******************************************************************************/
void EUSART_PrsTriggerEnable(EUSART_TypeDef *eusart,
                             const EUSART_PrsTriggerInit_TypeDef *init);

/***************************************************************************//**
 * Get EUSART STATUS register.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 *
 * @return STATUS register value.
 ******************************************************************************/
__STATIC_INLINE uint32_t EUSART_StatusGet(EUSART_TypeDef *eusart)
{
  return eusart->STATUS;
}

/***************************************************************************//**
 * Clear one or more pending EUSART interrupts.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 *
 * @param flags Pending EUSART interrupt source to clear. Use a bitwise logic OR
 *              combination of valid interrupt flags for EUSART module
 *              (EUSART_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE void EUSART_IntClear(EUSART_TypeDef *eusart, uint32_t flags)
{
  eusart->IF_CLR = flags;
}

/***************************************************************************//**
 * Disable one or more EUSART interrupts.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 *
 * @param flags Pending EUSART interrupt source to clear. Use a bitwise logic OR
 *              combination of valid interrupt flags for EUSART module
 *              (EUSART_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE void EUSART_IntDisable(EUSART_TypeDef *eusart, uint32_t flags)
{
  eusart->IEN_CLR = flags;
}

/***************************************************************************//**
 * Enable one or more EUSART interrupts.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 *
 * @param flags Pending EUSART interrupt source to clear. Use a bitwise logic OR
 *              combination of valid interrupt flags for EUSART module
 *              (EUSART_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE void EUSART_IntEnable(EUSART_TypeDef *eusart, uint32_t flags)
{
  eusart->IEN_SET = flags;
}

/***************************************************************************//**
 * Get pending EUSART interrupt flags.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 *
 * @return Pending EUSART interrupt sources.
 ******************************************************************************/
__STATIC_INLINE uint32_t EUSART_IntGet(EUSART_TypeDef *eusart)
{
  return eusart->IF;
}

/***************************************************************************//**
 * Get enabled and pending EUSART interrupt flags.
 * Useful for handling more interrupt sources in the same interrupt handler.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 *
 * @return Pending and enabled EUSART interrupt sources.
 ******************************************************************************/
__STATIC_INLINE uint32_t EUSART_IntGetEnabled(EUSART_TypeDef *eusart)
{
  uint32_t tmp;

  /* Store EUSARTx->IEN in temporary variable in order to define explicit order
   * of volatile accesses. */
  tmp = eusart->IEN;

  /* Bitwise AND of pending and enabled interrupts */
  return eusart->IF & tmp;
}

/***************************************************************************//**
 * Set one or more pending EUSART interrupts from SW.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 *
 * @param flags Interrupt source(s) to set to pending. Use a bitwise logic OR
 *              combination of valid interrupt flags for EUSART module
 *              (EUSART_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE void EUSART_IntSet(EUSART_TypeDef *eusart, uint32_t flags)
{
  eusart->IF_SET = flags;
}

/** @} (end addtogroup eusart) */
#endif /* defined(EUART_PRESENT) || defined(EUSART_PRESENT) */
#endif /* EM_EUSART_H */
