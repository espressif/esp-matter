/***************************************************************************//**
 * @file
 * @brief DMADRV API definition.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef __SILICON_LABS_DMADRV_H__
#define __SILICON_LABS_DMADRV_H__

#include "em_device.h"
#include "ecode.h"
#include "sl_enum.h"

#if defined(DMA_PRESENT) && (DMA_COUNT == 1)
#define EMDRV_DMADRV_UDMA
#define EMDRV_DMADRV_DMA_PRESENT
#include "em_dma.h"
#elif defined(LDMA_PRESENT) && (LDMA_COUNT == 1)
#define EMDRV_DMADRV_LDMA
#define EMDRV_DMADRV_DMA_PRESENT
#include "em_ldma.h"
#else
#error "No valid DMA engine defined."
#endif

#include "dmadrv_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup dmadrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup dmadrv_error_codes Error Codes
 * @{
 ******************************************************************************/

#define ECODE_EMDRV_DMADRV_OK                  (ECODE_OK)                               ///< A successful return value.
#define ECODE_EMDRV_DMADRV_PARAM_ERROR         (ECODE_EMDRV_DMADRV_BASE | 0x00000001)   ///< An illegal input parameter.
#define ECODE_EMDRV_DMADRV_NOT_INITIALIZED     (ECODE_EMDRV_DMADRV_BASE | 0x00000002)   ///< DMA is not initialized.
#define ECODE_EMDRV_DMADRV_ALREADY_INITIALIZED (ECODE_EMDRV_DMADRV_BASE | 0x00000003)   ///< DMA has already been initialized.
#define ECODE_EMDRV_DMADRV_CHANNELS_EXHAUSTED  (ECODE_EMDRV_DMADRV_BASE | 0x00000004)   ///< No DMA channels available.
#define ECODE_EMDRV_DMADRV_IN_USE              (ECODE_EMDRV_DMADRV_BASE | 0x00000005)   ///< DMA is in use.
#define ECODE_EMDRV_DMADRV_ALREADY_FREED       (ECODE_EMDRV_DMADRV_BASE | 0x00000006)   ///< A DMA channel was free.
#define ECODE_EMDRV_DMADRV_CH_NOT_ALLOCATED    (ECODE_EMDRV_DMADRV_BASE | 0x00000007)   ///< A channel is not reserved.

/** @} (end addtogroup error codes) */
/***************************************************************************//**
 * @brief
 *  DMADRV transfer completion callback function.
 *
 * @details
 *  The callback function is called when a transfer is complete.
 *
 * @param[in] channel
 *  The DMA channel number.
 *
 * @param[in] sequenceNo
 *  The number of times the callback was called. Useful on long chains of
 *  linked transfers or on endless ping-pong type transfers.
 *
 * @param[in] userParam
 *  Optional user parameter supplied on DMA invocation.
 *
 * @return
 *   When doing ping-pong transfers, return true to continue or false to
 *   stop transfers.
 ******************************************************************************/
typedef bool (*DMADRV_Callback_t)(unsigned int channel,
                                  unsigned int sequenceNo,
                                  void *userParam);

#if defined(DMA_PRESENT) && (DMA_COUNT == 1)

/// Maximum length of one DMA transfer.
#define DMADRV_MAX_XFER_COUNT ((int)((_DMA_CTRL_N_MINUS_1_MASK >> _DMA_CTRL_N_MINUS_1_SHIFT) + 1))

/// Peripherals that can trigger UDMA transfers.
#ifdef DOXY_DOC_ONLY
SL_ENUM_GENERIC(DMADRV_Peripheralsignal_t, uint32_t) {
#else
SL_ENUM_GENERIC(DMADRV_PeripheralSignal_t, uint32_t) {
#endif
  dmadrvPeripheralSignal_NONE = 0,                                        ///< No peripheral selected for DMA triggering.
  #if defined(DMAREQ_ADC0_SCAN)
  dmadrvPeripheralSignal_ADC0_SCAN = DMAREQ_ADC0_SCAN,                    ///< Trig on ADC0_SCAN.
  #endif
  #if defined(DMAREQ_ADC0_SINGLE)
  dmadrvPeripheralSignal_ADC0_SINGLE = DMAREQ_ADC0_SINGLE,                ///< Trig on ADC0_SINGLE.
  #endif
  #if defined(DMAREQ_AES_DATARD)
  dmadrvPeripheralSignal_AES_DATARD = DMAREQ_AES_DATARD,                  ///< Trig on AES_DATARD.
  #endif
  #if defined(DMAREQ_AES_DATAWR)
  dmadrvPeripheralSignal_AES_DATAWR = DMAREQ_AES_DATAWR,                  ///< Trig on AES_DATAWR.
  #endif
  #if defined(DMAREQ_AES_KEYWR)
  dmadrvPeripheralSignal_AES_KEYWR = DMAREQ_AES_KEYWR,                    ///< Trig on AES_KEYWR.
  #endif
  #if defined(DMAREQ_AES_XORDATAWR)
  dmadrvPeripheralSignal_AES_XORDATAWR = DMAREQ_AES_XORDATAWR,            ///< Trig on AES_XORDATAWR.
  #endif
  #if defined(DMAREQ_DAC0_CH0)
  dmadrvPeripheralSignal_DAC0_CH0 = DMAREQ_DAC0_CH0,                      ///< Trig on DAC0_CH0.
  #endif
  #if defined(DMAREQ_DAC0_CH1)
  dmadrvPeripheralSignal_DAC0_CH1 = DMAREQ_DAC0_CH1,                      ///< Trig on DAC0_CH1.
  #endif
  #if defined(DMAREQ_EBI_DDEMPTY)
  dmadrvPeripheralSignal_EBI_DDEMPTY = DMAREQ_EBI_DDEMPTY,                ///< Trig on EBI_DDEMPTY.
  #endif
  #if defined(DMAREQ_EBI_PXL0EMPTY)
  dmadrvPeripheralSignal_EBI_PXL0EMPTY = DMAREQ_EBI_PXL0EMPTY,            ///< Trig on EBI_PXL0EMPTY.
  #endif
  #if defined(DMAREQ_EBI_PXL1EMPTY)
  dmadrvPeripheralSignal_EBI_PXL1EMPTY = DMAREQ_EBI_PXL1EMPTY,            ///< Trig on EBI_PXL1EMPTY.
  #endif
  #if defined(DMAREQ_EBI_PXLFULL)
  dmadrvPeripheralSignal_EBI_PXLFULL = DMAREQ_EBI_PXLFULL,                ///< Trig on EBI_PXLFULL.
  #endif
  #if defined(DMAREQ_I2C0_RXDATAV)
  dmadrvPeripheralSignal_I2C0_RXDATAV = DMAREQ_I2C0_RXDATAV,              ///< Trig on I2C0_RXDATAV.
  #endif
  #if defined(DMAREQ_I2C0_TXBL)
  dmadrvPeripheralSignal_I2C0_TXBL = DMAREQ_I2C0_TXBL,                    ///< Trig on I2C0_TXBL.
  #endif
  #if defined(DMAREQ_I2C1_RXDATAV)
  dmadrvPeripheralSignal_I2C1_RXDATAV = DMAREQ_I2C1_RXDATAV,              ///< Trig on I2C1_RXDATAV.
  #endif
  #if defined(DMAREQ_I2C1_TXBL)
  dmadrvPeripheralSignal_I2C1_TXBL = DMAREQ_I2C1_TXBL,                    ///< Trig on I2C1_TXBL.
  #endif
  #if defined(DMAREQ_LESENSE_BUFDATAV)
  dmadrvPeripheralSignal_LESENSE_BUFDATAV = DMAREQ_LESENSE_BUFDATAV,      ///< Trig on LESENSE_BUFDATAV.
  #endif
  #if defined(DMAREQ_LEUART0_RXDATAV)
  dmadrvPeripheralSignal_LEUART0_RXDATAV = DMAREQ_LEUART0_RXDATAV,        ///< Trig on LEUART0_RXDATAV.
  #endif
  #if defined(DMAREQ_LEUART0_TXBL)
  dmadrvPeripheralSignal_LEUART0_TXBL = DMAREQ_LEUART0_TXBL,              ///< Trig on LEUART0_TXBL.
  #endif
  #if defined(DMAREQ_LEUART0_TXEMPTY)
  dmadrvPeripheralSignal_LEUART0_TXEMPTY = DMAREQ_LEUART0_TXEMPTY,        ///< Trig on LEUART0_TXEMPTY.
  #endif
  #if defined(DMAREQ_LEUART1_RXDATAV)
  dmadrvPeripheralSignal_LEUART1_RXDATAV = DMAREQ_LEUART1_RXDATAV,        ///< Trig on LEUART1_RXDATAV.
  #endif
  #if defined(DMAREQ_LEUART1_TXBL)
  dmadrvPeripheralSignal_LEUART1_TXBL = DMAREQ_LEUART1_TXBL,              ///< Trig on LEUART1_TXBL.
  #endif
  #if defined(DMAREQ_LEUART1_TXEMPTY)
  dmadrvPeripheralSignal_LEUART1_TXEMPTY = DMAREQ_LEUART1_TXEMPTY,        ///< Trig on LEUART1_TXEMPTY.
  #endif
  #if defined(DMAREQ_MSC_WDATA)
  dmadrvPeripheralSignal_MSC_WDATA = DMAREQ_MSC_WDATA,                    ///< Trig on MSC_WDATA.
  #endif
  #if defined(DMAREQ_TIMER0_CC0)
  dmadrvPeripheralSignal_TIMER0_CC0 = DMAREQ_TIMER0_CC0,                  ///< Trig on TIMER0_CC0.
  #endif
  #if defined(DMAREQ_TIMER0_CC1)
  dmadrvPeripheralSignal_TIMER0_CC1 = DMAREQ_TIMER0_CC1,                  ///< Trig on TIMER0_CC1.
  #endif
  #if defined(DMAREQ_TIMER0_CC2)
  dmadrvPeripheralSignal_TIMER0_CC2 = DMAREQ_TIMER0_CC2,                  ///< Trig on TIMER0_CC2.
  #endif
  #if defined(DMAREQ_TIMER0_UFOF)
  dmadrvPeripheralSignal_TIMER0_UFOF = DMAREQ_TIMER0_UFOF,                ///< Trig on TIMER0_UFOF.
  #endif
  #if defined(DMAREQ_TIMER1_CC0)
  dmadrvPeripheralSignal_TIMER1_CC0 = DMAREQ_TIMER1_CC0,                  ///< Trig on TIMER1_CC0.
  #endif
  #if defined(DMAREQ_TIMER1_CC1)
  dmadrvPeripheralSignal_TIMER1_CC1 = DMAREQ_TIMER1_CC1,                  ///< Trig on TIMER1_CC1.
  #endif
  #if defined(DMAREQ_TIMER1_CC2)
  dmadrvPeripheralSignal_TIMER1_CC2 = DMAREQ_TIMER1_CC2,                  ///< Trig on TIMER1_CC2.
  #endif
  #if defined(DMAREQ_TIMER1_UFOF)
  dmadrvPeripheralSignal_TIMER1_UFOF = DMAREQ_TIMER1_UFOF,                ///< Trig on TIMER1_UFOF.
  #endif
  #if defined(DMAREQ_TIMER2_CC0)
  dmadrvPeripheralSignal_TIMER2_CC0 = DMAREQ_TIMER2_CC0,                  ///< Trig on TIMER2_CC0.
  #endif
  #if defined(DMAREQ_TIMER2_CC1)
  dmadrvPeripheralSignal_TIMER2_CC1 = DMAREQ_TIMER2_CC1,                  ///< Trig on TIMER2_CC1.
  #endif
  #if defined(DMAREQ_TIMER2_CC2)
  dmadrvPeripheralSignal_TIMER2_CC2 = DMAREQ_TIMER2_CC2,                  ///< Trig on TIMER2_CC2.
  #endif
  #if defined(DMAREQ_TIMER2_UFOF)
  dmadrvPeripheralSignal_TIMER2_UFOF = DMAREQ_TIMER2_UFOF,                ///< Trig on TIMER2_UFOF.
  #endif
  #if defined(DMAREQ_TIMER3_CC0)
  dmadrvPeripheralSignal_TIMER3_CC0 = DMAREQ_TIMER3_CC0,                  ///< Trig on TIMER3_CC0.
  #endif
  #if defined(DMAREQ_TIMER3_CC1)
  dmadrvPeripheralSignal_TIMER3_CC1 = DMAREQ_TIMER3_CC1,                  ///< Trig on TIMER3_CC1.
  #endif
  #if defined(DMAREQ_TIMER3_CC2)
  dmadrvPeripheralSignal_TIMER3_CC2 = DMAREQ_TIMER3_CC2,                  ///< Trig on TIMER3_CC2.
  #endif
  #if defined(DMAREQ_TIMER3_UFOF)
  dmadrvPeripheralSignal_TIMER3_UFOF = DMAREQ_TIMER3_UFOF,                ///< Trig on TIMER3_UFOF.
  #endif
  #if defined(DMAREQ_UART0_RXDATAV)
  dmadrvPeripheralSignal_UART0_RXDATAV = DMAREQ_UART0_RXDATAV,            ///< Trig on UART0_RXDATAV.
  #endif
  #if defined(DMAREQ_UART0_TXBL)
  dmadrvPeripheralSignal_UART0_TXBL = DMAREQ_UART0_TXBL,                  ///< Trig on UART0_TXBL.
  #endif
  #if defined(DMAREQ_UART0_TXEMPTY)
  dmadrvPeripheralSignal_UART0_TXEMPTY = DMAREQ_UART0_TXEMPTY,            ///< Trig on UART0_TXEMPTY.
  #endif
  #if defined(DMAREQ_UART1_RXDATAV)
  dmadrvPeripheralSignal_UART1_RXDATAV = DMAREQ_UART1_RXDATAV,            ///< Trig on UART1_RXDATAV.
  #endif
  #if defined(DMAREQ_UART1_TXBL)
  dmadrvPeripheralSignal_UART1_TXBL = DMAREQ_UART1_TXBL,                  ///< Trig on UART1_TXBL.
  #endif
  #if defined(DMAREQ_UART1_TXEMPTY)
  dmadrvPeripheralSignal_UART1_TXEMPTY = DMAREQ_UART1_TXEMPTY,            ///< Trig on UART1_TXEMPTY.
  #endif
  #if defined(DMAREQ_USART0_RXDATAV)
  dmadrvPeripheralSignal_USART0_RXDATAV = DMAREQ_USART0_RXDATAV,          ///< Trig on USART0_RXDATAV.
  #endif
  #if defined(DMAREQ_USART0_TXBL)
  dmadrvPeripheralSignal_USART0_TXBL = DMAREQ_USART0_TXBL,                ///< Trig on USART0_TXBL.
  #endif
  #if defined(DMAREQ_USART0_TXEMPTY)
  dmadrvPeripheralSignal_USART0_TXEMPTY = DMAREQ_USART0_TXEMPTY,          ///< Trig on USART0_TXEMPTY.
  #endif
  #if defined(DMAREQ_USARTRF0_RXDATAV)
  dmadrvPeripheralSignal_USARTRF0_RXDATAV = DMAREQ_USARTRF0_RXDATAV,      ///< Trig on USARTRF0_RXDATAV.
  #endif
  #if defined(DMAREQ_USARTRF0_TXBL)
  dmadrvPeripheralSignal_USARTRF0_TXBL = DMAREQ_USARTRF0_TXBL,            ///< Trig on USARTRF0_TXBL.
  #endif
  #if defined(DMAREQ_USARTRF0_TXEMPTY)
  dmadrvPeripheralSignal_USARTRF0_TXEMPTY = DMAREQ_USARTRF0_TXEMPTY,      ///< Trig on USARTRF0_TXEMPTY.
  #endif
  #if defined(DMAREQ_USARTRF1_RXDATAV)
  dmadrvPeripheralSignal_USARTRF1_RXDATAV = DMAREQ_USARTRF1_RXDATAV,      ///< Trig on USARTRF1_RXDATAV.
  #endif
  #if defined(DMAREQ_USARTRF1_TXBL)
  dmadrvPeripheralSignal_USARTRF1_TXBL = DMAREQ_USARTRF1_TXBL,            ///< Trig on USARTRF1_TXBL.
  #endif
  #if defined(DMAREQ_USARTRF1_TXEMPTY)
  dmadrvPeripheralSignal_USARTRF1_TXEMPTY = DMAREQ_USARTRF1_TXEMPTY,      ///< Trig on USARTRF1_TXEMPTY.
  #endif
  #if defined(DMAREQ_USART1_RXDATAV)
  dmadrvPeripheralSignal_USART1_RXDATAV = DMAREQ_USART1_RXDATAV,          ///< Trig on USART1_RXDATAV.
  #endif
  #if defined(DMAREQ_USART1_RXDATAVRIGHT)
  dmadrvPeripheralSignal_USART1_RXDATAVRIGHT = DMAREQ_USART1_RXDATAVRIGHT,///< Trig on USART1_RXDATAVRIGHT.
  #endif
  #if defined(DMAREQ_USART1_TXBL)
  dmadrvPeripheralSignal_USART1_TXBL = DMAREQ_USART1_TXBL,                ///< Trig on USART1_TXBL.
  #endif
  #if defined(DMAREQ_USART1_TXBLRIGHT)
  dmadrvPeripheralSignal_USART1_TXBLRIGHT = DMAREQ_USART1_TXBLRIGHT,      ///< Trig on USART1_TXBLRIGHT.
  #endif
  #if defined(DMAREQ_USART1_TXEMPTY)
  dmadrvPeripheralSignal_USART1_TXEMPTY = DMAREQ_USART1_TXEMPTY,          ///< Trig on USART1_TXEMPTY.
  #endif
  #if defined(DMAREQ_USART2_RXDATAV)
  dmadrvPeripheralSignal_USART2_RXDATAV = DMAREQ_USART2_RXDATAV,          ///< Trig on USART2_RXDATAV.
  #endif
  #if defined(DMAREQ_USART2_RXDATAVRIGHT)
  dmadrvPeripheralSignal_USART2_RXDATAVRIGHT = DMAREQ_USART2_RXDATAVRIGHT,///< Trig on USART2_RXDATAVRIGHT.
  #endif
  #if defined(DMAREQ_USART2_TXBL)
  dmadrvPeripheralSignal_USART2_TXBL = DMAREQ_USART2_TXBL,                ///< Trig on USART2_TXBL.
  #endif
  #if defined(DMAREQ_USART2_TXBLRIGHT)
  dmadrvPeripheralSignal_USART2_TXBLRIGHT = DMAREQ_USART2_TXBLRIGHT,      ///< Trig on USART2_TXBLRIGHT.
  #endif
  #if defined(DMAREQ_USART2_TXEMPTY)
  dmadrvPeripheralSignal_USART2_TXEMPTY = DMAREQ_USART2_TXEMPTY,          ///< Trig on USART2_TXEMPTY.
  #endif
};

/// Data size of one UDMA transfer item.
#ifdef DOXY_DOC_ONLY
SL_ENUM(DMADRV_Datasize_t) {
#else
SL_ENUM(DMADRV_DataSize_t) {
#endif
  dmadrvDataSize1 = dmaDataSize1,     ///< Byte
  dmadrvDataSize2 = dmaDataSize2,     ///< Halfword
  dmadrvDataSize4 = dmaDataSize4      ///< Word
};

#endif // defined( DMA_PRESENT ) && ( DMA_COUNT == 1 )

#if defined(LDMA_PRESENT) && (LDMA_COUNT == 1)

/// Maximum length of one DMA transfer.
#define DMADRV_MAX_XFER_COUNT ((int)((_LDMA_CH_CTRL_XFERCNT_MASK >> _LDMA_CH_CTRL_XFERCNT_SHIFT) + 1))

#if defined(LDMAXBAR_COUNT) && (LDMAXBAR_COUNT > 0)
/// Peripherals that can trigger LDMA transfers.
SL_ENUM_GENERIC(DMADRV_PeripheralSignal_t, uint32_t) {
  dmadrvPeripheralSignal_NONE = LDMAXBAR_CH_REQSEL_SOURCESEL_NONE,                                                          ///< No peripheral selected for DMA triggering.
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER0CC0
  dmadrvPeripheralSignal_TIMER0_CC0 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER0CC0 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER0,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER0CC1
  dmadrvPeripheralSignal_TIMER0_CC1 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER0CC1 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER0,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER0CC2
  dmadrvPeripheralSignal_TIMER0_CC2 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER0CC2 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER0,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER0UFOF
  dmadrvPeripheralSignal_TIMER0_UFOF = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER0UFOF | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER0,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER1CC0
  dmadrvPeripheralSignal_TIMER1_CC0 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER1CC0 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER1,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER1CC1
  dmadrvPeripheralSignal_TIMER1_CC1 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER1CC1 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER1,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER1CC2
  dmadrvPeripheralSignal_TIMER1_CC2 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER1CC2 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER1,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER1UFOF
  dmadrvPeripheralSignal_TIMER1_UFOF = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER1UFOF | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER1,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART0RXDATAV
  dmadrvPeripheralSignal_USART0_RXDATAV = LDMAXBAR_CH_REQSEL_SIGSEL_USART0RXDATAV | LDMAXBAR_CH_REQSEL_SOURCESEL_USART0,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART0RXDATAVRIGHT
  dmadrvPeripheralSignal_USART0_RXDATAVRIGHT = LDMAXBAR_CH_REQSEL_SIGSEL_USART0RXDATAVRIGHT | LDMAXBAR_CH_REQSEL_SOURCESEL_USART0,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART0TXBL
  dmadrvPeripheralSignal_USART0_TXBL = LDMAXBAR_CH_REQSEL_SIGSEL_USART0TXBL | LDMAXBAR_CH_REQSEL_SOURCESEL_USART0,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART0TXBLRIGHT
  dmadrvPeripheralSignal_USART0_TXBLRIGHT = LDMAXBAR_CH_REQSEL_SIGSEL_USART0TXBLRIGHT | LDMAXBAR_CH_REQSEL_SOURCESEL_USART0,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART0TXEMPTY
  dmadrvPeripheralSignal_USART0_TXEMPTY = LDMAXBAR_CH_REQSEL_SIGSEL_USART0TXEMPTY | LDMAXBAR_CH_REQSEL_SOURCESEL_USART0,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART1RXDATAV
  dmadrvPeripheralSignal_USART1_RXDATAV = LDMAXBAR_CH_REQSEL_SIGSEL_USART1RXDATAV | LDMAXBAR_CH_REQSEL_SOURCESEL_USART1,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART1RXDATAVRIGHT
  dmadrvPeripheralSignal_USART1_RXDATAVRIGHT = LDMAXBAR_CH_REQSEL_SIGSEL_USART1RXDATAVRIGHT | LDMAXBAR_CH_REQSEL_SOURCESEL_USART1,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART1TXBL
  dmadrvPeripheralSignal_USART1_TXBL = LDMAXBAR_CH_REQSEL_SIGSEL_USART1TXBL | LDMAXBAR_CH_REQSEL_SOURCESEL_USART1,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART1TXBLRIGHT
  dmadrvPeripheralSignal_USART1_TXBLRIGHT = LDMAXBAR_CH_REQSEL_SIGSEL_USART1TXBLRIGHT | LDMAXBAR_CH_REQSEL_SOURCESEL_USART1,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART1TXEMPTY
  dmadrvPeripheralSignal_USART1_TXEMPTY = LDMAXBAR_CH_REQSEL_SIGSEL_USART1TXEMPTY | LDMAXBAR_CH_REQSEL_SOURCESEL_USART1,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART2RXDATAV
  dmadrvPeripheralSignal_USART2_RXDATAV = LDMAXBAR_CH_REQSEL_SIGSEL_USART2RXDATAV | LDMAXBAR_CH_REQSEL_SOURCESEL_USART2,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART2RXDATAVRIGHT
  dmadrvPeripheralSignal_USART2_RXDATAVRIGHT = LDMAXBAR_CH_REQSEL_SIGSEL_USART2RXDATAVRIGHT | LDMAXBAR_CH_REQSEL_SOURCESEL_USART2,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART2TXBL
  dmadrvPeripheralSignal_USART2_TXBL = LDMAXBAR_CH_REQSEL_SIGSEL_USART2TXBL | LDMAXBAR_CH_REQSEL_SOURCESEL_USART2,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART2TXBLRIGHT
  dmadrvPeripheralSignal_USART2_TXBLRIGHT = LDMAXBAR_CH_REQSEL_SIGSEL_USART2TXBLRIGHT | LDMAXBAR_CH_REQSEL_SOURCESEL_USART2,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART2TXEMPTY
  dmadrvPeripheralSignal_USART2_TXEMPTY = LDMAXBAR_CH_REQSEL_SIGSEL_USART2TXEMPTY | LDMAXBAR_CH_REQSEL_SOURCESEL_USART2,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_I2C0RXDATAV
  dmadrvPeripheralSignal_I2C0_RXDATAV = LDMAXBAR_CH_REQSEL_SIGSEL_I2C0RXDATAV | LDMAXBAR_CH_REQSEL_SOURCESEL_I2C0,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_I2C0TXBL
  dmadrvPeripheralSignal_I2C0_TXBL = LDMAXBAR_CH_REQSEL_SIGSEL_I2C0TXBL | LDMAXBAR_CH_REQSEL_SOURCESEL_I2C0,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_I2C1RXDATAV
  dmadrvPeripheralSignal_I2C1_RXDATAV = LDMAXBAR_CH_REQSEL_SIGSEL_I2C1RXDATAV | LDMAXBAR_CH_REQSEL_SOURCESEL_I2C1,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_I2C1TXBL
  dmadrvPeripheralSignal_I2C1_TXBL = LDMAXBAR_CH_REQSEL_SIGSEL_I2C1TXBL | LDMAXBAR_CH_REQSEL_SOURCESEL_I2C1,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_AGCRSSI
  dmadrvPeripheralSignal_AGC_RSSI = LDMAXBAR_CH_REQSEL_SIGSEL_AGCRSSI | LDMAXBAR_CH_REQSEL_SOURCESEL_AGC,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERBOF
  dmadrvPeripheralSignal_PROTIMER_BOF = LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERBOF | LDMAXBAR_CH_REQSEL_SOURCESEL_PROTIMER,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERCC0
  dmadrvPeripheralSignal_PROTIMER_CC0 = LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERCC0 | LDMAXBAR_CH_REQSEL_SOURCESEL_PROTIMER,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERCC1
  dmadrvPeripheralSignal_PROTIMER_CC1 = LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERCC1 | LDMAXBAR_CH_REQSEL_SOURCESEL_PROTIMER,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERCC2
  dmadrvPeripheralSignal_PROTIMER_CC2 = LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERCC2 | LDMAXBAR_CH_REQSEL_SOURCESEL_PROTIMER,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERCC3
  dmadrvPeripheralSignal_PROTIMER_CC3 = LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERCC3 | LDMAXBAR_CH_REQSEL_SOURCESEL_PROTIMER,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERCC4
  dmadrvPeripheralSignal_PROTIMER_CC4 = LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERCC4 | LDMAXBAR_CH_REQSEL_SOURCESEL_PROTIMER,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERPOF
  dmadrvPeripheralSignal_PROTIMER_POF = LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERPOF | LDMAXBAR_CH_REQSEL_SOURCESEL_PROTIMER,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERWOF
  dmadrvPeripheralSignal_PROTIMER_WOF = LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERWOF | LDMAXBAR_CH_REQSEL_SOURCESEL_PROTIMER,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_MODEMDEBUG
  dmadrvPeripheralSignal_MODEM_DEBUG = LDMAXBAR_CH_REQSEL_SIGSEL_MODEMDEBUG | LDMAXBAR_CH_REQSEL_SOURCESEL_MODEM,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_IADC0IADC_SCAN
  dmadrvPeripheralSignal_IADC0_IADC_SCAN = LDMAXBAR_CH_REQSEL_SIGSEL_IADC0IADC_SCAN | LDMAXBAR_CH_REQSEL_SOURCESEL_IADC0,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_IADC0IADC_SINGLE
  dmadrvPeripheralSignal_IADC0_IADC_SINGLE = LDMAXBAR_CH_REQSEL_SIGSEL_IADC0IADC_SINGLE | LDMAXBAR_CH_REQSEL_SOURCESEL_IADC0,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_IMEMWDATA
  dmadrvPeripheralSignal_IMEM_WDATA = LDMAXBAR_CH_REQSEL_SIGSEL_IMEMWDATA | LDMAXBAR_CH_REQSEL_SOURCESEL_IMEM,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER2CC0
  dmadrvPeripheralSignal_TIMER2_CC0 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER2CC0 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER2,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER2CC1
  dmadrvPeripheralSignal_TIMER2_CC1 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER2CC1 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER2,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER2CC2
  dmadrvPeripheralSignal_TIMER2_CC2 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER2CC2 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER2,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER2UFOF
  dmadrvPeripheralSignal_TIMER2_UFOF = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER2UFOF | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER2,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER3CC0
  dmadrvPeripheralSignal_TIMER3_CC0 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER3CC0 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER3,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER3CC1
  dmadrvPeripheralSignal_TIMER3_CC1 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER3CC1 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER3,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER3CC2
  dmadrvPeripheralSignal_TIMER3_CC2 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER3CC2 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER3,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER3UFOF
  dmadrvPeripheralSignal_TIMER3_UFOF = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER3UFOF | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER3,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_EUART0TXFL
  dmadrvPeripheralSignal_EUART0_TXBL = LDMAXBAR_CH_REQSEL_SIGSEL_EUART0TXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUART0,             ///< Trig on EUART0_TXBL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_EUART0RXFL
  dmadrvPeripheralSignal_EUART0_RXDATAV = LDMAXBAR_CH_REQSEL_SIGSEL_EUART0RXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUART0,          ///< Trig on EUART0_RXBL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_EUSART0TXFL
  dmadrvPeripheralSignal_EUSART0_TXBL = LDMAXBAR_CH_REQSEL_SIGSEL_EUSART0TXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART0,          ///< Trig on EUART0_TXBL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_EUSART0RXFL
  dmadrvPeripheralSignal_EUSART0_RXDATAV = LDMAXBAR_CH_REQSEL_SIGSEL_EUSART0RXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART0,       ///< Trig on EUART0_RXBL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_EUSART1TXFL
  dmadrvPeripheralSignal_EUSART1_TXBL = LDMAXBAR_CH_REQSEL_SIGSEL_EUSART1TXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART1,          ///< Trig on EUART1_TXBL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_EUSART1RXFL
  dmadrvPeripheralSignal_EUSART1_RXDATAV = LDMAXBAR_CH_REQSEL_SIGSEL_EUSART1RXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART1,       ///< Trig on EUART1_RXBL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_EUSART2TXFL
  dmadrvPeripheralSignal_EUSART2_TXBL = LDMAXBAR_CH_REQSEL_SIGSEL_EUSART2TXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART2,          ///< Trig on EUART2_TXBL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_EUSART2RXFL
  dmadrvPeripheralSignal_EUSART2_RXDATAV = LDMAXBAR_CH_REQSEL_SIGSEL_EUSART2RXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART2,       ///< Trig on EUART2_RXBL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_EUSART3TXFL
  dmadrvPeripheralSignal_EUSART3_TXBL = LDMAXBAR_CH_REQSEL_SIGSEL_EUSART3TXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART3,          ///< Trig on EUART2_TXBL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_EUSART3RXFL
  dmadrvPeripheralSignal_EUSART3_RXDATAV = LDMAXBAR_CH_REQSEL_SIGSEL_EUSART3RXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART3,       ///< Trig on EUART3_RXBL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_EUSART4TXFL
  dmadrvPeripheralSignal_EUSART4_TXBL = LDMAXBAR_CH_REQSEL_SIGSEL_EUSART4TXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART4,          ///< Trig on EUART4_TXBL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_EUSART4RXFL
  dmadrvPeripheralSignal_EUSART4_RXDATAV = LDMAXBAR_CH_REQSEL_SIGSEL_EUSART4RXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART4,       ///< Trig on EUART4_RXBL.
  #endif
};

#else
/// Peripherals that can trigger LDMA transfers.
SL_ENUM_GENERIC(DMADRV_PeripheralSignal_t, uint32_t) {
  dmadrvPeripheralSignal_NONE = LDMA_CH_REQSEL_SOURCESEL_NONE,                                                              ///< No peripheral selected for DMA triggering.
  #if defined(LDMA_CH_REQSEL_SIGSEL_ADC0SCAN)
  dmadrvPeripheralSignal_ADC0_SCAN = LDMA_CH_REQSEL_SIGSEL_ADC0SCAN | LDMA_CH_REQSEL_SOURCESEL_ADC0,                        ///< Trig on ADC0_SCAN.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_ADC0SINGLE)
  dmadrvPeripheralSignal_ADC0_SINGLE = LDMA_CH_REQSEL_SIGSEL_ADC0SINGLE | LDMA_CH_REQSEL_SOURCESEL_ADC0,                    ///< Trig on ADC0_SINGLE.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_ADC1SCAN)
  dmadrvPeripheralSignal_ADC1_SCAN = LDMA_CH_REQSEL_SIGSEL_ADC1SCAN | LDMA_CH_REQSEL_SOURCESEL_ADC1,                        ///< Trig on ADC1_SCAN.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_ADC1SINGLE)
  dmadrvPeripheralSignal_ADC1_SINGLE = LDMA_CH_REQSEL_SIGSEL_ADC1SINGLE | LDMA_CH_REQSEL_SOURCESEL_ADC1,                    ///< Trig on ADC1_SINGLE.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_VDAC0CH0)
  dmadrvPeripheralSignal_VDAC0_CH0 = LDMA_CH_REQSEL_SIGSEL_VDAC0CH0 | LDMA_CH_REQSEL_SOURCESEL_VDAC0,                       ///< Trig on VDAC0_CH0
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_VDAC0CH1)
  dmadrvPeripheralSignal_VDAC0_CH1 = LDMA_CH_REQSEL_SIGSEL_VDAC0CH1 | LDMA_CH_REQSEL_SOURCESEL_VDAC0,                       ///< Trig on VDAC0_CH1
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_AGCRSSI)
  dmadrvPeripheralSignal_AGC_RSSI = LDMA_CH_REQSEL_SIGSEL_AGCRSSI | LDMA_CH_REQSEL_SOURCESEL_AGC,                           ///< Trig on AGC_RSSI.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTODATA0RD)
  dmadrvPeripheralSignal_CRYPTO_DATA0RD = LDMA_CH_REQSEL_SIGSEL_CRYPTODATA0RD | LDMA_CH_REQSEL_SOURCESEL_CRYPTO,            ///< Trig on CRYPTO_DATA0RD.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTODATA0WR)
  dmadrvPeripheralSignal_CRYPTO_DATA0WR = LDMA_CH_REQSEL_SIGSEL_CRYPTODATA0WR | LDMA_CH_REQSEL_SOURCESEL_CRYPTO,            ///< Trig on CRYPTO_DATA0WR.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTODATA0XWR)
  dmadrvPeripheralSignal_CRYPTO_DATA0XWR = LDMA_CH_REQSEL_SIGSEL_CRYPTODATA0XWR | LDMA_CH_REQSEL_SOURCESEL_CRYPTO,          ///< Trig on CRYPTO_DATA0XWR.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTODATA1RD)
  dmadrvPeripheralSignal_CRYPTO_DATA1RD = LDMA_CH_REQSEL_SIGSEL_CRYPTODATA1RD | LDMA_CH_REQSEL_SOURCESEL_CRYPTO,            ///< Trig on CRYPTO_DATA1RD.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTODATA1WR)
  dmadrvPeripheralSignal_CRYPTO_DATA1WR = LDMA_CH_REQSEL_SIGSEL_CRYPTODATA1WR | LDMA_CH_REQSEL_SOURCESEL_CRYPTO,            ///< Trig on CRYPTO_DATA1WR.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTO0DATA0RD)
  dmadrvPeripheralSignal_CRYPTO0_DATA0RD = LDMA_CH_REQSEL_SIGSEL_CRYPTO0DATA0RD | LDMA_CH_REQSEL_SOURCESEL_CRYPTO0,         ///< Trig on CRYPTO0_DATA0RD.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTO0DATA0WR)
  dmadrvPeripheralSignal_CRYPTO0_DATA0WR = LDMA_CH_REQSEL_SIGSEL_CRYPTO0DATA0WR | LDMA_CH_REQSEL_SOURCESEL_CRYPTO0,         ///< Trig on CRYPTO0_DATA0WR.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTO0DATA0XWR)
  dmadrvPeripheralSignal_CRYPTO0_DATA0XWR = LDMA_CH_REQSEL_SIGSEL_CRYPTO0DATA0XWR | LDMA_CH_REQSEL_SOURCESEL_CRYPTO0,       ///< Trig on CRYPTO0_DATA0XWR.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTO0DATA1RD)
  dmadrvPeripheralSignal_CRYPTO0_DATA1RD = LDMA_CH_REQSEL_SIGSEL_CRYPTO0DATA1RD | LDMA_CH_REQSEL_SOURCESEL_CRYPTO0,         ///< Trig on CRYPTO0_DATA1RD.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTO0DATA1WR)
  dmadrvPeripheralSignal_CRYPTO0_DATA1WR = LDMA_CH_REQSEL_SIGSEL_CRYPTO0DATA1WR | LDMA_CH_REQSEL_SOURCESEL_CRYPTO0,         ///< Trig on CRYPTO0_DATA1WR.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTO1DATA0RD)
  dmadrvPeripheralSignal_CRYPTO1_DATA0RD = LDMA_CH_REQSEL_SIGSEL_CRYPTO1DATA0RD | LDMA_CH_REQSEL_SOURCESEL_CRYPTO1,         ///< Trig on CRYPTO1_DATA0RD.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTO1DATA0WR)
  dmadrvPeripheralSignal_CRYPTO1_DATA0WR = LDMA_CH_REQSEL_SIGSEL_CRYPTO1DATA0WR | LDMA_CH_REQSEL_SOURCESEL_CRYPTO1,         ///< Trig on CRYPTO1_DATA0WR.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTO1DATA0XWR)
  dmadrvPeripheralSignal_CRYPTO1_DATA0XWR = LDMA_CH_REQSEL_SIGSEL_CRYPTO1DATA0XWR | LDMA_CH_REQSEL_SOURCESEL_CRYPTO1,       ///< Trig on CRYPTO1_DATA0XWR.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTO1DATA1RD)
  dmadrvPeripheralSignal_CRYPTO1_DATA1RD = LDMA_CH_REQSEL_SIGSEL_CRYPTO1DATA1RD | LDMA_CH_REQSEL_SOURCESEL_CRYPTO1,         ///< Trig on CRYPTO1_DATA1RD.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTO1DATA1WR)
  dmadrvPeripheralSignal_CRYPTO1_DATA1WR = LDMA_CH_REQSEL_SIGSEL_CRYPTO1DATA1WR | LDMA_CH_REQSEL_SOURCESEL_CRYPTO1,         ///< Trig on CRYPTO1_DATA1WR.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_EBIPXL0EMPTY)
  dmadrvPeripheralSignal_EBI_PXL0EMPTY = LDMA_CH_REQSEL_SIGSEL_EBIPXL0EMPTY | LDMA_CH_REQSEL_SOURCESEL_EBI,                 ///< Trig on EBI_PXL0EMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_EBIPXL1EMPTY)
  dmadrvPeripheralSignal_EBI_PXL1EMPTY = LDMA_CH_REQSEL_SIGSEL_EBIPXL1EMPTY | LDMA_CH_REQSEL_SOURCESEL_EBI,                 ///< Trig on EBI_PXL1EMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_EBIPXLFULL)
  dmadrvPeripheralSignal_EBI_PXLFULL = LDMA_CH_REQSEL_SIGSEL_EBIPXLFULL | LDMA_CH_REQSEL_SOURCESEL_EBI,                      ///< Trig on EBI_PXLFULL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_EBIDDEMPTY)
  dmadrvPeripheralSignal_EBI_DDEMPTY = LDMA_CH_REQSEL_SIGSEL_EBIDDEMPTY | LDMA_CH_REQSEL_SOURCESEL_EBI,                      ///< Trig on EBI_DDEMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_EBIVSYNC)
  dmadrvPeripheralSignal_EBI_VSYNC = LDMA_CH_REQSEL_SIGSEL_EBIVSYNC | LDMA_CH_REQSEL_SOURCESEL_EBI,                          ///< Trig on EBI_VSYNC.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_EBIHSYNC)
  dmadrvPeripheralSignal_EBI_HSYNC = LDMA_CH_REQSEL_SIGSEL_EBIHSYNC | LDMA_CH_REQSEL_SOURCESEL_EBI,                          ///< Trig on EBI_HSYNC.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CSENDATA)
  dmadrvPeripheralSignal_CSEN_DATA = LDMA_CH_REQSEL_SIGSEL_CSENDATA | LDMA_CH_REQSEL_SOURCESEL_CSEN,                         ///< Trig on CSEN_DATA.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CSENBSLN)
  dmadrvPeripheralSignal_CSEN_BSLN = LDMA_CH_REQSEL_SIGSEL_CSENBSLN | LDMA_CH_REQSEL_SOURCESEL_CSEN,                         ///< Trig on CSEN_BSLN.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_LSENSEBUFDATAV)
  dmadrvPeripheralSignal_LESENSE_BUFDATAV = LDMA_CH_REQSEL_SIGSEL_LSENSEBUFDATAV | LDMA_CH_REQSEL_SOURCESEL_LESENSE,         ///< Trig on LESENSE_BUFDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_I2C0RXDATAV)
  dmadrvPeripheralSignal_I2C0_RXDATAV = LDMA_CH_REQSEL_SIGSEL_I2C0RXDATAV | LDMA_CH_REQSEL_SOURCESEL_I2C0,                  ///< Trig on I2C0_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_I2C0TXBL)
  dmadrvPeripheralSignal_I2C0_TXBL = LDMA_CH_REQSEL_SIGSEL_I2C0TXBL | LDMA_CH_REQSEL_SOURCESEL_I2C0,                        ///< Trig on I2C0_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_I2C1RXDATAV)
  dmadrvPeripheralSignal_I2C1_RXDATAV = LDMA_CH_REQSEL_SIGSEL_I2C1RXDATAV | LDMA_CH_REQSEL_SOURCESEL_I2C1,                  ///< Trig on I2C1_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_I2C1TXBL)
  dmadrvPeripheralSignal_I2C1_TXBL = LDMA_CH_REQSEL_SIGSEL_I2C1TXBL | LDMA_CH_REQSEL_SOURCESEL_I2C1,                        ///< Trig on I2C1_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_I2C2RXDATAV)
  dmadrvPeripheralSignal_I2C2_RXDATAV = LDMA_CH_REQSEL_SIGSEL_I2C2RXDATAV | LDMA_CH_REQSEL_SOURCESEL_I2C2,                  ///< Trig on I2C2_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_I2C2TXBL)
  dmadrvPeripheralSignal_I2C2_TXBL = LDMA_CH_REQSEL_SIGSEL_I2C2TXBL | LDMA_CH_REQSEL_SOURCESEL_I2C2,                        ///< Trig on I2C2_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_LEUART0RXDATAV)
  dmadrvPeripheralSignal_LEUART0_RXDATAV = LDMA_CH_REQSEL_SIGSEL_LEUART0RXDATAV | LDMA_CH_REQSEL_SOURCESEL_LEUART0,         ///< Trig on LEUART0_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_LEUART0TXBL)
  dmadrvPeripheralSignal_LEUART0_TXBL = LDMA_CH_REQSEL_SIGSEL_LEUART0TXBL | LDMA_CH_REQSEL_SOURCESEL_LEUART0,               ///< Trig on LEUART0_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_LEUART0TXEMPTY)
  dmadrvPeripheralSignal_LEUART0_TXEMPTY = LDMA_CH_REQSEL_SIGSEL_LEUART0TXEMPTY | LDMA_CH_REQSEL_SOURCESEL_LEUART0,         ///< Trig on LEUART0_TXEMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_LEUART1RXDATAV)
  dmadrvPeripheralSignal_LEUART1_RXDATAV = LDMA_CH_REQSEL_SIGSEL_LEUART1RXDATAV | LDMA_CH_REQSEL_SOURCESEL_LEUART1,         ///< Trig on LEUART1_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_LEUART1TXBL)
  dmadrvPeripheralSignal_LEUART1_TXBL = LDMA_CH_REQSEL_SIGSEL_LEUART1TXBL | LDMA_CH_REQSEL_SOURCESEL_LEUART1,               ///< Trig on LEUART1_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_LEUART1TXEMPTY)
  dmadrvPeripheralSignal_LEUART1_TXEMPTY = LDMA_CH_REQSEL_SIGSEL_LEUART1TXEMPTY | LDMA_CH_REQSEL_SOURCESEL_LEUART1,         ///< Trig on LEUART1_TXEMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_MODEMDEBUG)
  dmadrvPeripheralSignal_MODEM_DEBUG = LDMA_CH_REQSEL_SIGSEL_MODEMDEBUG | LDMA_CH_REQSEL_SOURCESEL_MODEM,                   ///< Trig on MODEM_DEBUG.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_MSCWDATA)
  dmadrvPeripheralSignal_MSC_WDATA = LDMA_CH_REQSEL_SIGSEL_MSCWDATA | LDMA_CH_REQSEL_SOURCESEL_MSC,                         ///< Trig on MSC_WDATA.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_PROTIMERBOF)
  dmadrvPeripheralSignal_PROTIMER_BOF = LDMA_CH_REQSEL_SIGSEL_PROTIMERBOF | LDMA_CH_REQSEL_SOURCESEL_PROTIMER,              ///< Trig on PROTIMER_BOF.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_PROTIMERCC0)
  dmadrvPeripheralSignal_PROTIMER_CC0 = LDMA_CH_REQSEL_SIGSEL_PROTIMERCC0 | LDMA_CH_REQSEL_SOURCESEL_PROTIMER,              ///< Trig on PROTIMER_CC0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_PROTIMERCC1)
  dmadrvPeripheralSignal_PROTIMER_CC1 = LDMA_CH_REQSEL_SIGSEL_PROTIMERCC1 | LDMA_CH_REQSEL_SOURCESEL_PROTIMER,              ///< Trig on PROTIMER_CC1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_PROTIMERCC2)
  dmadrvPeripheralSignal_PROTIMER_CC2 = LDMA_CH_REQSEL_SIGSEL_PROTIMERCC2 | LDMA_CH_REQSEL_SOURCESEL_PROTIMER,              ///< Trig on PROTIMER_CC2.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_PROTIMERCC3)
  dmadrvPeripheralSignal_PROTIMER_CC3 = LDMA_CH_REQSEL_SIGSEL_PROTIMERCC3 | LDMA_CH_REQSEL_SOURCESEL_PROTIMER,              ///< Trig on PROTIMER_CC3.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_PROTIMERCC4)
  dmadrvPeripheralSignal_PROTIMER_CC4 = LDMA_CH_REQSEL_SIGSEL_PROTIMERCC4 | LDMA_CH_REQSEL_SOURCESEL_PROTIMER,              ///< Trig on PROTIMER_CC4.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_PROTIMERPOF)
  dmadrvPeripheralSignal_PROTIMER_POF = LDMA_CH_REQSEL_SIGSEL_PROTIMERPOF | LDMA_CH_REQSEL_SOURCESEL_PROTIMER,              ///< Trig on PROTIMER_POF.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_PROTIMERWOF)
  dmadrvPeripheralSignal_PROTIMER_WOF = LDMA_CH_REQSEL_SIGSEL_PROTIMERWOF | LDMA_CH_REQSEL_SOURCESEL_PROTIMER,              ///< Trig on PROTIMER_WOF.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_PRSREQ0)
  dmadrvPeripheralSignal_PRS_REQ0 = LDMA_CH_REQSEL_SIGSEL_PRSREQ0 | LDMA_CH_REQSEL_SOURCESEL_PRS,                           ///< Trig on PRS_REQ0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_PRSREQ1)
  dmadrvPeripheralSignal_PRS_REQ1 = LDMA_CH_REQSEL_SIGSEL_PRSREQ1 | LDMA_CH_REQSEL_SOURCESEL_PRS,                           ///< Trig on PRS_REQ1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER0CC0)
  dmadrvPeripheralSignal_TIMER0_CC0 = LDMA_CH_REQSEL_SIGSEL_TIMER0CC0 | LDMA_CH_REQSEL_SOURCESEL_TIMER0,                    ///< Trig on TIMER0_CC0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER0CC1)
  dmadrvPeripheralSignal_TIMER0_CC1 = LDMA_CH_REQSEL_SIGSEL_TIMER0CC1 | LDMA_CH_REQSEL_SOURCESEL_TIMER0,                    ///< Trig on TIMER0_CC1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER0CC2)
  dmadrvPeripheralSignal_TIMER0_CC2 = LDMA_CH_REQSEL_SIGSEL_TIMER0CC2 | LDMA_CH_REQSEL_SOURCESEL_TIMER0,                    ///< Trig on TIMER0_CC2.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER0CC3)
  dmadrvPeripheralSignal_TIMER0_CC3 = LDMA_CH_REQSEL_SIGSEL_TIMER0CC3 | LDMA_CH_REQSEL_SOURCESEL_TIMER0,                    ///< Trig on TIMER0_CC3.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER0UFOF)
  dmadrvPeripheralSignal_TIMER0_UFOF = LDMA_CH_REQSEL_SIGSEL_TIMER0UFOF | LDMA_CH_REQSEL_SOURCESEL_TIMER0,                  ///< Trig on TIMER0_UFOF.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER1CC0)
  dmadrvPeripheralSignal_TIMER1_CC0 = LDMA_CH_REQSEL_SIGSEL_TIMER1CC0 | LDMA_CH_REQSEL_SOURCESEL_TIMER1,                    ///< Trig on TIMER1_CC0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER1CC1)
  dmadrvPeripheralSignal_TIMER1_CC1 = LDMA_CH_REQSEL_SIGSEL_TIMER1CC1 | LDMA_CH_REQSEL_SOURCESEL_TIMER1,                    ///< Trig on TIMER1_CC1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER1CC2)
  dmadrvPeripheralSignal_TIMER1_CC2 = LDMA_CH_REQSEL_SIGSEL_TIMER1CC2 | LDMA_CH_REQSEL_SOURCESEL_TIMER1,                    ///< Trig on TIMER1_CC2.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER1CC3)
  dmadrvPeripheralSignal_TIMER1_CC3 = LDMA_CH_REQSEL_SIGSEL_TIMER1CC3 | LDMA_CH_REQSEL_SOURCESEL_TIMER1,                    ///< Trig on TIMER1_CC3.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER1UFOF)
  dmadrvPeripheralSignal_TIMER1_UFOF = LDMA_CH_REQSEL_SIGSEL_TIMER1UFOF | LDMA_CH_REQSEL_SOURCESEL_TIMER1,                  ///< Trig on TIMER1_UFOF.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER2CC0)
  dmadrvPeripheralSignal_TIMER2_CC0 = LDMA_CH_REQSEL_SIGSEL_TIMER2CC0 | LDMA_CH_REQSEL_SOURCESEL_TIMER2,                    ///< Trig on TIMER2_CC0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER2CC1)
  dmadrvPeripheralSignal_TIMER2_CC1 = LDMA_CH_REQSEL_SIGSEL_TIMER2CC1 | LDMA_CH_REQSEL_SOURCESEL_TIMER2,                    ///< Trig on TIMER2_CC1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER2CC2)
  dmadrvPeripheralSignal_TIMER2_CC2 = LDMA_CH_REQSEL_SIGSEL_TIMER2CC2 | LDMA_CH_REQSEL_SOURCESEL_TIMER2,                    ///< Trig on TIMER2_CC2.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER2CC3)
  dmadrvPeripheralSignal_TIMER2_CC3 = LDMA_CH_REQSEL_SIGSEL_TIMER2CC3 | LDMA_CH_REQSEL_SOURCESEL_TIMER2,                    ///< Trig on TIMER2_CC3.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER2UFOF)
  dmadrvPeripheralSignal_TIMER2_UFOF = LDMA_CH_REQSEL_SIGSEL_TIMER2UFOF | LDMA_CH_REQSEL_SOURCESEL_TIMER2,                  ///< Trig on TIMER2_UFOF.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER3CC0)
  dmadrvPeripheralSignal_TIMER3_CC0 = LDMA_CH_REQSEL_SIGSEL_TIMER3CC0 | LDMA_CH_REQSEL_SOURCESEL_TIMER3,                    ///< Trig on TIMER3_CC0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER3CC1)
  dmadrvPeripheralSignal_TIMER3_CC1 = LDMA_CH_REQSEL_SIGSEL_TIMER3CC1 | LDMA_CH_REQSEL_SOURCESEL_TIMER3,                    ///< Trig on TIMER3_CC1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER3CC2)
  dmadrvPeripheralSignal_TIMER3_CC2 = LDMA_CH_REQSEL_SIGSEL_TIMER3CC2 | LDMA_CH_REQSEL_SOURCESEL_TIMER3,                    ///< Trig on TIMER3_CC2.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER3CC3)
  dmadrvPeripheralSignal_TIMER3_CC3 = LDMA_CH_REQSEL_SIGSEL_TIMER3CC3 | LDMA_CH_REQSEL_SOURCESEL_TIMER3,                    ///< Trig on TIMER3_CC3.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER3UFOF)
  dmadrvPeripheralSignal_TIMER3_UFOF = LDMA_CH_REQSEL_SIGSEL_TIMER3UFOF | LDMA_CH_REQSEL_SOURCESEL_TIMER3,                  ///< Trig on TIMER3_UFOF.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER4CC0)
  dmadrvPeripheralSignal_TIMER4_CC0 = LDMA_CH_REQSEL_SIGSEL_TIMER4CC0 | LDMA_CH_REQSEL_SOURCESEL_TIMER4,                    ///< Trig on TIMER4_CC0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER4CC1)
  dmadrvPeripheralSignal_TIMER4_CC1 = LDMA_CH_REQSEL_SIGSEL_TIMER4CC1 | LDMA_CH_REQSEL_SOURCESEL_TIMER4,                    ///< Trig on TIMER4_CC1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER4CC2)
  dmadrvPeripheralSignal_TIMER4_CC2 = LDMA_CH_REQSEL_SIGSEL_TIMER4CC2 | LDMA_CH_REQSEL_SOURCESEL_TIMER4,                    ///< Trig on TIMER4_CC2.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER4CC3)
  dmadrvPeripheralSignal_TIMER4_CC3 = LDMA_CH_REQSEL_SIGSEL_TIMER4CC3 | LDMA_CH_REQSEL_SOURCESEL_TIMER4,                    ///< Trig on TIMER4_CC3.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER4UFOF)
  dmadrvPeripheralSignal_TIMER4_UFOF = LDMA_CH_REQSEL_SIGSEL_TIMER4UFOF | LDMA_CH_REQSEL_SOURCESEL_TIMER4,                  ///< Trig on TIMER4_UFOF.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER5CC0)
  dmadrvPeripheralSignal_TIMER5_CC0 = LDMA_CH_REQSEL_SIGSEL_TIMER5CC0 | LDMA_CH_REQSEL_SOURCESEL_TIMER5,                    ///< Trig on TIMER5_CC0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER5CC1)
  dmadrvPeripheralSignal_TIMER5_CC1 = LDMA_CH_REQSEL_SIGSEL_TIMER5CC1 | LDMA_CH_REQSEL_SOURCESEL_TIMER5,                    ///< Trig on TIMER5_CC1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER5CC2)
  dmadrvPeripheralSignal_TIMER5_CC2 = LDMA_CH_REQSEL_SIGSEL_TIMER5CC2 | LDMA_CH_REQSEL_SOURCESEL_TIMER5,                    ///< Trig on TIMER5_CC2.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER5CC3)
  dmadrvPeripheralSignal_TIMER5_CC3 = LDMA_CH_REQSEL_SIGSEL_TIMER5CC3 | LDMA_CH_REQSEL_SOURCESEL_TIMER5,                    ///< Trig on TIMER5_CC3.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER5UFOF)
  dmadrvPeripheralSignal_TIMER5_UFOF = LDMA_CH_REQSEL_SIGSEL_TIMER5UFOF | LDMA_CH_REQSEL_SOURCESEL_TIMER5,                  ///< Trig on TIMER5_UFOF.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER6CC0)
  dmadrvPeripheralSignal_TIMER6_CC0 = LDMA_CH_REQSEL_SIGSEL_TIMER6CC0 | LDMA_CH_REQSEL_SOURCESEL_TIMER6,                    ///< Trig on TIMER6_CC0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER6CC1)
  dmadrvPeripheralSignal_TIMER6_CC1 = LDMA_CH_REQSEL_SIGSEL_TIMER6CC1 | LDMA_CH_REQSEL_SOURCESEL_TIMER6,                    ///< Trig on TIMER6_CC1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER6CC2)
  dmadrvPeripheralSignal_TIMER6_CC2 = LDMA_CH_REQSEL_SIGSEL_TIMER6CC2 | LDMA_CH_REQSEL_SOURCESEL_TIMER6,                    ///< Trig on TIMER6_CC2.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER6CC3)
  dmadrvPeripheralSignal_TIMER6_CC3 = LDMA_CH_REQSEL_SIGSEL_TIMER6CC3 | LDMA_CH_REQSEL_SOURCESEL_TIMER6,                    ///< Trig on TIMER6_CC3.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER6UFOF)
  dmadrvPeripheralSignal_TIMER6_UFOF = LDMA_CH_REQSEL_SIGSEL_TIMER6UFOF | LDMA_CH_REQSEL_SOURCESEL_TIMER6,                  ///< Trig on TIMER6_UFOF.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER0CC0)
  dmadrvPeripheralSignal_WTIMER0_CC0 = LDMA_CH_REQSEL_SIGSEL_WTIMER0CC0 | LDMA_CH_REQSEL_SOURCESEL_WTIMER0,                 ///< Trig on WTIMER0_CC0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER0CC1)
  dmadrvPeripheralSignal_WTIMER0_CC1 = LDMA_CH_REQSEL_SIGSEL_WTIMER0CC1 | LDMA_CH_REQSEL_SOURCESEL_WTIMER0,                 ///< Trig on WTIMER0_CC1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER0CC2)
  dmadrvPeripheralSignal_WTIMER0_CC2 = LDMA_CH_REQSEL_SIGSEL_WTIMER0CC2 | LDMA_CH_REQSEL_SOURCESEL_WTIMER0,                 ///< Trig on WTIMER0_CC2.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER0CC3)
  dmadrvPeripheralSignal_WTIMER0_CC3 = LDMA_CH_REQSEL_SIGSEL_WTIMER0CC3 | LDMA_CH_REQSEL_SOURCESEL_WTIMER0,                 ///< Trig on WTIMER0_CC3.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER0UFOF)
  dmadrvPeripheralSignal_WTIMER0_UFOF = LDMA_CH_REQSEL_SIGSEL_WTIMER0UFOF | LDMA_CH_REQSEL_SOURCESEL_WTIMER0,               ///< Trig on WTIMER0_UFOF.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER1CC0)
  dmadrvPeripheralSignal_WTIMER1_CC0 = LDMA_CH_REQSEL_SIGSEL_WTIMER1CC0 | LDMA_CH_REQSEL_SOURCESEL_WTIMER1,                 ///< Trig on WTIMER1_CC0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER1CC1)
  dmadrvPeripheralSignal_WTIMER1_CC1 = LDMA_CH_REQSEL_SIGSEL_WTIMER1CC1 | LDMA_CH_REQSEL_SOURCESEL_WTIMER1,                 ///< Trig on WTIMER1_CC1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER1CC2)
  dmadrvPeripheralSignal_WTIMER1_CC2 = LDMA_CH_REQSEL_SIGSEL_WTIMER1CC2 | LDMA_CH_REQSEL_SOURCESEL_WTIMER1,                 ///< Trig on WTIMER1_CC2.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER1CC3)
  dmadrvPeripheralSignal_WTIMER1_CC3 = LDMA_CH_REQSEL_SIGSEL_WTIMER1CC3 | LDMA_CH_REQSEL_SOURCESEL_WTIMER1,                 ///< Trig on WTIMER1_CC3.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER1UFOF)
  dmadrvPeripheralSignal_WTIMER1_UFOF = LDMA_CH_REQSEL_SIGSEL_WTIMER1UFOF | LDMA_CH_REQSEL_SOURCESEL_WTIMER1,               ///< Trig on WTIMER1_UFOF.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER2CC0)
  dmadrvPeripheralSignal_WTIMER2_CC0 = LDMA_CH_REQSEL_SIGSEL_WTIMER2CC0 | LDMA_CH_REQSEL_SOURCESEL_WTIMER2,                 ///< Trig on WTIMER2_CC0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER2CC1)
  dmadrvPeripheralSignal_WTIMER2_CC1 = LDMA_CH_REQSEL_SIGSEL_WTIMER2CC1 | LDMA_CH_REQSEL_SOURCESEL_WTIMER2,                 ///< Trig on WTIMER2_CC1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER2CC2)
  dmadrvPeripheralSignal_WTIMER2_CC2 = LDMA_CH_REQSEL_SIGSEL_WTIMER2CC2 | LDMA_CH_REQSEL_SOURCESEL_WTIMER2,                 ///< Trig on WTIMER2_CC2.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER2CC3)
  dmadrvPeripheralSignal_WTIMER2_CC3 = LDMA_CH_REQSEL_SIGSEL_WTIMER2CC3 | LDMA_CH_REQSEL_SOURCESEL_WTIMER2,                 ///< Trig on WTIMER2_CC3.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER2UFOF)
  dmadrvPeripheralSignal_WTIMER2_UFOF = LDMA_CH_REQSEL_SIGSEL_WTIMER2UFOF | LDMA_CH_REQSEL_SOURCESEL_WTIMER2,               ///< Trig on WTIMER2_UFOF.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER3CC0)
  dmadrvPeripheralSignal_WTIMER3_CC0 = LDMA_CH_REQSEL_SIGSEL_WTIMER3CC0 | LDMA_CH_REQSEL_SOURCESEL_WTIMER3,                 ///< Trig on WTIMER3_CC0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER3CC1)
  dmadrvPeripheralSignal_WTIMER3_CC1 = LDMA_CH_REQSEL_SIGSEL_WTIMER3CC1 | LDMA_CH_REQSEL_SOURCESEL_WTIMER3,                 ///< Trig on WTIMER3_CC1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER3CC2)
  dmadrvPeripheralSignal_WTIMER3_CC2 = LDMA_CH_REQSEL_SIGSEL_WTIMER3CC2 | LDMA_CH_REQSEL_SOURCESEL_WTIMER3,                 ///< Trig on WTIMER3_CC2.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER3CC3)
  dmadrvPeripheralSignal_WTIMER3_CC3 = LDMA_CH_REQSEL_SIGSEL_WTIMER3CC3 | LDMA_CH_REQSEL_SOURCESEL_WTIMER3,                 ///< Trig on WTIMER3_CC3.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER3UFOF)
  dmadrvPeripheralSignal_WTIMER3_UFOF = LDMA_CH_REQSEL_SIGSEL_WTIMER3UFOF | LDMA_CH_REQSEL_SOURCESEL_WTIMER3,               ///< Trig on WTIMER3_UFOF.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART0RXDATAV)
  dmadrvPeripheralSignal_USART0_RXDATAV = LDMA_CH_REQSEL_SIGSEL_USART0RXDATAV | LDMA_CH_REQSEL_SOURCESEL_USART0,            ///< Trig on USART0_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART0TXBL)
  dmadrvPeripheralSignal_USART0_TXBL = LDMA_CH_REQSEL_SIGSEL_USART0TXBL | LDMA_CH_REQSEL_SOURCESEL_USART0,                  ///< Trig on USART0_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART0TXEMPTY)
  dmadrvPeripheralSignal_USART0_TXEMPTY = LDMA_CH_REQSEL_SIGSEL_USART0TXEMPTY | LDMA_CH_REQSEL_SOURCESEL_USART0,            ///< Trig on USART0_TXEMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART1RXDATAV)
  dmadrvPeripheralSignal_USART1_RXDATAV = LDMA_CH_REQSEL_SIGSEL_USART1RXDATAV | LDMA_CH_REQSEL_SOURCESEL_USART1,            ///< Trig on USART1_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART1RXDATAVRIGHT)
  dmadrvPeripheralSignal_USART1_RXDATAVRIGHT = LDMA_CH_REQSEL_SIGSEL_USART1RXDATAVRIGHT | LDMA_CH_REQSEL_SOURCESEL_USART1,  ///< Trig on USART1_RXDATAVRIGHT.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART1TXBL)
  dmadrvPeripheralSignal_USART1_TXBL = LDMA_CH_REQSEL_SIGSEL_USART1TXBL | LDMA_CH_REQSEL_SOURCESEL_USART1,                  ///< Trig on USART1_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART1TXBLRIGHT)
  dmadrvPeripheralSignal_USART1_TXBLRIGHT = LDMA_CH_REQSEL_SIGSEL_USART1TXBLRIGHT | LDMA_CH_REQSEL_SOURCESEL_USART1,        ///< Trig on USART1_TXBLRIGHT.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART1TXEMPTY)
  dmadrvPeripheralSignal_USART1_TXEMPTY = LDMA_CH_REQSEL_SIGSEL_USART1TXEMPTY | LDMA_CH_REQSEL_SOURCESEL_USART1,            ///< Trig on USART1_TXEMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART2RXDATAV)
  dmadrvPeripheralSignal_USART2_RXDATAV = LDMA_CH_REQSEL_SIGSEL_USART2RXDATAV | LDMA_CH_REQSEL_SOURCESEL_USART2,            ///< Trig on USART2_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART2RXDATAVRIGHT)
  dmadrvPeripheralSignal_USART2_RXDATAVRIGHT = LDMA_CH_REQSEL_SIGSEL_USART2RXDATAVRIGHT | LDMA_CH_REQSEL_SOURCESEL_USART2,  ///< Trig on USART2_RXDATAVRIGHT.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART2TXBL)
  dmadrvPeripheralSignal_USART2_TXBL = LDMA_CH_REQSEL_SIGSEL_USART2TXBL | LDMA_CH_REQSEL_SOURCESEL_USART2,                  ///< Trig on USART2_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART2TXBLRIGHT)
  dmadrvPeripheralSignal_USART2_TXBLRIGHT = LDMA_CH_REQSEL_SIGSEL_USART2TXBLRIGHT | LDMA_CH_REQSEL_SOURCESEL_USART2,        ///< Trig on USART2_TXBLRIGHT.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART2TXEMPTY)
  dmadrvPeripheralSignal_USART2_TXEMPTY = LDMA_CH_REQSEL_SIGSEL_USART2TXEMPTY | LDMA_CH_REQSEL_SOURCESEL_USART2,            ///< Trig on USART2_TXEMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART3RXDATAV)
  dmadrvPeripheralSignal_USART3_RXDATAV = LDMA_CH_REQSEL_SIGSEL_USART3RXDATAV | LDMA_CH_REQSEL_SOURCESEL_USART3,            ///< Trig on USART3_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART3RXDATAVRIGHT)
  dmadrvPeripheralSignal_USART3_RXDATAVRIGHT = LDMA_CH_REQSEL_SIGSEL_USART3RXDATAVRIGHT | LDMA_CH_REQSEL_SOURCESEL_USART3,  ///< Trig on USART3_RXDATAVRIGHT.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART3TXBL)
  dmadrvPeripheralSignal_USART3_TXBL = LDMA_CH_REQSEL_SIGSEL_USART3TXBL | LDMA_CH_REQSEL_SOURCESEL_USART3,                  ///< Trig on USART3_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART3TXBLRIGHT)
  dmadrvPeripheralSignal_USART3_TXBLRIGHT = LDMA_CH_REQSEL_SIGSEL_USART3TXBLRIGHT | LDMA_CH_REQSEL_SOURCESEL_USART3,        ///< Trig on USART3_TXBLRIGHT.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART3TXEMPTY)
  dmadrvPeripheralSignal_USART3_TXEMPTY = LDMA_CH_REQSEL_SIGSEL_USART3TXEMPTY | LDMA_CH_REQSEL_SOURCESEL_USART3,            ///< Trig on USART3_TXEMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART4RXDATAV)
  dmadrvPeripheralSignal_USART4_RXDATAV = LDMA_CH_REQSEL_SIGSEL_USART4RXDATAV | LDMA_CH_REQSEL_SOURCESEL_USART4,            ///< Trig on USART4_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART4RXDATAVRIGHT)
  dmadrvPeripheralSignal_USART4_RXDATAVRIGHT = LDMA_CH_REQSEL_SIGSEL_USART4RXDATAVRIGHT | LDMA_CH_REQSEL_SOURCESEL_USART4,  ///< Trig on USART4_RXDATAVRIGHT.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART4TXBL)
  dmadrvPeripheralSignal_USART4_TXBL = LDMA_CH_REQSEL_SIGSEL_USART4TXBL | LDMA_CH_REQSEL_SOURCESEL_USART4,                  ///< Trig on USART4_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART4TXBLRIGHT)
  dmadrvPeripheralSignal_USART4_TXBLRIGHT = LDMA_CH_REQSEL_SIGSEL_USART4TXBLRIGHT | LDMA_CH_REQSEL_SOURCESEL_USART4,        ///< Trig on USART4_TXBLRIGHT.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART4TXEMPTY)
  dmadrvPeripheralSignal_USART4_TXEMPTY = LDMA_CH_REQSEL_SIGSEL_USART4TXEMPTY | LDMA_CH_REQSEL_SOURCESEL_USART4,            ///< Trig on USART4_TXEMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART5RXDATAV)
  dmadrvPeripheralSignal_USART5_RXDATAV = LDMA_CH_REQSEL_SIGSEL_USART5RXDATAV | LDMA_CH_REQSEL_SOURCESEL_USART5,            ///< Trig on USART5_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART5RXDATAVRIGHT)
  dmadrvPeripheralSignal_USART5_RXDATAVRIGHT = LDMA_CH_REQSEL_SIGSEL_USART5RXDATAVRIGHT | LDMA_CH_REQSEL_SOURCESEL_USART5,  ///< Trig on USART5_RXDATAVRIGHT.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART5TXBL)
  dmadrvPeripheralSignal_USART5_TXBL = LDMA_CH_REQSEL_SIGSEL_USART5TXBL | LDMA_CH_REQSEL_SOURCESEL_USART5,                  ///< Trig on USART5_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART5TXBLRIGHT)
  dmadrvPeripheralSignal_USART5_TXBLRIGHT = LDMA_CH_REQSEL_SIGSEL_USART5TXBLRIGHT | LDMA_CH_REQSEL_SOURCESEL_USART5,        ///< Trig on USART5_TXBLRIGHT.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART5TXEMPTY)
  dmadrvPeripheralSignal_USART5_TXEMPTY = LDMA_CH_REQSEL_SIGSEL_USART5TXEMPTY | LDMA_CH_REQSEL_SOURCESEL_USART5,            ///< Trig on USART5_TXEMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_UART0RXDATAV)
  dmadrvPeripheralSignal_UART0_RXDATAV = LDMA_CH_REQSEL_SIGSEL_UART0RXDATAV | LDMA_CH_REQSEL_SOURCESEL_UART0,               ///< Trig on UART0_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_UART0TXBL)
  dmadrvPeripheralSignal_UART0_TXBL = LDMA_CH_REQSEL_SIGSEL_UART0TXBL | LDMA_CH_REQSEL_SOURCESEL_UART0,                     ///< Trig on UART0_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_UART0TXEMPTY)
  dmadrvPeripheralSignal_UART0_TXEMPTY = LDMA_CH_REQSEL_SIGSEL_UART0TXEMPTY | LDMA_CH_REQSEL_SOURCESEL_UART0,               ///< Trig on UART0_TXEMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_UART1RXDATAV)
  dmadrvPeripheralSignal_UART1_RXDATAV = LDMA_CH_REQSEL_SIGSEL_UART1RXDATAV | LDMA_CH_REQSEL_SOURCESEL_UART1,               ///< Trig on UART1_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_UART1TXBL)
  dmadrvPeripheralSignal_UART1_TXBL = LDMA_CH_REQSEL_SIGSEL_UART1TXBL | LDMA_CH_REQSEL_SOURCESEL_UART1,                     ///< Trig on UART1_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_UART1TXEMPTY)
  dmadrvPeripheralSignal_UART1_TXEMPTY = LDMA_CH_REQSEL_SIGSEL_UART1TXEMPTY | LDMA_CH_REQSEL_SOURCESEL_UART1                ///< Trig on UART1_TXEMPTY.
  #endif
};
#endif

/// Data size of one LDMA transfer item.
SL_ENUM(DMADRV_DataSize_t) {
  dmadrvDataSize1 = ldmaCtrlSizeByte, ///< Byte
  dmadrvDataSize2 = ldmaCtrlSizeHalf, ///< Halfword
  dmadrvDataSize4 = ldmaCtrlSizeWord  ///< Word
};

#endif /* defined( LDMA_PRESENT ) && ( LDMA_COUNT == 1 ) */

Ecode_t DMADRV_AllocateChannel(unsigned int *channelId, void *capabilities);
Ecode_t DMADRV_DeInit(void);
Ecode_t DMADRV_FreeChannel(unsigned int channelId);
Ecode_t DMADRV_Init(void);

Ecode_t DMADRV_MemoryPeripheral(unsigned int          channelId,
                                DMADRV_PeripheralSignal_t peripheralSignal,
                                void                  *dst,
                                void                  *src,
                                bool                  srcInc,
                                int                   len,
                                DMADRV_DataSize_t     size,
                                DMADRV_Callback_t     callback,
                                void                  *cbUserParam);
Ecode_t DMADRV_PeripheralMemory(unsigned int          channelId,
                                DMADRV_PeripheralSignal_t peripheralSignal,
                                void                  *dst,
                                void                  *src,
                                bool                  dstInc,
                                int                   len,
                                DMADRV_DataSize_t     size,
                                DMADRV_Callback_t     callback,
                                void                  *cbUserParam);
Ecode_t DMADRV_MemoryPeripheralPingPong(unsigned int          channelId,
                                        DMADRV_PeripheralSignal_t peripheralSignal,
                                        void                  *dst,
                                        void                  *src0,
                                        void                  *src1,
                                        bool                  srcInc,
                                        int                   len,
                                        DMADRV_DataSize_t     size,
                                        DMADRV_Callback_t     callback,
                                        void                  *cbUserParam);
Ecode_t DMADRV_PeripheralMemoryPingPong(unsigned int          channelId,
                                        DMADRV_PeripheralSignal_t peripheralSignal,
                                        void                  *dst0,
                                        void                  *dst1,
                                        void                  *src,
                                        bool                  dstInc,
                                        int                   len,
                                        DMADRV_DataSize_t     size,
                                        DMADRV_Callback_t     callback,
                                        void                  *cbUserParam);

#if defined(EMDRV_DMADRV_LDMA)
Ecode_t DMADRV_LdmaStartTransfer(
  int                channelId,
  LDMA_TransferCfg_t *transfer,
  LDMA_Descriptor_t  *descriptor,
  DMADRV_Callback_t  callback,
  void               *cbUserParam);
#endif

Ecode_t DMADRV_PauseTransfer(unsigned int channelId);
Ecode_t DMADRV_ResumeTransfer(unsigned int channelId);
Ecode_t DMADRV_StopTransfer(unsigned int channelId);
Ecode_t DMADRV_TransferActive(unsigned int channelId, bool *active);
Ecode_t DMADRV_TransferCompletePending(unsigned int channelId, bool *pending);
Ecode_t DMADRV_TransferDone(unsigned int channelId, bool *done);
Ecode_t DMADRV_TransferRemainingCount(unsigned int channelId,
                                      int *remaining);

/** @} (end addtogroup dmadrv) */

#ifdef __cplusplus
}
#endif

#endif /* __SILICON_LABS_DMADRV_H__ */
