/***************************************************************************//**
 * @file
 * @brief SPK0838HT4H-B MEMS Microphone configuration file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef MIC_CONFIG_H
#define MIC_CONFIG_H

// -----------------------------------------------------------------------------
// Global defines

#define MIC_ENABLE_PORT       gpioPortD
#define MIC_ENABLE_PIN        0

#define MIC_PORT_DATA         gpioPortI
#define MIC_PIN_DATA          13
#define MIC_PORT_CLK          gpioPortI
#define MIC_PIN_CLK           14
#define MIC_PORT_WS           gpioPortI
#define MIC_PIN_WS            15

#define MIC_USART             USART3
#define MIC_USART_CLK         cmuClock_USART3

#define MIC_USART_LOC_DATA    USART_ROUTELOC0_RXLOC_LOC5
#define MIC_USART_LOC_CLK     USART_ROUTELOC0_CLKLOC_LOC5
#define MIC_USART_LOC_WS      USART_ROUTELOC0_CSLOC_LOC5

#define MIC_DMA_LEFT_SIGNAL   ldmaPeripheralSignal_USART3_RXDATAV
#define MIC_DMA_RIGHT_SIGNAL  ldmaPeripheralSignal_USART3_RXDATAVRIGHT

#endif // MIC_CONFIG_H
