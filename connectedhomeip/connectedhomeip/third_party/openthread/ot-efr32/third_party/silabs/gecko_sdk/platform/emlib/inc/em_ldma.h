/***************************************************************************//**
 * @file
 * @brief Direct memory access (LDMA) API
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

#ifndef EM_LDMA_H
#define EM_LDMA_H

#include "em_device.h"

#if defined(LDMA_PRESENT) && (LDMA_COUNT == 1)

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup ldma LDMA - Linked DMA
 * @brief Linked Direct Memory Access (LDMA) Peripheral API
 *
 * @details
 * LDMA API functions provide full support for the LDMA peripheral.
 *
 * LDMA supports these DMA transfer types:
 *
 * @li Memory to memory.
 * @li Memory to peripheral.
 * @li Peripheral to memory.
 * @li Peripheral to peripheral.
 * @li Constant value to memory.
 *
 * LDMA supports linked lists of DMA descriptors allowing:
 *
 * @li Circular and ping-pong buffer transfers.
 * @li Scatter-gather transfers.
 * @li Looped transfers.
 *
 * LDMA has some advanced features:
 *
 * @li Intra-channel synchronization (SYNC), allowing hardware events to
 *     pause and restart a DMA sequence.
 * @li Immediate-write (WRI), allowing DMA to write a constant anywhere
 *     in the memory map.
 * @li Complex flow control allowing if-else constructs.
 *
 * Basic understanding of LDMA controller is assumed. Please refer to
 * the reference manual for further details. The LDMA examples described
 * in the reference manual are particularly helpful in understanding LDMA
 * operations.
 *
 * In order to use the DMA controller, the initialization function @ref
 * LDMA_Init() must have been executed once (normally during system initialization).
 *
 * DMA transfers are initiated by a call to @ref LDMA_StartTransfer(),
 * transfer properties are controlled by the contents of @ref LDMA_TransferCfg_t
 * and @ref LDMA_Descriptor_t structure parameters.
 * The @htmlonly LDMA_Descriptor_t @endhtmlonly structure parameter may be a
 * pointer to an array of descriptors, descriptors in array should
 * be linked together as needed.
 *
 * Transfer and descriptor initialization macros are provided for the most common
 * transfer types. Due to the flexibility of LDMA peripheral, only a small
 * subset of all possible initializer macros are provided, users should create
 * new ones when needed.
 *
 * <b> Examples of LDMA usage: </b>
 *
 * A simple memory to memory transfer:
 *
 * @include em_ldma_single.c
 *
 * @n A linked list of three memory to memory transfers:
 *
 * @include em_ldma_link_memory.c
 *
 * @n DMA from serial port peripheral to memory:
 *
 * @include em_ldma_peripheral.c
 *
 * @n Ping-pong DMA from serial port peripheral to memory:
 *
 * @include em_ldma_pingpong.c
 *
 * @note LDMA module does not implement LDMA interrupt handler. A
 * template for an LDMA IRQ handler is included here as an example.
 *
 * @include em_ldma_irq.c
 *
 * @{
 ******************************************************************************/

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

/**
 * Controls the number of unit data transfers per arbitration
 * cycle, providing a means to balance DMA channels' load on the controller.
 */
typedef enum {
  ldmaCtrlBlockSizeUnit1    = _LDMA_CH_CTRL_BLOCKSIZE_UNIT1,    /**< One transfer per arbitration.     */
  ldmaCtrlBlockSizeUnit2    = _LDMA_CH_CTRL_BLOCKSIZE_UNIT2,    /**< Two transfers per arbitration.    */
  ldmaCtrlBlockSizeUnit3    = _LDMA_CH_CTRL_BLOCKSIZE_UNIT3,    /**< Three transfers per arbitration.  */
  ldmaCtrlBlockSizeUnit4    = _LDMA_CH_CTRL_BLOCKSIZE_UNIT4,    /**< Four transfers per arbitration.   */
  ldmaCtrlBlockSizeUnit6    = _LDMA_CH_CTRL_BLOCKSIZE_UNIT6,    /**< Six transfers per arbitration.    */
  ldmaCtrlBlockSizeUnit8    = _LDMA_CH_CTRL_BLOCKSIZE_UNIT8,    /**< Eight transfers per arbitration.  */
  ldmaCtrlBlockSizeUnit16   = _LDMA_CH_CTRL_BLOCKSIZE_UNIT16,   /**< 16 transfers per arbitration.     */
  ldmaCtrlBlockSizeUnit32   = _LDMA_CH_CTRL_BLOCKSIZE_UNIT32,   /**< 32 transfers per arbitration.     */
  ldmaCtrlBlockSizeUnit64   = _LDMA_CH_CTRL_BLOCKSIZE_UNIT64,   /**< 64 transfers per arbitration.     */
  ldmaCtrlBlockSizeUnit128  = _LDMA_CH_CTRL_BLOCKSIZE_UNIT128,  /**< 128 transfers per arbitration.    */
  ldmaCtrlBlockSizeUnit256  = _LDMA_CH_CTRL_BLOCKSIZE_UNIT256,  /**< 256 transfers per arbitration.    */
  ldmaCtrlBlockSizeUnit512  = _LDMA_CH_CTRL_BLOCKSIZE_UNIT512,  /**< 512 transfers per arbitration.    */
  ldmaCtrlBlockSizeUnit1024 = _LDMA_CH_CTRL_BLOCKSIZE_UNIT1024, /**< 1024 transfers per arbitration.   */
  ldmaCtrlBlockSizeAll      = _LDMA_CH_CTRL_BLOCKSIZE_ALL       /**< Lock arbitration during transfer. */
} LDMA_CtrlBlockSize_t;

/** DMA structure type. */
typedef enum {
  ldmaCtrlStructTypeXfer  = _LDMA_CH_CTRL_STRUCTTYPE_TRANSFER,    /**< TRANSFER transfer type.    */
  ldmaCtrlStructTypeSync  = _LDMA_CH_CTRL_STRUCTTYPE_SYNCHRONIZE, /**< SYNCHRONIZE transfer type. */
  ldmaCtrlStructTypeWrite = _LDMA_CH_CTRL_STRUCTTYPE_WRITE        /**< WRITE transfer type.       */
} LDMA_CtrlStructType_t;

/** DMA transfer block or cycle selector. */
typedef enum {
  ldmaCtrlReqModeBlock = _LDMA_CH_CTRL_REQMODE_BLOCK, /**< Each DMA request trigger transfer of one block.     */
  ldmaCtrlReqModeAll   = _LDMA_CH_CTRL_REQMODE_ALL    /**< A DMA request trigger transfer of a complete cycle. */
} LDMA_CtrlReqMode_t;

/** Source address increment unit size. */
typedef enum {
  ldmaCtrlSrcIncOne  = _LDMA_CH_CTRL_SRCINC_ONE,  /**< Increment source address by one unit data size.   */
  ldmaCtrlSrcIncTwo  = _LDMA_CH_CTRL_SRCINC_TWO,  /**< Increment source address by two unit data sizes.  */
  ldmaCtrlSrcIncFour = _LDMA_CH_CTRL_SRCINC_FOUR, /**< Increment source address by four unit data sizes. */
  ldmaCtrlSrcIncNone = _LDMA_CH_CTRL_SRCINC_NONE  /**< Do not increment source address.                  */
} LDMA_CtrlSrcInc_t;

/** DMA transfer unit size. */
typedef enum {
  ldmaCtrlSizeByte = _LDMA_CH_CTRL_SIZE_BYTE,     /**< Each unit transfer is a byte.      */
  ldmaCtrlSizeHalf = _LDMA_CH_CTRL_SIZE_HALFWORD, /**< Each unit transfer is a half-word. */
  ldmaCtrlSizeWord = _LDMA_CH_CTRL_SIZE_WORD      /**< Each unit transfer is a word.      */
} LDMA_CtrlSize_t;

/** Destination address increment unit size. */
typedef enum {
  ldmaCtrlDstIncOne  = _LDMA_CH_CTRL_DSTINC_ONE,  /**< Increment destination address by one unit data size.   */
  ldmaCtrlDstIncTwo  = _LDMA_CH_CTRL_DSTINC_TWO,  /**< Increment destination address by two unit data sizes.  */
  ldmaCtrlDstIncFour = _LDMA_CH_CTRL_DSTINC_FOUR, /**< Increment destination address by four unit data sizes. */
  ldmaCtrlDstIncNone = _LDMA_CH_CTRL_DSTINC_NONE  /**< Do not increment destination address.                  */
} LDMA_CtrlDstInc_t;

/** Source addressing mode. */
typedef enum {
  ldmaCtrlSrcAddrModeAbs = _LDMA_CH_CTRL_SRCMODE_ABSOLUTE, /**< Address fetched from a linked structure is absolute.  */
  ldmaCtrlSrcAddrModeRel = _LDMA_CH_CTRL_SRCMODE_RELATIVE  /**< Address fetched from a linked structure is relative.  */
} LDMA_CtrlSrcAddrMode_t;

/** Destination addressing mode. */
typedef enum {
  ldmaCtrlDstAddrModeAbs = _LDMA_CH_CTRL_DSTMODE_ABSOLUTE, /**< Address fetched from a linked structure is absolute.  */
  ldmaCtrlDstAddrModeRel = _LDMA_CH_CTRL_DSTMODE_RELATIVE  /**< Address fetched from a linked structure is relative.  */
} LDMA_CtrlDstAddrMode_t;

/** DMA link load address mode. */
typedef enum {
  ldmaLinkModeAbs = _LDMA_CH_LINK_LINKMODE_ABSOLUTE, /**< Link address is an absolute address value.            */
  ldmaLinkModeRel = _LDMA_CH_LINK_LINKMODE_RELATIVE  /**< Link address is a two's complement relative address.  */
} LDMA_LinkMode_t;

/** Insert extra arbitration slots to increase channel arbitration priority. */
typedef enum {
  ldmaCfgArbSlotsAs1 = _LDMA_CH_CFG_ARBSLOTS_ONE,  /**< One arbitration slot selected.    */
  ldmaCfgArbSlotsAs2 = _LDMA_CH_CFG_ARBSLOTS_TWO,  /**< Two arbitration slots selected.   */
  ldmaCfgArbSlotsAs4 = _LDMA_CH_CFG_ARBSLOTS_FOUR, /**< Four arbitration slots selected.  */
  ldmaCfgArbSlotsAs8 = _LDMA_CH_CFG_ARBSLOTS_EIGHT /**< Eight arbitration slots selected. */
} LDMA_CfgArbSlots_t;

/** Source address increment sign. */
typedef enum {
  ldmaCfgSrcIncSignPos = _LDMA_CH_CFG_SRCINCSIGN_POSITIVE, /**< Increment source address. */
  ldmaCfgSrcIncSignNeg = _LDMA_CH_CFG_SRCINCSIGN_NEGATIVE  /**< Decrement source address. */
} LDMA_CfgSrcIncSign_t;

/** Destination address increment sign. */
typedef enum {
  ldmaCfgDstIncSignPos = _LDMA_CH_CFG_DSTINCSIGN_POSITIVE, /**< Increment destination address. */
  ldmaCfgDstIncSignNeg = _LDMA_CH_CFG_DSTINCSIGN_NEGATIVE  /**< Decrement destination address. */
} LDMA_CfgDstIncSign_t;

#if defined(_LDMA_CH_CFG_STRUCTBUSPORT_MASK)
/** Structure fetch operation bus port. */
typedef enum {
  ldmaCfgStructBusPort0 = _LDMA_CH_CFG_STRUCTBUSPORT_AHBM0, /**< AHB Master 0 port. */
  ldmaCfgStructBusPort1 = _LDMA_CH_CFG_STRUCTBUSPORT_AHBM1  /**< AHB Master 1 port. */
} LDMA_CfgStructBusPort_t;
#endif

#if defined(_LDMA_CH_CFG_SRCBUSPORT_MASK)
/** Source operation bus port. */
typedef enum {
  ldmaCfgSrcBusPort0 = _LDMA_CH_CFG_SRCBUSPORT_AHBM0, /**< AHB Master 0 port. */
  ldmaCfgSrcBusPort1 = _LDMA_CH_CFG_SRCBUSPORT_AHBM1  /**< AHB Master 1 port. */
} LDMA_CfgSrcBusPort_t;
#endif

#if defined(_LDMA_CH_CFG_DSTBUSPORT_MASK)
/** Destination operation bus port. */
typedef enum {
  ldmaCfgDstBusPort0 = _LDMA_CH_CFG_DSTBUSPORT_AHBM0, /**< AHB Master 0 port. */
  ldmaCfgDstBusPort1 = _LDMA_CH_CFG_DSTBUSPORT_AHBM1  /**< AHB Master 1 port. */
} LDMA_CfgDstBusPort_t;
#endif

#if defined(_LDMA_CH_CTRL_EXTEND_MASK)
/** Rules table mode (interleaving destination). */
typedef enum {
  ldmaIlModeAbsolute   = _LDMA_CH_XCTRL_ILMODE_ABSOLUTE,    /**< Address by value in rules. Size of WORD */
  ldmaIlModeRelative16 = _LDMA_CH_XCTRL_ILMODE_RELATIVE16,  /**< Address by adding rules to DST. size of HALFWORD */
  ldmaIlModeRelative8  = _LDMA_CH_XCTRL_ILMODE_RELATIVE8    /**< Address by adding rules to DST. size of BYTE */
} LDMA_RulesTblMode_t;
#endif

#if defined(LDMAXBAR_COUNT) && (LDMAXBAR_COUNT > 0)
/** Peripherals that can trigger LDMA transfers. */
typedef enum {
  ldmaPeripheralSignal_NONE = LDMAXBAR_CH_REQSEL_SOURCESEL_NONE,                                                                ///< No peripheral selected for DMA triggering.
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_LDMAXBARPRSREQ0
  ldmaPeripheralSignal_LDMAXBAR_PRSREQ0 = LDMAXBAR_CH_REQSEL_SIGSEL_LDMAXBARPRSREQ0 | LDMAXBAR_CH_REQSEL_SOURCESEL_LDMAXBAR,    ///< Trigger on PRS REQ0.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_LDMAXBARPRSREQ1
  ldmaPeripheralSignal_LDMAXBAR_PRSREQ1 = LDMAXBAR_CH_REQSEL_SIGSEL_LDMAXBARPRSREQ1 | LDMAXBAR_CH_REQSEL_SOURCESEL_LDMAXBAR,    ///< Trigger on PRS REQ1.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER0CC0
  ldmaPeripheralSignal_TIMER0_CC0 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER0CC0 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER0,                  ///< Trigger on TIMER0_CC0.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER0CC1
  ldmaPeripheralSignal_TIMER0_CC1 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER0CC1 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER0,                  ///< Trigger on TIMER0_CC1.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER0CC2
  ldmaPeripheralSignal_TIMER0_CC2 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER0CC2 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER0,                  ///< Trigger on TIMER0_CC2.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER0UFOF
  ldmaPeripheralSignal_TIMER0_UFOF = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER0UFOF | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER0,                ///< Trigger on TIMER0_UFOF.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER1CC0
  ldmaPeripheralSignal_TIMER1_CC0 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER1CC0 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER1,                  ///< Trigger on TIMER1_CC0.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER1CC1
  ldmaPeripheralSignal_TIMER1_CC1 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER1CC1 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER1,                  ///< Trigger on TIMER1_CC1.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER1CC2
  ldmaPeripheralSignal_TIMER1_CC2 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER1CC2 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER1,                  ///< Trigger on TIMER1_CC2.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER1UFOF
  ldmaPeripheralSignal_TIMER1_UFOF = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER1UFOF | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER1,                ///< Trigger on TIMER1_UFOF.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART0RXDATAV
  ldmaPeripheralSignal_USART0_RXDATAV = LDMAXBAR_CH_REQSEL_SIGSEL_USART0RXDATAV | LDMAXBAR_CH_REQSEL_SOURCESEL_USART0,          ///< Trigger on USART0_RXDATAV.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART0RXDATAVRIGHT
  ldmaPeripheralSignal_USART0_RXDATAVRIGHT = LDMAXBAR_CH_REQSEL_SIGSEL_USART0RXDATAVRIGHT | LDMAXBAR_CH_REQSEL_SOURCESEL_USART0,///< Trigger on USART0_RXDATAVRIGHT.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART0TXBL
  ldmaPeripheralSignal_USART0_TXBL = LDMAXBAR_CH_REQSEL_SIGSEL_USART0TXBL | LDMAXBAR_CH_REQSEL_SOURCESEL_USART0,                ///< Trigger on USART0_TXBL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART0TXBLRIGHT
  ldmaPeripheralSignal_USART0_TXBLRIGHT = LDMAXBAR_CH_REQSEL_SIGSEL_USART0TXBLRIGHT | LDMAXBAR_CH_REQSEL_SOURCESEL_USART0,      ///< Trigger on USART0_TXBLRIGHT.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART0TXEMPTY
  ldmaPeripheralSignal_USART0_TXEMPTY = LDMAXBAR_CH_REQSEL_SIGSEL_USART0TXEMPTY | LDMAXBAR_CH_REQSEL_SOURCESEL_USART0,          ///< Trigger on USART0_TXEMPTY.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART1RXDATAV
  ldmaPeripheralSignal_USART1_RXDATAV = LDMAXBAR_CH_REQSEL_SIGSEL_USART1RXDATAV | LDMAXBAR_CH_REQSEL_SOURCESEL_USART1,          ///< Trigger on USART1_RXDATAV.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART1RXDATAVRIGHT
  ldmaPeripheralSignal_USART1_RXDATAVRIGHT = LDMAXBAR_CH_REQSEL_SIGSEL_USART1RXDATAVRIGHT | LDMAXBAR_CH_REQSEL_SOURCESEL_USART1,///< Trigger on USART1_RXDATAVRIGHT.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART1TXBL
  ldmaPeripheralSignal_USART1_TXBL = LDMAXBAR_CH_REQSEL_SIGSEL_USART1TXBL | LDMAXBAR_CH_REQSEL_SOURCESEL_USART1,                ///< Trigger on USART1_TXBL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART1TXBLRIGHT
  ldmaPeripheralSignal_USART1_TXBLRIGHT = LDMAXBAR_CH_REQSEL_SIGSEL_USART1TXBLRIGHT | LDMAXBAR_CH_REQSEL_SOURCESEL_USART1,      ///< Trigger on USART1_TXBLRIGHT.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART1TXEMPTY
  ldmaPeripheralSignal_USART1_TXEMPTY = LDMAXBAR_CH_REQSEL_SIGSEL_USART1TXEMPTY | LDMAXBAR_CH_REQSEL_SOURCESEL_USART1,          ///< Trigger on USART1_TXEMPTY.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART2RXDATAV
  ldmaPeripheralSignal_USART2_RXDATAV = LDMAXBAR_CH_REQSEL_SIGSEL_USART2RXDATAV | LDMAXBAR_CH_REQSEL_SOURCESEL_USART2,          ///< Trigger on USART2_RXDATAV.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART2RXDATAVRIGHT
  ldmaPeripheralSignal_USART2_RXDATAVRIGHT = LDMAXBAR_CH_REQSEL_SIGSEL_USART2RXDATAVRIGHT | LDMAXBAR_CH_REQSEL_SOURCESEL_USART2,///< Trigger on USART2_RXDATAVRIGHT.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART2TXBL
  ldmaPeripheralSignal_USART2_TXBL = LDMAXBAR_CH_REQSEL_SIGSEL_USART2TXBL | LDMAXBAR_CH_REQSEL_SOURCESEL_USART2,                ///< Trigger on USART2_TXBL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART2TXBLRIGHT
  ldmaPeripheralSignal_USART2_TXBLRIGHT = LDMAXBAR_CH_REQSEL_SIGSEL_USART2TXBLRIGHT | LDMAXBAR_CH_REQSEL_SOURCESEL_USART2,      ///< Trigger on USART2_TXBLRIGHT.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_USART2TXEMPTY
  ldmaPeripheralSignal_USART2_TXEMPTY = LDMAXBAR_CH_REQSEL_SIGSEL_USART2TXEMPTY | LDMAXBAR_CH_REQSEL_SOURCESEL_USART2,          ///< Trigger on USART2_TXEMPTY.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_I2C0RXDATAV
  ldmaPeripheralSignal_I2C0_RXDATAV = LDMAXBAR_CH_REQSEL_SIGSEL_I2C0RXDATAV | LDMAXBAR_CH_REQSEL_SOURCESEL_I2C0,                ///< Trigger on I2C0_RXDATAV.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_I2C0TXBL
  ldmaPeripheralSignal_I2C0_TXBL = LDMAXBAR_CH_REQSEL_SIGSEL_I2C0TXBL | LDMAXBAR_CH_REQSEL_SOURCESEL_I2C0,                      ///< Trigger on I2C0_TXBL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_I2C1RXDATAV
  ldmaPeripheralSignal_I2C1_RXDATAV = LDMAXBAR_CH_REQSEL_SIGSEL_I2C1RXDATAV | LDMAXBAR_CH_REQSEL_SOURCESEL_I2C1,                ///< Trigger on I2C1_RXDATAV.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_I2C1TXBL
  ldmaPeripheralSignal_I2C1_TXBL = LDMAXBAR_CH_REQSEL_SIGSEL_I2C1TXBL | LDMAXBAR_CH_REQSEL_SOURCESEL_I2C1,                      ///< Trigger on I2C1_TXBL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_AGCRSSI
  ldmaPeripheralSignal_AGC_RSSI = LDMAXBAR_CH_REQSEL_SIGSEL_AGCRSSI | LDMAXBAR_CH_REQSEL_SOURCESEL_AGC,                         ///< Trigger on AGC_RSSI.
  #endif
  #if defined(LDMAXBAR_CH_REQSEL_SIGSEL_PDMRXDATAV)
  ldmaPeripheralSignal_PDM_RXDATAV = LDMAXBAR_CH_REQSEL_SIGSEL_PDMRXDATAV | LDMAXBAR_CH_REQSEL_SOURCESEL_PDM,                   ///< Trigger on PDM_RXDATAV.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERBOF
  ldmaPeripheralSignal_PROTIMER_BOF = LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERBOF | LDMAXBAR_CH_REQSEL_SOURCESEL_PROTIMER,            ///< Trigger on PROTIMER_BOF.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERCC0
  ldmaPeripheralSignal_PROTIMER_CC0 = LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERCC0 | LDMAXBAR_CH_REQSEL_SOURCESEL_PROTIMER,            ///< Trigger on PROTIMER_CC0.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERCC1
  ldmaPeripheralSignal_PROTIMER_CC1 = LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERCC1 | LDMAXBAR_CH_REQSEL_SOURCESEL_PROTIMER,            ///< Trigger on PROTIMER_CC1.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERCC2
  ldmaPeripheralSignal_PROTIMER_CC2 = LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERCC2 | LDMAXBAR_CH_REQSEL_SOURCESEL_PROTIMER,            ///< Trigger on PROTIMER_CC2.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERCC3
  ldmaPeripheralSignal_PROTIMER_CC3 = LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERCC3 | LDMAXBAR_CH_REQSEL_SOURCESEL_PROTIMER,            ///< Trigger on PROTIMER_CC3.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERCC4
  ldmaPeripheralSignal_PROTIMER_CC4 = LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERCC4 | LDMAXBAR_CH_REQSEL_SOURCESEL_PROTIMER,            ///< Trigger on PROTIMER_CC4.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERPOF
  ldmaPeripheralSignal_PROTIMER_POF = LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERPOF | LDMAXBAR_CH_REQSEL_SOURCESEL_PROTIMER,            ///< Trigger on PROTIMER_POF.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERWOF
  ldmaPeripheralSignal_PROTIMER_WOF = LDMAXBAR_CH_REQSEL_SIGSEL_PROTIMERWOF | LDMAXBAR_CH_REQSEL_SOURCESEL_PROTIMER,            ///< Trigger on PROTIMER_WOF.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_MODEMDEBUG
  ldmaPeripheralSignal_MODEM_DEBUG = LDMAXBAR_CH_REQSEL_SIGSEL_MODEMDEBUG | LDMAXBAR_CH_REQSEL_SOURCESEL_MODEM,                 ///< Trigger on MODEM_DEBUG.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_IADC0IADC_SCAN
  ldmaPeripheralSignal_IADC0_IADC_SCAN = LDMAXBAR_CH_REQSEL_SIGSEL_IADC0IADC_SCAN | LDMAXBAR_CH_REQSEL_SOURCESEL_IADC0,         ///< Trigger on IADC0_IADC_SCAN.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_IADC0IADC_SINGLE
  ldmaPeripheralSignal_IADC0_IADC_SINGLE = LDMAXBAR_CH_REQSEL_SIGSEL_IADC0IADC_SINGLE | LDMAXBAR_CH_REQSEL_SOURCESEL_IADC0,     ///< Trigger on IADC0_IADC_SINGLE.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_MSCWDATA
  ldmaPeripheralSignal_MSC_WDATA = LDMAXBAR_CH_REQSEL_SIGSEL_MSCWDATA | LDMAXBAR_CH_REQSEL_SOURCESEL_MSC,                       ///< Trigger on MSC_WDATA.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER2CC0
  ldmaPeripheralSignal_TIMER2_CC0 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER2CC0 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER2,                  ///< Trigger on TIMER2_CC0.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER2CC1
  ldmaPeripheralSignal_TIMER2_CC1 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER2CC1 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER2,                  ///< Trigger on TIMER2_CC1.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER2CC2
  ldmaPeripheralSignal_TIMER2_CC2 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER2CC2 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER2,                  ///< Trigger on TIMER2_CC2.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER2UFOF
  ldmaPeripheralSignal_TIMER2_UFOF = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER2UFOF | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER2,                ///< Trigger on TIMER2_UFOF.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER3CC0
  ldmaPeripheralSignal_TIMER3_CC0 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER3CC0 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER3,                  ///< Trigger on TIMER3_CC0.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER3CC1
  ldmaPeripheralSignal_TIMER3_CC1 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER3CC1 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER3,                  ///< Trigger on TIMER3_CC1.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER3CC2
  ldmaPeripheralSignal_TIMER3_CC2 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER3CC2 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER3,                  ///< Trigger on TIMER3_CC2.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER3UFOF
  ldmaPeripheralSignal_TIMER3_UFOF = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER3UFOF | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER3,                ///< Trigger on TIMER3_UFOF.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER5CC0
  ldmaPeripheralSignal_TIMER5_CC0 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER5CC0 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER5,                  ///< Trigger on TIMER5_CC0.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER5CC1
  ldmaPeripheralSignal_TIMER5_CC1 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER5CC1 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER5,                  ///< Trigger on TIMER5_CC1.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER5CC2
  ldmaPeripheralSignal_TIMER5_CC2 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER5CC2 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER5,                  ///< Trigger on TIMER5_CC2.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER5UFOF
  ldmaPeripheralSignal_TIMER5_UFOF = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER5UFOF | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER5,                ///< Trigger on TIMER5_UFOF.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER6CC0
  ldmaPeripheralSignal_TIMER6_CC0 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER6CC0 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER6,                  ///< Trigger on TIMER6_CC0.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER6CC1
  ldmaPeripheralSignal_TIMER6_CC1 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER6CC1 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER6,                  ///< Trigger on TIMER6_CC1.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER6CC2
  ldmaPeripheralSignal_TIMER6_CC2 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER6CC2 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER6,                  ///< Trigger on TIMER6_CC2.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER6UFOF
  ldmaPeripheralSignal_TIMER6_UFOF = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER6UFOF | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER6,                ///< Trigger on TIMER6_UFOF.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER7CC0
  ldmaPeripheralSignal_TIMER7_CC0 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER7CC0 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER7,                  ///< Trigger on TIMER7_CC0.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER7CC1
  ldmaPeripheralSignal_TIMER7_CC1 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER7CC1 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER7,                  ///< Trigger on TIMER7_CC1.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER7CC2
  ldmaPeripheralSignal_TIMER7_CC2 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER7CC2 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER7,                  ///< Trigger on TIMER7_CC2.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER7UFOF
  ldmaPeripheralSignal_TIMER7_UFOF = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER7UFOF | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER7,                ///< Trigger on TIMER7_UFOF.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_LCD
  ldmaPeripheralSignal_LCD = LDMAXBAR_CH_REQSEL_SIGSEL_LCD | LDMAXBAR_CH_REQSEL_SOURCESEL_LCD,
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER4CC0
  ldmaPeripheralSignal_TIMER4_CC0 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER4CC0 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER4,                  ///< Trigger on TIMER4_CC0.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER4CC1
  ldmaPeripheralSignal_TIMER4_CC1 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER4CC1 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER4,                  ///< Trigger on TIMER4_CC1.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER4CC2
  ldmaPeripheralSignal_TIMER4_CC2 = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER4CC2 | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER4,                  ///< Trigger on TIMER4_CC2.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_TIMER4UFOF
  ldmaPeripheralSignal_TIMER4_UFOF = LDMAXBAR_CH_REQSEL_SIGSEL_TIMER4UFOF | LDMAXBAR_CH_REQSEL_SOURCESEL_TIMER4,                ///< Trigger on TIMER4_UFOF.
  #endif
  #if defined(LDMAXBAR_CH_REQSEL_SIGSEL_VDAC0CH0_REQ)
  ldmaPeripheralSignal_VDAC0CH0REQ = LDMAXBAR_CH_REQSEL_SIGSEL_VDAC0CH0_REQ | LDMAXBAR_CH_REQSEL_SOURCESEL_VDAC0,               ///< Trigger on VDAC0_CH0REQ.
  #endif
  #if defined(LDMAXBAR_CH_REQSEL_SIGSEL_VDAC0CH1_REQ)
  ldmaPeripheralSignal_VDAC0CH1REQ = LDMAXBAR_CH_REQSEL_SIGSEL_VDAC0CH1_REQ | LDMAXBAR_CH_REQSEL_SOURCESEL_VDAC0,               ///< Trigger on VDAC0_CH1REQ.
  #endif
  #if defined(LDMAXBAR_CH_REQSEL_SIGSEL_VDAC1CH0_REQ)
  ldmaPeripheralSignal_VDAC1CH0REQ = LDMAXBAR_CH_REQSEL_SIGSEL_VDAC1CH0_REQ | LDMAXBAR_CH_REQSEL_SOURCESEL_VDAC1,               ///< Trigger on VDAC1_CH0REQ.
  #endif
  #if defined(LDMAXBAR_CH_REQSEL_SIGSEL_VDAC1CH1_REQ)
  ldmaPeripheralSignal_VDAC1CH1REQ = LDMAXBAR_CH_REQSEL_SIGSEL_VDAC1CH1_REQ | LDMAXBAR_CH_REQSEL_SOURCESEL_VDAC1,               ///< Trigger on VDAC1_CH1REQ.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_EUART0RXFL
  ldmaPeripheralSignal_EUART0_RXFL = LDMAXBAR_CH_REQSEL_SIGSEL_EUART0RXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUART0,                ///< Trigger on EUART0_RXFL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_EUART0TXFL
  ldmaPeripheralSignal_EUART0_TXFL = LDMAXBAR_CH_REQSEL_SIGSEL_EUART0TXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUART0,                ///< Trigger on EUART0_TXFL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_EUSART0RXFL
  ldmaPeripheralSignal_EUSART0_RXFL = LDMAXBAR_CH_REQSEL_SIGSEL_EUSART0RXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART0,             ///< Trigger on EUSART0_RXFL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_EUSART0TXFL
  ldmaPeripheralSignal_EUSART0_TXFL = LDMAXBAR_CH_REQSEL_SIGSEL_EUSART0TXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART0,             ///< Trigger on EUSART0_TXFL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_EUSART1RXFL
  ldmaPeripheralSignal_EUSART1_RXFL = LDMAXBAR_CH_REQSEL_SIGSEL_EUSART1RXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART1,             ///< Trigger on EUSART1_RXFL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_EUSART1TXFL
  ldmaPeripheralSignal_EUSART1_TXFL = LDMAXBAR_CH_REQSEL_SIGSEL_EUSART1TXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART1,             ///< Trigger on EUSART1_TXFL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_EUSART2RXFL
  ldmaPeripheralSignal_EUSART2_RXFL = LDMAXBAR_CH_REQSEL_SIGSEL_EUSART2RXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART2,             ///< Trigger on EUSART2_RXFL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_EUSART2TXFL
  ldmaPeripheralSignal_EUSART2_TXFL = LDMAXBAR_CH_REQSEL_SIGSEL_EUSART2TXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART2,             ///< Trigger on EUSART2_TXFL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_EUSART3RXFL
  ldmaPeripheralSignal_EUSART3_RXFL = LDMAXBAR_CH_REQSEL_SIGSEL_EUSART3RXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART3,             ///< Trigger on EUSART3_RXFL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_EUSART3TXFL
  ldmaPeripheralSignal_EUSART3_TXFL = LDMAXBAR_CH_REQSEL_SIGSEL_EUSART3TXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART3,             ///< Trigger on EUSART3_TXFL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_EUSART4RXFL
  ldmaPeripheralSignal_EUSART4_RXFL = LDMAXBAR_CH_REQSEL_SIGSEL_EUSART4RXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART4,             ///< Trigger on EUSART4_RXFL.
  #endif
  #if defined LDMAXBAR_CH_REQSEL_SIGSEL_EUSART4TXFL
  ldmaPeripheralSignal_EUSART4_TXFL = LDMAXBAR_CH_REQSEL_SIGSEL_EUSART4TXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART4,             ///< Trigger on EUSART4_TXFL.
  #endif
  #if defined(LDMAXBAR_CH_REQSEL_SIGSEL_LESENSEFIFO)
  ldmaPeripheralSignal_LESENSE_BUFDATAV = LDMAXBAR_CH_REQSEL_SIGSEL_LESENSEFIFO | LDMAXBAR_CH_REQSEL_SOURCESEL_LESENSE,        ///< Trigger on LESENSEFIFO.
  #endif
} LDMA_PeripheralSignal_t;

#else
/** Peripherals that can trigger LDMA transfers. */
typedef enum {
  ldmaPeripheralSignal_NONE = LDMA_CH_REQSEL_SOURCESEL_NONE,                                                                ///< No peripheral selected for DMA triggering.
  #if defined(LDMA_CH_REQSEL_SIGSEL_ADC0SCAN)
  ldmaPeripheralSignal_ADC0_SCAN = LDMA_CH_REQSEL_SIGSEL_ADC0SCAN | LDMA_CH_REQSEL_SOURCESEL_ADC0,                          ///< Trigger on ADC0_SCAN.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_ADC0SINGLE)
  ldmaPeripheralSignal_ADC0_SINGLE = LDMA_CH_REQSEL_SIGSEL_ADC0SINGLE | LDMA_CH_REQSEL_SOURCESEL_ADC0,                      ///< Trigger on ADC0_SINGLE.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_ADC1SCAN)
  ldmaPeripheralSignal_ADC1_SCAN = LDMA_CH_REQSEL_SIGSEL_ADC1SCAN | LDMA_CH_REQSEL_SOURCESEL_ADC1,                          ///< Trigger on ADC1_SCAN.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_ADC1SINGLE)
  ldmaPeripheralSignal_ADC1_SINGLE = LDMA_CH_REQSEL_SIGSEL_ADC1SINGLE | LDMA_CH_REQSEL_SOURCESEL_ADC1,                      ///< Trigger on ADC1_SINGLE.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTODATA0RD)
  ldmaPeripheralSignal_CRYPTO_DATA0RD = LDMA_CH_REQSEL_SIGSEL_CRYPTODATA0RD | LDMA_CH_REQSEL_SOURCESEL_CRYPTO,              ///< Trigger on CRYPTO_DATA0RD.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTODATA0WR)
  ldmaPeripheralSignal_CRYPTO_DATA0WR = LDMA_CH_REQSEL_SIGSEL_CRYPTODATA0WR | LDMA_CH_REQSEL_SOURCESEL_CRYPTO,              ///< Trigger on CRYPTO_DATA0WR.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTODATA0XWR)
  ldmaPeripheralSignal_CRYPTO_DATA0XWR = LDMA_CH_REQSEL_SIGSEL_CRYPTODATA0XWR | LDMA_CH_REQSEL_SOURCESEL_CRYPTO,            ///< Trigger on CRYPTO_DATA0XWR.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTODATA1RD)
  ldmaPeripheralSignal_CRYPTO_DATA1RD = LDMA_CH_REQSEL_SIGSEL_CRYPTODATA1RD | LDMA_CH_REQSEL_SOURCESEL_CRYPTO,              ///< Trigger on CRYPTO_DATA1RD.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTODATA1WR)
  ldmaPeripheralSignal_CRYPTO_DATA1WR = LDMA_CH_REQSEL_SIGSEL_CRYPTODATA1WR | LDMA_CH_REQSEL_SOURCESEL_CRYPTO,              ///< Trigger on CRYPTO_DATA1WR.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTO0DATA0RD)
  ldmaPeripheralSignal_CRYPTO0_DATA0RD = LDMA_CH_REQSEL_SIGSEL_CRYPTO0DATA0RD | LDMA_CH_REQSEL_SOURCESEL_CRYPTO0,           ///< Trigger on CRYPTO0_DATA0RD.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTO0DATA0WR)
  ldmaPeripheralSignal_CRYPTO0_DATA0WR = LDMA_CH_REQSEL_SIGSEL_CRYPTO0DATA0WR | LDMA_CH_REQSEL_SOURCESEL_CRYPTO0,           ///< Trigger on CRYPTO0_DATA0WR.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTO0DATA0XWR)
  ldmaPeripheralSignal_CRYPTO0_DATA0XWR = LDMA_CH_REQSEL_SIGSEL_CRYPTO0DATA0XWR | LDMA_CH_REQSEL_SOURCESEL_CRYPTO0,         ///< Trigger on CRYPTO0_DATA0XWR.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTO0DATA1RD)
  ldmaPeripheralSignal_CRYPTO0_DATA1RD = LDMA_CH_REQSEL_SIGSEL_CRYPTO0DATA1RD | LDMA_CH_REQSEL_SOURCESEL_CRYPTO0,           ///< Trigger on CRYPTO0_DATA1RD.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTO0DATA1WR)
  ldmaPeripheralSignal_CRYPTO0_DATA1WR = LDMA_CH_REQSEL_SIGSEL_CRYPTO0DATA1WR | LDMA_CH_REQSEL_SOURCESEL_CRYPTO0,           ///< Trigger on CRYPTO0_DATA1WR.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTO1DATA0RD)
  ldmaPeripheralSignal_CRYPTO1_DATA0RD = LDMA_CH_REQSEL_SIGSEL_CRYPTO1DATA0RD | LDMA_CH_REQSEL_SOURCESEL_CRYPTO1,           ///< Trigger on CRYPTO1_DATA0RD.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTO1DATA0WR)
  ldmaPeripheralSignal_CRYPTO1_DATA0WR = LDMA_CH_REQSEL_SIGSEL_CRYPTO1DATA0WR | LDMA_CH_REQSEL_SOURCESEL_CRYPTO1,           ///< Trigger on CRYPTO1_DATA0WR.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTO1DATA0XWR)
  ldmaPeripheralSignal_CRYPTO1_DATA0XWR = LDMA_CH_REQSEL_SIGSEL_CRYPTO1DATA0XWR | LDMA_CH_REQSEL_SOURCESEL_CRYPTO1,         ///< Trigger on CRYPTO1_DATA0XWR.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTO1DATA1RD)
  ldmaPeripheralSignal_CRYPTO1_DATA1RD = LDMA_CH_REQSEL_SIGSEL_CRYPTO1DATA1RD | LDMA_CH_REQSEL_SOURCESEL_CRYPTO1,           ///< Trigger on CRYPTO1_DATA1RD.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CRYPTO1DATA1WR)
  ldmaPeripheralSignal_CRYPTO1_DATA1WR = LDMA_CH_REQSEL_SIGSEL_CRYPTO1DATA1WR | LDMA_CH_REQSEL_SOURCESEL_CRYPTO1,           ///< Trigger on CRYPTO1_DATA1WR.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CSENBSLN)
  ldmaPeripheralSignal_CSEN_BSLN = LDMA_CH_REQSEL_SIGSEL_CSENBSLN | LDMA_CH_REQSEL_SOURCESEL_CSEN,                          ///< Trigger on CSEN_BSLN.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_CSENDATA)
  ldmaPeripheralSignal_CSEN_DATA = LDMA_CH_REQSEL_SIGSEL_CSENDATA | LDMA_CH_REQSEL_SOURCESEL_CSEN,                          ///< Trigger on CSEN_DATA.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_EBIPXL0EMPTY)
  ldmaPeripheralSignal_EBI_PXL0EMPTY = LDMA_CH_REQSEL_SIGSEL_EBIPXL0EMPTY | LDMA_CH_REQSEL_SOURCESEL_EBI,                   ///< Trigger on EBI_PXL0EMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_EBIPXL1EMPTY)
  ldmaPeripheralSignal_EBI_PXL1EMPTY = LDMA_CH_REQSEL_SIGSEL_EBIPXL1EMPTY | LDMA_CH_REQSEL_SOURCESEL_EBI,                   ///< Trigger on EBI_PXL1EMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_EBIPXLFULL)
  ldmaPeripheralSignal_EBI_PXLFULL = LDMA_CH_REQSEL_SIGSEL_EBIPXLFULL | LDMA_CH_REQSEL_SOURCESEL_EBI,                       ///< Trigger on EBI_PXLFULL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_EBIDDEMPTY)
  ldmaPeripheralSignal_EBI_DDEMPTY = LDMA_CH_REQSEL_SIGSEL_EBIDDEMPTY | LDMA_CH_REQSEL_SOURCESEL_EBI,                       ///< Trigger on EBI_DDEMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_EBIVSYNC)
  ldmaPeripheralSignal_EBI_VSYNC = LDMA_CH_REQSEL_SIGSEL_EBIVSYNC | LDMA_CH_REQSEL_SOURCESEL_EBI,                           ///< Trigger on EBI_VSYNC.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_EBIHSYNC)
  ldmaPeripheralSignal_EBI_HSYNC = LDMA_CH_REQSEL_SIGSEL_EBIHSYNC | LDMA_CH_REQSEL_SOURCESEL_EBI,                           ///< Trigger on EBI_HSYNC.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_I2C0RXDATAV)
  ldmaPeripheralSignal_I2C0_RXDATAV = LDMA_CH_REQSEL_SIGSEL_I2C0RXDATAV | LDMA_CH_REQSEL_SOURCESEL_I2C0,                    ///< Trigger on I2C0_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_I2C0TXBL)
  ldmaPeripheralSignal_I2C0_TXBL = LDMA_CH_REQSEL_SIGSEL_I2C0TXBL | LDMA_CH_REQSEL_SOURCESEL_I2C0,                          ///< Trigger on I2C0_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_I2C1RXDATAV)
  ldmaPeripheralSignal_I2C1_RXDATAV = LDMA_CH_REQSEL_SIGSEL_I2C1RXDATAV | LDMA_CH_REQSEL_SOURCESEL_I2C1,                    ///< Trigger on I2C1_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_I2C1TXBL)
  ldmaPeripheralSignal_I2C1_TXBL = LDMA_CH_REQSEL_SIGSEL_I2C1TXBL | LDMA_CH_REQSEL_SOURCESEL_I2C1,                          ///< Trigger on I2C1_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_I2C2RXDATAV)
  ldmaPeripheralSignal_I2C2_RXDATAV = LDMA_CH_REQSEL_SIGSEL_I2C2RXDATAV | LDMA_CH_REQSEL_SOURCESEL_I2C2,                    ///< Trigger on I2C2_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_I2C2TXBL)
  ldmaPeripheralSignal_I2C2_TXBL = LDMA_CH_REQSEL_SIGSEL_I2C2TXBL | LDMA_CH_REQSEL_SOURCESEL_I2C2,                          ///< Trigger on I2C2_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_LESENSEBUFDATAV)
  ldmaPeripheralSignal_LESENSE_BUFDATAV = LDMA_CH_REQSEL_SIGSEL_LESENSEBUFDATAV | LDMA_CH_REQSEL_SOURCESEL_LESENSE,         ///< Trigger on LESENSE_BUFDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_LEUART0RXDATAV)
  ldmaPeripheralSignal_LEUART0_RXDATAV = LDMA_CH_REQSEL_SIGSEL_LEUART0RXDATAV | LDMA_CH_REQSEL_SOURCESEL_LEUART0,           ///< Trigger on LEUART0_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_LEUART0TXBL)
  ldmaPeripheralSignal_LEUART0_TXBL = LDMA_CH_REQSEL_SIGSEL_LEUART0TXBL | LDMA_CH_REQSEL_SOURCESEL_LEUART0,                 ///< Trigger on LEUART0_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_LEUART0TXEMPTY)
  ldmaPeripheralSignal_LEUART0_TXEMPTY = LDMA_CH_REQSEL_SIGSEL_LEUART0TXEMPTY | LDMA_CH_REQSEL_SOURCESEL_LEUART0,           ///< Trigger on LEUART0_TXEMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_LEUART1RXDATAV)
  ldmaPeripheralSignal_LEUART1_RXDATAV = LDMA_CH_REQSEL_SIGSEL_LEUART1RXDATAV | LDMA_CH_REQSEL_SOURCESEL_LEUART1,           ///< Trigger on LEUART1_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_LEUART1TXBL)
  ldmaPeripheralSignal_LEUART1_TXBL = LDMA_CH_REQSEL_SIGSEL_LEUART1TXBL | LDMA_CH_REQSEL_SOURCESEL_LEUART1,                 ///< Trigger on LEUART1_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_LEUART1TXEMPTY)
  ldmaPeripheralSignal_LEUART1_TXEMPTY = LDMA_CH_REQSEL_SIGSEL_LEUART1TXEMPTY | LDMA_CH_REQSEL_SOURCESEL_LEUART1,           ///< Trigger on LEUART1_TXEMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_MSCWDATA)
  ldmaPeripheralSignal_MSC_WDATA = LDMA_CH_REQSEL_SIGSEL_MSCWDATA | LDMA_CH_REQSEL_SOURCESEL_MSC,                           ///< Trigger on MSC_WDATA.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_PDMRXDATAV)
  ldmaPeripheralSignal_PDM_RXDATAV = LDMA_CH_REQSEL_SIGSEL_PDMRXDATAV | LDMA_CH_REQSEL_SOURCESEL_PDM,                       ///< Trigger on PDM_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_PRSREQ0)
  ldmaPeripheralSignal_PRS_REQ0 = LDMA_CH_REQSEL_SIGSEL_PRSREQ0 | LDMA_CH_REQSEL_SOURCESEL_PRS,                             ///< Trigger on PRS_REQ0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_PRSREQ1)
  ldmaPeripheralSignal_PRS_REQ1 = LDMA_CH_REQSEL_SIGSEL_PRSREQ1 | LDMA_CH_REQSEL_SOURCESEL_PRS,                             ///< Trigger on PRS_REQ1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER0CC0)
  ldmaPeripheralSignal_TIMER0_CC0 = LDMA_CH_REQSEL_SIGSEL_TIMER0CC0 | LDMA_CH_REQSEL_SOURCESEL_TIMER0,                      ///< Trigger on TIMER0_CC0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER0CC1)
  ldmaPeripheralSignal_TIMER0_CC1 = LDMA_CH_REQSEL_SIGSEL_TIMER0CC1 | LDMA_CH_REQSEL_SOURCESEL_TIMER0,                      ///< Trigger on TIMER0_CC1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER0CC2)
  ldmaPeripheralSignal_TIMER0_CC2 = LDMA_CH_REQSEL_SIGSEL_TIMER0CC2 | LDMA_CH_REQSEL_SOURCESEL_TIMER0,                      ///< Trigger on TIMER0_CC2.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER0UFOF)
  ldmaPeripheralSignal_TIMER0_UFOF = LDMA_CH_REQSEL_SIGSEL_TIMER0UFOF | LDMA_CH_REQSEL_SOURCESEL_TIMER0,                    ///< Trigger on TIMER0_UFOF.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER1CC0)
  ldmaPeripheralSignal_TIMER1_CC0 = LDMA_CH_REQSEL_SIGSEL_TIMER1CC0 | LDMA_CH_REQSEL_SOURCESEL_TIMER1,                      ///< Trigger on TIMER1_CC0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER1CC1)
  ldmaPeripheralSignal_TIMER1_CC1 = LDMA_CH_REQSEL_SIGSEL_TIMER1CC1 | LDMA_CH_REQSEL_SOURCESEL_TIMER1,                      ///< Trigger on TIMER1_CC1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER1CC2)
  ldmaPeripheralSignal_TIMER1_CC2 = LDMA_CH_REQSEL_SIGSEL_TIMER1CC2 | LDMA_CH_REQSEL_SOURCESEL_TIMER1,                      ///< Trigger on TIMER1_CC2.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER1CC3)
  ldmaPeripheralSignal_TIMER1_CC3 = LDMA_CH_REQSEL_SIGSEL_TIMER1CC3 | LDMA_CH_REQSEL_SOURCESEL_TIMER1,                      ///< Trigger on TIMER1_CC3.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER1UFOF)
  ldmaPeripheralSignal_TIMER1_UFOF = LDMA_CH_REQSEL_SIGSEL_TIMER1UFOF | LDMA_CH_REQSEL_SOURCESEL_TIMER1,                    ///< Trigger on TIMER1_UFOF.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER2CC0)
  ldmaPeripheralSignal_TIMER2_CC0 = LDMA_CH_REQSEL_SIGSEL_TIMER2CC0 | LDMA_CH_REQSEL_SOURCESEL_TIMER2,                      ///< Trigger on TIMER2_CC0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER2CC1)
  ldmaPeripheralSignal_TIMER2_CC1 = LDMA_CH_REQSEL_SIGSEL_TIMER2CC1 | LDMA_CH_REQSEL_SOURCESEL_TIMER2,                      ///< Trigger on TIMER2_CC1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER2CC2)
  ldmaPeripheralSignal_TIMER2_CC2 = LDMA_CH_REQSEL_SIGSEL_TIMER2CC2 | LDMA_CH_REQSEL_SOURCESEL_TIMER2,                      ///< Trigger on TIMER2_CC2.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER2UFOF)
  ldmaPeripheralSignal_TIMER2_UFOF = LDMA_CH_REQSEL_SIGSEL_TIMER2UFOF | LDMA_CH_REQSEL_SOURCESEL_TIMER2,                    ///< Trigger on TIMER2_UFOF.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER3CC0)
  ldmaPeripheralSignal_TIMER3_CC0 = LDMA_CH_REQSEL_SIGSEL_TIMER3CC0 | LDMA_CH_REQSEL_SOURCESEL_TIMER3,                      ///< Trigger on TIMER3_CC0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER3CC1)
  ldmaPeripheralSignal_TIMER3_CC1 = LDMA_CH_REQSEL_SIGSEL_TIMER3CC1 | LDMA_CH_REQSEL_SOURCESEL_TIMER3,                      ///< Trigger on TIMER3_CC1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER3CC2)
  ldmaPeripheralSignal_TIMER3_CC2 = LDMA_CH_REQSEL_SIGSEL_TIMER3CC2 | LDMA_CH_REQSEL_SOURCESEL_TIMER3,                      ///< Trigger on TIMER3_CC2.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER3UFOF)
  ldmaPeripheralSignal_TIMER3_UFOF = LDMA_CH_REQSEL_SIGSEL_TIMER3UFOF | LDMA_CH_REQSEL_SOURCESEL_TIMER3,                    ///< Trigger on TIMER3_UFOF.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER4CC0)
  ldmaPeripheralSignal_TIMER4_CC0 = LDMA_CH_REQSEL_SIGSEL_TIMER4CC0 | LDMA_CH_REQSEL_SOURCESEL_TIMER4,                      ///< Trigger on TIMER4_CC0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER4CC1)
  ldmaPeripheralSignal_TIMER4_CC1 = LDMA_CH_REQSEL_SIGSEL_TIMER4CC1 | LDMA_CH_REQSEL_SOURCESEL_TIMER4,                      ///< Trigger on TIMER4_CC1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER4CC2)
  ldmaPeripheralSignal_TIMER4_CC2 = LDMA_CH_REQSEL_SIGSEL_TIMER4CC2 | LDMA_CH_REQSEL_SOURCESEL_TIMER4,                      ///< Trigger on TIMER4_CC2.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER4UFOF)
  ldmaPeripheralSignal_TIMER4_UFOF = LDMA_CH_REQSEL_SIGSEL_TIMER4UFOF | LDMA_CH_REQSEL_SOURCESEL_TIMER4,                    ///< Trigger on TIMER4_UFOF.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER5CC0)
  ldmaPeripheralSignal_TIMER5_CC0 = LDMA_CH_REQSEL_SIGSEL_TIMER5CC0 | LDMA_CH_REQSEL_SOURCESEL_TIMER5,                      ///< Trigger on TIMER5_CC0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER5CC1)
  ldmaPeripheralSignal_TIMER5_CC1 = LDMA_CH_REQSEL_SIGSEL_TIMER5CC1 | LDMA_CH_REQSEL_SOURCESEL_TIMER5,                      ///< Trigger on TIMER5_CC1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER5CC2)
  ldmaPeripheralSignal_TIMER5_CC2 = LDMA_CH_REQSEL_SIGSEL_TIMER5CC2 | LDMA_CH_REQSEL_SOURCESEL_TIMER5,                      ///< Trigger on TIMER5_CC2.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER5UFOF)
  ldmaPeripheralSignal_TIMER5_UFOF = LDMA_CH_REQSEL_SIGSEL_TIMER5UFOF | LDMA_CH_REQSEL_SOURCESEL_TIMER5,                    ///< Trigger on TIMER5_UFOF.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER6CC0)
  ldmaPeripheralSignal_TIMER6_CC0 = LDMA_CH_REQSEL_SIGSEL_TIMER6CC0 | LDMA_CH_REQSEL_SOURCESEL_TIMER6,                      ///< Trigger on TIMER6_CC0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER6CC1)
  ldmaPeripheralSignal_TIMER6_CC1 = LDMA_CH_REQSEL_SIGSEL_TIMER6CC1 | LDMA_CH_REQSEL_SOURCESEL_TIMER6,                      ///< Trigger on TIMER6_CC1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER6CC2)
  ldmaPeripheralSignal_TIMER6_CC2 = LDMA_CH_REQSEL_SIGSEL_TIMER6CC2 | LDMA_CH_REQSEL_SOURCESEL_TIMER6,                      ///< Trigger on TIMER6_CC2.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_TIMER6UFOF)
  ldmaPeripheralSignal_TIMER6_UFOF = LDMA_CH_REQSEL_SIGSEL_TIMER6UFOF | LDMA_CH_REQSEL_SOURCESEL_TIMER6,                    ///< Trigger on TIMER6_UFOF.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_UART0RXDATAV)
  ldmaPeripheralSignal_UART0_RXDATAV = LDMA_CH_REQSEL_SIGSEL_UART0RXDATAV | LDMA_CH_REQSEL_SOURCESEL_UART0,                 ///< Trigger on UART0_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_UART0TXBL)
  ldmaPeripheralSignal_UART0_TXBL = LDMA_CH_REQSEL_SIGSEL_UART0TXBL | LDMA_CH_REQSEL_SOURCESEL_UART0,                       ///< Trigger on UART0_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_UART0TXEMPTY)
  ldmaPeripheralSignal_UART0_TXEMPTY = LDMA_CH_REQSEL_SIGSEL_UART0TXEMPTY | LDMA_CH_REQSEL_SOURCESEL_UART0,                 ///< Trigger on UART0_TXEMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_UART1RXDATAV)
  ldmaPeripheralSignal_UART1_RXDATAV = LDMA_CH_REQSEL_SIGSEL_UART1RXDATAV | LDMA_CH_REQSEL_SOURCESEL_UART1,                 ///< Trigger on UART1_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_UART1TXBL)
  ldmaPeripheralSignal_UART1_TXBL = LDMA_CH_REQSEL_SIGSEL_UART1TXBL | LDMA_CH_REQSEL_SOURCESEL_UART1,                       ///< Trigger on UART1_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_UART1TXEMPTY)
  ldmaPeripheralSignal_UART1_TXEMPTY = LDMA_CH_REQSEL_SIGSEL_UART1TXEMPTY | LDMA_CH_REQSEL_SOURCESEL_UART1,                 ///< Trigger on UART1_TXEMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART0RXDATAV)
  ldmaPeripheralSignal_USART0_RXDATAV = LDMA_CH_REQSEL_SIGSEL_USART0RXDATAV | LDMA_CH_REQSEL_SOURCESEL_USART0,              ///< Trigger on USART0_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART0TXBL)
  ldmaPeripheralSignal_USART0_TXBL = LDMA_CH_REQSEL_SIGSEL_USART0TXBL | LDMA_CH_REQSEL_SOURCESEL_USART0,                    ///< Trigger on USART0_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART0TXEMPTY)
  ldmaPeripheralSignal_USART0_TXEMPTY = LDMA_CH_REQSEL_SIGSEL_USART0TXEMPTY | LDMA_CH_REQSEL_SOURCESEL_USART0,              ///< Trigger on USART0_TXEMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART1RXDATAV)
  ldmaPeripheralSignal_USART1_RXDATAV = LDMA_CH_REQSEL_SIGSEL_USART1RXDATAV | LDMA_CH_REQSEL_SOURCESEL_USART1,              ///< Trigger on USART1_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART1RXDATAVRIGHT)
  ldmaPeripheralSignal_USART1_RXDATAVRIGHT = LDMA_CH_REQSEL_SIGSEL_USART1RXDATAVRIGHT | LDMA_CH_REQSEL_SOURCESEL_USART1,    ///< Trigger on USART1_RXDATAVRIGHT.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART1TXBL)
  ldmaPeripheralSignal_USART1_TXBL = LDMA_CH_REQSEL_SIGSEL_USART1TXBL | LDMA_CH_REQSEL_SOURCESEL_USART1,                    ///< Trigger on USART1_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART1TXBLRIGHT)
  ldmaPeripheralSignal_USART1_TXBLRIGHT = LDMA_CH_REQSEL_SIGSEL_USART1TXBLRIGHT | LDMA_CH_REQSEL_SOURCESEL_USART1,          ///< Trigger on USART1_TXBLRIGHT.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART1TXEMPTY)
  ldmaPeripheralSignal_USART1_TXEMPTY = LDMA_CH_REQSEL_SIGSEL_USART1TXEMPTY | LDMA_CH_REQSEL_SOURCESEL_USART1,              ///< Trigger on USART1_TXEMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART2RXDATAV)
  ldmaPeripheralSignal_USART2_RXDATAV = LDMA_CH_REQSEL_SIGSEL_USART2RXDATAV | LDMA_CH_REQSEL_SOURCESEL_USART2,              ///< Trigger on USART2_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART2TXBL)
  ldmaPeripheralSignal_USART2_TXBL = LDMA_CH_REQSEL_SIGSEL_USART2TXBL | LDMA_CH_REQSEL_SOURCESEL_USART2,                    ///< Trigger on USART2_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART2TXEMPTY)
  ldmaPeripheralSignal_USART2_TXEMPTY = LDMA_CH_REQSEL_SIGSEL_USART2TXEMPTY | LDMA_CH_REQSEL_SOURCESEL_USART2,              ///< Trigger on USART2_TXEMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART3RXDATAV)
  ldmaPeripheralSignal_USART3_RXDATAV = LDMA_CH_REQSEL_SIGSEL_USART3RXDATAV | LDMA_CH_REQSEL_SOURCESEL_USART3,              ///< Trigger on USART3_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART3RXDATAVRIGHT)
  ldmaPeripheralSignal_USART3_RXDATAVRIGHT = LDMA_CH_REQSEL_SIGSEL_USART3RXDATAVRIGHT | LDMA_CH_REQSEL_SOURCESEL_USART3,    ///< Trigger on USART3_RXDATAVRIGHT.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART3TXBL)
  ldmaPeripheralSignal_USART3_TXBL = LDMA_CH_REQSEL_SIGSEL_USART3TXBL | LDMA_CH_REQSEL_SOURCESEL_USART3,                    ///< Trigger on USART3_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART3TXBLRIGHT)
  ldmaPeripheralSignal_USART3_TXBLRIGHT = LDMA_CH_REQSEL_SIGSEL_USART3TXBLRIGHT | LDMA_CH_REQSEL_SOURCESEL_USART3,          ///< Trigger on USART3_TXBLRIGHT.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART3TXEMPTY)
  ldmaPeripheralSignal_USART3_TXEMPTY = LDMA_CH_REQSEL_SIGSEL_USART3TXEMPTY | LDMA_CH_REQSEL_SOURCESEL_USART3,              ///< Trigger on USART3_TXEMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART4RXDATAV)
  ldmaPeripheralSignal_USART4_RXDATAV = LDMA_CH_REQSEL_SIGSEL_USART4RXDATAV | LDMA_CH_REQSEL_SOURCESEL_USART4,              ///< Trigger on USART4_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART4RXDATAVRIGHT)
  ldmaPeripheralSignal_USART4_RXDATAVRIGHT = LDMA_CH_REQSEL_SIGSEL_USART4RXDATAVRIGHT | LDMA_CH_REQSEL_SOURCESEL_USART4,    ///< Trigger on USART4_RXDATAVRIGHT.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART4TXBL)
  ldmaPeripheralSignal_USART4_TXBL = LDMA_CH_REQSEL_SIGSEL_USART4TXBL | LDMA_CH_REQSEL_SOURCESEL_USART4,                    ///< Trigger on USART4_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART4TXBLRIGHT)
  ldmaPeripheralSignal_USART4_TXBLRIGHT = LDMA_CH_REQSEL_SIGSEL_USART4TXBLRIGHT | LDMA_CH_REQSEL_SOURCESEL_USART4,          ///< Trigger on USART4_TXBLRIGHT.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART4TXEMPTY)
  ldmaPeripheralSignal_USART4_TXEMPTY = LDMA_CH_REQSEL_SIGSEL_USART4TXEMPTY | LDMA_CH_REQSEL_SOURCESEL_USART4,              ///< Trigger on USART4_TXEMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART5RXDATAV)
  ldmaPeripheralSignal_USART5_RXDATAV = LDMA_CH_REQSEL_SIGSEL_USART5RXDATAV | LDMA_CH_REQSEL_SOURCESEL_USART5,              ///< Trigger on USART5_RXDATAV.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART5TXBL)
  ldmaPeripheralSignal_USART5_TXBL = LDMA_CH_REQSEL_SIGSEL_USART5TXBL | LDMA_CH_REQSEL_SOURCESEL_USART5,                    ///< Trigger on USART5_TXBL.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_USART5TXEMPTY)
  ldmaPeripheralSignal_USART5_TXEMPTY = LDMA_CH_REQSEL_SIGSEL_USART5TXEMPTY | LDMA_CH_REQSEL_SOURCESEL_USART5,              ///< Trigger on USART5_TXEMPTY.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_VDAC0CH0)
  ldmaPeripheralSignal_VDAC0_CH0 = LDMA_CH_REQSEL_SIGSEL_VDAC0CH0 | LDMA_CH_REQSEL_SOURCESEL_VDAC0,                         ///< Trigger on VDAC0_CH0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_VDAC0CH1)
  ldmaPeripheralSignal_VDAC0_CH1 = LDMA_CH_REQSEL_SIGSEL_VDAC0CH1 | LDMA_CH_REQSEL_SOURCESEL_VDAC0,                         ///< Trigger on VDAC0_CH1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER0CC0)
  ldmaPeripheralSignal_WTIMER0_CC0 = LDMA_CH_REQSEL_SIGSEL_WTIMER0CC0 | LDMA_CH_REQSEL_SOURCESEL_WTIMER0,                   ///< Trigger on WTIMER0_CC0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER0CC1)
  ldmaPeripheralSignal_WTIMER0_CC1 = LDMA_CH_REQSEL_SIGSEL_WTIMER0CC1 | LDMA_CH_REQSEL_SOURCESEL_WTIMER0,                   ///< Trigger on WTIMER0_CC1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER0CC2)
  ldmaPeripheralSignal_WTIMER0_CC2 = LDMA_CH_REQSEL_SIGSEL_WTIMER0CC2 | LDMA_CH_REQSEL_SOURCESEL_WTIMER0,                   ///< Trigger on WTIMER0_CC2.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER0UFOF)
  ldmaPeripheralSignal_WTIMER0_UFOF = LDMA_CH_REQSEL_SIGSEL_WTIMER0UFOF | LDMA_CH_REQSEL_SOURCESEL_WTIMER0,                 ///< Trigger on WTIMER0_UFOF.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER1CC0)
  ldmaPeripheralSignal_WTIMER1_CC0 = LDMA_CH_REQSEL_SIGSEL_WTIMER1CC0 | LDMA_CH_REQSEL_SOURCESEL_WTIMER1,                   ///< Trigger on WTIMER1_CC0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER1CC1)
  ldmaPeripheralSignal_WTIMER1_CC1 = LDMA_CH_REQSEL_SIGSEL_WTIMER1CC1 | LDMA_CH_REQSEL_SOURCESEL_WTIMER1,                   ///< Trigger on WTIMER1_CC1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER1CC2)
  ldmaPeripheralSignal_WTIMER1_CC2 = LDMA_CH_REQSEL_SIGSEL_WTIMER1CC2 | LDMA_CH_REQSEL_SOURCESEL_WTIMER1,                   ///< Trigger on WTIMER1_CC2.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER1CC3)
  ldmaPeripheralSignal_WTIMER1_CC3 = LDMA_CH_REQSEL_SIGSEL_WTIMER1CC3 | LDMA_CH_REQSEL_SOURCESEL_WTIMER1,                   ///< Trigger on WTIMER1_CC3.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER1UFOF)
  ldmaPeripheralSignal_WTIMER1_UFOF = LDMA_CH_REQSEL_SIGSEL_WTIMER1UFOF | LDMA_CH_REQSEL_SOURCESEL_WTIMER1,                 ///< Trigger on WTIMER1_UFOF.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER2CC0)
  ldmaPeripheralSignal_WTIMER2_CC0 = LDMA_CH_REQSEL_SIGSEL_WTIMER2CC0 | LDMA_CH_REQSEL_SOURCESEL_WTIMER2,                   ///< Trigger on WTIMER2_CC0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER2CC1)
  ldmaPeripheralSignal_WTIMER2_CC1 = LDMA_CH_REQSEL_SIGSEL_WTIMER2CC1 | LDMA_CH_REQSEL_SOURCESEL_WTIMER2,                   ///< Trigger on WTIMER2_CC1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER2CC2)
  ldmaPeripheralSignal_WTIMER2_CC2 = LDMA_CH_REQSEL_SIGSEL_WTIMER2CC2 | LDMA_CH_REQSEL_SOURCESEL_WTIMER2,                   ///< Trigger on WTIMER2_CC2.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER2UFOF)
  ldmaPeripheralSignal_WTIMER2_UFOF = LDMA_CH_REQSEL_SIGSEL_WTIMER2UFOF | LDMA_CH_REQSEL_SOURCESEL_WTIMER2,                 ///< Trigger on WTIMER2_UFOF.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER3CC0)
  ldmaPeripheralSignal_WTIMER3_CC0 = LDMA_CH_REQSEL_SIGSEL_WTIMER3CC0 | LDMA_CH_REQSEL_SOURCESEL_WTIMER3,                   ///< Trigger on WTIMER3_CC0.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER3CC1)
  ldmaPeripheralSignal_WTIMER3_CC1 = LDMA_CH_REQSEL_SIGSEL_WTIMER3CC1 | LDMA_CH_REQSEL_SOURCESEL_WTIMER3,                   ///< Trigger on WTIMER3_CC1.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER3CC2)
  ldmaPeripheralSignal_WTIMER3_CC2 = LDMA_CH_REQSEL_SIGSEL_WTIMER3CC2 | LDMA_CH_REQSEL_SOURCESEL_WTIMER3,                   ///< Trigger on WTIMER3_CC2.
  #endif
  #if defined(LDMA_CH_REQSEL_SIGSEL_WTIMER3UFOF)
  ldmaPeripheralSignal_WTIMER3_UFOF = LDMA_CH_REQSEL_SIGSEL_WTIMER3UFOF | LDMA_CH_REQSEL_SOURCESEL_WTIMER3,                 ///< Trigger on WTIMER3_UFOF.
  #endif
} LDMA_PeripheralSignal_t;
#endif

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/

/**
 * @brief
 *   DMA descriptor.
 * @details
 *   The LDMA DMA controller supports three different DMA descriptors. Each
 *   consists of four WORDs which map directly onto HW control registers for a
 *   given DMA channel. The three descriptor types are XFER, SYNC and WRI.
 *   Refer to the reference manual for further information.
 */
typedef union {
  /**
   *  TRANSFER DMA descriptor, this is the only descriptor type which can be
   *  used to start a DMA transfer.
   */
  struct {
    uint32_t  structType : 2;   /**< Set to 0 to select XFER descriptor type.        */
    uint32_t  reserved0  : 1;   /**< Reserved.                                       */
    uint32_t  structReq  : 1;   /**< DMA transfer trigger during LINKLOAD.           */
    uint32_t  xferCnt    : 11;  /**< Transfer count minus one.                       */
    uint32_t  byteSwap   : 1;   /**< Enable byte swapping transfers.                 */
    uint32_t  blockSize  : 4;   /**< Number of unit transfers per arbitration cycle. */
    uint32_t  doneIfs    : 1;   /**< Generate interrupt when done.                   */
    uint32_t  reqMode    : 1;   /**< Block or cycle transfer selector.               */
    uint32_t  decLoopCnt : 1;   /**< Enable looped transfers.                        */
    uint32_t  ignoreSrec : 1;   /**< Ignore single requests.                         */
    uint32_t  srcInc     : 2;   /**< Source address increment unit size.             */
    uint32_t  size       : 2;   /**< DMA transfer unit size.                         */
    uint32_t  dstInc     : 2;   /**< Destination address increment unit size.        */
    uint32_t  srcAddrMode : 1;  /**< Source addressing mode.                         */
    uint32_t  dstAddrMode : 1;  /**< Destination addressing mode.                    */

    uint32_t  srcAddr;          /**< DMA source address.                             */
    uint32_t  dstAddr;          /**< DMA destination address.                        */

    uint32_t  linkMode   : 1;   /**< Select absolute or relative link address.       */
    uint32_t  link       : 1;   /**< Enable LINKLOAD when transfer is done.          */
    int32_t   linkAddr   : 30;  /**< Address of next (linked) descriptor.            */
  } xfer;

  /** SYNCHRONIZE DMA descriptor, used for intra channel transfer
   *  synchronization.
   */
  struct {
    uint32_t  structType : 2;   /**< Set to 1 to select SYNC descriptor type.        */
    uint32_t  reserved0  : 1;   /**< Reserved.                                       */
    uint32_t  structReq  : 1;   /**< DMA transfer trigger during LINKLOAD.           */
    uint32_t  xferCnt    : 11;  /**< Transfer count minus one.                       */
    uint32_t  byteSwap   : 1;   /**< Enable byte swapping transfers.                 */
    uint32_t  blockSize  : 4;   /**< Number of unit transfers per arbitration cycle. */
    uint32_t  doneIfs    : 1;   /**< Generate interrupt when done.                   */
    uint32_t  reqMode    : 1;   /**< Block or cycle transfer selector.               */
    uint32_t  decLoopCnt : 1;   /**< Enable looped transfers.                        */
    uint32_t  ignoreSrec : 1;   /**< Ignore single requests.                         */
    uint32_t  srcInc     : 2;   /**< Source address increment unit size.             */
    uint32_t  size       : 2;   /**< DMA transfer unit size.                         */
    uint32_t  dstInc     : 2;   /**< Destination address increment unit size.        */
    uint32_t  srcAddrMode : 1;  /**< Source addressing mode.                         */
    uint32_t  dstAddrMode : 1;  /**< Destination addressing mode.                    */

    uint32_t  syncSet    : 8;   /**< Set bits in LDMA_CTRL.SYNCTRIG register.        */
    uint32_t  syncClr    : 8;   /**< Clear bits in LDMA_CTRL.SYNCTRIG register.      */
    uint32_t  reserved1  : 16;  /**< Reserved.                                       */
    uint32_t  matchVal   : 8;   /**< Sync trigger match value.                       */
    uint32_t  matchEn    : 8;   /**< Sync trigger match enable.                      */
    uint32_t  reserved2  : 16;  /**< Reserved.                                       */

    uint32_t  linkMode   : 1;   /**< Select absolute or relative link address.       */
    uint32_t  link       : 1;   /**< Enable LINKLOAD when transfer is done.          */
    int32_t   linkAddr   : 30;  /**< Address of next (linked) descriptor.            */
  } sync;

  /** WRITE DMA descriptor, used for write immediate operations.                     */
  struct {
    uint32_t  structType : 2;   /**< Set to 2 to select WRITE descriptor type.       */
    uint32_t  reserved0  : 1;   /**< Reserved.                                       */
    uint32_t  structReq  : 1;   /**< DMA transfer trigger during LINKLOAD.           */
    uint32_t  xferCnt    : 11;  /**< Transfer count minus one.                       */
    uint32_t  byteSwap   : 1;   /**< Enable byte swapping transfers.                 */
    uint32_t  blockSize  : 4;   /**< Number of unit transfers per arbitration cycle. */
    uint32_t  doneIfs    : 1;   /**< Generate interrupt when done.                   */
    uint32_t  reqMode    : 1;   /**< Block or cycle transfer selector.               */
    uint32_t  decLoopCnt : 1;   /**< Enable looped transfers.                        */
    uint32_t  ignoreSrec : 1;   /**< Ignore single requests.                         */
    uint32_t  srcInc     : 2;   /**< Source address increment unit size.             */
    uint32_t  size       : 2;   /**< DMA transfer unit size.                         */
    uint32_t  dstInc     : 2;   /**< Destination address increment unit size.        */
    uint32_t  srcAddrMode : 1;  /**< Source addressing mode.                         */
    uint32_t  dstAddrMode : 1;  /**< Destination addressing mode.                    */

    uint32_t  immVal;           /**< Data to be written at dstAddr.                  */
    uint32_t  dstAddr;          /**< DMA write destination address.                  */

    uint32_t  linkMode   : 1;   /**< Select absolute or relative link address.       */
    uint32_t  link       : 1;   /**< Enable LINKLOAD when transfer is done.          */
    int32_t   linkAddr   : 30;  /**< Address of next (linked) descriptor.            */
  } wri;
} LDMA_Descriptor_t;

#if defined(_LDMA_CH_CTRL_EXTEND_MASK)
/**
 * @brief
 *   DMA extended descriptor.
 * @details
 *   The extended descriptor adds additional fields for the extended features
 *   available on the MMLDMA peripheral: destination interleaving and bufferable.
 *   The same three different DMA descriptors supported by the LDMA DMA controller
 *   are available. Each consists of seven WORDs (instead of four in non-extended
 *   descriptors) which map directly onto HW control registers for a given DMA
 *   channel. The three descriptor types are XFER, SYNC and WRI. But the
 *   extended fields are true only for XFER. The extended fields are the following:
 *
 *             +-          +- CTRL
 *             | Original  |  SRC
 *             | Structure |  DST
 *   Extended  |           +- LINK
 *   Structure |              XCTRL
 *             |              Reserved for future usage
 *             +-             ILSRC
 *
 *   Refer to the reference manual for further information.
 */
typedef struct {
  uint32_t  structType : 2;     /**< Set to 0 to select XFER descriptor type.        */
  uint32_t  extend     : 1;     /**< Extend data structure.                          */
  uint32_t  structReq  : 1;     /**< DMA transfer trigger during LINKLOAD.           */
  uint32_t  xferCnt    : 11;    /**< Transfer count minus one.                       */
  uint32_t  byteSwap   : 1;     /**< Enable byte swapping transfers.                 */
  uint32_t  blockSize  : 4;     /**< Number of unit transfers per arbitration cycle. */
  uint32_t  doneIfs    : 1;     /**< Generate interrupt when done.                   */
  uint32_t  reqMode    : 1;     /**< Block or cycle transfer selector.               */
  uint32_t  decLoopCnt : 1;     /**< Enable looped transfers.                        */
  uint32_t  ignoreSrec : 1;     /**< Ignore single requests.                         */
  uint32_t  srcInc     : 2;     /**< Source address increment unit size.             */
  uint32_t  size       : 2;     /**< DMA transfer unit size.                         */
  uint32_t  dstInc     : 2;     /**< Destination address increment unit size.        */
  uint32_t  srcAddrMode : 1;    /**< Source addressing mode.                         */
  uint32_t  dstAddrMode : 1;    /**< Destination addressing mode.                    */

  uint32_t  srcAddr;            /**< DMA source address.                             */
  uint32_t  dstAddr;            /**< DMA destination address.                        */

  uint32_t  linkMode   : 1;     /**< Select absolute or relative link address.       */
  uint32_t  link       : 1;     /**< Enable LINKLOAD when transfer is done.          */
  int32_t   linkAddr   : 30;    /**< Address of next (linked) descriptor.            */

  uint32_t  reserved1  : 4;     /**< Reserved                                        */
  uint32_t  dstIlEn    : 1;     /**< Destination interleave.                         */
  uint32_t  IlMode     : 2;     /**< Interleave mode.                                */
  uint32_t  bufferable : 1;    /**< Reserved                                        */
  uint32_t  reserved2  : 24;    /**< Allow AHB buffering.                            */

  uint32_t  reserved3;          /**< Reserved                                        */
  uint32_t  IlSrc;              /**< DMA rules table base address in memory.         */
} LDMA_DescriptorExtend_t;
#endif

/** @brief LDMA initialization configuration structure. */
typedef struct {
  uint8_t               ldmaInitCtrlNumFixed;     /**< Arbitration mode separator. */
  uint8_t               ldmaInitCtrlSyncPrsClrEn; /**< PRS Synctrig clear enable.  */
  uint8_t               ldmaInitCtrlSyncPrsSetEn; /**< PRS Synctrig set enable.    */
  uint8_t               ldmaInitIrqPriority;      /**< LDMA IRQ priority (0..7).   */
} LDMA_Init_t;

/**
 * @brief
 *   DMA transfer configuration structure.
 * @details
 *   This structure configures all aspects of a DMA transfer.
 */
typedef struct {
  uint32_t                ldmaReqSel;            /**< Selects DMA trigger source.                  */
  uint8_t                 ldmaCtrlSyncPrsClrOff; /**< PRS Synctrig clear enables to clear.         */
  uint8_t                 ldmaCtrlSyncPrsClrOn;  /**< PRS Synctrig clear enables to set.           */
  uint8_t                 ldmaCtrlSyncPrsSetOff; /**< PRS Synctrig set enables to clear.           */
  uint8_t                 ldmaCtrlSyncPrsSetOn;  /**< PRS Synctrig set enables to set.             */
  bool                    ldmaReqDis;            /**< Mask the PRS trigger input.                  */
  bool                    ldmaDbgHalt;           /**< Dis. DMA trig when CPU is halted.            */
  LDMA_CfgArbSlots_t      ldmaCfgArbSlots;       /**< Arbitration slot number.                     */
  LDMA_CfgSrcIncSign_t    ldmaCfgSrcIncSign;     /**< Source address increment sign.               */
  LDMA_CfgDstIncSign_t    ldmaCfgDstIncSign;     /**< Destination address increment sign.          */
  uint8_t                 ldmaLoopCnt;           /**< Counter for looped transfers.                */
#if defined(_LDMA_CH_CFG_SRCBUSPORT_MASK)
  LDMA_CfgStructBusPort_t ldmaCfgStructBusPort;  /**< Structure fetch operation bus port.          */
  LDMA_CfgSrcBusPort_t    ldmaCfgSrcBusPort;     /**< Source operation bus port.                   */
  LDMA_CfgDstBusPort_t    ldmaCfgDstBusPort;     /**< Destination operation bus port.              */
#endif
} LDMA_TransferCfg_t;

/*******************************************************************************
 ********************************   DEFINES   **********************************
 ******************************************************************************/

/** @brief Size in words of a non-extended DMA descriptor. */
#define LDMA_DESCRIPTOR_NON_EXTEND_SIZE_WORD  4
/** @brief Size in words of an extended DMA descriptor. */
#define LDMA_DESCRIPTOR_EXTEND_SIZE_WORD      7

/*******************************************************************************
 **************************   STRUCT INITIALIZERS   ****************************
 ******************************************************************************/

/** @brief Default DMA initialization structure. */
#define LDMA_INIT_DEFAULT                                                                    \
  {                                                                                          \
    .ldmaInitCtrlNumFixed     = _LDMA_CTRL_NUMFIXED_DEFAULT,/* Fixed priority arbitration.*/ \
    .ldmaInitCtrlSyncPrsClrEn = 0,                         /* No PRS Synctrig clear enable*/ \
    .ldmaInitCtrlSyncPrsSetEn = 0,                         /* No PRS Synctrig set enable. */ \
    .ldmaInitIrqPriority      = 3                          /* IRQ priority level 3.       */ \
  }

/**
 * @brief
 *   Generic DMA transfer configuration for memory to memory transfers.
 */
#if defined(_LDMA_CH_CFG_SRCBUSPORT_MASK)
#define LDMA_TRANSFER_CFG_MEMORY()                                \
  {                                                               \
    0, 0, 0, 0, 0,                                                \
    false, false, ldmaCfgArbSlotsAs1,                             \
    ldmaCfgSrcIncSignPos, ldmaCfgDstIncSignPos, 0,                \
    ldmaCfgStructBusPort0, ldmaCfgSrcBusPort0, ldmaCfgDstBusPort0 \
  }
#else
#define LDMA_TRANSFER_CFG_MEMORY()                \
  {                                               \
    0, 0, 0, 0, 0,                                \
    false, false, ldmaCfgArbSlotsAs1,             \
    ldmaCfgSrcIncSignPos, ldmaCfgDstIncSignPos, 0 \
  }
#endif

/**
 * @brief
 *   Generic DMA transfer configuration for looped memory to memory transfers.
 */
#if defined(_LDMA_CH_CFG_SRCBUSPORT_MASK)
#define LDMA_TRANSFER_CFG_MEMORY_LOOP(loopCnt)                    \
  {                                                               \
    0, 0, 0, 0, 0,                                                \
    false, false, ldmaCfgArbSlotsAs1,                             \
    ldmaCfgSrcIncSignPos, ldmaCfgDstIncSignPos,                   \
    loopCnt,                                                      \
    ldmaCfgStructBusPort0, ldmaCfgSrcBusPort0, ldmaCfgDstBusPort0 \
  }
#else
#define LDMA_TRANSFER_CFG_MEMORY_LOOP(loopCnt)  \
  {                                             \
    0, 0, 0, 0, 0,                              \
    false, false, ldmaCfgArbSlotsAs1,           \
    ldmaCfgSrcIncSignPos, ldmaCfgDstIncSignPos, \
    loopCnt                                     \
  }
#endif

/**
 * @brief
 *   Generic DMA transfer configuration for memory to/from peripheral transfers.
 */
#if defined(_LDMA_CH_CFG_SRCBUSPORT_MASK)
#define LDMA_TRANSFER_CFG_PERIPHERAL(signal)                      \
  {                                                               \
    signal, 0, 0, 0, 0,                                           \
    false, false, ldmaCfgArbSlotsAs1,                             \
    ldmaCfgSrcIncSignPos, ldmaCfgDstIncSignPos, 0,                \
    ldmaCfgStructBusPort0, ldmaCfgSrcBusPort0, ldmaCfgDstBusPort0 \
  }
#else
#define LDMA_TRANSFER_CFG_PERIPHERAL(signal)      \
  {                                               \
    signal, 0, 0, 0, 0,                           \
    false, false, ldmaCfgArbSlotsAs1,             \
    ldmaCfgSrcIncSignPos, ldmaCfgDstIncSignPos, 0 \
  }
#endif

/**
 * @brief
 *   Generic DMA transfer configuration for looped memory to/from peripheral transfers.
 */
#if defined(_LDMA_CH_CFG_SRCBUSPORT_MASK)
#define LDMA_TRANSFER_CFG_PERIPHERAL_LOOP(signal, loopCnt)        \
  {                                                               \
    signal, 0, 0, 0, 0,                                           \
    false, false, ldmaCfgArbSlotsAs1,                             \
    ldmaCfgSrcIncSignPos, ldmaCfgDstIncSignPos, loopCnt,          \
    ldmaCfgStructBusPort0, ldmaCfgSrcBusPort0, ldmaCfgDstBusPort0 \
  }
#else
#define LDMA_TRANSFER_CFG_PERIPHERAL_LOOP(signal, loopCnt) \
  {                                                        \
    signal, 0, 0, 0, 0,                                    \
    false, false, ldmaCfgArbSlotsAs1,                      \
    ldmaCfgSrcIncSignPos, ldmaCfgDstIncSignPos, loopCnt    \
  }
#endif

/**
 * @brief
 *   DMA descriptor initializer for single memory to memory word transfer.
 * @param[in] src       Source data address.
 * @param[in] dest      Destination data address.
 * @param[in] count     Number of words to transfer.
 */
#define LDMA_DESCRIPTOR_SINGLE_M2M_WORD(src, dest, count) \
  {                                                       \
    .xfer =                                               \
    {                                                     \
      .structType   = ldmaCtrlStructTypeXfer,             \
      .structReq    = 1,                                  \
      .xferCnt      = (count) - 1,                        \
      .byteSwap     = 0,                                  \
      .blockSize    = ldmaCtrlBlockSizeUnit1,             \
      .doneIfs      = 1,                                  \
      .reqMode      = ldmaCtrlReqModeAll,                 \
      .decLoopCnt   = 0,                                  \
      .ignoreSrec   = 0,                                  \
      .srcInc       = ldmaCtrlSrcIncOne,                  \
      .size         = ldmaCtrlSizeWord,                   \
      .dstInc       = ldmaCtrlDstIncOne,                  \
      .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,             \
      .dstAddrMode  = ldmaCtrlDstAddrModeAbs,             \
      .srcAddr      = (uint32_t)(src),                    \
      .dstAddr      = (uint32_t)(dest),                   \
      .linkMode     = 0,                                  \
      .link         = 0,                                  \
      .linkAddr     = 0                                   \
    }                                                     \
  }

#if defined(_LDMA_CH_CTRL_EXTEND_MASK)
/**
 * @brief
 *   DMA descriptor initializer for single memory to memory word transfer
 *   using the extended descriptor fields.
 * @param[in] src       Source data address.
 * @param[in] dest      Destination data address.
 * @param[in] count     Number of words to transfer.
 */
#define LDMA_DESCRIPTOR_SINGLE_M2M_WORD_EXTEND(src, dest, count)                                     \
  {                                                                                                  \
    .structType   = ldmaCtrlStructTypeXfer,                                                          \
    .extend       = 1,                                                                               \
    .structReq    = 1,                                                                               \
    .xferCnt      = (count) - 1,                                                                     \
    .byteSwap     = 0,                                                                               \
    .blockSize    = ldmaCtrlBlockSizeUnit1,                                                          \
    .doneIfs      = 1,                                                                               \
    .reqMode      = ldmaCtrlReqModeAll,                                                              \
    .decLoopCnt   = 0,                                                                               \
    .ignoreSrec   = 0,                                                                               \
    .srcInc       = ldmaCtrlSrcIncOne,                                                               \
    .size         = ldmaCtrlSizeWord,                                                                \
    .dstInc       = ldmaCtrlDstIncNone, /* Ignored since destination addressing is non-sequential */ \
    .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,                                                          \
    .dstAddrMode  = ldmaCtrlDstAddrModeAbs,                                                          \
    .srcAddr      = (uint32_t)(src),                                                                 \
    .dstAddr      = (uint32_t)(dest),                                                                \
    .linkMode     = 0,                                                                               \
    .link         = 0,                                                                               \
    .linkAddr     = 0,                                                                               \
    .dstIlEn      = 0,                                                                               \
    .IlMode       = 0,                                                                               \
    .bufferable   = 0,                                                                               \
    .IlSrc        = 0                                                                                \
  }
#endif

/**
 * @brief
 *   DMA descriptor initializer for single memory to memory half-word transfer.
 * @param[in] src       Source data address.
 * @param[in] dest      Destination data address.
 * @param[in] count     Number of half-words to transfer.
 */
#define LDMA_DESCRIPTOR_SINGLE_M2M_HALF(src, dest, count) \
  {                                                       \
    .xfer =                                               \
    {                                                     \
      .structType   = ldmaCtrlStructTypeXfer,             \
      .structReq    = 1,                                  \
      .xferCnt      = (count) - 1,                        \
      .byteSwap     = 0,                                  \
      .blockSize    = ldmaCtrlBlockSizeUnit1,             \
      .doneIfs      = 1,                                  \
      .reqMode      = ldmaCtrlReqModeAll,                 \
      .decLoopCnt   = 0,                                  \
      .ignoreSrec   = 0,                                  \
      .srcInc       = ldmaCtrlSrcIncOne,                  \
      .size         = ldmaCtrlSizeHalf,                   \
      .dstInc       = ldmaCtrlDstIncOne,                  \
      .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,             \
      .dstAddrMode  = ldmaCtrlDstAddrModeAbs,             \
      .srcAddr      = (uint32_t)(src),                    \
      .dstAddr      = (uint32_t)(dest),                   \
      .linkMode     = 0,                                  \
      .link         = 0,                                  \
      .linkAddr     = 0                                   \
    }                                                     \
  }

#if defined(_LDMA_CH_CTRL_EXTEND_MASK)
/**
 * @brief
 *   DMA descriptor initializer for single memory to memory half-word transfer
 *   using the extended descriptor fields.
 * @param[in] src       Source data address.
 * @param[in] dest      Destination data address.
 * @param[in] count     Number of half-words to transfer.
 */
#define LDMA_DESCRIPTOR_SINGLE_M2M_HALF_EXTEND(src, dest, count) \
  {                                                              \
    .structType   = ldmaCtrlStructTypeXfer,                      \
    .extend       = 1,                                           \
    .structReq    = 1,                                           \
    .xferCnt      = (count) - 1,                                 \
    .byteSwap     = 0,                                           \
    .blockSize    = ldmaCtrlBlockSizeUnit1,                      \
    .doneIfs      = 1,                                           \
    .reqMode      = ldmaCtrlReqModeAll,                          \
    .decLoopCnt   = 0,                                           \
    .ignoreSrec   = 0,                                           \
    .srcInc       = ldmaCtrlSrcIncOne,                           \
    .size         = ldmaCtrlSizeHalf,                            \
    .dstInc       = ldmaCtrlDstIncNone,                          \
    .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,                      \
    .dstAddrMode  = ldmaCtrlDstAddrModeAbs,                      \
    .srcAddr      = (uint32_t)(src),                             \
    .dstAddr      = (uint32_t)(dest),                            \
    .linkMode     = 0,                                           \
    .link         = 0,                                           \
    .linkAddr     = 0,                                           \
    .dstIlEn      = 0,                                           \
    .IlMode       = 0,                                           \
    .bufferable   = 0,                                           \
    .IlSrc        = 0                                            \
  }
#endif

/**
 * @brief
 *   DMA descriptor initializer for single memory to memory byte transfer.
 * @param[in] src       Source data address.
 * @param[in] dest      Destination data address.
 * @param[in] count     Number of bytes to transfer.
 */
#define LDMA_DESCRIPTOR_SINGLE_M2M_BYTE(src, dest, count) \
  {                                                       \
    .xfer =                                               \
    {                                                     \
      .structType   = ldmaCtrlStructTypeXfer,             \
      .structReq    = 1,                                  \
      .xferCnt      = (count) - 1,                        \
      .byteSwap     = 0,                                  \
      .blockSize    = ldmaCtrlBlockSizeUnit1,             \
      .doneIfs      = 1,                                  \
      .reqMode      = ldmaCtrlReqModeAll,                 \
      .decLoopCnt   = 0,                                  \
      .ignoreSrec   = 0,                                  \
      .srcInc       = ldmaCtrlSrcIncOne,                  \
      .size         = ldmaCtrlSizeByte,                   \
      .dstInc       = ldmaCtrlDstIncOne,                  \
      .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,             \
      .dstAddrMode  = ldmaCtrlDstAddrModeAbs,             \
      .srcAddr      = (uint32_t)(src),                    \
      .dstAddr      = (uint32_t)(dest),                   \
      .linkMode     = 0,                                  \
      .link         = 0,                                  \
      .linkAddr     = 0                                   \
    }                                                     \
  }

#if defined(_LDMA_CH_CTRL_EXTEND_MASK)
/**
 * @brief
 *   DMA descriptor initializer for single memory to memory byte transfer
 *   using the extended descriptor fields.
 * @param[in] src       Source data address.
 * @param[in] dest      Destination data address.
 * @param[in] count     Number of bytes to transfer.
 */
#define LDMA_DESCRIPTOR_SINGLE_M2M_BYTE_EXTEND(src, dest, count) \
  {                                                              \
    .structType   = ldmaCtrlStructTypeXfer,                      \
    .extend       = 1,                                           \
    .structReq    = 1,                                           \
    .xferCnt      = (count) - 1,                                 \
    .byteSwap     = 0,                                           \
    .blockSize    = ldmaCtrlBlockSizeUnit1,                      \
    .doneIfs      = 1,                                           \
    .reqMode      = ldmaCtrlReqModeAll,                          \
    .decLoopCnt   = 0,                                           \
    .ignoreSrec   = 0,                                           \
    .srcInc       = ldmaCtrlSrcIncOne,                           \
    .size         = ldmaCtrlSizeByte,                            \
    .dstInc       = ldmaCtrlDstIncNone,                          \
    .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,                      \
    .dstAddrMode  = ldmaCtrlDstAddrModeAbs,                      \
    .srcAddr      = (uint32_t)(src),                             \
    .dstAddr      = (uint32_t)(dest),                            \
    .linkMode     = 0,                                           \
    .link         = 0,                                           \
    .linkAddr     = 0,                                           \
    .dstIlEn      = 0,                                           \
    .IlMode       = 0,                                           \
    .bufferable   = 0,                                           \
    .IlSrc        = 0                                            \
  }
#endif

/**
 * @brief
 *   DMA descriptor initializer for linked memory to memory word transfer.
 *
 *   Link address must be an absolute address.
 * @note
 *   The linkAddr member of the transfer descriptor is not initialized.
 *   linkAddr must be initialized by using the proper bits right-shift
 *   to get the correct bits from the absolute address.
 *   _LDMA_CH_LINK_LINKADDR_SHIFT should be used for that operation:
 * @code
     desc.linkAddr = ((int32_t)&next_desc) >> _LDMA_CH_LINK_LINKADDR_SHIFT;@endcode
 *   The opposite bit shift (left) must be done if linkAddr is read.
 * @param[in] src       Source data address.
 * @param[in] dest      Destination data address.
 * @param[in] count     Number of words to transfer.
 */
#define LDMA_DESCRIPTOR_LINKABS_M2M_WORD(src, dest, count) \
  {                                                        \
    .xfer =                                                \
    {                                                      \
      .structType   = ldmaCtrlStructTypeXfer,              \
      .structReq    = 1,                                   \
      .xferCnt      = (count) - 1,                         \
      .byteSwap     = 0,                                   \
      .blockSize    = ldmaCtrlBlockSizeUnit1,              \
      .doneIfs      = 0,                                   \
      .reqMode      = ldmaCtrlReqModeAll,                  \
      .decLoopCnt   = 0,                                   \
      .ignoreSrec   = 0,                                   \
      .srcInc       = ldmaCtrlSrcIncOne,                   \
      .size         = ldmaCtrlSizeWord,                    \
      .dstInc       = ldmaCtrlDstIncOne,                   \
      .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,              \
      .dstAddrMode  = ldmaCtrlDstAddrModeAbs,              \
      .srcAddr      = (uint32_t)(src),                     \
      .dstAddr      = (uint32_t)(dest),                    \
      .linkMode     = ldmaLinkModeAbs,                     \
      .link         = 1,                                   \
      .linkAddr     = 0 /* Must be set runtime ! */        \
    }                                                      \
  }

#if defined(_LDMA_CH_CTRL_EXTEND_MASK)
/**
 * @brief
 *   DMA descriptor initializer for linked memory to memory word transfer
 *   using the extended descriptor fields.
 *
 *   Link address must be an absolute address.
 * @note
 *   The linkAddr member of the transfer descriptor is not initialized.
 *   linkAddr must be initialized by using the proper bits right-shift
 *   to get the correct bits from the absolute address.
 *   _LDMA_CH_LINK_LINKADDR_SHIFT should be used for that operation:
 * @code
     desc.linkAddr = ((int32_t)&next_desc) >> _LDMA_CH_LINK_LINKADDR_SHIFT;@endcode
 *   The opposite bit shift (left) must be done if linkAddr is read.
 * @param[in] src       Source data address.
 * @param[in] dest      Destination data address.
 * @param[in] count     Number of words to transfer.
 */
#define LDMA_DESCRIPTOR_LINKABS_M2M_WORD_EXTEND(src, dest, count) \
  {                                                               \
    .structType   = ldmaCtrlStructTypeXfer,                       \
    .extend       = 1,                                            \
    .structReq    = 1,                                            \
    .xferCnt      = (count) - 1,                                  \
    .byteSwap     = 0,                                            \
    .blockSize    = ldmaCtrlBlockSizeUnit1,                       \
    .doneIfs      = 0,                                            \
    .reqMode      = ldmaCtrlReqModeAll,                           \
    .decLoopCnt   = 0,                                            \
    .ignoreSrec   = 0,                                            \
    .srcInc       = ldmaCtrlSrcIncOne,                            \
    .size         = ldmaCtrlSizeWord,                             \
    .dstInc       = ldmaCtrlDstIncNone,                           \
    .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,                       \
    .dstAddrMode  = ldmaCtrlDstAddrModeAbs,                       \
    .srcAddr      = (uint32_t)(src),                              \
    .dstAddr      = (uint32_t)(dest),                             \
    .linkMode     = ldmaLinkModeAbs,                              \
    .link         = 1,                                            \
    .linkAddr     = 0, /* Must be set runtime ! */                \
    .dstIlEn      = 0,                                            \
    .IlMode       = 0,                                            \
    .bufferable   = 0,                                            \
    .IlSrc        = 0                                             \
  }
#endif

/**
 * @brief
 *   DMA descriptor initializer for linked memory to memory half-word transfer.
 *
 *   Link address must be an absolute address.
 * @note
 *   The linkAddr member of the transfer descriptor is not initialized.
 *   linkAddr must be initialized by using the proper bits right-shift
 *   to get the correct bits from the absolute address.
 *   _LDMA_CH_LINK_LINKADDR_SHIFT should be used for that operation:
 * @code
     desc.linkAddr = ((int32_t)&next_desc) >> _LDMA_CH_LINK_LINKADDR_SHIFT;@endcode
 *   The opposite bit shift (left) must be done if linkAddr is read.
 * @param[in] src       Source data address.
 * @param[in] dest      Destination data address.
 * @param[in] count     Number of half-words to transfer.
 */
#define LDMA_DESCRIPTOR_LINKABS_M2M_HALF(src, dest, count) \
  {                                                        \
    .xfer =                                                \
    {                                                      \
      .structType   = ldmaCtrlStructTypeXfer,              \
      .structReq    = 1,                                   \
      .xferCnt      = (count) - 1,                         \
      .byteSwap     = 0,                                   \
      .blockSize    = ldmaCtrlBlockSizeUnit1,              \
      .doneIfs      = 0,                                   \
      .reqMode      = ldmaCtrlReqModeAll,                  \
      .decLoopCnt   = 0,                                   \
      .ignoreSrec   = 0,                                   \
      .srcInc       = ldmaCtrlSrcIncOne,                   \
      .size         = ldmaCtrlSizeHalf,                    \
      .dstInc       = ldmaCtrlDstIncOne,                   \
      .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,              \
      .dstAddrMode  = ldmaCtrlDstAddrModeAbs,              \
      .srcAddr      = (uint32_t)(src),                     \
      .dstAddr      = (uint32_t)(dest),                    \
      .linkMode     = ldmaLinkModeAbs,                     \
      .link         = 1,                                   \
      .linkAddr     = 0 /* Must be set runtime ! */        \
    }                                                      \
  }

#if defined(_LDMA_CH_CTRL_EXTEND_MASK)
/**
 * @brief
 *   DMA descriptor initializer for linked memory to memory half-word transfer
 *   using the extended descriptor fields.
 *
 *   Link address must be an absolute address.
 * @note
 *   The linkAddr member of the transfer descriptor is not initialized.
 *   linkAddr must be initialized by using the proper bits right-shift
 *   to get the correct bits from the absolute address.
 *   _LDMA_CH_LINK_LINKADDR_SHIFT should be used for that operation:
 * @code
     desc.linkAddr = ((int32_t)&next_desc) >> _LDMA_CH_LINK_LINKADDR_SHIFT;@endcode
 *   The opposite bit shift (left) must be done if linkAddr is read.
 * @param[in] src       Source data address.
 * @param[in] dest      Destination data address.
 * @param[in] count     Number of half-words to transfer.
 */
#define LDMA_DESCRIPTOR_LINKABS_M2M_HALF_EXTEND(src, dest, count) \
  {                                                               \
    .structType   = ldmaCtrlStructTypeXfer,                       \
    .extend       = 1,                                            \
    .structReq    = 1,                                            \
    .xferCnt      = (count) - 1,                                  \
    .byteSwap     = 0,                                            \
    .blockSize    = ldmaCtrlBlockSizeUnit1,                       \
    .doneIfs      = 0,                                            \
    .reqMode      = ldmaCtrlReqModeAll,                           \
    .decLoopCnt   = 0,                                            \
    .ignoreSrec   = 0,                                            \
    .srcInc       = ldmaCtrlSrcIncOne,                            \
    .size         = ldmaCtrlSizeHalf,                             \
    .dstInc       = ldmaCtrlDstIncNone,                           \
    .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,                       \
    .dstAddrMode  = ldmaCtrlDstAddrModeAbs,                       \
    .srcAddr      = (uint32_t)(src),                              \
    .dstAddr      = (uint32_t)(dest),                             \
    .linkMode     = ldmaLinkModeAbs,                              \
    .link         = 1,                                            \
    .linkAddr     = 0, /* Must be set runtime ! */                \
    .dstIlEn      = 0,                                            \
    .IlMode       = 0,                                            \
    .bufferable   = 0,                                            \
    .IlSrc        = 0                                             \
  }
#endif

/**
 * @brief
 *   DMA descriptor initializer for linked memory to memory byte transfer.
 *
 *   Link address must be an absolute address.
 * @note
 *   The linkAddr member of the transfer descriptor is not initialized.
 *   linkAddr must be initialized by using the proper bits right-shift
 *   to get the correct bits from the absolute address.
 *   _LDMA_CH_LINK_LINKADDR_SHIFT should be used for that operation:
 * @code
     desc.linkAddr = ((int32_t)&next_desc) >> _LDMA_CH_LINK_LINKADDR_SHIFT;@endcode
 *   The opposite bit shift (left) must be done if linkAddr is read.
 * @param[in] src       Source data address.
 * @param[in] dest      Destination data address.
 * @param[in] count     Number of bytes to transfer.
 */
#define LDMA_DESCRIPTOR_LINKABS_M2M_BYTE(src, dest, count) \
  {                                                        \
    .xfer =                                                \
    {                                                      \
      .structType   = ldmaCtrlStructTypeXfer,              \
      .structReq    = 1,                                   \
      .xferCnt      = (count) - 1,                         \
      .byteSwap     = 0,                                   \
      .blockSize    = ldmaCtrlBlockSizeUnit1,              \
      .doneIfs      = 0,                                   \
      .reqMode      = ldmaCtrlReqModeAll,                  \
      .decLoopCnt   = 0,                                   \
      .ignoreSrec   = 0,                                   \
      .srcInc       = ldmaCtrlSrcIncOne,                   \
      .size         = ldmaCtrlSizeByte,                    \
      .dstInc       = ldmaCtrlDstIncOne,                   \
      .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,              \
      .dstAddrMode  = ldmaCtrlDstAddrModeAbs,              \
      .srcAddr      = (uint32_t)(src),                     \
      .dstAddr      = (uint32_t)(dest),                    \
      .linkMode     = ldmaLinkModeAbs,                     \
      .link         = 1,                                   \
      .linkAddr     = 0 /* Must be set runtime ! */        \
    }                                                      \
  }

#if defined(_LDMA_CH_CTRL_EXTEND_MASK)
/**
 * @brief
 *   DMA descriptor initializer for linked memory to memory byte transfer
 *   using the extended descriptor fields.
 *
 *   Link address must be an absolute address.
 * @note
 *   The linkAddr member of the transfer descriptor is not initialized.
 *   linkAddr must be initialized by using the proper bits right-shift
 *   to get the correct bits from the absolute address.
 *   _LDMA_CH_LINK_LINKADDR_SHIFT should be used for that operation:
 * @code
     desc.linkAddr = ((int32_t)&next_desc) >> _LDMA_CH_LINK_LINKADDR_SHIFT;@endcode
 *   The opposite bit shift (left) must be done if linkAddr is read.
 * @param[in] src       Source data address.
 * @param[in] dest      Destination data address.
 * @param[in] count     Number of bytes to transfer.
 */
#define LDMA_DESCRIPTOR_LINKABS_M2M_BYTE_EXTEND(src, dest, count) \
  {                                                               \
    .structType   = ldmaCtrlStructTypeXfer,                       \
    .extend       = 1,                                            \
    .structReq    = 1,                                            \
    .xferCnt      = (count) - 1,                                  \
    .byteSwap     = 0,                                            \
    .blockSize    = ldmaCtrlBlockSizeUnit1,                       \
    .doneIfs      = 0,                                            \
    .reqMode      = ldmaCtrlReqModeAll,                           \
    .decLoopCnt   = 0,                                            \
    .ignoreSrec   = 0,                                            \
    .srcInc       = ldmaCtrlSrcIncOne,                            \
    .size         = ldmaCtrlSizeByte,                             \
    .dstInc       = ldmaCtrlDstIncNone,                           \
    .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,                       \
    .dstAddrMode  = ldmaCtrlDstAddrModeAbs,                       \
    .srcAddr      = (uint32_t)(src),                              \
    .dstAddr      = (uint32_t)(dest),                             \
    .linkMode     = ldmaLinkModeAbs,                              \
    .link         = 1,                                            \
    .linkAddr     = 0, /* Must be set runtime ! */                \
    .dstIlEn      = 0,                                            \
    .IlMode       = 0,                                            \
    .bufferable   = 0,                                            \
    .IlSrc        = 0                                             \
  }
#endif

/**
 * @brief
 *   DMA descriptor initializer for linked memory to memory word transfer.
 *
 *   Link address is a relative address.
 * @note
 *   The linkAddr member of the transfer descriptor is initialized to 4
 *   (regular descriptor) or 7 (extended descriptor),  assuming that
 *   the next descriptor immediately follows this descriptor (in memory).
 * @param[in] src       Source data address.
 * @param[in] dest      Destination data address.
 * @param[in] count     Number of words to transfer.
 * @param[in] linkjmp   Address of descriptor to link to, expressed as a
 *                      signed number of descriptors from "here".
 *                      1=one descriptor forward in memory,
 *                      0=this descriptor,
 *                      -1=one descriptor back in memory.
 */
#define LDMA_DESCRIPTOR_LINKREL_M2M_WORD(src, dest, count, linkjmp)    \
  {                                                                    \
    .xfer =                                                            \
    {                                                                  \
      .structType   = ldmaCtrlStructTypeXfer,                          \
      .structReq    = 1,                                               \
      .xferCnt      = (count) - 1,                                     \
      .byteSwap     = 0,                                               \
      .blockSize    = ldmaCtrlBlockSizeUnit1,                          \
      .doneIfs      = 0,                                               \
      .reqMode      = ldmaCtrlReqModeAll,                              \
      .decLoopCnt   = 0,                                               \
      .ignoreSrec   = 0,                                               \
      .srcInc       = ldmaCtrlSrcIncOne,                               \
      .size         = ldmaCtrlSizeWord,                                \
      .dstInc       = ldmaCtrlDstIncOne,                               \
      .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,                          \
      .dstAddrMode  = ldmaCtrlDstAddrModeAbs,                          \
      .srcAddr      = (uint32_t)(src),                                 \
      .dstAddr      = (uint32_t)(dest),                                \
      .linkMode     = ldmaLinkModeRel,                                 \
      .link         = 1,                                               \
      .linkAddr     = (linkjmp) * LDMA_DESCRIPTOR_NON_EXTEND_SIZE_WORD \
    }                                                                  \
  }

#if defined(_LDMA_CH_CTRL_EXTEND_MASK)
/**
 * @brief
 *   DMA descriptor initializer for linked memory to memory word transfer
 *   using the extended descriptor fields.
 *
 *   Link address is a relative address.
 * @note
 *   The linkAddr member of the transfer descriptor is initialized to 4
 *   (regular descriptor) or 7 (extended descriptor),  assuming that
 *   the next descriptor immediately follows this descriptor (in memory).
 * @param[in] src       Source data address.
 * @param[in] dest      Destination data address.
 * @param[in] count     Number of words to transfer.
 * @param[in] linkjmp   Address of descriptor to link to, expressed as a
 *                      signed number of descriptors from "here".
 *                      1=one descriptor forward in memory,
 *                      0=this descriptor,
 *                      -1=one descriptor back in memory.
 */
#define LDMA_DESCRIPTOR_LINKREL_M2M_WORD_EXTEND(src, dest, count, linkjmp) \
  {                                                                        \
    .structType   = ldmaCtrlStructTypeXfer,                                \
    .extend       = 1,                                                     \
    .structReq    = 1,                                                     \
    .xferCnt      = (count) - 1,                                           \
    .byteSwap     = 0,                                                     \
    .blockSize    = ldmaCtrlBlockSizeUnit1,                                \
    .doneIfs      = 0,                                                     \
    .reqMode      = ldmaCtrlReqModeAll,                                    \
    .decLoopCnt   = 0,                                                     \
    .ignoreSrec   = 0,                                                     \
    .srcInc       = ldmaCtrlSrcIncOne,                                     \
    .size         = ldmaCtrlSizeWord,                                      \
    .dstInc       = ldmaCtrlDstIncNone,                                    \
    .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,                                \
    .dstAddrMode  = ldmaCtrlDstAddrModeAbs,                                \
    .srcAddr      = (uint32_t)(src),                                       \
    .dstAddr      = (uint32_t)(dest),                                      \
    .linkMode     = ldmaLinkModeRel,                                       \
    .link         = 1,                                                     \
    .linkAddr     = (linkjmp) * LDMA_DESCRIPTOR_EXTEND_SIZE_WORD,          \
    .dstIlEn      = 0,                                                     \
    .IlMode       = 0,                                                     \
    .bufferable   = 0,                                                     \
    .IlSrc        = 0                                                      \
  }
#endif

/**
 * @brief
 *   DMA descriptor initializer for linked memory to memory half-word transfer.
 *
 *   Link address is a relative address.
 * @note
 *   The linkAddr member of the transfer descriptor is initialized to 4
 *   (regular descriptor) or 7 (extended descriptor),  assuming that
 *   the next descriptor immediately follows this descriptor (in memory).
 * @param[in] src       Source data address.
 * @param[in] dest      Destination data address.
 * @param[in] count     Number of half-words to transfer.
 * @param[in] linkjmp   Address of descriptor to link to, expressed as a
 *                      signed number of descriptors from "here".
 *                      1=one descriptor forward in memory,
 *                      0=this descriptor,
 *                      -1=one descriptor back in memory.
 */
#define LDMA_DESCRIPTOR_LINKREL_M2M_HALF(src, dest, count, linkjmp)    \
  {                                                                    \
    .xfer =                                                            \
    {                                                                  \
      .structType   = ldmaCtrlStructTypeXfer,                          \
      .structReq    = 1,                                               \
      .xferCnt      = (count) - 1,                                     \
      .byteSwap     = 0,                                               \
      .blockSize    = ldmaCtrlBlockSizeUnit1,                          \
      .doneIfs      = 0,                                               \
      .reqMode      = ldmaCtrlReqModeAll,                              \
      .decLoopCnt   = 0,                                               \
      .ignoreSrec   = 0,                                               \
      .srcInc       = ldmaCtrlSrcIncOne,                               \
      .size         = ldmaCtrlSizeHalf,                                \
      .dstInc       = ldmaCtrlDstIncOne,                               \
      .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,                          \
      .dstAddrMode  = ldmaCtrlDstAddrModeAbs,                          \
      .srcAddr      = (uint32_t)(src),                                 \
      .dstAddr      = (uint32_t)(dest),                                \
      .linkMode     = ldmaLinkModeRel,                                 \
      .link         = 1,                                               \
      .linkAddr     = (linkjmp) * LDMA_DESCRIPTOR_NON_EXTEND_SIZE_WORD \
    }                                                                  \
  }

#if defined(_LDMA_CH_CTRL_EXTEND_MASK)
/**
 * @brief
 *   DMA descriptor initializer for linked memory to memory half-word transfer
 *   using the extended descriptor fields.
 *
 *   Link address is a relative address.
 * @note
 *   The linkAddr member of the transfer descriptor is initialized to 4
 *   (regular descriptor) or 7 (extended descriptor),  assuming that
 *   the next descriptor immediately follows this descriptor (in memory).
 * @param[in] src       Source data address.
 * @param[in] dest      Destination data address.
 * @param[in] count     Number of half-words to transfer.
 * @param[in] linkjmp   Address of descriptor to link to, expressed as a
 *                      signed number of descriptors from "here".
 *                      1=one descriptor forward in memory,
 *                      0=this descriptor,
 *                      -1=one descriptor back in memory.
 */
#define LDMA_DESCRIPTOR_LINKREL_M2M_HALF_EXTEND(src, dest, count, linkjmp) \
  {                                                                        \
    .structType   = ldmaCtrlStructTypeXfer,                                \
    .extend       = 1,                                                     \
    .structReq    = 1,                                                     \
    .xferCnt      = (count) - 1,                                           \
    .byteSwap     = 0,                                                     \
    .blockSize    = ldmaCtrlBlockSizeUnit1,                                \
    .doneIfs      = 0,                                                     \
    .reqMode      = ldmaCtrlReqModeAll,                                    \
    .decLoopCnt   = 0,                                                     \
    .ignoreSrec   = 0,                                                     \
    .srcInc       = ldmaCtrlSrcIncOne,                                     \
    .size         = ldmaCtrlSizeHalf,                                      \
    .dstInc       = ldmaCtrlDstIncNone,                                    \
    .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,                                \
    .dstAddrMode  = ldmaCtrlDstAddrModeAbs,                                \
    .srcAddr      = (uint32_t)(src),                                       \
    .dstAddr      = (uint32_t)(dest),                                      \
    .linkMode     = ldmaLinkModeRel,                                       \
    .link         = 1,                                                     \
    .linkAddr     = (linkjmp) * LDMA_DESCRIPTOR_EXTEND_SIZE_WORD,          \
    .dstIlEn      = 0,                                                     \
    .IlMode       = 0,                                                     \
    .bufferable   = 0,                                                     \
    .IlSrc        = 0                                                      \
  }
#endif

/**
 * @brief
 *   DMA descriptor initializer for linked memory to memory byte transfer.
 *
 *   Link address is a relative address.
 * @note
 *   The linkAddr member of the transfer descriptor is initialized to 4
 *   (regular descriptor) or 7 (extended descriptor),  assuming that
 *   the next descriptor immediately follows this descriptor (in memory).
 * @param[in] src       Source data address.
 * @param[in] dest      Destination data address.
 * @param[in] count     Number of bytes to transfer.
 * @param[in] linkjmp   Address of descriptor to link to, expressed as a
 *                      signed number of descriptors from "here".
 *                      1=one descriptor forward in memory,
 *                      0=this descriptor,
 *                      -1=one descriptor back in memory.
 */
#define LDMA_DESCRIPTOR_LINKREL_M2M_BYTE(src, dest, count, linkjmp)    \
  {                                                                    \
    .xfer =                                                            \
    {                                                                  \
      .structType   = ldmaCtrlStructTypeXfer,                          \
      .structReq    = 1,                                               \
      .xferCnt      = (count) - 1,                                     \
      .byteSwap     = 0,                                               \
      .blockSize    = ldmaCtrlBlockSizeUnit1,                          \
      .doneIfs      = 0,                                               \
      .reqMode      = ldmaCtrlReqModeAll,                              \
      .decLoopCnt   = 0,                                               \
      .ignoreSrec   = 0,                                               \
      .srcInc       = ldmaCtrlSrcIncOne,                               \
      .size         = ldmaCtrlSizeByte,                                \
      .dstInc       = ldmaCtrlDstIncOne,                               \
      .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,                          \
      .dstAddrMode  = ldmaCtrlDstAddrModeAbs,                          \
      .srcAddr      = (uint32_t)(src),                                 \
      .dstAddr      = (uint32_t)(dest),                                \
      .linkMode     = ldmaLinkModeRel,                                 \
      .link         = 1,                                               \
      .linkAddr     = (linkjmp) * LDMA_DESCRIPTOR_NON_EXTEND_SIZE_WORD \
    }                                                                  \
  }

#if defined(_LDMA_CH_CTRL_EXTEND_MASK)
/**
 * @brief
 *   DMA descriptor initializer for linked memory to memory byte transfer
 *   using the extended descriptor fields.
 *
 *   Link address is a relative address.
 * @note
 *   The linkAddr member of the transfer descriptor is initialized to 4
 *   (regular descriptor) or 7 (extended descriptor),  assuming that
 *   the next descriptor immediately follows this descriptor (in memory).
 * @param[in] src       Source data address.
 * @param[in] dest      Destination data address.
 * @param[in] count     Number of bytes to transfer.
 * @param[in] linkjmp   Address of descriptor to link to, expressed as a
 *                      signed number of descriptors from "here".
 *                      1=one descriptor forward in memory,
 *                      0=this descriptor,
 *                      -1=one descriptor back in memory.
 */
#define LDMA_DESCRIPTOR_LINKREL_M2M_BYTE_EXTEND(src, dest, count, linkjmp) \
  {                                                                        \
    .structType   = ldmaCtrlStructTypeXfer,                                \
    .extend       = 1,                                                     \
    .structReq    = 1,                                                     \
    .xferCnt      = (count) - 1,                                           \
    .byteSwap     = 0,                                                     \
    .blockSize    = ldmaCtrlBlockSizeUnit1,                                \
    .doneIfs      = 0,                                                     \
    .reqMode      = ldmaCtrlReqModeAll,                                    \
    .decLoopCnt   = 0,                                                     \
    .ignoreSrec   = 0,                                                     \
    .srcInc       = ldmaCtrlSrcIncOne,                                     \
    .size         = ldmaCtrlSizeByte,                                      \
    .dstInc       = ldmaCtrlDstIncNone,                                    \
    .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,                                \
    .dstAddrMode  = ldmaCtrlDstAddrModeAbs,                                \
    .srcAddr      = (uint32_t)(src),                                       \
    .dstAddr      = (uint32_t)(dest),                                      \
    .linkMode     = ldmaLinkModeRel,                                       \
    .link         = 1,                                                     \
    .linkAddr     = (linkjmp) * LDMA_DESCRIPTOR_EXTEND_SIZE_WORD,          \
    .dstIlEn      = 0,                                                     \
    .IlMode       = 0,                                                     \
    .bufferable   = 0,                                                     \
    .IlSrc        = 0                                                      \
  }
#endif

/**
 * @brief
 *   DMA descriptor initializer for byte transfers from a peripheral to memory.
 * @param[in] src       Peripheral data source register address.
 * @param[in] dest      Destination data address.
 * @param[in] count     Number of bytes to transfer.
 */
#define LDMA_DESCRIPTOR_SINGLE_P2M_BYTE(src, dest, count) \
  {                                                       \
    .xfer =                                               \
    {                                                     \
      .structType   = ldmaCtrlStructTypeXfer,             \
      .structReq    = 0,                                  \
      .xferCnt      = (count) - 1,                        \
      .byteSwap     = 0,                                  \
      .blockSize    = ldmaCtrlBlockSizeUnit1,             \
      .doneIfs      = 1,                                  \
      .reqMode      = ldmaCtrlReqModeBlock,               \
      .decLoopCnt   = 0,                                  \
      .ignoreSrec   = 0,                                  \
      .srcInc       = ldmaCtrlSrcIncNone,                 \
      .size         = ldmaCtrlSizeByte,                   \
      .dstInc       = ldmaCtrlDstIncOne,                  \
      .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,             \
      .dstAddrMode  = ldmaCtrlDstAddrModeAbs,             \
      .srcAddr      = (uint32_t)(src),                    \
      .dstAddr      = (uint32_t)(dest),                   \
      .linkMode     = 0,                                  \
      .link         = 0,                                  \
      .linkAddr     = 0                                   \
    }                                                     \
  }

#if defined(_LDMA_CH_CTRL_EXTEND_MASK)
/**
 * @brief
 *   DMA descriptor initializer for byte transfers from a peripheral to memory
 *   using the extended descriptor fields.
 * @param[in] src       Peripheral data source register address.
 * @param[in] dest      Destination data address.
 * @param[in] count     Number of bytes to transfer.
 */
#define LDMA_DESCRIPTOR_SINGLE_P2M_BYTE_EXTEND(src, dest, count) \
  {                                                              \
    .structType   = ldmaCtrlStructTypeXfer,                      \
    .extend       = 1,                                           \
    .structReq    = 0,                                           \
    .xferCnt      = (count) - 1,                                 \
    .byteSwap     = 0,                                           \
    .blockSize    = ldmaCtrlBlockSizeUnit1,                      \
    .doneIfs      = 1,                                           \
    .reqMode      = ldmaCtrlReqModeBlock,                        \
    .decLoopCnt   = 0,                                           \
    .ignoreSrec   = 0,                                           \
    .srcInc       = ldmaCtrlSrcIncNone,                          \
    .size         = ldmaCtrlSizeByte,                            \
    .dstInc       = ldmaCtrlDstIncNone,                          \
    .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,                      \
    .dstAddrMode  = ldmaCtrlDstAddrModeAbs,                      \
    .srcAddr      = (uint32_t)(src),                             \
    .dstAddr      = (uint32_t)(dest),                            \
    .linkMode     = 0,                                           \
    .link         = 0,                                           \
    .linkAddr     = 0,                                           \
    .dstIlEn      = 0,                                           \
    .IlMode       = 0,                                           \
    .bufferable   = 0,                                           \
    .IlSrc        = 0                                            \
  }
#endif

/**
 * @brief
 *   DMA descriptor initializer for byte transfers from a peripheral to a peripheral.
 * @param[in] src       Peripheral data source register address.
 * @param[in] dest      Peripheral data destination register address.
 * @param[in] count     Number of bytes to transfer.
 */
#define LDMA_DESCRIPTOR_SINGLE_P2P_BYTE(src, dest, count) \
  {                                                       \
    .xfer =                                               \
    {                                                     \
      .structType   = ldmaCtrlStructTypeXfer,             \
      .structReq    = 0,                                  \
      .xferCnt      = (count) - 1,                        \
      .byteSwap     = 0,                                  \
      .blockSize    = ldmaCtrlBlockSizeUnit1,             \
      .doneIfs      = 1,                                  \
      .reqMode      = ldmaCtrlReqModeBlock,               \
      .decLoopCnt   = 0,                                  \
      .ignoreSrec   = 0,                                  \
      .srcInc       = ldmaCtrlSrcIncNone,                 \
      .size         = ldmaCtrlSizeByte,                   \
      .dstInc       = ldmaCtrlDstIncNone,                 \
      .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,             \
      .dstAddrMode  = ldmaCtrlDstAddrModeAbs,             \
      .srcAddr      = (uint32_t)(src),                    \
      .dstAddr      = (uint32_t)(dest),                   \
      .linkMode     = 0,                                  \
      .link         = 0,                                  \
      .linkAddr     = 0                                   \
    }                                                     \
  }

/**
 * @brief
 *   DMA descriptor initializer for byte transfers from memory to a peripheral.
 * @param[in] src       Source data address.
 * @param[in] dest      Peripheral data register destination address.
 * @param[in] count     Number of bytes to transfer.
 */
#define LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(src, dest, count) \
  {                                                       \
    .xfer =                                               \
    {                                                     \
      .structType   = ldmaCtrlStructTypeXfer,             \
      .structReq    = 0,                                  \
      .xferCnt      = (count) - 1,                        \
      .byteSwap     = 0,                                  \
      .blockSize    = ldmaCtrlBlockSizeUnit1,             \
      .doneIfs      = 1,                                  \
      .reqMode      = ldmaCtrlReqModeBlock,               \
      .decLoopCnt   = 0,                                  \
      .ignoreSrec   = 0,                                  \
      .srcInc       = ldmaCtrlSrcIncOne,                  \
      .size         = ldmaCtrlSizeByte,                   \
      .dstInc       = ldmaCtrlDstIncNone,                 \
      .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,             \
      .dstAddrMode  = ldmaCtrlDstAddrModeAbs,             \
      .srcAddr      = (uint32_t)(src),                    \
      .dstAddr      = (uint32_t)(dest),                   \
      .linkMode     = 0,                                  \
      .link         = 0,                                  \
      .linkAddr     = 0                                   \
    }                                                     \
  }

#if defined(_LDMA_CH_CTRL_EXTEND_MASK)
/**
 * @brief
 *   DMA descriptor initializer for byte transfers from memory to a peripheral
 *   using the extended descriptor fields.
 * @note
 *   For the extended descriptor, if IlMode uses the absolute addressing, the
 *   rules are used directly as the destination addresses of the corresponding
 *   data. Thus the argument 'dest' will be ignored by the DMA engine.
 * @param[in] src       Source data address.
 * @param[in] dest      Peripheral data register destination address.
 * @param[in] count     Number of bytes to transfer.
 */
#define LDMA_DESCRIPTOR_SINGLE_M2P_BYTE_EXTEND(src, dest, count) \
  {                                                              \
    .structType   = ldmaCtrlStructTypeXfer,                      \
    .extend       = 1,                                           \
    .structReq    = 0,                                           \
    .xferCnt      = (count) - 1,                                 \
    .byteSwap     = 0,                                           \
    .blockSize    = ldmaCtrlBlockSizeUnit1,                      \
    .doneIfs      = 1,                                           \
    .reqMode      = ldmaCtrlReqModeBlock,                        \
    .decLoopCnt   = 0,                                           \
    .ignoreSrec   = 0,                                           \
    .srcInc       = ldmaCtrlSrcIncOne,                           \
    .size         = ldmaCtrlSizeByte,                            \
    .dstInc       = ldmaCtrlDstIncNone,                          \
    .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,                      \
    .dstAddrMode  = ldmaCtrlDstAddrModeAbs,                      \
    .srcAddr      = (uint32_t)(src),                             \
    .dstAddr      = (uint32_t)(dest),                            \
    .linkMode     = 0,                                           \
    .link         = 0,                                           \
    .linkAddr     = 0,                                           \
    .dstIlEn      = 0,                                           \
    .IlMode       = 0,                                           \
    .bufferable   = 0,                                           \
    .IlSrc        = 0                                            \
  }
#endif

/**
 * @brief
 *   DMA descriptor initializer for byte transfers from a peripheral to memory.
 * @param[in] src       Peripheral data source register address.
 * @param[in] dest      Destination data address.
 * @param[in] count     Number of bytes to transfer.
 * @param[in] linkjmp   Address of descriptor to link to, expressed as a
 *                      signed number of descriptors from "here".
 *                      1=one descriptor forward in memory,
 *                      0=this descriptor,
 *                      -1=one descriptor back in memory.
 */
#define LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(src, dest, count, linkjmp)    \
  {                                                                    \
    .xfer =                                                            \
    {                                                                  \
      .structType   = ldmaCtrlStructTypeXfer,                          \
      .structReq    = 0,                                               \
      .xferCnt      = (count) - 1,                                     \
      .byteSwap     = 0,                                               \
      .blockSize    = ldmaCtrlBlockSizeUnit1,                          \
      .doneIfs      = 1,                                               \
      .reqMode      = ldmaCtrlReqModeBlock,                            \
      .decLoopCnt   = 0,                                               \
      .ignoreSrec   = 0,                                               \
      .srcInc       = ldmaCtrlSrcIncNone,                              \
      .size         = ldmaCtrlSizeByte,                                \
      .dstInc       = ldmaCtrlDstIncOne,                               \
      .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,                          \
      .dstAddrMode  = ldmaCtrlDstAddrModeAbs,                          \
      .srcAddr      = (uint32_t)(src),                                 \
      .dstAddr      = (uint32_t)(dest),                                \
      .linkMode     = ldmaLinkModeRel,                                 \
      .link         = 1,                                               \
      .linkAddr     = (linkjmp) * LDMA_DESCRIPTOR_NON_EXTEND_SIZE_WORD \
    }                                                                  \
  }

#if defined(_LDMA_CH_CTRL_EXTEND_MASK)
/**
 * @brief
 *   DMA descriptor initializer for byte transfers from a peripheral to memory
 *   using the extended descriptor fields.
 * @param[in] src       Peripheral data source register address.
 * @param[in] dest      Destination data address.
 * @param[in] count     Number of bytes to transfer.
 * @param[in] linkjmp   Address of descriptor to link to, expressed as a
 *                      signed number of descriptors from "here".
 *                      1=one descriptor forward in memory,
 *                      0=this descriptor,
 *                      -1=one descriptor back in memory.
 */
#define LDMA_DESCRIPTOR_LINKREL_P2M_BYTE_EXTEND(src, dest, count, linkjmp) \
  {                                                                        \
    .structType   = ldmaCtrlStructTypeXfer,                                \
    .extend       = 1,                                                     \
    .structReq    = 0,                                                     \
    .xferCnt      = (count) - 1,                                           \
    .byteSwap     = 0,                                                     \
    .blockSize    = ldmaCtrlBlockSizeUnit1,                                \
    .doneIfs      = 1,                                                     \
    .reqMode      = ldmaCtrlReqModeBlock,                                  \
    .decLoopCnt   = 0,                                                     \
    .ignoreSrec   = 0,                                                     \
    .srcInc       = ldmaCtrlSrcIncNone,                                    \
    .size         = ldmaCtrlSizeByte,                                      \
    .dstInc       = ldmaCtrlDstIncNone,                                    \
    .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,                                \
    .dstAddrMode  = ldmaCtrlDstAddrModeAbs,                                \
    .srcAddr      = (uint32_t)(src),                                       \
    .dstAddr      = (uint32_t)(dest),                                      \
    .linkMode     = ldmaLinkModeRel,                                       \
    .link         = 1,                                                     \
    .linkAddr     = (linkjmp) * LDMA_DESCRIPTOR_EXTEND_SIZE_WORD,          \
    .dstIlEn      = 0,                                                     \
    .IlMode       = 0,                                                     \
    .bufferable   = 0,                                                     \
    .IlSrc        = 0                                                      \
  }
#endif

/**
 * @brief
 *   DMA descriptor initializer for word transfers from a peripheral to memory.
 * @param[in] src       Peripheral data source register address.
 * @param[in] dest      Destination data address.
 * @param[in] count     Number of words to transfer.
 * @param[in] linkjmp   Address of descriptor to link to, expressed as a
 *                      signed number of descriptors from "here".
 *                      1=one descriptor forward in memory,
 *                      0=this descriptor,
 *                      -1=one descriptor back in memory.
 */
#define LDMA_DESCRIPTOR_LINKREL_P2M_WORD(src, dest, count, linkjmp)    \
  {                                                                    \
    .xfer =                                                            \
    {                                                                  \
      .structType   = ldmaCtrlStructTypeXfer,                          \
      .structReq    = 0,                                               \
      .xferCnt      = (count) - 1,                                     \
      .byteSwap     = 0,                                               \
      .blockSize    = ldmaCtrlBlockSizeUnit1,                          \
      .doneIfs      = 1,                                               \
      .reqMode      = ldmaCtrlReqModeBlock,                            \
      .decLoopCnt   = 0,                                               \
      .ignoreSrec   = 0,                                               \
      .srcInc       = ldmaCtrlSrcIncNone,                              \
      .size         = ldmaCtrlSizeWord,                                \
      .dstInc       = ldmaCtrlDstIncOne,                               \
      .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,                          \
      .dstAddrMode  = ldmaCtrlDstAddrModeAbs,                          \
      .srcAddr      = (uint32_t)(src),                                 \
      .dstAddr      = (uint32_t)(dest),                                \
      .linkMode     = ldmaLinkModeRel,                                 \
      .link         = 1,                                               \
      .linkAddr     = (linkjmp) * LDMA_DESCRIPTOR_NON_EXTEND_SIZE_WORD \
    }                                                                  \
  }

#if defined(_LDMA_CH_CTRL_EXTEND_MASK)
/**
 * @brief
 *   DMA descriptor initializer for word transfers from a peripheral to memory
 *   using the extended descriptor fields.
 * @param[in] src       Peripheral data source register address.
 * @param[in] dest      Destination data address.
 * @param[in] count     Number of words to transfer.
 * @param[in] linkjmp   Address of descriptor to link to, expressed as a
 *                      signed number of descriptors from "here".
 *                      1=one descriptor forward in memory,
 *                      0=this descriptor,
 *                      -1=one descriptor back in memory.
 */
#define LDMA_DESCRIPTOR_LINKREL_P2M_WORD_EXTEND(src, dest, count, linkjmp) \
  {                                                                        \
    .structType   = ldmaCtrlStructTypeXfer,                                \
    .extend       = 1,                                                     \
    .structReq    = 0,                                                     \
    .xferCnt      = (count) - 1,                                           \
    .byteSwap     = 0,                                                     \
    .blockSize    = ldmaCtrlBlockSizeUnit1,                                \
    .doneIfs      = 1,                                                     \
    .reqMode      = ldmaCtrlReqModeBlock,                                  \
    .decLoopCnt   = 0,                                                     \
    .ignoreSrec   = 0,                                                     \
    .srcInc       = ldmaCtrlSrcIncNone,                                    \
    .size         = ldmaCtrlSizeWord,                                      \
    .dstInc       = ldmaCtrlDstIncNone,                                    \
    .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,                                \
    .dstAddrMode  = ldmaCtrlDstAddrModeAbs,                                \
    .srcAddr      = (uint32_t)(src),                                       \
    .dstAddr      = (uint32_t)(dest),                                      \
    .linkMode     = ldmaLinkModeRel,                                       \
    .link         = 1,                                                     \
    .linkAddr     = (linkjmp) * LDMA_DESCRIPTOR_EXTEND_SIZE_WORD,          \
    .dstIlEn      = 0,                                                     \
    .IlMode       = 0,                                                     \
    .bufferable   = 0,                                                     \
    .IlSrc        = 0                                                      \
  }
#endif

/**
 * @brief
 *   DMA descriptor initializer for byte transfers from memory to a peripheral.
 * @param[in] src       Source data address.
 * @param[in] dest      Peripheral data register destination address.
 * @param[in] count     Number of bytes to transfer.
 * @param[in] linkjmp   Address of descriptor to link to, expressed as a
 *                      signed number of descriptors from "here".
 *                      1=one descriptor forward in memory,
 *                      0=this descriptor,
 *                      -1=one descriptor back in memory.
 */
#define LDMA_DESCRIPTOR_LINKREL_M2P_BYTE(src, dest, count, linkjmp)    \
  {                                                                    \
    .xfer =                                                            \
    {                                                                  \
      .structType   = ldmaCtrlStructTypeXfer,                          \
      .structReq    = 0,                                               \
      .xferCnt      = (count) - 1,                                     \
      .byteSwap     = 0,                                               \
      .blockSize    = ldmaCtrlBlockSizeUnit1,                          \
      .doneIfs      = 1,                                               \
      .reqMode      = ldmaCtrlReqModeBlock,                            \
      .decLoopCnt   = 0,                                               \
      .ignoreSrec   = 0,                                               \
      .srcInc       = ldmaCtrlSrcIncOne,                               \
      .size         = ldmaCtrlSizeByte,                                \
      .dstInc       = ldmaCtrlDstIncNone,                              \
      .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,                          \
      .dstAddrMode  = ldmaCtrlDstAddrModeAbs,                          \
      .srcAddr      = (uint32_t)(src),                                 \
      .dstAddr      = (uint32_t)(dest),                                \
      .linkMode     = ldmaLinkModeRel,                                 \
      .link         = 1,                                               \
      .linkAddr     = (linkjmp) * LDMA_DESCRIPTOR_NON_EXTEND_SIZE_WORD \
    }                                                                  \
  }

#if defined(_LDMA_CH_CTRL_EXTEND_MASK)
/**
 * @brief
 *   DMA descriptor initializer for byte transfers from memory to a peripheral
 *   using the extended descriptor fields.
 * @note
 *   For the extended descriptor, if IlMode uses the absolute addressing, the
 *   rules are used directly as the destination addresses of the corresponding
 *   data. Thus the argument 'dest' will be ignored by the DMA engine.
 * @param[in] src       Source data address.
 * @param[in] dest      Peripheral data register destination address.
 * @param[in] count     Number of bytes to transfer.
 * @param[in] linkjmp   Address of descriptor to link to, expressed as a
 *                      signed number of descriptors from "here".
 *                      1=one descriptor forward in memory,
 *                      0=this descriptor,
 *                      -1=one descriptor back in memory.
 */
#define LDMA_DESCRIPTOR_LINKREL_M2P_BYTE_EXTEND(src, dest, count, linkjmp) \
  {                                                                        \
    .structType   = ldmaCtrlStructTypeXfer,                                \
    .extend       = 1,                                                     \
    .structReq    = 0,                                                     \
    .xferCnt      = (count) - 1,                                           \
    .byteSwap     = 0,                                                     \
    .blockSize    = ldmaCtrlBlockSizeUnit1,                                \
    .doneIfs      = 1,                                                     \
    .reqMode      = ldmaCtrlReqModeBlock,                                  \
    .decLoopCnt   = 0,                                                     \
    .ignoreSrec   = 0,                                                     \
    .srcInc       = ldmaCtrlSrcIncOne,                                     \
    .size         = ldmaCtrlSizeByte,                                      \
    .dstInc       = ldmaCtrlDstIncNone,                                    \
    .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,                                \
    .dstAddrMode  = ldmaCtrlDstAddrModeAbs,                                \
    .srcAddr      = (uint32_t)(src),                                       \
    .dstAddr      = (uint32_t)(dest),                                      \
    .linkMode     = ldmaLinkModeRel,                                       \
    .link         = 1,                                                     \
    .linkAddr     = (linkjmp) * LDMA_DESCRIPTOR_EXTEND_SIZE_WORD,          \
    .dstIlEn      = 0,                                                     \
    .IlMode       = 0,                                                     \
    .bufferable   = 0,                                                     \
    .IlSrc        = 0                                                      \
  }
#endif

/**
 * @brief
 *   DMA descriptor initializer for Immediate WRITE transfer
 * @param[in] value     Immediate value to write.
 * @param[in] address   Write address.
 */
#define LDMA_DESCRIPTOR_SINGLE_WRITE(value, address) \
  {                                                  \
    .wri =                                           \
    {                                                \
      .structType   = ldmaCtrlStructTypeWrite,       \
      .structReq    = 1,                             \
      .xferCnt      = 0,                             \
      .byteSwap     = 0,                             \
      .blockSize    = 0,                             \
      .doneIfs      = 1,                             \
      .reqMode      = 0,                             \
      .decLoopCnt   = 0,                             \
      .ignoreSrec   = 0,                             \
      .srcInc       = 0,                             \
      .size         = 0,                             \
      .dstInc       = 0,                             \
      .srcAddrMode  = 0,                             \
      .dstAddrMode  = 0,                             \
      .immVal       = (value),                       \
      .dstAddr      = (uint32_t)(address),           \
      .linkMode     = 0,                             \
      .link         = 0,                             \
      .linkAddr     = 0                              \
    }                                                \
  }

/**
 * @brief
 *   DMA descriptor initializer for Immediate WRITE transfer
 *
 *   Link address must be an absolute address.
 * @note
 *   The linkAddr member of the transfer descriptor is not initialized.
 *   linkAddr must be initialized by using the proper bits right-shift
 *   to get the correct bits from the absolute address.
 *   _LDMA_CH_LINK_LINKADDR_SHIFT should be used for that operation:
 * @code
     desc.linkAddr = ((int32_t)&next_desc) >> _LDMA_CH_LINK_LINKADDR_SHIFT;@endcode
 *   The opposite bit shift (left) must be done if linkAddr is read.
 * @param[in] value     Immediate value to write.
 * @param[in] address   Write address.
 */
#define LDMA_DESCRIPTOR_LINKABS_WRITE(value, address) \
  {                                                   \
    .wri =                                            \
    {                                                 \
      .structType   = ldmaCtrlStructTypeWrite,        \
      .structReq    = 1,                              \
      .xferCnt      = 0,                              \
      .byteSwap     = 0,                              \
      .blockSize    = 0,                              \
      .doneIfs      = 0,                              \
      .reqMode      = 0,                              \
      .decLoopCnt   = 0,                              \
      .ignoreSrec   = 0,                              \
      .srcInc       = 0,                              \
      .size         = 0,                              \
      .dstInc       = 0,                              \
      .srcAddrMode  = 0,                              \
      .dstAddrMode  = 0,                              \
      .immVal       = (value),                        \
      .dstAddr      = (uint32_t)(address),            \
      .linkMode     = ldmaLinkModeAbs,                \
      .link         = 1,                              \
      .linkAddr     = 0 /* Must be set runtime ! */   \
    }                                                 \
  }

/**
 * @brief
 *   DMA descriptor initializer for Immediate WRITE transfer
 * @param[in] value     Immediate value to write.
 * @param[in] address   Write address.
 * @param[in] linkjmp   Address of descriptor to link to, expressed as a
 *                      signed number of descriptors from "here".
 *                      1=one descriptor forward in memory,
 *                      0=this descriptor,
 *                      -1=one descriptor back in memory.
 */
#define LDMA_DESCRIPTOR_LINKREL_WRITE(value, address, linkjmp)         \
  {                                                                    \
    .wri =                                                             \
    {                                                                  \
      .structType   = ldmaCtrlStructTypeWrite,                         \
      .structReq    = 1,                                               \
      .xferCnt      = 0,                                               \
      .byteSwap     = 0,                                               \
      .blockSize    = 0,                                               \
      .doneIfs      = 0,                                               \
      .reqMode      = 0,                                               \
      .decLoopCnt   = 0,                                               \
      .ignoreSrec   = 0,                                               \
      .srcInc       = 0,                                               \
      .size         = 0,                                               \
      .dstInc       = 0,                                               \
      .srcAddrMode  = 0,                                               \
      .dstAddrMode  = 0,                                               \
      .immVal       = (value),                                         \
      .dstAddr      = (uint32_t)(address),                             \
      .linkMode     = ldmaLinkModeRel,                                 \
      .link         = 1,                                               \
      .linkAddr     = (linkjmp) * LDMA_DESCRIPTOR_NON_EXTEND_SIZE_WORD \
    }                                                                  \
  }

/**
 * @brief
 *   DMA descriptor initializer for SYNC transfer
 * @param[in] set          Sync pattern bits to set.
 * @param[in] clr          Sync pattern bits to clear.
 * @param[in] matchValue   Sync pattern to match.
 * @param[in] matchEnable  Sync pattern bits to enable for match.
 */
#define LDMA_DESCRIPTOR_SINGLE_SYNC(set, clr, matchValue, matchEnable) \
  {                                                                    \
    .sync =                                                            \
    {                                                                  \
      .structType   = ldmaCtrlStructTypeSync,                          \
      .structReq    = 1,                                               \
      .xferCnt      = 0,                                               \
      .byteSwap     = 0,                                               \
      .blockSize    = 0,                                               \
      .doneIfs      = 1,                                               \
      .reqMode      = 0,                                               \
      .decLoopCnt   = 0,                                               \
      .ignoreSrec   = 0,                                               \
      .srcInc       = 0,                                               \
      .size         = 0,                                               \
      .dstInc       = 0,                                               \
      .srcAddrMode  = 0,                                               \
      .dstAddrMode  = 0,                                               \
      .syncSet      = (set),                                           \
      .syncClr      = (clr),                                           \
      .matchVal     = (matchValue),                                    \
      .matchEn      = (matchEnable),                                   \
      .linkMode     = 0,                                               \
      .link         = 0,                                               \
      .linkAddr     = 0                                                \
    }                                                                  \
  }

/**
 * @brief
 *   DMA descriptor initializer for SYNC transfer
 *
 *   Link address must be an absolute address.
 * @note
 *   The linkAddr member of the transfer descriptor is not initialized.
 *   linkAddr must be initialized by using the proper bits right-shift
 *   to get the correct bits from the absolute address.
 *   _LDMA_CH_LINK_LINKADDR_SHIFT should be used for that operation:
 * @code
     desc.linkAddr = ((int32_t)&next_desc) >> _LDMA_CH_LINK_LINKADDR_SHIFT;@endcode
 *   The opposite bit shift (left) must be done if linkAddr is read.
 * @param[in] set          Sync pattern bits to set.
 * @param[in] clr          Sync pattern bits to clear.
 * @param[in] matchValue   Sync pattern to match.
 * @param[in] matchEnable  Sync pattern bits to enable for match.
 */
#define LDMA_DESCRIPTOR_LINKABS_SYNC(set, clr, matchValue, matchEnable) \
  {                                                                     \
    .sync =                                                             \
    {                                                                   \
      .structType   = ldmaCtrlStructTypeSync,                           \
      .structReq    = 1,                                                \
      .xferCnt      = 0,                                                \
      .byteSwap     = 0,                                                \
      .blockSize    = 0,                                                \
      .doneIfs      = 0,                                                \
      .reqMode      = 0,                                                \
      .decLoopCnt   = 0,                                                \
      .ignoreSrec   = 0,                                                \
      .srcInc       = 0,                                                \
      .size         = 0,                                                \
      .dstInc       = 0,                                                \
      .srcAddrMode  = 0,                                                \
      .dstAddrMode  = 0,                                                \
      .syncSet      = (set),                                            \
      .syncClr      = (clr),                                            \
      .matchVal     = (matchValue),                                     \
      .matchEn      = (matchEnable),                                    \
      .linkMode     = ldmaLinkModeAbs,                                  \
      .link         = 1,                                                \
      .linkAddr     = 0 /* Must be set runtime ! */                     \
    }                                                                   \
  }

/**
 * @brief
 *   DMA descriptor initializer for SYNC transfer
 * @param[in] set          Sync pattern bits to set.
 * @param[in] clr          Sync pattern bits to clear.
 * @param[in] matchValue   Sync pattern to match.
 * @param[in] matchEnable  Sync pattern bits to enable for match.
 * @param[in] linkjmp   Address of descriptor to link to, expressed as a
 *                      signed number of descriptors from "here".
 *                      1=one descriptor forward in memory,
 *                      0=this descriptor,
 *                      -1=one descriptor back in memory.
 */
#define LDMA_DESCRIPTOR_LINKREL_SYNC(set, clr, matchValue, matchEnable, linkjmp) \
  {                                                                              \
    .sync =                                                                      \
    {                                                                            \
      .structType   = ldmaCtrlStructTypeSync,                                    \
      .structReq    = 1,                                                         \
      .xferCnt      = 0,                                                         \
      .byteSwap     = 0,                                                         \
      .blockSize    = 0,                                                         \
      .doneIfs      = 0,                                                         \
      .reqMode      = 0,                                                         \
      .decLoopCnt   = 0,                                                         \
      .ignoreSrec   = 0,                                                         \
      .srcInc       = 0,                                                         \
      .size         = 0,                                                         \
      .dstInc       = 0,                                                         \
      .srcAddrMode  = 0,                                                         \
      .dstAddrMode  = 0,                                                         \
      .syncSet      = (set),                                                     \
      .syncClr      = (clr),                                                     \
      .matchVal     = (matchValue),                                              \
      .matchEn      = (matchEnable),                                             \
      .linkMode     = ldmaLinkModeRel,                                           \
      .link         = 1,                                                         \
      .linkAddr     = (linkjmp) * LDMA_DESCRIPTOR_NON_EXTEND_SIZE_WORD           \
    }                                                                            \
  }

#if defined(_LDMA_CH_CTRL_EXTEND_MASK)
/**
 * @brief
 *   Initializer for the destination interleaving portion of the DMA extended descriptor.
 * @param[in] desc      Transfer-type descriptor.
 * @param[in] ilmode    Rules table addressing mode for interleaved data.
 * @param[in] ilsrc     Base address for rules table in memory.
 */
#define LDMA_DESCRIPTOR_EXTEND_DST_IL_CFG(desc, ilmode, ilsrc) \
  {                                                            \
    (desc).dstIlEn = true;                                     \
    (desc).IlMode  = (ilmode);                                 \
    (desc).IlSrc   = (uint32_t)(ilsrc);                        \
  }
#endif

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

void LDMA_DeInit(void);
void LDMA_EnableChannelRequest(int ch, bool enable);
void LDMA_Init(const LDMA_Init_t *init);
void LDMA_StartTransfer(int ch,
                        const LDMA_TransferCfg_t *transfer,
                        const LDMA_Descriptor_t  *descriptor);
#if defined(_LDMA_CH_CTRL_EXTEND_MASK)
void LDMA_StartTransferExtend(int ch,
                              const LDMA_TransferCfg_t *transfer,
                              const LDMA_DescriptorExtend_t *descriptor_ext);
#endif
void LDMA_StopTransfer(int ch);
bool LDMA_TransferDone(int ch);
uint32_t LDMA_TransferRemainingCount(int ch);

#if defined(_LDMA_SWRST_MASK)
/***************************************************************************//**
 * @brief
 *   Reset the LDMA.
 ******************************************************************************/
__STATIC_INLINE void LDMA_Reset(void)
{
  LDMA->SWRST_SET = LDMA_SWRST_SWRST;

  /* Wait for reset to complete. */
  while ((LDMA->SWRST & _LDMA_SWRST_RESETTING_MASK)) {
  }
}
#endif

/***************************************************************************//**
 * @brief
 *   Check if a certain channel is enabled.
 *
 * @param[in] ch
 *   LDMA channel to check.
 *
 * @return
 *   return true if the LDMA channel is enabled and false if the channel is not
 *   enabled.
 ******************************************************************************/
__STATIC_INLINE bool LDMA_ChannelEnabled(int ch)
{
  if ((ch < 0) || (ch > 31)) {
    return false;
  }
#if defined(_LDMA_CHSTATUS_MASK)
  return LDMA->CHSTATUS & (1 << ch);
#else
  // We've already confirmed ch is between 0 and 31,
  // so it's now safe to cast it to uint8_t
  return LDMA->CHEN & (1 << (uint8_t)ch);
#endif
}

/***************************************************************************//**
 * @brief
 *   Clear one or more pending LDMA interrupts.
 *
 * @param[in] flags
 *   Pending LDMA interrupt sources to clear. Use one or more valid
 *   interrupt flags for the LDMA module. The flags are LDMA_IFC_ERROR
 *   and one done flag for each channel.
 ******************************************************************************/
__STATIC_INLINE void LDMA_IntClear(uint32_t flags)
{
#if defined (LDMA_HAS_SET_CLEAR)
  LDMA->IF_CLR = flags;
#else
  LDMA->IFC = flags;
#endif
}

/***************************************************************************//**
 * @brief
 *   Disable one or more LDMA interrupts.
 *
 * @param[in] flags
 *   LDMA interrupt sources to disable. Use one or more valid
 *   interrupt flags for LDMA module. The flags are LDMA_IEN_ERROR
 *   and one done flag for each channel.
 ******************************************************************************/
__STATIC_INLINE void LDMA_IntDisable(uint32_t flags)
{
  LDMA->IEN &= ~flags;
}

/***************************************************************************//**
 * @brief
 *   Enable one or more LDMA interrupts.
 *
 * @note
 *   Depending on the use, a pending interrupt may already be set prior to
 *   enabling the interrupt. To ignore a pending interrupt, consider using
 *   LDMA_IntClear() prior to enabling the interrupt.
 *
 * @param[in] flags
 *   LDMA interrupt sources to enable. Use one or more valid
 *   interrupt flags for LDMA module. The flags are LDMA_IEN_ERROR
 *   and one done flag for each channel.
 ******************************************************************************/
__STATIC_INLINE void LDMA_IntEnable(uint32_t flags)
{
  LDMA->IEN |= flags;
}

/***************************************************************************//**
 * @brief
 *   Get pending LDMA interrupt flags.
 *
 * @note
 *   Event bits are not cleared by the use of this function.
 *
 * @return
 *   LDMA interrupt sources pending. Returns one or more valid
 *   interrupt flags for LDMA module. The flags are LDMA_IF_ERROR and
 *   one flag for each LDMA channel.
 ******************************************************************************/
__STATIC_INLINE uint32_t LDMA_IntGet(void)
{
  return LDMA->IF;
}

/***************************************************************************//**
 * @brief
 *   Get enabled and pending LDMA interrupt flags.
 *   Useful for handling more interrupt sources in the same interrupt handler.
 *
 * @note
 *   Interrupt flags are not cleared by the use of this function.
 *
 * @return
 *   Pending and enabled LDMA interrupt sources
 *   Return value is the bitwise AND of
 *   - the enabled interrupt sources in LDMA_IEN and
 *   - the pending interrupt flags LDMA_IF
 ******************************************************************************/
__STATIC_INLINE uint32_t LDMA_IntGetEnabled(void)
{
  uint32_t ien;

  ien = LDMA->IEN;
  return LDMA->IF & ien;
}

/***************************************************************************//**
 * @brief
 *   Set one or more pending LDMA interrupts
 *
 * @param[in] flags
 *   LDMA interrupt sources to set to pending. Use one or more valid
 *   interrupt flags for LDMA module. The flags are LDMA_IFS_ERROR and
 *   one done flag for each LDMA channel.
 ******************************************************************************/
__STATIC_INLINE void LDMA_IntSet(uint32_t flags)
{
#if defined (LDMA_HAS_SET_CLEAR)
  LDMA->IF_SET = flags;
#else
  LDMA->IFS = flags;
#endif
}

/** @} (end addtogroup ldma) */

#ifdef __cplusplus
}
#endif

#endif /* defined( LDMA_PRESENT ) && ( LDMA_COUNT == 1 ) */
#endif /* EM_LDMA_H */
